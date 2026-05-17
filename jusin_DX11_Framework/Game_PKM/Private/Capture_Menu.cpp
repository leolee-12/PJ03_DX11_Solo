#include "Capture_Menu.h"
#include "UIButton_Group.h"
#include "Actor_CaptureTarget.h"
#include "PokemonData_Manager.h"

#include "UISequence.h"
#include "UIButton.h"
#include "UIText.h"
#include "UIImage.h"

CCapture_Menu::CCapture_Menu()
{
}

void CCapture_Menu::Bind(CActor_CaptureTarget* pTarget)
{
	m_pTarget = pTarget;
	Refresh_Target();
}

CCapture_Menu::MENU CCapture_Menu::Get_FocusedMENU() const
{
	if (nullptr == m_pGroup)
		return MENU::END;

	const _int iIdx = m_pGroup->Get_FocusedIndex();
	if (iIdx < 0 || iIdx >= static_cast<_int>(MENU::END))
		return MENU::END;

	return static_cast<MENU>(iIdx);
}

HRESULT CCapture_Menu::Resolve_Widgets()
{
	m_pName = Find_Widget_As<CUIText>("widget_014");
	if (nullptr == m_pName)
		return E_FAIL;

	m_pCaughtStateIcon = Find_Widget_As<CUIImage>("widget_005");
	if (nullptr == m_pCaughtStateIcon)
		return E_FAIL;

	m_pLevelDigits[0] = Find_Widget_As<CUIImage>("widget_016");
	m_pLevelDigits[1] = Find_Widget_As<CUIImage>("widget_017");
	if (nullptr == m_pLevelDigits[0] || nullptr == m_pLevelDigits[1])
		return E_FAIL;

	Refresh_Target();
	return S_OK;
}

HRESULT CCapture_Menu::Resolve_Buttons()
{
	if (nullptr == m_pSequence)
		return E_FAIL;

	/* BTN 인덱스 ↔ uiseq widget id 매핑.
	   uiseq 변경 시 이 배열만 수정. */
	static constexpr const _char* s_WidgetIds[ETOUI(MENU::END)] =
	{
					"widget_006",  // BTN_0
					"widget_007",  // BTN_1
					"widget_008",  // BTN_2
					"widget_009",  // BTN_3
	};

	for (_uint i = 0; i < ETOUI(MENU::END); ++i)
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

HRESULT CCapture_Menu::Build_Group()
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

void CCapture_Menu::On_Update(_float fTimeDelta)
{
	(void)fTimeDelta;
	Refresh_Target();
}

void CCapture_Menu::Refresh_Target()
{
	if (nullptr != m_pCaughtStateIcon)
		m_pCaughtStateIcon->Set_Visible(false);

	if (nullptr == m_pTarget)
	{
		if (nullptr != m_pName)
			m_pName->Set_Text(TEXT(""));

		if (nullptr != m_pLevelDigits[0])
			m_pLevelDigits[0]->Set_Visible(false);
		if (nullptr != m_pLevelDigits[1])
			m_pLevelDigits[1]->Set_Visible(false);

		return;
	}

	const CPokemonData_Manager* pDataMgr = CPokemonData_Manager::GetInstance();
	const SPECIES_DATA* pSpecies = (nullptr != pDataMgr) ? pDataMgr->Find_Species(m_pTarget->Get_SpeciesID()) : nullptr;

	if (nullptr != m_pName)
		m_pName->Set_Text((nullptr != pSpecies) ? pSpecies->szName : TEXT(""));

	if (nullptr != m_pCaughtStateIcon)
		m_pCaughtStateIcon->Set_Visible(m_pTarget->Is_CaughtBefore());

	_uint iLevel = m_pTarget->Get_Level();
	if (iLevel > 99)
		iLevel = 99;

	Apply_TwoDigitImage(m_pLevelDigits, iLevel, true);
}

void CCapture_Menu::Apply_TwoDigitImage(CUIImage* (&pDigits)[2], _uint iValue, _bool bHideLeadingZero)
{
	if (nullptr == pDigits[0] || nullptr == pDigits[1])
		return;

	const _uint iTens = iValue / 10;
	const _uint iOnes = iValue % 10;

	if (bHideLeadingZero && 0 == iTens)
	{
		pDigits[0]->Set_Visible(false);
	}
	else
	{
		pDigits[0]->Set_Visible(true);
		pDigits[0]->Set_Texture(pDigits[0]->Get_TextureTag(), iTens);
	}

	pDigits[1]->Set_Visible(true);
	pDigits[1]->Set_Texture(pDigits[1]->Get_TextureTag(), iOnes);
}

CCapture_Menu* CCapture_Menu::Create()
{
	return new CCapture_Menu();
}

void CCapture_Menu::Free()
{
	/* m_Buttons 는 weak — 별도 해제 불필요 */
	m_pTarget = nullptr;
	m_pName = nullptr;
	m_pCaughtStateIcon = nullptr;
	for (auto& p : m_pLevelDigits) p = nullptr;

	__super::Free();
}
