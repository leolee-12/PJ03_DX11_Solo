#include "stdafx.h"
#include "Stray_Charge.h"
#include "GameInstance.h"
#include "PartObject.h"
#include "Client_Manager.h"
#include "Data_Manager.h"

IMPLEMENT_CREATE(CStray_Charge)
IMPLEMENT_CLONE(CStray_Charge, CGameObject)
CStray_Charge::CStray_Charge(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pDeviceContext)
	: CBullet(pDevice, pDeviceContext)
{
}

CStray_Charge::CStray_Charge(const CStray_Charge& rhs)
	: CBullet(rhs)
{
}

HRESULT CStray_Charge::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		RETURN_EFAIL;

	return S_OK;
}

HRESULT CStray_Charge::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		RETURN_EFAIL;

	return S_OK;
}

void CStray_Charge::Begin_Play(_cref_time fTimeDelta)
{
	__super::Begin_Play(fTimeDelta);

	weak_ptr<CEffect> pEffect = GET_SINGLE(CEffect_Manager)->Create_Effect<CEffect_Group>(TEXT("StrayCharge1"), shared_from_this());
}

void CStray_Charge::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
}

void CStray_Charge::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (m_pOwner.lock() != nullptr)
	{
		_matrix MuzzleMatrix = m_pOwner.lock()->Get_ModelCom().lock()->Get_BoneTransformMatrixWithParents(L"C_VFXMuzzleA_a");
		_float3 vLook = XMVector3Normalize(MuzzleMatrix.r[0]);
		_float3 vPos = MuzzleMatrix.r[3] +vLook * 0.9f;
		m_pTransformCom->Set_Position(1.f, vPos);
	}

	m_fAccTime += fTimeDelta;
}

void CStray_Charge::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (3.5f <= m_fAccTime)
		Set_Dead();
}

void CStray_Charge::Before_Render(_cref_time fTimeDelta)
{
	__super::Before_Render(fTimeDelta);
}

void CStray_Charge::End_Play(_cref_time fTimeDelta)
{
	__super::End_Play(fTimeDelta);
}

HRESULT CStray_Charge::Render()
{
	return S_OK;
}

HRESULT CStray_Charge::Ready_Components(void* pArg)
{
	return S_OK;
}

void CStray_Charge::OnCollision_Enter(CCollider* pThisCol, CCollider* pOtherCol)
{
}

void CStray_Charge::OnCollision_Stay(CCollider* pThisCol, CCollider* pOtherCol)
{
}

void CStray_Charge::OnCollision_Exit(CCollider* pThisCol, CCollider* pOtherCol)
{
}

void CStray_Charge::PhysX_OnCollision_Enter(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Enter(pThisCol, pOtherCol, ContactInfo);
}

void CStray_Charge::PhysX_OnCollision_Stay(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Stay(pThisCol, pOtherCol, ContactInfo);
}

void CStray_Charge::PhysX_OnCollision_Exit(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Exit(pThisCol, pOtherCol, ContactInfo);
}


void CStray_Charge::Free()
{
	__super::Free();
}
