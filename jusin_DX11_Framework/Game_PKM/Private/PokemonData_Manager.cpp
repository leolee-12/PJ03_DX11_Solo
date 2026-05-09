#include "PokemonData_Manager.h"

IMPLEMENT_SINGLETON(CPokemonData_Manager)

CPokemonData_Manager::CPokemonData_Manager()
{
}

HRESULT CPokemonData_Manager::Initialize()
{
	if (m_bInitialized)
		return S_OK;

	if (FAILED(Load_BuiltinSeed()))
		return E_FAIL;

	m_bInitialized = true;

	return S_OK;
}

const SPECIES_DATA* CPokemonData_Manager::Find_Species(_uint iDexNo) const
{
	auto iter = m_SpeciesTable.find(iDexNo);

	if (iter == m_SpeciesTable.end())
		return nullptr;

	return &iter->second;
}

const MOVE_DATA* CPokemonData_Manager::Find_Move(_uint iMoveID) const
{
	auto iter = m_MoveTable.find(iMoveID);

	if (iter == m_MoveTable.end())
		return nullptr;

	return &iter->second;
}

const ABILITY_DATA* CPokemonData_Manager::Find_Ability(_uint iAbilityID) const
{
	auto iter = m_AbilityTable.find(iAbilityID);

	if (iter == m_AbilityTable.end())
		return nullptr;

	return &iter->second;
}

const BATTLE_RULE_DESC* CPokemonData_Manager::Find_BattleRule(BATTLE_RULE eRule) const
{
	const size_t iIndex = static_cast<size_t>(eRule);

	if (iIndex >= static_cast<size_t>(BATTLE_RULE::END))
		return nullptr;

	return &m_BattleRules[iIndex];
}

_float CPokemonData_Manager::Get_TypeMultiplier(TYPE eAttack, TYPE eDefend) const
{
	const _uint iAttack = static_cast<_uint>(eAttack);
	const _uint iDefend = static_cast<_uint>(eDefend);

	if (iAttack >= g_kTypeCount || iDefend >= g_kTypeCount)
		return 1.f;

	return g_TypeChart[iAttack][iDefend];
}

