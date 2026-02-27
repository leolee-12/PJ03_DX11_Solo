#include "stdafx.h"
#include "Aerith/Weapon/Aerith_LustrousShield.h"

#include "Client_Manager.h"
#include "GameInstance.h"
#include "PartObject.h"

#include "Character.h"
#include "StatusComp.h"

#include "Trail_Effect.h"
#include "Effect_Group.h"

IMPLEMENT_CREATE(CAerith_LustrousShield)
IMPLEMENT_CLONE(CAerith_LustrousShield, CGameObject)
CAerith_LustrousShield::CAerith_LustrousShield(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pDeviceContext)
	: BASE(pDevice, pDeviceContext)
{
}

CAerith_LustrousShield::CAerith_LustrousShield(const CAerith_LustrousShield& rhs)
	: BASE(rhs)
{
}

HRESULT CAerith_LustrousShield::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		RETURN_EFAIL;

	return S_OK;
}

HRESULT CAerith_LustrousShield::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		RETURN_EFAIL;

	return S_OK;
}

void CAerith_LustrousShield::Begin_Play(_cref_time fTimeDelta)
{
	__super::Begin_Play(fTimeDelta);

	auto pTarget = GET_SINGLE(CClient_Manager)->Get_TargetMonster(AERITH);
	m_vTargetPos = pTarget.vTargetPos;

	/*_float3 vRanDir = XMVector3Normalize(XMLoadFloat3(&
		m_pGameInstance->RandomFloat3({ -9.f, 4.f, -4.f }, { 9.f, 20.f, 8.f })));
	vRanDir = XMVector3TransformNormal(vRanDir, m_pOwner.lock()->Get_TransformCom().lock()->Get_WorldMatrix());
	m_pTransformCom->Set_Look_Manual(vRanDir);*/

	/*_float4x4 matWorld = DynPtrCast<CGameObject>(m_pOwner.lock())->Get_TransformCom().lock()->Get_WorldMatrix();
	_float3 vPos = _float3(matWorld._41, matWorld._42, matWorld._43);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);*/


	PHYSXCOLLIDER_DESC ColliderDesc = {};
	PhysXColliderDesc::Setting_DynamicCollider_WithScale(ColliderDesc, PHYSXCOLLIDER_TYPE::BOX, CL_PLAYER_BODY | CL_PLAYER_ATTACK, 
		m_pTransformCom, { 1.2f, 0.2f, 1.2f }, false, nullptr, true);
	m_vPhysXColliderLocalOffset = { 0.f,0.f,0.f };



	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_PhysX_Collider"),
		TEXT("Com_PhysXColliderCom"), &(m_pPhysXColliderCom), &ColliderDesc)))
		return;

	// [여기에] 트레일 이펙트 생성 넣어주세요.
	//m_pEffect = GET_SINGLE(CObjPool_Manager)->Create_Object<CTrail_Effect>(TEXT("AerithNormalBullet"), L_EFFECT, nullptr, shared_from_this());
	GET_SINGLE(CEffect_Manager)->Create_Effect<CTrail_Effect>(TEXT("AerithNormalBullet"), shared_from_this());
}

void CAerith_LustrousShield::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
	/*if (m_pEffect)
		m_pEffect->Get_TransformCom().lock()->Set_Look_Manual(m_pTransformCom->Get_State(CTransform::STATE_LOOK));*/
}

void CAerith_LustrousShield::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);

	m_fAccTime += fTimeDelta;
	m_fAccChaseValue += fTimeDelta * 0.018f * 100.f;
	m_fSpeed += fTimeDelta * 2.f;
}

void CAerith_LustrousShield::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (5.f <= m_fAccTime)
		Set_Dead();

	m_fDamageAttack.Increase(fTimeDelta);
}

void CAerith_LustrousShield::Before_Render(_cref_time fTimeDelta)
{
	__super::Before_Render(fTimeDelta);
}

void CAerith_LustrousShield::End_Play(_cref_time fTimeDelta)
{
	__super::End_Play(fTimeDelta);
}

HRESULT CAerith_LustrousShield::Render()
{
	return S_OK;
}

HRESULT CAerith_LustrousShield::Ready_Components(void* pArg)
{


	return S_OK;
}

void CAerith_LustrousShield::PhysX_OnCollision_Enter(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Enter(pThisCol, pOtherCol, ContactInfo);
	if (m_fDamageAttack.IsMax() 
		&& (pOtherCol->Get_ColliderDesc().iFilterType & (CL_MONSTER_BODY | CL_MAP_STATIC)))
	{
		// 대미지 주는 함수, IStatusInterface 인터페이스로 구현됨.
		Status_DamageTo(m_strSkillName, pOtherCol, pOtherCol->Get_Owner(), pThisCol->Get_Owner());
		DynPtrCast<IStatusInterface>(Get_Owner().lock())->Get_StatusCom().lock()->Add_LimitBreak(1.f);

		weak_ptr<CEffect> pEffect = GET_SINGLE(CObjPool_Manager)->
			Create_Object<CEffect_Group>(TEXT("AirBursterSoulderBeamHit"), L_EFFECT, nullptr, shared_from_this());
		pEffect.lock()->Get_TransformCom().lock()->Set_Position(0.f,
			Get_TransformCom().lock()->Get_State(CTransform::STATE_POSITION));

		m_fDamageAttack.Reset();
	}
}

void CAerith_LustrousShield::PhysX_OnCollision_Stay(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Stay(pThisCol, pOtherCol, ContactInfo);
}

void CAerith_LustrousShield::PhysX_OnCollision_Exit(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Exit(pThisCol, pOtherCol, ContactInfo);
}


void CAerith_LustrousShield::Free()
{
	__super::Free();
}
