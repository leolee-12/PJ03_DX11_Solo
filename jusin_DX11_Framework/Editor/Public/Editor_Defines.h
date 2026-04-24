#ifndef Editor_Defines_h__
#define Editor_Defines_h__
#include <Windows.h>
#include <process.h>
#include <fstream>
#include <variant>

#include "Game_PKM_Defines.h"

#include "imgui.h"
#include "backends/imgui_impl_win32.h"
#include "backends/imgui_impl_dx11.h"
#include "ImGuizmo.h"

NS_BEGIN(Engine)
class CGameObject;
NS_END

extern HINSTANCE g_hInstance;
extern HWND g_hWnd;
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

NS_BEGIN(Editor)

inline constexpr unsigned int	g_iWinSizeX = { 1760 };
inline constexpr unsigned int	g_iWinSizeY = { 990 };

WNAME_TAG(TIMER_DEFAULT, L"Timer_Default");
WNAME_TAG(TIMER_FPS60, L"Timer_FPS60");

enum class PANEL { OUTLINER, MAP, PROPERTY, PLACEBROWSER, UI, MODEL, VIEWPORT, OBJECT, EFFECT, END };
inline constexpr size_t g_kNumPanels = static_cast<size_t>(PANEL::END);

using SelectionChangedCB = function<void(const vector<Engine::CGameObject*>&)>;

NS_END

#include "Editor_Function.h"
#include "Editor_Struct.h"

using namespace Editor;

#endif // Editor_Defines_h__