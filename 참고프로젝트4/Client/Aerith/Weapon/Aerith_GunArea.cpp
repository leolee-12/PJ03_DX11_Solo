#include "stdafx.h"
#include "Aerith/Weapon/Aerith_GunArea.h"
#include "Aerith/Weapon/Aerith_List_Weapon.h"

#include "Client_Manager.h"
#include "GameInstance.h"
#include "PartObject.h"

#include "Trail_Effect.h"
#include "Effect_Group.h"
#include "Light.h"
#include "Sound_Manager.h"

IMPLEMENT_CREATE(CAerith_GunArea)
IMPLEMENT_CLONE(CAerith_GunArea, CGameObject)
CAerith_GunArea::CAerith_GunArea(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pDeviceContext)
	: BASE(pDevice, pDeviceContext)
{
}

CAerith_GunArea::CAerith_GunArea(const CAerith_GunArea& rhs)
	: BASE(rhs)
{
}

HRESULT CAerith_GunArea::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		RETURN_EFAIL;

	return S_OK;
}

HRESULT CAerith_GunArea::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		RETURN_EFAIL;

	return S_OK;
}

void CAerith_GunArea::Begin_Play(_cref_time fTimeDelta)
{
	__super::Begin_Play(fTimeDelta);

	auto pTarget = GET_SINGLE(CClient_Manager)->Get_TargetMonster(AERITH);
	m_vTargetPos = pTarget.vTargetPos;

	if (!pTarget.pTarget.expired())
		Get_TransformCom().lock()->Set_Position(0.f, m_vTargetPos);

	/*_float3 vRanDir = XMVector3Normalize(XMLoadFloat3(&
		m_pGameInstance->RandomFloat3({ -9.f, 4.f, -4.f }, { 9.f, 20.f, 8.f })));
	vRanDir = XMVector3TransformNormal(vRanDir, m_pOwner.lock()->Get_TransformCom().lock()->Get_WorldMatrix());
	m_pTransformCom->Set_Look_Manual(vRanDir);*/

	/*_float4x4 matWorld = DynPtrCast<CGameObject>(m_pOwner.lock())->Get_TransformCom().lock()->Get_WorldMatrix();
	_float3 vPos = _float3(matWorld._41, matWorld._42, matWorld._43);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);*/


	PHYSXCOLLIDER_DESC ColliderDesc = {};
	PhysXColliderDesc::Setting_DynamicCollider_WithScale(ColliderDesc, PHYSXCOLLIDER_TYPE::SPHERE, 
		CL_PLAYER_ATTACK, m_pTransformCom, { 10.f, 10.f, 10.f }, false, nullptr, true);
	m_vPhysXColliderLocalOffset = { 0.f,0.f,0.f };

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_PhysX_Collider"),
		TEXT("Com_PhysXColliderCom"), &(m_pPhysXColliderCom), &ColliderDesc)))
		return;

	// [여기에] 트레일 이펙트 생성 넣어주세요.
	_float3 vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	m_pEffect = GET_SINGLE(CEffect_Manager)->Create_Effect<CParticle>(TEXT("AerithDisaster_Rain"), shared_from_this());
	m_pDisasterEffect = GET_SINGLE(CEffect_Manager)->Create_Effect<CEffect_Group>(TEXT("GRP_AerithDisaster"), shared_from_this());

	LIGHT_DESC LightDesc = {};
	LightDesc.bUseVolumetric = true;
	LightDesc.eType = LIGHT_DESC::TYPE_POINT;
	LightDesc.fRange = 20.f;
	LightDesc.strLightName = "LightFlameBullet" + to_string(m_pGameInstance->RandomInt(0, 10000000));
	LightDesc.vDiffuse = { 0.4f, 0.4f, 0.7f, 1.f };
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
	pLight->Set_RangeQuadDecrease(3.f);
	pLight->Set_LightDamping(3.f);
	pLight->Set_LightVolumeQuadDamping(3.f);
	pLight->Set_Dead();

	Set_StatusComByOwner("Aerith_Limit_NaturalDisaster_Elec");

	m_fLifeTime = 6.5f;

	m_pGameInstance->Play_3DSound(TEXT("FF7_Resident_Effect")
		, { "217_SE (Par_R_STElectric_01).wav" }
	, ESoundGroup::Effect, shared_from_this());

	m_pGameInstance->Play_3DSound(TEXT("FF7_Resident_B_Fire")
		, { "016_Magic_Fire01 (Par_Fire03_Hit_01).wav", "017_Magic_Fire01 (Par_Fire03_Hit_02).wav", "018_Magic_Fire01 (Par_Fire03_Hit_03).wav" }
	, ESoundGroup::Effect, shared_from_this());
}

void CAerith_GunArea::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
	if (m_pEffect)
		m_pEffect->Get_TransformCom().lock()->Set_Look_Manual(m_pTransformCom->Get_State(CTransform::STATE_LOOK));
}

