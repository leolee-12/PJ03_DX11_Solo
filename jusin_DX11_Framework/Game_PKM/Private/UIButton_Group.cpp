#include "UIButton_Group.h"
#include "GameInstance.h"

CUIButton_Group::CUIButton_Group()
	: m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CUIButton_Group::Initialize_Linear(_bool bWrapAround)
{
	m_eMode = MODE::LINEAR;
	m_iRows = 0;
	m_iCols = 0;
	m_bWrapAround = bWrapAround;

	return S_OK;
}

HRESULT CUIButton_Group::Initialize_Grid(_uint iRows, _uint iCols, _bool bWrapAround)
{
	if (0 == iRows || 0 == iCols)
		return E_INVALIDARG;

	m_eMode = MODE::GRID;
	m_iRows = iRows;
	m_iCols = iCols;
	m_bWrapAround = bWrapAround;

	return S_OK;
}

void CUIButton_Group::Add_Button(CUIButton* pButton)
{
	if (nullptr == pButton)
		return;

	m_Buttons.push_back(pButton);

	if (1 == m_Buttons.size())
	{
		m_iFocusedIndex = 0;

		if (m_bActive)
			pButton->Set_State(BUTTON_STATE::HOVER);
	}
	else
	{
		pButton->Set_State(BUTTON_STATE::NORMAL);
	}
}

void CUIButton_Group::Clear_Buttons()
{
	for (auto* pButton : m_Buttons)
	{
		if (nullptr != pButton)
			pButton->Set_State(BUTTON_STATE::NORMAL);
	}

	m_Buttons.clear();
	m_iFocusedIndex = 0;
	m_iActivatedIndex = -1;
	m_bWasActivated = false;
	m_bWasCancelled = false;
	m_iPressPulseIndex = -1;
	m_fPressPulseTimer = 0.f;
}

void CUIButton_Group::Set_Active(_bool bActive)
{
	m_bActive = bActive;
}

void CUIButton_Group::Set_FocusedIndex(_int iIndex)
{
	if (iIndex < 0 || iIndex >= static_cast<_int>(m_Buttons.size()))
		return;

	Apply_Focus_Change(iIndex);
}

CUIButton* CUIButton_Group::Get_FocusedButton() const
{
	if (m_iFocusedIndex < 0 || m_iFocusedIndex >= static_cast<_int>(m_Buttons.size()))
		return nullptr;

	return m_Buttons[m_iFocusedIndex];
}

void CUIButton_Group::Update(_float fTimeDelta)
{
	m_bWasActivated = false;
	m_bWasCancelled = false;
	m_iActivatedIndex = -1;

	if (m_iPressPulseIndex >= 0)
	{
		m_fPressPulseTimer -= fTimeDelta;

		if (m_fPressPulseTimer <= 0.f)
		{
			if (m_iPressPulseIndex < static_cast<_int>(m_Buttons.size()) &&
				nullptr != m_Buttons[m_iPressPulseIndex])
			{
				m_Buttons[m_iPressPulseIndex]->Set_State(BUTTON_STATE::HOVER);
			}

			m_iPressPulseIndex = -1;
			m_fPressPulseTimer = 0.f;
		}
	}

	if (false == m_bActive)
		return;

	if (m_Buttons.empty())
		return;

	if (nullptr == m_pGameInstance)
		return;

	NAVKEY eNav = Read_Direction_Key();

	if (NAVKEY::END != eNav)
	{
		_int iNext = Compute_Next_Index(eNav);

		if (iNext != m_iFocusedIndex)
			Apply_Focus_Change(iNext);
	}

	if (m_pGameInstance->Key_Down(m_byKeyBindings[ETOUI(NAVKEY::CONFIRM)]))
	{
		m_iActivatedIndex = m_iFocusedIndex;
		m_bWasActivated = true;
		Apply_Press_Pulse(m_iFocusedIndex);
	}
	else if (m_pGameInstance->Key_Down(m_byKeyBindings[ETOUI(NAVKEY::CANCEL)]))
	{
		m_bWasCancelled = true;
	}
}

