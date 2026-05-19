#include "Camera_Shake.h"
#include <cmath>

void CCamera_Shake::Start(_float fPower, _float fFrequency, _float fDuration)
{
	if (fDuration <= 0.f || fPower <= 0.f)
		return;

	m_bActive = true;
	m_fElapsed = 0.f;
	m_fDuration = fDuration;
	m_fPower = fPower;
	m_fFrequency = fFrequency;
}

void CCamera_Shake::Stop()
{
	m_bActive = false;
	m_fElapsed = 0.f;
	m_fDuration = 0.f;
}

_float3 CCamera_Shake::Evaluate(_float fTimeDelta)
{
	if (!m_bActive)
		return _float3();

	m_fElapsed += fTimeDelta;

	if (m_fElapsed >= m_fDuration)
	{
		m_bActive = false;
		return _float3();
	}

	const _float fT = m_fElapsed * m_fFrequency;
	const _float fAttenu = 1.f - (m_fElapsed / m_fDuration);
	const _float fAmp = m_fPower * fAttenu;

	return _float3(
		sinf(fT * m_vSeed.x) * fAmp,
		sinf(fT * m_vSeed.y + 0.7f) * fAmp,
		sinf(fT * m_vSeed.z + 1.3f) * fAmp);
}