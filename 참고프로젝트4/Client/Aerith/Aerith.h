#pragma once

#include "Client_Defines.h"
#include "Player.h"

BEGIN(Client)

class CAerith : public CPlayer
{
	INFO_CLASS(CAerith, CPlayer)

protected:
	CAerith(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CAerith(const CAerith& rhs);
	virtual ~CAerith() = default;


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
	HRESULT Ready_PartObjects();

public:
	virtual void OnCollision_Enter(class CCollider* pThisCol, class  CCollider* pOtherCol) override;
	virtual void OnCollision_Stay(class  CCollider* pThisCol, class  CCollider* pOtherCol) override;
	virtual void OnCollision_Exit(class  CCollider* pThisCol, class  CCollider* pOtherCol) override;

public:
	virtual void PhysX_OnCollision_Enter(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo) override;
	virtual void PhysX_OnCollision_Stay(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo) override;
	virtual void PhysX_OnCollision_Exit(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo) override;

public:
	virtual void PhysX_Raycast_Stay(weak_ptr<CGameObject> pOther, _uint iOtherColLayer, _float4 vOrigin, _float4 vDirection, _float4 vColPosition);

public:
	virtual void Status_Damaged(_uint iStatusDamaged, _uint iHitPower, _uint iAddHitPower) override;

public:
	virtual void Set_State_AIMode(_bool bAIMode) override;

public:
	static shared_ptr<CAerith> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	virtual void Free() override;


	// 상태머신
	_uint		m_iTestAnimIndex = { 0 };
};

END