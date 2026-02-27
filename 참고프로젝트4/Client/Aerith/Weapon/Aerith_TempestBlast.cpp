#include "stdafx.h"
#include "Aerith/Weapon/Aerith_TempestBlast.h"

#include "Client_Manager.h"
#include "GameInstance.h"
#include "PartObject.h"

#include "Trail_Effect.h"
#include "Effect_Group.h"
#include "Sound_Manager.h"

IMPLEMENT_CREATE(CAerith_TempestBlast)
IMPLEMENT_CLONE(CAerith_TempestBlast, CGameObject)
CAerith_TempestBlast::CAerith_TempestBlast(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pDeviceContext)
	: BASE(pDevice, pDeviceContext)
{
}

CAerith_TempestBlast::CAerith_TempestBlast(const CAerith_TempestBlast& rhs)
	: BASE(rhs)
{
}

HRESULT CAerith_TempestBlast::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		RETURN_EFAIL;

	return S_OK;
}

HRESULT CAerith_TempestBlast::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		RETURN_EFAIL;

	return S_OK;
}

void CAerith_TempestBlast::Begin_Play(_cref_time fTimeDelta)
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
	_float3 vPos = Get_TransformCom().lock()->Get_State(CTransform::STATE_POSITION);
	_float3 vLook = Get_TransformCom().lock()->Get_State(CTransform::STATE_LOOK);
	GET_SINGLE(CEffect_Manager)->Create_Effect<CParticle>(L"GRP_AerithTempest_Blast", shared_from_this());
	/**/

	Set_StatusComByOwner("Aerith_TempestBlast");
}

void CAerith_TempestBlast::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);

	if (m_pEffect)
		m_pEffect->Get_TransformCom().lock()->Set_Look_Manual(m_pTransformCom->Get_State(CTransform::STATE_LOOK));
}

void CAerith_TempestBlast::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);

	m_fAccTime += fTimeDelta;
}

void CAerith_TempestBlast::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (1.f <= m_fAccTime)
		Set_Dead();

	// 일정 틱마다 대미지 (ReserveReset을 사용, 충도
	if (m_fDamageTiming.Increase(fTimeDelta))
	{
		m_pGameInstance->Play_3DSound(TEXT("FF7_Resident_Battle")
			, { "079_SE (Par_Item_Grenade_01).wav", "080_SE (Par_Item_Grenade_01).wav", "081_SE (Par_Item_Grenade_01).wav", "082_SE (Par_Item_Grenade_01).wav" }
		, ESoundGroup::Effect, shared_from_this());
	}
}

void CAerith_TempestBlast::Before_Render(_cref_time fTimeDelta)
{
	__super::Before_Render(fTimeDelta);
}

void CAerith_TempestBlast::End_Play(_cref_time fTimeDelta)
{
	__super::End_Play(fTimeDelta);

	// 삭제시 
}

HRESULT CAerith_TempestBlast::Render()
{
	return S_OK;
}

HRESULT CAerith_TempestBlast::Ready_Components(void* pArg)
{


	return S_OK;
}

void CAerith_TempestBlast::PhysX_OnCollision_Enter(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
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

void CAerith_TempestBlast::PhysX_OnCollision_Stay(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Stay(pThisCol, pOtherCol, ContactInfo);
}

void CAerith_TempestBlast::PhysX_OnCollision_Exit(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Exit(pThisCol, pOtherCol, ContactInfo);
}


void CAerith_TempestBlast::Free()
{
	__super::Free();
}
