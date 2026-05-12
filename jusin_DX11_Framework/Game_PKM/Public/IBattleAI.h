#pragma once
#include "Base.h"
#include "Battle_Context.h"

NS_BEGIN(Game_PKM)

class IBattleCommand;

class IBattleAI abstract : public CBase
{
protected:
	IBattleAI() = default;
	virtual ~IBattleAI() = default;

public:
	virtual IBattleCommand* Decide(const BATTLE_CONTEXT& ctx, _uint iSide) PURE;
};

NS_END