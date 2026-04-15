#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class CLayer final : public CBase
{
private:
	CLayer();
	virtual ~CLayer() = default;

public:
	class CComponent* Get_Component(const WNameID strComponentTag, _uint iIndex);
	const list<class CGameObject*>& Get_ObjectList() const { return m_GameObjects; }

	HRESULT Add_GameObject(class CGameObject* pGameObject);
	void Priority_Update(_float fTimeDelta);
	void Update(_float fTimeDelta);
	void Late_Update(_float fTimeDelta);

private:
	list<class CGameObject*> m_GameObjects;

public:
	static CLayer* Create();

protected:
	virtual void Free() override;
};

NS_END