#pragma once
#include "UIController.h"
#include "Battle_Data.h"

NS_BEGIN(Engine)
class CUIImage;
class CUIText;
class CUIProgressBar;
NS_END

NS_BEGIN(Game_PKM)

class CPlayer_Status;
class CUIButton_Entry;

class CEntry final : public CUIController
{
private:
	CEntry();
	virtual ~CEntry() = default;

public:
	void Bind(CPlayer_Status* pPlayerState);
	_int Get_SelectedSlot() const { return m_iSelectedSlot; }

	virtual void Update(_float fTimeDelta) override;
	virtual void Open() override;
	virtual void Open(_bool bForceReset) override;
	virtual void Close() override;

protected:
	virtual HRESULT Resolve_Widgets() override;
	virtual HRESULT Resolve_Buttons() override;
	virtual HRESULT Build_Group() override;

	virtual void On_Refresh() override;
	virtual void On_Update(_float fTimeDelta) override;

private:
	struct SLOT
	{
		CUIButton_Entry* pPlate{ nullptr };
		CUIImage* pNumber{ nullptr };
		CUIImage* pPoke{ nullptr };
		CUIText* pName{ nullptr };
		CUIImage* pHPFrame{ nullptr };
		CUIProgressBar* pHP{ nullptr };
		CUIText* pLVNum{ nullptr };
	};

	static constexpr _uint SLOT_COUNT = g_kMaxPartySize;

	SLOT             m_Slots[SLOT_COUNT]{};
	CUIButton_Entry* m_Buttons[SLOT_COUNT]{ nullptr };

	CPlayer_Status* m_pPlayerState{ nullptr };

	_int m_iSelectedSlot{ -1 };
	_int m_iIntroTweenHandle{ 0 };
	_float m_fOpenInputBlockTimer{ 0.f };

private:
	void Apply_PartyToUI();
	void Show_Slot(_uint i, _bool bShow);
	void Apply_Slot(_uint i, const POKEMON_INSTANCE& tInst);
	void Apply_Selected_State(_uint i, _bool bSelected);

	void Toggle_Or_Swap(_int iFocusedIndex);
	void Cancel_Or_Close();

	void Play_Intro_Tween();

public:
	static CEntry* Create();

protected:
	virtual void Free() override;
};

NS_END