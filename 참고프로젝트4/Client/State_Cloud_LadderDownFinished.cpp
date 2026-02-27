#include "stdafx.h"
#include "State_list_Cloud.h"

CState_Cloud_LadderDownFinished::CState_Cloud_LadderDownFinished(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine)
	:CState_Cloud(pActor, pStatemachine)
{
}

HRESULT CState_Cloud_LadderDownFinished::Initialize_State(CState* pPreviousState)
{
	__super::Initialize_State(pPreviousState);
		
	m_pActor_ModelCom.lock()->Set_Animation("Ladder|N_LadderD02_3", 1.3f, false, true,2.f);	
	m_pActor.lock()->Get_PhysXControllerCom().lock()->Enable_Gravity(true);
	return S_OK;
}

void CState_Cloud_LadderDownFinished::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
	
	if (m_pActor_ModelCom.lock()->IsAnimation_Finished())
		m_pStateMachineCom.lock()->Enter_State<CState_Cloud_Idle>();
}

void CState_Cloud_LadderDownFinished::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CState_Cloud_LadderDownFinished::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CState_Cloud_LadderDownFinished::Transition_State(CState* pNextState)
{
	__super::Transition_State(pNextState);

	m_pActor.lock()->Get_PhysXControllerCom().lock()->Set_EnableSimulation(true);
}

bool CState_Cloud_LadderDownFinished::isValid_NextState(CState* state)
{
	return true;
}

void CState_Cloud_LadderDownFinished::Free()
{
}
