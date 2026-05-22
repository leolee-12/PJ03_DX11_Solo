#pragma once
#include "Game_PKM_Defines.h"
#include "ParticleCurve.h"
#include "ParticleEmitter.h"
#include <string>

NS_BEGIN(Game_PKM)

enum class EFFECT_SLOT : _ubyte
{
	CENTER,
	HEAD,
	CHEST,
	FOOT,
	MUZZLE,
	END
};

enum class EFFECT_VFX_TARGET : _ubyte
{
	ATTACKER,
	DEFENDER,
	END
};

struct EMITTER_DEFINITION
{
	/* 식별 */
	_string strName = "";

	/* 입자 풀 / spawn */
	_uint   iCapacity = 256;
	_float  fSpawnRate = 50.f;
	_uint   iBurstCount = 0;

	/* 초기 분포 */
	_float2 vLifeTimeRange = { 1.f, 2.f };
	_float2 vSpeedRange = { 1.f, 3.f };
	_float2 vSizeRange = { 0.2f, 0.5f };
	_float2 vRotationRange = { 0.f, 0.f };
	_float2 vRotationSpeedRange = { 0.f, 0.f };
	_float3 vEmitDirection = { 0.f, 1.f, 0.f };
	_float  fEmitConeHalfAngle = 0.f;

	/* 렌더 모드 */
	BILLBOARD_MODE eBillboard = BILLBOARD_MODE::VIEW_ALIGNED;
	_float3 vBillboardFixedAxis = { 0.f, 1.f, 0.f };

	BLEND_MODE eBlend = BLEND_MODE::ADDITIVE;

	_bool bAutoDestroyOnEmpty = true;
	_float fStartDelay = 0.f;

	_uint  iAtlasCols = 1;
	_uint  iAtlasRows = 1;
	_float fAtlasFps = 0.f;
	_bool  bAtlasLoop = false;
	_bool  bMirrorUV = false;
	_bool  bIgnoreDepth = false;

	/* 시간 커브 */
	CCurveFloat  curveSize;
	CCurveColor  curveColor;
	CCurveFloat  curveAlpha;

	WNameID strTextureProtoTag = PROTO_COM_TEX_DUMMY_WHITE;
};

struct MESH_EFFECT_DEFINITION
{
	enum class SCALE_AXIS { Z_ONLY, UNIFORM };

	_string strName = "";

	/* INVALID_TAG 기본값은 g_EffectMeshOptions[]의 "None" 행과 매칭된다.
	   strShaderProtoTag INVALID_TAG 시 CEffect_Mesh::Initialize가 PROTO_COM_SHADER_EFFECT_BEAM으로
fallback. */
	WNameID strModelProtoTag = { INVALID_TAG };
	WNameID strShaderProtoTag = { INVALID_TAG };
	WNameID strTextureProtoTag = PROTO_COM_TEX_DUMMY_WHITE;

	BLEND_MODE eBlend = BLEND_MODE::ADDITIVE;
	_bool   bIgnoreDepth = false;

	SCALE_AXIS  eScaleAxis = SCALE_AXIS::Z_ONLY;

	CCurveFloat curveScale;
	CCurveColor curveColor;
	CCurveFloat curveAlpha;

	_float  fLifeTime = 1.f;
};

struct EFFECT_DEFINITION
{
	_string strID = "";
	vector<EMITTER_DEFINITION>     Emitters;
	vector<MESH_EFFECT_DEFINITION> Meshes;
};

struct EFFECT_TEXTURE_OPTION
{
	const char* pLabel;
	WNameID strTag;
	const char* pProtoTag;
	const _tchar* pTextureFilePath;
	const _tchar* pDebugName;
};

struct EFFECT_MESH_OPTION
{
	const char* pLabel;
	WNameID      strTag;
	const char* pProtoTag;
	const _char* pModelFilePath;   /* CModel::Create는 narrow path */
	const _tchar* pDebugName;
};

#define EFFECT_TEXTURE_OPTION_ROW(label, tag, path, proto) \
        { label, tag, proto, path, TEXT(proto) }

