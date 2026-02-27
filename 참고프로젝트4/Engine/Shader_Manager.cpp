#include "Shader_Manager.h"
#include "Utility_File.h"

CShader_Manager::CShader_Manager(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: m_pDevice(pDevice), m_pDeviceContext(pContext)
{
}

HRESULT CShader_Manager::Initialize(const wstring& strMainPath)
{
	m_strMainPath = strMainPath;

	return S_OK;
}

CShader_Manager* CShader_Manager::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const wstring& strMainPath)
{
	ThisClass* pInstance = new ThisClass(pDevice, pContext);

	if (FAILED(pInstance->Initialize(strMainPath)))
	{
		MSG_BOX("RenderMgr Create Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CShader_Manager::Free()
{
	// 셰이더 코드 해제
	for (size_t i = 0; i < ECast(EShaderType::Size); i++)
	{
		for (auto& Pair : m_mapShaderData[i])
			Pair.second->Free();
		m_mapShaderData[i].clear();
	}
	
	m_mapEffects.clear();
}

HRESULT CShader_Manager::Load_Shader(const wstring& strFileName, const EShaderType eType, const _char* pEntryPoint)
{
	_uint iIndex = ECast(eType);
	if (eType == EShaderType::Size)
		RETURN_EFAIL;

	FShaderData* pData = nullptr;
	auto iter = m_mapShaderData[iIndex].find(strFileName);
	if (iter != m_mapShaderData[iIndex].end())
	{
		if ((*iter).second->IsLoaded())
			RETURN_EFAIL;

		// 코드를 로드하지 않고 키에 들어간 셰이더가 있다면 지금 로드하도록 한다.
		pData = (*iter).second;
		if (nullptr == pData)
			RETURN_EFAIL;
	}
	else
	{
		pData = FShaderData::Create();
		if (nullptr == pData)
			RETURN_EFAIL;

		m_mapShaderData[iIndex].emplace(strFileName, pData);
	}

	// 여기서 셰이더 컴파일 혹은 컴파일된 셰이더 코드를 가지고 Blob에 저장한다.
	ComPtr<ID3DBlob> pBlob = { nullptr };
	if (strFileName.find(TEXT(".cso")) != wstring::npos)
	{
		pBlob = Read_ShaderBinary(m_strMainPath + strFileName);
	}
	else if (strFileName.find(TEXT(".hlsl")) != wstring::npos)
	{
		pBlob = Complie_Shader(m_strMainPath + strFileName, pEntryPoint);
	}
	else
		return E_FAIL;

	if (nullptr == pBlob)
		RETURN_EFAIL;




	ComPtr<ID3D11DeviceChild> pShaderBuffer = { nullptr };
	switch (eType)
	{
	case Engine::EShaderType::Vertex:
	{
		ID3D11VertexShader* pShader = Cast<ID3D11VertexShader*>(pShaderBuffer.Get());
		ComPtr<ID3D11VertexShader> pComShader = pShader;
		if (FAILED(m_pDevice->CreateVertexShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pComShader)))
			RETURN_EFAIL;
		pShaderBuffer = Cast<ID3D11DeviceChild*>(pComShader.Get());
		break;
	}
	case Engine::EShaderType::Pixel:
	{
		ID3D11PixelShader* pShader = Cast<ID3D11PixelShader*>(pShaderBuffer.Get());
		ComPtr<ID3D11PixelShader> pComShader = pShader;
		if (FAILED(m_pDevice->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pComShader)))
			RETURN_EFAIL;
		pShaderBuffer = Cast<ID3D11DeviceChild*>(pShader);
		break;
	}
	case Engine::EShaderType::Geometry:
	{
		ID3D11GeometryShader* pShader = Cast<ID3D11GeometryShader*>(pShaderBuffer.Get());
		if (FAILED(m_pDevice->CreateGeometryShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pShader)))
			RETURN_EFAIL;
		pShaderBuffer = Cast<ID3D11DeviceChild*>(pShader);
		break;
	}
	case Engine::EShaderType::Hull:
	{
		ID3D11HullShader* pShader = Cast<ID3D11HullShader*>(pShaderBuffer.Get());
		if (FAILED(m_pDevice->CreateHullShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pShader)))
			RETURN_EFAIL;
		pShaderBuffer = Cast<ID3D11DeviceChild*>(pShader);
		break;
	}
	case Engine::EShaderType::Domain:
	{
		ID3D11DomainShader* pShader = Cast<ID3D11DomainShader*>(pShaderBuffer.Get());
		if (FAILED(m_pDevice->CreateDomainShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pShader)))
			RETURN_EFAIL;
		pShaderBuffer = Cast<ID3D11DeviceChild*>(pShader);
		break;
	}
	case Engine::EShaderType::Compute:
	{
		ID3D11ComputeShader* pShader = Cast<ID3D11ComputeShader*>(pShaderBuffer.Get());
		if (FAILED(m_pDevice->CreateComputeShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pShader)))
			RETURN_EFAIL;
		pShaderBuffer = Cast<ID3D11DeviceChild*>(pShader);
		break;
	}
	default:
		break;
	}

	pData->Set_Shader(strFileName, pBlob.Get(), pShaderBuffer.Get());

	return S_OK;
}


