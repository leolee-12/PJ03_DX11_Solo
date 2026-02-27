#pragma once

#include "PartObject.h"
#include "Client_Defines.h"
#include "IStatusInterface.h"

BEGIN(Client)

/// <summary>
/// 스테이터스 기능이 필요한 파트 오브젝트를 위한 상속 클래스
/// </summary>
class CWeaponPart abstract : public CPartObject, public IStatusInterface
{
	INFO_CLASS(CWeaponPart, CPartObject)

protected:
	CWeaponPart(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CWeaponPart(const CWeaponPart& rhs);
	virtual ~CWeaponPart() = default;

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


public:
	virtual void OnCollision_Enter(class CCollider* pThisCol, class  CCollider* pOtherCol);
	virtual void OnCollision_Stay(class  CCollider* pThisCol, class  CCollider* pOtherCol);
	virtual void OnCollision_Exit(class  CCollider* pThisCol, class  CCollider* pOtherCol);

public:
	virtual void PhysX_OnCollision_Enter(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo);
	virtual void PhysX_OnCollision_Stay(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo);
	virtual void PhysX_OnCollision_Exit(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo);

protected:
	shared_ptr<CTrail_Buffer>	m_pWeaponTrail = { nullptr };

public:
	void	Activate_Trail(_bool bCheck);

#pragma region IStatusInterface
public:
	// GameObject중에 Status를 가지는 하위 클래스를 찾아 스테이터스를 전달 받습니다.
	virtual HRESULT Set_StatusComByGameObject(shared_ptr<CGameObject> pGameObject) override;
	virtual HRESULT Set_StatusComByOwner(const string& strSkillName = "") override;
	virtual void Status_Damaged(_uint iStatusDamaged, _uint iHitPower, _uint iAddHitPower) override {}
	virtual weak_ptr<class CStatusComp> Get_StatusCom() override;
	void Set_SkillName(const string& strName) { m_strSkillName = strName; }

protected:
	weak_ptr<class CStatusComp> m_pStatusCom;
	string						m_strSkillName = { "" };
#pragma endregion

	virtual void Free() override;
};

END