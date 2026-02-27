#include "stdafx.h"

#include "UI_CharaSelectLight.h"
#include "RenderInstance.h"

CUI_CharaSelectLight::CUI_CharaSelectLight(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUIObject{ pDevice ,pContext }
{
}

CUI_CharaSelectLight::CUI_CharaSelectLight(const CUI_CharaSelectLight& Prototype)
	:CUIObject{ Prototype }
{
}

HRESULT CUI_CharaSelectLight::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_CharaSelectLight::Initialize(void* pArg)
{

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_fSizeX = 200 , m_fSizeY = 200;
	m_fPosX = 630, m_fPosY = 160;

	UI_DESC* Desc = static_cast<UI_DESC*>(pArg);
	m_iTextureIndex = Desc->iNumUI;

	if (m_iTextureIndex == 1)
	{
		m_fScaledValue = 50.f;
		m_fSizeX = 400.f;
	}


	__super::Set_UI_Setting(m_fSizeX, m_fSizeY, m_fPosX, m_fPosY, 0.f);

	return S_OK;
}

void CUI_CharaSelectLight::Camera_Update(_float fTimeDelta)
{
	__super::Camera_Update(fTimeDelta);
}

void CUI_CharaSelectLight::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);

	ScaledAnimation(fTimeDelta);
}

void CUI_CharaSelectLight::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	RENDER_OBJECT tDesc{};
	tDesc.tGlowDesc.iPassIndex = 7;
	tDesc.tGlowDesc.fGlowFactor = 1.2f;

	m_pRenderInstance->Add_RenderObject(CRenderer::RG_PRIORITY, this );
}

HRESULT CUI_CharaSelectLight::Render(_float fTimeDelta)
{
	if (FAILED(__super::Bind_ShaderResources()))
		return E_FAIL;;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 1)))
		return E_FAIL;

	_float fAlphaValue = { 0.5f };
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlphaTimer", &(fAlphaValue), sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(25)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

void CUI_CharaSelectLight::ScaledAnimation(_float fTimeDelta)
{
	_float fLimit = 0.f;

	if (m_iTextureIndex == 0)
		fLimit = 150.f;
	else if(m_iTextureIndex == 1)
		fLimit = 50.f;

	if (m_fScaledValue >= fLimit)
		m_bValueSwitch = FALSE;
	else if(m_fScaledValue <= 0.f)
		m_bValueSwitch = TRUE;

	m_bValueSwitch ? m_fScaledValue += fTimeDelta * 40.f : m_fScaledValue -= fTimeDelta * 40.f;

	__super::Set_UI_Setting((m_fSizeX + m_fScaledValue) * ( m_iTextureIndex + 1), m_fSizeY + m_fScaledValue, m_fPosX, m_fPosY, 0.f);
}

HRESULT CUI_CharaSelectLight::Ready_Components()
{
	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_CHARACTER, TEXT("Prototype_Component_Texture_UI_CharacterSelectLight"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}


CUI_CharaSelectLight* CUI_CharaSelectLight::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_CharaSelectLight* pInstatnce = new CUI_CharaSelectLight(pDevice, pContext);

	if (FAILED(pInstatnce->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUI_CharaSelectLight"));
		Safe_Release(pInstatnce);
	}

	return pInstatnce;
}

CGameObject* CUI_CharaSelectLight::Clone(void* pArg)
{
	CUI_CharaSelectLight* pInstatnce = new CUI_CharaSelectLight(*this);

	if (FAILED(pInstatnce->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloend : CUI_CharaSelectLight"));
		Safe_Release(pInstatnce);
	}

	return pInstatnce;
}

void CUI_CharaSelectLight::Free()
{
	__super::Free();
}
