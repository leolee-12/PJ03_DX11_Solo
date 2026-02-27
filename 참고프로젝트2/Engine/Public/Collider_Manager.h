#pragma once

#include "Base.h"

/* 1. 화면에 그려야할 객체들을 그리는 순서대로 보관한다. */
/* 2. 보관하고 있는 객체들의 렌더함수를 호출한다.(렌더콜) */

BEGIN(Engine)

class CCollider;

class CCollider_Manager final : public CBase
{
public:
	enum COLLIDERGROUP {
		CG_1P_BODY,
		CG_1P_Energy_Attack,
		CG_1P_Ranged_Attack,   // 1P 원거리 공격
		CG_1P_Melee_Attack,    // 1P 근접 공격
		CG_1P_REFLECT,
		CG_2P_BODY,
		CG_2P_Energy_Attack,
		CG_2P_Ranged_Attack,   // 2P 원거리 공격
		CG_2P_Melee_Attack,    // 2P 근접 공격
		CG_2P_REFLECT,
		CG_EFFECT_LAYER,
		CG_END
	};

private:
	CCollider_Manager();
	virtual ~CCollider_Manager() = default;

public:
	void Update(_float fTimeDelta);

public:
	HRESULT Add_ColliderObject(COLLIDERGROUP eRenderGroup, CCollider* pColliderCom);
	HRESULT Check_Collision(_float);

	_bool IsColliding(CCollider* a, CCollider* b);
	HRESULT Release_Collider(const CCollider* Collider);
	HRESULT Destory_ColliderGroup();
	void	Destroy_Reserve(COLLIDERGROUP eRenderGroup);
	void	Destroy_Reserve(CCollider* pCollider);       // 개별 콜라이더 삭제 예약 함수

private:
	void Process_1P_Body_2P_Body(pair<CCollider*, CCollider*> pairCollider, _float fTimeDelta, map<pair<CCollider*, CCollider*>, _bool>& currentCollisions);
	void Process_1P_Energy_Skill_2P_Energy_Skill_Group(const vector<pair<CCollider*, CCollider*>>& collisions, _float fTimeDelta, map<pair<CCollider*, CCollider*>, _bool>& currentCollisions);
	void Process_1P_Body_2P_Energy_Skill_Group(const vector<pair<CCollider*, CCollider*>>& collisions, _float fTimeDelta, map<pair<CCollider*, CCollider*>, _bool>& currentCollisions);
	void Process_1P_Energy_Skill_2P_Body_Group(const vector<pair<CCollider*, CCollider*>>& collisions, _float fTimeDelta, map<pair<CCollider*, CCollider*>, _bool>& currentCollisions);

	void Process_1P_Ranged_Skill_2P_Body(pair<CCollider*, CCollider*> pairCollider, _float fTimeDelta, map<pair<CCollider*, CCollider*>, _bool>& currentCollisions);
	void Process_1P_Ranged_Skill_2P_Energy_Skill_Group(const vector<pair<CCollider*, CCollider*>>& collisions, _float fTimeDelta, map<pair<CCollider*, CCollider*>, _bool>& currentCollisions);
	void Process_1P_Ranged_Skill_2P_Ranged_Skill(pair<CCollider*, CCollider*> pairCollider, _float fTimeDelta, map<pair<CCollider*, CCollider*>, _bool>& currentCollisions);
	void Process_1P_Melee_Skill_2P_Body(pair<CCollider*, CCollider*> pairCollider, _float fTimeDelta, map<pair<CCollider*, CCollider*>, _bool>& currentCollisions);
	void Process_1P_Body_2P_Ranged_Skill(pair<CCollider*, CCollider*> pairCollider, _float fTimeDelta, map<pair<CCollider*, CCollider*>, _bool>& currentCollisions);
	void Process_1P_Energy_Skill_2P_Ranged_Skill_Group(const vector<pair<CCollider*, CCollider*>>& collisions, _float fTimeDelta, map<pair<CCollider*, CCollider*>, _bool>& currentCollisions);
	void Process_1P_Body_2P_Melee_Skill(pair<CCollider*, CCollider*> pairCollider, _float fTimeDelta, map<pair<CCollider*, CCollider*>, _bool>& currentCollisions);
	void Process_1P_Melee_2P_Melee_Skill(pair<CCollider*, CCollider*> pairCollider, _float fTimeDelta, map<pair<CCollider*, CCollider*>, _bool>& currentCollisions);
	void Process_1P_Range_2P_Melee_Skill(pair<CCollider*, CCollider*> pairCollider, _float fTimeDelta, map<pair<CCollider*, CCollider*>, _bool>& currentCollisions);
	void Process_1P_Melee_2P_Range_Skill(pair<CCollider*, CCollider*> pairCollider, _float fTimeDelta, map<pair<CCollider*, CCollider*>, _bool>& currentCollisions);
	void Process_1P_Reflect_2P_Energy_Skill_Group(const vector<pair<CCollider*, CCollider*>>& collisions, _float fTimeDelta, map<pair<CCollider*, CCollider*>, _bool>& currentCollisions);
	void Process_1P_Reflect_2P_Range_Skill(pair<CCollider*, CCollider*> pairCollider, _float fTimeDelta, map<pair<CCollider*, CCollider*>, _bool>& currentCollisions);
	void Process_1P_Reflect_2P_Melee_Skill(pair<CCollider*, CCollider*> pairCollider, _float fTimeDelta, map<pair<CCollider*, CCollider*>, _bool>& currentCollisions);
	void Process_1P_Energy_2P_Reflect_Group(const vector<pair<CCollider*, CCollider*>>& collisions, _float fTimeDelta, map<pair<CCollider*, CCollider*>, _bool>& currentCollisions);
	void Process_1P_Range_2P_Reflect(pair<CCollider*, CCollider*> pairCollider, _float fTimeDelta, map<pair<CCollider*, CCollider*>, _bool>& currentCollisions);
	void Process_1P_Melee_2P_Reflect(pair<CCollider*, CCollider*> pairCollider, _float fTimeDelta, map<pair<CCollider*, CCollider*>, _bool>& currentCollisions);


private:
	list<CCollider*>			m_Colliders[CG_END];
	list<COLLIDERGROUP>			m_Destroy_Reserve_Collider_Group;
	list<CCollider*>			m_Destory_Reserve_Collider; // 개별 콜라이더 삭제 예약 리스트

	//m_CollisionHistory: 이전 프레임에서의 충돌 상태를 저장하는 맵입니다.
	// 키는 두 콜라이더의 포인터를 담은 쌍(pair)이고, 값은 이 쌍이 충돌 중인지 여부를 나타내는 부울 값입니다.
	map<pair<CCollider*, CCollider*>, _bool> m_CollisionHistory;
	class CGameInstance* m_pGameInstance = { nullptr };

private:
	void ProcessCollisionResults(_float fTimeDelta);
private:
	vector<pair<CCollider*, CCollider*>> m_CollisionResults;
	mutex m_ResultMutex;

public:
	static CCollider_Manager* Create();
	virtual void Free() override;
};
END