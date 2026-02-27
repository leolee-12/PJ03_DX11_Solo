#include "stdafx.h"

#include "UI_SkillGauge.h"
#include "RenderInstance.h"
#include "BattleInterface.h"

CUI_SkillGauge::CUI_SkillGauge(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUI_Skill{ pDevice ,pContext }
{
}

CUI_SkillGauge::CUI_SkillGauge(const CUI_SkillGauge& Prototype)
	:CUI_Skill{ Prototype }
{
}

HRESULT CUI_SkillGauge::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_SkillGauge::Initialize(void* pArg)
{

	m_fSizeX = 416.f;
	m_fPosX = 332.f;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	__super::Set_UI_Setting(m_fSizeX, 52.f, m_fPosX, 650);

	return S_OK;
}

void CUI_SkillGauge::Camera_Update(_float fTimeDelta)
{
	__super::Camera_Update(fTimeDelta);
}

void CUI_SkillGauge::Update(_float fTimeDelta)
{
	EffectRadioValue(m_fRadioValue, fTimeDelta);
}

void CUI_SkillGauge::Late_Update(_float fTimeDelta)
{
	m_pRenderInstance->Add_RenderObject(CRenderer::RG_UI, this);
}

HRESULT CUI_SkillGauge::Render(_float fTimeDelta)
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(21)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_SkillGauge::Ready_Components()
{
	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_SKillGauge"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* For.Com_Effect_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_SkillGaugeEff"),
		TEXT("Com_Effect_Texture"), reinterpret_cast<CComponent**>(&m_pEffectTexture))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_SkillGauge::Bind_ShaderResources()
{
	if (FAILED(__super::Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	if (FAILED(m_pEffectTexture->Bind_ShaderResource(m_pShaderCom, "g_MaskTexture", 0)))
		return E_FAIL;

	_uint iSkillCount = CBattleInterface_Manager::Get_Instance()->Get_KiNumber(m_eLRPos + 1);

	_bool bOnEffect = FALSE;
	(iSkillCount >= 3) ? bOnEffect = TRUE : bOnEffect = FALSE;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_Radio", &m_fRadioValue, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_bState", &bOnEffect, sizeof(_bool))))
		return E_FAIL;


	return S_OK;
}

void CUI_SkillGauge::EffectRadioValue(_float& fRadioValue, _float fTimeDelta)
{
	if (fRadioValue >= 0.5f)
		m_bValueSwitching = FALSE;
	else if(fRadioValue <= 0.f)
		m_bValueSwitching = TRUE;

	m_bValueSwitching ? fRadioValue += fTimeDelta : fRadioValue -= fTimeDelta;
	
}

CUI_SkillGauge* CUI_SkillGauge::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_SkillGauge* pInstatnce = new CUI_SkillGauge(pDevice, pContext);

	if (FAILED(pInstatnce->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUI_SkillGauge"));
		Safe_Release(pInstatnce);
	}

	return pInstatnce;
}

CGameObject* CUI_SkillGauge::Clone(void* pArg)
{
	CUI_SkillGauge* pInstatnce = new CUI_SkillGauge(*this);

	if (FAILED(pInstatnce->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloend : CUI_SkillGauge"));
		Safe_Release(pInstatnce);
	}

	return pInstatnce;
}

void CUI_SkillGauge::Free()
{
	Safe_Release(m_pEffectTexture);

	__super::Free();
}
