#pragma once

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CLayer final : public CBase
{
	INFO_CLASS(CLayer, CBase)

private:
	CLayer();
	virtual ~CLayer() = default;

public:
	HRESULT Add_GameObject( shared_ptr<class CGameObject> pGameObject);
	void Priority_Tick(_cref_time fTimeDelta);
	void Tick(_cref_time fTimeDelta);
	void Late_Tick(_cref_time fTimeDelta);
	void Before_Render(_cref_time fTimeDelta);

public:
	shared_ptr<class CComponent> Get_Component(const wstring& strComponentTag, _uint iIndex, const wstring& strPartTag);

private:
	list< shared_ptr<class CGameObject>>			m_GameObjects;

public:
	void Clear_DeadObjects();

	list< shared_ptr<class CGameObject>>* Get_ObjectList() { return &m_GameObjects; }
	 shared_ptr<class CGameObject> Get_Object(_uint iIndex);
public:
	static CLayer* Create();
	virtual void Free() override;
};

END