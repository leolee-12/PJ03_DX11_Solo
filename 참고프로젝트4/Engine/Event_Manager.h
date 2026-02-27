#pragma once
#include "Base.h"

BEGIN(Engine)

#define LEVELEVENT_MAKE [&,this](weak_ptr<CBase> pCaller,shared_ptr<CGameObject> pOther)->_bool
#define TICKEVENT_MAKE [&,this](_cref_time fTimeDelta)->_bool

class ENGINE_DLL CEvent_Manager :    public CBase
{
	INFO_CLASS(CEvent_Manager, CBase)

private:
	CEvent_Manager();
	virtual ~CEvent_Manager() = default;

public:
	HRESULT Initialize(_uint iNumLevels);
	void	Tick(_cref_time fTimeDelta);

public:
	void Add_LevelEvent(_uint iLevelIndex,string sEvent, function<_bool(weak_ptr<CBase>,shared_ptr<CGameObject>)> Func);
	void Erase_LevelEvent(_uint iLevelIndex,string sEvent);
	HRESULT Excute_LevelEvent(_uint iLevelIndex,string sEvent, weak_ptr<CBase> _pCaller, shared_ptr<CGameObject> pOther);
	
	void Add_TickEvent(weak_ptr<CBase> _pSubscriber, function<_bool(_cref_time)> Func);
	void Erase_TickEvent(weak_ptr<CBase> _pSubscriber);
	void Clear_TickEvents();

	void Clear(_uint iLevelIndex);
private:
	_uint											m_iNumLevels = { 0 };

	unordered_map < string, vector<function<_bool(weak_ptr<CBase>, shared_ptr<CGameObject>)>>>* m_mapLevelEvent;
	typedef unordered_map < string, vector<function<_bool(weak_ptr<CBase>, shared_ptr<CGameObject>)>>> LEVELEVENTS;

	vector<pair<weak_ptr<CBase>, function<_bool(_cref_time)>>>		m_mapTickEvent;

public:
	static CEvent_Manager* Create(_uint iNumLevels);
	virtual void	Free();
};

END