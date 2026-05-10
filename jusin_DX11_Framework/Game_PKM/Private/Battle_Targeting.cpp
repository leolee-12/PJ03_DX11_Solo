#include "Battle_Targeting.h"
#include "Battler.h"

void BattleTargeting::Resolve(const BATTLE_CONTEXT& ctx, _uint iActorSide, _uint iActorSlot,
	const MOVE_DATA& tMove, TARGET_LIST& tOutTargets)
{
	(void)iActorSlot;
	(void)tMove;

	tOutTargets = {};

	CBattler* pTarget = ctx.Get_Foe(iActorSide);
	if (nullptr == pTarget)
		return;

	if (false == pTarget->Is_Alive())
		return;

	tOutTargets.aTargets[tOutTargets.iCount++] = pTarget;
}

void BattleTargeting::Collect_AliveSide(const BATTLE_CONTEXT& ctx, _uint iSide, TARGET_LIST&
	tOutTargets)
{
	tOutTargets = {};

	CBattler* pBattler = ctx.Get_Self(iSide);
	if (nullptr == pBattler)
		return;

	if (false == pBattler->Is_Alive())
		return;

	tOutTargets.aTargets[tOutTargets.iCount++] = pBattler;
}