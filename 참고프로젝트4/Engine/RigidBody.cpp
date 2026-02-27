#include "RigidBody.h"
#include "GameObject.h"

IMPLEMENT_CREATE(CRigidBody)
IMPLEMENT_CLONE(CRigidBody,CComponent)

CRigidBody::CRigidBody(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: CComponent(pDevice,pContext)
{
}

CRigidBody::CRigidBody(const CRigidBody& rhs)
	: CComponent(rhs)
{
}

HRESULT CRigidBody::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CRigidBody::Initialize(void* pArg)
{
	__super::Initialize(pArg);

	m_fDrag = 0.1f;
	m_fMass = 1.f;
	m_fMaxSpeed = 30.f;
	m_fMaxVerticalSpeed = 3.5f;
	m_vVelocity = {};
	m_vVerticalVelocity = {};
	m_bEnableGravity = true;

	return S_OK;
}

void CRigidBody::Priority_Tick(_cref_time fTimeDelta)
{
}

void CRigidBody::Tick(_cref_time fTimeDelta)
{
	if (m_pOwnerTransformCom.lock() == nullptr)
		m_pOwnerTransformCom = m_pOwner.lock()->Get_TransformCom();

	Excute_Gravity(fTimeDelta);

	_vector vVelocity = XMLoadFloat3(&m_vVelocity);
	_vector vVerticalVelocity = XMLoadFloat3(&m_vVerticalVelocity);
	_vector vAcceleration = XMLoadFloat3(&m_vAcceleration);

	vVelocity += vAcceleration * fTimeDelta;
	vVelocity = (m_fDrag < 1.f) ? (vVelocity * (1.f - m_fDrag)) : (vVelocity = {0.f, 0.f, 0.f});

	_float fClampSpeed = Clamp(0.f, m_fMaxSpeed, XMVectorGetX(XMVector3Length(vVelocity)));
	
	vVelocity = XMVector3Normalize(vVelocity) * fClampSpeed;

	_vector vAddPosition = vVelocity + vVerticalVelocity;
	if (!XMVector3Equal(vAddPosition, XMVectorZero()))
		m_pOwnerTransformCom.lock()->Add_Position(fTimeDelta, vAddPosition);
	XMStoreFloat3(&m_vVelocity, vVelocity);
}

void CRigidBody::Late_Tick(_cref_time fTimeDelta)
{
}

void CRigidBody::Add_Force(_vector vDir, const _float& fPower, const FORCE_MODE& eForceMode)
{
	_vector vMoveValue = {};
	vDir = XMVector3Normalize(vDir);

	switch (eForceMode)
	{
	case FORCE:
		vMoveValue = XMLoadFloat3(&m_vAcceleration);
		vMoveValue += vDir * fPower / m_fMass;
		XMStoreFloat3(&m_vAcceleration, vMoveValue);
		break;

	case IMPULSE:
		vMoveValue = XMLoadFloat3(&m_vVelocity);
		vMoveValue += vDir * fPower / m_fMass;
		XMStoreFloat3(&m_vVelocity, vMoveValue);
		break;

	default:
		break;
	}	
}

void CRigidBody::Add_Force(CTransform::STATE eAxis, const _float& fPower, const FORCE_MODE& eForceMode)
{
	_vector vMoveValue = {};
	_vector vDir = XMVector3Normalize(m_pOwnerTransformCom.lock()->Get_State(eAxis));

	switch (eForceMode)
	{
	case FORCE:
		vMoveValue = XMLoadFloat3(&m_vAcceleration);
		vMoveValue += vDir * fPower / m_fMass;
		XMStoreFloat3(&m_vAcceleration, vMoveValue);
		break;

	case IMPULSE:
		vMoveValue = XMLoadFloat3(&m_vVelocity);
		vMoveValue += vDir * fPower / m_fMass;
		XMStoreFloat3(&m_vVelocity, vMoveValue);
		break;

	default:
		break;
	}
}

void CRigidBody::Reset_Force(const FORCE_MODE& eForceMode)
{
	switch (eForceMode)
	{
	case FORCE:
		m_vAcceleration = {0.f,0.f,0.f};
		break;

	case IMPULSE:
		m_vVelocity = { 0.f,0.f,0.f };
		break;

	default:
		break;
	}
}

void CRigidBody::Excute_Gravity(_cref_time fTimeDelta)
{
	if (!m_pOwnerTransformCom.lock()->Get_IsOnGround())
	{
		if (m_bEnableGravity)
		{
			_float fClampSpeed = Clamp(-m_fMaxVerticalSpeed, m_fMaxVerticalSpeed, XMVectorGetY(XMLoadFloat3(&m_vVerticalVelocity) + XMVectorSet(0.f, -1.f, 0.f, 1.f) * m_fGravity * 0.034f * fTimeDelta));

			XMStoreFloat3(&m_vVerticalVelocity,XMVectorSet(0.f, fClampSpeed,0.f,1.f));

			//m_pOwnerTransformCom->Add_Position(fTimeDelta, XMVectorSet(0.f, -1.f, 0.f, 1.f) * m_fGravity * fTimeDelta);
		}
	}
	else
		m_vVerticalVelocity = { 0.f,0.f,0.f };
}

void CRigidBody::Free()
{
	__super::Free();
}
