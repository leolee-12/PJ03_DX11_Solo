#include "Shader.h"
#include "Texture.h"

#include "GameInstance.h"
#include "Shader_Manager.h"

vector<_uint_64>				CShader::g_UsingRenderIDs;
vector<weak_ptr<CShader>>		CShader::g_ShaderComps;

IMPLEMENT_CREATE(CShader)
IMPLEMENT_CREATE_EX3(CShader, const wstring&, strShaderFilePath, const D3D11_INPUT_ELEMENT_DESC*, pElements, _uint, iNumElements)
IMPLEMENT_CLONE(CShader, CComponent)

CShader::CShader(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: CComponent(pDevice, pContext)
{
}

CShader::CShader(const CShader& rhs)
	: CComponent(rhs)
	, m_pEffectData(rhs.m_pEffectData)
	, m_pShaderManager(rhs.m_pShaderManager)
	, m_iRenderFlag(rhs.m_iRenderFlag)
{
	Safe_AddRef(m_pShaderManager);
}

HRESULT CShader::Initialize_Prototype()
{
	if (FAILED(Link_ToShaderManager()))
		RETURN_EFAIL;

	return S_OK;
}

/* 셰이더파일을 빌드하여 ID3DX11Effect를 만들어냈다. */
/* ID3DX11Effect로부터 Pass에 까지 접근하여 인자도 던진 정점정보를 이 셰이더 패스에서 잘 받아줄 수있는지에 대한 InputLayout을 만들었다. */
HRESULT CShader::Initialize_Prototype(const wstring& strShaderFilePath, const D3D11_INPUT_ELEMENT_DESC* pElements, _uint iNumElements)
{
	Link_Effect(strShaderFilePath, pElements, iNumElements);
//;	if (FAILED(Link_ToShaderManager()))
//		RETURN_EFAIL;
//
//	_uint		iHlslFlag = 0;
//
//#ifdef _DEBUG
//	iHlslFlag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
//#else
//	iHlslFlag = D3DCOMPILE_OPTIMIZATION_LEVEL1;
//#endif
//
//	/*strShaderFilePath경로에 작성되어있는 hlsl언어 번역 빌드하여 ID3DX11Effect라는 녀석을 만들자.  */
//	if (FAILED(D3DX11CompileEffectFromFile(strShaderFilePath.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, iHlslFlag, 0, m_pDevice.Get(), &m_pEffect, nullptr)))
//		RETURN_EFAIL;
//
//	ID3DX11EffectTechnique* pTechnique = m_pEffect->GetTechniqueByIndex(0);
//	if (nullptr == pTechnique)
//		RETURN_EFAIL;
//
//	pTechnique->GetDesc(&m_TechniqueDesc);
//
//	for (uint32_t i = 0; i < m_TechniqueDesc.Passes; i++)
//	{
//		ID3DX11EffectPass* pPass = pTechnique->GetPassByIndex(i);
//
//		D3DX11_PASS_DESC		PassDesc;
//		pPass->GetDesc(&PassDesc);
//
//		/* InputLayout : 내가 그리기위해 사용하는 정점의 입력정보.  */
//		/* dx11에서는 고정기능 렌더링파이프라인에 대한 기능이 삭제되었다. */
//		/* 우리가 직접 렌더링 파이프라인을 수행해야한다.(사용자정의 렌더링파이프라인) -> Shader */
//		/* 그래서 우리에겐 반드시 셰이더가 필요하다. */
//		/* 우리가 이 정점들을 그리기위해서는 셰이더가 필요하고, 이 셰이더는 반드시 내가 그릴려고하는 정점을 받아줄 수 있어야한다. */
//		/* 내가 그리려고하는 정점이 사용하려고하는 셰이더에 입력이 가능한지?에 대한 체크를 사전에 미리 처리하고.
//		가능하다면 dx11이 InputLayout이란 객체를 만들어준다. */
//		ID3D11InputLayout* pInputLayout = nullptr;
//
//		if (FAILED(m_pDevice->CreateInputLayout(pElements, iNumElements, PassDesc.pIAInputSignature, PassDesc.IAInputSignatureSize, &pInputLayout)))
//			RETURN_EFAIL;
//
//		m_InputLayouts.push_back(pInputLayout);
//	}

	return S_OK;
}

HRESULT CShader::Initialize(void* pArg)
{
	// 클론할 때만 ID를 부여한다.
	//Allocate_RenderID();

	return S_OK;
}

HRESULT CShader::Link_Effect(const wstring& strEffectFileName, const D3D11_INPUT_ELEMENT_DESC* pElements, _uint iNumElements, const D3D_SHADER_MACRO* pShaderMacro)
{
	// 셰이더 매니저에 의존성이 있음, 항상 전역적으로 다룸
	if (FAILED(Link_ToShaderManager()))
		RETURN_EFAIL;

	// 있던거 끊기
	Unlink_Effect();

	// 있던거면 파일 이름으로 검색해서 찾는다.
	shared_ptr<FEffectData> pEffectData = m_pShaderManager->Find_EffectData(strEffectFileName, pShaderMacro);

	if (!pEffectData)
	{
		// 없으면 로드 시키고, 그래도 실패하면 파일 못 찾는거임.
		if (FAILED(m_pShaderManager->Load_Effect(strEffectFileName, pElements, iNumElements, pShaderMacro)))
			RETURN_EFAIL;

		// 로드성공시 다시 찾아온다.
		pEffectData = m_pShaderManager->Find_EffectData(strEffectFileName, pShaderMacro);
	}
	m_pEffectData = pEffectData;

	return S_OK;
}

HRESULT CShader::Unlink_Effect()
{
	m_pEffectData.reset();

	return S_OK;
}

HRESULT CShader::IsRender_Ready()
{
	if (nullptr == m_pEffectData->pEffect)
		RETURN_EFAIL;

	return S_OK;
}

HRESULT CShader::Link_ToShaderManager()
{
	if (m_pShaderManager)
		return S_OK;

	if (nullptr == m_pGameInstance
		|| nullptr == (m_pShaderManager = m_pGameInstance->Get_ShaderManager()))
		RETURN_EFAIL;

	Safe_AddRef(m_pShaderManager);

	return S_OK;
}

shared_ptr<CShader> CShader::Find_ShaderCompByID(_uint iID)
{
	return g_ShaderComps[iID].lock();
}

_uint CShader::Find_Unused_RenderID(_bool bAllocate)
{
	// 앞에서부터 순차적으로 비어있는 곳인지 찾는다. O(n / 64)
	_bool bFound = false;
	_uint iFoundIndex = { 0 };
	for (_uint i = 0; i < Cast<_uint>(g_UsingRenderIDs.size()); i++)
	{
		if (g_UsingRenderIDs[i] != UINT64_MAX)
		{
			bFound = true;
			iFoundIndex = i;
			break;
		}
	}
	// 비어있는 곳이 없으면 새로 할당함.
	if (!bFound)
	{
		g_UsingRenderIDs.push_back(0ULL);
		iFoundIndex = Cast<_uint>(g_UsingRenderIDs.size() - 1);
	}

	// 인덱스 * 64 + 비어있는 비트 = 새로 할당할 ID O(n / 64 + m)
	_uint iBitNum = { 0 };
	for (_uint i = 0; i < 64; i++)
	{
		// 각 비트를 검사한다.
		if (!(g_UsingRenderIDs[iFoundIndex] & (1ULL << i)))
		{
			iBitNum = i;
			if (bAllocate)
				g_UsingRenderIDs[iFoundIndex] ^= (1ULL << i);
			break;
		}
	}

#ifdef _DEBUG
#if SHADER_COMP_DEBUG
	cout << "RenderID : " << iFoundIndex * 64 + iBitNum << endl;
#endif  
#endif // _DEBUG

	// 완성된 ID값 반환
	return iFoundIndex * 64 + iBitNum;
}

void CShader::Allocate_RenderID()
{
	m_iRenderID = Find_Unused_RenderID(true);
	// 등록
	g_ShaderComps.resize(m_iRenderID + 1);
	g_ShaderComps[m_iRenderID] = DynPtrCast<CShader>(shared_from_this());
}

void CShader::Remove_RenderID()
{
	if (m_iRenderID == UINT_MAX)
		return;

	_uint iIndex = m_iRenderID / 64;
	_uint_64 iSubIndex = m_iRenderID - (iIndex * 64);

	g_UsingRenderIDs[iIndex] ^= (1ULL << iSubIndex);

	// 포인터도 삭제
	g_ShaderComps[m_iRenderID].reset();
}

HRESULT CShader::Begin(_uint iPassIndex, _bool bBindRenderID, const _char* pConstantName, const _char* pFlagConstantName)
{
	// 패스 돌리기전에 렌더 ID 바인딩을 한다면 바인드를 시도한다.
	if (bBindRenderID)
		Bind_RenderID(pConstantName, pFlagConstantName);

	// 여기부터 패스 바인드
	if (iPassIndex >= m_pEffectData->tTechniqueDesc.Passes)
		RETURN_EFAIL;

	ID3DX11EffectTechnique* pTechnique = m_pEffectData->pEffect->GetTechniqueByIndex(0);
	if (nullptr == pTechnique)
		RETURN_EFAIL;

	ID3DX11EffectPass* pPass = pTechnique->GetPassByIndex(iPassIndex);

	pPass->Apply(0, m_pContext.Get());

	m_pContext->IASetInputLayout(m_pEffectData->pInputLayouts[iPassIndex].Get());

	return S_OK;
}

HRESULT CShader::Bind_RenderID(const _char* pConstantName, const _char* pFlagConstantName)
{
	// ID 바인드
	ID3DX11EffectVariable* pVariable = m_pEffectData->pEffect->GetVariableByName(pConstantName);
	if (nullptr == pVariable)
		RETURN_EFAIL;

	if (FAILED(pVariable->SetRawValue(&m_iRenderID, 0, sizeof(_uint))))
		RETURN_EFAIL;

	// 플래그 바인드
	pVariable = m_pEffectData->pEffect->GetVariableByName(pFlagConstantName);
	if (nullptr == pVariable)
		RETURN_EFAIL;

	if (FAILED(pVariable->SetRawValue(&m_iRenderFlag, 0, sizeof(_uint))))
		RETURN_EFAIL;

	return S_OK;
}

HRESULT CShader::Bind_Matrix(const _char* pConstantName, const _float4x4* pMatrix)
{
	/* 이 셰이더에 선언되어있는 전역변수의 핸들을 얻어온다.*/
	//전역변수 이름으로 가져오고
	ID3DX11EffectVariable* pVariable = m_pEffectData->pEffect->GetVariableByName(pConstantName);
	if (nullptr == pVariable)
		RETURN_EFAIL;

	//전역변수 자료형(Matrix)에 맞게 변형
	ID3DX11EffectMatrixVariable* pMatrixVariable = pVariable->AsMatrix();
	if (nullptr == pMatrixVariable)
		RETURN_EFAIL;

	//변형한 전역변수에 값 세팅
	return pMatrixVariable->SetMatrix((_float*)pMatrix);
}

HRESULT CShader::Bind_Matrices(const _char* pConstantName, const _float4x4* pMatrix, _uint iNumMatrices)
{	
	/* 이 셰이더에 선언되어있는 전역변수의 핸들을 얻어온다.*/
	//전역변수 이름으로 가져오고
	ID3DX11EffectVariable* pVariable = m_pEffectData->pEffect->GetVariableByName(pConstantName);
	if (nullptr == pVariable)
		RETURN_EFAIL;

	//전역변수 자료형(Matrix)에 맞게 변형
	ID3DX11EffectMatrixVariable* pMatrixVariable = pVariable->AsMatrix();
	if (nullptr == pMatrixVariable)
		RETURN_EFAIL;

	//변형한 전역변수에 값 세팅
	return pMatrixVariable->SetMatrixArray((_float*)pMatrix,0,iNumMatrices);
}

HRESULT CShader::Bind_SRV(const _char* pConstantName, ID3D11ShaderResourceView* pSRV)
{	
	/* 이 셰이더에 선언되어있는 전역변수의 핸들을 얻어온다.*/
	//전역변수 이름으로 가져오고
	ID3DX11EffectVariable* pVariable = m_pEffectData->pEffect->GetVariableByName(pConstantName);
	if (nullptr == pVariable)
		RETURN_EFAIL;

	//전역변수 자료형(SRV)에 맞게 변형
	ID3DX11EffectShaderResourceVariable* pSRVariable = pVariable->AsShaderResource();
	if (nullptr == pSRVariable)
		RETURN_EFAIL;

	//변형한 전역변수에 값 세팅
	return pSRVariable->SetResource(pSRV);
}

HRESULT CShader::Bind_SRVs(const _char* pConstantName, ID3D11ShaderResourceView** ppSRV, _uint iNumTextures)
{

	/* 이 셰이더에 선언되어있는 전역변수의 핸들을 얻어온다.*/
	//전역변수 이름으로 가져오고
	ID3DX11EffectVariable* pVariable = m_pEffectData->pEffect->GetVariableByName(pConstantName);
	if (nullptr == pVariable)
		RETURN_EFAIL;

	//전역변수 자료형(SRV)에 맞게 변형
	ID3DX11EffectShaderResourceVariable* pSRVariable = pVariable->AsShaderResource();
	if (nullptr == pSRVariable)
		RETURN_EFAIL;

	//변형한 전역변수에 값 세팅
	return pSRVariable->SetResourceArray(ppSRV,0,iNumTextures);
}


HRESULT CShader::Bind_RawValue(const _char* pConstantName, const void* pData, _uint iSize)
{
	ID3DX11EffectVariable* pVariable = m_pEffectData->pEffect->GetVariableByName(pConstantName);
	if (nullptr == pVariable)
		RETURN_EFAIL;

	return pVariable->SetRawValue(pData, 0, iSize);
}



void CShader::Free()
{
	__super::Free();

	Safe_Release(m_pShaderManager);
	//Remove_RenderID();
}
