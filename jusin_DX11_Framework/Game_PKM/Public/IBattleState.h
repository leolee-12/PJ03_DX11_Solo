#pragma once
#include "Base.h"
#include "Battle_Context.h"

NS_BEGIN(Game_PKM)

class IBattleState abstract : public CBase
{
protected:
	IBattleState() = default;
	virtual ~IBattleState() = default;

public:
	virtual BATTLE_PHASE Get_Phase() const PURE;

	virtual void OnEnter(const BATTLE_CONTEXT& ctx) PURE;
	virtual void Update(const BATTLE_CONTEXT& ctx, _float fTimeDelta) PURE;
	virtual void OnExit(const BATTLE_CONTEXT& ctx) PURE;

protected:
	virtual void Free() override
	{
		__super::Free();
	}
};

NS_END