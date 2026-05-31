#pragma once
#include "Base.h"
#include "Battle_Context.h"

NS_BEGIN(Game_PKM)

/* IBattleAction_Step
   - 시퀀서(CBattle_ActionSequencer)가 순차 실행하는 한 단계.
   - 각 step 은 자체 상태(타이머 등)를 보유하고 Is_Complete 가 true 가 될 때까지
	 매 프레임 Update 호출을 받는다.
   - 완료 판정은 시간 / 페이싱 락 / 외부 신호 등 step 종류별 자유. */
	class IBattleAction_Step abstract : public CBase
{
protected:
	IBattleAction_Step() = default;
	virtual ~IBattleAction_Step() = default;

public:
	virtual void  OnEnter(const BATTLE_CONTEXT& ctx) PURE;
	virtual void  Update(const BATTLE_CONTEXT& ctx, _float fTimeDelta) PURE;
	virtual _bool Is_Complete(const BATTLE_CONTEXT& ctx) const PURE;

protected:
	virtual void Free() override
	{
		__super::Free();
	}
};

NS_END