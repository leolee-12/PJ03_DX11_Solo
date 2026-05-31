#include "Entry.h"
#include "UIButton_Entry.h"
#include "Player_Status.h"
#include "PokemonData_Manager.h"

#include "UISequence.h"
#include "UIAnimator.h"
#include "UIObject.h"
#include "UIImage.h"
#include "UIText.h"
#include "UIProgressBar.h"
#include "UIButton_Group.h"

#include <tchar.h>

namespace
{
	// uiseq 위젯 id 매핑. uiseq 의 displayName/id 가 바뀌면 본 배열만 갱신.
	constexpr const _char* s_PlateIds[6] =
	{ "widget_001", "widget_002", "widget_003", "widget_004", "widget_005", "widget_006" };
	constexpr const _char* s_NumberIds[6] =
	{ "widget_007", "widget_008", "widget_009", "widget_010", "widget_011", "widget_012" };
	constexpr const _char* s_PokeIds[6] =
	{ "widget_013", "widget_014", "widget_015", "widget_016", "widget_017", "widget_018" };
	constexpr const _char* s_NameIds[6] =
	{ "widget_019", "widget_020", "widget_021", "widget_022", "widget_023", "widget_024" };
	constexpr const _char* s_HPFrameIds[6] =
	{ "widget_025", "widget_026", "widget_027", "widget_028", "widget_029", "widget_030" };
	constexpr const _char* s_HPIds[6] =
	{ "widget_031", "widget_032", "widget_033", "widget_034", "widget_035", "widget_036" };
	constexpr const _char* s_LVNumIds[6] =
	{ "widget_037", "widget_038", "widget_039", "widget_040", "widget_041", "widget_042" };

	constexpr _float kOpenInputBlockSec = 0.08f;
	constexpr _float kIntroOffsetX = 60.f;
	constexpr _float kIntroDuration = 0.25f;
}

CEntry::CEntry()
{
}

void CEntry::Bind(CPlayer_Status* pPlayerState)
{
	m_pPlayerState = pPlayerState;
	On_Refresh();
}

void CEntry::Set_Mode(ENTRY_MODE eMode)
{
	if (ENTRY_MODE::REORDER != eMode &&
		ENTRY_MODE::SELECT != eMode)
		return;

	m_eMode = eMode;
	m_iSelectedSlot = -1;
	Apply_PartyToUI();
}

void CEntry::Update(_float fTimeDelta)
{
	// 베이스 Update 는 Cancel 시 무조건 Close. Selected 흡수가 필요하여 분기를 직접 처리.
	if (false == m_bOpen)
		return;

	On_Update(fTimeDelta);

	if (nullptr == m_pGroup)
		return;

	if (m_fOpenInputBlockTimer > 0.f)
	{
		m_fOpenInputBlockTimer -= fTimeDelta;

		if (m_fOpenInputBlockTimer <= 0.f)
		{
			m_fOpenInputBlockTimer = 0.f;
			m_pGroup->Set_Active(true);
		}

		return;
	}

	if (false == m_pGroup->Is_Active())
		m_pGroup->Set_Active(true);

	m_pGroup->Update(fTimeDelta);

	if (m_pGroup->Was_Activated_This_Frame())
		Handle_SlotActivate(m_pGroup->Get_Activated_Index());

	if (m_pGroup->Was_Cancelled_This_Frame())
		Cancel_Or_Close();
}

void CEntry::Open()
{
	m_iSelectedSlot = -1;
	for (_uint i = 0; i < SLOT_COUNT; ++i)
	{
		if (m_Slots[i].pPlate)
			m_Slots[i].pPlate->Reset_SelectedVisual();
	}

	__super::Open();

	if (m_pGroup)
		m_pGroup->Set_Active(false);
	m_fOpenInputBlockTimer = kOpenInputBlockSec;

	Apply_PartyToUI();
	Play_Intro_Tween();
}

void CEntry::Open(_bool bForceReset)
{
	m_iSelectedSlot = -1;
	for (_uint i = 0; i < SLOT_COUNT; ++i)
	{
		if (m_Slots[i].pPlate)
			m_Slots[i].pPlate->Reset_SelectedVisual();
	}

	__super::Open(bForceReset);

	if (m_pGroup)
		m_pGroup->Set_Active(false);
	m_fOpenInputBlockTimer = kOpenInputBlockSec;

	Apply_PartyToUI();
	Play_Intro_Tween();
}

