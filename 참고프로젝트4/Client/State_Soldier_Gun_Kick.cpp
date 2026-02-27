#include "stdafx.h"
#include "State_List_Soldier_Gun.h"
#include "GameInstance.h"
#include "Character.h"


CState_Soldier_Gun_Kick::CState_Soldier_Gun_Kick(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine)
	:CState_Soldier_Gun(pActor, pStatemachine)
{
}

HRESULT CState_Soldier_Gun_Kick::Initialize_State(CState* pPreviousState)
{
	__super::Initialize_State(pPreviousState);

	m_fTimeAcc = 0.f;
	int iRandom = rand() % 2;

	if (iRandom == 0)
		m_pActor_ModelCom.lock()->Set_Animation("Main|B_WalkL01_1", 1.f, false, false);
	else
		m_pActor_ModelCom.lock()->Set_Animation("Main|B_WalkR01_1", 1.f, false, false);

	m_pActor_TransformCom.lock()->Set_Move_AnimationPosition(false);

	return S_OK;
}

void CState_Soldier_Gun_Kick::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);

	if (InRange(m_fDistance, 6.f, 15.f, "[]") && m_pActor_ModelCom.lock()->Get_CurrentAnimationName() != "Main|B_StepB01")
		m_pStateMachineCom.lock()->Set_State<CState_Soldier_Gun_Shoot>();

	m_fTimeAcc += fTimeDelta;

	m_pActor_TransformCom.lock()->Look_At_OnLand(m_vTargetPos);

	if (m_pActor_ModelCom.lock()->Get_CurrentAnimationName() == "Main|B_WalkL01_1")
	{
		m_pActor_TransformCom.lock()->Go_Left(fTimeDelta * 0.3f);
	}
	else if (m_pActor_ModelCom.lock()->Get_CurrentAnimationName() == "Main|B_WalkR01_1")
	{
		m_pActor_TransformCom.lock()->Go_Right(fTimeDelta * 0.3f);
	}

	if (m_pActor_ModelCom.lock()->Get_CurrentAnimationName() == "Main|N_Run01_1")
	{
		m_pActor_TransformCom.lock()->Go_Straight(fTimeDelta);

		if (InRange(m_fDistance, 0.f, 1.5f, "[]"))
			m_pActor_ModelCom.lock()->Set_Animation("Main|B_AtkKick01", 1.f, false, false);
		else if (m_pActor_ModelCom.lock()->IsAnimation_Finished())
			m_pActor_ModelCom.lock()->Set_Animation("Main|N_Run01_1", 1.f, false, false);
	}

	if (m_pActor_ModelCom.lock()->Get_CurrentAnimationName() == "Main|B_WalkR01_1" || m_pActor_ModelCom.lock()->Get_CurrentAnimationName() == "Main|B_WalkL01_1")
	{
		if (0.5f < m_fTimeAcc && InRange(m_fDistance, 0.f, 1.5f, "[)"))
			m_pActor_ModelCom.lock()->Set_Animation("Main|B_AtkKick01", 1.f, false, false);
		else if (0.5f < m_fTimeAcc && InRange(m_fDistance, 1.5f, 6.f, "[)"))
			m_pActor_ModelCom.lock()->Set_Animation("Main|N_Run01_1", 1.f, false, false);


	}

	if (m_pActor_ModelCom.lock()->IsAnimation_Finished("Main|B_AtkKick01"))
	{
		m_pActor_ModelCom.lock()->Set_Animation("Main|B_StepB01", 1.f, false, false);
		m_pActor_TransformCom.lock()->Set_Move_AnimationPosition(true);
	}

}

void CState_Soldier_Gun_Kick::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (m_pActor_ModelCom.lock()->IsAnimation_Finished("Main|B_StepB01"))
	{
		m_pStateMachineCom.lock()->Enter_State<CState_Soldier_Gun_Run>();
	}

	if (m_pActor_ModelCom.lock()->Get_CurrentAnimationName() == "Main|B_AtkKick01" && m_pActor_ModelCom.lock()->IsAnimation_Range(24, 30))
	{
		static_pointer_cast<CCharacter>(m_pActor.lock())->TurnOn_Weapon(L"Part_Kick");
	}
	else
	{
		static_pointer_cast<CCharacter>(m_pActor.lock())->TurnOff_Weapon(L"Part_Kick");
	}
}

void CState_Soldier_Gun_Kick::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CState_Soldier_Gun_Kick::Transition_State(CState* pNextState)
{
	__super::Transition_State(pNextState);


}

bool CState_Soldier_Gun_Kick::isValid_NextState(CState* state)
{
	if (m_pActor_ModelCom.lock()->IsAnimation_Finished())
		return true;
	else
		return false;

	return true;
}

void CState_Soldier_Gun_Kick::Free()
{
}
