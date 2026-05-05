#pragma once
#include "Base.h"
#include "Game_PKM_Defines.h"
#include "Battle_Data.h"

NS_BEGIN(Game_PKM)

class CPokemonData_Manager : public CBase
{
	DECLARE_SINGLETON(CPokemonData_Manager)

private:
	CPokemonData_Manager();
	virtual ~CPokemonData_Manager() = default;

public:
	HRESULT Initialize();

public:
	const SPECIES_DATA* Find_Species(_uint iDexNo) const;
	const MOVE_DATA* Find_Move(_uint iMoveID) const;
	const ABILITY_DATA* Find_Ability(_uint iAbilityID) const;
	const BATTLE_RULE_DESC* Find_BattleRule(BATTLE_RULE eRule) const;

	_float Get_TypeMultiplier(TYPE eAttack, TYPE eDefend) const;

private:
	HRESULT Load_BuiltinSeed();

private:
	std::unordered_map<_uint, SPECIES_DATA> m_SpeciesTable;
	std::unordered_map<_uint, MOVE_DATA> m_MoveTable;
	std::unordered_map<_uint, ABILITY_DATA> m_AbilityTable;

	BATTLE_RULE_DESC m_BattleRules[static_cast<size_t>(BATTLE_RULE::END)] = {};

	_bool m_bInitialized = { false };

private:
	virtual void Free() override;
};

NS_END