inline constexpr EFFECT_TEXTURE_OPTION g_EffectTextureOptions[] =
{
	EFFECT_TEXTURE_OPTION_ROW("Dummy White", PROTO_COM_TEX_DUMMY_WHITE, nullptr, "Prototype_Component_Texture_Dummy_White"),

	EFFECT_TEXTURE_OPTION_ROW("Ball Absorb 0 Line702 Sml", PROTO_COM_TEX_EFT_BALL_ABSORB_0_LINE702_SML_O,
		TEXT("../../Resources/Effects/Ball_absorb/Ball_absorb_0_line702_sml_o.png"), "Prototype_Component_Texture_Effect_Ball_Absorb_0_Line702_Sml_O"),
	EFFECT_TEXTURE_OPTION_ROW("Ball Absorb 0 Mask702", PROTO_COM_TEX_EFT_BALL_ABSORB_0_MASK702_O,
		TEXT("../../Resources/Effects/Ball_absorb/Ball_absorb_0_mask702_o.png"), "Prototype_Component_Texture_Effect_Ball_Absorb_0_Mask702_O"),
	EFFECT_TEXTURE_OPTION_ROW("Ball Absorb 1 Circle004 Sml", PROTO_COM_TEX_EFT_BALL_ABSORB_1_CIRCLE004_SML_M,
		TEXT("../../Resources/Effects/Ball_absorb/Ball_absorb_1_circle004_sml_m.png"), "Prototype_Component_Texture_Effect_Ball_Absorb_1_Circle004_Sml_M"),
	EFFECT_TEXTURE_OPTION_ROW("Ball Absorb 1 Circle005 Sml", PROTO_COM_TEX_EFT_BALL_ABSORB_1_CIRCLE005_SML_M,
		TEXT("../../Resources/Effects/Ball_absorb/Ball_absorb_1_circle005_sml_m.png"), "Prototype_Component_Texture_Effect_Ball_Absorb_1_Circle005_Sml_M"),
	EFFECT_TEXTURE_OPTION_ROW("Ball Absorb 2 Fire003", PROTO_COM_TEX_EFT_BALL_ABSORB_2_FIRE003_M,
		TEXT("../../Resources/Effects/Ball_absorb/Ball_absorb_2_fire003_m.png"), "Prototype_Component_Texture_Effect_Ball_Absorb_2_Fire003_M"),
	EFFECT_TEXTURE_OPTION_ROW("Ball Absorb 2 Hit011", PROTO_COM_TEX_EFT_BALL_ABSORB_2_HIT011_M,
		TEXT("../../Resources/Effects/Ball_absorb/Ball_absorb_2_hit011_m.png"), "Prototype_Component_Texture_Effect_Ball_Absorb_2_Hit011_M"),
	EFFECT_TEXTURE_OPTION_ROW("Ball Absorb 3 Flow701", PROTO_COM_TEX_EFT_BALL_ABSORB_3_FLOW701_O,
		TEXT("../../Resources/Effects/Ball_absorb/Ball_absorb_3_flow701_o.png"), "Prototype_Component_Texture_Effect_Ball_Absorb_3_Flow701_O"),

	EFFECT_TEXTURE_OPTION_ROW("Ball Hit 0 Mask702", PROTO_COM_TEX_EFT_BALL_HIT_0_MASK702_O,
		TEXT("../../Resources/Effects/Ball_hit/fxpt_0_mask702_o.png"), "Prototype_Component_Texture_Effect_Ball_Hit_0_Mask702_O"),
	EFFECT_TEXTURE_OPTION_ROW("Ball Hit 0 Smoke002", PROTO_COM_TEX_EFT_BALL_HIT_0_SMOKE002_M,
		TEXT("../../Resources/Effects/Ball_hit/fxpt_0_smoke002_m.png"), "Prototype_Component_Texture_Effect_Ball_Hit_0_Smoke002_M"),
	EFFECT_TEXTURE_OPTION_ROW("Ball Hit 0 Smoke005", PROTO_COM_TEX_EFT_BALL_HIT_0_SMOKE005_M,
		TEXT("../../Resources/Effects/Ball_hit/fxpt_0_smoke005_m.png"), "Prototype_Component_Texture_Effect_Ball_Hit_0_Smoke005_M"),
	EFFECT_TEXTURE_OPTION_ROW("Ball Hit 0 Smoke203", PROTO_COM_TEX_EFT_BALL_HIT_0_SMOKE203_A,
		TEXT("../../Resources/Effects/Ball_hit/fxpt_0_smoke203_a.png"), "Prototype_Component_Texture_Effect_Ball_Hit_0_Smoke203_A"),
	EFFECT_TEXTURE_OPTION_ROW("Ball Hit 0 Smoke702", PROTO_COM_TEX_EFT_BALL_HIT_0_SMOKE702_O,
		TEXT("../../Resources/Effects/Ball_hit/fxpt_0_smoke702_o.png"), "Prototype_Component_Texture_Effect_Ball_Hit_0_Smoke702_O"),
	EFFECT_TEXTURE_OPTION_ROW("Ball Hit 1 Circle003 Sml", PROTO_COM_TEX_EFT_BALL_HIT_1_CIRCLE003_SML_M,
		TEXT("../../Resources/Effects/Ball_hit/fxpt_1_circle003_sml_m.png"), "Prototype_Component_Texture_Effect_Ball_Hit_1_Circle003_Sml_M"),
	EFFECT_TEXTURE_OPTION_ROW("Ball Hit 1 Flash001 Sml", PROTO_COM_TEX_EFT_BALL_HIT_1_FLASH001_SML_M,
		TEXT("../../Resources/Effects/Ball_hit/fxpt_1_flash001_sml_m.png"), "Prototype_Component_Texture_Effect_Ball_Hit_1_Flash001_Sml_M"),

	EFFECT_TEXTURE_OPTION_ROW("Capture Failed 0 Blur003 Sml", PROTO_COM_TEX_EFT_CAPTURE_FAILED_0_BLUR003_SML_M,
		TEXT("../../Resources/Effects/Capture_failed/fxpt_0_blur003_sml_m.png"), "Prototype_Component_Texture_Effect_Capture_Failed_0_Blur003_Sml_M"),
	EFFECT_TEXTURE_OPTION_ROW("Capture Failed 0 Line701", PROTO_COM_TEX_EFT_CAPTURE_FAILED_0_LINE701_O,
		TEXT("../../Resources/Effects/Capture_failed/fxpt_0_line701_o.png"), "Prototype_Component_Texture_Effect_Capture_Failed_0_Line701_O"),
	EFFECT_TEXTURE_OPTION_ROW("Capture Failed 0 Mask702", PROTO_COM_TEX_EFT_CAPTURE_FAILED_0_MASK702_O,
		TEXT("../../Resources/Effects/Capture_failed/fxpt_0_mask702_o.png"), "Prototype_Component_Texture_Effect_Capture_Failed_0_Mask702_O"),
	EFFECT_TEXTURE_OPTION_ROW("Capture Failed 0 Smoke702", PROTO_COM_TEX_EFT_CAPTURE_FAILED_0_SMOKE702_O,
		TEXT("../../Resources/Effects/Capture_failed/fxpt_0_smoke702_o.png"), "Prototype_Component_Texture_Effect_Capture_Failed_0_Smoke702_O"),
	EFFECT_TEXTURE_OPTION_ROW("Capture Failed 1 Circle004 Sml", PROTO_COM_TEX_EFT_CAPTURE_FAILED_1_CIRCLE004_SML_M,
		TEXT("../../Resources/Effects/Capture_failed/fxpt_1_circle004_sml_m.png"), "Prototype_Component_Texture_Effect_Capture_Failed_1_Circle004_Sml_M"),
	EFFECT_TEXTURE_OPTION_ROW("Capture Failed 1 Circle005 Sml", PROTO_COM_TEX_EFT_CAPTURE_FAILED_1_CIRCLE005_SML_M,
		TEXT("../../Resources/Effects/Capture_failed/fxpt_1_circle005_sml_m.png"), "Prototype_Component_Texture_Effect_Capture_Failed_1_Circle005_Sml_M"),
	EFFECT_TEXTURE_OPTION_ROW("Capture Failed 2 Dust701", PROTO_COM_TEX_EFT_CAPTURE_FAILED_2_DUST701_O,
		TEXT("../../Resources/Effects/Capture_failed/fxpt_2_dust701_o.png"), "Prototype_Component_Texture_Effect_Capture_Failed_2_Dust701_O"),
	EFFECT_TEXTURE_OPTION_ROW("Capture Failed 2 Hit011", PROTO_COM_TEX_EFT_CAPTURE_FAILED_2_HIT011_M,
		TEXT("../../Resources/Effects/Capture_failed/fxpt_2_hit011_m.png"), "Prototype_Component_Texture_Effect_Capture_Failed_2_Hit011_M"),
	EFFECT_TEXTURE_OPTION_ROW("Capture Failed 2 Smoke703", PROTO_COM_TEX_EFT_CAPTURE_FAILED_2_SMOKE703_O,
		TEXT("../../Resources/Effects/Capture_failed/fxpt_2_smoke703_o.png"), "Prototype_Component_Texture_Effect_Capture_Failed_2_Smoke703_O"),
	EFFECT_TEXTURE_OPTION_ROW("Capture Failed 2 Water009", PROTO_COM_TEX_EFT_CAPTURE_FAILED_2_WATER009_M,
		TEXT("../../Resources/Effects/Capture_failed/fxpt_2_water009_m.png"), "Prototype_Component_Texture_Effect_Capture_Failed_2_Water009_M"),

	EFFECT_TEXTURE_OPTION_ROW("Capture Hit 0 Line702", PROTO_COM_TEX_EFT_CAPTURE_HIT_0_LINE702_O,
		TEXT("../../Resources/Effects/Capture_hit/fxpt_0_line702_o.png"), "Prototype_Component_Texture_Effect_Capture_Hit_0_Line702_O"),
	EFFECT_TEXTURE_OPTION_ROW("Capture Hit 0 Mask702", PROTO_COM_TEX_EFT_CAPTURE_HIT_0_MASK702_O,
		TEXT("../../Resources/Effects/Capture_hit/fxpt_0_mask702_o.png"), "Prototype_Component_Texture_Effect_Capture_Hit_0_Mask702_O"),
	EFFECT_TEXTURE_OPTION_ROW("Capture Hit 0 Mask703", PROTO_COM_TEX_EFT_CAPTURE_HIT_0_MASK703_O,
		TEXT("../../Resources/Effects/Capture_hit/fxpt_0_mask703_o.png"), "Prototype_Component_Texture_Effect_Capture_Hit_0_Mask703_O"),
	EFFECT_TEXTURE_OPTION_ROW("Capture Hit 0 Smoke702", PROTO_COM_TEX_EFT_CAPTURE_HIT_0_SMOKE702_O,
		TEXT("../../Resources/Effects/Capture_hit/fxpt_0_smoke702_o.png"), "Prototype_Component_Texture_Effect_Capture_Hit_0_Smoke702_O"),
	EFFECT_TEXTURE_OPTION_ROW("Capture Hit 1 Circle005", PROTO_COM_TEX_EFT_CAPTURE_HIT_1_CIRCLE005_M,
		TEXT("../../Resources/Effects/Capture_hit/fxpt_1_circle005_m.png"), "Prototype_Component_Texture_Effect_Capture_Hit_1_Circle005_M"),
	EFFECT_TEXTURE_OPTION_ROW("Capture Hit 1 Flash001 Sml", PROTO_COM_TEX_EFT_CAPTURE_HIT_1_FLASH001_SML_M,
		TEXT("../../Resources/Effects/Capture_hit/fxpt_1_flash001_sml_m.png"), "Prototype_Component_Texture_Effect_Capture_Hit_1_Flash001_Sml_M"),
	EFFECT_TEXTURE_OPTION_ROW("Capture Hit 2 Dust701", PROTO_COM_TEX_EFT_CAPTURE_HIT_2_DUST701_O,
		TEXT("../../Resources/Effects/Capture_hit/fxpt_2_dust701_o.png"), "Prototype_Component_Texture_Effect_Capture_Hit_2_Dust701_O"),
	EFFECT_TEXTURE_OPTION_ROW("Capture Hit 2 Smoke001", PROTO_COM_TEX_EFT_CAPTURE_HIT_2_SMOKE001_M,
		TEXT("../../Resources/Effects/Capture_hit/fxpt_2_smoke001_m.png"), "Prototype_Component_Texture_Effect_Capture_Hit_2_Smoke001_M"),
	EFFECT_TEXTURE_OPTION_ROW("Capture Hit 2 Transform001", PROTO_COM_TEX_EFT_CAPTURE_HIT_2_TRANSFORM001_M,
		TEXT("../../Resources/Effects/Capture_hit/fxpt_2_transform001_m.png"), "Prototype_Component_Texture_Effect_Capture_Hit_2_Transform001_M"),
	EFFECT_TEXTURE_OPTION_ROW("Capture Hit 3 Cloud001", PROTO_COM_TEX_EFT_CAPTURE_HIT_3_CLOUD001_M,
		TEXT("../../Resources/Effects/Capture_hit/fxpt_3_cloud001_m.png"), "Prototype_Component_Texture_Effect_Capture_Hit_3_Cloud001_M"),

	EFFECT_TEXTURE_OPTION_ROW("Capture Success 0 Line701", PROTO_COM_TEX_EFT_CAPTURE_SUCCESS_0_LINE701_O,
		TEXT("../../Resources/Effects/Capture_success/fxpt_0_line701_o.png"), "Prototype_Component_Texture_Effect_Capture_Success_0_Line701_O"),
	EFFECT_TEXTURE_OPTION_ROW("Capture Success 0 Mask003", PROTO_COM_TEX_EFT_CAPTURE_SUCCESS_0_MASK003_M,
		TEXT("../../Resources/Effects/Capture_success/fxpt_0_mask003_m.png"), "Prototype_Component_Texture_Effect_Capture_Success_0_Mask003_M"),
	EFFECT_TEXTURE_OPTION_ROW("Capture Success 0 Ma Star001 Sml", PROTO_COM_TEX_EFT_CAPTURE_SUCCESS_0_MA_STAR001_SML_M,
		TEXT("../../Resources/Effects/Capture_success/fxpt_0_ma_star001_sml_m.png"), "Prototype_Component_Texture_Effect_Capture_Success_0_Ma_Star001_Sml_M"),
	EFFECT_TEXTURE_OPTION_ROW("Capture Success 1 Circle001 Sml", PROTO_COM_TEX_EFT_CAPTURE_SUCCESS_1_CIRCLE001_SML_M,
		TEXT("../../Resources/Effects/Capture_success/fxpt_1_circle001_sml_m.png"), "Prototype_Component_Texture_Effect_Capture_Success_1_Circle001_Sml_M"),

	EFFECT_TEXTURE_OPTION_ROW("Hit 0 Line701", PROTO_COM_TEX_EFT_HIT_0_LINE701_O,
		TEXT("../../Resources/Effects/Hit/fxpt_0_line701_o.png"), "Prototype_Component_Texture_Effect_Hit_0_Line701_O"),
	EFFECT_TEXTURE_OPTION_ROW("Hit 0 Ring001", PROTO_COM_TEX_EFT_HIT_0_RING001_M,
		TEXT("../../Resources/Effects/Hit/fxpt_0_ring001_m.png"), "Prototype_Component_Texture_Effect_Hit_0_Ring001_M"),
	EFFECT_TEXTURE_OPTION_ROW("Hit 1 Circle002", PROTO_COM_TEX_EFT_HIT_1_CIRCLE002_M,
		TEXT("../../Resources/Effects/Hit/fxpt_1_circle002_m.png"), "Prototype_Component_Texture_Effect_Hit_1_Circle002_M"),
	EFFECT_TEXTURE_OPTION_ROW("Hit 1 Circle004", PROTO_COM_TEX_EFT_HIT_1_CIRCLE004_M,
		TEXT("../../Resources/Effects/Hit/fxpt_1_circle004_m.png"), "Prototype_Component_Texture_Effect_Hit_1_Circle004_M"),
	EFFECT_TEXTURE_OPTION_ROW("Hit 2 Hit014", PROTO_COM_TEX_EFT_HIT_2_HIT014_M,
		TEXT("../../Resources/Effects/Hit/fxpt_2_hit014_m.png"), "Prototype_Component_Texture_Effect_Hit_2_Hit014_M"),
	EFFECT_TEXTURE_OPTION_ROW("Hit 2 Wind001", PROTO_COM_TEX_EFT_HIT_2_WIND001_M,
		TEXT("../../Resources/Effects/Hit/fxpt_2_wind001_m.png"), "Prototype_Component_Texture_Effect_Hit_2_Wind001_M"),

	EFFECT_TEXTURE_OPTION_ROW("Thunder 2 Hit009", PROTO_COM_TEX_EFT_THUNDER_2_HIT009_M,
		TEXT("../../Resources/Effects/Thunder/fxpt_2_hit009_m.png"), "Prototype_Component_Texture_Effect_Thunder_2_Hit009_M"),
	EFFECT_TEXTURE_OPTION_ROW("Thunder 2 Hit014", PROTO_COM_TEX_EFT_THUNDER_2_HIT014_M,
		TEXT("../../Resources/Effects/Thunder/fxpt_2_hit014_m.png"), "Prototype_Component_Texture_Effect_Thunder_2_Hit014_M"),
	EFFECT_TEXTURE_OPTION_ROW("Thunder 2 Thunder009", PROTO_COM_TEX_EFT_THUNDER_2_THUNDER009_M,
		TEXT("../../Resources/Effects/Thunder/fxpt_2_thunder009_m.png"), "Prototype_Component_Texture_Effect_Thunder_2_Thunder009_M"),
	EFFECT_TEXTURE_OPTION_ROW("Thunder 2 Thunder010", PROTO_COM_TEX_EFT_THUNDER_2_THUNDER010_M,
		TEXT("../../Resources/Effects/Thunder/fxpt_2_thunder010_m.png"), "Prototype_Component_Texture_Effect_Thunder_2_Thunder010_M"),
	EFFECT_TEXTURE_OPTION_ROW("Thunder 2 Thunder702", PROTO_COM_TEX_EFT_THUNDER_2_THUNDER702_O,
		TEXT("../../Resources/Effects/Thunder/fxpt_2_thunder702_o.png"), "Prototype_Component_Texture_Effect_Thunder_2_Thunder702_O"),
	EFFECT_TEXTURE_OPTION_ROW("Thunder 2 Thunder703", PROTO_COM_TEX_EFT_THUNDER_2_THUNDER703_O,
		TEXT("../../Resources/Effects/Thunder/fxpt_2_thunder703_o.png"), "Prototype_Component_Texture_Effect_Thunder_2_Thunder703_O"),
};

