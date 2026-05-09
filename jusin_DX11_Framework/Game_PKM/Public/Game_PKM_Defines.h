#ifndef Game_PKM_Defines_h__
#define Game_PKM_Defines_h__

#include <Windows.h>
#include <process.h>
#include <concurrent_queue.h>

#include "Engine_Defines.h"
#include "Game_PKM_Enum.h"
#include "Game_PKM_Struct.h"

#include "Game_PKM_Tags.h"

NS_BEGIN(Game_PKM)

inline constexpr _float3 g_kDir_UP = { 0.f, 0.f,  1.f };
inline constexpr _float3 g_kDir_DOWN = { 0.f, 0.f, -1.f };
inline constexpr _float3 g_kDir_LEFT = { -1.f, 0.f,  0.f };
inline constexpr _float3 g_kDir_RIGHT = { 1.f, 0.f,  0.f };

inline constexpr _float3 g_kDir_UP_LEFT = { -INV_SQRT2, 0.f,  INV_SQRT2 };
inline constexpr _float3 g_kDir_UP_RIGHT = { INV_SQRT2, 0.f,  INV_SQRT2 };
inline constexpr _float3 g_kDir_DOWN_LEFT = { -INV_SQRT2, 0.f, -INV_SQRT2 };
inline constexpr _float3 g_kDir_DOWN_RIGHT = { INV_SQRT2, 0.f, -INV_SQRT2 };



NS_END



NS_BEGIN(ObjFlag)

enum : unsigned int
{
	ON_GROUND = 1u << 8,
	INVINCIBLE = 1u << 9
};

NS_END

using namespace Game_PKM;

#endif // Game_PKM_Defines_h__