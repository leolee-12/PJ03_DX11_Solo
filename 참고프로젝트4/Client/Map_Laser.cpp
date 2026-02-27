#include "stdafx.h"
#include "Map_Laser.h"
#include "GameInstance.h"
#include "PartObject.h"
#include "Client_Manager.h"

#include "CommonModelComp.h"
#include "PhysX_Manager.h"

#include "Effect_Group.h"
#include "Effect_Manager.h"
#include "StatusComp.h"
#include "Sound_Manager.h"

IMPLEMENT_CREATE(CMap_Laser)
IMPLEMENT_CLONE(CMap_Laser, CGameObject)
CMap_Laser::CMap_Laser(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pDeviceContext)
	: CBullet(pDevice, pDeviceContext)
{
}

CMap_Laser::CMap_Laser(const CMap_Laser& rhs)
	: CBullet(rhs)
{
}

HRESULT CMap_Laser::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		RETURN_EFAIL;

	return S_OK;
}

HRESULT CMap_Laser::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		RETURN_EFAIL;

	if (FAILED(Ready_Components(nullptr)))
		RETURN_EFAIL;

	MAPLASERDESC* pDesc = (MAPLASERDESC*)pArg;

	m_fRotation = pDesc->fRotaion;
	m_eType = pDesc->eType;
	m_vPos = pDesc->vPos;
	m_vPos.y -= 0.2f;

	m_TimeChecker1 = FTimeChecker(2.6f);
	m_TimeChecker2 = FTimeChecker(1.3f);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vPos);

	return S_OK;
}

void CMap_Laser::Begin_Play(_cref_time fTimeDelta)
{
	__super::Begin_Play(fTimeDelta);

	m_pTransformCom->Set_Look_Manual(XMVectorSet(0.f, 1.f, 0.f, 0.f));
	m_pTransformCom->Rotation(_float3(0.f, 0.f, 1.f), XMConvertToRadians(m_fRotation));

	if (m_eType == NORMAL)
		m_pEffect = GET_SINGLE(CEffect_Manager)->Create_Effect<CEffect_Group>(TEXT("Map_Laser_Group"), shared_from_this());
	else if (m_eType == FAST)
		m_pEffect = GET_SINGLE(CEffect_Manager)->Create_Effect<CEffect_Group>(TEXT("Map_Laser_Group_Fast"), shared_from_this());

	m_pStatusCom->Init_StatsByTable(CStatusComp::ETableType::Enemy, "Map_Laser");

	Set_SkillName("Map_Laser");
}

void CMap_Laser::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);

	if (m_eType == NORMAL && 0.8f <= m_fAccTime)
	{
		if (m_pPhysXColliderCom == nullptr)
		{
			PHYSXCOLLIDER_DESC ColliderDesc = {};
			PhysXColliderDesc::Setting_DynamicCollider_WithScale(ColliderDesc, PHYSXCOLLIDER_TYPE::BOX, CL_LASER, m_pTransformCom, { 0.7f, 0.5f, 300.f }, false, nullptr, true);

			if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_PhysX_Collider"),
				TEXT("Com_PhysXColliderCom"), &(m_pPhysXColliderCom), &ColliderDesc)))
				return;
		}
	}

	else if (m_eType == FAST && 0.4f <= m_fAccTime)
	{
		if (m_pPhysXColliderCom == nullptr)
		{
			PHYSXCOLLIDER_DESC ColliderDesc = {};
			PhysXColliderDesc::Setting_DynamicCollider_WithScale(ColliderDesc, PHYSXCOLLIDER_TYPE::BOX, CL_LASER, m_pTransformCom, { 0.7f, 300.f, 0.5f }, false, nullptr, true);
			
			if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_PhysX_Collider"),
				TEXT("Com_PhysXColliderCom"), &(m_pPhysXColliderCom), &ColliderDesc)))
				return;
		}
	}
}

void CMap_Laser::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);

	m_fAccTime += fTimeDelta;


}

