#include "stdafx.h"
#include "Soldier_Bullet.h"
#include "GameInstance.h"
#include "PartObject.h"
#include "Client_Manager.h"
#include "Sound_Manager.h"

IMPLEMENT_CREATE(CSoldier_Bullet)
IMPLEMENT_CLONE(CSoldier_Bullet, CGameObject)
CSoldier_Bullet::CSoldier_Bullet(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pDeviceContext)
	: CBullet(pDevice, pDeviceContext)
{
}

CSoldier_Bullet::CSoldier_Bullet(const CSoldier_Bullet& rhs)
	: CBullet(rhs)
{
}

HRESULT CSoldier_Bullet::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		RETURN_EFAIL;

	return S_OK;
}

HRESULT CSoldier_Bullet::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		RETURN_EFAIL;

	return S_OK;
}

void CSoldier_Bullet::Begin_Play(_cref_time fTimeDelta)
{
	__super::Begin_Play(fTimeDelta);

	auto pTarget = GET_SINGLE(CClient_Manager)->Find_TargetPlayer(m_pOwner);

	shared_ptr<CGameObject> pGun = m_pOwner.lock()->Find_PartObject(L"Part_Gun");

	_float4x4 matWorld = dynamic_pointer_cast<CPartObject>(pGun)->Get_WorldMatrix();
	_float3 vPos = _float3(matWorld._41,matWorld._42,matWorld._43);

	m_pTransformCom->Look_At_OnLand(pTarget.vTargetPos);

	_float3 vLook = pTarget.vDirToTarget;
	
	vPos += XMVector3Normalize(vLook) * 1.2f;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);

	m_pTransformCom->Look_At_OnLand(pTarget.vTargetPos);

	PHYSXCOLLIDER_DESC ColliderDesc = {};
	PhysXColliderDesc::Setting_DynamicCollider_WithScale(ColliderDesc, PHYSXCOLLIDER_TYPE::BOX, CL_MONSTER_ATTACK, m_pTransformCom, { 0.2f,0.2f,1.f }, false, nullptr, true);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_PhysX_Collider"),
		TEXT("Com_PhysXColliderCom"), &(m_pPhysXColliderCom), &ColliderDesc)))
		return;

	weak_ptr<CEffect> pEffect = GET_SINGLE(CEffect_Manager)->Create_Effect<CTrail_Effect>(TEXT("SecuritySoldierBullet1"), shared_from_this());

	// 부모 스테이터스 연결 및 스킬 이름 설정(고정 스킬이름 사용시 여기에 사용)
	Set_StatusComByOwner("Soldier_Gun");
}

void CSoldier_Bullet::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
}

void CSoldier_Bullet::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);

	m_fAccTime += fTimeDelta;

	m_pTransformCom->Go_Straight(fTimeDelta * 5.f);

}

void CSoldier_Bullet::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (5 <= m_fAccTime)
		Set_Dead();
}

void CSoldier_Bullet::Before_Render(_cref_time fTimeDelta)
{
	__super::Before_Render(fTimeDelta);
}

void CSoldier_Bullet::End_Play(_cref_time fTimeDelta)
{
	__super::End_Play(fTimeDelta);
}

HRESULT CSoldier_Bullet::Render()
{
	return S_OK;
}

HRESULT CSoldier_Bullet::Ready_Components(void* pArg)
{
	return S_OK;
}

void CSoldier_Bullet::OnCollision_Enter(CCollider* pThisCol, CCollider* pOtherCol)
{
}

void CSoldier_Bullet::OnCollision_Stay(CCollider* pThisCol, CCollider* pOtherCol)
{
}

void CSoldier_Bullet::OnCollision_Exit(CCollider* pThisCol, CCollider* pOtherCol)
{
}

void CSoldier_Bullet::PhysX_OnCollision_Enter(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Enter(pThisCol, pOtherCol, ContactInfo);

	_bool bUseDamage = false;
	if ((bUseDamage = (pOtherCol->Get_ColliderDesc().iFilterType == CL_PLAYER_BODY))
		|| pOtherCol->Get_ColliderDesc().iFilterType == CL_WALL)
	{
		// 대미지 주는 함수, IStatusInterface 인터페이스로 구현됨.
		if (bUseDamage)
			Status_DamageTo(m_strSkillName, pOtherCol, pOtherCol->Get_Owner(), pThisCol->Get_Owner());

		shared_ptr<PxContactPairPoint> pContactPoint(new PxContactPairPoint[ContactInfo.contactCount]);
		PxU32 nbContacts = ContactInfo.extractContacts(pContactPoint.get(), ContactInfo.contactCount);

		if (nbContacts >= 1)
		{
			GET_SINGLE(CEffect_Manager)->Create_Hit_Effect<CEffect_Group>(L"Player_BulletHit(Jiho)", shared_from_this(),
				Convert_Vector(pContactPoint.get()[0].position), Convert_Vector(pContactPoint.get()[0].normal));
		
			m_pGameInstance->Play_3DSound(TEXT("FF7_Resident_Battle"),
				{ "124_SE (Par_R_Bullet_02).wav" },
				ESoundGroup::Effect, shared_from_this());
		}
		
		Set_Dead();
	}
}

void CSoldier_Bullet::PhysX_OnCollision_Stay(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Stay(pThisCol, pOtherCol, ContactInfo);
}

void CSoldier_Bullet::PhysX_OnCollision_Exit(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Exit(pThisCol, pOtherCol, ContactInfo);
}


void CSoldier_Bullet::Free()
{
	__super::Free();
}
