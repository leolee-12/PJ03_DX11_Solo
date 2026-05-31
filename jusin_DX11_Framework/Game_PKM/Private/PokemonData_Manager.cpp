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

const ITEM_DATA* CPokemonData_Manager::Find_Item(_uint iItemID) const
{
	auto iter = m_ItemTable.find(iItemID);

	if (iter == m_ItemTable.end())
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
		wcscpy_s(tData.szName, L"이상해씨");
		tData.eType1 = TYPE::GRASS;
		tData.eType2 = TYPE::POISON;
		tData.iBaseHP = 45; tData.iBaseAtk = 49; tData.iBaseDef = 49;
		tData.iBaseSpAtk = 65; tData.iBaseSpDef = 65; tData.iBaseSpd = 45;
		tData.iAbility1 = 1;
		tData.iLearnset[0] = 33;
		tData.iLearnset[1] = 22;
		tData.strModelTag = PROTO_COM_MODEL_PM0001_00;
		tData.pRenderMappingPath = "../../Resources/Models/pkm/pm0001_00/pm0001_00_mapping.json";
		tData.eRenderRuleKey = RENDER_RULE_KEY::POKEMON_DEFAULT;
		m_SpeciesTable.emplace(tData.iDexNo, tData);
	}

	{
		SPECIES_DATA tData{};
		tData.iDexNo = 4;
		wcscpy_s(tData.szName, L"파이리");
		tData.eType1 = TYPE::FIRE;
		tData.eType2 = TYPE::NONE;
		tData.iBaseHP = 39; tData.iBaseAtk = 52; tData.iBaseDef = 43;
		tData.iBaseSpAtk = 60; tData.iBaseSpDef = 50; tData.iBaseSpd = 65;
		tData.iAbility1 = 2;
		tData.iLearnset[0] = 10;
		tData.iLearnset[1] = 52;
		tData.strModelTag = PROTO_COM_MODEL_PM0004_00;
		tData.pRenderMappingPath = "../../Resources/Models/pkm/pm0004_00/pm0004_00_mapping.json";
		tData.eRenderRuleKey = RENDER_RULE_KEY::POKEMON_DEFAULT;
		m_SpeciesTable.emplace(tData.iDexNo, tData);
	}

	{
		SPECIES_DATA tData{};
		tData.iDexNo = 7;
		wcscpy_s(tData.szName, L"꼬부기");
		tData.eType1 = TYPE::WATER;
		tData.eType2 = TYPE::NONE;
		tData.iBaseHP = 44; tData.iBaseAtk = 48; tData.iBaseDef = 65;
		tData.iBaseSpAtk = 50; tData.iBaseSpDef = 64; tData.iBaseSpd = 43;
		tData.iAbility1 = 3;
		tData.iLearnset[0] = 33;
		tData.iLearnset[1] = 55;
		tData.strModelTag = PROTO_COM_MODEL_PM0007_00;
		tData.pRenderMappingPath = "../../Resources/Models/pkm/pm0007_00/pm0007_00_mapping.json";
		tData.eRenderRuleKey = RENDER_RULE_KEY::POKEMON_DEFAULT;
		m_SpeciesTable.emplace(tData.iDexNo, tData);
	}

	{
		SPECIES_DATA tData{};
		tData.iDexNo = 25;
		wcscpy_s(tData.szName, L"피카츄");
		tData.eType1 = TYPE::ELECTRIC;
		tData.eType2 = TYPE::NONE;
		tData.iBaseHP = 35; tData.iBaseAtk = 55; tData.iBaseDef = 40;
		tData.iBaseSpAtk = 50; tData.iBaseSpDef = 50; tData.iBaseSpd = 90;
		tData.iAbility1 = 9;
		tData.iLearnset[0] = 84;
		tData.iLearnset[1] = 201;
		tData.iLearnset[2] = 202;
		tData.iLearnset[3] = 0;
		tData.strModelTag = PROTO_COM_MODEL_PM0025_00;
		tData.pRenderMappingPath = "../../Resources/Models/pkm/pm0025_00/pm0025_00_mapping.json";
		tData.eRenderRuleKey = RENDER_RULE_KEY::POKEMON_DEFAULT;
		m_SpeciesTable.emplace(tData.iDexNo, tData);
	}

	auto AddSpecies = [this](_uint iDexNo, const _tchar* pName,
		TYPE eType1, TYPE eType2,
		_ushort iHP, _ushort iAtk, _ushort iDef, _ushort iSpAtk, _ushort iSpDef, _ushort iSpd,
		_uint iAbility1, _uint iAbility2, _uint iHiddenAbility,
		_uint iMove0, _uint iMove1, _uint iMove2, _uint iMove3,
		WNameID strModelTag, const _char* pMappingPath)
		{
			SPECIES_DATA tData{};
			tData.iDexNo = iDexNo;
			wcscpy_s(tData.szName, pName);
			tData.eType1 = eType1;
			tData.eType2 = eType2;
			tData.iBaseHP = iHP; tData.iBaseAtk = iAtk; tData.iBaseDef = iDef;
			tData.iBaseSpAtk = iSpAtk; tData.iBaseSpDef = iSpDef; tData.iBaseSpd = iSpd;
			tData.iAbility1 = iAbility1;
			tData.iAbility2 = iAbility2;
			tData.iHiddenAbility = iHiddenAbility;
			tData.iLearnset[0] = iMove0;
			tData.iLearnset[1] = iMove1;
			tData.iLearnset[2] = iMove2;
			tData.iLearnset[3] = iMove3;
			tData.strModelTag = strModelTag;
			tData.pRenderMappingPath = pMappingPath;
			tData.eRenderRuleKey = RENDER_RULE_KEY::POKEMON_DEFAULT;
			m_SpeciesTable.emplace(tData.iDexNo, tData);
		};

	AddSpecies(10, L"캐터피", TYPE::BUG, TYPE::NONE, 45, 30, 35, 20, 20, 45, 19, 0, 50,
		33, 450, 0, 0, PROTO_COM_MODEL_PM0010_00, "../../Resources/Models/pkm/pm0010_00/pm0010_00_mapping.json");

	AddSpecies(41, L"주뱃", TYPE::POISON, TYPE::FLYING, 40, 45, 35, 30, 40, 55, 39, 0, 151,
		141, 44, 310, 0, PROTO_COM_MODEL_PM0041_00, "../../Resources/Models/pkm/pm0041_00/pm0041_00_mapping.json");

	AddSpecies(43, L"뚜벅쵸", TYPE::GRASS, TYPE::POISON, 45, 50, 55, 75, 65, 30, 34, 0, 50,
		71, 22, 0, 0, PROTO_COM_MODEL_PM0043_00, "../../Resources/Models/pkm/pm0043_00/pm0043_00_mapping.json");

	AddSpecies(59, L"윈디", TYPE::FIRE, TYPE::NONE, 90, 110, 80, 100, 80, 95, 22, 18, 154,
		52, 44, 0, 0, PROTO_COM_MODEL_PM0059_00, "../../Resources/Models/pkm/pm0059_00/pm0059_00_mapping.json");

	AddSpecies(74, L"꼬마돌", TYPE::ROCK, TYPE::GROUND, 40, 80, 100, 30, 30, 20, 69, 5, 8,
		88, 0, 0, 0, PROTO_COM_MODEL_PM0074_00, "../../Resources/Models/pkm/pm0074_00/pm0074_00_mapping.json");

	AddSpecies(95, L"롱스톤", TYPE::ROCK, TYPE::GROUND, 35, 45, 160, 30, 45, 70, 69, 5, 133,
		88, 0, 0, 0, PROTO_COM_MODEL_PM0095_00, "../../Resources/Models/pkm/pm0095_00/pm0095_00_mapping.json");

	AddSpecies(121, L"아쿠스타", TYPE::WATER, TYPE::PSYCHIC, 60, 75, 85, 100, 85, 115, 35, 30, 148,
		55, 0, 0, 0, PROTO_COM_MODEL_PM0121_00, "../../Resources/Models/pkm/pm0121_00/pm0121_00_mapping.json");

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

	AddMove(10, L"할퀴기", TYPE::NORMAL, MOVE_CATEGORY::PHYSICAL, 40, 100, 35);
	AddMove(22, L"덩쿨채찍", TYPE::GRASS, MOVE_CATEGORY::PHYSICAL, 45, 100, 25);
	AddMove(33, L"몸통박치기", TYPE::NORMAL, MOVE_CATEGORY::PHYSICAL, 40, 100, 35);
	AddMove(52, L"불꽃세례", TYPE::FIRE, MOVE_CATEGORY::SPECIAL, 40, 100, 25);
	AddMove(55, L"몰대포", TYPE::WATER, MOVE_CATEGORY::SPECIAL, 40, 100, 25);
	AddMove(84, L"전기쇼크", TYPE::ELECTRIC, MOVE_CATEGORY::SPECIAL, 55, 100, 30);
	AddMove(201, L"참방참방서핑", TYPE::WATER, MOVE_CATEGORY::SPECIAL, 80, 100, 15);
	AddMove(202, L"둥실둥실폴", TYPE::FLYING, MOVE_CATEGORY::PHYSICAL, 90, 95, 15);
	AddMove(203, L"파치파치액셀", TYPE::ELECTRIC, MOVE_CATEGORY::PHYSICAL, 500, 100, 15);
	AddMove(44, L"물기", TYPE::DARK, MOVE_CATEGORY::PHYSICAL, 60, 100, 25);
	AddMove(71, L"흡수", TYPE::GRASS, MOVE_CATEGORY::SPECIAL, 20, 100, 25);
	AddMove(83, L"회오리불꽃", TYPE::FIRE, MOVE_CATEGORY::SPECIAL, 35, 85, 15);
	AddMove(88, L"돌떨구기", TYPE::ROCK, MOVE_CATEGORY::PHYSICAL, 30, 90, 15);
	AddMove(93, L"염동력", TYPE::PSYCHIC, MOVE_CATEGORY::SPECIAL, 50, 100, 25);
	AddMove(98, L"전광석화", TYPE::NORMAL, MOVE_CATEGORY::PHYSICAL, 40, 100, 30, 1);
	AddMove(127, L"폭포오르기", TYPE::WATER, MOVE_CATEGORY::PHYSICAL, 80, 100, 15);
	AddMove(141, L"흡혈", TYPE::BUG, MOVE_CATEGORY::PHYSICAL, 80, 100, 10);
	AddMove(157, L"스톤샤워", TYPE::ROCK, MOVE_CATEGORY::PHYSICAL, 75, 90, 10);
	AddMove(242, L"깨물어부수기", TYPE::DARK, MOVE_CATEGORY::PHYSICAL, 80, 100, 15);
	AddMove(246, L"원시의힘", TYPE::ROCK, MOVE_CATEGORY::SPECIAL, 60, 100, 5);
	AddMove(310, L"놀래키기", TYPE::GHOST, MOVE_CATEGORY::PHYSICAL, 30, 100, 15);
	AddMove(450, L"벌레먹음", TYPE::BUG, MOVE_CATEGORY::PHYSICAL, 60, 100, 20);

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
	AddAbility(5, L"Sturdy");
	AddAbility(8, L"Sand Veil");
	AddAbility(18, L"Flash Fire");
	AddAbility(19, L"Shield Dust");
	AddAbility(22, L"Intimidate");
	AddAbility(30, L"Natural Cure");
	AddAbility(34, L"Chlorophyll");
	AddAbility(35, L"Illuminate");
	AddAbility(39, L"Inner Focus");
	AddAbility(50, L"Run Away");
	AddAbility(55, L"Hustle");
	AddAbility(62, L"Guts");
	AddAbility(69, L"Rock Head");
	AddAbility(133, L"Weak Armor");
	AddAbility(148, L"Analytic");
	AddAbility(151, L"Infiltrator");
	AddAbility(153, L"Moxie");
	AddAbility(154, L"Justified");

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

HRESULT CPokemonData_Manager::Validate_Seed() const
{
	return E_NOTIMPL;
}

void CPokemonData_Manager::Free()
{
	m_SpeciesTable.clear();
	m_MoveTable.clear();
	m_AbilityTable.clear();
	m_ItemTable.clear();

	m_bInitialized = false;

	__super::Free();
}