#include "stdafx.h"
#include "Aerith/State/State_List_Aerith.h"

CState_Aerith_Event_PassOver::CState_Aerith_Event_PassOver(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine)
	: BASE(pActor, pStatemachine)
{
}

HRESULT CState_Aerith_Event_PassOver::Initialize_State(CState* pPreviousState)
{
	__super::Initialize_State(pPreviousState);

	m_pActor_ModelCom.lock()->Set_Animation("Battle00(FreeRun)|B_Passover_short", 1.3f, false, true, 2.f);

	return S_OK;
}

void CState_Aerith_Event_PassOver::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
	
	if (m_pActor_ModelCom.lock()->IsAnimation_Finished())
		m_pStateMachineCom.lock()->Enter_State<CState_Aerith_Idle>();
}

void CState_Aerith_Event_PassOver::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CState_Aerith_Event_PassOver::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CState_Aerith_Event_PassOver::Transition_State(CState* pNextState)
{
	__super::Transition_State(pNextState);
}

bool CState_Aerith_Event_PassOver::isValid_NextState(CState* state)
{
	return true;
}

void CState_Aerith_Event_PassOver::Free()
{
}
