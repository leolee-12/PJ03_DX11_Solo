#pragma once

#include "Client_Defines.h"
#include "Bullet.h"

BEGIN(Client)

class CEffect;

class CMap_Laser : public CBullet
{
	INFO_CLASS(CMap_Laser, CBullet)

public:
	enum LaserType
	{
		NORMAL, FAST, TYPE_END
	};
	typedef struct tagLaserDesc :public GAMEOBJECT_DESC
	{
		_float3 vPos;
		LaserType eType;
		_float fRotaion;

	}MAPLASERDESC;

private:
	CMap_Laser(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pDeviceContext);
	CMap_Laser(const CMap_Laser& rhs);
	virtual ~CMap_Laser() = default;

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
	static shared_ptr<CMap_Laser> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	virtual void Free() override;

private:
	_float m_fRotation = { 0.f };
	_float3 m_vPos = { 0.f,0.f,0.f };
	LaserType m_eType = TYPE_END;

private:
	weak_ptr<CEffect>	m_pEffect;
	wstring		m_strBoneName;

private:
	FTimeChecker	m_TimeChecker1;
	FTimeChecker	m_TimeChecker2;

};

END