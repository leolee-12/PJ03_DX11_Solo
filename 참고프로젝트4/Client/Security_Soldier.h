#pragma once
#include "Client_Defines.h"
#include "Monster.h"

BEGIN(Client)

class CSecurity_Soldier : public CMonster
{
	INFO_CLASS(CSecurity_Soldier, CMonster)

protected:
	CSecurity_Soldier(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CSecurity_Soldier(const CSecurity_Soldier& rhs);
	virtual ~CSecurity_Soldier() = default;


public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Begin_Play(_cref_time fTimeDelta) override;
	virtual void Priority_Tick(_cref_time fTimeDelta) override;
	virtual void Tick(_cref_time fTimeDelta) override;
	virtual void Late_Tick(_cref_time fTimeDelta) override;
	virtual void Before_Render(_cref_time fTimeDelta) override;
	virtual void End_Play(_cref_time fTimeDelta) override;
	virtual HRESULT Render() override;

protected:
	HRESULT Ready_Components(void* pArg);
	HRESULT Bind_ShaderResources();
	HRESULT Ready_State();
	HRESULT Ready_PartObjects();

public:
	virtual void Load_Json(const json& In_Json);

private:
	_int m_iBulletCount = { 20 };
	_int m_iTest = 0;

public:
	GETSET_EX2(_int, m_iBulletCount, BulletCount, GET, SET)
	void DiscountBullet() { m_iBulletCount--; }

public:
	virtual void OnCollision_Enter(class CCollider* pThisCol, class  CCollider* pOtherCol);
	virtual void OnCollision_Stay(class  CCollider* pThisCol, class  CCollider* pOtherCol);
	virtual void OnCollision_Exit(class  CCollider* pThisCol, class  CCollider* pOtherCol);

public:
	virtual void PhysX_OnCollision_Enter(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo);
	virtual void PhysX_OnCollision_Stay(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo);
	virtual void PhysX_OnCollision_Exit(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo);

public:
	virtual void Status_Damaged(_uint iStatusDamaged, _uint iHitPower, _uint iAddHitPower) override;

public:
	static shared_ptr<CSecurity_Soldier> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	virtual void Free() override;


	// 상태머신
	_uint		m_iTestAnimIndex = { 0 };
};

END