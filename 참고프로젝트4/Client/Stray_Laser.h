#pragma once

#include "Client_Defines.h"
#include "Bullet.h"

BEGIN(Client)

class CEffect;

/// <summary>
/// 에어버스터의 핑거빔
/// </summary>
class CStray_Laser : public CBullet
{
	INFO_CLASS(CStray_Laser, CBullet)

private:
	CStray_Laser(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pDeviceContext);
	CStray_Laser(const CStray_Laser& rhs);
	virtual ~CStray_Laser() = default;

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
	static shared_ptr<CStray_Laser> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	virtual void Free() override;

private:
	_bool m_bIsChaseEnd = { false };		// 체이스 종료하면 쭉 직진
	_float3	m_vTargetPos = {};				// 목표 타격지점.
	_float m_fAccChaseValue = { 0.f };		// 추적 각도 크기. 점점 커지도록 만든다.
	_float m_fSpeed = { 2.f };
	FTimeChecker	m_fDamageTick = { 0.4f };
	FTimeChecker	m_fTimeChecker = { 0.f };

	_bool			m_bHit = { false };

private:
	PLAYER_TYPE	m_eTargetType = { PLAYER_TYPE::PLAYER_END };

private: // 임시
	weak_ptr<CEffect>	m_pEffect;
	wstring		m_strBoneName;

public:
	GETSET_EX2(wstring, m_strBoneName, BoneName, GET, SET)
};

END