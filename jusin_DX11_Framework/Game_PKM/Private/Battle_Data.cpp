#include "Battle_Data.h"
#include "PokemonData_Manager.h"

NS_BEGIN(Game_PKM)

_ushort Calc_HP_Stat(_ushort iBase, _ubyte iIV, _ubyte iEV, _ubyte iLevel)
{
	const _uint iCore = (2u * iBase + iIV + (iEV / 4u)) * iLevel / 100u;
	return static_cast<_ushort>(iCore + iLevel + 10u);
}

_ushort Calc_Other_Stat(_ushort iBase, _ubyte iIV, _ubyte iEV, _ubyte iLevel, _float fNatureMul)
{
	const _uint iCore = (2u * iBase + iIV + (iEV / 4u)) * iLevel / 100u + 5u;
	return static_cast<_ushort>(static_cast<_uint>(static_cast<_float>(iCore) * fNatureMul));
}

_float Get_NatureMultiplier(NATURE eNature, STAT eStat)
{
	if (STAT::HP == eStat)
		return 1.f;

	static const _float s_kNatureTable[static_cast<size_t>(NATURE::END)][5] =
	{
			{ 1.0f, 1.0f, 1.0f, 1.0f, 1.0f }, // HARDY
			{ 1.1f, 0.9f, 1.0f, 1.0f, 1.0f }, // LONELY
			{ 1.1f, 1.0f, 1.0f, 1.0f, 0.9f }, // BRAVE
			{ 1.1f, 1.0f, 0.9f, 1.0f, 1.0f }, // ADAMANT
			{ 1.1f, 1.0f, 1.0f, 0.9f, 1.0f }, // NAUGHTY

			{ 0.9f, 1.1f, 1.0f, 1.0f, 1.0f }, // BOLD
			{ 1.0f, 1.0f, 1.0f, 1.0f, 1.0f }, // DOCILE
			{ 1.0f, 1.1f, 1.0f, 1.0f, 0.9f }, // RELAXED
			{ 1.0f, 1.1f, 0.9f, 1.0f, 1.0f }, // IMPISH
			{ 1.0f, 1.1f, 1.0f, 0.9f, 1.0f }, // LAX

			{ 0.9f, 1.0f, 1.0f, 1.0f, 1.1f }, // TIMID
			{ 1.0f, 0.9f, 1.0f, 1.0f, 1.1f }, // HASTY
			{ 1.0f, 1.0f, 1.0f, 1.0f, 1.0f }, // SERIOUS
			{ 1.0f, 1.0f, 0.9f, 1.0f, 1.1f }, // JOLLY
			{ 1.0f, 1.0f, 1.0f, 0.9f, 1.1f }, // NAIVE

			{ 0.9f, 1.0f, 1.1f, 1.0f, 1.0f }, // MODEST
			{ 1.0f, 0.9f, 1.1f, 1.0f, 1.0f }, // MILD
			{ 1.0f, 1.0f, 1.1f, 1.0f, 0.9f }, // QUIET
			{ 1.0f, 1.0f, 1.0f, 1.0f, 1.0f }, // BASHFUL
			{ 1.0f, 1.0f, 1.1f, 0.9f, 1.0f }, // RASH

			{ 0.9f, 1.0f, 1.0f, 1.1f, 1.0f }, // CALM
			{ 1.0f, 0.9f, 1.0f, 1.1f, 1.0f }, // GENTLE
			{ 1.0f, 1.0f, 1.0f, 1.1f, 0.9f }, // SASSY
			{ 1.0f, 1.0f, 0.9f, 1.1f, 1.0f }, // CAREFUL
			{ 1.0f, 1.0f, 1.0f, 1.0f, 1.0f }, // QUIRKY
	};

	const size_t iRow = static_cast<size_t>(eNature);
	const size_t iCol = static_cast<size_t>(eStat) - 1u;

	if (iRow >= static_cast<size_t>(NATURE::END) || iCol >= 5u)
		return 1.f;

	return s_kNatureTable[iRow][iCol];
}

void Recalc_All_Stats(POKEMON_INSTANCE& tInstance, const SPECIES_DATA& tSpecies)
{
	using S = STAT;

	tInstance.iStat[static_cast<size_t>(S::HP)] =
		Calc_HP_Stat(tSpecies.iBaseHP, tInstance.iIV[static_cast<size_t>(S::HP)],
			tInstance.iEV[static_cast<size_t>(S::HP)], tInstance.iLevel);

	tInstance.iStat[static_cast<size_t>(S::ATK)] =
		Calc_Other_Stat(tSpecies.iBaseAtk, tInstance.iIV[static_cast<size_t>(S::ATK)],
			tInstance.iEV[static_cast<size_t>(S::ATK)], tInstance.iLevel,
			Get_NatureMultiplier(tInstance.eNature, S::ATK));

	tInstance.iStat[static_cast<size_t>(S::DEF)] =
		Calc_Other_Stat(tSpecies.iBaseDef, tInstance.iIV[static_cast<size_t>(S::DEF)],
			tInstance.iEV[static_cast<size_t>(S::DEF)], tInstance.iLevel,
			Get_NatureMultiplier(tInstance.eNature, S::DEF));

	tInstance.iStat[static_cast<size_t>(S::SPATK)] =
		Calc_Other_Stat(tSpecies.iBaseSpAtk, tInstance.iIV[static_cast<size_t>(S::SPATK)],
			tInstance.iEV[static_cast<size_t>(S::SPATK)], tInstance.iLevel,
			Get_NatureMultiplier(tInstance.eNature, S::SPATK));

	tInstance.iStat[static_cast<size_t>(S::SPDEF)] =
		Calc_Other_Stat(tSpecies.iBaseSpDef, tInstance.iIV[static_cast<size_t>(S::SPDEF)],
			tInstance.iEV[static_cast<size_t>(S::SPDEF)], tInstance.iLevel,
			Get_NatureMultiplier(tInstance.eNature, S::SPDEF));

	tInstance.iStat[static_cast<size_t>(S::SPD)] =
		Calc_Other_Stat(tSpecies.iBaseSpd, tInstance.iIV[static_cast<size_t>(S::SPD)],
			tInstance.iEV[static_cast<size_t>(S::SPD)], tInstance.iLevel,
			Get_NatureMultiplier(tInstance.eNature, S::SPD));
}

