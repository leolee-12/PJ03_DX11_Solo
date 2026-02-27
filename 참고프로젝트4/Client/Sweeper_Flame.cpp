#include "stdafx.h"
#include "Sweeper_Flame.h"
#include "GameInstance.h"
#include "PartObject.h"
#include "Client_Manager.h"
#include "Sound_Manager.h"

IMPLEMENT_CREATE(CSweeper_Flame)
IMPLEMENT_CLONE(CSweeper_Flame, CGameObject)
CSweeper_Flame::CSweeper_Flame(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pDeviceContext)
	: CBullet(pDevice, pDeviceContext)
{
}

CSweeper_Flame::CSweeper_Flame(const CSweeper_Flame& rhs)
	: CBullet(rhs)
{
}

HRESULT CSweeper_Flame::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		RETURN_EFAIL;

	return S_OK;
}

HRESULT CSweeper_Flame::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		RETURN_EFAIL;

	return S_OK;
}

void CSweeper_Flame::Begin_Play(_cref_time fTimeDelta)
{
	__super::Begin_Play(fTimeDelta);

	PHYSXCOLLIDER_DESC ColliderDesc = {};
	PhysXColliderDesc::Setting_DynamicCollider_WithScale(ColliderDesc, PHYSXCOLLIDER_TYPE::BOX, CL_MONSTER_ATTACK, m_pTransformCom, { 1.f,1.f,1.f }, false, nullptr, true);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_PhysX_Collider"),
		TEXT("Com_PhysXColliderCom"), &(m_pPhysXColliderCom), &ColliderDesc)))
		return;

	weak_ptr<CEffect> pEffect = GET_SINGLE(CEffect_Manager)->Create_Effect<CParticle>(TEXT("Sweeper_Fire"), shared_from_this());

	// 스킬 설정
	Set_StatusComByOwner("Sweeper_Flame");
}

void CSweeper_Flame::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
}

void CSweeper_Flame::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);

	m_fAccTime += fTimeDelta;
	
	m_pTransformCom->Go_Straight(fTimeDelta * 1.2f);
}

void CSweeper_Flame::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (2.f <= m_fAccTime)
		Set_Dead();
}

void CSweeper_Flame::Before_Render(_cref_time fTimeDelta)
{
	__super::Before_Render(fTimeDelta);
}

void CSweeper_Flame::End_Play(_cref_time fTimeDelta)
{
	__super::End_Play(fTimeDelta);
}

HRESULT CSweeper_Flame::Render()
{
	return S_OK;
}

HRESULT CSweeper_Flame::Ready_Components(void* pArg)
{
	return S_OK;
}

void CSweeper_Flame::OnCollision_Enter(CCollider* pThisCol, CCollider* pOtherCol)
{
}

void CSweeper_Flame::OnCollision_Stay(CCollider* pThisCol, CCollider* pOtherCol)
{
}

void CSweeper_Flame::OnCollision_Exit(CCollider* pThisCol, CCollider* pOtherCol)
{
}

void CSweeper_Flame::PhysX_OnCollision_Enter(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Enter(pThisCol, pOtherCol, ContactInfo);
	
	_bool bUseDamage = false;
	if (bUseDamage = (pOtherCol->Get_ColliderDesc().iFilterType == CL_PLAYER_BODY)
		|| pOtherCol->Get_ColliderDesc().iFilterType == CL_WALL)
	{
		// 대미지 주는 함수, IStatusInterface 인터페이스로 구현됨.
		if (bUseDamage && pOtherCol->Get_ColliderDesc().iFilterType == CL_PLAYER_BODY)
			Status_DamageTo(m_strSkillName, pOtherCol, pOtherCol->Get_Owner(), pThisCol->Get_Owner());

		m_pGameInstance->Play_3DSound(TEXT("FF7_Sweeper_Effect")
			, { "064_SE_Sweeper (Par_SmokeShot_Hit_01).wav", "065_SE_Sweeper (Par_SmokeShot_Hit_01).wav", "066_SE_Sweeper (Par_SmokeShot_Hit_01).wav" }
			, ESoundGroup::Effect, shared_from_this());
	}
}

void CSweeper_Flame::PhysX_OnCollision_Stay(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Stay(pThisCol, pOtherCol, ContactInfo);

	
}

void CSweeper_Flame::PhysX_OnCollision_Exit(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Exit(pThisCol, pOtherCol, ContactInfo);
}


void CSweeper_Flame::Free()
{
	__super::Free();
}
