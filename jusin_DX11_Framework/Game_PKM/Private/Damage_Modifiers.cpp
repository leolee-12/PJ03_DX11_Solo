#include "Damage_Modifiers.h"
#include "Battler.h"
#include "PokemonData_Manager.h"
#include "Battle_Math.h"

void CStatStageModifier::Apply(const BATTLE_CONTEXT& ctx, DAMAGE_PIPE_DATA& pipe)
{
	(void)ctx;

	if (nullptr == pipe.pAttacker || nullptr == pipe.pDefender || nullptr == pipe.pMove)
		return;

	const _bool bPhysical = (MOVE_CATEGORY::PHYSICAL == pipe.pMove->eCategory);
	const STAGE_INDEX eAtkIndex = bPhysical ? STAGE_INDEX::ATK : STAGE_INDEX::SPATK;
	const STAGE_INDEX eDefIndex = bPhysical ? STAGE_INDEX::DEF : STAGE_INDEX::SPDEF;

	pipe.fAttackMul *= BattleMath::StageMul(pipe.pAttacker->Get_StatStage(eAtkIndex));
	pipe.fDefenseMul *= BattleMath::StageMul(pipe.pDefender->Get_StatStage(eDefIndex));
}

void CTypeChartModifier::Apply(const BATTLE_CONTEXT& ctx, DAMAGE_PIPE_DATA& pipe)
{
	if (nullptr == pipe.pMove || nullptr == pipe.pDefender || nullptr == ctx.pDataMgr)
		return;

	POKEMON_INSTANCE* pDefenderInst = pipe.pDefender->Get_Instance();
	if (nullptr == pDefenderInst)
		return;

	const SPECIES_DATA* pSpecies = ctx.pDataMgr->Find_Species(pDefenderInst->iSpeciesID);
	if (nullptr == pSpecies)
		return;

	const _float fType1 = ctx.pDataMgr->Get_TypeMultiplier(pipe.pMove->eType, pSpecies->eType1);
	const _float fType2 = (TYPE::NONE == pSpecies->eType2) ? 1.f :
		ctx.pDataMgr->Get_TypeMultiplier(pipe.pMove->eType, pSpecies->eType2);

	pipe.fEffectiveness *= fType1 * fType2;
}

void CStabModifier::Apply(const BATTLE_CONTEXT& ctx, DAMAGE_PIPE_DATA& pipe)
{
	if (nullptr == pipe.pAttacker || nullptr == pipe.pMove || nullptr == ctx.pDataMgr)
		return;

	POKEMON_INSTANCE* pAttackerInst = pipe.pAttacker->Get_Instance();
	if (nullptr == pAttackerInst)
		return;

	const SPECIES_DATA* pSpecies = ctx.pDataMgr->Find_Species(pAttackerInst->iSpeciesID);
	if (nullptr == pSpecies)
		return;

	if (pipe.pMove->eType == pSpecies->eType1 || pipe.pMove->eType == pSpecies->eType2)
		pipe.fStabMul *= 1.5f;
}

void CAbilityModifier::Apply(const BATTLE_CONTEXT& ctx, DAMAGE_PIPE_DATA& pipe)
{
	(void)ctx;
	(void)pipe;
}

void CItemModifier::Apply(const BATTLE_CONTEXT& ctx, DAMAGE_PIPE_DATA& pipe)
{
	(void)ctx;
	(void)pipe;
}

void CWeatherModifier::Apply(const BATTLE_CONTEXT& ctx, DAMAGE_PIPE_DATA& pipe)
{
	(void)ctx;
	(void)pipe;
}

void CFieldModifier::Apply(const BATTLE_CONTEXT& ctx, DAMAGE_PIPE_DATA& pipe)
{
	(void)ctx;
	(void)pipe;
}

void CCritModifier::Apply(const BATTLE_CONTEXT& ctx, DAMAGE_PIPE_DATA& pipe)
{
	(void)ctx;

	pipe.bCrit = ((rand() % 24) == 0);
	pipe.fCritMul = pipe.bCrit ? 1.5f : 1.f;
}

void CRandomRollModifier::Apply(const BATTLE_CONTEXT& ctx, DAMAGE_PIPE_DATA& pipe)
{
	(void)ctx;

	const _int iRoll = 85 + (rand() % 16);
	pipe.fRandomMul = static_cast<_float>(iRoll) / 100.f;

	const _float fPower = static_cast<_float>(pipe.iBasePower) * pipe.fPowerMul;
	const _float fAttack = static_cast<_float>(pipe.iAttackStat) * pipe.fAttackMul;
	const _float fDefense = static_cast<_float>(pipe.iDefenseStat) * pipe.fDefenseMul;

	if (fDefense <= 0.f)
	{
		pipe.iFinalDamage = 0;
		return;
	}

	_float fDamage = ((2.f * pipe.iAttackerLevel / 5.f + 2.f) * fPower * fAttack / fDefense) / 50.f +
		2.f;

	fDamage *= pipe.fStabMul
		* pipe.fEffectiveness
		* pipe.fAbilityMul
		* pipe.fItemMul
		* pipe.fWeatherMul
		* pipe.fFieldMul
		* pipe.fCritMul
		* pipe.fRandomMul;

	if (pipe.fEffectiveness <= 0.f)
		fDamage = 0.f;
	else if (fDamage < 1.f)
		fDamage = 1.f;

	if (fDamage > 65535.f)
		fDamage = 65535.f;

	pipe.iFinalDamage = static_cast<_ushort>(fDamage);
}