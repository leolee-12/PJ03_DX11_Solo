#include "stdafx.h"

#include "UI_SkillNumber.h"
#include "RenderInstance.h"

CUI_SkillNumber::CUI_SkillNumber(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUI_Skill{ pDevice ,pContext }
{
}

CUI_SkillNumber::CUI_SkillNumber(const CUI_SkillNumber& Prototype)
	:CUI_Skill{ Prototype }
{
}

HRESULT CUI_SkillNumber::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_SkillNumber::Initialize(void* pArg)
{
	m_fPosX = 85.f;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	__super::Set_UI_Setting(117.f, 117.f, m_fPosX, 650);

	return S_OK;
}

void CUI_SkillNumber::Camera_Update(_float fTimeDelta)
{
	__super::Camera_Update(fTimeDelta);

	
}

void CUI_SkillNumber::Update(_float fTimeDelta)
{
	if (m_fColorValue >= 1.f)
		m_bSignSwitch = FALSE;
	else if (m_fColorValue <= 0.5f)
		m_bSignSwitch = TRUE;

	m_bSignSwitch ? m_fColorValue += fTimeDelta : m_fColorValue -= fTimeDelta;
}

void CUI_SkillNumber::Late_Update(_float fTimeDelta)
{
	m_pRenderInstance->Add_RenderObject(CRenderer::RG_UI, this);
}

HRESULT CUI_SkillNumber::Render(_float fTimeDelta)
{
	if (FAILED(__super::Bind_ShaderResources()))
		return E_FAIL;;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_iSkillNumber)))
		return E_FAIL;

	_uint iPass = 0;

	if (m_iSkillNumber == 7)
	{
		iPass = 2;
		_vector vColor = { 1 + m_fColorValue , 1 + m_fColorValue, 1 + m_fColorValue };
		if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &vColor, sizeof(_vector))))
			return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Begin(iPass)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_SkillNumber::Ready_Components()
{
	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_SPNum"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;


	return S_OK;
}

CUI_SkillNumber* CUI_SkillNumber::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_SkillNumber* pInstatnce = new CUI_SkillNumber(pDevice, pContext);

	if (FAILED(pInstatnce->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUI_SkillNumber"));
		Safe_Release(pInstatnce);
	}

	return pInstatnce;
}

CGameObject* CUI_SkillNumber::Clone(void* pArg)
{
	CUI_SkillNumber* pInstatnce = new CUI_SkillNumber(*this);

	if (FAILED(pInstatnce->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloend : CUI_SkillNumber"));
		Safe_Release(pInstatnce);
	}

	return pInstatnce;
}

void CUI_SkillNumber::Free()
{
	__super::Free();
}
