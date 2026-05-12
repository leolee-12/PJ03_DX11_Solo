#include "Battle_MoveMenu.h"
#include "Battle_Manager.h"
#include "Battler.h"
#include "PokemonData_Manager.h"
#include "UISequence.h"
#include "UIButton.h"
#include "UIText.h"
#include "UIImage.h"
#include "UIButton_Group.h"

CBattle_MoveMenu::CBattle_MoveMenu()
{
}

void CBattle_MoveMenu::Bind(CBattle_Manager* pManager)
{
	m_pManager = pManager;
}

CBattle_MoveMenu::SLOT CBattle_MoveMenu::Get_FocusedSlot() const
{
	if (nullptr == m_pGroup)
		return SLOT::END;

	const _int iIdx = m_pGroup->Get_FocusedIndex();
	if (iIdx < 0 || iIdx >= static_cast<_int>(SLOT::END))
		return SLOT::END;

	return static_cast<SLOT>(iIdx);
}

HRESULT CBattle_MoveMenu::Resolve_Widgets()
{
	if (nullptr == m_pSequence)
		return E_FAIL;

	static constexpr const _char* s_NameIds[ETOUI(SLOT::END)] =
	{
			"widget_005", "widget_006", "widget_007", "widget_008"
	};
	static constexpr const _char* s_PPIds[ETOUI(SLOT::END)] =
	{
			"widget_009", "widget_010", "widget_011", "widget_012"
	};
	static constexpr const _char* s_IconIds[ETOUI(SLOT::END)] =
	{
			"widget_013", "widget_014", "widget_015", "widget_016"
	};

	for (_uint i = 0; i < ETOUI(SLOT::END); ++i)
	{
		m_Names[i] = Find_Widget_As<CUIText>(s_NameIds[i]);
		m_PPs[i] = Find_Widget_As<CUIText>(s_PPIds[i]);
		m_Icons[i] = Find_Widget_As<CUIImage>(s_IconIds[i]);

		if (nullptr == m_Names[i] || nullptr == m_PPs[i] || nullptr == m_Icons[i])
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CBattle_MoveMenu::Resolve_Buttons()
{
	if (nullptr == m_pSequence)
		return E_FAIL;

	static constexpr const _char* s_ButtonIds[ETOUI(SLOT::END)] =
	{
			"widget_001", "widget_002", "widget_003", "widget_004"
	};

	for (_uint i = 0; i < ETOUI(SLOT::END); ++i)
	{
		CUIObject* pObj = m_pSequence->Find_Widget(s_ButtonIds[i]);
		if (nullptr == pObj)
			return E_FAIL;

		CUIButton* pBtn = dynamic_cast<CUIButton*>(pObj);
		if (nullptr == pBtn)
			return E_FAIL;

		m_Buttons[i] = pBtn;
	}

	return S_OK;
}

HRESULT CBattle_MoveMenu::Build_Group()
{
	m_pGroup = CUIButton_Group::Create();
	if (nullptr == m_pGroup)
		return E_FAIL;

	if (FAILED(m_pGroup->Initialize_Linear(true)))
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
		m_pGroup->Add_Button(pBtn);
	}

	return S_OK;
}

void CBattle_MoveMenu::On_Refresh()
{
	Refresh_Slots();
}

void CBattle_MoveMenu::On_Update(_float fTimeDelta)
{
	(void)fTimeDelta;
	Refresh_Slots();
}

void CBattle_MoveMenu::Refresh_Slots()
{
	auto Hide_Slot = [](CUIButton* pBtn, CUIText* pName, CUIText* pPP, CUIImage* pIcon)
		{
			if (nullptr != pBtn)  pBtn->Set_Visible(false);
			if (nullptr != pName) pName->Set_Text(TEXT(""));
			if (nullptr != pPP)   pPP->Set_Text(TEXT(""));
			if (nullptr != pIcon) pIcon->Set_Visible(false);
		};

	if (nullptr == m_pManager)
	{
		for (_uint i = 0; i < ETOUI(SLOT::END); ++i)
			Hide_Slot(m_Buttons[i], m_Names[i], m_PPs[i], m_Icons[i]);
		return;
	}

	CBattler* pPlayer = m_pManager->Get_Battler(g_kBattleSide_Player);
	if (nullptr == pPlayer || nullptr == pPlayer->Get_Instance())
	{
		for (_uint i = 0; i < ETOUI(SLOT::END); ++i)
			Hide_Slot(m_Buttons[i], m_Names[i], m_PPs[i], m_Icons[i]);
		return;
	}

	auto* pDataMgr = CPokemonData_Manager::GetInstance();

	for (_uint i = 0; i < ETOUI(SLOT::END); ++i)
	{
		const _uint iMoveID = pPlayer->Get_MoveID(i);
		const MOVE_DATA* pMove = (0 != iMoveID && nullptr != pDataMgr) ? pDataMgr->Find_Move(iMoveID) : nullptr;

		if (nullptr == pMove)
		{
			Hide_Slot(m_Buttons[i], m_Names[i], m_PPs[i], m_Icons[i]);
			continue;
		}

		if (nullptr != m_Buttons[i])
			m_Buttons[i]->Set_Visible(true);

		if (nullptr != m_Names[i])
			m_Names[i]->Set_Text(pMove->szName);

		if (nullptr != m_PPs[i])
		{
			const _ubyte iCur = pPlayer->Get_PP(i);
			const _ubyte iMax = pMove->iMaxPP;

			wchar_t szBuf[32] = {};
			swprintf_s(szBuf, L"PP %d/%d", static_cast<int>(iCur), static_cast<int>(iMax));
			m_PPs[i]->Set_Text(szBuf);
		}

		if (nullptr != m_Icons[i])
		{
			m_Icons[i]->Set_Visible(true);
			m_Icons[i]->Set_Texture(m_Icons[i]->Get_TextureTag(), static_cast<_uint>(pMove->eType));
		}
	}
}

CBattle_MoveMenu* CBattle_MoveMenu::Create()
{
	return new CBattle_MoveMenu();
}

void CBattle_MoveMenu::Free()
{
	m_pManager = nullptr;

	for (auto& p : m_Buttons) p = nullptr;
	for (auto& p : m_Names)   p = nullptr;
	for (auto& p : m_PPs)     p = nullptr;
	for (auto& p : m_Icons)   p = nullptr;

	__super::Free();
}