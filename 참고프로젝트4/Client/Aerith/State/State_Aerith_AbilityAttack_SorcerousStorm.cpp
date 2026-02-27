#include "stdafx.h"
#include "Aerith/State/State_List_Aerith.h"

#include "Aerith/Weapon/Aerith_List_Weapon.h"

CState_Aerith_AbilityAttack_SorcerousStorm::CState_Aerith_AbilityAttack_SorcerousStorm(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine)
	: BASE(pActor, pStatemachine)
{
}

HRESULT CState_Aerith_AbilityAttack_SorcerousStorm::Initialize_State(CState* pPreviousState)
{
	__super::Initialize_State(pPreviousState);

	Check_UseCommand();

	//임시 가까운 몬스터로 타겟팅 (선택으로 변경하기)
	m_TargetInfo = GET_SINGLE(CClient_Manager)->Get_TargetMonster(AERITH);

	if (m_TargetInfo.pTarget.lock() != nullptr)
	{
		m_pActor_TransformCom.lock()->Look_At_OnLand(m_TargetInfo.vTargetPos);
		GET_SINGLE(CCamera_Manager)->Get_ThirdPersonCamera(AERITH).lock()->Turn_To_Look(m_pActor_TransformCom.lock()->Get_State(CTransform::STATE_LOOK));
	}
	
	m_pActor_ModelCom.lock()->Set_Animation("Battle00|B_AtkBurst03_0", 1.3f, false, true, 2.f);

	/*shared_ptr< CAttackCollider> pCollider;
	m_pGameInstance->Add_CloneObject<CAttackCollider>(m_pGameInstance->Get_CreateLevelIndex(), L_EFFECT, 
		L"Prototype_GameObject_AttackCollider", nullptr, &pCollider);
	m_pAttackCollider = pCollider;
	m_pAttackCollider.lock()->Get_TransformCom().lock()->Set_Scaling(2.f, 2.f, 10.f);*/


	return S_OK;
}

void CState_Aerith_AbilityAttack_SorcerousStorm::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);

	// 특정 프레임에 폭풍객체 소환

	if (m_pActor_ModelCom.lock()->IsAnimation_Frame_Once(40.f))
	{
		Create_Storm();
	}
	
	/*if(m_pActor_ModelCom.lock()->IsAnimation_Range(64.f,67.f))
	{
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
	}*/

	if (m_pActor_ModelCom.lock()->IsAnimation_Finished())
		m_pStateMachineCom.lock()->Enter_State<CState_Aerith_Idle>();
}

void CState_Aerith_AbilityAttack_SorcerousStorm::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CState_Aerith_AbilityAttack_SorcerousStorm::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CState_Aerith_AbilityAttack_SorcerousStorm::Transition_State(CState* pNextState)
{
	__super::Transition_State(pNextState);
	/*if (!m_pAttackCollider.expired())
	{
		m_pAttackCollider.lock()->Set_Dead();
		m_pAttackCollider.lock() = nullptr;
	}*/
}

bool CState_Aerith_AbilityAttack_SorcerousStorm::isValid_NextState(CState* state)
{
	return true;
}

void CState_Aerith_AbilityAttack_SorcerousStorm::Free()
{
}

void CState_Aerith_AbilityAttack_SorcerousStorm::Create_Storm()
{
	shared_ptr<CAerith_SorcerousStorm> pBullet = { nullptr };
	m_pGameInstance->Add_CloneObject(m_pGameInstance->Get_CreateLevelIndex(), L_OBJECT,
		TEXT("Prototype_GameObject_Aerith_SorcerousStorm"), nullptr, &pBullet);
	pBullet->Set_Owner(m_pActor);

	_matrix ActorMatrix = m_pActor.lock()->Get_TransformCom().lock()->Get_WorldFloat4x4();
	pBullet->Get_TransformCom().lock()->Set_Position(0.f, ActorMatrix.r[3]);
	pBullet->Get_TransformCom().lock()->Set_Look_Manual(ActorMatrix.r[2]);

	m_TargetInfo = GET_SINGLE(CClient_Manager)->Get_TargetMonster(AERITH);
	if (m_TargetInfo.pTarget.lock() != nullptr)
	{
		pBullet->Get_TransformCom().lock()->Set_Position(0.f, m_TargetInfo.vTargetPos);
	}
}
