#include "stdafx.h"
#include "State_Guard_Hound.h"


CState_Guard_Hound::CState_Guard_Hound(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine)
	:CState_Monster(pActor, pStatemachine)
{
}

HRESULT CState_Guard_Hound::Initialize_State(CState* pPreviousState)
{
	__super::Initialize_State(pPreviousState);
	return S_OK;
}

void CState_Guard_Hound::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
}

void CState_Guard_Hound::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CState_Guard_Hound::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CState_Guard_Hound::Transition_State(CState* pNextState)
{
	__super::Transition_State(pNextState);
}

bool CState_Guard_Hound::isValid_NextState(CState* state)
{
	return true;
}
void CState_Guard_Hound::Free()
{
}
