#pragma once
#include "Client_Defines.h"
#include "DynamicObject.h"
#include "Utility/LogicDeviceBasic.h"
#include "IStatusInterface.h"

#include "Effect_Manager.h"

BEGIN(Client)

class CBullet : public CDynamicObject, public IStatusInterface
{
	INFO_CLASS(CBullet, CDynamicObject)
	
protected:
	CBullet(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CBullet(const CBullet& rhs);
	virtual ~CBullet() = default;

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

public:
	virtual void OnCollision_Enter(class CCollider* pThisCol, class CCollider* pOtherCol);
	virtual void OnCollision_Stay(class  CCollider* pThisCol, class CCollider* pOtherCol);
	virtual void OnCollision_Exit(class  CCollider* pThisCol, class CCollider* pOtherCol);

public:
	virtual void PhysX_OnCollision_Enter(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo);
	virtual void PhysX_OnCollision_Stay(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo);
	virtual void PhysX_OnCollision_Exit(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo);

#pragma region IStatusInterface
public:
	// GameObject중에 Status를 가지는 하위 클래스를 찾아 스테이터스를 전달 받습니다.
	virtual HRESULT Set_StatusComByGameObject(shared_ptr<CGameObject> pGameObject) override;
	virtual HRESULT Set_StatusComByOwner(const string& strSkillName) override;
	virtual void Status_Damaged(_uint iStatusDamaged, _uint iHitPower, _uint iAddHitPower) override {}
	virtual weak_ptr<class CStatusComp> Get_StatusCom() override;
	void Set_SkillName(const string& strSkillName) { m_strSkillName = strSkillName; }
#pragma endregion


protected:
	_float							m_fAccTime = 0.f;
	FGauge							m_fLifeTime = { 6.f };
	shared_ptr<class CStatusComp>	m_pStatusCom = { nullptr };
	string							m_strSkillName = "";	// 대미지에 적용할 스킬 이름이다.

public:
	static shared_ptr<CBullet> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	virtual void Free() override;

};

END