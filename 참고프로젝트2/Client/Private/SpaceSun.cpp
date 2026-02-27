#include "stdafx.h"
#include "..\Public\SpaceSun.h"

#include "RenderInstance.h"
#include "GameInstance.h"

CSpaceSun::CSpaceSun(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject { pDevice, pContext }
{

}

CSpaceSun::CSpaceSun(const CSpaceSun & Prototype)
	: CGameObject{ Prototype }
{

}

HRESULT CSpaceSun::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSpaceSun::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	//m_pTransformCom->Set_Scaled(0.01f, 0.01f, 0.01f);
	//m_pTransformCom->Rotation(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(180.f));

	/*
	    string          strTagName; 
    XMFLOAT4        vGlowColor;
    float           fGlowFactor;
    unsigned int    iPassIndex;
	*/
	m_RendererDesc.tGlowDesc.iPassIndex = 8;
	m_RendererDesc.tGlowDesc.fGlowFactor = 1.f;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, -1.5f, -200.f, 1.f));
	m_iGameObjectData = 8;
	CMap_Manager::Get_Instance()->Push_MapObject(CMap_Manager::MAP_SPACE,
		static_cast<CMap_Manager::Map_Object_Key*>(pArg)->m_PrototypeKey, this);
	return S_OK;
}

void CSpaceSun::Camera_Update(_float fTimeDelta)
{
	static bool isOneCheck = false;
	if (m_isPlus == true && m_isMaintain == false)
	{
		m_fAccTime += fTimeDelta * 0.25;
		isOneCheck = false;
	}
	else if (m_isPlus == false && m_isMaintain == false)
		m_fAccTime -= fTimeDelta * 0.25;

	if (m_isMaintain == false && m_isPlus == true && m_fAccTime > 0.7f )
	{
		m_fAccTime = 0.7f;
		m_isMaintain = true;

		m_fOneTime = 0.f;
	}
	else if (m_isMaintain == false && m_isPlus == false && m_fAccTime < 0.3f)
	{
		m_fAccTime = 0.3f;
		m_isMaintain = true;

		m_fOneTime = -3.25f;
		
	}

	if (m_isMaintain == true)
	{
		if (isOneCheck == false && m_isRainbowSwitch == false)
		{
			m_isRainbowSwitch = true;
			isOneCheck = true;
		}

		m_fOneTime += fTimeDelta;
		if (m_fOneTime > 3.5f)
		{
			
			m_isMaintain = false;
			m_fOneTime = 0.f;
			if(m_isPlus == true)
				m_fAccTime = 0.7f;
			else
				m_fAccTime = 0.3f;

			m_isPlus = !m_isPlus;
		}
	}

	//if (m_isRainbowSwitch == false)
	//{
	//	m_fRainbowTime += fTimeDelta;

	//	if (m_fRainbowTime > 5.f)
	//	{
	//		m_isRainbowSwitch = true;
	//		m_fRainbowTime = 0.f;
	//	}
	//}

	if (m_isRainbowSwitch == true)
	{
		m_fRainbowTime += fTimeDelta * 0.5f;
		m_pTransformCom_Rainbow->Set_Scaled(0.6f + m_fRainbowTime, 0.6f + m_fRainbowTime, 1.f + m_fRainbowTime);
		
		if (m_fRainbowTime > 1.f)
		{
			m_isRainbowSwitch = false;
			m_fRainbowTime = 0.f;
			m_pTransformCom_Rainbow->Set_Scaled(0.6f, 0.6f, 1.f);
		}
	}

}

void CSpaceSun::Update(_float fTimeDelta)
{

}

void CSpaceSun::Late_Update(_float fTimeDelta)
{
	m_pRenderInstance->Add_RenderObject(CRenderer::RG_GLOW_PRI, this, &m_RendererDesc);
	m_pRenderInstance->Add_RenderObject(CRenderer::RG_BLEND_PRI, this);
	m_pRenderInstance->Add_RenderObject(CRenderer::RG_BLEND_PRI, this);
}

HRESULT CSpaceSun::Render(_float fTimeDelta)
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;
	_int iCount;
	if (m_iIndex == 1)
	{
		iCount = 1;
		if (FAILED(m_pShaderCom->Bind_RawValue("g_SunMeshIndex", &iCount, sizeof(int))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Bind_RawValue("g_Time", &m_fAccTime, sizeof(float))))
			return E_FAIL;
		
		if (FAILED(m_pModelCom->Bind_MaterialSRV(m_pShaderCom, aiTextureType_DIFFUSE, "g_DiffuseTexture", 1)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(7)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(1)))
			return E_FAIL;

		m_iIndex = 2;
	}
	else if (m_iIndex == 0)
	{
		iCount = 0;
		if (FAILED(m_pShaderCom->Bind_RawValue("g_SunMeshIndex", &iCount, sizeof(int))))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_MaterialSRV(m_pShaderCom, aiTextureType_DIFFUSE, "g_DiffuseTexture", 0)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(7)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(0)))
			return E_FAIL;

		m_iIndex = 1;

	}
	else if (m_iIndex == 2)
	{
		m_iIndex = 0;
		if (m_isRainbowSwitch == false)
			return S_OK;

		if (FAILED(m_pTransformCom_Rainbow->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_RawValue("g_Time", &m_fRainbowTime, sizeof(float))))
			return E_FAIL;

		if (FAILED(m_pModelCom_Rainbow->Bind_MaterialSRV(m_pShaderCom, aiTextureType_DIFFUSE, "g_DiffuseTexture", 0)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(8)))
			return E_FAIL;

		if (FAILED(m_pModelCom_Rainbow->Render(0)))
			return E_FAIL;

	}



	return S_OK;
}

HRESULT CSpaceSun::Ready_Components()
{
	/* Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxSpace"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;
	/* Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Space_Light"),
		TEXT("Com_Texture_Light"), reinterpret_cast<CComponent**>(&m_pTextureCom_Light))))
		return E_FAIL;
	/* Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Space_Rainbow"),
		TEXT("Com_Texture_Rainbow"), reinterpret_cast<CComponent**>(&m_pTextureCom_Rainbow))))
		return E_FAIL;
	//Prototype_Component_Texture_Space_Light Prototype_Component_Texture_Space_Rainbow
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_SpaceSun"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_SpaceRainbow"),
		TEXT("Com_Rect"), reinterpret_cast<CComponent**>(&m_pModelCom_Rainbow))))
		return E_FAIL;

	m_pTransformCom_Rainbow = CTransform::Create(m_pDevice, m_pContext);

	if (nullptr == m_pTransformCom_Rainbow)
		return E_FAIL;

	m_pTransformCom_Rainbow->Set_Scaled(0.5f, 0.5f, 1.f);
	m_pTransformCom_Rainbow->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 75.f, 300.f, 1.f));
	
	return S_OK;
}

HRESULT CSpaceSun::Bind_ShaderResources()
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

CSpaceSun * CSpaceSun::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CSpaceSun*		pInstance = new CSpaceSun(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CSpaceSun"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CSpaceSun::Clone(void * pArg)
{
	CSpaceSun*		pInstance = new CSpaceSun(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CSpaceSun"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSpaceSun::Free()
{

	Safe_Release(m_pTransformCom_Rainbow);
	Safe_Release(m_pModelCom_Rainbow);
	Safe_Release(m_pTextureCom_Rainbow);
	Safe_Release(m_pTextureCom_Light);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);

	__super::Free();
}
