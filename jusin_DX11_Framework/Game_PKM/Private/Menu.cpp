#include "Menu.h"
#include "UISequence.h"
#include "UIButton.h"
#include "UIButton_Group.h"

CMenu::CMenu()
{
}

CMenu::MENU_ENTRY CMenu::Get_FocusedEntry() const
{
	if (nullptr == m_pGroup)
		return MENU_ENTRY::END;

	const _int iIdx = m_pGroup->Get_FocusedIndex();
	if (iIdx < 0 || iIdx >= static_cast<_int>(MENU_ENTRY::END))
		return MENU_ENTRY::END;

	return static_cast<MENU_ENTRY>(iIdx);
}

HRESULT CMenu::Resolve_Buttons()
{
	if (nullptr == m_pSequence)
		return E_FAIL;

	/* MENU_ENTRY 인덱스 ↔ uiseq widget id 매핑.
	   uiseq 변경 시 이 배열만 수정. */
	static constexpr const _char* s_WidgetIds[ETOUI(MENU_ENTRY::END)] =
	{
			"widget_002",  // PARTNER
			"widget_007",  // DEX
			"widget_003",  // BAG
			"widget_004",  // ENTRY
			"widget_005",  // LINK
			"widget_006",  // REPORT
	};

	for (_uint i = 0; i < ETOUI(MENU_ENTRY::END); ++i)
	{
		CUIObject* pObj = m_pSequence->Find_Widget(s_WidgetIds[i]);
		if (nullptr == pObj)
			return E_FAIL;

		CUIButton* pBtn = dynamic_cast<CUIButton*>(pObj);
		if (nullptr == pBtn)
			return E_FAIL;

		m_Buttons[i] = pBtn;  // weak - sequence(children) 가 소유
	}

	return S_OK;
}

HRESULT CMenu::Build_Group()
{
	m_pGroup = CUIButton_Group::Create();
	if (nullptr == m_pGroup)
		return E_FAIL;

	if (FAILED(m_pGroup->Initialize_Linear(true)))   // wrap-around
	{
		Safe_Release(m_pGroup);
		return E_FAIL;
	}

	for (CUIButton* pBtn : m_Buttons)
	{
		if (nullptr == pBtn)
		{
			Safe_Release(m_pGroup);
			return E_FAIL;
		}
		m_pGroup->Add_Button(pBtn);  // weak
	}

	return S_OK;
}

CMenu* CMenu::Create()
{
	return new CMenu();
}

void CMenu::Free()
{
	/* m_Buttons 는 weak - 별도 해제 불필요 */
	__super::Free();   // CUIController::Free -> Safe_Release(m_pGroup, m_pGameInstance)
}