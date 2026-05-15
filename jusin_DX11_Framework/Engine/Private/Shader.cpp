#include "Shader.h"

namespace
{
	/* 캐시 정책 상수 (작업 디렉터리 기준 상대경로) */
	constexpr const _tchar* CACHE_ROOT = L"../../Cached/";
	constexpr const _tchar* SHADER_INCLUDE_DEFINES = L"../../ShaderFiles/Engine_Shader_Defines.hlsli";

	/* .hlsl 입력 경로 -> .fxo 캐시 경로
	   예) L"../../ShaderFiles/Shader_VtxMesh.hlsl"
			-> Release: L"../../ShaderFiles/Cached/Shader_VtxMesh.fxo"
			-> Debug  : L"../../ShaderFiles/Cached/Shader_VtxMesh_d.fxo" */
	_wstring Make_CachedFxoPath(const _tchar* pHlslPath)
	{
		_wstring src{ pHlslPath };

		size_t iSepPos = src.find_last_of(L"/\\");
		_wstring baseName = (_wstring::npos == iSepPos) ? src : src.substr(iSepPos + 1);

		size_t iDotPos = baseName.find_last_of(L'.');
		if (_wstring::npos != iDotPos)
			baseName.resize(iDotPos);

		_wstring result;
		result.reserve(64);
		result.append(CACHE_ROOT);
		result.append(baseName);
#ifdef _DEBUG
		result.append(L"_d");
#endif
		result.append(L".fxo");

		return result;
	}

	/* 파일 최종 쓰기 시각 획득. 부재/실패 시 false */
	_bool Get_FileWriteTimeW(const _tchar* pPath, FILETIME& outTime)
	{
		WIN32_FILE_ATTRIBUTE_DATA fad{};
		if (FALSE == GetFileAttributesExW(pPath, GetFileExInfoStandard, &fad))
			return false;

		outTime = fad.ftLastWriteTime;
		return true;
	}

	/* 입력이 파일 경로일 때, 그 파일의 상위 디렉터리만 만든다.
	   상위 한 단계만 만들면 충분한 환경 (예: jusin_DX11_Framework/Cached/) 을 가정. */
	void Ensure_DirectoryW(const wchar_t* pFilePath)
	{
		_wstring path{ pFilePath };
		size_t iSepPos = path.find_last_of(L"/\\");
		if (_wstring::npos == iSepPos)
			return;

		_wstring dir = path.substr(0, iSepPos);
		CreateDirectoryW(dir.c_str(), nullptr); /* 이미 있으면 ERROR_ALREADY_EXISTS — 무시 */
	}

	/* blob → 파일. 성공 true. */
	_bool Save_BlobToFile(const _tchar* pPath, const void* pData, size_t Size)
	{
		Ensure_DirectoryW(pPath);

		std::ofstream ofs(pPath, std::ios::binary | std::ios::trunc);
		if (!ofs.is_open())
			return false;

		ofs.write(static_cast<const char*>(pData), static_cast<std::streamsize>(Size));
		return ofs.good();
	}

	/* 파일 → 메모리 버퍼. 성공 true. */
	bool Load_FileToBlob(const _tchar* pPath, vector<unsigned char>& outBytes)
	{
		std::ifstream ifs(pPath, std::ios::binary | std::ios::ate);
		if (!ifs.is_open())
			return false;

		std::streamsize size = ifs.tellg();
		if (size <= 0)
			return false;

		outBytes.resize(static_cast<size_t>(size));
		ifs.seekg(0, std::ios::beg);
		if (!ifs.read(reinterpret_cast<char*>(outBytes.data()), size))
			return false;

		return true;
	}
}

CShader::CShader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent{ pDevice, pContext }
{
}

CShader::CShader(const CShader& Prototype)
	: CComponent{ Prototype }
	, m_pEffect{ Prototype.m_pEffect }
	, m_iNumPasses{ Prototype.m_iNumPasses }
	, m_InputLayouts{ Prototype.m_InputLayouts }
{
	for (auto& pInputLayout : m_InputLayouts)
		Safe_AddRef(pInputLayout);

	Safe_AddRef(m_pEffect);
}

