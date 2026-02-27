#include "stdafx.h"
#include "Sweeper_Bomb.h"
#include "GameInstance.h"
#include "PartObject.h"
#include "Trail_Effect.h"
#include "Effect_Group.h"
#include "Client_Manager.h"
#include "Sound_Manager.h"

IMPLEMENT_CREATE(CSweeper_Bomb)
IMPLEMENT_CLONE(CSweeper_Bomb, CGameObject)
CSweeper_Bomb::CSweeper_Bomb(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pDeviceContext)
	: CBullet(pDevice, pDeviceContext)
{
}

CSweeper_Bomb::CSweeper_Bomb(const CSweeper_Bomb& rhs)
	: CBullet(rhs)
{
}

HRESULT CSweeper_Bomb::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		RETURN_EFAIL;

	return S_OK;
}

HRESULT CSweeper_Bomb::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		RETURN_EFAIL;

	return S_OK;
}

void CSweeper_Bomb::Begin_Play(_cref_time fTimeDelta)
{
	__super::Begin_Play(fTimeDelta);

	PHYSXCOLLIDER_DESC ColliderDesc = {};
	PhysXColliderDesc::Setting_DynamicCollider_WithScale(ColliderDesc, PHYSXCOLLIDER_TYPE::BOX, CL_MONSTER_ATTACK, m_pTransformCom, {1.f,1.f,1.f }, false, nullptr, true);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_PhysX_Collider"),
		TEXT("Com_PhysXColliderCom"), &(m_pPhysXColliderCom), &ColliderDesc)))
		return;

	weak_ptr<CEffect> pEffect = GET_SINGLE(CEffect_Manager)->Create_Effect<CEffect_Group>(TEXT("SweeperBomb(Jiho)"), shared_from_this());
	// 스킬 설정
	Set_StatusComByOwner("Sweeper_BombHit");

	m_fLifeTime = { 0.5f };

	m_pGameInstance->Play_3DSound(TEXT("FF7_Sweeper_Effect")
			, { "058_SE_Sweeper (Par_ChargeAtk_Hit_01).wav" }
			, ESoundGroup::Effect, shared_from_this());
}

void CSweeper_Bomb::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
}

void CSweeper_Bomb::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (m_fLifeTime.Increase(fTimeDelta))
	{
		Set_Dead();
	}
}

void CSweeper_Bomb::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CSweeper_Bomb::Before_Render(_cref_time fTimeDelta)
{
	__super::Before_Render(fTimeDelta);
}

void CSweeper_Bomb::End_Play(_cref_time fTimeDelta)
{
	__super::End_Play(fTimeDelta);
}

HRESULT CSweeper_Bomb::Render()
{
	return S_OK;
}

HRESULT CSweeper_Bomb::Ready_Components(void* pArg)
{
	return S_OK;
}

void CSweeper_Bomb::OnCollision_Enter(CCollider* pThisCol, CCollider* pOtherCol)
{
}

void CSweeper_Bomb::OnCollision_Stay(CCollider* pThisCol, CCollider* pOtherCol)
{
}

void CSweeper_Bomb::OnCollision_Exit(CCollider* pThisCol, CCollider* pOtherCol)
{
}

void CSweeper_Bomb::PhysX_OnCollision_Enter(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Enter(pThisCol, pOtherCol, ContactInfo);

	_bool bUseDamage = (pOtherCol->Get_ColliderDesc().iFilterType == CL_PLAYER_BODY);
	if (bUseDamage)
	{
		// 대미지 주는 함수, IStatusInterface 인터페이스로 구현됨.
		if (bUseDamage)
			Status_DamageTo(m_strSkillName, pOtherCol, pOtherCol->Get_Owner(), pThisCol->Get_Owner());

		//weak_ptr<CEffect> pEffect = GET_SINGLE(CObjPool_Manager)->Create_Object<CEffect_Group>(TEXT("Cloud_Bullet_Hit"), L_EFFECT, nullptr, shared_from_this());
		//pEffect.lock()->Get_TransformCom().lock()->Set_WorldFloat4x4(pEffect.lock()->Get_TransformCom().lock()->Get_WorldFloat4x4() *
		//	m_pTransformCom->Get_WorldFloat4x4());
		Set_Dead();
	}
}

void CSweeper_Bomb::PhysX_OnCollision_Stay(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Stay(pThisCol, pOtherCol, ContactInfo);

}

void CSweeper_Bomb::PhysX_OnCollision_Exit(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Exit(pThisCol, pOtherCol, ContactInfo);
}


void CSweeper_Bomb::Free()
{
	__super::Free();
}
