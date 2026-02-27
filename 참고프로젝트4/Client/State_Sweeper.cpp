#include "stdafx.h"
#include "State_Sweeper.h"


CState_Sweeper::CState_Sweeper(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine)
	:CState_Monster(pActor, pStatemachine)
{
}

HRESULT CState_Sweeper::Initialize_State(CState* pPreviousState)
{
	__super::Initialize_State(pPreviousState);
	return S_OK;
}

void CState_Sweeper::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
}

void CState_Sweeper::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CState_Sweeper::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CState_Sweeper::Transition_State(CState* pNextState)
{
	__super::Transition_State(pNextState);
}

bool CState_Sweeper::isValid_NextState(CState* state)
{
	return true;
}
void CState_Sweeper::Free()
{
}