#undef EFFECT_TEXTURE_OPTION_ROW

#define EFFECT_MESH_OPTION_ROW(label, tag, path, proto) \
          { label, tag, proto, path, TEXT(proto) }

/* "None" 행은 MESH_EFFECT_DEFINITION::strModelProtoTag = INVALID_TAG 기본값과 매칭되는 placeholder.
   실제 빔/실드 mesh 행은 §4-E에서 g_EffectMeshOptions[]에 추가한다. */
inline constexpr EFFECT_MESH_OPTION g_EffectMeshOptions[] =
{
	EFFECT_MESH_OPTION_ROW("None", INVALID_TAG, nullptr, ""),

	EFFECT_MESH_OPTION_ROW("At Beam", PROTO_COM_MODEL_STATIC_AT_BEAM, "../../Resources/Models/StaticMeshs/at_beam.wmodel",
		"Prototype_Component_Model_Static_At_Beam"),
	EFFECT_MESH_OPTION_ROW("Ball", PROTO_COM_MODEL_STATIC_BALL, "../../Resources/Models/StaticMeshs/ball.wmodel",
		"Prototype_Component_Model_Static_Ball"),
	EFFECT_MESH_OPTION_ROW("Balloon", PROTO_COM_MODEL_STATIC_BALLOON, "../../Resources/Models/StaticMeshs/balloon.wmodel",
		"Prototype_Component_Model_Static_Balloon"),
	EFFECT_MESH_OPTION_ROW("Cone", PROTO_COM_MODEL_STATIC_CONE, "../../Resources/Models/StaticMeshs/cone.wmodel",
		"Prototype_Component_Model_Static_Cone"),
	EFFECT_MESH_OPTION_ROW("Cube", PROTO_COM_MODEL_STATIC_CUBE, "../../Resources/Models/StaticMeshs/cube.wmodel",
		"Prototype_Component_Model_Static_Cube"),
	EFFECT_MESH_OPTION_ROW("Cylinder", PROTO_COM_MODEL_STATIC_CYLINDER, "../../Resources/Models/StaticMeshs/cylinder.wmodel",
		"Prototype_Component_Model_Static_Cylinder"),
	EFFECT_MESH_OPTION_ROW("Df Beam", PROTO_COM_MODEL_STATIC_DF_BEAM, "../../Resources/Models/StaticMeshs/df_beam.wmodel",
		"Prototype_Component_Model_Static_Df_Beam"),
	EFFECT_MESH_OPTION_ROW("Ring", PROTO_COM_MODEL_STATIC_RING, "../../Resources/Models/StaticMeshs/ring.wmodel",
		"Prototype_Component_Model_Static_Ring"),
	EFFECT_MESH_OPTION_ROW("Sonic", PROTO_COM_MODEL_STATIC_SONIC, "../../Resources/Models/StaticMeshs/sonic.wmodel",
		"Prototype_Component_Model_Static_Sonic"),
	EFFECT_MESH_OPTION_ROW("Sphere", PROTO_COM_MODEL_STATIC_SPHERE, "../../Resources/Models/StaticMeshs/sphere.wmodel",
		"Prototype_Component_Model_Static_Sphere"),
	EFFECT_MESH_OPTION_ROW("Stone", PROTO_COM_MODEL_STATIC_STONE, "../../Resources/Models/StaticMeshs/stone.wmodel",
		"Prototype_Component_Model_Static_Stone"),
	EFFECT_MESH_OPTION_ROW("Surf", PROTO_COM_MODEL_STATIC_SURF, "../../Resources/Models/StaticMeshs/surf.wmodel",
		"Prototype_Component_Model_Static_Surf"),
};

