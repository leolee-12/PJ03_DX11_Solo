#include "stdafx.h"
#include "..\Public\SpaceStone.h"

#include "RenderInstance.h"
#include "GameInstance.h"

CSpaceStone::CSpaceStone(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject { pDevice, pContext }
{

}

CSpaceStone::CSpaceStone(const CSpaceStone & Prototype)
	: CGameObject{ Prototype }
{

}

HRESULT CSpaceStone::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSpaceStone::Initialize(void * pArg)
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

void CSpaceStone::Camera_Update(_float fTimeDelta)
{

}

void CSpaceStone::Update(_float fTimeDelta)
{

}

void CSpaceStone::Late_Update(_float fTimeDelta)
{
	m_pRenderInstance->Add_RenderObject(CRenderer::RG_SPACEMAP, this);
}

HRESULT CSpaceStone::Render(_float fTimeDelta)
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_MaterialSRV(m_pShaderCom, aiTextureType_DIFFUSE, "g_DiffuseTexture", i)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(4)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CSpaceStone::Ready_Components()
{
	/* Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxSpace"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;


	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_SpaceStone"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CSpaceStone::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	
	return S_OK;
}

CSpaceStone * CSpaceStone::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CSpaceStone*		pInstance = new CSpaceStone(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CSpaceStone"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CSpaceStone::Clone(void * pArg)
{
	CSpaceStone*		pInstance = new CSpaceStone(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CSpaceStone"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSpaceStone::Free()
{

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);

	__super::Free();
}