ID3DBlob* CShader_Manager::Read_ShaderBinary(const wstring& strFile)
{
	ifstream fin(strFile, ios::binary);
	if (fin.fail())
		return nullptr;

	fin.seekg(0, ios_base::end);
	ifstream::pos_type iSize = Cast<_int>(fin.tellg());
	fin.seekg(0, ios_base::beg);

	ID3DBlob* pBlob = nullptr;
	if (FAILED(D3DCreateBlob(iSize, &pBlob)))
		return nullptr;

	fin.read((_char*)pBlob->GetBufferPointer(), iSize);
	fin.close();

	return pBlob;
}

ID3DBlob* CShader_Manager::Complie_Shader(const wstring& strFile, const _char* pEntryPoint)
{
	ID3DBlob* pBlob = nullptr;
	if (FAILED(D3DCompileFromFile(strFile.c_str(), nullptr, nullptr, pEntryPoint, "cs_5_0", 0, 0, &pBlob, nullptr)))
		return nullptr;

	return pBlob;
}


const ComPtr<ID3DBlob> CShader_Manager::Find_ShaderByte(const EShaderType eType, const wstring& strName) const
{
	_uint iIndex = ECast(eType);

	auto iter = m_mapShaderData[iIndex].find(strName);
	if (iter == m_mapShaderData[iIndex].end() || !(*iter).second->IsLoaded())
		return nullptr;

	return (*iter).second->CRef_ShaderByte();
}



HRESULT CShader_Manager::Load_Effect(const wstring& strFileName, const D3D11_INPUT_ELEMENT_DESC* pElements, _uint iNumElements, const D3D_SHADER_MACRO* pShaderMacro)
{
	wstring strMacroDefine = Make_MacroToWstring(pShaderMacro);

	wstring strSaveKey = strFileName + strMacroDefine;
	auto iter = m_mapEffects.find(strSaveKey);
	if (iter != m_mapEffects.end())
		RETURN_EFAIL;

	_uint		iHlslFlag = 0;
	HRESULT		hr = 0;

#ifdef _DEBUG
	iHlslFlag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	iHlslFlag = D3DCOMPILE_OPTIMIZATION_LEVEL1;
#endif

	ComPtr<ID3DX11Effect> pEffect;
	wstring strTest = CUtility_File::PathFinder(m_strMainPath, strFileName);
	if (FAILED(hr = D3DX11CompileEffectFromFile(strTest.c_str(), pShaderMacro,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, iHlslFlag, 0, m_pDevice.Get(), pEffect.GetAddressOf(), nullptr)))
		RETURN_EFAIL;

	ID3DX11EffectTechnique* pTechnique = pEffect->GetTechniqueByIndex(0);
	if (nullptr == pTechnique)
		RETURN_EFAIL;

	D3DX11_TECHNIQUE_DESC tTechniqueDesc;
	pTechnique->GetDesc(&tTechniqueDesc);


	shared_ptr<FEffectData> pEffectData = FEffectData::Create();
	pEffectData->tTechniqueDesc = tTechniqueDesc;
	pEffectData->pEffect = pEffect;
	pEffectData->pElements = pElements;
	pEffectData->iNumElements = iNumElements;
	pEffectData->pShaderMacro = pShaderMacro;

	for (_uint i = 0; i < tTechniqueDesc.Passes; i++)
	{
		ID3DX11EffectPass* pPass = pTechnique->GetPassByIndex(i);

		D3DX11_PASS_DESC	tPassDesc;
		pPass->GetDesc(&tPassDesc);

		ComPtr<ID3D11InputLayout> pInputLayout = { nullptr };
		if (FAILED(m_pDevice->CreateInputLayout(pElements, iNumElements, tPassDesc.pIAInputSignature, tPassDesc.IAInputSignatureSize, pInputLayout.GetAddressOf())))
			RETURN_EFAIL;


		pEffectData->pInputLayouts.push_back(pInputLayout);
	}
	m_mapEffects.emplace(strSaveKey, pEffectData);

	return S_OK;
}

HRESULT CShader_Manager::Recompile_EffectsAll()
{
	/*const _char* batFilePath = "D:\VisualStudio Projects\TeamProject139\Framework\Copy.bat";

	_int iResult = system(batFilePath);

	if (iResult != 0)
	{
		cerr << "Engine 셰이더 파일 복사 실패" << endl;
		RETURN_EFAIL;
	}*/

	for (auto iter = m_mapEffects.begin(); iter != m_mapEffects.end(); ++iter)
	{
		if ((*iter).second)
		{
			Recompile_Effect((*iter).first, (*iter).second);
		}
	}

	return S_OK;
}

