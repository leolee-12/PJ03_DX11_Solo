#include "stdafx.h"
#include "Aerith/Weapon/Aerith_NormalBullet.h"

#include "Client_Manager.h"
#include "GameInstance.h"
#include "PartObject.h"
#include "Character.h"
#include "StatusComp.h"

#include "Trail_Effect.h"
#include "Effect_Group.h"
#include "UI_Manager.h"
#include "Light.h"
#include "Sound_Manager.h"

IMPLEMENT_CREATE(CAerith_NormalBullet)
IMPLEMENT_CLONE(CAerith_NormalBullet, CGameObject)
CAerith_NormalBullet::CAerith_NormalBullet(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pDeviceContext)
	: BASE(pDevice, pDeviceContext)
{
}

CAerith_NormalBullet::CAerith_NormalBullet(const CAerith_NormalBullet& rhs)
	: BASE(rhs)
{
}

HRESULT CAerith_NormalBullet::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		RETURN_EFAIL;

	return S_OK;
}

HRESULT CAerith_NormalBullet::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		RETURN_EFAIL;

	return S_OK;
}

void CAerith_NormalBullet::Begin_Play(_cref_time fTimeDelta)
{
	__super::Begin_Play(fTimeDelta);

	auto pTarget = GET_SINGLE(CClient_Manager)->Get_TargetMonster(AERITH);
	if (!pTarget.pTarget.expired())
	{
		m_vTargetPos = pTarget.vTargetPos;
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
		m_pTransformCom, { 0.2f, 0.2f, 0.2f }, false, nullptr, true);
	m_vPhysXColliderLocalOffset = { 0.f,0.f,0.f };



	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_PhysX_Collider"),
		TEXT("Com_PhysXColliderCom"), &(m_pPhysXColliderCom), &ColliderDesc)))
		return;

	// [여기에] 트레일 이펙트 생성 넣어주세요.
	/*m_pEffect = GET_SINGLE(CObjPool_Manager)
		->Create_Object<CEffect_Group>(TEXT("GRP_AerithNormalBullet"), L_EFFECT, nullptr, shared_from_this());*/
	m_pEffect = GET_SINGLE(CEffect_Manager)->Create_Effect<CEffect_Group>(TEXT("GRP_AerithNormalBullet"), shared_from_this());

	m_pGameInstance->Play_3DSound(TEXT("FF7_Aerith_Effect")
		, { "074_SE_Aerith (par_Shoot_Bullet_01).wav" }
	, ESoundGroup::Effect, shared_from_this());
}

void CAerith_NormalBullet::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
	/*if (m_pEffect)
		m_pEffect->Get_TransformCom().lock()->Set_Look_Manual(m_pTransformCom->Get_State(CTransform::STATE_LOOK));*/
}

void CAerith_NormalBullet::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);

	m_fAccChaseValue += fTimeDelta;
	m_fSpeed += fTimeDelta * 2.f;

	if (m_fLifeTime.Increase(fTimeDelta))
	{
		Set_Dead();
		if(m_pEffect)
			m_pEffect->Set_Dead();
		Create_Effect(m_pTransformCom->Get_State(CTransform::STATE_POSITION), m_pTransformCom->Get_State(CTransform::STATE_LOOK));
	}

	auto pTarget = GET_SINGLE(CClient_Manager)->Get_TargetMonster(AERITH);
	if (!pTarget.pTarget.expired())
	{
		m_vTargetPos = pTarget.vTargetPos;
		m_vTargetPos += pTarget.pTarget.lock()->Get_PhysXColliderLocalOffset();
	}

	if (m_fTrailCreate_MinTime.Increase(fTimeDelta))
	{
		if (m_fTrailCreate_Time.Update(fTimeDelta))
		{
			_float3 vPos = Get_TransformCom().lock()->Get_State(CTransform::STATE_POSITION);
			_float3 vLook = Get_TransformCom().lock()->Get_State(CTransform::STATE_LOOK);
			GET_SINGLE(CEffect_Manager)
				->Create_Hit_Effect<CParticle>(L"AerithBulletTrail", shared_from_this(), vPos, vLook);
		}
	}

	if (!pTarget.pTarget.expired())
		m_pTransformCom->Look_At(m_vTargetPos, m_fAccChaseValue);

	if (m_fRotateTime.Update(fTimeDelta))
	{
		_vector vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
		_vector vUp = m_pTransformCom->Get_State(CTransform::STATE_UP);
		{
			_float fRotate = XMConvertToRadians(m_pGameInstance->Random(-10.f, 10.f));
			m_pTransformCom->Rotation(vRight, fRotate);
		}
		{
			_float fRotate = XMConvertToRadians(m_pGameInstance->Random(-10.f, 10.f));
			m_pTransformCom->Rotation(vUp, fRotate);
		}
	}

	m_pTransformCom->Go_Straight(fTimeDelta * m_fSpeed, nullptr);
}

