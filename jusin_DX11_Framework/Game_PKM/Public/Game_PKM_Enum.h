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

NS_END

#endif // Game_PKM_Enum_h__