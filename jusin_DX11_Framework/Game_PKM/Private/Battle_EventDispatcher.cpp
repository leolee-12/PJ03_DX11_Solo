#include "Battle_EventDispatcher.h"
#include "IBattleEventListener.h"

CBattle_EventDispatcher::CBattle_EventDispatcher()
{
}

HRESULT CBattle_EventDispatcher::Initialize()
{
	m_vListeners.reserve(8);
	return S_OK;
}

HRESULT CBattle_EventDispatcher::Subscribe(IBattleEventListener* pListener)
{
	if (nullptr == pListener)
		return E_FAIL;

	for (auto* pCurrent : m_vListeners)
	{
		if (pCurrent == pListener)
			return S_OK;
	}

	Safe_AddRef(pListener);
	m_vListeners.push_back(pListener);

	return S_OK;
}

void CBattle_EventDispatcher::Unsubscribe(IBattleEventListener* pListener)
{
	if (nullptr == pListener)
		return;

	for (auto iter = m_vListeners.begin(); iter != m_vListeners.end(); ++iter)
	{
		if (*iter == pListener)
		{
			Safe_Release(*iter);
			m_vListeners.erase(iter);
			return;
		}
	}
}

void CBattle_EventDispatcher::Clear()
{
	for (auto& pListener : m_vListeners)
		Safe_Release(pListener);

	m_vListeners.clear();
}

void CBattle_EventDispatcher::Publish(const BATTLE_EVENT_BASE& tEvent)
{
	const auto vSnapshot = m_vListeners;

	for (auto* pListener : vSnapshot)
	{
		if (nullptr == pListener)
			continue;

		pListener->On_BattleEvent(tEvent);
	}
}

CBattle_EventDispatcher* CBattle_EventDispatcher::Create()
{
	CBattle_EventDispatcher* pInstance = new CBattle_EventDispatcher();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CBattle_EventDispatcher");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBattle_EventDispatcher::Free()
{
	Clear();

	__super::Free();
}