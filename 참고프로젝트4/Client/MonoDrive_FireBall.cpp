#include "stdafx.h"
#include "MonoDrive_FireBall.h"
#include "GameInstance.h"
#include "PartObject.h"
#include "Trail_Effect.h"
#include "Effect_Group.h"
#include "Data_Manager.h"
#include "Client_Manager.h"

IMPLEMENT_CREATE(CMonoDrive_FireBall)
IMPLEMENT_CLONE(CMonoDrive_FireBall, CGameObject)
CMonoDrive_FireBall::CMonoDrive_FireBall(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pDeviceContext)
	: CBullet(pDevice, pDeviceContext)
{
}

CMonoDrive_FireBall::CMonoDrive_FireBall(const CMonoDrive_FireBall& rhs)
	: CBullet(rhs)
{
}

HRESULT CMonoDrive_FireBall::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		RETURN_EFAIL;

	return S_OK;
}

HRESULT CMonoDrive_FireBall::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		RETURN_EFAIL;

	return S_OK;
}

void CMonoDrive_FireBall::Begin_Play(_cref_time fTimeDelta)
{	
	__super::Begin_Play(fTimeDelta);

	PHYSXCOLLIDER_DESC ColliderDesc = {};
	PhysXColliderDesc::Setting_DynamicCollider_WithScale(ColliderDesc, PHYSXCOLLIDER_TYPE::BOX, CL_MONSTER_ATTACK, m_pTransformCom, { 0.2f,0.2f,1.f }, false, nullptr, true);
	m_vPhysXColliderLocalOffset = { 0.f,0.f,0.f };

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_PhysX_Collider"),
		TEXT("Com_PhysXColliderCom"), &(m_pPhysXColliderCom), &ColliderDesc)))
		return;

	Set_StatusComByOwner("MonoDrive_FireBall");

	GET_SINGLE(CEffect_Manager)->Create_Effect<CEffect_Group>(L"Monodrive_FireBall_Group", shared_from_this());
}

void CMonoDrive_FireBall::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
}

void CMonoDrive_FireBall::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);

	auto pTarget = GET_SINGLE(CClient_Manager)->Find_TargetPlayer(m_pOwner);

	_float3 vMediatePos = pTarget.vTargetPos;
	vMediatePos.y += 1.f;

	_vector vOwnerPos = m_pOwner.lock()->Get_TransformCom().lock()->Get_State(CTransform::STATE_POSITION);
	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_float fMaxLength = XMVector3Length(vOwnerPos - vMediatePos).m128_f32[0];

	_float fCurLength = XMVector3Length(vOwnerPos - vPos).m128_f32[0];

	_float fChaseValueSpeed = (fCurLength / fMaxLength) * 15.f;

	m_fAccChaseValue += fTimeDelta * 0.018f * 15.f * fChaseValueSpeed;
	m_fSpeed += fTimeDelta * fChaseValueSpeed * 2.f;

	m_pTransformCom->Look_At(vMediatePos, m_fAccChaseValue);

	m_pTransformCom->Go_Straight(fTimeDelta * m_fSpeed);

}

void CMonoDrive_FireBall::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CMonoDrive_FireBall::Before_Render(_cref_time fTimeDelta)
{
	__super::Before_Render(fTimeDelta);
}

void CMonoDrive_FireBall::End_Play(_cref_time fTimeDelta)
{
	__super::End_Play(fTimeDelta);
}

HRESULT CMonoDrive_FireBall::Render()
{
	return S_OK;
}

HRESULT CMonoDrive_FireBall::Ready_Components(void* pArg)
{
	return S_OK;
}

void CMonoDrive_FireBall::OnCollision_Enter(CCollider* pThisCol, CCollider* pOtherCol)
{
}

void CMonoDrive_FireBall::OnCollision_Stay(CCollider* pThisCol, CCollider* pOtherCol)
{
}

void CMonoDrive_FireBall::OnCollision_Exit(CCollider* pThisCol, CCollider* pOtherCol)
{
}

void CMonoDrive_FireBall::PhysX_OnCollision_Enter(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Enter(pThisCol, pOtherCol, ContactInfo);

	_bool bCollideEnemy = false;
	if ((bCollideEnemy = (pOtherCol->Get_ColliderDesc().iFilterType == CL_PLAYER_BODY)) || pOtherCol->Get_ColliderDesc().iFilterType == CL_WALL)
	{
		// 대미지 주는 함수, IStatusInterface 인터페이스로 구현됨.
		if (bCollideEnemy && pOtherCol->Get_ColliderDesc().iFilterType == CL_PLAYER_BODY)
			Status_DamageTo(m_strSkillName, pOtherCol, pOtherCol->Get_Owner(), pThisCol->Get_Owner());

		weak_ptr<CEffect> pEffect = GET_SINGLE(CObjPool_Manager)->Create_Object<CEffect_Group>(TEXT("PlayerHit(Jiho)"), L_EFFECT, nullptr, shared_from_this());		
		pEffect.lock()->Get_TransformCom().lock()->Set_WorldFloat4x4(pEffect.lock()->Get_TransformCom().lock()->Get_WorldFloat4x4() *
		m_pTransformCom->Get_WorldFloat4x4());
		Set_Dead();
	}
}

void CMonoDrive_FireBall::PhysX_OnCollision_Stay(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Stay(pThisCol, pOtherCol, ContactInfo);
}

void CMonoDrive_FireBall::PhysX_OnCollision_Exit(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Exit(pThisCol, pOtherCol, ContactInfo);
}


void CMonoDrive_FireBall::Free()
{
	__super::Free();
}
