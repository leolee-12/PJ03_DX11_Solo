#pragma once

#include "Base.h"

/* 1. 여러개의 타이머를 키로 관리한다. */
/* 2. 지정한 타이머의 타임델타를 계산하도록 하고 리턴한다. */

BEGIN(Engine)

class CTimer_Manager final : public CBase
{
private:
	CTimer_Manager();
	virtual ~CTimer_Manager() = default;

public:
	HRESULT Add_Timer(const _wstring& strTimerTag);
	_float Compute_TimeDelta(const _wstring& strTimerTag);
	void Set_TimeScale(_float fTimeScale) { m_fTimeScale = fTimeScale; }
	_float Get_TimeScale() const { return m_fTimeScale; }
	_float Get_UnscaledDeltaTime(const _wstring& strTimerTag);
	_float Get_ScaledDeltaTime(const _wstring& strTimerTag);

private:
	map<const _wstring, class CTimer*>			m_Timers;

private:
	class CTimer* Find_Timer(const _wstring& strTimeTag);
	_float m_fTimeScale = 1.0f; // 기본 시간 스케일은 정상 속도

public:
	static CTimer_Manager* Create();
	virtual void Free() override;
};

END

