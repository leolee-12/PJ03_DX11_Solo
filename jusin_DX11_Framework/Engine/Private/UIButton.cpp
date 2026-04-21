#include "UIButton.h"

#include "GameInstance.h"

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

_uint CUIButton::Get_CurrentTextureIndex() const
{
	_uint iIndex = m_iTextureIndices[ETOUI(m_eState)];

	if (INVALID_INDEX != iIndex)
		return iIndex;

	return m_iTextureIndices[ETOUI(UI_BUTTON_STATE::NORMAL)];
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
		m_strShaderTag = pDesc->strShaderTag;
		m_strVIBufferTag = pDesc->strVIBufferTag;

		m_iTextureLevel = pDesc->iTextureLevel;
		m_iShaderLevel = pDesc->iShaderLevel;
		m_iVIBufferLevel = pDesc->iVIBufferLevel;

		m_iTextureIndices[ETOUI(UI_BUTTON_STATE::NORMAL)]	= pDesc->iNormalTextureIndex;
		m_iTextureIndices[ETOUI(UI_BUTTON_STATE::HOVER)]	= pDesc->iHoverTextureIndex;
		m_iTextureIndices[ETOUI(UI_BUTTON_STATE::PRESSED)]	= pDesc->iPressedTextureIndex;
		m_iTextureIndices[ETOUI(UI_BUTTON_STATE::DISABLED)]	= pDesc->iDisabledTextureIndex;
		
		Set_Interactable(pDesc->bInteractable);
		m_vColor = pDesc->vColor;
	}

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
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
	if (!m_bVisible) return;

	m_pGameInstance->Add_RenderGroup(RENDERID::UI, this);
}

HRESULT CUIButton::Render()
{
	const _uint iIndex = Get_CurrentTextureIndex();
	if (!Has_ValidData(iIndex)) return S_OK;

	if (FAILED(Bind_ShaderResources(iIndex))) return E_FAIL;
	if (FAILED(m_pShaderCom->Begin(0))) return E_FAIL;
	if (FAILED(m_pVIBufferCom->Bind_Resources())) return E_FAIL;
	if (FAILED(m_pVIBufferCom->Render())) return E_FAIL;
	return S_OK;
}

HRESULT CUIButton::Ready_Components()
{
	if (FAILED(__super::Add_Component(m_iShaderLevel, m_strShaderTag, COM_SHADER, reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(m_iVIBufferLevel, m_strVIBufferTag, COM_VIBUFFER, reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(m_iTextureLevel, m_strTextureTag, COM_TEXTURE, reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUIButton::Bind_ShaderResources(_uint iTextureIndex)
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(__super::Bind_ShaderResource(m_pShaderCom, "g_ViewMatrix", D3DTS::VIEW)))
		return E_FAIL;
	if (FAILED(__super::Bind_ShaderResource(m_pShaderCom, "g_ProjMatrix", D3DTS::PROJ)))
		return E_FAIL;
	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", iTextureIndex)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_vColor, sizeof(_float4))))
		return E_FAIL;

	return S_OK;
}

_bool CUIButton::Has_ValidData(_uint iTextureIndex) const
{
	return (nullptr != m_pShaderCom)
		&& (nullptr != m_pVIBufferCom)
		&& (nullptr != m_pTextureCom)
		&& (INVALID_INDEX != iTextureIndex);
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

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);
}