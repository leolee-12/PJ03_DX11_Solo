#include "ComputeShaderComp.h"

#include "GameInstance.h"

IMPLEMENT_CREATE(CComputeShaderComp)
IMPLEMENT_CREATE_EX2(CComputeShaderComp, const wstring&, strShaderFilePath, const _char*, pEntryPoint)
IMPLEMENT_CLONE(CComputeShaderComp, CComponent)

CComputeShaderComp::CComputeShaderComp(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: CComponent(pDevice, pContext)
{
}

CComputeShaderComp::CComputeShaderComp(const CComputeShaderComp& rhs)
	: CComponent(rhs)
	, m_pShader(rhs.m_pShader)
	, m_pShaderManager(rhs.m_pShaderManager)
{
	Safe_AddRef(m_pShaderManager);
}

HRESULT CComputeShaderComp::Initialize_Prototype()
{
	if (FAILED(Link_ToShaderManager()))
		return E_FAIL;

	return S_OK;
}

HRESULT CComputeShaderComp::Initialize_Prototype(const wstring& strShaderFilePath, const _char* pEntryPoint)
{
	if (FAILED(Link_Shader(strShaderFilePath, pEntryPoint, nullptr)))
		return E_FAIL;

	return S_OK;
}

HRESULT CComputeShaderComp::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CComputeShaderComp::Link_Shader(const wstring& strEffectFileName, const _char* pEntryPoint, const D3D_SHADER_MACRO* pShaderMacro)
{
	// 셰이더 매니저에 의존성이 있음, 항상 전역적으로 다룸
	if (FAILED(Link_ToShaderManager()))
		return E_FAIL;

	// 있던거 끊기
	Unlink_Shader();

	// 있던거면 파일 이름으로 검색해서 찾는다.
	m_pShader = m_pShaderManager->Find_ShaderBuffer<EShaderType::Compute>(strEffectFileName);
	if (nullptr == m_pShader)
	{
		// 없으면 로드 시키고, 그래도 실패하면 파일 못 찾는거임.
		if (FAILED(m_pShaderManager->Load_Shader(strEffectFileName, EShaderType::Compute, pEntryPoint)))
			return E_FAIL;

		// 로드성공시 다시 찾아온다.
		m_pShader = m_pShaderManager->Find_ShaderBuffer<EShaderType::Compute>(strEffectFileName);
	}

	return S_OK;
}

HRESULT CComputeShaderComp::Unlink_Shader()
{
	m_pShader.Reset();

	return S_OK;
}

HRESULT CComputeShaderComp::IsShader_Ready()
{
	if (nullptr == m_pShader)
		return E_FAIL;

	return S_OK;
}

HRESULT CComputeShaderComp::Link_ToShaderManager()
{
	if (m_pShaderManager)
		return S_OK;

	if (nullptr == m_pGameInstance
		|| nullptr == (m_pShaderManager = m_pGameInstance->Get_ShaderManager()))
		return E_FAIL;

	Safe_AddRef(m_pShaderManager);

	return S_OK;
}

void CComputeShaderComp::Free()
{
	__super::Free();

	Safe_Release(m_pShaderManager);
}
