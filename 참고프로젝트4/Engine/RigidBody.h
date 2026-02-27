#pragma once

#include "Transform.h"

BEGIN(Engine)

class ENGINE_DLL CRigidBody : public CComponent
{
	INFO_CLASS(CRigidBody, CComponent)
public:
	enum FORCE_MODE {FORCE,IMPULSE,FORCE_MODE_END};
	struct FORCE
	{
		_float3 vDir;
		_float	fPower;
	};

private:
	explicit CRigidBody(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	explicit CRigidBody(const CRigidBody& rhs);
	virtual ~CRigidBody() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

public:
	virtual void Priority_Tick(_cref_time fTimeDelta);
	virtual void Tick(_cref_time fTimeDelta);
	virtual void Late_Tick(_cref_time fTimeDelta);

public:
	void Add_Force(_vector vDir, const _float& fPower, const FORCE_MODE& eForceMode = FORCE);
	void Add_Force(CTransform::STATE eAxis, const _float& fPower, const FORCE_MODE& eForceMode = FORCE);

	void Reset_Force(const FORCE_MODE& eForceMode = FORCE);
	void Excute_Gravity(_cref_time fTimeDelta);

private:
	_bool		m_bEnableGravity = { true };
	_float3		m_vVelocity = {};
	_float3		m_vVerticalVelocity = {};
	_float3		m_vAcceleration = {};
	_float		m_fDrag = {}; //ÀúÇ×·Â
	_float		m_fGravity = { 9.8f };
	_float		m_fMass = {};
	_float		m_fMaxSpeed = {};
	_float		m_fMaxVerticalSpeed = {};

	weak_ptr<CTransform> m_pOwnerTransformCom;



public:
	static shared_ptr<CRigidBody> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual shared_ptr<CComponent> Clone(void* pArg) override;
	virtual void Free() override;

};

END