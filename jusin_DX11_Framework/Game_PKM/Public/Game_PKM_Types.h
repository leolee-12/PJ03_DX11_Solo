#ifndef Game_PKM_Types_h__
#define Game_PKM_Types_h__

NS_BEGIN(Game_PKM)

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
{ return static_cast<TYPE>(static_cast<_uint32>(a) | static_cast<_uint32>(b)); }

inline constexpr TYPE operator&(TYPE a, TYPE b) noexcept
{ return static_cast<TYPE>(static_cast<_uint32>(a) & static_cast<_uint32>(b)); }

inline constexpr TYPE& operator|=(TYPE& a, TYPE b) noexcept
{ a = a | b; return a; }

inline constexpr TYPE operator~(TYPE a) noexcept
{ return static_cast<TYPE>(~static_cast<_uint32>(a)); }

inline constexpr TYPE operator^(TYPE a, TYPE b) noexcept
{ return static_cast<TYPE>(static_cast<_uint32>(a) ^ static_cast<_uint32>(b)); }

inline constexpr TYPE ToMask(TYPE type) noexcept
{ return static_cast<TYPE>(1u << static_cast<_uint32>(type)); }

inline constexpr bool HasFlag(TYPE value, TYPE flag) noexcept
{ return (value & flag) != TYPE::NONE; }

inline constexpr TYPE RemoveFlag(TYPE value, TYPE flag) noexcept
{ return static_cast<TYPE>(static_cast<uint32_t>(value) & ~static_cast<uint32_t>(flag)); }

NS_END

#endif // Game_PKM_Types_h__