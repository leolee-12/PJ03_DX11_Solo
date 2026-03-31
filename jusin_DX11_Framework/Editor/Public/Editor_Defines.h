#pragma once
#include <Windows.h>
#include <process.h>

#include "Game_PKM_Defines.h"
#include "Editor_Struct.h"

#include "imgui.h"
#include "backends/imgui_impl_win32.h"
#include "backends/imgui_impl_dx11.h"
#include "ImGuizmo.h"

NS_BEGIN(Engine)
class CGameObject;
NS_END

NS_BEGIN(Editor)

constexpr unsigned int	g_iWinSizeX = { 1280 };
constexpr unsigned int	g_iWinSizeY = { 720 };

WNAME_TAG(TIMER_DEFAULT, L"Timer_Default");
WNAME_TAG(TIMER_FPS60, L"Timer_FPS60");

enum class PANEL { OUTLINER, PROPERTY, PLACEBROWSER, VIEWPORT, UITOOL, MAP, OBJECT, EFFECT, END };
constexpr size_t PANEL_COUNT = static_cast<size_t>(PANEL::END);

using SelectionChangedCB = function<void(const vector<Engine::CGameObject*>&)>;

NS_END

extern HINSTANCE g_hInstance;
extern HWND g_hWnd;
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

using namespace Editor;