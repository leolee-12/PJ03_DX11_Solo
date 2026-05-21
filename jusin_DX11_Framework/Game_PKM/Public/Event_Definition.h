#pragma once
#include "Base.h"
#include "Event_Defines.h"

NS_BEGIN(Game_PKM)

class CEvent_Definition final : public CBase
{
private:
	CEvent_Definition();
	virtual ~CEvent_Definition() = default;

public:
	HRESULT Initialize(const _wstring& strSequenceID);

	const _wstring& Get_SequenceID() const { return m_strSequenceID; }
	const vector<EVENT_STEP_GROUP>& Get_Groups() const { return m_Groups; }

	void Add_Step(const EVENT_STEP_DESC& tStep);
	void Add_Group(const EVENT_STEP_GROUP& tGroup);

public:
	static CEvent_Definition* Create(const _wstring& strSequenceID);

private:
	_wstring m_strSequenceID;
	vector<EVENT_STEP_GROUP> m_Groups;

private:
	virtual void Free() override;
};

NS_END