#undef EFFECT_MESH_OPTION_ROW

inline const EFFECT_TEXTURE_OPTION* Effect_FindTextureOption(WNameID strTag)
{
	for (const auto& option : g_EffectTextureOptions)
	{
		if (option.strTag == strTag)
			return &option;
	}

	return nullptr;
}

inline const EFFECT_MESH_OPTION* Effect_FindMeshOption(WNameID strTag)
{
	for (const auto& option : g_EffectMeshOptions)
	{
		if (option.strTag == strTag)
			return &option;
	}
	return nullptr;
}

/* helper: EMITTER_DEFINITION → CParticleEmitter::EMITTER_DESC 변환.
   vSpawnPos는 CEffect가 root 위치를 결정하여 emitter들에 전달. */
inline CParticleEmitter::EMITTER_DESC Make_EmitterDesc(const EMITTER_DEFINITION& def, const _float3& vSpawnPos)
{
	CParticleEmitter::EMITTER_DESC desc{};
	desc.vSpawnPos = vSpawnPos;
	desc.iCapacity = def.iCapacity;
	desc.fSpawnRate = def.fSpawnRate;
	desc.iBurstCount = def.iBurstCount;
	desc.vLifeTimeRange = def.vLifeTimeRange;
	desc.vSpeedRange = def.vSpeedRange;
	desc.vSizeRange = def.vSizeRange;
	desc.vRotationRange = def.vRotationRange;
	desc.vRotationSpeedRange = def.vRotationSpeedRange;
	desc.vEmitDirection = def.vEmitDirection;
	desc.fEmitConeHalfAngle = def.fEmitConeHalfAngle;
	desc.eBillboard = def.eBillboard;
	desc.vBillboardFixedAxis = def.vBillboardFixedAxis;
	desc.eBlend = def.eBlend;
	desc.bAutoDestroyOnEmpty = def.bAutoDestroyOnEmpty;
	desc.fStartDelay = def.fStartDelay;
	desc.iAtlasCols = def.iAtlasCols;
	desc.iAtlasRows = def.iAtlasRows;
	desc.fAtlasFps = def.fAtlasFps;
	desc.bAtlasLoop = def.bAtlasLoop;
	desc.bMirrorUV = def.bMirrorUV;
	desc.bIgnoreDepth = def.bIgnoreDepth;
	desc.strTextureProtoTag = def.strTextureProtoTag;
	desc.curveSize = def.curveSize;
	desc.curveColor = def.curveColor;
	desc.curveAlpha = def.curveAlpha;
	return desc;
}

