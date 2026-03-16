#ifndef Game_PKM_Defines_h__
#define Game_PKM_Defines_h__

#include <Windows.h>

namespace Game_PKM
{
	enum class LEVEL { STATIC, LOADING, LOGO, GAMEPLAY, END };
}

extern HINSTANCE	g_hInstance;	// EXE에서 정의, Game_PKM.lib 코드에서 참조
extern HWND			g_hWnd;			// EXE에서 정의, Game)PKM.lib 코드에서 참조

using namespace Game_PKM;

#endif // Game_PKM_Defines_h__