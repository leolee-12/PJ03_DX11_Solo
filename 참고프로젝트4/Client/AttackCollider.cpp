#include "stdafx.h"
#include "AttackCollider.h"

#include "Client_Manager.h"
#include "PhysX_Collider.h"
#include "GameInstance.h"
#include "StatusComp.h"
#include "Player.h"

IMPLEMENT_CREATE(CAttackCollider)
IMPLEMENT_CLONE(CAttackCollider, CGameObject)

CAttackCollider::CAttackCollider(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: CDynamicObject(pDevice, pContext)
{
}

CAttackCollider::CAttackCollider(const CAttackCollider& rhs)
	: CDynamicObject(rhs)
{
}

HRESULT CAttackCollider::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		RETURN_EFAIL;
	return S_OK;
}

HRESULT CAttackCollider::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		RETURN_EFAIL;
	if (FAILED(Ready_Components(pArg)))
		RETURN_EFAIL;

	return S_OK;
}

void CAttackCollider::Begin_Play(_cref_time fTimeDelta)
{
	__super::Begin_Play(fTimeDelta);

	_float3 Scale = Get_ScaleFromMatrix(m_pTransformCom->Get_WorldFloat4x4());
	PHYSXCOLLIDER_DESC ColliderDesc = {};
	PhysXColliderDesc::Setting_DynamicCollider(ColliderDesc, PHYSXCOLLIDER_TYPE::BOX, CL_PLAYER_ATTACK, m_pTransformCom, false, nullptr, true);
	m_vPhysXColliderLocalOffset = { 0.f,Scale.y * 0.5f,0.f };

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_PhysX_Collider"),
		TEXT("Com_PhysXColliderCom"), &(m_pPhysXColliderCom), &ColliderDesc)))
		return;

	m_pPhysXColliderCom->PutToSleep();
}

void CAttackCollider::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
}

void CAttackCollider::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CAttackCollider::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CAttackCollider::Before_Render(_cref_time fTimeDelta)
{
	__super::Before_Render(fTimeDelta);
}

void CAttackCollider::End_Play(_cref_time fTimeDelta)
{
	__super::End_Play(fTimeDelta);
}

HRESULT CAttackCollider::Render()
{
	return S_OK;
}

HRESULT CAttackCollider::Ready_Components(void* pArg)
{
	return S_OK;
}

void CAttackCollider::OnCollision_Enter(CCollider* pThisCol, CCollider* pOtherCol)
{
}

void CAttackCollider::OnCollision_Stay(CCollider* pThisCol, CCollider* pOtherCol)
{
}

void CAttackCollider::OnCollision_Exit(CCollider* pThisCol, CCollider* pOtherCol)
{
}

void CAttackCollider::PhysX_OnCollision_Enter(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	// 몬스터와 상호작용
	if (pOtherCol->Get_ColliderDesc().iFilterType == CL_MONSTER_BODY)
	{
		// HP가 0일 때는 대미지와 이펙트를 터뜨리지 않음.
		if (DynPtrCast<IStatusInterface>(pOtherCol->Get_Owner().lock())->Get_StatusCom().lock()->IsZeroHP())
			return;

		// 대미지 주는 함수, IStatusInterface 인터페이스로 구현됨.
		Status_DamageTo(m_strSkillName, pOtherCol, pOtherCol->Get_Owner(), pThisCol->Get_Owner());

		DynPtrCast<IStatusInterface>(Get_Owner().lock())->Get_StatusCom().lock()->Add_LimitBreak(1.f);

		if (GET_SINGLE(CClient_Manager)->IsPlayable(static_pointer_cast<CPlayer>(m_pOwner.lock())->Get_PlayerType()))
		{
			GET_SINGLE(CClient_Manager)->Register_Event_AdjustTimeDelta(m_pOwner, 0.3f, 0.1f);
			GET_SINGLE(CClient_Manager)->Register_Event_MotionBlur(m_pOwner, 0.4f);
		}
	}
}

void CAttackCollider::PhysX_OnCollision_Stay(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
}

void CAttackCollider::PhysX_OnCollision_Exit(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
}

HRESULT CAttackCollider::Set_StatusComByGameObject(shared_ptr<CGameObject> pGameObject)
{
	CGameObject* pObj = pGameObject.get();

	CCharacter* pChr = DynCast<CCharacter*>(pObj);
	if (nullptr != pChr)
	{
		m_pStatusCom = pChr->Get_StatusCom();
		if (m_pStatusCom.expired())
			assert(true);
	}
	else
		assert(true);

	return S_OK;
}

HRESULT CAttackCollider::Set_StatusComByOwner(const string& strSkillName)
{
	if (m_pOwner.expired())
		return E_FAIL;

	CGameObject* pObj = m_pOwner.lock().get();

	CCharacter* pChr = DynCast<CCharacter*>(pObj);
	if (nullptr != pChr)
	{
		m_pStatusCom = pChr->Get_StatusCom();
		if (m_pStatusCom.expired())
			assert(true);
	}
	else
		assert(true);

	m_strSkillName = strSkillName;

	return S_OK;
}

weak_ptr<class CStatusComp> CAttackCollider::Get_StatusCom()
{
	if (!m_pShaderCom)
		Set_StatusComByOwner(m_strSkillName);

	return m_pStatusCom;
}

void CAttackCollider::Free()
{
	__super::Free();
}
