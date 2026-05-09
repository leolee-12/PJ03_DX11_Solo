#ifndef Game_PKM_Enum_h__
#define Game_PKM_Enum_h__

NS_BEGIN(Game_PKM)

enum class LEVEL { STATIC, LOADING, LOGO, GAMEPLAY, BATTLE, END };

namespace Navigation
{
	enum NAV_CELL_OPTION : _uint
	{
		NAV_NONE			= 0,
		NAV_GROUND			= 1 << 0,   // 지면 : Walkable
		NAV_GRASS			= 1 << 1,   // 풀숲 : 야생포켓몬 출현
		NAV_WATER			= 1 << 2,   // 물	: 일반적으로 이동 불가
		NAV_LEDGE			= 1 << 3,   // 언덕 : 한 방향으로만 이동 가능
		NAV_SECOND_FLOOR	= 1 << 4,   // 2층  : 다리, 층계 등
	};
}

enum class TEXTURE_SAMPLE_MODE : _uint { SINGLE, BI_HORIZONTAL, BI_VERTICAL, QUAD, END };

// 포켓몬 타입
enum class TYPE : _uint32
{
	NONE = 0,
	NORMAL,
	FIGHTING,
	FLYING,
	POISON,
	GROUND,
	ROCK,
	BUG,
	GHOST,
	FIRE,
	WATER,
	GRASS,
	ELECTRIC,
	PSYCHIC,
	ICE,
	DRAGON,
	STEEL,
	DARK,
	FAIRY,
	STELLA,
	QUESTION
};

inline constexpr TYPE operator|(TYPE a, TYPE b) noexcept
{
	return static_cast<TYPE>(static_cast<_uint32>(a) | static_cast<_uint32>(b));
}

inline constexpr TYPE operator&(TYPE a, TYPE b) noexcept
{
	return static_cast<TYPE>(static_cast<_uint32>(a) & static_cast<_uint32>(b));
}

inline constexpr TYPE& operator|=(TYPE& a, TYPE b) noexcept
{
	a = a | b; return a;
}

inline constexpr TYPE operator~(TYPE a) noexcept
{
	return static_cast<TYPE>(~static_cast<_uint32>(a));
}

inline constexpr TYPE operator^(TYPE a, TYPE b) noexcept
{
	return static_cast<TYPE>(static_cast<_uint32>(a) ^ static_cast<_uint32>(b));
}

inline constexpr TYPE ToMask(TYPE type) noexcept
{
	return static_cast<TYPE>(1u << static_cast<_uint32>(type));
}

inline constexpr bool HasFlag(TYPE value, TYPE flag) noexcept
{
	return (value & flag) != TYPE::NONE;
}

inline constexpr TYPE RemoveFlag(TYPE value, TYPE flag) noexcept
{
	return static_cast<TYPE>(static_cast<uint32_t>(value) & ~static_cast<uint32_t>(flag));
}

enum class MOVE_CATEGORY : _ubyte { PHYSICAL, SPECIAL, STATUS, END };

enum class STATUS_CONDITION : _ubyte { NONE, POISON, PARALYSIS, BURN, SLEEP, FROZEN, BAD_POISON, END };

enum class BATTLE_RULE : _ubyte { WILD_SINGLE, TRAINER_SINGLE, TRAINER_DOUBLE, CUTSCENE, TUTORIAL, END };

enum class STAT : _ubyte { HP, ATK, DEF, SPATK, SPDEF, SPD, END };

enum class NATURE : _ubyte {HARDY,		LONELY,		BRAVE,		ADAMANT,	NAUGHTY,
							BOLD,		DOCILE,		RELAXED,	IMPISH,		LAX,
							TIMID,		HASTY,		SERIOUS,	JOLLY,		NAIVE,
							MODEST,		MILD,		QUIET,		BASHFUL,	RASH,
							CALM,		GENTLE,		SASSY,		CAREFUL,	QUIRKY,		END };

enum class TRAINER_AI : _ubyte { RANDOM, BASIC, STRATEGIC, SCRIPTED, END };

enum class ENVIRONMENT_TYPE : _ubyte { PLAIN, GRASS, FOREST, CAVE, WATER, DESERT, SNOW, URBAN, END };

enum class WEATHER_TYPE : _ubyte { NONE, SUNNY, RAIN, SAND, HAIL, FOG, END };

enum class TERRAIN_TYPE : _ubyte { NONE, GRASS_FIELD, ELECTRIC_FIELD, PSYCHIC_FIELD, MISTY_FIELD, END };

enum class ACTION_TYPE : _ubyte { NONE, USE_MOVE, SWITCH, USE_ITEM, RUN, END };

enum class BATTLE_PHASE : _ubyte { INTRO, INPUT_PLAYER, INPUT_OPPONENT, RESOLVE_ORDER, RESOLVE_ACTION_1, RESOLVE_ACTION_2, RESOLVE_END_TURN, CHECK_END, OUTRO, DONE, END };

NS_END

#endif // Game_PKM_Enum_h__