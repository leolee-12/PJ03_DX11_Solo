#include "stdafx.h"
#include "State_List_Soldier_Gun.h"


CState_Soldier_Gun_Walk::CState_Soldier_Gun_Walk(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine)
	:CState_Soldier_Gun(pActor, pStatemachine)
{
}

HRESULT CState_Soldier_Gun_Walk::Initialize_State(CState* pPreviousState)
{
	__super::Initialize_State(pPreviousState);
	return S_OK;
}

void CState_Soldier_Gun_Walk::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
}

void CState_Soldier_Gun_Walk::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CState_Soldier_Gun_Walk::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CState_Soldier_Gun_Walk::Transition_State(CState* pNextState)
{
	__super::Transition_State(pNextState);
}

bool CState_Soldier_Gun_Walk::isValid_NextState(CState* state)
{
	return true;
}

void CState_Soldier_Gun_Walk::Free()
{
}
