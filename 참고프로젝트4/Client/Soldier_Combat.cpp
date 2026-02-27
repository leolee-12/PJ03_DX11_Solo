#include "stdafx.h"
#include "Soldier_Combat.h"
#include "GameInstance.h"
#include "Particle.h"
#include "Client_Manager.h"
#include "Effect_Manager.h"
#include "Sound_Manager.h"

IMPLEMENT_CREATE(CSoldier_Combat)
IMPLEMENT_CLONE(CSoldier_Combat, CGameObject)


CSoldier_Combat::CSoldier_Combat(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: BASE(pDevice, pContext)
{
}

CSoldier_Combat::CSoldier_Combat(const CSoldier_Combat& rhs)
	: BASE(rhs)
{
}

HRESULT CSoldier_Combat::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSoldier_Combat::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		RETURN_EFAIL;

	if (FAILED(Ready_Components(pArg)))
		RETURN_EFAIL;

	return S_OK;
}

void CSoldier_Combat::Begin_Play(_cref_time fTimeDelta)
{
	__super::Begin_Play(fTimeDelta);

	PHYSXCOLLIDER_DESC ColliderDesc = {};
	PhysXColliderDesc::Setting_DynamicCollider_WithScale(ColliderDesc, PHYSXCOLLIDER_TYPE::BOX, CL_MONSTER_ATTACK, m_pTransformCom, { 0.5f,1.f,0.5f }, false, nullptr, true);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_PhysX_Collider"),
		TEXT("Com_PhysXColliderCom"), &(m_pPhysXColliderCom), &ColliderDesc)))
		return;

	m_pPhysXColliderCom->PutToSleep();

	Set_SkillName("Soldier_Combat");
}

void CSoldier_Combat::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
}

void CSoldier_Combat::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CSoldier_Combat::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CSoldier_Combat::Before_Render(_cref_time fTimeDelta)
{
	__super::Before_Render(fTimeDelta);

	//if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, shared_from_this())))
	//	return;
}

void CSoldier_Combat::End_Play(_cref_time fTimeDelta)
{
	__super::End_Play(fTimeDelta);
}

HRESULT CSoldier_Combat::Render()
{
	return S_OK;
}

HRESULT CSoldier_Combat::Ready_Components(void* pArg)
{
	return S_OK;
}

HRESULT CSoldier_Combat::Bind_ShaderResources()
{
	// 공통 행렬 바인딩
	if (FAILED(m_pModelCom->ShaderComp()->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::TS_VIEW))))
		RETURN_EFAIL;
	if (FAILED(m_pModelCom->ShaderComp()->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::TS_PROJ))))
		RETURN_EFAIL;

	// 월드 행렬 바인딩
	if (FAILED(m_pModelCom->ShaderComp()->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		RETURN_EFAIL;

	return S_OK;
}

void CSoldier_Combat::OnCollision_Enter(CCollider* pThisCol, CCollider* pOtherCol)
{
}

void CSoldier_Combat::OnCollision_Stay(CCollider* pThisCol, CCollider* pOtherCol)
{
}

void CSoldier_Combat::OnCollision_Exit(CCollider* pThisCol, CCollider* pOtherCol)
{
}

void CSoldier_Combat::PhysX_OnCollision_Enter(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Enter(pThisCol, pOtherCol, ContactInfo);

	_bool bUseDamage = false;
	if ((bUseDamage = (pOtherCol->Get_ColliderDesc().iFilterType == CL_PLAYER_BODY)))
	{
		// 대미지 주는 함수, IStatusInterface 인터페이스로 구현됨.
		if (bUseDamage)
			Status_DamageTo(m_strSkillName, pOtherCol, pOtherCol->Get_Owner(), pThisCol->Get_Owner());

		PxContactPairPoint* pContactPoint = new PxContactPairPoint[ContactInfo.contactCount];
		PxU32 nbContacts = ContactInfo.extractContacts(pContactPoint, ContactInfo.contactCount);

		if (nbContacts >= 1)
		{
			GET_SINGLE(CEffect_Manager)->Create_Hit_Effect<CEffect_Group>(L"PlayerHit(Jiho)", shared_from_this(),
				Convert_Vector(pContactPoint[0].position), Convert_Vector(pContactPoint[0].normal));
		
			// 사운드
			m_pGameInstance->Play_3DSound(TEXT("FF7_SecuritySoldier_Effect"),
				{ "011_SE_SecuritySoldier (B_AtkCombat02_0_01-01).wav" },
				ESoundGroup::Effect, shared_from_this());
		}
	}
}

void CSoldier_Combat::PhysX_OnCollision_Stay(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Stay(pThisCol, pOtherCol, ContactInfo);
}

void CSoldier_Combat::PhysX_OnCollision_Exit(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Exit(pThisCol, pOtherCol, ContactInfo);
}

void CSoldier_Combat::Free()
{
	__super::Free();
}
