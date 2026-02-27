#include "stdafx.h"

#include "UI_SkillEffect.h"
#include "RenderInstance.h"

CUI_SkillEffect::CUI_SkillEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUI_Skill{ pDevice ,pContext }
{
}

CUI_SkillEffect::CUI_SkillEffect(const CUI_SkillEffect& Prototype)
	:CUI_Skill{ Prototype }
{
}

HRESULT CUI_SkillEffect::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_SkillEffect::Initialize(void* pArg)
{
	m_fPosX = 340.f;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_iTotalIndex = 4;
	// 665
	__super::Set_UI_Setting(408.f , 102.f , m_fPosX, 650, 0.1f);

	return S_OK;
}

void CUI_SkillEffect::Camera_Update(_float fTimeDelta)
{
	__super::Camera_Update(fTimeDelta);

	m_fAnimTimer += fTimeDelta * 10; 
	m_fRenderTimer += fTimeDelta;
}

void CUI_SkillEffect::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);

	if (m_fAnimTimer >= 1.f)
	{
		m_iCurrIndex++;
		m_fAnimTimer = 0.f;

		if (m_iCurrIndex >= 4)
		{
			m_iCurrIndex = 0;
			
		}
	}

	if (m_fRenderTimer >= 2.f)
	{
		m_fRenderTimer = 0.f;
	}

}

void CUI_SkillEffect::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	if(m_iSkillNumber >= 4 && m_fRenderTimer >= 1.75f)
	m_pRenderInstance->Add_RenderObject(CRenderer::RG_UI, this);
}

HRESULT CUI_SkillEffect::Render(_float fTimeDelta)
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(10)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_SkillEffect::Ready_Components()
{
	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_SkillEffect"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;


	return S_OK;
}

HRESULT CUI_SkillEffect::Bind_ShaderResources()
{
	if (FAILED(__super::Bind_ShaderResources()))
		return E_FAIL;

	
	if (FAILED(m_pShaderCom->Bind_RawValue("iNumSprite", &m_iTotalIndex, sizeof(_uint))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("iSpriteIndex", &m_iCurrIndex, sizeof(_uint))))
		return E_FAIL;

	return S_OK;
}

CUI_SkillEffect* CUI_SkillEffect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_SkillEffect* pInstatnce = new CUI_SkillEffect(pDevice, pContext);

	if (FAILED(pInstatnce->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUI_SkillEffect"));
		Safe_Release(pInstatnce);
	}

	return pInstatnce;
}

CGameObject* CUI_SkillEffect::Clone(void* pArg)
{
	CUI_SkillEffect* pInstatnce = new CUI_SkillEffect(*this);

	if (FAILED(pInstatnce->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloend : CUI_SkillEffect"));
		Safe_Release(pInstatnce);
	}

	return pInstatnce;
}

void CUI_SkillEffect::Free()
{
	__super::Free();
}
