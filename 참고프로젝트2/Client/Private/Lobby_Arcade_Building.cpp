#include "stdafx.h"
#include "..\Public\Lobby_Arcade_Building.h"

#include "RenderInstance.h"
#include "GameInstance.h"

CLobby_Arcade_Building::CLobby_Arcade_Building(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject { pDevice, pContext }
{

}

CLobby_Arcade_Building::CLobby_Arcade_Building(const CLobby_Arcade_Building & Prototype)
	: CGameObject{ Prototype }
{

}

HRESULT CLobby_Arcade_Building::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CLobby_Arcade_Building::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Set_State_Position(_float3(59.5f, 0.f, 0.f));

	return S_OK;
}

void CLobby_Arcade_Building::Camera_Update(_float fTimeDelta)
{
	
}

void CLobby_Arcade_Building::Update(_float fTimeDelta)
{
}

void CLobby_Arcade_Building::Late_Update(_float fTimeDelta)
{
	m_pRenderInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CLobby_Arcade_Building::Render(_float fTimeDelta)
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

		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CLobby_Arcade_Building::Ready_Components()
{
	/* Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_LOBBY, TEXT("Prototype_Component_Model_Arcade_Mode_Building"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLobby_Arcade_Building::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	
	return S_OK;
}

CLobby_Arcade_Building * CLobby_Arcade_Building::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CLobby_Arcade_Building*		pInstance = new CLobby_Arcade_Building(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CLobby_Arcade_Building"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CLobby_Arcade_Building::Clone(void * pArg)
{
	CLobby_Arcade_Building*		pInstance = new CLobby_Arcade_Building(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CLobby_Arcade_Building"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLobby_Arcade_Building::Free()
{
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);

	__super::Free();
}