void CAerith_GunArea::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);

	//if (m_fLifeTime.Increase_UpToPointOnce(fTimeDelta, 3.f))
	//{
	//	_matrix Transform = Get_TransformCom().lock()->Get_WorldMatrix();
	//	// 1층 벌칸 생성
	//	for (_uint i = 0; i < 12; i++)
	//	{
	//		_float fRadian = XMConvertToRadians(i * 30.f);
	//		_vector vRot = XMQuaternionRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), fRadian);
	//		_matrix RotMatrix = XMMatrixRotationQuaternion(vRot);
	//		_vector vLook = XMVector3TransformNormal(XMVectorSet(0.f, 0.f, 1.f, 0.f), RotMatrix);
	//		_vector vRight = XMVector3Cross(vLook, XMVectorSet(0.f, 1.f, 0.f, 0.f));
	//		_vector vRot2 = XMQuaternionRotationAxis(vRight, XMConvertToRadians(20.f));
	//		_matrix RotMatrix2 = XMMatrixRotationQuaternion(vRot2);
	//		vLook = XMVector3TransformNormal(vLook, RotMatrix2);

	//		Create_Vulcan(Transform.r[3] + vLook * m_fRadius, -vLook);
	//	}

	//	// 2층 벌칸 생성
	//	for (_uint i = 0; i < 6; i++)
	//	{
	//		_float fRadian = XMConvertToRadians(i * 60.f);
	//		_vector vRot = XMQuaternionRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), fRadian);
	//		_matrix RotMatrix = XMMatrixRotationQuaternion(vRot);
	//		_vector vLook = XMVector3TransformNormal(XMVectorSet(0.f, 0.f, 1.f, 0.f), RotMatrix);
	//		_vector vRight = XMVector3Cross(vLook, XMVectorSet(0.f, 1.f, 0.f, 0.f));
	//		_vector vRot2 = XMQuaternionRotationAxis(vRight, XMConvertToRadians(50.f));
	//		_matrix RotMatrix2 = XMMatrixRotationQuaternion(vRot2);
	//		vLook = XMVector3TransformNormal(vLook, RotMatrix2);

	//		Create_Vulcan(Transform.r[3] + vLook * m_fRadius, -vLook);
	//	}

	//	// 3층 벌칸 생성
	//	for (_uint i = 0; i < 3; i++)
	//	{
	//		_float fRadian = XMConvertToRadians(i * 120.f);
	//		_vector vRot = XMQuaternionRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), fRadian);
	//		_matrix RotMatrix = XMMatrixRotationQuaternion(vRot);
	//		_vector vLook = XMVector3TransformNormal(XMVectorSet(0.f, 0.f, 1.f, 0.f), RotMatrix);
	//		_vector vRight = XMVector3Cross(vLook, XMVectorSet(0.f, 1.f, 0.f, 0.f));
	//		_vector vRot2 = XMQuaternionRotationAxis(vRight, XMConvertToRadians(75.f));
	//		_matrix RotMatrix2 = XMMatrixRotationQuaternion(vRot2);
	//		vLook = XMVector3TransformNormal(vLook, RotMatrix2);

	//		Create_Vulcan(Transform.r[3] + vLook * m_fRadius, -vLook);
	//	}
	//}

	if (m_fFlameTickStart.Increase(fTimeDelta))
	{
		Set_SkillName("Aerith_Limit_NaturalDisaster_Flame");
		m_fFlameTick.Update(fTimeDelta);
	}
	
	if (m_fLifeTime.Increase(fTimeDelta))
	{
		Set_Dead();
		m_pDisasterEffect->Set_Dead();
		m_pEffect->Set_Dead();
	}
	
	
	if (m_pDisasterEffect->Get_IsEffectDead())
	{
		Set_Dead();
		m_pEffect->Set_Dead();
	}
}

void CAerith_GunArea::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

}

void CAerith_GunArea::Before_Render(_cref_time fTimeDelta)
{
	__super::Before_Render(fTimeDelta);
}

void CAerith_GunArea::End_Play(_cref_time fTimeDelta)
{
	__super::End_Play(fTimeDelta);
}

HRESULT CAerith_GunArea::Render()
{
	return S_OK;
}

HRESULT CAerith_GunArea::Ready_Components(void* pArg)
{


	return S_OK;
}

void CAerith_GunArea::PhysX_OnCollision_Enter(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Enter(pThisCol, pOtherCol, ContactInfo);

	if (!m_fFlameTickStart.IsMax() && pOtherCol->Get_ColliderDesc().iFilterType == CL_MONSTER_BODY)
	{
		// 대미지 주는 함수, IStatusInterface 인터페이스로 구현됨.
		Status_DamageTo(m_strSkillName, pOtherCol, pOtherCol->Get_Owner(), pThisCol->Get_Owner());
	}
}

void CAerith_GunArea::PhysX_OnCollision_Stay(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Stay(pThisCol, pOtherCol, ContactInfo);

	if (m_fFlameTick.IsTicked() && pOtherCol->Get_ColliderDesc().iFilterType == CL_MONSTER_BODY)
	{
		// 대미지 주는 함수, IStatusInterface 인터페이스로 구현됨.
		Status_DamageTo(m_strSkillName, pOtherCol, pOtherCol->Get_Owner(), pThisCol->Get_Owner());
	}
}

void CAerith_GunArea::PhysX_OnCollision_Exit(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Exit(pThisCol, pOtherCol, ContactInfo);
}


void CAerith_GunArea::Free()
{
	__super::Free();
}

void CAerith_GunArea::Create_Vulcan(_fvector vPos, _fvector vLook)
{
	// 벌컨 생성
	shared_ptr<CAerith_GunArea_Vulcan> pBullet = { nullptr };
	m_pGameInstance->Add_CloneObject(m_pGameInstance->Get_CreateLevelIndex(), L_OBJECT,
		TEXT("Prototype_GameObject_Aerith_GunArea_Vulcan"), nullptr, &pBullet);
	pBullet->Set_Owner(m_pOwner);

	pBullet->Get_TransformCom().lock()->Set_Position(0.f, vPos);
	pBullet->Get_TransformCom().lock()->Set_Look_Manual(vLook);
}