inline _string Effect_BillboardToString(BILLBOARD_MODE eMode)
{
	switch (eMode)
	{
	case BILLBOARD_MODE::AXIS_LOCKED:
		return "AXIS_LOCKED";
	case BILLBOARD_MODE::FIXED_NORMAL:
		return "FIXED_NORMAL";
	case BILLBOARD_MODE::VELOCITY_ALIGNED:
		return "VELOCITY_ALIGNED";
	default:
		return "VIEW_ALIGNED";
	}
}

inline BILLBOARD_MODE Effect_BillboardFromString(const _string& strValue)
{
	if (strValue == "AXIS_LOCKED")
		return BILLBOARD_MODE::AXIS_LOCKED;
	if (strValue == "FIXED_NORMAL")
		return BILLBOARD_MODE::FIXED_NORMAL;
	if (strValue == "VELOCITY_ALIGNED")
		return BILLBOARD_MODE::VELOCITY_ALIGNED;

	return BILLBOARD_MODE::VIEW_ALIGNED;
}

inline _string Effect_BlendToString(BLEND_MODE eMode)
{
	return (eMode == BLEND_MODE::ALPHA)
		? "ALPHA"
		: "ADDITIVE";
}

inline BLEND_MODE Effect_BlendFromString(const _string& strValue)
{
	return (strValue == "ALPHA")
		? BLEND_MODE::ALPHA
		: BLEND_MODE::ADDITIVE;
}