HRESULT CShader::Initialize_Prototype(const _tchar* pShaderFilePath, const D3D11_INPUT_ELEMENT_DESC* pElements, _uint iNumElements)
{
	_uint       iHlslFlag = {};

#ifdef _DEBUG
	iHlslFlag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	iHlslFlag = D3DCOMPILE_OPTIMIZATION_LEVEL1;
#endif

	/* 1) 캐시 유효성 판정
		 - .fxo 가 존재하고
		 - .fxo mtime >= .hlsl mtime
		 - .fxo mtime >= Engine_Shader_Defines.hlsli mtime (hlsli 가 없으면 그 조건은 패스)
		 위 3가지를 모두 만족할 때만 캐시 사용. */
	const _wstring fxoPath = Make_CachedFxoPath(pShaderFilePath);

	_bool bCacheValid = false;
	{
		FILETIME ftFxo{}, ftHlsl{}, ftHlsli{};
		const _bool bHaveFxo = Get_FileWriteTimeW(fxoPath.c_str(), ftFxo);
		const _bool bHaveHlsl = Get_FileWriteTimeW(pShaderFilePath, ftHlsl);
		const _bool bHaveHlsli = Get_FileWriteTimeW(SHADER_INCLUDE_DEFINES, ftHlsli);

		if (bHaveFxo && bHaveHlsl)
		{
			const _bool bFxoFreshVsHlsl = CompareFileTime(&ftFxo, &ftHlsl) >= 0;
			const _bool bFxoFreshVsHlsli = !bHaveHlsli || CompareFileTime(&ftFxo, &ftHlsli) >= 0;
			bCacheValid = bFxoFreshVsHlsl && bFxoFreshVsHlsli;
		}
	}

	/* 2) HLSL 컴파일 람다 — 캐시 무효 또는 캐시 손상 시 호출.
			 성공 시 *ppOutBlob 에 결과 blob, .fxo 파일도 갱신. */
	auto fnCompileAndSave = [this, pShaderFilePath, iHlslFlag, &fxoPath](ID3DBlob** ppOutBlob) ->
		HRESULT
		{
			ID3DBlob* pErrorBlob = nullptr;

			const HRESULT hr = D3DCompileFromFile(
				pShaderFilePath,
				nullptr,                            /* pDefines */
				D3D_COMPILE_STANDARD_FILE_INCLUDE,  /* pInclude — 기존 동작 유지 */
				nullptr,                            /* pEntrypoint — effect 는 entry 없음 */
				"fx_5_0",                           /* pTarget — effect 5.0 */
				iHlslFlag,
				0,                                  /* Flags2 */
				ppOutBlob,
				&pErrorBlob);

#ifdef _DEBUG
			if (FAILED(hr) && nullptr != pErrorBlob)
			{
				OutputDebugStringA("[CShader] HLSL Compile Error:\n");
				OutputDebugStringA(static_cast<const char*>(pErrorBlob->GetBufferPointer()));
				OutputDebugStringA("\n");
			}
#endif
			Safe_Release(pErrorBlob);

			if (FAILED(hr))
				return hr;

			/* 저장 실패는 무시 — 메모리상 blob 으로 Effect 생성은 계속 가능 */
			Save_BlobToFile(fxoPath.c_str(),
				(*ppOutBlob)->GetBufferPointer(),
				(*ppOutBlob)->GetBufferSize());
			return S_OK;
		};

	/* 3) Effect 생성 — 캐시 로드 우선, 실패 시 컴파일로 폴백 */
	bool bEffectCreated = false;

	if (bCacheValid)
	{
		vector<unsigned char> cachedBytes;
		if (Load_FileToBlob(fxoPath.c_str(), cachedBytes))
		{
			if (SUCCEEDED(D3DX11CreateEffectFromMemory(
				cachedBytes.data(),
				cachedBytes.size(),
				0,                  /* FXFlags */
				m_pDevice,
				&m_pEffect)))
			{
				bEffectCreated = true;
			}
		}
		/* (g) fallback: 로드 또는 Effect 생성 실패 시 아래 컴파일 분기로 흘러감 */
	}

	if (false == bEffectCreated)
	{
		ID3DBlob* pCompiledBlob = nullptr;

		if (FAILED(fnCompileAndSave(&pCompiledBlob)))
		{
			Safe_Release(pCompiledBlob);
			return E_FAIL;
		}

		const HRESULT hr = D3DX11CreateEffectFromMemory(
			pCompiledBlob->GetBufferPointer(),
			pCompiledBlob->GetBufferSize(),
			0,
			m_pDevice,
			&m_pEffect);

		Safe_Release(pCompiledBlob);

		if (FAILED(hr))
			return E_FAIL;
	}

	/* 4) InputLayout 생성 — 기존 로직 그대로 */
	ID3DX11EffectTechnique* pTechnique = m_pEffect->GetTechniqueByIndex(0);

	if (nullptr == pTechnique)
		return E_FAIL;
	
	D3DX11_TECHNIQUE_DESC TechniqueDesc{};
	pTechnique->GetDesc(&TechniqueDesc);
	m_iNumPasses = TechniqueDesc.Passes;

	for (_uint i = 0; i < m_iNumPasses; i++)
	{
		ID3DX11EffectPass* pPass = pTechnique->GetPassByIndex(i);
		D3DX11_PASS_DESC PassDesc{};
		pPass->GetDesc(&PassDesc);

		ID3D11InputLayout* pInputLayout = { nullptr };
		if (FAILED(m_pDevice->CreateInputLayout(pElements, iNumElements, PassDesc.pIAInputSignature,
			PassDesc.IAInputSignatureSize, &pInputLayout)))
			return E_FAIL;

		m_InputLayouts.push_back(pInputLayout);
	}

	return S_OK;
}

