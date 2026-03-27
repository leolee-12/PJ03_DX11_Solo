#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class CTimer_Manager final : public CBase
{
private:
	CTimer_Manager();
	virtual ~CTimer_Manager() = default;

public:
	HRESULT Add_Timer(WNameID strTimerTag);
	_float Compute_Timer(WNameID strTimerTag);


private:
	WNameMap<class CTimer*>	m_Timers;

private:
	class CTimer* Find_Timer(WNameID strTimerTag);

public:
	static CTimer_Manager*	Create();

protected:
	virtual void Free() override;
};

NS_END