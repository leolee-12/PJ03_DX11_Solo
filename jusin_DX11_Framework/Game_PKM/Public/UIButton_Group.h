#pragma once
#include "Game_PKM_Defines.h"
#include "Base.h"
#include "UIButton.h"

NS_BEGIN(Engine)
class CGameInstance;
NS_END

NS_BEGIN(Game_PKM)

class CUIButton_Group final : public CBase
{
public:
	enum class MODE { LINEAR, GRID, END };
	enum class NAVKEY { UP, DOWN, LEFT, RIGHT, CONFIRM, CANCEL, END };

private:
	using BUTTON_STATE = CUIButton::UI_BUTTON_STATE;

private:
	CUIButton_Group();
	virtual ~CUIButton_Group() = default;

public:
	HRESULT Initialize_Linear(_bool bWrapAround = true);
	HRESULT Initialize_Grid(_uint iRows, _uint iCols, _bool bWrapAround = true);

	void Add_Button(CUIButton* pButton);
	void Clear_Buttons();
	_uint Get_ButtonCount() const { return static_cast<_uint>(m_Buttons.size()); }

	void Set_Active(_bool bActive);
	_bool Is_Active() const { return m_bActive; }

	void Set_FocusedIndex(_int iIndex);
	_int Get_FocusedIndex() const { return m_iFocusedIndex; }
	CUIButton* Get_FocusedButton() const;

	void Update(_float fTimeDelta);

	_bool Was_Activated_This_Frame() const { return m_bWasActivated; }
	_int Get_Activated_Index() const { return m_iActivatedIndex; }
	_bool Was_Cancelled_This_Frame() const { return m_bWasCancelled; }

	void Set_KeyBinding(NAVKEY eNav, _ubyte byDIK);

private:
	NAVKEY Read_Direction_Key() const;
	_int Compute_Next_Index(NAVKEY eNav) const;
	void Apply_Focus_Change(_int iNewIndex);
	void Apply_Press_Pulse(_int iIndex);

private:
	CGameInstance* m_pGameInstance{ nullptr };

	vector<CUIButton*> m_Buttons;

	MODE m_eMode{ MODE::LINEAR };
	_uint m_iRows{ 0 };
	_uint m_iCols{ 0 };
	_bool m_bWrapAround{ true };

	_int m_iFocusedIndex{ 0 };
	_int m_iActivatedIndex{ -1 };
	_bool m_bWasActivated{ false };
	_bool m_bWasCancelled{ false };

	_bool m_bActive{ true };

	_ubyte m_byKeyBindings[ETOUI(NAVKEY::END)] =
	{
			DIK_UP,
			DIK_DOWN,
			DIK_LEFT,
			DIK_RIGHT,
			DIK_RETURN,
			DIK_ESCAPE
	};

	_int m_iPressPulseIndex{ -1 };
	_float m_fPressPulseTimer{ 0.f };

public:
	static CUIButton_Group* Create();

protected:
	virtual void Free() override;
};

NS_END