#pragma once
#include "UIController.h"

NS_BEGIN(Engine)
class CUIButton;
class CUIImage;
class CUIText;
NS_END

NS_BEGIN(Game_PKM)
class CActor_CaptureTarget;

/* UI_Get.uiseq 전용 컨트롤러.
   - 4 버튼 입력 처리: MENU::READY(준비한다) / BAG(가방) / HELP(도움말) / RUN(도망간다)
   - 그룹 모드: LINEAR + WrapAround=true
   - 비-버튼 위젯(info / info2 / Line / Line2 / Icon) 의 데이터 바인딩은 본 단위 범위 외. 컨트롤러는 4 버튼만 다룬다.
   - Activate / Cancel 콜백은 베이스의 function 사용. 호출 측에서 인덱스를 MENU 로 다룸. */

class CCapture_Menu final : public CUIController
{
public:
	enum class MENU { READY, BAG, HELP, RUN, END };

private:
	CCapture_Menu();
	virtual ~CCapture_Menu() = default;

public:
	MENU Get_FocusedMENU() const;
	void Bind(CActor_CaptureTarget* pTarget);
	virtual void Open() override;
	virtual void Open(_bool bForceReset) override;

protected:
	/* 베이스 Initialize() 가 호출하는 훅 - 직접 호출 금지 */
	virtual HRESULT Resolve_Widgets() override;
	virtual HRESULT Resolve_Buttons() override;
	virtual HRESULT Build_Group()     override;
	virtual void    On_Update(_float fTimeDelta) override;

private:
	/* 위젯 id -> 버튼 weak 매핑. BTN 순서와 1:1. uiseq 의 widget id 가 바뀌면 cpp 의 s_WidgetIds[] 만 수정. */
	CUIButton* m_Buttons[ETOUI(MENU::END)]{ nullptr };
	CActor_CaptureTarget* m_pTarget = { nullptr };
	CUIText* m_pName = { nullptr };
	CUIImage* m_pCaughtStateIcon = { nullptr };
	CUIImage* m_pLevelDigits[2] = { nullptr, nullptr };

private:
	void Refresh_Target();
	void Apply_TwoDigitImage(CUIImage* (&pDigits)[2], _uint iValue, _bool bHideLeadingZero);
	void Restore_VisibleWidgets();

public:
	static CCapture_Menu* Create();

private:
	virtual void Free() override;
};

NS_END
