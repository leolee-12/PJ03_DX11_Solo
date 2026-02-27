#include "..\Public\Timer_Manager.h"
#include "Timer.h"

CTimer_Manager::CTimer_Manager()
{
}

HRESULT CTimer_Manager::Add_Timer(const _wstring & strTimerTag)
{
	if (nullptr != Find_Timer(strTimerTag))
		return E_FAIL;

	CTimer*		pTimer = CTimer::Create();
	if (nullptr == pTimer)
		return E_FAIL;

	m_Timers.emplace(strTimerTag, pTimer);

	return S_OK;
}

_float CTimer_Manager::Compute_TimeDelta(const _wstring& strTimerTag)
{
	CTimer* pTimer = Find_Timer(strTimerTag);

	if (nullptr == pTimer)
		return 0.f;

	// 비스케일 델타 타임 계산
	_float fUnscaledDeltaTime = pTimer->Compute_TimeDelta();

	// 시간 스케일 적용된 델타 타임 계산
	_float fScaledDeltaTime = fUnscaledDeltaTime * m_fTimeScale;

	// 타이머에 델타 타임 저장
	pTimer->Set_UnscaledDeltaTime(fUnscaledDeltaTime);
	pTimer->Set_ScaledDeltaTime(fScaledDeltaTime);

	return fScaledDeltaTime;
}

_float CTimer_Manager::Get_UnscaledDeltaTime(const _wstring& strTimerTag)
{
	CTimer* pTimer = Find_Timer(strTimerTag);

	if (nullptr == pTimer)
		return 0.f;

	return pTimer->Get_UnscaledDeltaTime();
}

_float CTimer_Manager::Get_ScaledDeltaTime(const _wstring& strTimerTag)
{
	CTimer* pTimer = Find_Timer(strTimerTag);

	if (nullptr == pTimer)
		return 0.f;

	return pTimer->Get_ScaledDeltaTime();
}

CTimer * CTimer_Manager::Find_Timer(const _wstring & strTimerTag)
{
	auto	iter = m_Timers.find(strTimerTag);
	if (iter == m_Timers.end())
		return nullptr;

	return iter->second;	
}

CTimer_Manager * CTimer_Manager::Create()
{
	return new CTimer_Manager();
}

void CTimer_Manager::Free()
{
	__super::Free();

	for (auto& Pair : m_Timers)
		Safe_Release(Pair.second);

	m_Timers.clear();
}