inline _string Effect_ScaleAxisToString(MESH_EFFECT_DEFINITION::SCALE_AXIS eAxis)
{
	return (MESH_EFFECT_DEFINITION::SCALE_AXIS::UNIFORM == eAxis)
		? "UNIFORM"
		: "Z_ONLY";
}

inline MESH_EFFECT_DEFINITION::SCALE_AXIS Effect_ScaleAxisFromString(const _string& strValue)
{
	return ("UNIFORM" == strValue)
		? MESH_EFFECT_DEFINITION::SCALE_AXIS::UNIFORM
		: MESH_EFFECT_DEFINITION::SCALE_AXIS::Z_ONLY;
}

inline json Effect_SerializeCurveFloat(const CCurveFloat& curve)
{
	json jKeys = json::array();

	for (const auto& key : curve.Get_Keys())
		jKeys.push_back({ key.t, key.v });

	return jKeys;
}

inline json Effect_SerializeCurveColor(const CCurveColor& curve)
{
	json jKeys = json::array();

	for (const auto& key : curve.Get_Keys())
		jKeys.push_back({ key.t, { key.v.x, key.v.y, key.v.z, key.v.w } });

	return jKeys;
}

inline void Effect_ParseCurveFloat(const json& jKeys, CCurveFloat& outCurve)
{
	outCurve.Clear();

	if (!jKeys.is_array())
		return;

	for (const auto& jk : jKeys)
	{
		if (!jk.is_array() || jk.size() < 2)
			continue;

		const _float fTime = jk[0].get<_float>();
		const _float fValue = jk[1].get<_float>();

		outCurve.Add_Key(fTime, fValue);
	}
}

inline void Effect_ParseCurveColor(const json& jKeys, CCurveColor& outCurve)
{
	outCurve.Clear();

	if (!jKeys.is_array())
		return;

	for (const auto& jk : jKeys)
	{
		if (!jk.is_array() || jk.size() < 2)
			continue;
		if (!jk[1].is_array() || jk[1].size() < 4)
			continue;

		const _float fTime = jk[0].get<_float>();
		const _float4 vValue = _float4(
			jk[1][0].get<_float>(),
			jk[1][1].get<_float>(),
			jk[1][2].get<_float>(),
			jk[1][3].get<_float>());

		outCurve.Add_Key(fTime, vValue);
	}
}

template <typename T>
inline void Effect_GetOpt(const json& j, const _char* pKey, T& out)
{
	if (j.contains(pKey))
		out = j[pKey].get<T>();
}

inline void Effect_ReadFloat2(const json& j, _float2& out)
{
	if (j.is_array() && j.size() >= 2)
		out = _float2(
			j[0].get<_float>(),
			j[1].get<_float>());
}

inline void Effect_ReadFloat3(const json& j, _float3& out)
{
	if (j.is_array() && j.size() >= 3)
		out = _float3(
			j[0].get<_float>(),
			j[1].get<_float>(),
			j[2].get<_float>());
}

