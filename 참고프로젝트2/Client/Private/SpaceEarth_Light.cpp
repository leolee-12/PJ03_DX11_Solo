#include "stdafx.h"
#include "..\Public\SpaceEarth_Light.h"

#include "RenderInstance.h"
#include "GameInstance.h"

CSpaceEarth_Light::CSpaceEarth_Light(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject { pDevice, pContext }
{

}

CSpaceEarth_Light::CSpaceEarth_Light(const CSpaceEarth_Light & Prototype)
	: CGameObject{ Prototype }
{

}

HRESULT CSpaceEarth_Light::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSpaceEarth_Light::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;
	//m_pTransformCom->Set_Scaled(1.f, 1.f, 1.f);
	m_pTransformCom->Rotation(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(-90.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(100.f, -100.f, -200.f, 1.f));

	m_iGameObjectData = 7;
	m_RendererDesc.tGlowDesc.iPassIndex = 7;
	m_RendererDesc.tGlowDesc.fGlowFactor = 3.2f;
	m_RendererDesc.tGlowDesc.vGlowColor = _float4(0, 0, 1, 1);

	CMap_Manager::Get_Instance()->Push_MapObject(CMap_Manager::MAP_SPACE,
		static_cast<CMap_Manager::Map_Object_Key*>(pArg)->m_PrototypeKey, this);
	return S_OK;
}

void CSpaceEarth_Light::Camera_Update(_float fTimeDelta)
{
	m_fAccTime += fTimeDelta;

	if (m_isStarSwitch == false)
	{
		m_fMaskStar_Value_1 += fTimeDelta * 0.2f;
		m_fMaskStar_Value_2 -= fTimeDelta * 0.2f;

		if (m_fMaskStar_Value_1 >= 0.3f)
		{
			m_isStarSwitch = true;
			m_fMaskStar_Value_1 = 0.3f;
			m_fMaskStar_Value_2 = 0.f;
		}
	}
	else
	{
		m_fMaskStar_Value_1 -= fTimeDelta * 0.2f;
		m_fMaskStar_Value_2 += fTimeDelta * 0.2f;

		if (m_fMaskStar_Value_1 <= 0)
		{
			m_isStarSwitch = false;
			m_fMaskStar_Value_1 = 0.f;
			m_fMaskStar_Value_2 = 0.3f;
		}
	}
}

void CSpaceEarth_Light::Update(_float fTimeDelta)
{
}

void CSpaceEarth_Light::Late_Update(_float fTimeDelta)
{
	//_vector vPos = m_pGameInstance->Get_CamPosition_Vector();
	//vPos = XMVectorSetZ(vPos, XMVectorGetZ(vPos) + 100);

	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);

	m_pRenderInstance->Add_RenderObject(CRenderer::RG_GLOW_PRI, this, &m_RendererDesc);
}

HRESULT CSpaceEarth_Light::Render(_float fTimeDelta)
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pShaderCom->Begin(3)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CSpaceEarth_Light::Ready_Components()
{
	/* Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxSpace"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Space_Earth_Diffuse"),
		TEXT("Com_Texture_Diffuse"), reinterpret_cast<CComponent**>(&m_pTextureCom_Diffuse))))
		return E_FAIL;
	
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Space_Earth_Light"),
		TEXT("Com_Texture_Light"), reinterpret_cast<CComponent**>(&m_pTextureCom_Light))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Space_Earth_Shadow"),
		TEXT("Com_Texture_Shadow"), reinterpret_cast<CComponent**>(&m_pTextureCom_Shadow))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Space_Earth_Cloud"),
		TEXT("Com_Texture_Cloud"), reinterpret_cast<CComponent**>(&m_pTextureCom_Cloud))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_SpaceEarth"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CSpaceEarth_Light::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;


	if (FAILED(m_pTextureCom_Diffuse->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", 0)))
		return E_FAIL;

	if (FAILED(m_pTextureCom_Light->Bind_ShaderResource(m_pShaderCom, "g_EarthLight", 0)))
		return E_FAIL;
	if (FAILED(m_pTextureCom_Shadow->Bind_ShaderResource(m_pShaderCom, "g_EarthShadow", 0)))
		return E_FAIL;

	if (FAILED(m_pTextureCom_Cloud->Bind_ShaderResource(m_pShaderCom, "g_EarthCloud0", 0)))
		return E_FAIL;
	if (FAILED(m_pTextureCom_Cloud->Bind_ShaderResource(m_pShaderCom, "g_EarthCloud1", 1)))
		return E_FAIL;
	if (FAILED(m_pTextureCom_Cloud->Bind_ShaderResource(m_pShaderCom, "g_EarthCloud2", 2)))
		return E_FAIL;

	//if (FAILED(m_pShaderCom->Bind_RawValue("g_MaskStar_Value_1", &m_fMaskStar_Value_1, sizeof(float))))
	//	return E_FAIL;

	//if (FAILED(m_pShaderCom->Bind_RawValue("g_MaskStar_Value_2", &m_fMaskStar_Value_2, sizeof(float))))
	//	return E_FAIL;

	//if (FAILED(m_pShaderCom->Bind_RawValue("g_Time", &m_fAccTime, sizeof(float))))
	//	return E_FAIL;
	
	return S_OK;
}

CSpaceEarth_Light * CSpaceEarth_Light::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CSpaceEarth_Light*		pInstance = new CSpaceEarth_Light(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CSpaceEarth_Light"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CSpaceEarth_Light::Clone(void * pArg)
{
	CSpaceEarth_Light*		pInstance = new CSpaceEarth_Light(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CSpaceEarth_Light"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSpaceEarth_Light::Free()
{

	Safe_Release(m_pTextureCom_Diffuse);
	Safe_Release(m_pTextureCom_Light);
	Safe_Release(m_pTextureCom_Shadow);
	Safe_Release(m_pTextureCom_Cloud);

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);

	__super::Free();
}
