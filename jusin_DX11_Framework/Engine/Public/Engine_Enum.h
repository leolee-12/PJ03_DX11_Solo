#ifndef Engine_Enum_h__
#define Engine_Enum_h__

namespace Engine
{
	enum class WINMODE { FULL, WIN };

	enum class PROTOTYPE { GAMEOBJECT, COMPONENT };

	enum class RENDERID { PRIORITY, SHADOW, NONBLEND, NONLIGHT, BLEND, UI, OUTLINEMASK, END };
	
	enum class STATE {	RIGHT, UP, LOOK, POSITION, END };

	enum class D3DTS { VIEW, PROJ, END };

	enum class LIGHT { DIRECTIONAL, POINT, SPOT, END };

	enum class MODEL { NONANIM, ANIM, END };

	enum class VTXPOINT { A, B, C, END };

	enum class LINE { AB, BC, CA, END };

	enum class COLLIDER { AABB, OBB, SPHERE, END };

	enum class DEFERRED { DEBUG, DIRECTIONAL, POINT, SPOT, COMBINED, COMBINED_SHADOW, END };

	enum class CHANNELID { BGM, UI, VOICE, AMBIENT, SFX, MAXCHANNEL };

	enum class ANIM_UPDATE_RESULT
	{
		PLAYING = 0,
		FINISHED = 1,
		LOOP_WRAPPED = 2
	};

	enum class DIMB { LBUTTON, RBUTTON, WHEEL, END };

	enum class DIMM { X, Y, WHEEL, END };

	enum class INPUT_STATE { GAMEPLAY, OBSERVE, MENU, NAVIGATE, LOCKED, END };

	enum class MATERIAL_TYPE
	{
		NONE = 0,
		DIFFUSE = 1,
		SPECULAR = 2,
		AMBIENT = 3,
		EMISSIVE = 4,
		HEIGHT = 5,
		NORMALS = 6,
		SHININESS = 7,
		OPACITY = 8,
		DISPLACEMENT = 9,
		LIGHTMAP = 10,
		REFLECTION = 11,
		BASE_COLOR = 12,
		NORMAL_CAMERA = 13,
		EMISSION_COLOR = 14,
		METALNESS = 15,
		DIFFUSE_ROUGHNESS = 16,
		AMBIENT_OCCLUSION = 17,
		UNKNOWN = 18,
		SHEEN = 19,
		CLEARCOAT = 20,
		TRANSMISSION = 21,
		MAYA_BASE = 22,
		MAYA_SPECULAR = 23,
		MAYA_SPECULAR_COLOR = 24,
		MAYA_SPECULAR_ROUGHNESS = 25,
		ANISOTROPY = 26,
		GLTF_METALLIC_ROUGHNESS = 27,
		LAYER_MASK = 28,	// lym (레이어 마스크)
		LAYER_COLOR = 29,	// lyc (눈동자 레이어 컬러)
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
			/* NAVIGATE	*/ UI_NAVIGATE | SYSTEM | TOOL,
			/* LOCKED	*/ SYSTEM | TOOL
		};
	}
}

#endif // Engine_Enum_h__