inline HRESULT Effect_ParseEmitterJson(const json& je, EMITTER_DEFINITION& out)
{
	if (!je.is_object())
		return E_FAIL;

	Effect_GetOpt(je, "name", out.strName);
	Effect_GetOpt(je, "capacity", out.iCapacity);
	Effect_GetOpt(je, "spawnRate", out.fSpawnRate);
	Effect_GetOpt(je, "burstCount", out.iBurstCount);

	if (je.contains("lifeTimeRange"))
		Effect_ReadFloat2(je["lifeTimeRange"], out.vLifeTimeRange);
	if (je.contains("speedRange"))
		Effect_ReadFloat2(je["speedRange"], out.vSpeedRange);
	if (je.contains("sizeRange"))
		Effect_ReadFloat2(je["sizeRange"], out.vSizeRange);
	if (je.contains("rotationRange"))
		Effect_ReadFloat2(je["rotationRange"], out.vRotationRange);
	if (je.contains("rotationSpeedRange"))
		Effect_ReadFloat2(je["rotationSpeedRange"], out.vRotationSpeedRange);
	if (je.contains("emitDirection"))
		Effect_ReadFloat3(je["emitDirection"], out.vEmitDirection);

	Effect_GetOpt(je, "emitConeHalfAngle", out.fEmitConeHalfAngle);

	if (je.contains("billboard"))
		out.eBillboard = Effect_BillboardFromString(je["billboard"].get<_string>());
	if (je.contains("billboardFixedAxis"))
		Effect_ReadFloat3(je["billboardFixedAxis"], out.vBillboardFixedAxis);
	if (je.contains("blend"))
		out.eBlend = Effect_BlendFromString(je["blend"].get<_string>());

	Effect_GetOpt(je, "autoDestroyOnEmpty", out.bAutoDestroyOnEmpty);
	Effect_GetOpt(je, "startDelay", out.fStartDelay);
	Effect_GetOpt(je, "atlasCols", out.iAtlasCols);
	Effect_GetOpt(je, "atlasRows", out.iAtlasRows);
	Effect_GetOpt(je, "atlasFps", out.fAtlasFps);
	Effect_GetOpt(je, "atlasLoop", out.bAtlasLoop);
	Effect_GetOpt(je, "mirrorUV", out.bMirrorUV);
	Effect_GetOpt(je, "ignoreDepth", out.bIgnoreDepth);

	if (je.contains("textureProtoTag"))
	{
		const _string strTag = je["textureProtoTag"].get<_string>();
		if (!strTag.empty())
			out.strTextureProtoTag = WNAME(StoW(strTag));
	}

	if (je.contains("curves") && je["curves"].is_object())
	{
		const json& jCurves = je["curves"];

		if (jCurves.contains("size"))
			Effect_ParseCurveFloat(jCurves["size"], out.curveSize);
		if (jCurves.contains("color"))
			Effect_ParseCurveColor(jCurves["color"], out.curveColor);
		if (jCurves.contains("alpha"))
			Effect_ParseCurveFloat(jCurves["alpha"], out.curveAlpha);
	}

	return S_OK;
}

inline HRESULT Effect_ParseMeshEmitterJson(const json& jm, MESH_EFFECT_DEFINITION& out)
{
	if (!jm.is_object())
		return E_FAIL;

	Effect_GetOpt(jm, "name", out.strName);

	if (jm.contains("modelProtoTag"))
	{
		const _string strTag = jm["modelProtoTag"].get<_string>();
		if (!strTag.empty())
			out.strModelProtoTag = WNAME(StoW(strTag));
	}
	if (jm.contains("shaderProtoTag"))
	{
		const _string strTag = jm["shaderProtoTag"].get<_string>();
		if (!strTag.empty())
			out.strShaderProtoTag = WNAME(StoW(strTag));
	}
	if (jm.contains("textureProtoTag"))
	{
		const _string strTag = jm["textureProtoTag"].get<_string>();
		if (!strTag.empty())
			out.strTextureProtoTag = WNAME(StoW(strTag));
	}

	if (jm.contains("blend"))
		out.eBlend = Effect_BlendFromString(jm["blend"].get<_string>());

	Effect_GetOpt(jm, "ignoreDepth", out.bIgnoreDepth);

	if (jm.contains("scaleAxis"))
		out.eScaleAxis = Effect_ScaleAxisFromString(jm["scaleAxis"].get<_string>());

	Effect_GetOpt(jm, "lifeTime", out.fLifeTime);

	if (jm.contains("curves") && jm["curves"].is_object())
	{
		const json& jCurves = jm["curves"];

		if (jCurves.contains("scale"))
			Effect_ParseCurveFloat(jCurves["scale"], out.curveScale);
		if (jCurves.contains("color"))
			Effect_ParseCurveColor(jCurves["color"], out.curveColor);
		if (jCurves.contains("alpha"))
			Effect_ParseCurveFloat(jCurves["alpha"], out.curveAlpha);
	}

	return S_OK;
}

inline HRESULT Effect_ParseDefinitionJson(const json& jRoot, EFFECT_DEFINITION& out)
{
	if (!jRoot.is_object())
		return E_FAIL;

	if (!jRoot.contains("id") || !jRoot["id"].is_string())
		return E_FAIL;

	out = {};
	out.strID = jRoot["id"].get<_string>();

	if (jRoot.contains("emitters") && jRoot["emitters"].is_array())
	{
		for (const auto& je : jRoot["emitters"])
		{
			EMITTER_DEFINITION emitter{};

			if (FAILED(Effect_ParseEmitterJson(je, emitter)))
				continue;

			out.Emitters.push_back(emitter);
		}
	}

	if (jRoot.contains("meshes") && jRoot["meshes"].is_array())
	{
		for (const auto& jm : jRoot["meshes"])
		{
			MESH_EFFECT_DEFINITION mesh{};

			if (FAILED(Effect_ParseMeshEmitterJson(jm, mesh)))
				continue;

			out.Meshes.push_back(mesh);
		}
	}

	return S_OK;
}

inline _string Effect_TextureTagToString(WNameID strTag)
{
	if (const EFFECT_TEXTURE_OPTION* pOption = Effect_FindTextureOption(strTag))
		return pOption->pProtoTag;

#ifdef _DEBUG
	const _string strLookup = WtoS(_wstring(Engine::WNameRegistry::Lookup(strTag)));
	return (strLookup == "<unknown>") ? _string{} : strLookup;
#else
	return {};
#endif
}

