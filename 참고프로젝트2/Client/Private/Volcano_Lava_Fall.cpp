#include "stdafx.h"
#include "..\Public\Volcano_Lava_Fall.h"
#include "Volcano_Defines.h"
#include "RenderInstance.h"
#include "GameInstance.h"

CVolcano_Lava_Fall::CVolcano_Lava_Fall(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject { pDevice, pContext }
{

}

CVolcano_Lava_Fall::CVolcano_Lava_Fall(const CVolcano_Lava_Fall & Prototype)
	: CGameObject{ Prototype }
{

}

HRESULT CVolcano_Lava_Fall::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CVolcano_Lava_Fall::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	//m_pTransformCom->Set_Scaled(0.01f, 0.01f, 0.01f);
	//m_pTransformCom->Rotation(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(180.f));
	CMap_Manager::Get_Instance()->Push_MapObject(CMap_Manager::MAP_VOLCANO,
		static_cast<CMap_Manager::Map_Object_Key*>(pArg)->m_PrototypeKey, this);
	return S_OK;
}

void CVolcano_Lava_Fall::Camera_Update(_float fTimeDelta)
{
	//m_fAccTime += fTimeDelta * 5;
}

void CVolcano_Lava_Fall::Update(_float fTimeDelta)
{

}

void CVolcano_Lava_Fall::Late_Update(_float fTimeDelta)
{
	m_pRenderInstance->Add_RenderObject(CRenderer::RG_MAP, this);
}

HRESULT CVolcano_Lava_Fall::Render(_float fTimeDelta)
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		//if (FAILED(m_pModelCom->Bind_MaterialSRV(m_pShaderCom, aiTextureType_DIFFUSE, "g_DiffuseTexture", 0)))
		//	return E_FAIL;
		//_int iLavaFallIndex = i;
		//if (FAILED(m_pShaderCom->Bind_RawValue("g_LavaFallIndex", &iLavaFallIndex, sizeof(_int))))
		//	return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(VO_LAVAFALL + i)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CVolcano_Lava_Fall::Ready_Components()
{
	/* Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxVolcano"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	///* Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_vo_river01"),
		TEXT("Com_Texture_Diffuse"), reinterpret_cast<CComponent**>(&m_pTextureCom[0]))))
		return E_FAIL;

	///* Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_vo_river02"),
		TEXT("Com_Texture_Mask1"), reinterpret_cast<CComponent**>(&m_pTextureCom[1]))))
		return E_FAIL;

	///* Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_vo_river03"),
		TEXT("Com_Texture_Mask2"), reinterpret_cast<CComponent**>(&m_pTextureCom[2]))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_vo_lava_fall"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CVolcano_Lava_Fall::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pTextureCom[0]->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", 0)))
		return E_FAIL;
	//if (FAILED(m_pTextureCom[1]->Bind_ShaderResource(m_pShaderCom, "g_MaskTexture", 0)))
	//	return E_FAIL;
	//if (FAILED(m_pTextureCom[2]->Bind_ShaderResource(m_pShaderCom, "g_TMaskTexture", 0)))
	//	return E_FAIL;
	//if (FAILED(m_pShaderCom->Bind_RawValue("g_fSpriteSize", &m_fSpriteSize, sizeof(_float2))))
	//	return E_FAIL;

	//if (FAILED(m_pShaderCom->Bind_RawValue("g_fSpriteCurPos", &m_fSpriteCurPos, sizeof(_float2))))
	//	return E_FAIL;

	//if (FAILED(m_pShaderCom->Bind_RawValue("g_Time", &m_fAccTime, sizeof(float))))
	//	return E_FAIL;
	
	return S_OK;
}

CVolcano_Lava_Fall * CVolcano_Lava_Fall::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CVolcano_Lava_Fall*		pInstance = new CVolcano_Lava_Fall(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CVolcano_Lava_Fall"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CVolcano_Lava_Fall::Clone(void * pArg)
{
	CVolcano_Lava_Fall*		pInstance = new CVolcano_Lava_Fall(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CVolcano_Lava_Fall"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVolcano_Lava_Fall::Free()
{
	for (size_t i = 0; i < 3; i++)
		Safe_Release(m_pTextureCom[i]);

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);

	__super::Free();
}
