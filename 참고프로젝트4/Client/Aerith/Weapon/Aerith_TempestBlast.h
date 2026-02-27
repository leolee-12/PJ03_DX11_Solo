#pragma once

#include "Client_Defines.h"
#include "Bullet.h"
#include "Utility/LogicDeviceBasic.h"


BEGIN(Client)

class CEffect;

/// <summary>
/// 템페스트 투사체 적중으로 생성된 객체
/// </summary>
class CAerith_TempestBlast : public CBullet
{
	INFO_CLASS(CAerith_TempestBlast, CBullet)

private:
	CAerith_TempestBlast(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pDeviceContext);
	CAerith_TempestBlast(const CAerith_TempestBlast& rhs);
	virtual ~CAerith_TempestBlast() = default;

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
	virtual void PhysX_OnCollision_Enter(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo);
	virtual void PhysX_OnCollision_Stay(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo);
	virtual void PhysX_OnCollision_Exit(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo);

public:
	static shared_ptr<CAerith_TempestBlast> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	virtual void Free() override;

private:
	void Create_Blast();

private:
	_bool m_bIsChaseEnd = { false };		// 체이스 종료하면 쭉 직진
	_float3	m_vTargetPos = {};				// 목표 타격지점.
	_float m_fAccChaseValue = { 0.f };		// 추적 각도 크기. 점점 커지도록 만든다.
	_float m_fSpeed = { 3.2f };
	_float m_fCheckPosTime = { 1.f };		// 추적 끄는 시간

private:
	shared_ptr<CEffect>	m_pEffect = { nullptr };

	FGauge	m_fDamageTiming = { 0.f };
	_bool	m_bDamageTiming_Reset = { false };
};

END