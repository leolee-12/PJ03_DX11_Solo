#pragma once
#include "Event_Defines.h"
#include "Event_Context.h"

NS_BEGIN(Game_PKM)
class CEvent_Definition;
class CEventAction;

class CEventSequence_Player final : public CBase
{
private:
	CEventSequence_Player();
	virtual ~CEventSequence_Player() = default;

public:
	HRESULT Initialize(const CEvent_Definition* pSequence, const EVENT_CONTEXT& tContext);

	EVENT_PLAY_STATE Update(_float fTimeDelta);
	void Cancel();

	EVENT_PLAY_STATE Get_State() const { return m_eState; }
	const EVENT_CONTEXT& Get_Context() const { return m_tContext; }
	EVENT_CONTEXT& Get_Context() { return m_tContext; }

public:
	static CEventSequence_Player* Create(const CEvent_Definition* pSequence, const EVENT_CONTEXT& tContext);

private:
	void Release_CurrentAction();

private:
	const CEvent_Definition* m_pSequence = { nullptr };   // weak - EventManager owns definitions
	EVENT_CONTEXT m_tContext{};

	_uint m_iGroupIndex = { 0 };
	_uint m_iStepIndex = { 0 };

	CEventAction* m_pCurrentAction = { nullptr };
	_bool m_bCurrentStarted = { false };

	EVENT_PLAY_STATE m_eState = { EVENT_PLAY_STATE::IDLE };

private:
	virtual void Free() override;
};

NS_END