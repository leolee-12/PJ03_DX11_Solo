#include "Player_Data.h"

NS_BEGIN(Game_PKM)

void PokedexOps::Clear(POKEDEX& tPokedex)
{
	for (_uint i = 0; i <= g_kMaxDexNo; ++i)
		tPokedex.arrStates[i] = POKEDEX_STATE::UNSEEN;
}

POKEDEX_STATE PokedexOps::Get(const POKEDEX& tPokedex, _uint iDexNo)
{
	if (0 == iDexNo || iDexNo > g_kMaxDexNo)
		return POKEDEX_STATE::UNSEEN;

	return tPokedex.arrStates[iDexNo];
}

_bool PokedexOps::Mark_Seen(POKEDEX& tPokedex, _uint iDexNo)
{
	if (0 == iDexNo || iDexNo > g_kMaxDexNo)
		return false;

	if (POKEDEX_STATE::UNSEEN != tPokedex.arrStates[iDexNo])
		return false;

	tPokedex.arrStates[iDexNo] = POKEDEX_STATE::SEEN;
	return true;
}

_bool PokedexOps::Mark_Caught(POKEDEX& tPokedex, _uint iDexNo)
{
	if (0 == iDexNo || iDexNo > g_kMaxDexNo)
		return false;

	if (POKEDEX_STATE::CAUGHT == tPokedex.arrStates[iDexNo])
		return false;

	tPokedex.arrStates[iDexNo] = POKEDEX_STATE::CAUGHT;
	return true;
}

_uint PokedexOps::Count(const POKEDEX& tPokedex, POKEDEX_STATE eState)
{
	_uint iCount = 0;

	for (_uint i = 1; i <= g_kMaxDexNo; ++i)
	{
		if (eState == tPokedex.arrStates[i])
			++iCount;
	}

	return iCount;
}

void BoxOps::Clear(BOX& tBox)
{
	memset(&tBox, 0, sizeof(BOX));
	tBox.iCount = 0;
}

_bool BoxOps::Add(BOX& tBox, const POKEMON_INSTANCE& tInstance)
{
	if (tBox.iCount >= g_kMaxTempBoxSize)
		return false;

	tBox.arrSlots[tBox.iCount] = tInstance;
	++tBox.iCount;

	return true;
}

POKEMON_INSTANCE* BoxOps::Get(BOX& tBox, _uint iSlot)
{
	if (iSlot >= tBox.iCount)
		return nullptr;

	return &tBox.arrSlots[iSlot];
}

const POKEMON_INSTANCE* BoxOps::Get(const BOX& tBox, _uint iSlot)
{
	if (iSlot >= tBox.iCount)
		return nullptr;

	return &tBox.arrSlots[iSlot];
}

_bool BoxOps::Has_Empty_Slot(const BOX& tBox)
{
	return tBox.iCount < g_kMaxTempBoxSize;
}

NS_END