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

#include <json/json.hpp>
using json = nlohmann::json;

#include <array>
#include <vector>
#include <list>
#include <map>
#include <algorithm>
#include <functional>
#include <string>
#include <unordered_set>
#include <unordered_map>
#include <ctime>
#include <atomic>

using namespace std;

#include "Engine_WName.h"
#include "Engine_Enum.h"
#include "Engine_Macro.h"
#include "Engine_Struct.h"
#include "Engine_Typedef.h"
#include "Engine_Function.h"

namespace Engine
{
	WNAME_TAG(COM_TRANSFORM, L"Com_Transform");
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
