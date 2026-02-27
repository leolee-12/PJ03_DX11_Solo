#include "stdafx.h"
#include "State_List_Soldier_Gun.h"


CState_Soldier_Gun_Hit::CState_Soldier_Gun_Hit(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine)
	:CState_Soldier_Gun(pActor, pStatemachine)
{
}

HRESULT CState_Soldier_Gun_Hit::Initialize_State(CState* pPreviousState)
{
	__super::Initialize_State(pPreviousState);
	m_pActor_ModelCom.lock()->Set_Animation("Main|B_DmgF01", 1.f, false);
	m_pActor_TransformCom.lock()->Set_Move_AnimationPosition(true);
	return S_OK;
}

void CState_Soldier_Gun_Hit::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
	if (m_pActor_ModelCom.lock()->IsAnimation_Finished())
		m_pStateMachineCom.lock()->Enter_State<CState_Soldier_Gun_Run>();
}

void CState_Soldier_Gun_Hit::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CState_Soldier_Gun_Hit::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CState_Soldier_Gun_Hit::Transition_State(CState* pNextState)
{
	__super::Transition_State(pNextState);
}

bool CState_Soldier_Gun_Hit::isValid_NextState(CState* state)
{
	return true;
}

void CState_Soldier_Gun_Hit::Free()
{
}
