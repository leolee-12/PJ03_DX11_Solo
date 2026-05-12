#include "Battle_CommandMenu.h"
#include "UISequence.h"
#include "UIButton.h"
#include "UIButton_Group.h"

CBattle_CommandMenu::CBattle_CommandMenu()
{
}

CBattle_CommandMenu::COMMAND CBattle_CommandMenu::Get_FocusedCommand() const
{
	if (nullptr == m_pGroup)
		return COMMAND::END;

	const _int iIdx = m_pGroup->Get_FocusedIndex();
	if (iIdx < 0 || iIdx >= static_cast<_int>(COMMAND::END))
		return COMMAND::END;

	return static_cast<COMMAND>(iIdx);
}

HRESULT CBattle_CommandMenu::Resolve_Buttons()
{
	if (nullptr == m_pSequence)
		return E_FAIL;

	/* COMMAND 인덱스 ↔ uiseq widget id 매핑.
	   uiseq 변경 시 이 배열만 수정. */
	static constexpr const _char* s_WidgetIds[ETOUI(COMMAND::END)] =
	{
			"widget_001",  // FIGHT
			"widget_002",  // POKE
			"widget_003",  // BAG
	};

	for (_uint i = 0; i < ETOUI(COMMAND::END); ++i)
	{
		CUIObject* pObj = m_pSequence->Find_Widget(s_WidgetIds[i]);
		if (nullptr == pObj)
			return E_FAIL;

		CUIButton* pBtn = dynamic_cast<CUIButton*>(pObj);
		if (nullptr == pBtn)
			return E_FAIL;

		m_Buttons[i] = pBtn;  // weak — sequence(children) 가 소유
	}

	return S_OK;
}

HRESULT CBattle_CommandMenu::Build_Group()
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

CBattle_CommandMenu* CBattle_CommandMenu::Create()
{
	return new CBattle_CommandMenu();
}

void CBattle_CommandMenu::Free()
{
	/* m_Buttons 는 weak — 별도 해제 불필요 */
	__super::Free();
}