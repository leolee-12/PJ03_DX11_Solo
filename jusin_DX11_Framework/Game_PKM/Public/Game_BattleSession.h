#ifndef Game_PKM_BattleSession_h__
#define Game_PKM_BattleSession_h__

#include "Game_PKM_Defines.h"
#include "Game_BattleData.h"

NS_BEGIN(Game_PKM)

inline constexpr _uint g_kBattleSide_Player = 0;
inline constexpr _uint g_kBattleSide_Opponent = 1;
inline constexpr _uint g_kBattleSideCount = 2;

enum class STAGE_INDEX : _ubyte
{
	ATK, DEF, SPATK, SPDEF, SPD, ACC, EVA, COUNT
};

namespace VolatileFlag
{
	enum : _uint
	{
		CONFUSION = 1u << 0,		// È¥¶õ
		LEECH_SEED = 1u << 1,		// ¾¾»Ñ¸®±â
		ENCORE = 1u << 2,			// ¾ÞÄÝ
		TAUNT = 1u << 3,			// µµ¹ß
		DISABLE = 1u << 4,			// »ç½½¹­±â
		FLINCH = 1u << 5,			// Ç®Á×À½
		SUBSTITUTE = 1u << 6,		// ´ëÅ¸Ãâµ¿
		FOCUS_ENERGY = 1u << 7,		// ±âÃæÀü
	};

	inline constexpr _uint VOLATILE_TURN_SLOT_COUNT = 8;
}

struct BATTLE_ENV
{
	ENVIRONMENT_TYPE eEnvironment;
	BATTLE_RULE      eRule;
	_uint            iBGResourceID;
	_bool            bCanRun;
	_bool            bCanCapture;
	_bool            bExpGain;
	_uint            iZoneID;
};

struct BATTLE_SLOT
{
	POKEMON_INSTANCE* pPokemon;

	_byte  iStatStage[static_cast<size_t>(STAGE_INDEX::COUNT)];
	_uint  iVolatileFlags;
	_ubyte iVolatileTurnCount[VolatileFlag::VOLATILE_TURN_SLOT_COUNT];
	_uint  iLastMoveUsed;
	_bool  bProtected;
	_bool  bMustRecharge;
};

struct FIELD_STATE
{
	WEATHER_TYPE eWeather;
	_ubyte       iWeatherTurns;
	TERRAIN_TYPE eTerrain;
	_ubyte       iTerrainTurns;

	_bool        bTrickRoom;
	_ubyte       iTrickRoomTurns;
	_bool        bGravity;
	_ubyte       iGravityTurns;

	_bool        bReflect[g_kBattleSideCount];
	_ubyte       iReflectTurns[g_kBattleSideCount];
	_bool        bLightScreen[g_kBattleSideCount];
	_ubyte       iLightScreenTurns[g_kBattleSideCount];
	_bool        bStealth[g_kBattleSideCount];
	_ubyte       iSpikesLayer[g_kBattleSideCount];
	_bool        bTailwind[g_kBattleSideCount];
	_ubyte       iTailwindTurns[g_kBattleSideCount];
};

struct TURN_CONTEXT
{
	struct ACTION
	{
		ACTION_TYPE eType;
		_uint       iParam;
		_byte       iPriority;
	};

	ACTION tAction[g_kBattleSideCount];
	_uint  iFirstSide;
	_uint  iTurnNumber;
};

NS_END

#endif // Game_PKM_BattleSession_h__