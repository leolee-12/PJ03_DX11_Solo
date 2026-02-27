#include "stdafx.h"
#include "Aerith/Weapon/Aerith_SorcerousStorm.h"

#include "Client_Manager.h"
#include "GameInstance.h"
#include "PartObject.h"

#include "Character.h"
#include "StatusComp.h"

#include "Trail_Effect.h"
#include "Effect_Group.h"
#include "Light.h"
#include "Sound_Manager.h"

IMPLEMENT_CREATE(CAerith_SorcerousStorm)
IMPLEMENT_CLONE(CAerith_SorcerousStorm, CGameObject)
CAerith_SorcerousStorm::CAerith_SorcerousStorm(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pDeviceContext)
	: BASE(pDevice, pDeviceContext)
{
}

CAerith_SorcerousStorm::CAerith_SorcerousStorm(const CAerith_SorcerousStorm& rhs)
	: BASE(rhs)
{
}

HRESULT CAerith_SorcerousStorm::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		RETURN_EFAIL;

	return S_OK;
}

HRESULT CAerith_SorcerousStorm::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		RETURN_EFAIL;

	return S_OK;
}

void CAerith_SorcerousStorm::Begin_Play(_cref_time fTimeDelta)
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
		m_pTransformCom, { 12.f, 8.f, 12.f }, false, nullptr, true);
	m_vPhysXColliderLocalOffset = { 0.f, 3.f, 0.f };



	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_PhysX_Collider"),
		TEXT("Com_PhysXColliderCom"), &(m_pPhysXColliderCom), &ColliderDesc)))
		return;

	_float3 vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	//_float3 vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);

	// [여기에] 트레일 이펙트 생성 넣어주세요.
	m_pEffect = GET_SINGLE(CEffect_Manager)->Create_Effect<CEffect_Group>(TEXT("GRP_AerithThunder_Spark"), shared_from_this());

	Set_StatusComByOwner("Aerith_Ability_SorcerousStorm");

	LIGHT_DESC LightDesc = {};
	LightDesc.bUseVolumetric = true;
	LightDesc.eType = LIGHT_DESC::TYPE_POINT;
	LightDesc.fRange = 6.5f;
	LightDesc.strLightName = "LightNormalBullet" + to_string(m_pGameInstance->RandomInt(0, 10000000));
	LightDesc.vDiffuse = { 2.2f, 2.2f, 3.2f, 1.f };
	LightDesc.fSpotPower = 30.f;
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

	m_pGameInstance->Play_3DSound(TEXT("FF7_Resident_Battle")
		, { "217_SE (Par_R_STElectric_01).wav" }
	, ESoundGroup::Effect, shared_from_this());
}

void CAerith_SorcerousStorm::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);

	/*if (m_pEffect)
		m_pEffect->Get_TransformCom().lock()->Set_Look_Manual(m_pTransformCom->Get_State(CTransform::STATE_LOOK));*/
}

void CAerith_SorcerousStorm::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (m_fLifeTime.Increase(fTimeDelta))
		Set_Dead();

	m_fDamageTimer.Update(fTimeDelta);
}

void CAerith_SorcerousStorm::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CAerith_SorcerousStorm::Before_Render(_cref_time fTimeDelta)
{
	__super::Before_Render(fTimeDelta);
}

void CAerith_SorcerousStorm::End_Play(_cref_time fTimeDelta)
{
	__super::End_Play(fTimeDelta);
}

HRESULT CAerith_SorcerousStorm::Render()
{
	return S_OK;
}

HRESULT CAerith_SorcerousStorm::Ready_Components(void* pArg)
{


	return S_OK;
}

void CAerith_SorcerousStorm::PhysX_OnCollision_Enter(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Enter(pThisCol, pOtherCol, ContactInfo);
}

void CAerith_SorcerousStorm::PhysX_OnCollision_Stay(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Stay(pThisCol, pOtherCol, ContactInfo);

	_bool bUseDamage = pOtherCol->Get_ColliderDesc().iFilterType == CL_MONSTER_BODY;
	if (m_fDamageTimer.IsTicked() && bUseDamage)
	{
		// 대미지 계산
		if (bUseDamage)
			Status_DamageTo(m_strSkillName, pOtherCol, pOtherCol->Get_Owner(), pThisCol->Get_Owner());
		/*DynPtrCast<IStatusInterface>(Get_Owner().lock())->Get_StatusCom().lock()->Add_LimitBreak(1.f);

		weak_ptr<CEffect> pEffect = GET_SINGLE(CObjPool_Manager)->
			Create_Object<CEffect_Group>(TEXT("AirBursterSoulderBeamHit"), L_EFFECT, nullptr, shared_from_this());
		pEffect.lock()->Get_TransformCom().lock()->Set_Position(0.f,
			Get_TransformCom().lock()->Get_State(CTransform::STATE_POSITION));*/
	}
}

void CAerith_SorcerousStorm::PhysX_OnCollision_Exit(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Exit(pThisCol, pOtherCol, ContactInfo);
}


void CAerith_SorcerousStorm::Free()
{
	__super::Free();
}
