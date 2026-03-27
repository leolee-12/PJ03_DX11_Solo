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

		static const unsigned int		iNumElements = { 5 };

		static constexpr D3D11_INPUT_ELEMENT_DESC   Elements[] = {
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 44, D3D11_INPUT_PER_VERTEX_DATA, 0},
		};
	};
}

#endif // Engine_Struct_h__