#include "stdafx.h"
#include "State_list_Cloud.h"

#include "WeaponPart.h"

CState_Cloud_AbilityAttack_Blade::CState_Cloud_AbilityAttack_Blade(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine)
	:CState_Cloud(pActor, pStatemachine)
{
}

HRESULT CState_Cloud_AbilityAttack_Blade::Initialize_State(CState* pPreviousState)
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
	
	m_pActor_ModelCom.lock()->Set_Animation("Battle00|B_AtkBurst05_0", 1.3f, false, true, 2.f);

	shared_ptr< CAttackCollider> pCollider;
	m_pGameInstance->Add_CloneObject<CAttackCollider>(m_pGameInstance->Get_CreateLevelIndex(), L_EFFECT, L"Prototype_GameObject_AttackCollider", nullptr, &pCollider);
	m_pAttackCollider = pCollider;
	m_pAttackCollider.lock()->Set_Owner(m_pActor);
	m_pAttackCollider.lock()->Set_StatusComByOwner("Cloud_Ability_Blade");
	m_pAttackCollider.lock()->Get_TransformCom().lock()->Set_Scaling(2.f, 2.f, 10.f);

	TurnOn_WeaponEffect();

	return S_OK;
}

void CState_Cloud_AbilityAttack_Blade::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
	
	TurnOnOff_Weapon(64.f, 69.f,true);
	if(m_pActor_ModelCom.lock()->IsAnimation_Frame_Once(64.f))
	{
		/*auto pWeapon = DynPtrCast<CWeaponPart>(m_pActor.lock()->Find_PartObject(TEXT("Part_Weapon")));
		if (pWeapon) { pWeapon->Set_SkillName("Cloud_Ability_Blade"); }*/
		
		
		_float3 vOffset = m_pAttackCollider.lock()->Get_PhysXColliderLocalOffset();
		m_pAttackCollider.lock()->Set_PhysXColliderLocalOffset({ 0.f, vOffset.y, 8.f });

		m_pAttackCollider.lock()->Get_PhysXColliderCom().lock()->WakeUp();
	}
	else if (m_pActor_ModelCom.lock()->IsAnimation_Range(68.f, 71.f))
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

void CState_Cloud_AbilityAttack_Blade::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CState_Cloud_AbilityAttack_Blade::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CState_Cloud_AbilityAttack_Blade::Transition_State(CState* pNextState)
{
	__super::Transition_State(pNextState);

	if (!m_pAttackCollider.expired())
	{
		m_pAttackCollider.lock()->Set_Dead();
		m_pAttackCollider.lock() = nullptr;
	}

	TurnOff_WeaponEffect();
}

bool CState_Cloud_AbilityAttack_Blade::isValid_NextState(CState* state)
{
	return true;
}

void CState_Cloud_AbilityAttack_Blade::Free()
{
}
