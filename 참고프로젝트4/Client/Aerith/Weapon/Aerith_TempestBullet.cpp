#include "stdafx.h"
#include "Aerith/Weapon/Aerith_List_Weapon.h"

#include "Client_Manager.h"
#include "GameInstance.h"
#include "PartObject.h"

#include "Character.h"
#include "StatusComp.h"

#include "Trail_Effect.h"
#include "Effect_Group.h"
#include "Sound_Manager.h"

IMPLEMENT_CREATE(CAerith_TempestBullet)
IMPLEMENT_CLONE(CAerith_TempestBullet, CGameObject)
CAerith_TempestBullet::CAerith_TempestBullet(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pDeviceContext)
	: BASE(pDevice, pDeviceContext)
{
}

CAerith_TempestBullet::CAerith_TempestBullet(const CAerith_TempestBullet& rhs)
	: BASE(rhs)
{
}

HRESULT CAerith_TempestBullet::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		RETURN_EFAIL;

	return S_OK;
}

HRESULT CAerith_TempestBullet::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		RETURN_EFAIL;

	return S_OK;
}

void CAerith_TempestBullet::Begin_Play(_cref_time fTimeDelta)
{
	__super::Begin_Play(fTimeDelta);

	auto pTarget = GET_SINGLE(CClient_Manager)->Get_TargetMonster(AERITH);
	if (!pTarget.pTarget.expired())
	{
		m_vTargetPos = pTarget.vTargetPos;
		m_vTargetPos += pTarget.pTarget.lock()->Get_PhysXColliderLocalOffset();
	}

	/*_float3 vRanDir = XMVector3Normalize(XMLoadFloat3(&
		m_pGameInstance->RandomFloat3({ -9.f, 4.f, -4.f }, { 9.f, 20.f, 8.f })));
	vRanDir = XMVector3TransformNormal(vRanDir, m_pOwner.lock()->Get_TransformCom().lock()->Get_WorldMatrix());
	m_pTransformCom->Set_Look_Manual(vRanDir);*/

	/*_float4x4 matWorld = DynPtrCast<CGameObject>(m_pOwner.lock())->Get_TransformCom().lock()->Get_WorldMatrix();
	_float3 vPos = _float3(matWorld._41, matWorld._42, matWorld._43);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);*/


	PHYSXCOLLIDER_DESC ColliderDesc = {};
	PhysXColliderDesc::Setting_DynamicCollider_WithScale(ColliderDesc, PHYSXCOLLIDER_TYPE::BOX, CL_PLAYER_ATTACK, 
		m_pTransformCom, { 0.4f, 0.4f, 0.4f }, false, nullptr, true);
	m_vPhysXColliderLocalOffset = { 0.f, 0.f, 0.f };



	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_PhysX_Collider"),
		TEXT("Com_PhysXColliderCom"), &(m_pPhysXColliderCom), &ColliderDesc)))
		return;

	// [여기에] 트레일 이펙트 생성 넣어주세요.
	m_pEffect = GET_SINGLE(CObjPool_Manager)
		->Create_Object<CEffect_Group>(TEXT("GRP_AerithTempestBullet"), L_EFFECT, nullptr, shared_from_this());

	Set_StatusComByOwner("Aerith_TempestHit");

	
}

void CAerith_TempestBullet::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
	if (m_pEffect)
		m_pEffect->Get_TransformCom().lock()->Set_Look_Manual(m_pTransformCom->Get_State(CTransform::STATE_LOOK));
}

void CAerith_TempestBullet::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);

	m_fAccTime += fTimeDelta;
	m_fAccChaseValue += fTimeDelta * 0.018f * 100.f;
	m_fSpeed += fTimeDelta * 2.f;

	auto pTarget = GET_SINGLE(CClient_Manager)->Get_TargetMonster(AERITH);
	if (!pTarget.pTarget.expired())
	{
		m_vTargetPos = pTarget.vTargetPos;
		m_vTargetPos += pTarget.pTarget.lock()->Get_PhysXColliderLocalOffset();
	}

	if (!pTarget.pTarget.expired())
		m_pTransformCom->Look_At(m_vTargetPos, m_fAccChaseValue);

	m_pTransformCom->Go_Straight(fTimeDelta * m_fSpeed);
}

void CAerith_TempestBullet::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (5.f <= m_fAccTime)
		Set_Dead();
}

void CAerith_TempestBullet::Before_Render(_cref_time fTimeDelta)
{
	__super::Before_Render(fTimeDelta);
}

void CAerith_TempestBullet::End_Play(_cref_time fTimeDelta)
{
	__super::End_Play(fTimeDelta);
}

HRESULT CAerith_TempestBullet::Render()
{
	return S_OK;
}

HRESULT CAerith_TempestBullet::Ready_Components(void* pArg)
{


	return S_OK;
}

void CAerith_TempestBullet::PhysX_OnCollision_Enter(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Enter(pThisCol, pOtherCol, ContactInfo);

	if (pOtherCol->Get_ColliderDesc().iFilterType == CL_MONSTER_BODY)
	{
		Status_DamageTo(m_strSkillName, pOtherCol, pOtherCol->Get_Owner(), pThisCol->Get_Owner());
		DynPtrCast<IStatusInterface>(Get_Owner().lock())->Get_StatusCom().lock()->Add_LimitBreak(1.f);
		Create_Duration();
		Set_Dead();
	}
}

void CAerith_TempestBullet::PhysX_OnCollision_Stay(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Stay(pThisCol, pOtherCol, ContactInfo);
}

void CAerith_TempestBullet::PhysX_OnCollision_Exit(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Exit(pThisCol, pOtherCol, ContactInfo);
}


void CAerith_TempestBullet::Free()
{
	__super::Free();
}

void CAerith_TempestBullet::Create_Duration()
{
	shared_ptr<CAerith_TempestDuration> pBullet = { nullptr };
	m_pGameInstance->Add_CloneObject(m_pGameInstance->Get_CreateLevelIndex(), L_OBJECT,
		TEXT("Prototype_GameObject_Aerith_TempestDuration"), nullptr, &pBullet);
	pBullet->Set_Owner(m_pOwner);

	_matrix ActorMatrix = Get_TransformCom().lock()->Get_WorldFloat4x4();
	_vector vLook = ActorMatrix.r[2];
	vLook.m128_f32[1] = 0.f;
	vLook = XMVector3Normalize(vLook);
	pBullet->Get_TransformCom().lock()->Set_Look_Manual(vLook);
	pBullet->Get_TransformCom().lock()->Set_Position(0.f, ActorMatrix.r[3]);

	m_pGameInstance->Play_3DSound(TEXT("FF7_Rod_Effect")
		, { "027_SE_MagicalRod (SE_AtkS001_hit; SE_AtkW001_hit; SE_AtkL001_hit).wav" }
	, ESoundGroup::Effect, shared_from_this());
	
}