void CAerith_NormalBullet::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CAerith_NormalBullet::Before_Render(_cref_time fTimeDelta)
{
	__super::Before_Render(fTimeDelta);
}

void CAerith_NormalBullet::End_Play(_cref_time fTimeDelta)
{
	__super::End_Play(fTimeDelta);
}

HRESULT CAerith_NormalBullet::Render()
{
	return S_OK;
}

HRESULT CAerith_NormalBullet::Ready_Components(void* pArg)
{

	return S_OK;
}

void CAerith_NormalBullet::PhysX_OnCollision_Enter(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Enter(pThisCol, pOtherCol, ContactInfo);

	// 적과 충돌할 때
	if (pOtherCol->Get_ColliderDesc().iFilterType == CL_MONSTER_BODY)
	{
		// 대미지 주는 함수, IStatusInterface 인터페이스로 구현됨.
		Status_DamageTo(m_strSkillName, pOtherCol, pOtherCol->Get_Owner(), pThisCol->Get_Owner());
		Get_StatusCom().lock()->Add_LimitBreak(3.f);

		shared_ptr<PxContactPairPoint> pContactPoint(new PxContactPairPoint[ContactInfo.contactCount]);
		PxU32 nbContacts = ContactInfo.extractContacts(pContactPoint.get(), ContactInfo.contactCount);

		if (nbContacts >= 1)
		{
			Create_Effect(Convert_Vector(pContactPoint.get()->position), Convert_Vector(pContactPoint.get()->normal));
		}
		
		
		Set_Dead();
		if(m_pEffect)
			m_pEffect->Set_Dead();
	}
}

void CAerith_NormalBullet::PhysX_OnCollision_Stay(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Stay(pThisCol, pOtherCol, ContactInfo);

	if (pOtherCol->Get_ColliderDesc().iFilterType == CL_MONSTER_BODY)
	{
		// 대미지 주는 함수, IStatusInterface 인터페이스로 구현됨.
		Status_DamageTo(m_strSkillName, pOtherCol, pOtherCol->Get_Owner(), pThisCol->Get_Owner());
		Get_StatusCom().lock()->Add_LimitBreak(1.f);

		shared_ptr<PxContactPairPoint> pContactPoint(new PxContactPairPoint[ContactInfo.contactCount]);
		PxU32 nbContacts = ContactInfo.extractContacts(pContactPoint.get(), ContactInfo.contactCount);

		if (nbContacts >= 1)
		{
			Create_Effect(Convert_Vector(pContactPoint.get()->position), Convert_Vector(pContactPoint.get()->normal));
		}

		Set_Dead();
		if(m_pEffect)
			m_pEffect->Set_Dead();
	}
}

void CAerith_NormalBullet::PhysX_OnCollision_Exit(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Exit(pThisCol, pOtherCol, ContactInfo);
}


void CAerith_NormalBullet::Free()
{
	__super::Free();
}

void CAerith_NormalBullet::Create_Effect(_float3 vPos, _float3 vLook)
{
	// 이펙트
	weak_ptr<CParticle> pEffect;
	GET_SINGLE(CEffect_Manager)->Create_Hit_Effect<CParticle>(L"AerithBulletHit", shared_from_this(),
		vPos, vLook);

	GET_SINGLE(CEffect_Manager)->Create_Hit_Effect<CParticle>(L"AerithBulletDistortion", shared_from_this(),
		vPos, vLook);

	// 라이트
	LIGHT_DESC LightDesc = {};
	LightDesc.bUseVolumetric = true;
	LightDesc.eType = LIGHT_DESC::TYPE_POINT;
	LightDesc.fRange = 1.75f;
	LightDesc.strLightName = "LightNormalBullet" + to_string(m_pGameInstance->RandomInt(0, 10000000));
	LightDesc.vDiffuse = { 2.f, 2.2f, 2.2f, 1.f };
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

	// 사운드
	m_pGameInstance->Play_3DSound(TEXT("FF7_Aerith_Effect")
		, { "075_SE_Aerith (par_Shoot_Hit_01).wav", "076_SE_Aerith (par_Shoot_Hit_01).wav" }
	, ESoundGroup::Effect, shared_from_this());
}