CUIButton_Group::NAVKEY CUIButton_Group::Read_Direction_Key() const
{
	if (nullptr == m_pGameInstance)
		return NAVKEY::END;

	if (m_pGameInstance->Key_Down(m_byKeyBindings[ETOUI(NAVKEY::UP)]))
		return NAVKEY::UP;

	if (m_pGameInstance->Key_Down(m_byKeyBindings[ETOUI(NAVKEY::DOWN)]))
		return NAVKEY::DOWN;

	if (m_pGameInstance->Key_Down(m_byKeyBindings[ETOUI(NAVKEY::LEFT)]))
		return NAVKEY::LEFT;

	if (m_pGameInstance->Key_Down(m_byKeyBindings[ETOUI(NAVKEY::RIGHT)]))
		return NAVKEY::RIGHT;

	return NAVKEY::END;
}

_int CUIButton_Group::Compute_Next_Index(NAVKEY eNav) const
{
	const _int iSize = static_cast<_int>(m_Buttons.size());

	if (0 == iSize)
		return m_iFocusedIndex;

	if (MODE::LINEAR == m_eMode)
	{
		_int iDelta = 0;

		if (NAVKEY::UP == eNav || NAVKEY::LEFT == eNav)
			iDelta = -1;
		else if (NAVKEY::DOWN == eNav || NAVKEY::RIGHT == eNav)
			iDelta = 1;

		_int iNext = m_iFocusedIndex + iDelta;

		if (m_bWrapAround)
			iNext = (iNext % iSize + iSize) % iSize;
		else
			iNext = clamp(iNext, 0, iSize - 1);

		return iNext;
	}

	if (0 == m_iRows || 0 == m_iCols)
		return m_iFocusedIndex;

	const _int iRows = static_cast<_int>(m_iRows);
	const _int iCols = static_cast<_int>(m_iCols);

	_int iRow = m_iFocusedIndex / iCols;
	_int iCol = m_iFocusedIndex % iCols;

	switch (eNav)
	{
	case NAVKEY::UP:
		--iRow;
		break;

	case NAVKEY::DOWN:
		++iRow;
		break;

	case NAVKEY::LEFT:
		--iCol;
		break;

	case NAVKEY::RIGHT:
		++iCol;
		break;

	default:
		break;
	}

	if (m_bWrapAround)
	{
		iRow = (iRow % iRows + iRows) % iRows;
		iCol = (iCol % iCols + iCols) % iCols;
	}
	else
	{
		iRow = clamp(iRow, 0, iRows - 1);
		iCol = clamp(iCol, 0, iCols - 1);
	}

	_int iNext = iRow * iCols + iCol;

	if (iNext >= iSize)
		iNext = iSize - 1;

	return iNext;
}

void CUIButton_Group::Apply_Focus_Change(_int iNewIndex)
{
	if (iNewIndex < 0 || iNewIndex >= static_cast<_int>(m_Buttons.size()))
		return;

	if (m_iFocusedIndex >= 0 && m_iFocusedIndex < static_cast<_int>(m_Buttons.size()))
	{
		if (nullptr != m_Buttons[m_iFocusedIndex])
			m_Buttons[m_iFocusedIndex]->Set_State(BUTTON_STATE::NORMAL);
	}

	m_iFocusedIndex = iNewIndex;

	if (nullptr != m_Buttons[m_iFocusedIndex])
		m_Buttons[m_iFocusedIndex]->Set_State(BUTTON_STATE::HOVER);
}

void CUIButton_Group::Apply_Press_Pulse(_int iIndex)
{
	if (iIndex < 0 || iIndex >= static_cast<_int>(m_Buttons.size()))
		return;

	if (nullptr != m_Buttons[iIndex])
		m_Buttons[iIndex]->Set_State(BUTTON_STATE::PRESSED);

	m_iPressPulseIndex = iIndex;
	m_fPressPulseTimer = 0.1f;
}

void CUIButton_Group::Set_KeyBinding(NAVKEY eNav, _ubyte byDIK)
{
	if (NAVKEY::END == eNav)
		return;

	m_byKeyBindings[ETOUI(eNav)] = byDIK;
}

CUIButton_Group* CUIButton_Group::Create()
{
	return new CUIButton_Group();
}

void CUIButton_Group::Free()
{
	m_Buttons.clear();

	Safe_Release(m_pGameInstance);

	__super::Free();
}