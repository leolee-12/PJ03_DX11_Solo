#include "stdafx.h"
#include "..\Public\SpaceMeteo.h"

#include "RenderInstance.h"
#include "GameInstance.h"

CSpaceMeteo::CSpaceMeteo(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject { pDevice, pContext }
{

}

CSpaceMeteo::CSpaceMeteo(const CSpaceMeteo & Prototype)
	: CGameObject{ Prototype }
{

}

HRESULT CSpaceMeteo::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSpaceMeteo::Initialize(void * pArg)
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

void CSpaceMeteo::Camera_Update(_float fTimeDelta)
{

}

void CSpaceMeteo::Update(_float fTimeDelta)
{

}

void CSpaceMeteo::Late_Update(_float fTimeDelta)
{
	m_pRenderInstance->Add_RenderObject(CRenderer::RG_SPACEMAP, this);
}

HRESULT CSpaceMeteo::Render(_float fTimeDelta)
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	for (size_t j = 0; j < 3; j++)
	{
		_uint		iNumMeshes = m_pModelCom[j]->Get_NumMeshes();

		for (size_t i = 0; i < iNumMeshes; i++)
		{
			if (FAILED(m_pModelCom[j]->Bind_MaterialSRV(m_pShaderCom, aiTextureType_DIFFUSE, "g_DiffuseTexture", i)))
				return E_FAIL;

			if (FAILED(m_pShaderCom->Begin(4)))
				return E_FAIL;

			if (FAILED(m_pModelCom[j]->Render(i)))
				return E_FAIL;
		}
	}

	return S_OK;
}

HRESULT CSpaceMeteo::Ready_Components()
{
	/* Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxSpace"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;


	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_SpaceMeteo_1"),
		TEXT("Com_Model_0"), reinterpret_cast<CComponent**>(&m_pModelCom[0]))))
		return E_FAIL;
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_SpaceMeteo_2"),
		TEXT("Com_Model_1"), reinterpret_cast<CComponent**>(&m_pModelCom[1]))))
		return E_FAIL;
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_SpaceMeteo_3"),
		TEXT("Com_Model_2"), reinterpret_cast<CComponent**>(&m_pModelCom[2]))))
		return E_FAIL;

	return S_OK;
}

HRESULT CSpaceMeteo::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	
	return S_OK;
}

CSpaceMeteo * CSpaceMeteo::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CSpaceMeteo*		pInstance = new CSpaceMeteo(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CSpaceMeteo"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CSpaceMeteo::Clone(void * pArg)
{
	CSpaceMeteo*		pInstance = new CSpaceMeteo(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CSpaceMeteo"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSpaceMeteo::Free()
{

	Safe_Release(m_pShaderCom);
	for (size_t i = 0; i < 3; i++)
	{
		Safe_Release(m_pModelCom[i]);
	}

	__super::Free();
}
