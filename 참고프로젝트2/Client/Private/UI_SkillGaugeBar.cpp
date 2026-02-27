#include "stdafx.h"

#include "UI_SkillGaugeBar.h"
#include "RenderInstance.h"
#include "Character.h"

CUI_SkillGaugeBar::CUI_SkillGaugeBar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUI_Skill{ pDevice ,pContext }
{
}

CUI_SkillGaugeBar::CUI_SkillGaugeBar(const CUI_SkillGaugeBar& Prototype)
	:CUI_Skill{ Prototype }
{
}

HRESULT CUI_SkillGaugeBar::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_SkillGaugeBar::Initialize(void* pArg)
{
	m_fSizeX = 416.f;
	m_fPosX = 333.f;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	__super::Set_UI_Setting(m_fSizeX, 26.f, m_fPosX, 650, 0.5f);

	return S_OK;
}

void CUI_SkillGaugeBar::Camera_Update(_float fTimeDelta)
{
	__super::Camera_Update(fTimeDelta);

	m_fMaskUVTimer += fTimeDelta *0.5f;

	if (m_fMaskUVTimer >= 1.f)
		m_fMaskUVTimer = 0.f;
}

void CUI_SkillGaugeBar::Update(_float fTimeDelta)
{

}

void CUI_SkillGaugeBar::Late_Update(_float fTimeDelta)
{
	m_pRenderInstance->Add_RenderObject(CRenderer::RG_UI, this);


}

HRESULT CUI_SkillGaugeBar::Render(_float fTimeDelta)
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(3)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_SkillGaugeBar::Ready_Components()
{
	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_SKillGaugeBar"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pSkillTexture[0]))))
		return E_FAIL;

	/* For.Com_NextTexture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_SKillGaugeBar"),
		TEXT("Com_NextTexture"), reinterpret_cast<CComponent**>(&m_pSkillTexture[1]))))
		return E_FAIL;

	/* For.Com_FlowMaskTexture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_SkillFlowEffect"),
		TEXT("Com_FlowMaskTexture"), reinterpret_cast<CComponent**>(&m_pSkillFlowEffect))))
		return E_FAIL;


	return S_OK;
}

HRESULT CUI_SkillGaugeBar::Bind_ShaderResources()
{
	if (FAILED(__super::Bind_ShaderResources()))
		return E_FAIL;;

	if (FAILED(m_pSkillTexture[0]->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_iSkillNumber)))
		return E_FAIL;

	if (m_pMainPawn != nullptr && m_pMainPawn->Get_PawnDesc().iSKillCount != 7)
	{
		if (FAILED(m_pSkillTexture[1]->Bind_ShaderResource(m_pShaderCom, "g_NextTexture", m_iSkillNumber + 1)))
			return E_FAIL;
	}

	if (FAILED(m_pSkillFlowEffect->Bind_ShaderResource(m_pShaderCom, "g_MaskTexture", 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_Radio", &m_fSkillRadio, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_MaskTimer", &m_fMaskUVTimer, sizeof(_float))))
		return E_FAIL;

	_bool bMaxGauge = FALSE;
	(m_iSkillNumber == 7) ? bMaxGauge = TRUE : bMaxGauge = FALSE;
	
	if (FAILED(m_pShaderCom->Bind_RawValue("g_bState", &bMaxGauge, sizeof(_bool))))
		return E_FAIL;

	return S_OK;
}

CUI_SkillGaugeBar* CUI_SkillGaugeBar::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_SkillGaugeBar* pInstatnce = new CUI_SkillGaugeBar(pDevice, pContext);

	if (FAILED(pInstatnce->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUI_SkillGaugeBar"));
		Safe_Release(pInstatnce);
	}

	return pInstatnce;
}

CGameObject* CUI_SkillGaugeBar::Clone(void* pArg)
{
	CUI_SkillGaugeBar* pInstatnce = new CUI_SkillGaugeBar(*this);

	if (FAILED(pInstatnce->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloend : CUI_SkillGaugeBar"));
		Safe_Release(pInstatnce);
	}

	return pInstatnce;
}

void CUI_SkillGaugeBar::Free()
{
	for (int i = 0; i < 2; i++)
	{
		Safe_Release(m_pSkillTexture[i]);
	}

	Safe_Release(m_pSkillFlowEffect);

	__super::Free();
}
