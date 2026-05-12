#pragma once
#include "Base.h"
#include "Battle_Events.h"

NS_BEGIN(Game_PKM)

class IBattleEventListener;

class CBattle_EventDispatcher final : public CBase
{
private:
	CBattle_EventDispatcher();
	virtual ~CBattle_EventDispatcher() = default;

public:
	HRESULT Initialize();

	HRESULT Subscribe(IBattleEventListener* pListener);
	void Unsubscribe(IBattleEventListener* pListener);
	void Clear();

	void Publish(const BATTLE_EVENT_BASE& tEvent);

	template<typename TEvent>
	void Emit(const TEvent& tEvent)
	{
		Publish(static_cast<const BATTLE_EVENT_BASE&>(tEvent));
	}

private:
	std::vector<IBattleEventListener*> m_vListeners;

public:
	static CBattle_EventDispatcher* Create();

private:
	virtual void Free() override;
};

NS_END