#pragma once
#include "Battle_Context.h"
#include "Battle_Data.h"

NS_BEGIN(Game_PKM)

class CBattler;

namespace BattleMath
{
	_float StageMul(_byte iStage);

	_bool Roll_Accuracy(_ubyte iAccuracy, CBattler* pAtk, CBattler* pDef);
	_bool Roll_EffectChance(_ubyte iChance);
	_bool Roll_RunSuccess(const BATTLE_CONTEXT& ctx, _uint iSide);

	_ushort Pick_AttackStat(POKEMON_INSTANCE& tInst, MOVE_CATEGORY eCat);
	_ushort Pick_DefenseStat(POKEMON_INSTANCE& tInst, MOVE_CATEGORY eCat);

	void Apply_MoveEffect(const BATTLE_CONTEXT& ctx, const MOVE_DATA& tMove,
		CBattler* pAtk, CBattler* pDef);
}

NS_END