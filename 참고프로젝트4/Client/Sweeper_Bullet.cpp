#include "stdafx.h"
#include "Sweeper_Bullet.h"
#include "GameInstance.h"
#include "PartObject.h"
#include "Trail_Effect.h"
#include "Effect_Group.h"
#include "Client_Manager.h"
#include "Sound_Manager.h"

IMPLEMENT_CREATE(CSweeper_Bullet)
IMPLEMENT_CLONE(CSweeper_Bullet, CGameObject)
CSweeper_Bullet::CSweeper_Bullet(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pDeviceContext)
	: CBullet(pDevice, pDeviceContext)
{
}

CSweeper_Bullet::CSweeper_Bullet(const CSweeper_Bullet& rhs)
	: CBullet(rhs)
{
}

HRESULT CSweeper_Bullet::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		RETURN_EFAIL;

	return S_OK;
}

HRESULT CSweeper_Bullet::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		RETURN_EFAIL;

	return S_OK;
}

void CSweeper_Bullet::Begin_Play(_cref_time fTimeDelta)
{
	__super::Begin_Play(fTimeDelta);

	PHYSXCOLLIDER_DESC ColliderDesc = {};
	PhysXColliderDesc::Setting_DynamicCollider_WithScale(ColliderDesc, PHYSXCOLLIDER_TYPE::BOX, CL_MONSTER_ATTACK, m_pTransformCom, { 0.2f,0.2f,1.f }, false, nullptr, true);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_PhysX_Collider"),
		TEXT("Com_PhysXColliderCom"), &(m_pPhysXColliderCom), &ColliderDesc)))
		return;
	
	auto pTarget = GET_SINGLE(CClient_Manager)->Find_TargetPlayer(m_pOwner);

	_vector vLook = XMVector4Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK));

	_float fAngle = XMConvertToDegrees(acosf(XMVectorGetX(XMVector4Dot(pTarget.vDirToTarget, vLook))));

	if (fAngle <= 50.f)
	{
		m_pTransformCom->Look_At_OnLand(pTarget.vTargetPos);
	}

	_float3 vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	vPos.y -= 0.2f;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);

	// 스킬 설정
	Set_StatusComByOwner("Sweeper_Bullet");

	shared_ptr<CEffect> pEffect = GET_SINGLE(CEffect_Manager)->Create_Effect<CTrail_Effect>(TEXT("SecuritySoldierBullet3"), shared_from_this());
}

void CSweeper_Bullet::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
}

void CSweeper_Bullet::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);

	m_fAccTime += fTimeDelta;
	
	m_pTransformCom->Go_Straight(fTimeDelta * 5.f);

}

void CSweeper_Bullet::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (5 <= m_fAccTime)
		Set_Dead();
}

void CSweeper_Bullet::Before_Render(_cref_time fTimeDelta)
{
	__super::Before_Render(fTimeDelta);
}

void CSweeper_Bullet::End_Play(_cref_time fTimeDelta)
{
	__super::End_Play(fTimeDelta);
}

HRESULT CSweeper_Bullet::Render()
{
	return S_OK;
}

HRESULT CSweeper_Bullet::Ready_Components(void* pArg)
{
	return S_OK;
}

void CSweeper_Bullet::OnCollision_Enter(CCollider* pThisCol, CCollider* pOtherCol)
{
}	

void CSweeper_Bullet::OnCollision_Stay(CCollider* pThisCol, CCollider* pOtherCol)
{
}

void CSweeper_Bullet::OnCollision_Exit(CCollider* pThisCol, CCollider* pOtherCol)
{
}

void CSweeper_Bullet::PhysX_OnCollision_Enter(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Enter(pThisCol, pOtherCol, ContactInfo);

	_bool bUseDamage = false;
	if (bUseDamage = (pOtherCol->Get_ColliderDesc().iFilterType == CL_PLAYER_BODY) 
		|| pOtherCol->Get_ColliderDesc().iFilterType == CL_WALL)
	{
		// 대미지 주는 함수, IStatusInterface 인터페이스로 구현됨.
		if (bUseDamage && pOtherCol->Get_ColliderDesc().iFilterType == CL_PLAYER_BODY)
			Status_DamageTo(m_strSkillName, pOtherCol, pOtherCol->Get_Owner(), pThisCol->Get_Owner());

		PxContactPairPoint* pContactPoint = new PxContactPairPoint[ContactInfo.contactCount];
		PxU32 nbContacts = ContactInfo.extractContacts(pContactPoint, ContactInfo.contactCount);

		if (nbContacts >= 1)	
		{
			GET_SINGLE(CEffect_Manager)->Create_Hit_Effect<CEffect_Group>(L"Player_BulletHit(Jiho)", shared_from_this(),
				Convert_Vector(pContactPoint[0].position), Convert_Vector(pContactPoint[0].normal));
		}

		m_pGameInstance->Play_3DSound(TEXT("FF7_Resident_Battle")
			, { "124_SE (Par_R_Bullet_02).wav" }
			, ESoundGroup::Effect, shared_from_this());

		Set_Dead();
	}
}	

void CSweeper_Bullet::PhysX_OnCollision_Stay(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Stay(pThisCol, pOtherCol, ContactInfo);
}

void CSweeper_Bullet::PhysX_OnCollision_Exit(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Exit(pThisCol, pOtherCol, ContactInfo);
}


void CSweeper_Bullet::Free()
{
	__super::Free();
}
