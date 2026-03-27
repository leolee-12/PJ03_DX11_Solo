#include "Timer_Manager.h"
#include "Timer.h"

CTimer_Manager::CTimer_Manager()
{
}

HRESULT CTimer_Manager::Add_Timer(WNameID strTimerTag)
{
	if (nullptr != Find_Timer(strTimerTag))
		return E_FAIL;

	CTimer* pTimer = CTimer::Create();

	if (nullptr == pTimer)
		return E_FAIL;

	m_Timers.emplace(strTimerTag, pTimer);

	return S_OK;
}

_float CTimer_Manager::Compute_Timer(WNameID strTimerTag)
{
	CTimer* pTimer = Find_Timer(strTimerTag);

	if (nullptr == pTimer)
		return 0.f;

	pTimer->Update_Timer();

	return pTimer->Get_TimeDelta();
}

CTimer* CTimer_Manager::Find_Timer(WNameID strTimerTag)
{
	auto pp = m_Timers.find(strTimerTag);

	return pp ? *pp : nullptr;
}

CTimer_Manager* CTimer_Manager::Create()
{
	return new CTimer_Manager();
}

void CTimer_Manager::Free()
{
	__super::Free();

	m_Timers.for_each([](auto& pair) { Safe_Release(pair.second); });
	m_Timers.clear();
}