HRESULT CShader_Manager::Recompile_Effect(const wstring& strFileName, shared_ptr<FEffectData> pEffectData)
{
	_uint		iHlslFlag = 0;
	HRESULT		hr = 0;

#ifdef _DEBUG
	iHlslFlag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	iHlslFlag = D3DCOMPILE_OPTIMIZATION_LEVEL1;
#endif

	// 셰이더 재컴파일
	ComPtr<ID3DX11Effect> pEffect;
	wstring strTest = CUtility_File::PathFinder(m_strMainPath, strFileName);
	if (FAILED(hr = D3DX11CompileEffectFromFile(strTest.c_str(), pEffectData->pShaderMacro,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, iHlslFlag, 0, m_pDevice.Get(), pEffect.GetAddressOf(), nullptr)))
	{
		cout<<"[ "<< ConvertToString(strFileName) << ": " << "재컴파일 오류! ]" << endl;
		return E_FAIL;
	}

	ID3DX11EffectTechnique* pTechnique = pEffect->GetTechniqueByIndex(0);
	if (nullptr == pTechnique)
	{
		cout << "[ "<< ConvertToString(strFileName) << ": " << "재컴파일 오류! ]" << endl;
		return E_FAIL;
	}

	D3DX11_TECHNIQUE_DESC tTechniqueDesc;
	pTechnique->GetDesc(&tTechniqueDesc);

	// 재컴파일 정보 삽입
	pEffectData->tTechniqueDesc = tTechniqueDesc;
	pEffectData->pEffect = pEffect;

	// 레이아웃 초기화 후 재 삽입
	pEffectData->pInputLayouts.clear();
	for (_uint i = 0; i < tTechniqueDesc.Passes; i++)
	{
		ID3DX11EffectPass* pPass = pTechnique->GetPassByIndex(i);

		D3DX11_PASS_DESC	tPassDesc;
		pPass->GetDesc(&tPassDesc);

		ComPtr<ID3D11InputLayout> pInputLayout = { nullptr };
		if (FAILED(m_pDevice->CreateInputLayout(pEffectData->pElements, pEffectData->iNumElements,
			tPassDesc.pIAInputSignature, tPassDesc.IAInputSignatureSize, pInputLayout.GetAddressOf())))
			RETURN_EFAIL;

		pEffectData->pInputLayouts.push_back(pInputLayout);
	}

	cout << ConvertToString(strFileName) << ": " << "재컴파일 완료" << endl;

	return S_OK;
}

ID3DX11Effect* CShader_Manager::Find_Effect(const wstring& strKey) const
{
	auto iter = m_mapEffects.find(strKey);
	if (iter == m_mapEffects.end())
		return nullptr;

	return (*iter).second->pEffect.Get();
}

wstring CShader_Manager::Make_MacroToWstring(const D3D_SHADER_MACRO* pShaderMacro) const
{
	wstring strMacroDefine = TEXT("");
	if (pShaderMacro != nullptr)
	{
		_uint iErrorIter = 0U;
		_int i = 0;
		_char	szMacroId[MAX_PATH] = {};
		_int MacroIdIndex = 0;
		while (true)
		{
			// 이름이 nullptr 이면 종료한다.
			if ((pShaderMacro[i].Name) == nullptr)
				break;

			szMacroId[MacroIdIndex++] = '_';
			// 오류 방지
			if (++iErrorIter >= MAX_PATH)
				return wstring();

			for (size_t j = 0; j < strlen(pShaderMacro[i].Name); j++)
			{
				szMacroId[MacroIdIndex++] = pShaderMacro[i].Name[j];
				// 오류 방지
				if (++iErrorIter >= MAX_PATH)
					return wstring();
			}

			szMacroId[MacroIdIndex++] = '_';
			// 오류 방지
			if (++iErrorIter >= MAX_PATH)
				return wstring();

			for (size_t j = 0; j < strlen(pShaderMacro[i].Definition); j++)
			{
				szMacroId[MacroIdIndex++] = pShaderMacro[i].Definition[j];
				// 오류 방지
				if (++iErrorIter >= MAX_PATH)
					return wstring();
			}

			++i;
		}

		strMacroDefine = ConvertToWstring(szMacroId);
	}

	return strMacroDefine;
}

shared_ptr<FEffectData> CShader_Manager::Find_EffectData(const wstring& strKey, const D3D_SHADER_MACRO* pShaderMacro) const
{
	wstring strMacroDefine = Make_MacroToWstring(pShaderMacro);

	auto iter = m_mapEffects.find(strKey + strMacroDefine);
	if (iter == m_mapEffects.end())
		return nullptr;

	return (*iter).second;
}