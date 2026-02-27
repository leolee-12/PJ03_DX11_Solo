#include "stdafx.h"
#include "Soldier_Bomb.h"
#include "GameInstance.h"
#include "PartObject.h"
#include "Trail_Effect.h"
#include "Effect_Group.h"
#include "Client_Manager.h"
#include "Sound_Manager.h"

IMPLEMENT_CREATE(CSoldier_Bomb)
IMPLEMENT_CLONE(CSoldier_Bomb, CGameObject)
CSoldier_Bomb::CSoldier_Bomb(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pDeviceContext)
	: CBullet(pDevice, pDeviceContext)
{
}

CSoldier_Bomb::CSoldier_Bomb(const CSoldier_Bomb& rhs)
	: CBullet(rhs)
{
}

HRESULT CSoldier_Bomb::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		RETURN_EFAIL;

	return S_OK;
}

HRESULT CSoldier_Bomb::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		RETURN_EFAIL;

	return S_OK;
}

void CSoldier_Bomb::Begin_Play(_cref_time fTimeDelta)
{
	__super::Begin_Play(fTimeDelta);

	PHYSXCOLLIDER_DESC ColliderDesc = {};
	PhysXColliderDesc::Setting_DynamicCollider_WithScale(ColliderDesc, PHYSXCOLLIDER_TYPE::BOX, CL_MONSTER_ATTACK, m_pTransformCom, { 3.f,3.f,3.f }, false, nullptr, true);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_PhysX_Collider"),
		TEXT("Com_PhysXColliderCom"), &(m_pPhysXColliderCom), &ColliderDesc)))
		return;

	weak_ptr<CEffect> pEffect = GET_SINGLE(CEffect_Manager)->Create_Effect<CEffect_Group>(TEXT("Grenade_Boom(Jiho)"), shared_from_this());

	// 부모 스테이터스 연결 및 스킬 이름 설정(고정 스킬이름 사용시 여기에 사용)
	Set_StatusComByOwner("Soldier_Grenade");

	// 사운드
	m_pGameInstance->Play_3DSound(TEXT("FF7_SecuritySoldier_Effect"), 
		{ "087_SE_SecuritySoldier (Par_WE1000_13_Bomb_Bullet_01).wav" }, 
		ESoundGroup::Effect, shared_from_this());
}

void CSoldier_Bomb::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
}

void CSoldier_Bomb::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);

	m_fAccTime += fTimeDelta;

}

void CSoldier_Bomb::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (m_fAccTime >= 0.8f)
	{
		Set_Dead();
	}
}

void CSoldier_Bomb::Before_Render(_cref_time fTimeDelta)
{
	__super::Before_Render(fTimeDelta);
}

void CSoldier_Bomb::End_Play(_cref_time fTimeDelta)
{
	__super::End_Play(fTimeDelta);
}

HRESULT CSoldier_Bomb::Render()
{
	return S_OK;
}

HRESULT CSoldier_Bomb::Ready_Components(void* pArg)
{
	return S_OK;
}

void CSoldier_Bomb::OnCollision_Enter(CCollider* pThisCol, CCollider* pOtherCol)
{
}

void CSoldier_Bomb::OnCollision_Stay(CCollider* pThisCol, CCollider* pOtherCol)
{
}

void CSoldier_Bomb::OnCollision_Exit(CCollider* pThisCol, CCollider* pOtherCol)
{
}

void CSoldier_Bomb::PhysX_OnCollision_Enter(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Enter(pThisCol, pOtherCol, ContactInfo);
	_bool bUseDamage = false;
	if ((bUseDamage = (pOtherCol->Get_ColliderDesc().iFilterType == CL_PLAYER_BODY)))
	{
		if (bUseDamage)
			Status_DamageTo(m_strSkillName, pOtherCol, pOtherCol->Get_Owner(), pThisCol->Get_Owner());
	}
}

void CSoldier_Bomb::PhysX_OnCollision_Stay(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Stay(pThisCol, pOtherCol, ContactInfo);
}

void CSoldier_Bomb::PhysX_OnCollision_Exit(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Exit(pThisCol, pOtherCol, ContactInfo);
}


void CSoldier_Bomb::Free()
{
	__super::Free();
}
