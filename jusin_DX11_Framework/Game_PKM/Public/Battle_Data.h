#ifndef Battle_Data_h__
#define Battle_Data_h__
#include "Game_PKM_Defines.h"

NS_BEGIN(Game_PKM)

inline constexpr _uint		g_kMaxLearnSet = 128;
inline constexpr _uint		g_kTypeCount = 21;
inline constexpr _uint		g_kMaxPartySize = 6;
inline constexpr _uint		g_kMaxMovesPerPokemon = 4;
inline constexpr _ubyte		g_kMaxIV = 31;
inline constexpr _ubyte		g_kMaxEVPerStat = 252;
inline constexpr _ushort	g_kMaxEVTotal = 510;
inline constexpr _ubyte		g_kMaxLevel = 100;
inline constexpr _ubyte		g_kMinLevel = 1;

inline constexpr _float g_TypeChart[g_kTypeCount][g_kTypeCount] =
{
	/*           NONE NOR  FIG  FLY  POI  GND  ROK  BUG  GHO  FIR  WAT  GRS  ELE  PSY  ICE  DRG  STL  DRK  FAI  STA  QUE */
	/* NONE  */{ 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f },
	/* NOR   */{ 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, .5f, 1.f, 0.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, .5f, 1.f, 1.f, 1.f, 1.f },
	/* FIG   */{ 1.f, 2.f, 1.f, .5f, .5f, 1.f, 2.f, .5f, 0.f, 1.f, 1.f, 1.f, 1.f, .5f, 2.f, 1.f, 2.f, 2.f, .5f, 1.f, 1.f },
	/* FLY   */{ 1.f, 1.f, 2.f, 1.f, 1.f, 1.f, .5f, 2.f, 1.f, 1.f, 1.f, 2.f, .5f, 1.f, 1.f, 1.f, .5f, 1.f, 1.f, 1.f, 1.f },
	/* POI   */{ 1.f, 1.f, 1.f, 1.f, .5f, .5f, .5f, 1.f, .5f, 1.f, 1.f, 2.f, 1.f, 1.f, 1.f, 1.f, 0.f, 1.f, 2.f, 1.f, 1.f },
	/* GND   */{ 1.f, 1.f, 1.f, 0.f, 2.f, 1.f, 2.f, .5f, 1.f, 2.f, 1.f, .5f, 2.f, 1.f, 1.f, 1.f, 2.f, 1.f, 1.f, 1.f, 1.f },
	/* ROK   */{ 1.f, 1.f, .5f, 2.f, 1.f, .5f, 1.f, 2.f, 1.f, 2.f, 1.f, 1.f, 1.f, 1.f, 2.f, 1.f, .5f, 1.f, 1.f, 1.f, 1.f },
	/* BUG   */{ 1.f, 1.f, .5f, .5f, .5f, 1.f, 1.f, 1.f, .5f, .5f, 1.f, 2.f, 1.f, 2.f, 1.f, 1.f, .5f, 2.f, .5f, 1.f, 1.f },
	/* GHO   */{ 1.f, 0.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 2.f, 1.f, 1.f, 1.f, 1.f, 2.f, 1.f, 1.f, 1.f, .5f, 1.f, 1.f, 1.f },
	/* FIR   */{ 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, .5f, 2.f, 1.f, .5f, .5f, 2.f, 1.f, 1.f, 2.f, .5f, 2.f, 1.f, 1.f, 1.f, 1.f },
	/* WAT   */{ 1.f, 1.f, 1.f, 1.f, 1.f, 2.f, 2.f, 1.f, 1.f, 2.f, .5f, .5f, 1.f, 1.f, 1.f, .5f, 1.f, 1.f, 1.f, 1.f, 1.f },
	/* GRS   */{ 1.f, 1.f, 1.f, .5f, .5f, 2.f, 2.f, .5f, 1.f, .5f, 2.f, .5f, 1.f, 1.f, 1.f, .5f, .5f, 1.f, 1.f, 1.f, 1.f },
	/* ELE   */{ 1.f, 1.f, 1.f, 2.f, 1.f, 0.f, 1.f, 1.f, 1.f, 1.f, 2.f, .5f, .5f, 1.f, 1.f, .5f, 1.f, 1.f, 1.f, 1.f, 1.f },
	/* PSY   */{ 1.f, 1.f, 2.f, 1.f, 2.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, .5f, 1.f, 1.f, .5f, 0.f, 1.f, 1.f, 1.f },
	/* ICE   */{ 1.f, 1.f, 1.f, 2.f, 1.f, 2.f, 1.f, 1.f, 1.f, .5f, .5f, 2.f, 1.f, 1.f, .5f, 2.f, .5f, 1.f, 1.f, 1.f, 1.f },
	/* DRG   */{ 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 2.f, .5f, 1.f, 0.f, 1.f, 1.f },
	/* STL   */{ 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 2.f, 1.f, 1.f, .5f, .5f, 1.f, .5f, 1.f, 2.f, 1.f, .5f, 1.f, 2.f, 1.f, 1.f },
	/* DRK   */{ 1.f, 1.f, .5f, 1.f, 1.f, 1.f, 1.f, 1.f, 2.f, 1.f, 1.f, 1.f, 1.f, 2.f, 1.f, 1.f, 1.f, .5f, .5f, 1.f, 1.f },
	/* FAI   */{ 1.f, 1.f, 2.f, 1.f, .5f, 1.f, 1.f, 1.f, 1.f, .5f, 1.f, 1.f, 1.f, 1.f, 1.f, 2.f, .5f, 2.f, 1.f, 1.f, 1.f },
	/* STA   */{ 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f },
	/* QUE   */{ 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f },
};