void Assign_Moves(POKEMON_INSTANCE& tInstance, const _uint* pMoveIDs, _uint iMoveCount, const CPokemonData_Manager* pDataMgr)
{
	for (_uint i = 0; i < g_kMaxMovesPerPokemon; ++i)
	{
		tInstance.iMoves[i] = 0;
		tInstance.iCurrentPP[i] = 0;
	}

	if (nullptr == pMoveIDs || nullptr == pDataMgr)
		return;

	const _uint iCount = (iMoveCount < g_kMaxMovesPerPokemon) ?
		iMoveCount : g_kMaxMovesPerPokemon;

	for (_uint i = 0; i < iCount; ++i)
	{
		const _uint iMoveID = pMoveIDs[i];
		if (0 == iMoveID)
			continue;

		const MOVE_DATA* pMove = pDataMgr->Find_Move(iMoveID);
		if (nullptr == pMove)
			continue;

		tInstance.iMoves[i] = iMoveID;
		tInstance.iCurrentPP[i] = pMove->iMaxPP;
	}
}

POKEMON_INSTANCE Build_PokemonInstance(
	const SPECIES_DATA& tSpecies,
	_ubyte iLevel,
	_uint iOriginalTrainerID,
	_uint iCapturedAtZoneID,
	const CPokemonData_Manager* pDataMgr)
{
	POKEMON_INSTANCE tInstance = {};

	tInstance.iSpeciesID = tSpecies.iDexNo;
	wcscpy_s(tInstance.szNickname, tSpecies.szName);

	tInstance.iLevel = iLevel;
	tInstance.iExp = 0;

	for (size_t i = 0; i < static_cast<size_t>(STAT::END); ++i)
	{
		tInstance.iIV[i] = g_kMaxIV;
		tInstance.iEV[i] = 0;
	}

	tInstance.eNature = NATURE::JOLLY;
	tInstance.iAbilityID = tSpecies.iAbility1;

	const _uint iInitialMoves[g_kMaxMovesPerPokemon] =
	{
			tSpecies.iLearnset[0],
			tSpecies.iLearnset[1],
			tSpecies.iLearnset[2],
			tSpecies.iLearnset[3],
	};

	Assign_Moves(tInstance, iInitialMoves, g_kMaxMovesPerPokemon, pDataMgr);

	tInstance.eStatus = STATUS_CONDITION::NONE;
	tInstance.iHeldItemID = 0;

	Recalc_All_Stats(tInstance, tSpecies);
	tInstance.iCurrentHP = tInstance.iStat[static_cast<size_t>(STAT::HP)];

	tInstance.iOriginalTrainerID = iOriginalTrainerID;
	tInstance.iCapturedAtZoneID = iCapturedAtZoneID;

	return tInstance;
}

void PartyOps::Clear(PARTY& tParty)
{
	memset(&tParty, 0, sizeof(PARTY));
	tParty.iCount = 0;
}

_bool PartyOps::Add(PARTY& tParty, const POKEMON_INSTANCE& tInstance)
{
	if (tParty.iCount >= g_kMaxPartySize)
		return false;

	tParty.arrSlots[tParty.iCount] = tInstance;
	++tParty.iCount;

	return true;
}

POKEMON_INSTANCE* PartyOps::Get(PARTY& tParty, _uint iSlot)
{
	if (iSlot >= tParty.iCount)
		return nullptr;

	return &tParty.arrSlots[iSlot];
}

const POKEMON_INSTANCE* PartyOps::Get(const PARTY& tParty, _uint iSlot)
{
	if (iSlot >= tParty.iCount)
		return nullptr;

	return &tParty.arrSlots[iSlot];
}

_bool PartyOps::Has_Empty_Slot(const PARTY& tParty)
{
	return tParty.iCount < g_kMaxPartySize;
}

_uint PartyOps::Find_First_Alive(const PARTY& tParty)
{
	for (_uint i = 0; i < tParty.iCount; ++i)
	{
		if (tParty.arrSlots[i].iCurrentHP > 0)
			return i;
	}

	return g_kMaxPartySize;
}

NS_END