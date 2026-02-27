#include "stdafx.h"
#include "State_MonoDrive.h"


CState_MonoDrive::CState_MonoDrive(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine)
	:CState_Monster(pActor, pStatemachine)
{
}

HRESULT CState_MonoDrive::Initialize_State(CState* pPreviousState)
{
	__super::Initialize_State(pPreviousState);
	return S_OK;
}

void CState_MonoDrive::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
}

void CState_MonoDrive::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CState_MonoDrive::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CState_MonoDrive::Transition_State(CState* pNextState)
{
	__super::Transition_State(pNextState);
}

bool CState_MonoDrive::isValid_NextState(CState* state)
{
	return true;
}
void CState_MonoDrive::Free()
{
}
