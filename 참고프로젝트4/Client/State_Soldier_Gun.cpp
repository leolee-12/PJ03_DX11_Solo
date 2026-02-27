#include "stdafx.h"
#include "State_Soldier_Gun.h"


CState_Soldier_Gun::CState_Soldier_Gun(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine)
	:CState_Monster(pActor, pStatemachine)
{
}

HRESULT CState_Soldier_Gun::Initialize_State(CState* pPreviousState)
{
	__super::Initialize_State(pPreviousState);
	return S_OK;
}

void CState_Soldier_Gun::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
}

void CState_Soldier_Gun::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CState_Soldier_Gun::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CState_Soldier_Gun::Transition_State(CState* pNextState)
{
	__super::Transition_State(pNextState);
}

bool CState_Soldier_Gun::isValid_NextState(CState* state)
{
	return true;
}
void CState_Soldier_Gun::Free()
{
}
