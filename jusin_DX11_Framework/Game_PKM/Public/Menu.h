#pragma once
#include "UIController.h"

NS_BEGIN(Engine)
class CUIButton;
NS_END

NS_BEGIN(Game_PKM)

/* UI_Menu.uiseq 전용 컨트롤러.
   - 6개 버튼(Partner / Dex / Bag / Entry / Link / Report) 입력 처리
   - 그룹 모드: LINEAR + WrapAround=true
   - 활성화 콜백은 베이스의 function<void(_int)> 사용. 호출 측에서 static_cast<MENU_ENTRY> 로 다룬다.
 */
    class CMenu final : public CUIController
{
public:
    enum class MENU_ENTRY { PARTNER, DEX, BAG, ENTRY, LINK, REPORT, END };

private:
    CMenu();
    virtual ~CMenu() = default;

public:
    /* m_pGroup->Get_FocusedIndex() 를 enum 으로 변환.
       범위 밖이면 MENU_ENTRY::END 반환. */
    MENU_ENTRY Get_FocusedEntry() const;

protected:
    /* 베이스 Initialize() 가 호출하는 훅 - 직접 호출 금지 */
    virtual HRESULT Resolve_Buttons() override;
    virtual HRESULT Build_Group()     override;

private:
    /* 위젯 id -> 버튼 weak 매핑.
       MENU_ENTRY 순서와 1:1. uiseq 의 widget id 가 바뀌면 cpp 의 s_WidgetIds[] 만 수정. */
    CUIButton* m_Buttons[ETOUI(MENU_ENTRY::END)]{ nullptr };

public:
    static CMenu* Create();

private:
    virtual void Free() override;
};

NS_END