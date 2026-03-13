#pragma once
#include <Windows.h>
#include <process.h>

namespace Client
{
	constexpr unsigned int		g_iWinSizeX = { 1280 };
	constexpr unsigned int		g_iWinSizeY = { 720 };

	enum class LEVEL { STATIC, LOADING, LOGO, GAMEPLAY, END };
}

extern HWND g_hWnd;

using namespace Client;