#ifndef Engine_Enum_h__
#define Engine_Enum_h__

namespace Engine
{
	enum class WINMODE { FULL, WIN };

	enum class PROTOTYPE { GAMEOBJECT, COMPONENT };

	enum class RENDERID { PRIORITY, NONBLEND, BLEND, UI, END };
	
	enum class STATE {	RIGHT, UP, LOOK, POSITION, END };

	enum class D3DTS { VIEW, PROJ, END };

	enum class LIGHT { DIRECTIONAL, POINT,/* SPOT,*/ END };

	enum class MODEL { NONANIM, ANIM, END };

	//// Dynamic 컴포넌트 경우 매 프레임마다 갱신해야하는 컴포넌트 집단
	//enum COMPONENTID { ID_DYNAMIC, ID_STATIC, ID_END };
	//
	//enum ROTATION { ROT_X, ROT_Y, ROT_Z, ROT_END };
	//
	//enum TEXTUREID { TEX_NORMAL, TEX_CUBE, TEX_END };

	enum class DIMB { LBUTTON, RBUTTON, WHEEL, END };

	enum class DIMM { X, Y, WHEEL, END };

	enum class INPUT_STATE { GAMEPLAY, OBSERVE, MENU, NAVIGATE, LOCKED, END };
	
	enum class TEXTURE_TYPE
	{
		DUMMY = 0,
		DIFFUSE,
		SPECULAR,
		AMBIENT,
		EMISSIVE,
		HEIGHT,
		NORMALS,
		SHININESS,
		OPACITY,
		END
	};

	namespace KeyGroup
	{
		enum  : unsigned int
		{
			MOVEMENT	= 1u << 0,	// WASD, 화살표 등 이동 관련 키
			CAMERA		= 1u << 1,	// 마우스 이동, 마우스 버튼
			ACTION		= 1u << 2,	// 공격, 스킬 등 게임 액션 키
			UI_NAVIGATE = 1u << 3,	// Enter, 방향키, Tab 등 UI 탐색 키
			SYSTEM		= 1u << 4,	// Esc, F1-F12 등 시스템 제어 키
			TOOL		= 1u << 5,	// 개발 관련으로 사용하는 키
			ALL			= 0xFFFFFFFF
		};

		inline constexpr unsigned int s_AllowedGroups[static_cast<unsigned int>(INPUT_STATE::END)] =
		{
			/* GAMEPLAY */ ALL,
			/* OBSERVE	*/ SYSTEM,
			/* MENU		*/ UI_NAVIGATE | SYSTEM,
			/* NAVIGATE	*/ CAMERA | SYSTEM | TOOL,
			/* LOCKED	*/ SYSTEM | TOOL
		};
	}
}
#endif // Engine_Enum_h__
