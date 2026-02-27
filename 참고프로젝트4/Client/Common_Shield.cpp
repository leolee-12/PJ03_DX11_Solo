#include "stdafx.h"
#include "Common_Shield.h"

#include "Client_Manager.h"
#include "GameInstance.h"
#include "PartObject.h"

#include "Trail_Effect.h"
#include "Effect_Group.h"
#include "Light.h"

IMPLEMENT_CREATE(CCommon_Shield)
IMPLEMENT_CLONE(CCommon_Shield, CGameObject)
CCommon_Shield::CCommon_Shield(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pDeviceContext)
	: BASE(pDevice, pDeviceContext)
{
}

CCommon_Shield::CCommon_Shield(const CCommon_Shield& rhs)
	: BASE(rhs)
{
}

HRESULT CCommon_Shield::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		RETURN_EFAIL;

	return S_OK;
}

HRESULT CCommon_Shield::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		RETURN_EFAIL;

	return S_OK;
}

void CCommon_Shield::Begin_Play(_cref_time fTimeDelta)
{
	__super::Begin_Play(fTimeDelta);

	auto pTarget = GET_SINGLE(CClient_Manager)->Get_TargetMonster(AERITH);
	m_vTargetPos = pTarget.vTargetPos;

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
		CL_PLAYER_ATTACK, m_pTransformCom, { 0.1f, 0.1f, 0.1f }, false, nullptr, true);
	m_vPhysXColliderLocalOffset = { 0.f,0.f,0.f };

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_PhysX_Collider"),
		TEXT("Com_PhysXColliderCom"), &(m_pPhysXColliderCom), &ColliderDesc)))
		return;

	// [여기에] 트레일 이펙트 생성 넣어주세요.
	_float3 vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	m_pEffect = GET_SINGLE(CEffect_Manager)->Create_Effect<CTrail_Effect>(TEXT("Common_Shield"), shared_from_this());
	//m_pDisasterEffect = GET_SINGLE(CEffect_Manager)->Create_Effect<CEffect_Group>(TEXT("GRP_AerithDisaster"), shared_from_this());

	LIGHT_DESC LightDesc = {};
	LightDesc.bUseVolumetric = true;
	LightDesc.eType = LIGHT_DESC::TYPE_POINT;
	LightDesc.fRange = 3.f;
	LightDesc.strLightName = "ShieldLight" + to_string(m_pGameInstance->RandomInt(0, 10000000));
	LightDesc.vDiffuse = { 0.4f, 0.4f, 0.4f, 1.f };
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

	m_fLifeTime = 1.f;
}

void CCommon_Shield::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);

	if (m_pEffect)
		m_pEffect->Get_TransformCom().lock()->Set_Look_Manual(m_pTransformCom->Get_State(CTransform::STATE_LOOK));
}

void CCommon_Shield::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (!m_pOwner.expired())
	{
		auto pTransform = m_pOwner.lock()->Get_TransformCom().lock();
		_float3 vPos = pTransform->Get_State(CTransform::STATE_POSITION) + m_vOffset;
		
		m_pTransformCom->Set_Position(0.f, vPos);
	}
	
	if (m_fLifeTime.Increase(fTimeDelta))
	{
		Set_Dead();

		if (m_pEffect)
			m_pEffect->Set_Dead();
	}
}

void CCommon_Shield::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

}

void CCommon_Shield::Before_Render(_cref_time fTimeDelta)
{
	__super::Before_Render(fTimeDelta);
}

void CCommon_Shield::End_Play(_cref_time fTimeDelta)
{
	__super::End_Play(fTimeDelta);
}

HRESULT CCommon_Shield::Render()
{
	return S_OK;
}

HRESULT CCommon_Shield::Ready_Components(void* pArg)
{


	return S_OK;
}

void CCommon_Shield::PhysX_OnCollision_Enter(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Enter(pThisCol, pOtherCol, ContactInfo);

}

void CCommon_Shield::PhysX_OnCollision_Stay(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Stay(pThisCol, pOtherCol, ContactInfo);

}

void CCommon_Shield::PhysX_OnCollision_Exit(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Exit(pThisCol, pOtherCol, ContactInfo);
}


void CCommon_Shield::Free()
{
	__super::Free();
}
