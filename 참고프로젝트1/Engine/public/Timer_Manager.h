#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class CTimer;
class CTimer_Manager final : public CBase
{
private:
	explicit CTimer_Manager();
	virtual ~CTimer_Manager() = default;

public:
	_float			Get_TimeDelta(const _wstring& pTimerTag);

public:
	HRESULT			Add_Timer(const _wstring& pTimerTag);
	void			Update_TimeDelta(const _wstring& pTimerTag);

private:
	CTimer* Find_Timer(const _wstring& pTimerTag);	

private:	
	map<const _wstring, CTimer*>			m_Timers;

public:
	static CTimer_Manager* Create();
	virtual void		Free();
};

NS_END