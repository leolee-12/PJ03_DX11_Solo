#include "stdafx.h"
#include "Aerith/Weapon/Aerith_TempestDuration.h"
#include "Aerith/Weapon/Aerith_TempestBlast.h"

#include "Client_Manager.h"
#include "GameInstance.h"
#include "PartObject.h"

#include "Trail_Effect.h"
#include "Effect_Group.h"

IMPLEMENT_CREATE(CAerith_TempestDuration)
IMPLEMENT_CLONE(CAerith_TempestDuration, CGameObject)
CAerith_TempestDuration::CAerith_TempestDuration(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pDeviceContext)
	: BASE(pDevice, pDeviceContext)
{
}

CAerith_TempestDuration::CAerith_TempestDuration(const CAerith_TempestDuration& rhs)
	: BASE(rhs)
{
}

HRESULT CAerith_TempestDuration::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		RETURN_EFAIL;

	return S_OK;
}

HRESULT CAerith_TempestDuration::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		RETURN_EFAIL;

	return S_OK;
}

void CAerith_TempestDuration::Begin_Play(_cref_time fTimeDelta)
{
	__super::Begin_Play(fTimeDelta);

	/*auto pTarget = GET_SINGLE(CClient_Manager)->Get_TargetMonster(AERITH);
	m_vTargetPos = pTarget.vTargetPos;*/

	/*_float3 vRanDir = XMVector3Normalize(XMLoadFloat3(&
		m_pGameInstance->RandomFloat3({ -9.f, 4.f, -4.f }, { 9.f, 20.f, 8.f })));
	vRanDir = XMVector3TransformNormal(vRanDir, m_pOwner.lock()->Get_TransformCom().lock()->Get_WorldMatrix());
	m_pTransformCom->Set_Look_Manual(vRanDir);*/

	/*_float4x4 matWorld = DynPtrCast<CGameObject>(m_pOwner.lock())->Get_TransformCom().lock()->Get_WorldMatrix();
	_float3 vPos = _float3(matWorld._41, matWorld._42, matWorld._43);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);*/


	PHYSXCOLLIDER_DESC ColliderDesc = {};
	PhysXColliderDesc::Setting_DynamicCollider_WithScale(ColliderDesc, PHYSXCOLLIDER_TYPE::BOX, CL_PLAYER_ATTACK, 
		m_pTransformCom, { 1.2f, 1.2f, 1.2f }, false, nullptr, true);
	m_vPhysXColliderLocalOffset = { 0.f, 0.f, 0.f };



	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_PhysX_Collider"),
		TEXT("Com_PhysXColliderCom"), &(m_pPhysXColliderCom), &ColliderDesc)))
		return;

	// [여기에] 트레일 이펙트 생성 넣어주세요.
	m_pEffect = GET_SINGLE(CObjPool_Manager)
		->Create_Object<CEffect_Group>(TEXT("GRP_AerithTempestDuration"), L_EFFECT, nullptr, shared_from_this());

	Set_StatusComByOwner("Aerith_TempestHit");
}

void CAerith_TempestDuration::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);

	if (m_pEffect)
		m_pEffect->Get_TransformCom().lock()->Set_Look_Manual(m_pTransformCom->Get_State(CTransform::STATE_LOOK));
}

void CAerith_TempestDuration::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);

	m_fAccTime += fTimeDelta;
}

void CAerith_TempestDuration::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (4.5f < m_fAccTime && m_fAccTime <= 4.5f + fTimeDelta)
		Create_Blast();

	if (5.f <= m_fAccTime)
	{
		Create_Blast();
		Set_Dead();
	}

	// 일정 틱마다 대미지 (ReserveReset을 사용, 충도
	m_fDamageTiming.Increase(fTimeDelta);

	m_pTransformCom->Rotation(_float3(0.f, 1.f, 0.f), fTimeDelta);
}

void CAerith_TempestDuration::Before_Render(_cref_time fTimeDelta)
{
	__super::Before_Render(fTimeDelta);
}

void CAerith_TempestDuration::End_Play(_cref_time fTimeDelta)
{
	__super::End_Play(fTimeDelta);

	// 삭제시 
}

HRESULT CAerith_TempestDuration::Render()
{
	return S_OK;
}

HRESULT CAerith_TempestDuration::Ready_Components(void* pArg)
{


	return S_OK;
}

void CAerith_TempestDuration::PhysX_OnCollision_Enter(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Enter(pThisCol, pOtherCol, ContactInfo);

	_bool bUseDamage = false;
	if (m_fDamageTiming.IsMax() && (bUseDamage = (pOtherCol->Get_ColliderDesc().iFilterType == CL_MONSTER_BODY)
		|| (pOtherCol->Get_ColliderDesc().iFilterType == CL_MAP_STATIC)))
	{
		if (bUseDamage)
			Status_DamageTo(m_strSkillName, pOtherCol, pOtherCol->Get_Owner(), pThisCol->Get_Owner());

		/*weak_ptr<CEffect> pEffect = GET_SINGLE(CObjPool_Manager)->
			Create_Object<CEffect_Group>(TEXT("AirBursterSoulderBeamHit"), L_EFFECT, nullptr, shared_from_this());
		pEffect.lock()->Get_TransformCom().lock()->Set_Position(0.f,
			Get_TransformCom().lock()->Get_State(CTransform::STATE_POSITION));*/

		//m_fDamageTiming.ReserveReset();
	}
}

void CAerith_TempestDuration::PhysX_OnCollision_Stay(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Stay(pThisCol, pOtherCol, ContactInfo);
}

void CAerith_TempestDuration::PhysX_OnCollision_Exit(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Exit(pThisCol, pOtherCol, ContactInfo);
}


void CAerith_TempestDuration::Free()
{
	__super::Free();
}

void CAerith_TempestDuration::Create_Blast()
{
	shared_ptr<CAerith_TempestBlast> pBullet = { nullptr };
	m_pGameInstance->Add_CloneObject(m_pGameInstance->Get_CreateLevelIndex(), L_OBJECT,
		TEXT("Prototype_GameObject_Aerith_TempestBlast"), nullptr, &pBullet);
	pBullet->Set_Owner(m_pOwner.lock());

	_matrix ActorMatrix = Get_TransformCom().lock()->Get_WorldFloat4x4();
	//pBullet->Get_TransformCom().lock()->Set_Look_Manual(ActorMatrix.r[2]);
	pBullet->Get_TransformCom().lock()->Set_Position(0.f, ActorMatrix.r[3]);
}
