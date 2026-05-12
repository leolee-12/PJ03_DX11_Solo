#include "Battle_Math.h"
#include "Battler.h"
#include "Battle_Manager.h"

_float BattleMath::StageMul(_byte iStage)
{
	int iClamped = static_cast<int>(iStage);

	if (iClamped > 6)
		iClamped = 6;

	if (iClamped < -6)
		iClamped = -6;

	if (iClamped >= 0)
		return static_cast<_float>(2 + iClamped) / 2.f;

	return 2.f / static_cast<_float>(2 - iClamped);
}

_bool BattleMath::Roll_Accuracy(_ubyte iAccuracy, CBattler* pAtk, CBattler* pDef)
{
	if (0 == iAccuracy)
		return true;

	_byte iAccStage = 0;
	_byte iEvaStage = 0;

	if (nullptr != pAtk)
		iAccStage = pAtk->Get_StatStage(STAGE_INDEX::ACC);

	if (nullptr != pDef)
		iEvaStage = pDef->Get_StatStage(STAGE_INDEX::EVA);

	const _float fStageMul = StageMul(iAccStage) / StageMul(iEvaStage);
	const _uint iFinalAccuracy = static_cast<_uint>(static_cast<_float>(iAccuracy) * fStageMul);

	if (iFinalAccuracy >= 100)
		return true;

	return static_cast<_uint>(rand() % 100) < iFinalAccuracy;
}

_bool BattleMath::Roll_EffectChance(_ubyte iChance)
{
	if (0 == iChance)
		return false;

	if (iChance >= 100)
		return true;

	return static_cast<_uint>(rand() % 100) < iChance;
}

_bool BattleMath::Roll_RunSuccess(const BATTLE_CONTEXT & ctx, _uint iSide)
{
	(void)iSide;

	if (nullptr == ctx.pManager)
		return false;

	if (false == ctx.pManager->Get_Env().bCanRun)
		return false;

	return true;
}

_ushort BattleMath::Pick_AttackStat(POKEMON_INSTANCE& tInst, MOVE_CATEGORY eCat)
{
	switch (eCat)
	{
	case MOVE_CATEGORY::PHYSICAL:
		return tInst.iStat[static_cast<size_t>(STAT::ATK)];

	case MOVE_CATEGORY::SPECIAL:
		return tInst.iStat[static_cast<size_t>(STAT::SPATK)];

	default:
		return 0;
	}
}

_ushort BattleMath::Pick_DefenseStat(POKEMON_INSTANCE& tInst, MOVE_CATEGORY eCat)
{
	switch (eCat)
	{
	case MOVE_CATEGORY::PHYSICAL:
		return tInst.iStat[static_cast<size_t>(STAT::DEF)];

	case MOVE_CATEGORY::SPECIAL:
		return tInst.iStat[static_cast<size_t>(STAT::SPDEF)];

	default:
		return 0;
	}
}

void BattleMath::Apply_MoveEffect(const BATTLE_CONTEXT& ctx, const MOVE_DATA& tMove,
	CBattler* pAtk, CBattler* pDef)
{
	(void)ctx;
	(void)tMove;
	(void)pAtk;
	(void)pDef;
}