void CMap_Laser::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);


	//if (m_eType == NORMAL && m_fTimeAcc >= 0.8f)
	//{
	//	PxRaycastHit hit;
	//	if (GET_SINGLE(CPhysX_Manager)->RaycastMulti(shared_from_this(), CL_MONSTER_ATTACK, m_pTransformCom->Get_State(CTransform::STATE_POSITION), XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK)), 1000.f, &hit))
	//	{
	//		if (hit.actor->userData != nullptr)
	//		{
	//			CPhysX_Collider* pCollider = (CPhysX_Collider*)hit.actor->userData;

	//			if (pCollider && pCollider->Get_ColliderDesc().iFilterType == CL_PLAYER_BODY)
	//			{
	//				Status_DamageTo(m_strSkillName, pCollider, pCollider->Get_Owner(), Get_Owner());

	//				m_pGameInstance->Play_3DSound(TEXT("FF7_Resident_Battle")
	//					, { "044_SE_Stray (Par_Laser_Hit_01).wav", "045_SE_Stray (Par_Laser_Hit_01).wav", "046_SE_Stray (Par_Laser_Hit_01).wav", "047_SE_Stray (Par_Laser_Hit_01).wav" }
	//				, ESoundGroup::Effect, shared_from_this());
	//			}
	//		}
	//	} // 레이 충돌
	//}
	//else if (m_eType == FAST && m_fTimeAcc >= 0.4f)
	//{
	//	PxRaycastHit hit;
	//	if (GET_SINGLE(CPhysX_Manager)->RaycastSingle(shared_from_this(), CL_LASER, m_pTransformCom->Get_State(CTransform::STATE_POSITION), XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK)), 1000.f, hit))
	//	{
	//		if (hit.actor->userData != nullptr)
	//		{
	//			CPhysX_Collider* pCollider = (CPhysX_Collider*)hit.actor->userData;

	//			if (pCollider && pCollider->Get_ColliderDesc().iFilterType == CL_PLAYER_BODY)
	//			{
	//				Status_DamageTo(m_strSkillName, pCollider, pCollider->Get_Owner(), Get_Owner());

	//				m_pGameInstance->Play_3DSound(TEXT("FF7_Resident_Battle")
	//					, { "044_SE_Stray (Par_Laser_Hit_01).wav", "045_SE_Stray (Par_Laser_Hit_01).wav", "046_SE_Stray (Par_Laser_Hit_01).wav", "047_SE_Stray (Par_Laser_Hit_01).wav" }
	//				, ESoundGroup::Effect, shared_from_this());
	//			}
	//		}
	//	} // 레이 충돌
	//}

	if (m_eType == NORMAL && m_TimeChecker1.Update(fTimeDelta))
		Set_Dead();
	else if (m_eType == FAST && m_TimeChecker2.Update(fTimeDelta))
		Set_Dead();
}

void CMap_Laser::Before_Render(_cref_time fTimeDelta)
{
	__super::Before_Render(fTimeDelta);
}

void CMap_Laser::End_Play(_cref_time fTimeDelta)
{
	__super::End_Play(fTimeDelta);
}

HRESULT CMap_Laser::Render()
{
	return S_OK;
}

HRESULT CMap_Laser::Ready_Components(void* pArg)
{
	// 스테이터스 컴포넌트
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Status"),
		TEXT("Com_StatusCom"), &(m_pStatusCom), nullptr)))
		RETURN_EFAIL;

	return S_OK;
}

void CMap_Laser::PhysX_OnCollision_Enter(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Enter(pThisCol, pOtherCol, ContactInfo);

	if (pOtherCol->Get_ColliderDesc().iFilterType == CL_PLAYER_BODY)
	{
		Status_DamageTo(m_strSkillName, pOtherCol, pOtherCol->Get_Owner(), pThisCol->Get_Owner());

		m_pGameInstance->Play_3DSound(TEXT("FF7_Resident_Battle")
			, { "044_SE_Stray (Par_Laser_Hit_01).wav", "045_SE_Stray (Par_Laser_Hit_01).wav", "046_SE_Stray (Par_Laser_Hit_01).wav", "047_SE_Stray (Par_Laser_Hit_01).wav" }
		, ESoundGroup::Effect, shared_from_this());
	}
}

void CMap_Laser::PhysX_OnCollision_Stay(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Stay(pThisCol, pOtherCol, ContactInfo);
}

void CMap_Laser::PhysX_OnCollision_Exit(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Exit(pThisCol, pOtherCol, ContactInfo);
}


void CMap_Laser::Free()
{
	__super::Free();
}
