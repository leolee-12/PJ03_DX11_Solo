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
        m_eVAlign = pDesc->eVAlign;
        m_bWordWrap = pDesc->bWordWrap;
        m_bClipToRect = pDesc->bClipToRect;
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

    const _float4 vRect = Get_RenderRect();

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

    const UICANVAS_TRANSFORM& tCanvas = Get_CanvasTransform();

    _float2 vDrawScale{ 1.f, 1.f };
    switch (Get_CanvasDesc().eScalePolicy)
    {
    case UI_SCALE_POLICY::STRETCH:
        vDrawScale = { tCanvas.fScaleX, tCanvas.fScaleY };
        break;

    case UI_SCALE_POLICY::MATCH_WIDTH:
        vDrawScale = { tCanvas.fScaleX, tCanvas.fScaleX };
        break;

    case UI_SCALE_POLICY::MATCH_HEIGHT:
        vDrawScale = { tCanvas.fScaleY, tCanvas.fScaleY };
        break;

    case UI_SCALE_POLICY::UNIFORM_FIT:
    default:
        vDrawScale = { tCanvas.fUniformScale, tCanvas.fUniformScale };
        break;
    }

    const XMVECTOR vColor = XMLoadFloat4(&m_vColor);
    const _float2 vDrawPos = { fAnchorX, fAnchorY };

    if (m_bWordWrap)
    {
        vector<_wstring> Lines;
        _wstring strLine;

        auto PushLine = [&]()
            {
                Lines.push_back(strLine);
                strLine.clear();
            };

        for (wchar_t ch : m_strText)
        {
            if (L'\r' == ch)
                continue;

            if (L'\n' == ch)
            {
                PushLine();
                continue;
            }

            _wstring strCandidate = strLine;
            strCandidate.push_back(ch);

            const _float2 vCandidateSize = m_pGameInstance->Measure_Text(m_strFontTag, strCandidate.c_str());
            if (false == strLine.empty() && vCandidateSize.x * vDrawScale.x > vRect.z)
            {
                PushLine();
                strLine.push_back(ch);
            }
            else
            {
                strLine = strCandidate;
            }
        }

        if (false == strLine.empty() || Lines.empty())
            Lines.push_back(strLine);

        const _float2 vLineBase = m_pGameInstance->Measure_Text(m_strFontTag, L"A");
        const _float fLineHeight = vLineBase.y * vDrawScale.y;
        if (fLineHeight <= 0.f)
            return S_OK;

        _uint iDrawableLines = static_cast<_uint>(Lines.size());
        if (m_bClipToRect)
        {
            const _uint iMaxLines = static_cast<_uint>(vRect.w / fLineHeight);
            if (0 == iMaxLines)
                return S_OK;

            if (iDrawableLines > iMaxLines)
                iDrawableLines = iMaxLines;
        }

        const _float fTotalHeight = fLineHeight * static_cast<_float>(iDrawableLines);
        _float fStartY = vRect.y + (vRect.w - fTotalHeight) * 0.5f;

        if (UI_TEXT_VALIGN::TOP == m_eVAlign)
            fStartY = vRect.y;
        else if (UI_TEXT_VALIGN::BOTTOM == m_eVAlign)
            fStartY = vRect.y + vRect.w - fTotalHeight;

        for (_uint i = 0; i < iDrawableLines; ++i)
        {
            if (Lines[i].empty())
                continue;

            _float2 vLineOrigin{ 0.f, 0.f };
            const _float2 vLineSize = m_pGameInstance->Measure_Text(m_strFontTag, Lines[i].c_str());

            if (UI_TEXT_ALIGN::CENTER == m_eAlign)
                vLineOrigin.x = vLineSize.x * 0.5f;
            else if (UI_TEXT_ALIGN::RIGHT == m_eAlign)
                vLineOrigin.x = vLineSize.x;

            if (FAILED(m_pGameInstance->Draw_Text(m_strFontTag,
                Lines[i].c_str(),
                { fAnchorX, fStartY + fLineHeight * static_cast<_float>(i) },
                vColor,
                m_fRotation,
                vLineOrigin,
                vDrawScale)))
                return E_FAIL;
        }

        return S_OK;
    }

    if (FAILED(m_pGameInstance->Draw_Text(m_strFontTag,
        m_strText.c_str(),
        vDrawPos,
        vColor,
        m_fRotation,
        vOrigin,
        vDrawScale)))
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
