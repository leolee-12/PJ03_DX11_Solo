#pragma once
#include <Windows.h>
#include <process.h>

#include "Game_PKM_Defines.h"

namespace Client
{
	constexpr unsigned int g_iWinSizeX = { 1280 };
	constexpr unsigned int g_iWinSizeY = { 720 };

	static Engine::WNameID TIMER_DEFAULT = WNAME(L"Timer_Default");
	static Engine::WNameID TIMER_FPS60 = WNAME(L"Timer_FPS60");
}

extern HINSTANCE g_hInstance;
extern HWND g_hWnd;

using namespace Client;