#pragma once
#include "Game_PKM_Defines.h"

NS_BEGIN(Game_PKM)

struct CParticle final
{
	_float3 vPosition = {};
	_float3 vVelocity = {};
	_float3 vAcceleration = {};

	_float fSize = 1.f;
	_float fSizeStart = 1.f;
	_float fSizeEnd = 1.f;

	_float4 vColor = { 1.f, 1.f, 1.f, 1.f };
	_float4 vColorStart = { 1.f, 1.f, 1.f, 1.f };
	_float4 vColorEnd = { 1.f, 1.f, 1.f, 1.f };

	_float fRotation = 0.f;
	_float fRotationSpeed = 0.f;

	_float fAge = 0.f;
	_float fLifeTime = 1.f;
	_float fRandomSeed = 0.f;

	_uint iAtlasIndex = 0;
};

NS_END