#include "stdafx.h"
#include "Stray_Laser.h"
#include "GameInstance.h"
#include "PartObject.h"
#include "Client_Manager.h"

#include "CommonModelComp.h"
#include "PhysX_Manager.h"

#include "Effect_Manager.h"
#include "Sound_Manager.h"

IMPLEMENT_CREATE(CStray_Laser)
IMPLEMENT_CLONE(CStray_Laser, CGameObject)
CStray_Laser::CStray_Laser(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pDeviceContext)
	: CBullet(pDevice, pDeviceContext)
{
}

CStray_Laser::CStray_Laser(const CStray_Laser& rhs)
	: CBullet(rhs)
{
}

HRESULT CStray_Laser::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		RETURN_EFAIL;

	return S_OK;
}

HRESULT CStray_Laser::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		RETURN_EFAIL;

	m_fTimeChecker = FTimeChecker(2.f);

	return S_OK;
}

void CStray_Laser::Begin_Play(_cref_time fTimeDelta)
{
	__super::Begin_Play(fTimeDelta);

	auto pTarget = GET_SINGLE(CClient_Manager)->Find_TargetPlayer(m_pOwner);

	PHYSXCOLLIDER_DESC ColliderDesc = {};
	PhysXColliderDesc::Setting_DynamicCollider_WithScale(ColliderDesc, PHYSXCOLLIDER_TYPE::BOX, CL_MONSTER_ATTACK, m_pTransformCom, { 0.2f, 0.2f, 0.2f }, false, nullptr, true);
	m_vPhysXColliderLocalOffset = { 0.f,0.f,0.f };

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_PhysX_Collider"),
		TEXT("Com_PhysXColliderCom"), &(m_pPhysXColliderCom), &ColliderDesc)))
		return;

	// [여기에] 트레일 이펙트 생성 넣어주세요.
	//m_pEffect = GET_SINGLE(CObjPool_Manager)->Create_Object<CEffect_Group>(TEXT("AirBursterFingerBeam"), L_EFFECT, nullptr, shared_from_this());
	m_pEffect = GET_SINGLE(CEffect_Manager)->Create_Effect<CEffect_Group>(TEXT("GRP_AirBursterFingerBeam"), shared_from_this());

	// 부모 스테이터스 연결 및 스킬 이름 설정(고정 스킬이름 사용시 여기에 사용)
	Set_StatusComByOwner("Stray_Laser");
	m_fDamageTick.Update(0.398f);
}

void CStray_Laser::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);

	if (m_fTimeChecker.Update(fTimeDelta))
		Set_Dead();
}

void CStray_Laser::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);

	m_fDamageTick.Update(fTimeDelta);
}

void CStray_Laser::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
	PxRaycastHit hit;
	if (m_fDamageTick.IsTicked() && GET_SINGLE(CPhysX_Manager)->RaycastSingle(shared_from_this(), CL_MONSTER_ATTACK, m_pTransformCom->Get_State(CTransform::STATE_POSITION), XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK)), 1000.f, hit))
	{
		if (hit.actor->userData != nullptr)
		{
			CPhysX_Collider* pCollider = (CPhysX_Collider*)hit.actor->userData;

			if (pCollider && pCollider->Get_ColliderDesc().iFilterType == CL_PLAYER_BODY)
			{
				Status_DamageTo(m_strSkillName, pCollider, pCollider->Get_Owner(), Get_Owner());

				m_pGameInstance->Play_3DSound(TEXT("FF7_Resident_Battle")
					, { "044_SE_Stray (Par_Laser_Hit_01).wav", "045_SE_Stray (Par_Laser_Hit_01).wav", "046_SE_Stray (Par_Laser_Hit_01).wav", "047_SE_Stray (Par_Laser_Hit_01).wav" }
				, ESoundGroup::Effect, shared_from_this());
			}
		}
	}
} // 레이 충돌


void CStray_Laser::Before_Render(_cref_time fTimeDelta)
{
	__super::Before_Render(fTimeDelta);
}

void CStray_Laser::End_Play(_cref_time fTimeDelta)
{
	__super::End_Play(fTimeDelta);
}

HRESULT CStray_Laser::Render()
{
	return S_OK;
}

HRESULT CStray_Laser::Ready_Components(void* pArg)
{
	return S_OK;
}

void CStray_Laser::PhysX_OnCollision_Enter(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Enter(pThisCol, pOtherCol, ContactInfo);
}

void CStray_Laser::PhysX_OnCollision_Stay(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Stay(pThisCol, pOtherCol, ContactInfo);
}

void CStray_Laser::PhysX_OnCollision_Exit(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Exit(pThisCol, pOtherCol, ContactInfo);
}


void CStray_Laser::Free()
{
	__super::Free();
}
