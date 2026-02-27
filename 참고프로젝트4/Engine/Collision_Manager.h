#pragma once

#include "Base.h"


BEGIN(Engine)

union COLLIDER_ID
{
	struct
	{
		_uint iLeft_ID;
		_uint iRight_ID;
	};
	_ulonglong ID;

	_bool operator<(const COLLIDER_ID& ref)const
	{
		return ID < ref.ID;
	}
};

class CCollider;

class CCollision_Manager final : public CBase
{
	INFO_CLASS(CCollision_Manager, CBase)

private:
	CCollision_Manager();
	virtual ~CCollision_Manager() = default;

public:
	HRESULT Initialize(_uint& iNumLayer);
	void Tick();
	void Update_CollisionGroup(const _uint& iLeftLayer, const _uint& iRightLayer);

	_bool Check_LayerGroup(const _uint& iLeftLayer, const _uint& iRightLayer);

	void Add_Collider(const _uint& iLayer, CCollider* pCollider);
	void Add_LayerGroup(const _uint& iLeftLayer, const _uint& iRightLayer);
	void Remove_DeadCollider();

	void Set_CollisionInfo(map<COLLIDER_ID, _bool>::iterator& iter, CCollider* pLeftCol, CCollider* pRightCol);

public:
	void Clear();

private:
	vector<list<CCollider*>>   m_pColliderList;

	map<COLLIDER_ID, _bool>	   m_mapCollisionInfo;
	map<_uint, CCollider*>	   m_mapColliderID;
	vector<pair<_uint, _uint>> m_vecCheckLayer;

public:
	static CCollision_Manager* Create(_uint& iNumLayer);
	virtual void Free() override;
};

END
