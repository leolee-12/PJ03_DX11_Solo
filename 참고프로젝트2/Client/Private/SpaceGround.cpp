#include "stdafx.h"
#include "..\Public\SpaceGround.h"

#include "RenderInstance.h"
#include "GameInstance.h"

CSpaceGround::CSpaceGround(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject { pDevice, pContext }
{

}

CSpaceGround::CSpaceGround(const CSpaceGround & Prototype)
	: CGameObject{ Prototype }
{

}

HRESULT CSpaceGround::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSpaceGround::Initialize(void * pArg)
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

void CSpaceGround::Camera_Update(_float fTimeDelta)
{
	//m_fAccTime += fTimeDelta * 5;
}

void CSpaceGround::Update(_float fTimeDelta)
{

}

void CSpaceGround::Late_Update(_float fTimeDelta)
{
	m_pRenderInstance->Add_RenderObject(CRenderer::RG_SPACEMAP, this);
}

HRESULT CSpaceGround::Render(_float fTimeDelta)
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		_int iCount = i;
		if (FAILED(m_pShaderCom->Bind_RawValue("g_GroundCount", &iCount, sizeof(int))))
			return E_FAIL;
		
		if (FAILED(m_pShaderCom->Begin(5)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CSpaceGround::Ready_Components()
{
	/* Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxSpace"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Space_Crater"),
		TEXT("Com_Texture_Crater"), reinterpret_cast<CComponent**>(&m_pTextureCom_Crater))))
		return E_FAIL;
	/* Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Space_Ground_Shadow"),
		TEXT("Com_Texture_Shadow"), reinterpret_cast<CComponent**>(&m_pTextureCom_Ground_Shadow))))
		return E_FAIL;
	/* Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Space_Ground_Pattern"),
		TEXT("Com_Texture_Pattern"), reinterpret_cast<CComponent**>(&m_pTextureCom_Ground_Pattern))))
		return E_FAIL;
	/* Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Space_Ground_Cliff"),
		TEXT("Com_Texture_Cliff"), reinterpret_cast<CComponent**>(&m_pTextureCom_Ground_Cliff))))
		return E_FAIL;
	/* Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Space_Ground"),
		TEXT("Com_Texture_Diffuse"), reinterpret_cast<CComponent**>(&m_pTextureCom_Diffuse))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_SpaceGround"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CSpaceGround::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pTextureCom_Diffuse->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", 0)))
		return E_FAIL;

	if (FAILED(m_pTextureCom_Ground_Pattern->Bind_ShaderResource(m_pShaderCom, "g_GroundPattern", 0)))
		return E_FAIL;

	if (FAILED(m_pTextureCom_Ground_Shadow->Bind_ShaderResource(m_pShaderCom, "g_GroundShadow", 0)))
		return E_FAIL;

	if (FAILED(m_pTextureCom_Ground_Cliff->Bind_ShaderResource(m_pShaderCom, "g_GroundCliff", 0)))
		return E_FAIL;

	if (FAILED(m_pTextureCom_Crater->Bind_ShaderResource(m_pShaderCom, "g_GroundCrater", 0)))
		return E_FAIL;

	//if (FAILED(m_pShaderCom->Bind_RawValue("g_fSpriteSize", &m_fSpriteSize, sizeof(_float2))))
	//	return E_FAIL;

	//if (FAILED(m_pShaderCom->Bind_RawValue("g_fSpriteCurPos", &m_fSpriteCurPos, sizeof(_float2))))
	//	return E_FAIL;

	//if (FAILED(m_pShaderCom->Bind_RawValue("g_Time", &m_fAccTime, sizeof(float))))
	//	return E_FAIL;
	
	return S_OK;
}

CSpaceGround * CSpaceGround::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CSpaceGround*		pInstance = new CSpaceGround(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CSpaceGround"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CSpaceGround::Clone(void * pArg)
{
	CSpaceGround*		pInstance = new CSpaceGround(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CSpaceGround"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSpaceGround::Free()
{
	Safe_Release(m_pTextureCom_Ground_Pattern);
	Safe_Release(m_pTextureCom_Ground_Shadow);
	Safe_Release(m_pTextureCom_Ground_Cliff);
	Safe_Release(m_pTextureCom_Crater);

	Safe_Release(m_pTextureCom_Diffuse);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);

	__super::Free();
}
