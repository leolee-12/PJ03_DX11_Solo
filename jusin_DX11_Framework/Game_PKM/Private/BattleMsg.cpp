#include "BattleMsg.h"
#include "UIImage.h"
#include "UIText.h"

CBattleMsg::CBattleMsg()
{
}

void CBattleMsg::Set_Message(const _wstring& strMessage)
{
	m_strFull = strMessage;
	On_Refresh();
}

void CBattleMsg::Complete()
{
	m_iCharsShown = static_cast<_uint>(m_strFull.length());

	if (nullptr != m_pText)
		m_pText->Set_Text(m_strFull);

	if (nullptr != m_pIcon)
		m_pIcon->Set_Visible(true);

	m_bIconActive = true;
}

_bool CBattleMsg::Is_Done() const
{
	return m_iCharsShown >= static_cast<_uint>(m_strFull.length());
}

HRESULT CBattleMsg::Resolve_Widgets()
{
	static constexpr const _char* s_IconWidgetId = "widget_001";
	static constexpr const _char* s_BoxWidgetId = "widget_002";
	static constexpr const _char* s_TextWidgetId = "widget_003";

	m_pIcon = Find_Widget_As<CUIImage>(s_IconWidgetId);
	if (nullptr == m_pIcon)
		return E_FAIL;

	m_pBox = Find_Widget_As<CUIImage>(s_BoxWidgetId);
	if (nullptr == m_pBox)
		return E_FAIL;

	m_pText = Find_Widget_As<CUIText>(s_TextWidgetId);
	if (nullptr == m_pText)
		return E_FAIL;

	On_Refresh();

	return S_OK;
}

void CBattleMsg::On_Refresh()
{
	m_fTimer = 0.f;
	m_iCharsShown = 0;
	m_bIconActive = false;

	if (nullptr != m_pIcon)
		m_pIcon->Set_Visible(false);

	if (nullptr != m_pText)
		m_pText->Set_Text(L"");
}

void CBattleMsg::On_Update(_float fTimeDelta)
{
	if (nullptr == m_pText)
		return;

	if (true == Is_Done())
	{
		if (false == m_bIconActive && nullptr != m_pIcon)
		{
			m_pIcon->Set_Visible(true);
			m_bIconActive = true;
		}

		return;
	}

	if (nullptr != m_pIcon)
		m_pIcon->Set_Visible(false);

	m_fTimer += fTimeDelta;

	const _uint iLength = static_cast<_uint>(m_strFull.length());
	const _uint iTargetChars = static_cast<_uint>(m_fTimer * m_fCharsPerSecond);

	m_iCharsShown = iTargetChars;
	if (m_iCharsShown > iLength)
		m_iCharsShown = iLength;

	m_pText->Set_Text(m_strFull.substr(0, m_iCharsShown));
}

CBattleMsg* CBattleMsg::Create()
{
	return new CBattleMsg();
}

void CBattleMsg::Free()
{
	m_pIcon = nullptr;  // weak
	m_pBox = nullptr;   // weak
	m_pText = nullptr;  // weak

	__super::Free();
}