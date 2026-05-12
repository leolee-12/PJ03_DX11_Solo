#pragma once
#include "Game_PKM_Defines.h"
#include "Battle_Data.h"
#include "Battle_Session.h"

NS_BEGIN(Game_PKM)

class CBattler;

enum class DAMAGE_SOURCE : _ubyte
{
	MOVE,
	POISON,
	BURN,
	SANDSTORM,
	HAIL,
	LEECH_SEED,
	RECOIL,
	CONFUSION_SELF,
	OTHER,
	END
};

struct DAMAGE_PIPE_DATA
{
	CBattler* pAttacker = { nullptr };
	CBattler* pDefender = { nullptr };
	const MOVE_DATA* pMove = { nullptr };
	const FIELD_STATE* pField = { nullptr };

	_ushort iBasePower = { 0 };
	_ushort iAttackStat = { 0 };
	_ushort iDefenseStat = { 0 };
	_ubyte iAttackerLevel = { 1 };

	_float fPowerMul = { 1.f };
	_float fAttackMul = { 1.f };
	_float fDefenseMul = { 1.f };
	_float fEffectiveness = { 1.f };
	_float fStabMul = { 1.f };
	_float fAbilityMul = { 1.f };
	_float fItemMul = { 1.f };
	_float fWeatherMul = { 1.f };
	_float fFieldMul = { 1.f };
	_float fCritMul = { 1.f };
	_bool bCrit = { false };
	_float fRandomMul = { 1.f };

	_ushort iFinalDamage = { 0 };
};

NS_END