#pragma once

#include "Client_Defines.h"
#include "Bullet.h"

BEGIN(Client)

class CEffect;

/// <summary>
/// 에어리스 평타
/// </summary>
class CAerith_NormalBullet : public CBullet
{
	INFO_CLASS(CAerith_NormalBullet, CBullet)

private:
	CAerith_NormalBullet(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pDeviceContext);
	CAerith_NormalBullet(const CAerith_NormalBullet& rhs);
	virtual ~CAerith_NormalBullet() = default;

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
	static shared_ptr<CAerith_NormalBullet> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	virtual void Free() override;

private:
	void Create_Effect(_float3 vPos, _float3 vLook);

private:
	_bool m_bIsChaseEnd = { false };		// 체이스 종료하면 쭉 직진
	_float3	m_vTargetPos = {};				// 목표 타격지점.
	_float m_fAccChaseValue = { 0.f };		// 추적 각도 크기. 점점 커지도록 만든다.
	_float m_fSpeed = { 3.2f };
	_float m_fCheckPosTime = { 1.f };		// 추적 끄는 시간
	FTimeChecker m_fRotateTime = { 0.1f };
	FGauge			m_fLifeTime = { 2.f };

private:
	FGauge			m_fTrailCreate_MinTime = { 0.1f };
	FTimeChecker	m_fTrailCreate_Time = { 0.02f };

private:
	shared_ptr<CEffect>	m_pEffect = { nullptr };
};

END