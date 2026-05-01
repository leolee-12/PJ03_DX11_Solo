#ifndef Game_PKM_Struct_h__
#define Game_PKM_Struct_h__

NS_BEGIN(Game_PKM)

struct PARTICLE_UI_STATE final
{
	_float2 vOffset = {};
	_float2 vVelocity = {};
	_float2 vSize = {};

	_float fAge = {};
	_float fLifeTime = {};
	_float fRotation = {};
	_float fRotationSpeed = {};
	_float fAlpha = { 1.f };

	_float fMaskRotation = {};
	_float fMaskRotationSpeed = {};

	_uint iTextureIndex = {};
	_uint iFrameIndex = {};
	TEXTURE_ATLAS_LAYOUT eAtlasLayout = { TEXTURE_ATLAS_LAYOUT::SINGLE };

	_bool isAlive = { false };
};

struct PARTICLE_3D_STATE final
{
	_float3 vInitPos{};
	_float3 vPos{};
	_float3 vVelocity{};

	_float fSize{};
	_float fAge{};
	_float fLifeTime{};

	_bool isAlive{ false };
};

struct VTXPARTICLE_UI_INSTANCE
{
	XMFLOAT4 vRight;
	XMFLOAT4 vUp;
	XMFLOAT4 vTranslation;
	XMFLOAT4 vColor;
	XMFLOAT4 vUVTransform;
	XMFLOAT4 vMaskUVTransform;
	XMFLOAT4 vParams;

	// vParams.x = mask strength
	// vParams.y = atlas layout
	// vParams.z = frame index
	// vParams.w = use mask
};

struct VTXUI_PARTICLE_INSTANCE_DESC
{
	static constexpr unsigned int iNumElements = { 9 };
	static constexpr D3D11_INPUT_ELEMENT_DESC Elements[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},

		{"TEXCOORD", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"TEXCOORD", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"TEXCOORD", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"TEXCOORD", 4, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"TEXCOORD", 5, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 64, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"TEXCOORD", 6, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 80, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"TEXCOORD", 7, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 96, D3D11_INPUT_PER_INSTANCE_DATA, 1}
	};
};

NS_END

#endif // Game_PKM_Struct_h__