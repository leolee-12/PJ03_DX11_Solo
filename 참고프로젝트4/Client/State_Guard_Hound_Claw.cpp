#include "stdafx.h"
#include "State_List_Guard_Hound.h"
#include "Character.h"

CState_Guard_Hound_Claw::CState_Guard_Hound_Claw(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine)
	:CState_Guard_Hound(pActor, pStatemachine)
{
}

HRESULT CState_Guard_Hound_Claw::Initialize_State(CState* pPreviousState)
{
	__super::Initialize_State(pPreviousState);

	m_pActor_ModelCom.lock()->Set_Animation("Main|B_Run01_1", 1.f, true);
	m_pActor_TransformCom.lock()->Set_Move_AnimationPosition(false);

	return S_OK;
}

void CState_Guard_Hound_Claw::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);

	//if (m_pActor_ModelCom.lock()->Get_CurrentAnimationName() == "Main|B_AtkBite01"&&m_pActor_ModelCom.lock()->IsAnimation_Range(7,25))
	//{
	//	static_pointer_cast<CCharacter>(m_pActor.lock())->TurnOn_Weapon(L"Part_Bite");
	//}
	//else
	//{
	//	static_pointer_cast<CCharacter>(m_pActor.lock())->TurnOff_Weapon(L"Part_Bite");
	//}
	m_pActor_TransformCom.lock()->Look_At_OnLand(m_vTargetPos);

	if (InRange(m_fDistance, 0.f, 2.5f, "[]") && m_pActor_ModelCom.lock()->Get_CurrentAnimationName() == "Main|B_Run01_1")
	{
		m_pActor_TransformCom.lock()->Set_Move_AnimationPosition(true);
		m_pActor_ModelCom.lock()->Set_Animation("Main|B_AtkClaw01", 1.f, false);
	}
	else if (m_pActor_ModelCom.lock()->Get_CurrentAnimationName() == "Main|B_Run01_1" && m_pActor_ModelCom.lock()->IsAnimation_Finished())
	{
		m_pActor_ModelCom.lock()->Set_Animation("Main|B_Run01_1", 1.f, true);
	}

	if (m_pActor_ModelCom.lock()->Get_CurrentAnimationName() == "Main|B_Run01_1")
		m_pActor_TransformCom.lock()->Go_Straight(fTimeDelta * 1.2f);

	if (m_pActor_ModelCom.lock()->IsAnimation_Finished("Main|B_AtkClaw01"))
	{

		m_pActor_ModelCom.lock()->Set_Animation("Main|B_StepB01", 1.f, false);
	}

	if(m_pActor_ModelCom.lock()->IsAnimation_Finished("Main|B_StepB01"))
		m_pStateMachineCom.lock()->Enter_State<CState_Guard_Hound_Run>();
}

void CState_Guard_Hound_Claw::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (m_pActor_ModelCom.lock()->Get_CurrentAnimationName() == "Main|B_AtkClaw01" && m_pActor_ModelCom.lock()->IsAnimation_Range(15, 20))
	{
		static_pointer_cast<CCharacter>(m_pActor.lock())->TurnOn_Weapon(L"Part_Claw");
	}
	else
	{
		static_pointer_cast<CCharacter>(m_pActor.lock())->TurnOff_Weapon(L"Part_Claw");
	}
}

void CState_Guard_Hound_Claw::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CState_Guard_Hound_Claw::Transition_State(CState* pNextState)
{
	__super::Transition_State(pNextState);
}

bool CState_Guard_Hound_Claw::isValid_NextState(CState* state)
{
	return true;
}

void CState_Guard_Hound_Claw::Free()
{
}
