#ifndef Engine_Defines_h__
#define Engine_Defines_h__

#include <d3d11.h>
#include <DirectXMath.h>
#include <DirectXCollision.h>
#include <d3dcompiler.h>

#define DIRECTINPUT_VERSION	0x0800
#include <dinput.h>

#include <fx11/d3dx11effect.h>
#include <DirectTK/DDSTextureLoader.h>
#include <DirectTK/WICTextureLoader.h>
#include <DirectTK/SpriteBatch.h>
#include <DirectTK/SpriteFont.h>
#include <DirectTK/ScreenGrab.h>
#include <DirectTK/PrimitiveBatch.h>
#include <DirectTK/VertexTypes.h>
#include <DirectTK/Effects.h>

using namespace DirectX;

//#pragma warning(push, 0)
//#include <PhysX/PxPhysicsAPI.h>
//#pragma warning(pop)

#include "json/json.hpp"
using json = nlohmann::json;

#include <array>
#include <vector>
#include <list>
#include <queue>
#include <map>
#include <algorithm>
#include <functional>
#include <string>
#include <set>
#include <unordered_set>
#include <unordered_map>
#include <ctime>
#include <atomic>
#include <mutex>
#include <fstream>
#include <variant>

using namespace std;

#include "Engine_WName.h"
#include "Engine_Enum.h"
#include "Engine_Macro.h"
#include "Engine_Struct.h"
#include "Engine_Typedef.h"
#include "Engine_Function.h"

#include "Engine_Tags.h"
#include "Engine_UI.h"

namespace Engine
{
	inline constexpr _uint g_iMaxWidth = 8192;
	inline constexpr _uint g_iMaxHeight = 4608;

	inline constexpr _uint INVALID_INDEX = static_cast<_uint>(-1);

	inline constexpr _float4 g_kBlack	= { 0.f, 0.f, 0.f, 1.f };
	inline constexpr _float4 g_kRed		= { 1.f, 0.f, 0.f, 1.f };
	inline constexpr _float4 g_kGreen	= { 0.f, 1.f, 0.f, 1.f };
	inline constexpr _float4 g_kBlue	= { 0.f, 0.f, 1.f, 1.f };
	inline constexpr _float4 g_kYellow	= { 1.f, 1.f, 0.f, 1.f };
	inline constexpr _float4 g_kMagenta	= { 1.f, 0.f, 1.f, 1.f };
	inline constexpr _float4 g_kCyan	= { 0.f, 1.f, 1.f, 1.f };
	inline constexpr _float4 g_kWhite	= { 1.f, 1.f, 1.f, 1.f };

	inline constexpr XMVECTORF32 g_XMBlack		= { 0.f, 0.f, 0.f, 1.f };
	inline constexpr XMVECTORF32 g_XMRed		= { 1.f, 0.f, 0.f, 1.f };
	inline constexpr XMVECTORF32 g_XMGreen		= { 0.f, 1.f, 0.f, 1.f };
	inline constexpr XMVECTORF32 g_XMBlue		= { 0.f, 0.f, 1.f, 1.f };
	inline constexpr XMVECTORF32 g_XMYellow		= { 1.f, 1.f, 0.f, 1.f };
	inline constexpr XMVECTORF32 g_XMMagenta	= { 1.f, 0.f, 1.f, 1.f };
	inline constexpr XMVECTORF32 g_XMCyan		= { 0.f, 1.f, 1.f, 1.f };
	inline constexpr XMVECTORF32 g_XMWhite		= { 1.f, 1.f, 1.f, 1.f };

	inline constexpr _uint g_iNumMeshBones = { 512 };
	inline constexpr _uint g_kMaxTraverse = { 64 };
	inline constexpr _float g_kDefaultBlendDuration = { 0.2f };
	inline constexpr _float g_kDebugOffset_Y = { 0.05f };
}

namespace ObjFlag
{
	enum : unsigned int
	{
		DEAD = 1u << 0,
		ACTIVE = 1u << 1,
		FLAG_3 = 1u << 2,
		FLAG_4 = 1u << 3,
		FLAG_5 = 1u << 4,
		FLAG_6 = 1u << 5,
		FLAG_7 = 1u << 6,
		FLAG_8 = 1u << 7,
	};
}

#pragma warning(disable : 4251)

#ifdef _DEBUG
#include <iostream>
#include <io.h>
#include <fcntl.h>

inline void OpenDebugConsole()
{
	AllocConsole();

	FILE* fp = nullptr;

	freopen_s(&fp, "CONOUT$", "w", stdout);
	freopen_s(&fp, "CONOUT$", "w", stderr);
	freopen_s(&fp, "CONIN$", "r", stdin);

	_setmode(_fileno(stdout), _O_U16TEXT);
	_setmode(_fileno(stderr), _O_U16TEXT);

	std::ios::sync_with_stdio();

	SetConsoleTitle(L"Debug Console");

	std::wcout << L"[Debug Console Opened]\n";
}

inline void DebugPrintW(const wchar_t* text)
{
	DWORD written = 0;
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	if (hConsole != INVALID_HANDLE_VALUE && hConsole != nullptr)
	{
		WriteConsoleW(
			hConsole,
			text,
			static_cast<DWORD>(wcslen(text)),
			&written,
			nullptr
		);
	}

	wcout << '\n';
}

inline void CloseDebugConsole()
{
	FreeConsole();
}

#define DBG_LOG(msg) std::wcout << msg << std::endl
#else
inline void OpenDebugConsole() {}
inline void CloseDebugConsole() {}
#define DBG_LOG(msg)

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

//#ifndef DBG_NEW 
//
//#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ ) 
//#define new DBG_NEW 
//
//#endif
#endif

using namespace Engine;

#endif // Engine_Defines_h__
