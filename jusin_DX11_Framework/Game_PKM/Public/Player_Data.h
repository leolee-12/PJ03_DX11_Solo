#pragma once
#include "Game_PKM_Defines.h"
#include "Battle_Data.h"

NS_BEGIN(Game_PKM)

inline constexpr _uint g_kMaxDexNo = 153;
inline constexpr _uint g_kMaxTempBoxSize = 120;

enum class POKEDEX_STATE : _ubyte
{
	UNSEEN,
	SEEN,
	CAUGHT,
	END
};

struct POKEDEX
{
	POKEDEX_STATE arrStates[g_kMaxDexNo + 1] = {};
};

struct BOX
{
	POKEMON_INSTANCE arrSlots[g_kMaxTempBoxSize] = {};
	_uint iCount = {};
};

NS_BEGIN(PokedexOps)
void Clear(POKEDEX& tPokedex);
POKEDEX_STATE Get(const POKEDEX& tPokedex, _uint iDexNo);
_bool Mark_Seen(POKEDEX& tPokedex, _uint iDexNo);
_bool Mark_Caught(POKEDEX& tPokedex, _uint iDexNo);
_uint Count(const POKEDEX& tPokedex, POKEDEX_STATE eState);
NS_END	// PokedexOps

NS_BEGIN(BoxOps)
void Clear(BOX& tBox);
_bool Add(BOX& tBox, const POKEMON_INSTANCE& tInstance);
POKEMON_INSTANCE* Get(BOX& tBox, _uint iSlot);
const POKEMON_INSTANCE* Get(const BOX& tBox, _uint iSlot);
_bool Has_Empty_Slot(const BOX& tBox);
NS_END	// BoxOps

NS_END	// Game_PKM