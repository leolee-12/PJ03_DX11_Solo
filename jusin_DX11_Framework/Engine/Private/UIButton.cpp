#include "UIButton.h"

#include "GameInstance.h"

CUIButton::CUIButton(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIObject{ pDevice, pContext }
{
}

CUIButton::CUIButton(const CUIButton& Prototype)
	: CUIObject{ Prototype }
	, m_strTextureTag{ Prototype.m_strTextureTag }
	, m_strShaderTag{ Prototype.m_strShaderTag }
	, m_strVIBufferTag{ Prototype.m_strVIBufferTag }
	, m_iTextureIndex{ Prototype.m_iTextureIndex }
	, m_iTextureLevel{ Prototype.m_iTextureLevel }
	, m_iShaderLevel{ Prototype.m_iShaderLevel }
	, m_iVIBufferLevel{ Prototype.m_iVIBufferLevel }
	, m_bInteractable{ Prototype.m_bInteractable }
	, m_vColor{ Prototype.m_vColor }
{
}

void CUIButton::Set_State(UI_BUTTON_STATE eState)
{
	if (ETOUI(UI_BUTTON_STATE::END) <= ETOUI(eState))
		return;

	if (false == m_bInteractable && UI_BUTTON_STATE::DISABLED != eState)
		return;

	const UI_BUTTON_STATE eOld = m_eState;
	if (eOld == eState)
		return;

	m_eState = eState;
	On_State_Changed(eOld, eState);
}

void CUIButton::Set_Interactable(_bool bInteractable)
{
	if (m_bInteractable == bInteractable)
		return;

	m_bInteractable = bInteractable;

	if (false == m_bInteractable)
	{
		Set_State(UI_BUTTON_STATE::DISABLED);
		return;
	}

	if (UI_BUTTON_STATE::DISABLED == m_eState)
		Set_State(UI_BUTTON_STATE::NORMAL);
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
	__super::Update(fTimeDelta);
}

void CUIButton::Late_Update(_float fTimeDelta)
{
	if (!m_bVisible) return;

	m_pGameInstance->Add_RenderGroup(RENDERID::UI, this);
}

HRESULT CUIButton::Render()
{
	if (nullptr == m_pShaderCom || nullptr == m_pTextureCom || nullptr == m_pVIBufferCom)
		return S_OK;

	if (FAILED(Bind_BaseMatrices(m_pShaderCom)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_iTextureIndex)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_vColor, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Resources()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

_bool CUIButton::Can_Apply_Tween_Target(UI_TWEEN_TARGET eTarget) const
{
	switch (eTarget)
	{
	case UI_TWEEN_TARGET::COLOR_R:
	case UI_TWEEN_TARGET::COLOR_G:
	case UI_TWEEN_TARGET::COLOR_B:
	case UI_TWEEN_TARGET::COLOR_A:
		return true;

	default: return __super::Can_Apply_Tween_Target(eTarget);
	}
}
HRESULT CUIButton::Apply_Tween_Target(UI_TWEEN_TARGET eTarget, _float fValue)
{
	switch (eTarget)
	{
	case UI_TWEEN_TARGET::COLOR_R: m_vColor.x = fValue; return S_OK;
	case UI_TWEEN_TARGET::COLOR_G: m_vColor.y = fValue; return S_OK;
	case UI_TWEEN_TARGET::COLOR_B: m_vColor.z = fValue; return S_OK;
	case UI_TWEEN_TARGET::COLOR_A: m_vColor.w = fValue; return S_OK;
	default: return __super::Apply_Tween_Target(eTarget, fValue);
	}
}

HRESULT CUIButton::Ready_Components()
{
	if (FAILED(__super::Add_Component(m_iShaderLevel, m_strShaderTag,
		COM_SHADER, reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(m_iTextureLevel, m_strTextureTag,
		COM_TEXTURE, reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(m_iVIBufferLevel, m_strVIBufferTag,
		COM_VIBUFFER, reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUIButton::Bind_BaseMatrices(CShader* pShader)
{
	if (nullptr == pShader)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(pShader, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(__super::Bind_ShaderResource(pShader, "g_ViewMatrix", D3DTS::VIEW)))
		return E_FAIL;
	if (FAILED(__super::Bind_ShaderResource(pShader, "g_ProjMatrix", D3DTS::PROJ)))
		return E_FAIL;

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

	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
}