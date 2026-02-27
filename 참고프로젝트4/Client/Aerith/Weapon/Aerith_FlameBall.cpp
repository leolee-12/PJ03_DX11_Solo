#include "stdafx.h"
#include "Aerith/Weapon/Aerith_FlameBall.h"
#include "Aerith/Weapon/Aerith_FlameBlast.h"

#include "Client_Manager.h"
#include "GameInstance.h"
#include "PartObject.h"

#include "Character.h"
#include "StatusComp.h"

#include "Trail_Effect.h"
#include "Effect_Group.h"
#include "Light.h"
#include "Sound_Manager.h"

IMPLEMENT_CREATE(CAerith_FlameBall)
IMPLEMENT_CLONE(CAerith_FlameBall, CGameObject)
CAerith_FlameBall::CAerith_FlameBall(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pDeviceContext)
	: BASE(pDevice, pDeviceContext)
{
}

CAerith_FlameBall::CAerith_FlameBall(const CAerith_FlameBall& rhs)
	: BASE(rhs)
{
}

HRESULT CAerith_FlameBall::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		RETURN_EFAIL;

	return S_OK;
}

HRESULT CAerith_FlameBall::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		RETURN_EFAIL;

	return S_OK;
}

void CAerith_FlameBall::Begin_Play(_cref_time fTimeDelta)
{
	__super::Begin_Play(fTimeDelta);

	auto pTarget = GET_SINGLE(CClient_Manager)->Get_TargetMonster(AERITH);
	m_vTargetPos = pTarget.vTargetPos;


	PHYSXCOLLIDER_DESC ColliderDesc = {};
	PhysXColliderDesc::Setting_DynamicCollider_WithScale(ColliderDesc, PHYSXCOLLIDER_TYPE::BOX, CL_PLAYER_ATTACK, m_pTransformCom, { 0.3f, 0.3f, 0.3f }, false, nullptr, true);
	m_vPhysXColliderLocalOffset = { 0.f,0.f,0.f };


	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_PhysX_Collider"),
		TEXT("Com_PhysXColliderCom"), &(m_pPhysXColliderCom), &ColliderDesc)))
		return;

	// [여기에] 트레일 이펙트 생성 넣어주세요.
	/*m_pEffect = GET_SINGLE(CObjPool_Manager)
		->Create_Object<CParticle>(TEXT("AerithFlame_Ball"), L_EFFECT, nullptr, shared_from_this());*/

	GET_SINGLE(CEffect_Manager)->Create_Effect<CParticle>(TEXT("AerithFlame_Ball"), shared_from_this());

	Set_StatusComByOwner("Aerith_Ability_FlameSpell_Hit");

	// 사운드
	m_pGameInstance->Play_3DSound(TEXT("FF7_Resident_B_Fire")
		, { "019_Magic_Fire01 (Par_Fire03_Shot_01).wav" }
	, ESoundGroup::Effect, shared_from_this());
}

void CAerith_FlameBall::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);

	/*if (m_pEffect)
		m_pEffect->Get_TransformCom().lock()->Set_Look_Manual(m_pTransformCom->Get_State(CTransform::STATE_LOOK));*/
}

void CAerith_FlameBall::Tick(_cref_time fTimeDelta)
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

	if (m_fTrailTime.Update(fTimeDelta))
	{
		_float3 vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		_float3 vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);

		GET_SINGLE(CEffect_Manager)
			->Create_Hit_Effect<CEffect_Group>(L"AerithFlame_Trail", shared_from_this(),
			vPos, vLook);
	}
}

void CAerith_FlameBall::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (5.f <= m_fAccTime)
		Set_Dead();
}

void CAerith_FlameBall::Before_Render(_cref_time fTimeDelta)
{
	__super::Before_Render(fTimeDelta);
}

void CAerith_FlameBall::End_Play(_cref_time fTimeDelta)
{
	__super::End_Play(fTimeDelta);
}

HRESULT CAerith_FlameBall::Render()
{
	return S_OK;
}

HRESULT CAerith_FlameBall::Ready_Components(void* pArg)
{


	return S_OK;
}

void CAerith_FlameBall::PhysX_OnCollision_Enter(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Enter(pThisCol, pOtherCol, ContactInfo);
	if (pOtherCol->Get_ColliderDesc().iFilterType & (CL_MONSTER_BODY | CL_MAP_STATIC))
	{
		// 대미지 주는 함수, IStatusInterface 인터페이스로 구현됨.
		Status_DamageTo(m_strSkillName, pOtherCol, pOtherCol->Get_Owner(), pThisCol->Get_Owner());
		DynPtrCast<IStatusInterface>(Get_Owner().lock())->Get_StatusCom().lock()->Add_LimitBreak(1.f);

		shared_ptr<PxContactPairPoint> pContactPoint(new PxContactPairPoint[ContactInfo.contactCount]);
		PxU32 nbContacts = ContactInfo.extractContacts(pContactPoint.get(), ContactInfo.contactCount);

		shared_ptr<CAerith_FlameBlast> pBullet = { nullptr };
		m_pGameInstance->Add_CloneObject(m_pGameInstance->Get_CreateLevelIndex(), L_OBJECT,
			TEXT("Prototype_GameObject_Aerith_FlameBlast"), nullptr, &pBullet);
		pBullet->Set_Owner(m_pOwner);

		pBullet->Get_TransformCom().lock()->Set_Position(0.f, Convert_Vector(pContactPoint.get()->position));

		Set_Dead();
		//m_pEffect->Set_Dead();
	}
}

void CAerith_FlameBall::PhysX_OnCollision_Stay(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Stay(pThisCol, pOtherCol, ContactInfo);
}

void CAerith_FlameBall::PhysX_OnCollision_Exit(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Exit(pThisCol, pOtherCol, ContactInfo);
}


void CAerith_FlameBall::Free()
{
	__super::Free();
}

void CAerith_FlameBall::Create_Effect(_float3 vPos, _float3 vLook)
{
	GET_SINGLE(CEffect_Manager)->Create_Effect<CEffect_Group>(TEXT("GRP_AerithFlameBlast"), shared_from_this());

	LIGHT_DESC LightDesc = {};
	LightDesc.bUseVolumetric = true;
	LightDesc.eType = LIGHT_DESC::TYPE_POINT;
	LightDesc.fRange = 4.5f;
	LightDesc.strLightName = "LightFlameBullet" + to_string(m_pGameInstance->RandomInt(0, 10000000));
	LightDesc.vDiffuse = { 4.f, 2.2f, 2.2f, 1.f };
	LightDesc.fSpotPower = 10.f;
	LightDesc.vPosition = { vPos.x, vPos.y, vPos.z, 1.f };
	LightDesc.vAmbient = { 0.2f, 0.2f, 0.2f, 1.f };
	LightDesc.vEmissive = { 0.2f, 0.2f, 0.16f, 1.f };
	LightDesc.vSpecular = { 0.2f, 0.2f, 0.16f, 1.f };

	/*FRapidJson::Open_Json(GI()->MainJSONScriptPath() + L"Test.json")
		.Read_Data("LightTest", LightDesc.fSpotPower);*/

	shared_ptr<CLight>    pLight = nullptr;
	m_pGameInstance->Add_Light(LightDesc, &pLight);
	pLight->Set_RangeLinearDecrease(3.f);
	pLight->Set_RangeQuadDecrease(5.f);
	pLight->Set_LightDamping(3.f);
	pLight->Set_LightVolumeQuadDamping(3.f);
	pLight->Set_Dead();
}
