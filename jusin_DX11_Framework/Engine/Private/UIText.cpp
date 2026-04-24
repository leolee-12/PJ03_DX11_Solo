#include "UIText.h"
#include "GameInstance.h"

CUIText::CUIText(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIObject{ pDevice, pContext }
{
}

CUIText::CUIText(const CUIText& Prototype)
	: CUIObject{ Prototype }
{
}

HRESULT CUIText::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUIText::Initialize(void* pArg)
{
	if (nullptr != pArg)
	{
		auto pDesc = static_cast<UITEXT_DESC*>(pArg);
		m_strText = pDesc->strText;
		m_strFontTag = pDesc->strFontTag;
		m_vColor = pDesc->vColor;
		m_eAlign = pDesc->eAlign;
	}

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CUIText::Priority_Update(_float fTimeDelta)
{
}

void CUIText::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
}

void CUIText::Late_Update(_float fTimeDelta)
{
	if (!m_bVisible || m_strText.empty() || INVALID_TAG == m_strFontTag)
		return;

	m_pGameInstance->Add_RenderGroup(RENDERID::UI, this);
}

HRESULT CUIText::Render()
{
	if (m_strText.empty() || INVALID_TAG == m_strFontTag)
		return S_OK;

	const _float4 vRect = Get_ScreenRect();

	_float fAnchorX = vRect.x;
	switch (m_eAlign)
	{
	case UI_TEXT_ALIGN::LEFT:
		fAnchorX = vRect.x;
		break;

	case UI_TEXT_ALIGN::CENTER:
		fAnchorX = vRect.x + vRect.z * 0.5f;
		break;

	case UI_TEXT_ALIGN::RIGHT:
		fAnchorX = vRect.x + vRect.z;
		break;

	default:
		break;
	}

	const _float fAnchorY = vRect.y + vRect.w * 0.5f;
	const _float2 vTextSize = m_pGameInstance->Measure_Text(m_strFontTag, m_strText.c_str());

	_float2 vOrigin{ 0.f, vTextSize.y * 0.5f };
	switch (m_eAlign)
	{
	case UI_TEXT_ALIGN::LEFT:
		vOrigin.x = 0.f;
		break;

	case UI_TEXT_ALIGN::CENTER:
		vOrigin.x = vTextSize.x * 0.5f;
		break;

	case UI_TEXT_ALIGN::RIGHT:
		vOrigin.x = vTextSize.x;
		break;

	default:
		break;
	}

	const XMVECTOR vColor = XMLoadFloat4(&m_vColor);
	const _float2 vPosition{ fAnchorX, fAnchorY };
	const _float2 vScale{ 1.f, 1.f };

	if (FAILED(m_pGameInstance->Draw_Text(m_strFontTag,
		m_strText.c_str(),
		vPosition,
		vColor,
		m_fRotation,
		vOrigin,
		vScale)))
	{
		return E_FAIL;
	}

	return S_OK;
}

_bool CUIText::Can_Apply_Tween_Target(UI_TWEEN_TARGET eTarget) const
{
	switch (eTarget)
	{
	case UI_TWEEN_TARGET::COLOR_R:
	case UI_TWEEN_TARGET::COLOR_G:
	case UI_TWEEN_TARGET::COLOR_B:
	case UI_TWEEN_TARGET::COLOR_A:
		return true;

	default:
		return __super::Can_Apply_Tween_Target(eTarget);
	}
}

HRESULT CUIText::Apply_Tween_Target(UI_TWEEN_TARGET eTarget, _float fValue)
{
	switch (eTarget)
	{
	case UI_TWEEN_TARGET::COLOR_R:
		m_vColor.x = fValue;
		return S_OK;

	case UI_TWEEN_TARGET::COLOR_G:
		m_vColor.y = fValue;
		return S_OK;

	case UI_TWEEN_TARGET::COLOR_B:
		m_vColor.z = fValue;
		return S_OK;

	case UI_TWEEN_TARGET::COLOR_A:
		m_vColor.w = fValue;
		return S_OK;

	default:
		return __super::Apply_Tween_Target(eTarget, fValue);
	}
}

CUIText* CUIText::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUIText* pInstance = new CUIText(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CUIText");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUIText::Clone(void* pArg)
{
	CUIText* pInstance = new CUIText(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUIText");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUIText::Free()
{
	__super::Free();
}
