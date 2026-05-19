#pragma once
#include "UIController.h"

NS_BEGIN(Engine)
class CUIButton;
NS_END

NS_BEGIN(Game_PKM)

/* UI_BattleCommand.uiseq 전용 컨트롤러.
   - 3개 버튼(Fight / Poke / Bag) 입력 처리
   - 그룹 모드: LINEAR + WrapAround=true
   - 활성화 콜백은 베이스의 function<void(_int)> 사용. 호출 측에서 static_cast<COMMAND> 로 다룬다.
   - 본 트랙 정책: POKE / BAG 는 결정 보고만 하고 실제 처리는 Director 에서 무시 (메뉴 재오픈). */
	class CBattle_CommandMenu final : public CUIController
{
public:
	enum class COMMAND { FIGHT, POKE, BAG, END };

private:
	CBattle_CommandMenu();
	virtual ~CBattle_CommandMenu() = default;

public:
	/* m_pGroup->Get_FocusedIndex() 를 enum 으로 변환.
	   범위 밖이면 COMMAND::END 반환. */
	COMMAND Get_FocusedCommand() const;

protected:
	/* 베이스 Initialize() 가 호출하는 훅 - 직접 호출 금지 */
	virtual HRESULT Resolve_Buttons() override;
	virtual HRESULT Build_Group()     override;

private:
	/* 위젯 id -> 버튼 weak 매핑.
	   COMMAND 순서와 1:1. uiseq 의 widget id 가 바뀌면 cpp 의 s_WidgetIds[] 만 수정. */
	CUIButton* m_Buttons[ETOUI(COMMAND::END)]{ nullptr };

public:
	static CBattle_CommandMenu* Create();

private:
	virtual void Free() override;
};

NS_END