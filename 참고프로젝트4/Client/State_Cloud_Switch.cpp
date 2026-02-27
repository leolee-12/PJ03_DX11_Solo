#include "stdafx.h"
#include "State_list_Cloud.h"

CState_Cloud_Switch::CState_Cloud_Switch(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine)
	:CState_Cloud(pActor, pStatemachine)
{
}

HRESULT CState_Cloud_Switch::Initialize_State(CState* pPreviousState)
{
	__super::Initialize_State(pPreviousState);

	m_pActor_ModelCom.lock()->Set_Animation("Common|CMN_PC0000_00_Switch01_0", 1.3f, false, true, 2.f);

	return S_OK;
}

void CState_Cloud_Switch::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
		
	if (m_pActor_ModelCom.lock()->IsAnimation_Finished())
		m_pStateMachineCom.lock()->Enter_State<CState_Cloud_Idle>();
}

void CState_Cloud_Switch::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CState_Cloud_Switch::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CState_Cloud_Switch::Transition_State(CState* pNextState)
{
	__super::Transition_State(pNextState);
}

bool CState_Cloud_Switch::isValid_NextState(CState* state)
{
	return true;
}

void CState_Cloud_Switch::Free()
{
}