HRESULT CShader::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CShader::Begin(_uint iPassIndex)
{
	if (iPassIndex >= m_iNumPasses)
		return E_FAIL;
	
	ID3DX11EffectPass* pPass =
		m_pEffect->GetTechniqueByIndex(0)->GetPassByIndex(iPassIndex);

	if (nullptr == pPass)
		return E_FAIL;

	m_pContext->GSSetShader(nullptr, nullptr, 0);

	m_pContext->IASetInputLayout(m_InputLayouts[iPassIndex]);

	m_pEffect->GetTechniqueByIndex(0)->GetPassByIndex(iPassIndex)->Apply(0, m_pContext);


	return S_OK;
}

HRESULT CShader::Bind_Matrix(const _char* pConstName, const _float4x4* pMatrix)
{
	ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(pConstName);
	if (!pVariable->IsValid())
		return E_FAIL;

	ID3DX11EffectMatrixVariable* pMatrixVariable = pVariable->AsMatrix();
	if (!pMatrixVariable->IsValid())
		return E_FAIL;

	return pMatrixVariable->SetMatrix(reinterpret_cast<const _float*>(pMatrix));
}

HRESULT CShader::Bind_Matrices(const _char* pConstName, const _float4x4* pMatrices, _uint iNumMatrices)
{
	ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(pConstName);
	if (nullptr == pVariable)
		return E_FAIL;

	ID3DX11EffectMatrixVariable* pMatrixVariable = pVariable->AsMatrix();
	if (nullptr == pMatrixVariable)
		return E_FAIL;

	return pMatrixVariable->SetMatrixArray(reinterpret_cast<const _float*>(pMatrices), 0, iNumMatrices);
}

HRESULT CShader::Bind_SRV(const _char* pConstName, ID3D11ShaderResourceView* pSRV)
{
	ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(pConstName);
	if (!pVariable->IsValid())
		return E_FAIL;

	ID3DX11EffectShaderResourceVariable* pSRVariable = pVariable->AsShaderResource();
	if (!pSRVariable->IsValid())
		return E_FAIL;

	return pSRVariable->SetResource(pSRV);
}

HRESULT CShader::Bind_SRVs(const _char* pConstName, ID3D11ShaderResourceView** ppSRVArray, _uint iNumSRVs)
{
	ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(pConstName);
	if (!pVariable->IsValid())
		return E_FAIL;

	ID3DX11EffectShaderResourceVariable* pSRVariable = pVariable->AsShaderResource();
	if (!pSRVariable->IsValid())
		return E_FAIL;

	return pSRVariable->SetResourceArray(ppSRVArray, 0, iNumSRVs);
}

HRESULT CShader::Bind_RawValue(const _char* pConstName, const void* pValue, _uint iLength)
{
	ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(pConstName);
	if (nullptr == pVariable)
		return E_FAIL;

	return pVariable->SetRawValue(pValue, 0, iLength);
}

CShader* CShader::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pShaderFilePath, const D3D11_INPUT_ELEMENT_DESC* pElements, _uint iNumElements)
{
	CShader* pInstance = new CShader(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pShaderFilePath, pElements, iNumElements)))
	{
		MSG_BOX("Failed to Created : CShader");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CShader::Clone(void* pArg)
{
	CShader* pInstance = new CShader(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CShader");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CShader::Free()
{
	__super::Free();

	for (auto& pInputLayout : m_InputLayouts)
		Safe_Release(pInputLayout);

	m_InputLayouts.clear();

	Safe_Release(m_pEffect);
}
