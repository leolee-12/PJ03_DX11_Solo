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

enum class TEXTURE_ATLAS_LAYOUT : _uint { SINGLE, BI_HORIZONTAL, BI_VERTICAL, QUAD, END };

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

NS_END

#endif // Game_PKM_Enum_h__