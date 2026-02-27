#include "stdafx.h"
#include "Aerith/Weapon/Aerith_GunArea_Vulcan.h"

#include "Client_Manager.h"
#include "GameInstance.h"
#include "PartObject.h"

#include "Trail_Effect.h"
#include "Effect_Group.h"

IMPLEMENT_CREATE(CAerith_GunArea_Vulcan)
IMPLEMENT_CLONE(CAerith_GunArea_Vulcan, CGameObject)
CAerith_GunArea_Vulcan::CAerith_GunArea_Vulcan(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pDeviceContext)
	: BASE(pDevice, pDeviceContext)
{
}

CAerith_GunArea_Vulcan::CAerith_GunArea_Vulcan(const CAerith_GunArea_Vulcan& rhs)
	: BASE(rhs)
{
}

HRESULT CAerith_GunArea_Vulcan::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		RETURN_EFAIL;

	return S_OK;
}

HRESULT CAerith_GunArea_Vulcan::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		RETURN_EFAIL;

	

	return S_OK;
}

void CAerith_GunArea_Vulcan::Begin_Play(_cref_time fTimeDelta)
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
	PhysXColliderDesc::Setting_DynamicCollider_WithScale(ColliderDesc, PHYSXCOLLIDER_TYPE::BOX, CL_PLAYER_ATTACK, 
		m_pTransformCom, { 1.2f, 1.2f, 1.2f }, false, nullptr, true);
	m_vPhysXColliderLocalOffset = { 0.f, 0.f, 0.f };



	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_PhysX_Collider"),
		TEXT("Com_PhysXColliderCom"), &(m_pPhysXColliderCom), &ColliderDesc)))
		return;

	// [여기에] 트레일 이펙트 생성 넣어주세요.
	//m_pEffect = GET_SINGLE(CObjPool_Manager)->Create_Object<CTrail_Effect>(TEXT("SecuritySoldierBullet1"), L_EFFECT, nullptr, shared_from_this());
}

void CAerith_GunArea_Vulcan::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);

	/*if (m_pEffect)
		m_pEffect->Get_TransformCom().lock()->Set_Look_Manual(m_pTransformCom->Get_State(CTransform::STATE_LOOK));*/
}

void CAerith_GunArea_Vulcan::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);

	m_fLifeTime.Increase(fTimeDelta);

	if (m_fBulletShootPulse.Update(fTimeDelta))
	{
		Create_Bullet();
	}
}

void CAerith_GunArea_Vulcan::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (m_fLifeTime.IsMax())
		Set_Dead();
}

void CAerith_GunArea_Vulcan::Before_Render(_cref_time fTimeDelta)
{
	__super::Before_Render(fTimeDelta);
}

void CAerith_GunArea_Vulcan::End_Play(_cref_time fTimeDelta)
{
	__super::End_Play(fTimeDelta);
}

HRESULT CAerith_GunArea_Vulcan::Render()
{
	return S_OK;
}

HRESULT CAerith_GunArea_Vulcan::Ready_Components(void* pArg)
{


	return S_OK;
}

void CAerith_GunArea_Vulcan::PhysX_OnCollision_Enter(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	/*if (pOtherCol->Get_ColliderDesc().iFilterType == CL_MONSTER_BODY
		|| pOtherCol->Get_ColliderDesc().iFilterType == CL_MAP_STATIC)
	{
		weak_ptr<CEffect> pEffect = GET_SINGLE(CObjPool_Manager)->
			Create_Object<CEffect_Group>(TEXT("AirBursterSoulderBeamHit"), L_EFFECT, nullptr, shared_from_this());
		pEffect.lock()->Get_TransformCom().lock()->Set_Position(0.f,
			Get_TransformCom().lock()->Get_State(CTransform::STATE_POSITION));
		Set_Dead();
	}*/
}

void CAerith_GunArea_Vulcan::PhysX_OnCollision_Stay(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
}

void CAerith_GunArea_Vulcan::PhysX_OnCollision_Exit(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
}


void CAerith_GunArea_Vulcan::Free()
{
	__super::Free();
}

void CAerith_GunArea_Vulcan::Create_Bullet()
{
	// 벌컨 생성
	shared_ptr<CAerith_GunArea_Vulcan> pBullet = { nullptr };
	m_pGameInstance->Add_CloneObject(m_pGameInstance->Get_CreateLevelIndex(), L_OBJECT,
		TEXT("Prototype_GameObject_Aerith_GunArea_Bullet"), nullptr, &pBullet);
	pBullet->Set_Owner(m_pOwner);

	_matrix ActorMatrix = Get_TransformCom().lock()->Get_WorldFloat4x4();
	//SocketMatrix *= ActorMatrix;
	ActorMatrix.r[3] += XMVector3Normalize(ActorMatrix.r[2]) * 0.5f;

	pBullet->Get_TransformCom().lock()->Set_Look_Manual(ActorMatrix.r[2]);
	pBullet->Get_TransformCom().lock()->Set_Position(0.f, ActorMatrix.r[3]);
}