HRESULT CPokemonData_Manager::Load_BuiltinSeed()
{
	{
		SPECIES_DATA tData{};
		tData.iDexNo = 1;
		wcscpy_s(tData.szName, L"ÀÌ»óÇØ¾¾");
		tData.eType1 = TYPE::GRASS;
		tData.eType2 = TYPE::POISON;
		tData.iBaseHP = 45; tData.iBaseAtk = 49; tData.iBaseDef = 49;
		tData.iBaseSpAtk = 65; tData.iBaseSpDef = 65; tData.iBaseSpd = 45;
		tData.iAbility1 = 1;
		tData.iLearnset[0] = 33;
		tData.iLearnset[1] = 22;
		tData.strModelTag = PROTO_COM_MODEL_PM0001_00;
		m_SpeciesTable.emplace(tData.iDexNo, tData);
	}

	{
		SPECIES_DATA tData{};
		tData.iDexNo = 4;
		wcscpy_s(tData.szName, L"ÆÄÀÌ¸®");
		tData.eType1 = TYPE::FIRE;
		tData.eType2 = TYPE::NONE;
		tData.iBaseHP = 39; tData.iBaseAtk = 52; tData.iBaseDef = 43;
		tData.iBaseSpAtk = 60; tData.iBaseSpDef = 50; tData.iBaseSpd = 65;
		tData.iAbility1 = 2;
		tData.iLearnset[0] = 10;
		tData.iLearnset[1] = 52;
		tData.strModelTag = PROTO_COM_MODEL_PM0004_00;
		m_SpeciesTable.emplace(tData.iDexNo, tData);
	}

	{
		SPECIES_DATA tData{};
		tData.iDexNo = 7;
		wcscpy_s(tData.szName, L"²¿ºÎ±â");
		tData.eType1 = TYPE::WATER;
		tData.eType2 = TYPE::NONE;
		tData.iBaseHP = 44; tData.iBaseAtk = 48; tData.iBaseDef = 65;
		tData.iBaseSpAtk = 50; tData.iBaseSpDef = 64; tData.iBaseSpd = 43;
		tData.iAbility1 = 3;
		tData.iLearnset[0] = 33;
		tData.iLearnset[1] = 55;
		tData.strModelTag = PROTO_COM_MODEL_PM0007_00;
		m_SpeciesTable.emplace(tData.iDexNo, tData);
	}

	{
		SPECIES_DATA tData{};
		tData.iDexNo = 25;
		wcscpy_s(tData.szName, L"ÇÇÄ«Ãò");
		tData.eType1 = TYPE::ELECTRIC;
		tData.eType2 = TYPE::NONE;
		tData.iBaseHP = 35; tData.iBaseAtk = 55; tData.iBaseDef = 40;
		tData.iBaseSpAtk = 50; tData.iBaseSpDef = 50; tData.iBaseSpd = 90;
		tData.iAbility1 = 9;
		tData.iLearnset[0] = 84;
		tData.iLearnset[1] = 33;
		tData.strModelTag = PROTO_COM_MODEL_PM0025_00;
		m_SpeciesTable.emplace(tData.iDexNo, tData);
	}

	auto AddMove = [this](_uint iID, const _tchar* pName, TYPE eType, MOVE_CATEGORY eCategory,
		_ushort iPower, _ubyte iAccuracy, _ubyte iMaxPP, _byte iPriority = 0)
		{
			MOVE_DATA tData{};
			tData.iMoveID = iID;
			wcscpy_s(tData.szName, pName);
			tData.eType = eType;
			tData.eCategory = eCategory;
			tData.iPower = iPower;
			tData.iAccuracy = iAccuracy;
			tData.iMaxPP = iMaxPP;
			tData.iPriority = iPriority;
			m_MoveTable.emplace(iID, tData);
		};

	AddMove(10, L"Scratch", TYPE::NORMAL, MOVE_CATEGORY::PHYSICAL, 40, 100, 35);
	AddMove(22, L"Vine Whip", TYPE::GRASS, MOVE_CATEGORY::PHYSICAL, 45, 100, 25);
	AddMove(33, L"Tackle", TYPE::NORMAL, MOVE_CATEGORY::PHYSICAL, 40, 100, 35);
	AddMove(52, L"Ember", TYPE::FIRE, MOVE_CATEGORY::SPECIAL, 40, 100, 25);
	AddMove(55, L"Water Gun", TYPE::WATER, MOVE_CATEGORY::SPECIAL, 40, 100, 25);
	AddMove(84, L"Thundershock", TYPE::ELECTRIC, MOVE_CATEGORY::SPECIAL, 40, 100, 30);

	auto AddAbility = [this](_uint iID, const _tchar* pName, _uint iEffectID = 0)
		{
			ABILITY_DATA tData{};
			tData.iAbilityID = iID;
			wcscpy_s(tData.szName, pName);
			tData.iEffectID = iEffectID;
			m_AbilityTable.emplace(iID, tData);
		};

	AddAbility(1, L"Overgrow");
	AddAbility(2, L"Blaze");
	AddAbility(3, L"Torrent");
	AddAbility(9, L"Static");

	auto& tWild = m_BattleRules[static_cast<size_t>(BATTLE_RULE::WILD_SINGLE)];
	tWild.eRule = BATTLE_RULE::WILD_SINGLE;
	tWild.bCanRun = true;
	tWild.bCanCapture = true;
	tWild.bCanUseItem = true;
	tWild.bExpGain = true;
	tWild.bMoneyGain = false;
	tWild.bPlayerControlled = true;
	tWild.bShowTutorialUI = false;
	tWild.iAILevel = 0;

	auto& tTrainerSingle = m_BattleRules[static_cast<size_t>(BATTLE_RULE::TRAINER_SINGLE)];
	tTrainerSingle.eRule = BATTLE_RULE::TRAINER_SINGLE;
	tTrainerSingle.bCanRun = false;
	tTrainerSingle.bCanCapture = false;
	tTrainerSingle.bCanUseItem = true;
	tTrainerSingle.bExpGain = true;
	tTrainerSingle.bMoneyGain = true;
	tTrainerSingle.bPlayerControlled = true;
	tTrainerSingle.bShowTutorialUI = false;
	tTrainerSingle.iAILevel = 2;

	auto& tTrainerDouble = m_BattleRules[static_cast<size_t>(BATTLE_RULE::TRAINER_DOUBLE)];
	tTrainerDouble = tTrainerSingle;
	tTrainerDouble.eRule = BATTLE_RULE::TRAINER_DOUBLE;

	auto& tCutscene = m_BattleRules[static_cast<size_t>(BATTLE_RULE::CUTSCENE)];
	tCutscene.eRule = BATTLE_RULE::CUTSCENE;
	tCutscene.bCanRun = false;
	tCutscene.bCanCapture = false;
	tCutscene.bCanUseItem = false;
	tCutscene.bExpGain = false;
	tCutscene.bMoneyGain = false;
	tCutscene.bPlayerControlled = false;
	tCutscene.bShowTutorialUI = false;
	tCutscene.iAILevel = 1;

	auto& tTutorial = m_BattleRules[static_cast<size_t>(BATTLE_RULE::TUTORIAL)];
	tTutorial.eRule = BATTLE_RULE::TUTORIAL;
	tTutorial.bCanRun = false;
	tTutorial.bCanCapture = false;
	tTutorial.bCanUseItem = false;
	tTutorial.bExpGain = false;
	tTutorial.bMoneyGain = false;
	tTutorial.bPlayerControlled = true;
	tTutorial.bShowTutorialUI = true;
	tTutorial.iAILevel = 0;

	return S_OK;
}

void CPokemonData_Manager::Free()
{
	__super::Free();

	m_SpeciesTable.clear();
	m_MoveTable.clear();
	m_AbilityTable.clear();

	m_bInitialized = false;
}