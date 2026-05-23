#ifndef Game_PKM_Struct_h__
#define Game_PKM_Struct_h__

NS_BEGIN(Game_PKM)

#pragma region VERTEX & PARTICLE
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
	TEXTURE_SAMPLE_MODE eAtlasLayout = { TEXTURE_SAMPLE_MODE::SINGLE };

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

struct VTXUI_INSTANCE
{
	_float4 vRight;
	_float4 vUp;
	_float4 vTranslation;
	_float4 vColor;
	_float4 vUVTransform;
	_float4 vMaskUVTransform;
	_float4 vParams;

	// vParams.x = mask strength
	// vParams.y = base sample mode
	// vParams.z = mask sample mode
	// vParams.w = render mode

	// vParams.w:
	// 0 = diff only
	// 1 = diff + mask
	// 2 = sub only
	// 3 = sub + mask
};

struct VTXUI_INSTANCE_DESC
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

struct VTXPARTICLE3D_INSTANCE
{
	_float3 vCenter;        // emitter local 좌표
	_float  fSize;
	_float  fRotation;      // 빌보드 평면 내 회전 (radian)
	_float3 vVelocity;      // emitter local 좌표. VELOCITY_ALIGNED 빌보드에서만 사용
	_float4 vColor;         // RGBA
	_float2 vAgeLife;       // (age, lifeTime)
	_float2 _pad1;
	_float4 vAtlasUV;       // (offsetU, offsetV, scaleU, scaleV) - M4 시점 (0,0,1,1) 고정
};
// stride = 12+4 + 4+12 + 16 + 8+8 + 16 = 80 bytes

struct VTXPARTICLE3D_INSTANCE_DESC
{
	static constexpr unsigned int iNumElements = { 7 };
	static constexpr D3D11_INPUT_ELEMENT_DESC Elements[] =
	{
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA,   0},
			{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 12, D3D11_INPUT_PER_VERTEX_DATA,   0},

			{"TEXCOORD", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0,  D3D11_INPUT_PER_INSTANCE_DATA, 1}, // vCenter + fSize
			{"TEXCOORD", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D11_INPUT_PER_INSTANCE_DATA, 1}, // fRotation + _pad0
			{"TEXCOORD", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32, D3D11_INPUT_PER_INSTANCE_DATA, 1}, // vColor
			{"TEXCOORD", 4, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48, D3D11_INPUT_PER_INSTANCE_DATA, 1}, // vAgeLife + _pad1
			{"TEXCOORD", 5, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 64, D3D11_INPUT_PER_INSTANCE_DATA, 1}, // vAtlasUV
	};
};

struct VTXFIELDGRASS_INSTANCE
{
	_float4 vRight;
	_float4 vUp;
	_float4 vLook;
	_float4 vTranslation;
};

struct VTXFIELDGRASS_INSTANCE_DESC
{
	static constexpr unsigned int iNumElements = { 9 };
	static constexpr D3D11_INPUT_ELEMENT_DESC Elements[] =
	{
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"TANGENT",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 44, D3D11_INPUT_PER_VERTEX_DATA, 0},

			{"WORLD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0,  D3D11_INPUT_PER_INSTANCE_DATA, 1},
			{"WORLD", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D11_INPUT_PER_INSTANCE_DATA, 1},
			{"WORLD", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32, D3D11_INPUT_PER_INSTANCE_DATA, 1},
			{"WORLD", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48, D3D11_INPUT_PER_INSTANCE_DATA, 1},
	};
};
#pragma endregion

NS_END

#endif // Game_PKM_Struct_h__