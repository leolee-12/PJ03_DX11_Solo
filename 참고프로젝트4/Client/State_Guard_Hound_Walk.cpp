#include "stdafx.h"
#include "State_Guard_Hound_Walk.h"


CState_Guard_Hound_Walk::CState_Guard_Hound_Walk(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine)
	:CState_Monster(pActor, pStatemachine)
{
}

HRESULT CState_Guard_Hound_Walk::Initialize_State(CState* pPreviousState)
{
	__super::Initialize_State(pPreviousState);
	return S_OK;
}

void CState_Guard_Hound_Walk::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
}

void CState_Guard_Hound_Walk::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CState_Guard_Hound_Walk::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CState_Guard_Hound_Walk::Transition_State(CState* pNextState)
{
	__super::Transition_State(pNextState);
}

bool CState_Guard_Hound_Walk::isValid_NextState(CState* state)
{
	return true;
}

void CState_Guard_Hound_Walk::Free()
{
}
