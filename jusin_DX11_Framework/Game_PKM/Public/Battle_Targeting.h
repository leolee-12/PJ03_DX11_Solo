#pragma once
#include "Battle_Context.h"
#include "Battle_Data.h"

NS_BEGIN(Game_PKM)

class CBattler;

namespace BattleTargeting
{
	inline constexpr _uint g_kMaxTargets = g_kMaxBattlers;

	struct TARGET_LIST
	{
		CBattler* aTargets[g_kMaxTargets] = {};
		_uint iCount = {};
	};

	void Resolve(const BATTLE_CONTEXT& ctx, _uint iActorSide, _uint iActorSlot,
		const MOVE_DATA& tMove, TARGET_LIST& tOutTargets);

	void Collect_AliveSide(const BATTLE_CONTEXT& ctx, _uint iSide, TARGET_LIST& tOutTargets);
}

NS_END