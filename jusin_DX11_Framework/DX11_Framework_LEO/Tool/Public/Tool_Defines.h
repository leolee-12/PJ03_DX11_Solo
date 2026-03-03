#pragma once
#include <Windows.h>
#include <process.h>

#include "imgui.h"
#include "backends/imgui_impl_win32.h"
#include "backends/imgui_impl_dx11.h"

namespace Tool
{
	static constexpr unsigned int	g_iWinSizeX = { 1280 };
	static constexpr unsigned int	g_iWinSizeY = { 720 };

	enum class LEVEL { STATIC, LOADING, EDITLOGO, EDITPLAY, END };
	enum class EDITOR_MODE { MAP, OBJECT, UI, EFFECT, END };
}

extern HWND g_hWnd;
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

using namespace Tool;