struct SPECIES_DATA
{
	_uint   iDexNo = {};
	_tchar  szName[32] = {};
	_tchar	szForm[32] = {};

	TYPE    eType1 = { TYPE::NONE };
	TYPE    eType2 = { TYPE::NONE };

	_ushort iBaseHP = {};
	_ushort iBaseAtk = {};
	_ushort iBaseDef = {};
	_ushort iBaseSpAtk = {};
	_ushort iBaseSpDef = {};
	_ushort iBaseSpd = {};

	_uint   iAbility1 = {};
	_uint   iAbility2 = {};
	_uint   iHiddenAbility = {};

	_uint   iLearnset[g_kMaxLearnSet] = {};
	WNameID strModelTag = {};
};

struct MOVE_DATA
{
	_uint			iMoveID = {};
	_tchar			szName[32] = {};

	TYPE			eType = { TYPE::NONE };
	MOVE_CATEGORY	eCategory = { MOVE_CATEGORY::END };

	_ushort			iPower = {};
	_ubyte			iAccuracy = {};
	_ubyte			iMaxPP = {};
	_byte			iPriority = {};

	_uint			iEffectID = {};
	_ubyte			iEffectChance = {};
	_uint			iFlags = {};
};

struct ABILITY_DATA
{
	_uint	iAbilityID = {};
	_tchar	szName[32] = {};
	_uint	iEffectID = {};
};

struct BATTLE_RULE_DESC
{
	BATTLE_RULE	eRule = { BATTLE_RULE::END };

	_bool		bCanRun = {};
	_bool		bCanCapture = {};
	_bool		bCanUseItem = {};
	_bool		bExpGain = {};
	_bool		bMoneyGain = {};
	_bool		bPlayerControlled = {};
	_bool		bShowTutorialUI = {};

	_uint		iAILevel = {};
};

struct POKEMON_INSTANCE
{
	_uint   iSpeciesID = {};
	_tchar  szNickname[16] = {};

	_ubyte  iLevel = {};
	_uint   iExp = {};

	_ubyte  iIV[static_cast<size_t>(STAT::END)] = {};
	_ubyte  iEV[static_cast<size_t>(STAT::END)] = {};

	NATURE  eNature = { NATURE::END };
	_uint   iAbilityID = {};

	_uint   iMoves[g_kMaxMovesPerPokemon] = {};
	_ubyte  iCurrentPP[g_kMaxMovesPerPokemon] = {};

	_ushort	iCurrentHP = {};
	STATUS_CONDITION eStatus = { STATUS_CONDITION::NONE };
	_uint	iHeldItemID = {};

	_ushort iStat[static_cast<size_t>(STAT::END)] = {};

	_uint   iOriginalTrainerID = {};
	_uint   iCapturedAtZoneID = {};
};

struct PARTY
{
	POKEMON_INSTANCE arrSlots[g_kMaxPartySize] = {};
	_ubyte iCount = {};
};

struct TRAINER_DATA
{
	_uint iTrainerID = {};
	_tchar szName[32] = {};

	TRAINER_AI eAIType = { TRAINER_AI::END };
	PARTY tParty = {};

	_uint iRewardMoney = {};
	_uint iRewardItemID = {};

	_uint iEncounterDialogID = {};
	_uint iVictoryDialogID = {};
	_uint iDefeatDialogID = {};
};

_ushort	Calc_HP_Stat(_ushort iBase, _ubyte iIV, _ubyte iEV, _ubyte iLevel);
_ushort	Calc_Other_Stat(_ushort iBase, _ubyte iIV, _ubyte iEV, _ubyte iLevel, _float fNatureMul);
_float	Get_NatureMultiplier(NATURE eNature, STAT eStat);
void	Recalc_All_Stats(POKEMON_INSTANCE& tInstance, const SPECIES_DATA& tSpecies);

namespace PartyOps
{
	void Clear(PARTY& tParty);
	_bool Add(PARTY& tParty, const POKEMON_INSTANCE& tInstance);
	POKEMON_INSTANCE* Get(PARTY& tParty, _uint iSlot);
	const POKEMON_INSTANCE* Get(const PARTY& tParty, _uint iSlot);
	_uint Find_First_Alive(const PARTY& tParty);
}

NS_END

#endif // Battle_Data_h__