#pragma once

#include "Base.h"

BEGIN(Engine)

class CSoundPoint;

class CSoundPoint_Manager final : public CBase
{
private:
	CSoundPoint_Manager();
	virtual ~CSoundPoint_Manager() = default;

public:
	HRESULT Initialize();
	void	Tick(_cref_time fTimeDelta);
	HRESULT Render();

public:
	HRESULT Add_SoundPoint(const string& strName, const struct TSoundPointDesc& Desc, shared_ptr<CSoundPoint>* ppOut = nullptr );
	HRESULT Add_SoundPoint(const wstring& FilePath, vector<shared_ptr<CSoundPoint>>* SoundPointVector = nullptr);

	_bool Exists_SoundPointName(const string& strName);

	void	Release_SoundPoint(shared_ptr<CSoundPoint> pLight);
	void	Reset_SoundPoint();

	shared_ptr<CSoundPoint>	Make_SoundPoint_On_Owner(weak_ptr<class CGameObject> _pOwner);

private:
	list<shared_ptr<CSoundPoint>>			m_SoundPoints;

public:
	shared_ptr<CSoundPoint> Find_SoundPoint(const string& Name);

	void Clear() { Free(); };

public:
	static CSoundPoint_Manager* Create();
	virtual void Free() override;
};

END