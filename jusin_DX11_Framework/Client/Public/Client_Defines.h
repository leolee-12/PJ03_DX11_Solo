#pragma once
#include <Windows.h>
#include <process.h>

#include "Game_PKM_Defines.h"

namespace Client
{
	inline constexpr unsigned int g_iWinSizeX = { 1280 };
	inline constexpr unsigned int g_iWinSizeY = { 720 };

	WNAME_TAG(TIMER_DEFAULT, L"Timer_Default");
	WNAME_TAG(TIMER_FPS60, L"Timer_FPS60");
}

extern HINSTANCE g_hInstance;
extern HWND g_hWnd;

using namespace Client;