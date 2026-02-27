#include "stdafx.h"

#include "UI_ComboFont.h"
#include "RenderInstance.h"

CUI_ComboFont::CUI_ComboFont(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUI_Combo{ pDevice ,pContext }
{
}

CUI_ComboFont::CUI_ComboFont(const CUI_ComboFont& Prototype)
	:CUI_Combo{ Prototype }
{
}

HRESULT CUI_ComboFont::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_ComboFont::Initialize(void* pArg)
{

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	UI_DESC* pComboDesc = static_cast<UI_DESC*>(pArg);
	m_eLRPos = pComboDesc->eLRPos;

	m_fPosX = 253.f;

	if (m_eLRPos == LEFT)
	{
		m_fPosX += 1280 * 0.5f + 300.f;
	}

	__super::Set_UI_Setting(162.f, 162.f, m_fPosX, 268.f, 0.f);

	return S_OK;
}

void CUI_ComboFont::Camera_Update(_float fTimeDelta)
{
	__super::Camera_Update(fTimeDelta);
}

void CUI_ComboFont::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
}

void CUI_ComboFont::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	if(m_iComboCount >= 2)
		m_pRenderInstance->Add_RenderObject(CRenderer::RG_UI, this);
}

HRESULT CUI_ComboFont::Render(_float fTimeDelta)
{
	if (FAILED(__super::Bind_ShaderResources()))
		return E_FAIL;;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", SetColor())))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_ComboFont::Ready_Components()
{
	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_ComboFont"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;


	return S_OK;
}

CUI_ComboFont* CUI_ComboFont::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_ComboFont* pInstatnce = new CUI_ComboFont(pDevice, pContext);

	if (FAILED(pInstatnce->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUI_ComboFont"));
		Safe_Release(pInstatnce);
	}

	return pInstatnce;
}

CGameObject* CUI_ComboFont::Clone(void* pArg)
{
	CUI_ComboFont* pInstatnce = new CUI_ComboFont(*this);

	if (FAILED(pInstatnce->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloend : CUI_ComboFont"));
		Safe_Release(pInstatnce);
	}

	return pInstatnce;
}

void CUI_ComboFont::Free()
{
	__super::Free();
}