inline _string Effect_MeshTagToString(WNameID strTag)
{
	if (const EFFECT_MESH_OPTION* pOption = Effect_FindMeshOption(strTag))
		return (nullptr != pOption->pProtoTag) ? pOption->pProtoTag : _string{};

#ifdef _DEBUG
	const _string strLookup = WtoS(_wstring(Engine::WNameRegistry::Lookup(strTag)));
	return (strLookup == "<unknown>") ? _string{} : strLookup;
#else
	return {};
#endif
}

inline json Effect_SerializeEmitterJson(const EMITTER_DEFINITION& emitter)
{
	json j;
	j["name"] = emitter.strName;
	j["capacity"] = emitter.iCapacity;
	j["spawnRate"] = emitter.fSpawnRate;
	j["burstCount"] = emitter.iBurstCount;
	j["lifeTimeRange"] = { emitter.vLifeTimeRange.x, emitter.vLifeTimeRange.y };
	j["speedRange"] = { emitter.vSpeedRange.x, emitter.vSpeedRange.y };
	j["sizeRange"] = { emitter.vSizeRange.x, emitter.vSizeRange.y };
	j["rotationRange"] = { emitter.vRotationRange.x, emitter.vRotationRange.y };
	j["rotationSpeedRange"] = { emitter.vRotationSpeedRange.x, emitter.vRotationSpeedRange.y };
	j["emitDirection"] = { emitter.vEmitDirection.x, emitter.vEmitDirection.y,
emitter.vEmitDirection.z };
	j["emitConeHalfAngle"] = emitter.fEmitConeHalfAngle;
	j["billboard"] = Effect_BillboardToString(emitter.eBillboard);
	j["billboardFixedAxis"] = { emitter.vBillboardFixedAxis.x, emitter.vBillboardFixedAxis.y,
emitter.vBillboardFixedAxis.z };
	j["blend"] = Effect_BlendToString(emitter.eBlend);
	j["autoDestroyOnEmpty"] = emitter.bAutoDestroyOnEmpty;
	j["startDelay"] = emitter.fStartDelay;
	j["atlasCols"] = emitter.iAtlasCols;
	j["atlasRows"] = emitter.iAtlasRows;
	j["atlasFps"] = emitter.fAtlasFps;
	j["atlasLoop"] = emitter.bAtlasLoop;
	j["mirrorUV"] = emitter.bMirrorUV;
	j["ignoreDepth"] = emitter.bIgnoreDepth;

	const _string strTextureTag = Effect_TextureTagToString(emitter.strTextureProtoTag);
	if (!strTextureTag.empty())
		j["textureProtoTag"] = strTextureTag;

	json jCurves;
	if (!emitter.curveSize.IsEmpty())
		jCurves["size"] = Effect_SerializeCurveFloat(emitter.curveSize);
	if (!emitter.curveColor.IsEmpty())
		jCurves["color"] = Effect_SerializeCurveColor(emitter.curveColor);
	if (!emitter.curveAlpha.IsEmpty())
		jCurves["alpha"] = Effect_SerializeCurveFloat(emitter.curveAlpha);
	if (!jCurves.empty())
		j["curves"] = jCurves;

	return j;
}

inline json Effect_SerializeMeshEmitterJson(const MESH_EFFECT_DEFINITION& mesh)
{
	json j;
	j["name"] = mesh.strName;

	const _string strModelTag = Effect_MeshTagToString(mesh.strModelProtoTag);
	if (!strModelTag.empty())
		j["modelProtoTag"] = strModelTag;

	/* shader / texture는 텍스처 카탈로그 lookup으로 통일.
	   beam 셰이더는 텍스처 카탈로그가 아니므로 fallback registry lookup이 동작해야 release에서도
유실 없음 — §4-E 등록 시 같은 카탈로그 적용. */
	const _string strShaderTag = Effect_TextureTagToString(mesh.strShaderProtoTag);
	if (!strShaderTag.empty())
		j["shaderProtoTag"] = strShaderTag;

	const _string strTextureTag = Effect_TextureTagToString(mesh.strTextureProtoTag);
	if (!strTextureTag.empty())
		j["textureProtoTag"] = strTextureTag;

	j["blend"] = Effect_BlendToString(mesh.eBlend);
	j["ignoreDepth"] = mesh.bIgnoreDepth;
	j["scaleAxis"] = Effect_ScaleAxisToString(mesh.eScaleAxis);
	j["lifeTime"] = mesh.fLifeTime;

	json jCurves;
	if (!mesh.curveScale.IsEmpty())
		jCurves["scale"] = Effect_SerializeCurveFloat(mesh.curveScale);
	if (!mesh.curveColor.IsEmpty())
		jCurves["color"] = Effect_SerializeCurveColor(mesh.curveColor);
	if (!mesh.curveAlpha.IsEmpty())
		jCurves["alpha"] = Effect_SerializeCurveFloat(mesh.curveAlpha);
	if (!jCurves.empty())
		j["curves"] = jCurves;

	return j;
}

inline json Effect_SerializeDefinitionJson(const EFFECT_DEFINITION& def)
{
	json root;
	root["id"] = def.strID;
	root["emitters"] = json::array();

	for (const auto& emitter : def.Emitters)
		root["emitters"].push_back(Effect_SerializeEmitterJson(emitter));

	/* 빈 meshes 배열은 출력하지 않는다 — 기존 emitter-only JSON 재저장 시 회귀 0. */
	if (!def.Meshes.empty())
	{
		root["meshes"] = json::array();
		for (const auto& mesh : def.Meshes)
			root["meshes"].push_back(Effect_SerializeMeshEmitterJson(mesh));
	}

	return root;
}

NS_END