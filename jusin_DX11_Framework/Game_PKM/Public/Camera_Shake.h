#pragma once
#include "Game_PKM_Defines.h"

NS_BEGIN(Game_PKM)

class CCamera_Shake final
{
public:
	CCamera_Shake() = default;
	~CCamera_Shake() = default;

	/* fDuration / fPower <= 0 이면 무시. 호출 중 재호출하면 직전 shake 를 덮어쓰고 처음부터. */
	void Start(_float fPower, _float fFrequency, _float fDuration);
	void Stop();

	_bool Is_Active() const { return m_bActive; }

	/* finalPose.vPosition 에 더할 offset 만 반환. m_CurrentPose 는 호출자가 보존(§함정 1). */
	_float3 Evaluate(_float fTimeDelta);

private:
	_bool   m_bActive = false;
	_float  m_fElapsed = 0.f;
	_float  m_fDuration = 0.f;
	_float  m_fPower = 0.f;
	_float  m_fFrequency = 0.f;

	/* 축마다 다른 위상 시드 — 같은 진동수에서도 X/Y/Z 패턴이 달라 자연스러움. */
	_float3 m_vSeed = { 1.0f, 1.7f, 2.3f };
};

NS_END