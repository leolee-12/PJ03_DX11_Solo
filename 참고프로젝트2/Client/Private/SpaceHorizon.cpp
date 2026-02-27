#include "stdafx.h"
#include "..\Public\SpaceHorizon.h"

#include "RenderInstance.h"
#include "GameInstance.h"

CSpaceHorizon::CSpaceHorizon(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject { pDevice, pContext }
{

}

CSpaceHorizon::CSpaceHorizon(const CSpaceHorizon & Prototype)
	: CGameObject{ Prototype }
{

}

HRESULT CSpaceHorizon::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSpaceHorizon::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	//m_pTransformCom->Set_Scaled(0.01f, 0.01f, 0.01f);
	//m_pTransformCom->Rotation(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(180.f));

	CMap_Manager::Get_Instance()->Push_MapObject(CMap_Manager::MAP_SPACE,
		static_cast<CMap_Manager::Map_Object_Key*>(pArg)->m_PrototypeKey, this);

	return S_OK;
}

void CSpaceHorizon::Camera_Update(_float fTimeDelta)
{
	//m_fAccTime += fTimeDelta * 5;
}

void CSpaceHorizon::Update(_float fTimeDelta)
{

}

void CSpaceHorizon::Late_Update(_float fTimeDelta)
{
	m_pRenderInstance->Add_RenderObject(CRenderer::RG_BLEND_PRI, this);
}

HRESULT CSpaceHorizon::Render(_float fTimeDelta)
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_MaterialSRV(m_pShaderCom, aiTextureType_DIFFUSE, "g_DiffuseTexture", i)))
			return E_FAIL;
		
		if (FAILED(m_pShaderCom->Begin(6)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CSpaceHorizon::Ready_Components()
{
	/* Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxSpace"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Space_Horizon"),
		TEXT("Com_Texture_Diffuse"), reinterpret_cast<CComponent**>(&m_pTextureCom_Diffuse))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_SpaceHorizon"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CSpaceHorizon::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	//if (FAILED(m_pTextureCom_Diffuse->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", 0)))
	//	return E_FAIL;

	//if (FAILED(m_pShaderCom->Bind_RawValue("g_fSpriteSize", &m_fSpriteSize, sizeof(_float2))))
	//	return E_FAIL;

	//if (FAILED(m_pShaderCom->Bind_RawValue("g_fSpriteCurPos", &m_fSpriteCurPos, sizeof(_float2))))
	//	return E_FAIL;

	//if (FAILED(m_pShaderCom->Bind_RawValue("g_Time", &m_fAccTime, sizeof(float))))
	//	return E_FAIL;
	
	return S_OK;
}

CSpaceHorizon * CSpaceHorizon::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CSpaceHorizon*		pInstance = new CSpaceHorizon(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CSpaceHorizon"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CSpaceHorizon::Clone(void * pArg)
{
	CSpaceHorizon*		pInstance = new CSpaceHorizon(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CSpaceHorizon"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSpaceHorizon::Free()
{

	Safe_Release(m_pTextureCom_Diffuse);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);

	__super::Free();
}
