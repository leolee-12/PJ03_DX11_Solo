#include "stdafx.h"
#include "State_list_Cloud.h"

CState_Cloud_AbilityAttack_Slash::CState_Cloud_AbilityAttack_Slash(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine)
	:CState_Cloud(pActor, pStatemachine)
{
}

HRESULT CState_Cloud_AbilityAttack_Slash::Initialize_State(CState* pPreviousState)
{
	__super::Initialize_State(pPreviousState);
	
	Check_UseCommand();
	
	//임시 가까운 몬스터로 타겟팅 (선택으로 변경하기)
	if (m_bPlayable || !m_TargetInfo.pTarget.lock())
		m_TargetInfo = GET_SINGLE(CClient_Manager)->Get_TargetMonster(CLOUD);

	if (m_TargetInfo.pTarget.lock() != nullptr)
	{
		m_pActor_TransformCom.lock()->Look_At_OnLand(m_TargetInfo.vTargetPos);
		GET_SINGLE(CCamera_Manager)->Get_ThirdPersonCamera(CLOUD).lock()->Turn_To_Look(m_pActor_TransformCom.lock()->Get_State(CTransform::STATE_LOOK));
	}


	m_pActor_ModelCom.lock()->Set_Animation("Battle00|B_AtkBurst02_0", 1.15f, false, true, 2.f);

	shared_ptr< CAttackCollider> pCollider;
	m_pGameInstance->Add_CloneObject<CAttackCollider>(m_pGameInstance->Get_CreateLevelIndex(), L_EFFECT, L"Prototype_GameObject_AttackCollider", nullptr, &pCollider);
	m_pAttackCollider = pCollider;
	m_pAttackCollider.lock()->Set_Owner(m_pActor);
	m_pAttackCollider.lock()->Set_StatusComByOwner("Cloud_Ability_Slash");
	m_pAttackCollider.lock()->Get_TransformCom().lock()->Set_Scaling(2.f, 2.f, 6.f);

	TurnOn_WeaponEffect();
	return S_OK;
}

void CState_Cloud_AbilityAttack_Slash::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
	
	TurnOnOff_Weapon(35.f, 41.f,true);
	if (m_pActor_ModelCom.lock()->IsAnimation_Range(35.f, 38.f))
	{
		_float3 vOffset = m_pAttackCollider.lock()->Get_PhysXColliderLocalOffset();
		m_pAttackCollider.lock()->Set_PhysXColliderLocalOffset({ 0.f, vOffset.y, 2.f });

		m_pAttackCollider.lock()->Get_PhysXColliderCom().lock()->WakeUp();
	}
	else if (m_pActor_ModelCom.lock()->IsAnimation_Range(40.f, 43.f))
	{
		if (!m_pAttackCollider.expired())
		{
			m_pAttackCollider.lock()->Set_Dead();
			m_pAttackCollider.lock() = nullptr;
		}
	}
	if (m_pAttackCollider.lock())
	{
		m_pAttackCollider.lock()->Get_TransformCom().lock()->Set_State(CTransform::STATE_POSITION, m_pActor_TransformCom.lock()->Get_State(CTransform::STATE_POSITION));
		m_pAttackCollider.lock()->Get_TransformCom().lock()->Set_Look(m_pActor_TransformCom.lock()->Get_State(CTransform::STATE_LOOK));
	}

	if (m_pActor_ModelCom.lock()->IsAnimation_Finished())
		m_pStateMachineCom.lock()->Enter_State<CState_Cloud_Idle>();
}

void CState_Cloud_AbilityAttack_Slash::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CState_Cloud_AbilityAttack_Slash::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CState_Cloud_AbilityAttack_Slash::Transition_State(CState* pNextState)
{
	__super::Transition_State(pNextState);
	if (!m_pAttackCollider.expired())
	{
		m_pAttackCollider.lock()->Set_Dead();
		m_pAttackCollider.lock() = nullptr;
	}

	TurnOff_WeaponEffect();
}

bool CState_Cloud_AbilityAttack_Slash::isValid_NextState(CState* state)
{
	return true;
}

void CState_Cloud_AbilityAttack_Slash::Free()
{
}
