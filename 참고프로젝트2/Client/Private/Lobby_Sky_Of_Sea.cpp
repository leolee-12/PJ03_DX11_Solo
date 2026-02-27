#include "stdafx.h"
#include "..\Public\Lobby_Sky_Of_Sea.h"

#include "RenderInstance.h"
#include "GameInstance.h"

CLobby_Sky_Of_Sea::CLobby_Sky_Of_Sea(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{

}

CLobby_Sky_Of_Sea::CLobby_Sky_Of_Sea(const CLobby_Sky_Of_Sea& Prototype)
	: CGameObject{ Prototype }
{

}

HRESULT CLobby_Sky_Of_Sea::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CLobby_Sky_Of_Sea::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Set_State_Position(_float3(0.f, 300.f, 0.f));

	return S_OK;
}

void CLobby_Sky_Of_Sea::Camera_Update(_float fTimeDelta)
{

}

void CLobby_Sky_Of_Sea::Update(_float fTimeDelta)
{
	m_fTime += fTimeDelta;
}

void CLobby_Sky_Of_Sea::Late_Update(_float fTimeDelta)
{
	m_pRenderInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CLobby_Sky_Of_Sea::Render(_float fTimeDelta)
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		/* 모델이 가지고 있는 머테리얼 중 i번째 메시가 사용해야하는 머테리얼구조체의 aiTextureType_DIFFUSE번째 텍스쳐를 */
		/* m_pShaderCom에 있는 g_DiffuseTexture변수에 던져. */
		if (FAILED(m_pModelCom->Bind_MaterialSRV(m_pShaderCom, aiTextureType_DIFFUSE, "g_DiffuseTexture", i)))
			return E_FAIL;
		// m_pModelCom->Bind_MaterialSRV(m_pShaderCom, aiTextureType_NORMALS, "g_NormalTexture", i);

		if (FAILED(m_pShaderCom->Begin(1)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CLobby_Sky_Of_Sea::Ready_Components()
{
	/* Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_LOBBY, TEXT("Prototype_Component_Model_Lobby_Sky_Sea_Of_Cloud"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLobby_Sky_Of_Sea::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fTime", &m_fTime, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

CLobby_Sky_Of_Sea* CLobby_Sky_Of_Sea::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLobby_Sky_Of_Sea* pInstance = new CLobby_Sky_Of_Sea(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CLobby_Sky_Of_Sea"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CLobby_Sky_Of_Sea::Clone(void* pArg)
{
	CLobby_Sky_Of_Sea* pInstance = new CLobby_Sky_Of_Sea(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CLobby_Sky_Of_Sea"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLobby_Sky_Of_Sea::Free()
{
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);

	__super::Free();
}
