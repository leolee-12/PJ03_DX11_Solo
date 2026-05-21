#pragma once
#include "Event_Defines.h"
#include "Event_Context.h"

NS_BEGIN(Game_PKM)

class CEventAction abstract : public CBase
{
protected:
	CEventAction() = default;
	virtual ~CEventAction() = default;

public:
	virtual EVENT_ACTION_KIND Get_Kind() const = 0;
	virtual HRESULT Initialize(const EVENT_STEP_DESC& tDesc) { return S_OK; }
	virtual HRESULT Start(EVENT_CONTEXT& tContext) = 0;
	virtual EVENT_PLAY_STATE Update(EVENT_CONTEXT& tContext, _float fTimeDelta) = 0;
	virtual void Cancel(EVENT_CONTEXT& tContext) {}
	virtual _bool Is_Blocking() const { return true; }

public:
	static CEventAction* Create_Action(const EVENT_STEP_DESC& tDesc);

protected:
	virtual void Free() override;
};

NS_END