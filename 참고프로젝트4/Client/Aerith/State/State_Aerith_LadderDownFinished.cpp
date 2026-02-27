#include "stdafx.h"
#include "Aerith/State/State_List_Aerith.h"

CState_Aerith_LadderDownFinished::CState_Aerith_LadderDownFinished(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine)
	: BASE(pActor, pStatemachine)
{
}

HRESULT CState_Aerith_LadderDownFinished::Initialize_State(CState* pPreviousState)
{
	__super::Initialize_State(pPreviousState);
		
	m_pActor_ModelCom.lock()->Set_Animation("Ladder|N_LadderD02_3", 1.3f, false, true,2.f);	
	m_pActor.lock()->Get_PhysXControllerCom().lock()->Enable_Gravity(true);
	return S_OK;
}

void CState_Aerith_LadderDownFinished::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
	
	if (m_pActor_ModelCom.lock()->IsAnimation_Finished())
		m_pStateMachineCom.lock()->Enter_State<CState_Aerith_Idle>();
}

void CState_Aerith_LadderDownFinished::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CState_Aerith_LadderDownFinished::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CState_Aerith_LadderDownFinished::Transition_State(CState* pNextState)
{
	__super::Transition_State(pNextState);

	m_pActor.lock()->Get_PhysXControllerCom().lock()->Set_EnableSimulation(true);
}

bool CState_Aerith_LadderDownFinished::isValid_NextState(CState* state)
{
	return true;
}

void CState_Aerith_LadderDownFinished::Free()
{
}
