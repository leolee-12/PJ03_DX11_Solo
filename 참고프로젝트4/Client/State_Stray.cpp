#include "stdafx.h"
#include "State_Stray.h"


CState_Stray::CState_Stray(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine)
	:CState_Monster(pActor, pStatemachine)
{
}

HRESULT CState_Stray::Initialize_State(CState* pPreviousState)
{
	__super::Initialize_State(pPreviousState);
	return S_OK;
}

void CState_Stray::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
}

void CState_Stray::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CState_Stray::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CState_Stray::Transition_State(CState* pNextState)
{
	__super::Transition_State(pNextState);
}

bool CState_Stray::isValid_NextState(CState* state)
{
	return true;
}
void CState_Stray::Free()
{
}
