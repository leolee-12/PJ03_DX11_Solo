#include "UIButton.h"

CUIButton::CUIButton(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIObject{ pDevice, pContext }
{
}

CUIButton::CUIButton(const CUIButton& Prototype)
	: CUIObject{ Prototype }
{
}

void CUIButton::Set_State(UI_BUTTON_STATE eState)
{
	if (ETOUI(UI_BUTTON_STATE::END) <= ETOUI(eState))
		return;

	if (false == m_bInteractable && UI_BUTTON_STATE::DISABLED != eState)
		return;

	m_eState = eState;
}

void CUIButton::Set_Interactable(_bool bInteractable)
{
	m_bInteractable = bInteractable;

	if (false == m_bInteractable)
	{
		m_eState = UI_BUTTON_STATE::DISABLED;
		return;
	}

	if (UI_BUTTON_STATE::DISABLED == m_eState)
		m_eState = UI_BUTTON_STATE::NORMAL;
}

void CUIButton::Set_TextureIndex(UI_BUTTON_STATE eState, _uint iTextureIndex)
{
	if (ETOUI(UI_BUTTON_STATE::END) <= ETOUI(eState))
		return;

	m_iTextureIndices[ETOUI(eState)] = iTextureIndex;
}

_uint CUIButton::Get_TextureIndex(UI_BUTTON_STATE eState) const
{
	if (ETOUI(UI_BUTTON_STATE::END) <= ETOUI(eState))
		return INVALID_INDEX;

	return m_iTextureIndices[ETOUI(eState)];
}

HRESULT CUIButton::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUIButton::Initialize(void* pArg)
{
	if (nullptr != pArg)
	{
		auto pDesc = static_cast<UIBUTTON_DESC*>(pArg);
		m_strTextureTag = pDesc->strTextureTag;
		m_iTextureIndices[ETOUI(UI_BUTTON_STATE::NORMAL)]	= pDesc->iNormalTextureIndex;
		m_iTextureIndices[ETOUI(UI_BUTTON_STATE::HOVER)]	= pDesc->iHoverTextureIndex;
		m_iTextureIndices[ETOUI(UI_BUTTON_STATE::PRESSED)]	= pDesc->iPressedTextureIndex;
		m_iTextureIndices[ETOUI(UI_BUTTON_STATE::DISABLED)]	= pDesc->iDisabledTextureIndex;
		Set_Interactable(pDesc->bInteractable);
	}

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CUIButton::Priority_Update(_float fTimeDelta)
{
}

void CUIButton::Update(_float fTimeDelta)
{
}

void CUIButton::Late_Update(_float fTimeDelta)
{
}

HRESULT CUIButton::Render()
{
	return S_OK;
}

CUIButton* CUIButton::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUIButton* pInstance = new CUIButton(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CUIButton");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUIButton::Clone(void* pArg)
{
	CUIButton* pInstance = new CUIButton(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUIButton");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUIButton::Free()
{
	__super::Free();
}