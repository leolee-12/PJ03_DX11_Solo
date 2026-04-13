#ifndef Engine_Struct_h__
#define Engine_Struct_h__

namespace Engine
{
	struct ENGINE_DESC
	{
		HINSTANCE hInstance;
		HWND hWnd;
		WINMODE eWinMode;
		unsigned int iViewportWidth, iViewportHeight;
		unsigned int iNumLevels;
	};

	struct LIGHT_DESC
	{
		LIGHT eType;
		XMFLOAT4 vDiffuse, vAmbient, vSpecular;

		XMFLOAT4 vDirection;
		XMFLOAT4 vPosition;
		float fRange;
	};

	struct SCALING_KEY
	{
		XMFLOAT3 vScale;
		float fTrackPosition;
	};

	struct ROTATION_KEY
	{
		XMFLOAT4 vRotation;
		float fTrackPosition;
	};

	struct POSITION_KEY
	{
		XMFLOAT3 vTranslation;
		float fTrackPosition;
	};

	struct BONE_SRT
	{
		XMFLOAT3 vScale;
		XMFLOAT4 vRotation;
		XMFLOAT3 vTranslation;
	};

	struct RENDER_TABLE
	{
		vector<array<unsigned int, ETOUI(MATERIAL_TYPE::END)>> variants;
		vector<unsigned int> passes;

		void Ready_RenderTable(unsigned int iNumMaterials)
		{
			variants.resize(iNumMaterials);
			for (auto& slots : variants)
				slots.fill(0);
			passes.assign(iNumMaterials, 0);
		}
	};

	// 버텍스 구조체
	struct VTXPOS
	{
		XMFLOAT3 vPosition;

		static constexpr unsigned int iNumElements = { 1 };
		static constexpr D3D11_INPUT_ELEMENT_DESC Elements[] =
		{
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		};
	};

	struct VTXTEX
	{
		XMFLOAT3 vPosition;
		XMFLOAT2 vTexcoord;

		static constexpr unsigned int iNumElements = { 2 };
		static constexpr D3D11_INPUT_ELEMENT_DESC Elements[] =
		{
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
		};
	};

	struct VTXCUBE
	{
		XMFLOAT3 vPosition;
		XMFLOAT3 vTexcoord;

		static const unsigned int iNumElements = { 2 };

		static constexpr D3D11_INPUT_ELEMENT_DESC Elements[] =
		{
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
		};
	};

	struct VTXNORTEX
	{
		XMFLOAT3 vPosition;
		XMFLOAT3 vNormal;
		XMFLOAT2 vTexcoord;

		static constexpr unsigned int iNumElements = { 3 };
		static constexpr D3D11_INPUT_ELEMENT_DESC Elements[] =
		{
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0}
		};
	};

	struct VTXMESH
	{
		XMFLOAT3 vPosition;
		XMFLOAT3 vNormal;
		XMFLOAT2 vTexcoord;

		XMFLOAT3 vTangent;
		XMFLOAT3 vBinormal;

		static const unsigned int iNumElements = { 5 };

		static constexpr D3D11_INPUT_ELEMENT_DESC Elements[] =
		{
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 44, D3D11_INPUT_PER_VERTEX_DATA, 0},
		};
	};

	struct VTXANIMMESH
	{
		XMFLOAT3 vPosition;
		XMFLOAT3 vNormal;
		XMFLOAT2 vTexcoord;
				 
		XMFLOAT3 vTangent;
		XMFLOAT3 vBinormal;
				 
		XMUINT4 vBlendIndex;
		XMFLOAT4 vBlendWeight;

		static const unsigned int iNumElements = { 7 };

		static constexpr D3D11_INPUT_ELEMENT_DESC Elements[] =
		{
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 44, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"BLENDINDICES", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0, 56, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"BLENDWEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 72, D3D11_INPUT_PER_VERTEX_DATA, 0},
		};
	};



	// 바이너리 모델 관련
	struct WMODEL_HEADER
	{
		char szMagic[4];			// "WMDL"
		uint32_t iVersion;       // 포맷 버전
		uint32_t iModelType;     // MODEL enum 값
		uint32_t iNumMeshes;
		uint32_t iNumMaterials;
		uint32_t iNumBones;
		uint32_t iNumAnimations;
	};

	struct WMODEL_BONE
	{
		char szName[MAX_PATH];
		int32_t iParentIndex;
		XMFLOAT4X4 transformation;
	};

	struct WMODEL_MESH
	{
		char szName[MAX_PATH];
		uint32_t iMaterialIndex;
		vector<VTXMESH> nonAnimVertices;		// NONANIM 전용
		vector<VTXANIMMESH> animVertices;		// ANIM 전용
		vector<uint32_t> indices;
		vector<uint32_t> boneIndices;			// ANIM 전용
		vector<XMFLOAT4X4> offsetMatrices;		// ANIM 전용
	};

	struct WMODEL_MATERIAL
	{
		vector<string> TexturePaths[ETOUI(MATERIAL_TYPE::END)];
	};

	struct WMODEL_CHANNEL
	{
		uint32_t iBoneIndex;
		XMFLOAT3 vDefaultScale;
		XMFLOAT4 vDefaultRotation;
		XMFLOAT3 vDefaultTranslation;
		vector<SCALING_KEY> scalingKeys;
		vector<ROTATION_KEY> rotationKeys;
		vector<POSITION_KEY> positionKeys;
	};

	struct WMODEL_ANIMATION
	{
		char szName[MAX_PATH];
		float fDuration;
		float fTicksPerSecond;
		vector<WMODEL_CHANNEL> channels;
	};



	// UI 관련
	struct UIANCHOR_DESC
	{
		UI_ANCHOR eAnchor = { UI_ANCHOR::MC };
		float fOffsetX = {};
		float fOffsetY = {};
		bool bUseAnchoredPos = { false };
	};

	struct UILAYOUT_DESC
	{
		UI_LAYOUT eLayout = { UI_LAYOUT::NONE };
		float fPadding = {};
		float fSpacing = {};
	};

	struct UILAYOUT_SLOT_DESC
	{
		XMFLOAT4 vMargin = {};
		float fDesiredSizeX = {};
		float fDesiredSizeY = {};
	};
}

#endif // Engine_Struct_h__