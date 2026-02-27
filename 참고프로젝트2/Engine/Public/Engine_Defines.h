#pragma once

#pragma warning (disable : 4251)

#include <d3d11.h>

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>
#include <DirectXCollision.h>

#include <DirectXTK\DDSTextureLoader.h>
#include <DirectXTK\WICTextureLoader.h>
#include <DirectXTK\PrimitiveBatch.h>
#include <DirectXTK\Effects.h>
#include <DirectXTK\VertexTypes.h>
#include <DirectXTK\SpriteBatch.h>
#include <DirectXTK\SpriteFont.h>

#include <DirectXTK\ScreenGrab.h>
#include <Effects11\d3dx11effect.h>

#include <assimp\Importer.hpp>
#include <assimp\scene.h>
#include <assimp\postprocess.h>

using namespace DirectX;

#include <vector>
#include <list>
#include <map>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <queue>
#include <set>
#include <tuple>
#include <mutex>
#include <future>
#include <iostream>

using namespace std;

#include "Engine_Macro.h"
#include "Engine_Struct.h"
#include "Engine_TypeDef.h"
#include "Engine_Function.h"

namespace Engine
{
	enum MOUSEKEYSTATE { DIMK_LBUTTON, DIMK_RBUTTON, DIMK_WHEEL, DIMK_X, DIMK_END };
	enum MOUSEMOVESTATE { DIMM_X, DIMM_Y, DIMM_WHEEL, DIMM_END };

	enum DATATYPE
	{
		TYPE_GAMEOBJECT_DESC,
		TYPE_FILEDATA
	};
}

using namespace Engine;

#ifdef _DEBUG

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#ifndef DBG_NEW 

#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ ) 
#define new DBG_NEW 

#endif

#endif // _DEBUG

