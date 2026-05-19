#pragma once
#include "Game_PKM_Defines.h"
#include "ParticleCurve.h"
#include "ParticleEmitter.h"
#include <string>

NS_BEGIN(Game_PKM)

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
	_float3 vEmitDirection = { 0.f, 1.f, 0.f };
	_float  fEmitConeHalfAngle = 0.f;

	/* 렌더 모드 */
	CParticleEmitter::EMITTER_DESC::BILLBOARD_MODE eBillboard =
		CParticleEmitter::EMITTER_DESC::BILLBOARD_MODE::VIEW_ALIGNED;
	_float3 vBillboardFixedAxis = { 0.f, 1.f, 0.f };

	CParticleEmitter::EMITTER_DESC::BLEND_MODE eBlend =
		CParticleEmitter::EMITTER_DESC::BLEND_MODE::ADDITIVE;

	/* 시간 커브 */
	CCurveFloat  curveSize;
	CCurveColor  curveColor;
	CCurveFloat  curveAlpha;

	/* 텍스처는 prototype tag (Loader 단계에서 등록되어 있어야 함). M7a 시점은
	   Manager가 emitter 생성 시 desc에 옮겨주지 않고 default(흰 더미)를 그대로 사용.
	   텍스처 prototype 선택은 M7b 또는 별도 단위에서 emitter에 노출. */
	WNameID strTextureProtoTag = PROTO_COM_TEX_DUMMY_WHITE;
};

struct EFFECT_DEFINITION
{
	_string strID = "";
	vector<EMITTER_DEFINITION> Emitters;
};

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
	desc.vEmitDirection = def.vEmitDirection;
	desc.fEmitConeHalfAngle = def.fEmitConeHalfAngle;
	desc.eBillboard = def.eBillboard;
	desc.vBillboardFixedAxis = def.vBillboardFixedAxis;
	desc.eBlend = def.eBlend;
	desc.strTextureProtoTag = def.strTextureProtoTag;
	desc.curveSize = def.curveSize;
	desc.curveColor = def.curveColor;
	desc.curveAlpha = def.curveAlpha;
	return desc;
}

NS_END