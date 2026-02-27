#include "stdafx.h"
#include "Aerith/Weapon/Aerith_GunArea_Bullet.h"

#include "Client_Manager.h"
#include "GameInstance.h"
#include "PartObject.h"

#include "Trail_Effect.h"
#include "Effect_Group.h"

IMPLEMENT_CREATE(CAerith_GunArea_Bullet)
IMPLEMENT_CLONE(CAerith_GunArea_Bullet, CGameObject)
CAerith_GunArea_Bullet::CAerith_GunArea_Bullet(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pDeviceContext)
	: BASE(pDevice, pDeviceContext)
{
}

CAerith_GunArea_Bullet::CAerith_GunArea_Bullet(const CAerith_GunArea_Bullet& rhs)
	: BASE(rhs)
{
}

HRESULT CAerith_GunArea_Bullet::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		RETURN_EFAIL;

	return S_OK;
}

HRESULT CAerith_GunArea_Bullet::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		RETURN_EFAIL;

	return S_OK;
}

void CAerith_GunArea_Bullet::Begin_Play(_cref_time fTimeDelta)
{
	__super::Begin_Play(fTimeDelta);

	m_fLifeTime = 5.f;

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
	PhysXColliderDesc::Setting_DynamicCollider_WithScale(ColliderDesc, PHYSXCOLLIDER_TYPE::BOX, CL_PLAYER_ATTACK, m_pTransformCom, { 0.2f, 0.2f, 0.2f }, false, nullptr, true);
	m_vPhysXColliderLocalOffset = { 0.f,0.f,0.f };



	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_PhysX_Collider"),
		TEXT("Com_PhysXColliderCom"), &(m_pPhysXColliderCom), &ColliderDesc)))
		return;

	// [여기에] 트레일 이펙트 생성 넣어주세요.
	m_pEffect = GET_SINGLE(CObjPool_Manager)->Create_Object<CTrail_Effect>(TEXT("SecuritySoldierBullet1"), L_EFFECT, nullptr, shared_from_this());
	m_pEffect->Get_TransformCom().lock()->Set_Scaling(4.f, 4.f, 4.f);
	m_pEffect->Get_TransformCom().lock()->Rotation(0.f, XMConvertToRadians(-90.f), 0.f);
}

void CAerith_GunArea_Bullet::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
	if (m_pEffect)
		m_pEffect->Get_TransformCom().lock()->Set_Look_Manual(m_pTransformCom->Get_State(CTransform::STATE_LOOK));
}

void CAerith_GunArea_Bullet::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);

	m_fLifeTime.Increase(fTimeDelta);
	m_fSpeed += fTimeDelta * 2.f;

	m_pTransformCom->Go_Straight(fTimeDelta * m_fSpeed);
}

void CAerith_GunArea_Bullet::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (m_fLifeTime.IsMax())
	{
		Set_Dead();
		m_pEffect->Set_Dead();
	}
}

void CAerith_GunArea_Bullet::Before_Render(_cref_time fTimeDelta)
{
	__super::Before_Render(fTimeDelta);
}

void CAerith_GunArea_Bullet::End_Play(_cref_time fTimeDelta)
{
	__super::End_Play(fTimeDelta);
}

HRESULT CAerith_GunArea_Bullet::Render()
{
	return S_OK;
}

HRESULT CAerith_GunArea_Bullet::Ready_Components(void* pArg)
{


	return S_OK;
}

void CAerith_GunArea_Bullet::PhysX_OnCollision_Enter(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	if (pOtherCol->Get_ColliderDesc().iFilterType & (CL_MONSTER_BODY | CL_MAP_STATIC))
	{
		/*weak_ptr<CEffect> pEffect = GET_SINGLE(CObjPool_Manager)->
			Create_Object<CEffect_Group>(TEXT("AirBursterSoulderBeamHit"), L_EFFECT, nullptr, shared_from_this());
		pEffect.lock()->Get_TransformCom().lock()->Set_Position(0.f,
			Get_TransformCom().lock()->Get_State(CTransform::STATE_POSITION));*/
		Set_Dead();
		m_pEffect->Set_Dead();
	}
}

void CAerith_GunArea_Bullet::PhysX_OnCollision_Stay(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
}

void CAerith_GunArea_Bullet::PhysX_OnCollision_Exit(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
}


void CAerith_GunArea_Bullet::Free()
{
	__super::Free();
}
