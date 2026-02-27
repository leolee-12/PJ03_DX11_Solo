#include "stdafx.h"
#include "..\Public\SpaceRock.h"

#include "RenderInstance.h"
#include "GameInstance.h"

CSpaceRock::CSpaceRock(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject { pDevice, pContext }
{

}

CSpaceRock::CSpaceRock(const CSpaceRock & Prototype)
	: CGameObject{ Prototype }
{

}

HRESULT CSpaceRock::Initialize_Prototype()
{
	
	return S_OK;
}

HRESULT CSpaceRock::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;
	CTransform::TRANSFORM_DESC tDesc{};
	tDesc.fRotationPerSec = XMConvertToRadians(5.f);
	m_pTransformCom->SetUp_TransformDesc(&tDesc);
	//m_pTransformCom->Set_Scaled(0.01f, 0.01f, 0.01f);
	//m_pTransformCom->Rotation(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(45.f));
	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(-30.f, 10.f, -200.f, 1.f));

	CMap_Manager::Get_Instance()->Push_MapObject(CMap_Manager::MAP_SPACE,
		static_cast<CMap_Manager::Map_Object_Key*>(pArg)->m_PrototypeKey, this);
	return S_OK;
}

void CSpaceRock::Camera_Update(_float fTimeDelta)
{

}

void CSpaceRock::Update(_float fTimeDelta)
{

}

void CSpaceRock::Late_Update(_float fTimeDelta)
{
	m_pRenderInstance->Add_RenderObject(CRenderer::RG_SPACEMAP, this);
}

HRESULT CSpaceRock::Render(_float fTimeDelta)
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	for (size_t j = 0; j < 3; j++)
	{
		_uint		iNumMeshes = m_pModelCom[j]->Get_NumMeshes();

		for (size_t i = 0; i < iNumMeshes; i++)
		{
			if (FAILED(
				(m_isBreakRock[j] == false ? m_pModelCom[j]->Bind_MaterialSRV(m_pShaderCom, aiTextureType_DIFFUSE, "g_DiffuseTexture", i) 
				: m_pBRModelCom[j]->Bind_MaterialSRV(m_pShaderCom, aiTextureType_DIFFUSE, "g_DiffuseTexture", i))
			))
				return E_FAIL;

			if (FAILED(m_pShaderCom->Begin(4)))
				return E_FAIL;

			if (FAILED((m_isBreakRock[j] == false ? m_pModelCom[j]->Render(i) : m_pBRModelCom[j]->Render(i))))
				return E_FAIL;
		}
	}

	return S_OK;
}

HRESULT CSpaceRock::Ready_Components()
{
	/* Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxSpace"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;


	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_SpaceRock_1"),
		TEXT("Com_Model_0"), reinterpret_cast<CComponent**>(&m_pModelCom[0]))))
		return E_FAIL;
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_SpaceRock_2"),
		TEXT("Com_Model_1"), reinterpret_cast<CComponent**>(&m_pModelCom[1]))))
		return E_FAIL;
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_SpaceRock_3"),
		TEXT("Com_Model_2"), reinterpret_cast<CComponent**>(&m_pModelCom[2]))))
		return E_FAIL;
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_SpaceBRRock_1"),
		TEXT("Com_BRModel_0"), reinterpret_cast<CComponent**>(&m_pBRModelCom[0]))))
		return E_FAIL;
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_SpaceBRRock_2"),
		TEXT("Com_BRModel_1"), reinterpret_cast<CComponent**>(&m_pBRModelCom[1]))))
		return E_FAIL;
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_SpaceBRRock_3"),
		TEXT("Com_BRModel_2"), reinterpret_cast<CComponent**>(&m_pBRModelCom[2]))))
		return E_FAIL;

	// Prototype_Component_Model_SpaceBRRock_1
	return S_OK;
}

HRESULT CSpaceRock::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	
	return S_OK;
}

CSpaceRock * CSpaceRock::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CSpaceRock*		pInstance = new CSpaceRock(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CSpaceRock"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CSpaceRock::Clone(void * pArg)
{
	CSpaceRock*		pInstance = new CSpaceRock(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CSpaceRock"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSpaceRock::Free()
{

	Safe_Release(m_pShaderCom);
	for (size_t i = 0; i < 3; i++)
	{
		Safe_Release(m_pModelCom[i]);
		Safe_Release(m_pBRModelCom[i]);
	}

	__super::Free();
}
