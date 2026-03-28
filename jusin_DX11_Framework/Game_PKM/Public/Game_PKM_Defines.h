#ifndef Game_PKM_Defines_h__
#define Game_PKM_Defines_h__

#include <Windows.h>
#include <process.h>

#include "Engine_Defines.h"
#include "Game_PKM_Tags.h"

NS_BEGIN(Game_PKM)

enum class LEVEL { STATIC, LOADING, LOGO, GAMEPLAY, END };

NS_END

namespace ObjFlag
{
	enum : unsigned int
	{
		ON_GROUND = 1u << 8,
		INVINCIBLE = 1u << 9
	};
}

using namespace Game_PKM;

#endif // Game_PKM_Defines_h__