void CEntry::Close()
{
	m_iSelectedSlot = -1;
	for (_uint i = 0; i < SLOT_COUNT; ++i)
	{
		if (m_Slots[i].pPlate)
			m_Slots[i].pPlate->Reset_SelectedVisual();
	}
	m_fOpenInputBlockTimer = 0.f;
	
	__super::Close();
}

HRESULT CEntry::Resolve_Widgets()
{
	if (nullptr == m_pSequence)
		return E_FAIL;

	for (_uint i = 0; i < SLOT_COUNT; ++i)
	{
		m_Slots[i].pPlate = Find_Widget_As<CUIButton_Entry>(s_PlateIds[i]);
		m_Slots[i].pNumber = Find_Widget_As<CUIImage>(s_NumberIds[i]);
		m_Slots[i].pPoke = Find_Widget_As<CUIImage>(s_PokeIds[i]);
		m_Slots[i].pName = Find_Widget_As<CUIText>(s_NameIds[i]);
		m_Slots[i].pHPFrame = Find_Widget_As<CUIImage>(s_HPFrameIds[i]);
		m_Slots[i].pHP = Find_Widget_As<CUIProgressBar>(s_HPIds[i]);
		m_Slots[i].pLVNum = Find_Widget_As<CUIText>(s_LVNumIds[i]);

		if (nullptr == m_Slots[i].pPlate ||
			nullptr == m_Slots[i].pNumber ||
			nullptr == m_Slots[i].pPoke ||
			nullptr == m_Slots[i].pName ||
			nullptr == m_Slots[i].pHPFrame ||
			nullptr == m_Slots[i].pHP ||
			nullptr == m_Slots[i].pLVNum)
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CEntry::Resolve_Buttons()
{
	for (_uint i = 0; i < SLOT_COUNT; ++i)
	{
		if (nullptr == m_Slots[i].pPlate)
			return E_FAIL;

		m_Buttons[i] = m_Slots[i].pPlate;
	}

	return S_OK;
}

HRESULT CEntry::Build_Group()
{
	m_pGroup = CUIButton_Group::Create();
	if (nullptr == m_pGroup)
		return E_FAIL;

	if (FAILED(m_pGroup->Initialize_Linear(true)))
	{
		Safe_Release(m_pGroup);
		return E_FAIL;
	}

	for (_uint i = 0; i < SLOT_COUNT; ++i)
	{
		if (nullptr == m_Buttons[i])
		{
			Safe_Release(m_pGroup);
			return E_FAIL;
		}
		m_pGroup->Add_Button(m_Buttons[i]);
	}

	return S_OK;
}

void CEntry::On_Refresh()
{
	Apply_PartyToUI();
}

void CEntry::On_Update(_float /*fTimeDelta*/)
{
	Apply_PartyToUI();
}

void CEntry::Apply_PartyToUI()
{
	if (nullptr == m_pPlayerState)
	{
		for (_uint i = 0; i < SLOT_COUNT; ++i)
			Show_Slot(i, false);
		return;
	}

	const PARTY& tParty = m_pPlayerState->Get_Party();

	for (_uint i = 0; i < SLOT_COUNT; ++i)
	{
		const _bool bActive = (i < tParty.iCount);
		Show_Slot(i, bActive);

		if (bActive)
			Apply_Slot(i, tParty.arrSlots[i]);

		Apply_Selected_State(i, bActive && (static_cast<_int>(i) == m_iSelectedSlot));
	}
}

void CEntry::Show_Slot(_uint i, _bool bShow)
{
	SLOT& s = m_Slots[i];
	if (s.pPlate)   s.pPlate->Set_Visible(bShow);
	if (s.pNumber)  s.pNumber->Set_Visible(bShow);
	if (s.pPoke)    s.pPoke->Set_Visible(bShow);
	if (s.pName)    s.pName->Set_Visible(bShow);
	if (s.pHPFrame) s.pHPFrame->Set_Visible(bShow);
	if (s.pHP)      s.pHP->Set_Visible(bShow);
	if (s.pLVNum)   s.pLVNum->Set_Visible(bShow);
}

void CEntry::Apply_Slot(_uint i, const POKEMON_INSTANCE& tInst)
{
	SLOT& s = m_Slots[i];

	if (s.pPoke)
		s.pPoke->Set_Texture(s.pPoke->Get_TextureTag(), tInst.iSpeciesID);

	if (s.pName)
	{
		const SPECIES_DATA* pSpecies =
			CPokemonData_Manager::GetInstance()->Find_Species(tInst.iSpeciesID);
		if (pSpecies)
			s.pName->Set_Text(_wstring(pSpecies->szName));
	}

	if (s.pLVNum)
	{
		const _uint iLevel = (tInst.iLevel > 99u) ? 99u : tInst.iLevel;
		_tchar szBuf[16] = {};
		_stprintf_s(szBuf, TEXT("Lv. %02u"), iLevel);
		s.pLVNum->Set_Text(_wstring(szBuf));
	}

	if (s.pHP)
	{
		const _ushort iMaxHP = tInst.iStat[static_cast<size_t>(STAT::HP)];
		const _float fHP01 = (iMaxHP > 0)
			? static_cast<_float>(tInst.iCurrentHP) / static_cast<_float>(iMaxHP)
			: 0.f;
		s.pHP->Set_FillAmount(fHP01);
	}
}

void CEntry::Apply_Selected_State(_uint i, _bool bSelected)
{
	if (m_Slots[i].pPlate)
		m_Slots[i].pPlate->Set_Selected(bSelected);
}

void CEntry::Handle_SlotActivate(_int iFocusedIndex)
{
	if (nullptr == m_pPlayerState)
		return;

	if (iFocusedIndex < 0 || iFocusedIndex >= static_cast<_int>(SLOT_COUNT))
		return;

	PARTY& tParty = m_pPlayerState->Get_Party();

	if (static_cast<_uint>(iFocusedIndex) >= tParty.iCount)
		return;

	if (ENTRY_MODE::SELECT == m_eMode)
	{
		if (m_fnOnActivate)
			m_fnOnActivate(iFocusedIndex);

		return;
	}

	if (m_iSelectedSlot < 0)
	{
		m_iSelectedSlot = iFocusedIndex;
	}
	else if (m_iSelectedSlot == iFocusedIndex)
	{
		m_iSelectedSlot = -1;
	}
	else
	{
		PartyOps::Swap(tParty,
			static_cast<_uint>(m_iSelectedSlot),
			static_cast<_uint>(iFocusedIndex));
		m_iSelectedSlot = -1;
	}

	Apply_PartyToUI();
}

void CEntry::Cancel_Or_Close()
{
	if (m_iSelectedSlot >= 0)
	{
		m_iSelectedSlot = -1;
		Apply_PartyToUI();
		return;
	}

	if (m_fnOnCancel)
		m_fnOnCancel();
	Close();
}

void CEntry::Play_Intro_Tween()
{
	if (nullptr == m_pSequence)
		return;

	CUIAnimator* pAnimator = m_pSequence->Get_Animator();
	if (nullptr == pAnimator)
		return;

	if (0 != m_iIntroTweenHandle)
	{
		pAnimator->Stop_Tween(m_iIntroTweenHandle);
		m_iIntroTweenHandle = 0;
	}

	CUITween::UITWEEN_DESC tDesc{};
	tDesc.eTarget = UI_TWEEN_TARGET::ANCHOR_OFFSET_X;
	tDesc.fStart = kIntroOffsetX;
	tDesc.fEnd = 0.f;
	tDesc.fDuration = kIntroDuration;
	tDesc.fDelay = 0.f;
	tDesc.eEase = UI_EASE::EASE_OUT_CUBIC;
	tDesc.eLoop = UI_TWEEN_LOOP::NONE;

	m_iIntroTweenHandle = pAnimator->Play_Tween(tDesc);
}

CEntry* CEntry::Create()
{
	return new CEntry();
}

void CEntry::Free()
{
	m_pPlayerState = nullptr;  // weak
	for (_uint i = 0; i < SLOT_COUNT; ++i)
	{
		m_Slots[i] = {};
		m_Buttons[i] = nullptr;
	}

	__super::Free();
}