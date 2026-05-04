#pragma once
#include "Base.h"
#include "UIButton_Group.h"

NS_BEGIN(Engine)
class CGameInstance;
class CUISequence;
NS_END

NS_BEGIN(Game_PKM)

/* UI 컨트롤러 추상 베이스 */
// - CUISequence weak 보관 (레벨이 소유)
// - CUIButton_Group owned (Free 시 Safe_Release)
// - 입력·포커스·활성화 처리는 Group 에 위임, 디스패치만 담당
// - 위젯 id → 버튼 매핑은 서브클래스 책임 (Resolve_Buttons / Build_Group)

	class CUIController : public CBase
{
public:
	using ACTIVATE_CALLBACK = function<void(_int)>;
	using CANCEL_CALLBACK = function<void()>;

	enum class FOCUS_POLICY { RESET_ON_OPEN, REMEMBER_LAST, END };

protected:
	CUIController();
	virtual ~CUIController() = default;

public:
	virtual HRESULT Initialize(CUISequence* pSequence);
	virtual void    Update(_float fTimeDelta);
	virtual void    Open();
	virtual void    Open(_bool bForceReset);
	virtual void    Close();

	_bool   Is_Open() const { return m_bOpen; }

	void    Set_FocusPolicy(FOCUS_POLICY ePolicy);
	void    Set_OnActivate(ACTIVATE_CALLBACK fn);
	void    Set_OnCancel(CANCEL_CALLBACK   fn);
	void    Set_KeyBinding(CUIButton_Group::NAVKEY eNav, _ubyte byDIK);

	CUIButton_Group* Get_Group() const { return m_pGroup; }
	CUISequence* Get_Sequence() const { return m_pSequence; }

protected:
	/* 서브클래스 책임 — 순서:
	   Initialize() 가 Resolve_Buttons() → Build_Group() 순으로 호출.
	   Resolve_Buttons() 에서 sequence->Find_Widget 으로 CUIButton* 모으고,
	   Build_Group() 에서 Group::Create + Initialize_Linear/Grid + Add_Button x N */
	virtual HRESULT Resolve_Buttons() PURE;
	virtual HRESULT Build_Group() PURE;

protected:
	CGameInstance* m_pGameInstance{ nullptr };  // AddRef
	CUISequence* m_pSequence{ nullptr };  // weak (level owns)
	CUIButton_Group* m_pGroup{ nullptr };  // owned

	_bool             m_bOpen{ false };
	FOCUS_POLICY      m_eFocusPolicy{ FOCUS_POLICY::RESET_ON_OPEN };
	_int              m_iLastFocusedIndex{ 0 };

	ACTIVATE_CALLBACK m_fnOnActivate{};
	CANCEL_CALLBACK   m_fnOnCancel{};

protected:
	virtual void Free() override;
};

NS_END