#include "stdafx.h"
#include "Aerith/Weapon/Aerith_RayOfJudge.h"

#include "Client_Manager.h"
#include "GameInstance.h"
#include "PartObject.h"

#include "Character.h"
#include "StatusComp.h"

#include "Trail_Effect.h"
#include "Effect_Group.h"
#include "Effect_Manager.h"
#include "PhysX_Manager.h"
#include "Sound_Manager.h"

IMPLEMENT_CREATE(CAerith_RayOfJudge)
IMPLEMENT_CLONE(CAerith_RayOfJudge, CGameObject)
CAerith_RayOfJudge::CAerith_RayOfJudge(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pDeviceContext)
	: BASE(pDevice, pDeviceContext)
{
}

CAerith_RayOfJudge::CAerith_RayOfJudge(const CAerith_RayOfJudge& rhs)
	: BASE(rhs)
{
}

HRESULT CAerith_RayOfJudge::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		RETURN_EFAIL;

	return S_OK;
}

HRESULT CAerith_RayOfJudge::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		RETURN_EFAIL;

	return S_OK;
}

void CAerith_RayOfJudge::Begin_Play(_cref_time fTimeDelta)
{
	__super::Begin_Play(fTimeDelta);

	auto pTarget = GET_SINGLE(CClient_Manager)->Get_TargetMonster(AERITH);
	if (!pTarget.pTarget.expired())
	{
		m_vTargetPos = pTarget.vTargetPos;
		m_vTargetPos += pTarget.pTarget.lock()->Get_PhysXColliderLocalOffset();
	}

	_vector vLook = m_pOwner.lock()->Get_TransformCom().lock()->Get_State(CTransform::STATE_LOOK);

	/*_float3 vRanDir = XMVector3Normalize(XMLoadFloat3(&
		m_pGameInstance->RandomFloat3({ -9.f, 4.f, -4.f }, { 9.f, 20.f, 8.f })));
	vRanDir = XMVector3TransformNormal(vRanDir, m_pOwner.lock()->Get_TransformCom().lock()->Get_WorldMatrix());
	m_pTransformCom->Set_Look_Manual(vRanDir);*/

	/*_float4x4 matWorld = DynPtrCast<CGameObject>(m_pOwner.lock())->Get_TransformCom().lock()->Get_WorldMatrix();
	_float3 vPos = _float3(matWorld._41, matWorld._42, matWorld._43);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);*/


	m_pTransformCom->Set_Speed(1.f);

	PHYSXCOLLIDER_DESC ColliderDesc = {};
	PhysXColliderDesc::Setting_DynamicCollider_WithScale(ColliderDesc, PHYSXCOLLIDER_TYPE::BOX, 
		CL_PLAYER_ATTACK, m_pTransformCom, { 0.2f, 0.2f, 0.2f }, false, nullptr, true);
	m_vPhysXColliderLocalOffset = { 0.f,0.f,0.f };

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_PhysX_Collider"),
		TEXT("Com_PhysXColliderCom"), &(m_pPhysXColliderCom), &ColliderDesc)))
		return;

	// [여기에] 트레일 이펙트 생성 넣어주세요.
	//m_pEffect = GET_SINGLE(CObjPool_Manager)->Create_Object<CEffect_Group>(TEXT("AirBursterFingerBeam"), L_EFFECT, nullptr, shared_from_this());
	m_pEffect = GET_SINGLE(CEffect_Manager)->Create_Effect<CEffect_Group>(TEXT("GRP_AerithRay_Beam"), shared_from_this());

	Set_StatusComByOwner("Aerith_Ability_RayOfJudge");

	m_fLifeTime = 2.5f;

	m_pGameInstance->Play_3DSound(TEXT("FF7_Resident_Effect")
		, { "217_SE (Par_R_STElectric_01).wav" }
	, ESoundGroup::Effect, shared_from_this());
}

void CAerith_RayOfJudge::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);

	if (m_pEffect && m_pEffect->Get_IsEffectDead())
		Set_Dead();
}

void CAerith_RayOfJudge::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);

	// 라이프 타임
	if (m_fLifeTime.Increase(fTimeDelta))
	{
		Set_Dead();

		if(m_pEffect)
			m_pEffect->Set_Dead();
	}

	// 대미지 간격
	m_fDamageTick.Update(fTimeDelta);

	// 오너의 위치에 붙이기
	if (!m_pOwner.expired())
	{
		auto pPart = m_pOwner.lock()->Find_PartObject(TEXT("Part_Weapon"));
		if (pPart)
		{
			_matrix SocketMatrix = pPart->Get_WorldMatrixFromBone(TEXT("L_RodAGimmickS_End"));
			_matrix ActorMatrix = m_pOwner.lock()->Get_ModelCom().lock()->
				Get_BoneTransformMatrixWithParents(TEXT("R_Weapon_a"));
			ActorMatrix *= SocketMatrix;
			m_pTransformCom->Set_Position(fTimeDelta, SocketMatrix.r[3]);
		}
	}

	// 적 추적 및 Look 방향 설정
	auto pTarget = GET_SINGLE(CClient_Manager)->Get_TargetMonster(AERITH);
	if (!pTarget.pTarget.expired())
	{
		m_vTargetPos = pTarget.vTargetPos;
		m_vTargetPos += pTarget.pTarget.lock()->Get_PhysXColliderLocalOffset();
	}

	if (!pTarget.pTarget.expired())
		m_pTransformCom->Look_At(m_vTargetPos, fTimeDelta);
}

void CAerith_RayOfJudge::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	PxRaycastHit hit;
	if (GET_SINGLE(CPhysX_Manager)->RaycastSingle(shared_from_this(), CL_PLAYER_ATTACK,
		m_pTransformCom->Get_State(CTransform::STATE_POSITION), XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK)), 1000.f, hit))
	{
		if (m_fDamageTick.IsTicked() && hit.actor->userData != nullptr)
		{
			CPhysX_Collider* pCollider = (CPhysX_Collider*)hit.actor->userData;
			if (pCollider->Get_ColliderDesc().iFilterType == CL_MONSTER_BODY)
			{
				Status_DamageTo(m_strSkillName, pCollider, pCollider->Get_Owner(), Get_Owner());

				GET_SINGLE(CEffect_Manager)->Create_Effect<CParticle>(TEXT("AerithRay_HitParticle"), shared_from_this(), CEffect::USE_NONE, Convert_Vector(hit.position));
			}
			DynPtrCast<IStatusInterface>(Get_Owner().lock())->Get_StatusCom().lock()->Add_LimitBreak(1.f);
		}
	} // 레이 충돌
}

void CAerith_RayOfJudge::Before_Render(_cref_time fTimeDelta)
{
	__super::Before_Render(fTimeDelta);
}

void CAerith_RayOfJudge::End_Play(_cref_time fTimeDelta)
{
	__super::End_Play(fTimeDelta);
}

HRESULT CAerith_RayOfJudge::Render()
{
	return S_OK;
}

HRESULT CAerith_RayOfJudge::Ready_Components(void* pArg)
{
	return S_OK;
}

void CAerith_RayOfJudge::PhysX_OnCollision_Enter(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Enter(pThisCol, pOtherCol, ContactInfo);

}

void CAerith_RayOfJudge::PhysX_OnCollision_Stay(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Stay(pThisCol, pOtherCol, ContactInfo);
}

void CAerith_RayOfJudge::PhysX_OnCollision_Exit(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Exit(pThisCol, pOtherCol, ContactInfo);
}

void CAerith_RayOfJudge::Free()
{
	__super::Free();
}
