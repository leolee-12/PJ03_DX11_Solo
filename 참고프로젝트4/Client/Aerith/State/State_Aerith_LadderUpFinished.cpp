#include "stdafx.h"
#include "Aerith/State/State_List_Aerith.h"

CState_Aerith_LadderUpFinished::CState_Aerith_LadderUpFinished(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine)
	: BASE(pActor, pStatemachine)
{
}

HRESULT CState_Aerith_LadderUpFinished::Initialize_State(CState* pPreviousState)
{
	__super::Initialize_State(pPreviousState);

	m_pActor_ModelCom.lock()->Set_Animation("Ladder|N_LadderU01_2", 1.3f, false, true, 2.f);
	return S_OK;
}

void CState_Aerith_LadderUpFinished::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);

	if (m_pActor_ModelCom.lock()->IsAnimation_Finished())
		m_pStateMachineCom.lock()->Enter_State<CState_Aerith_Idle>();
}

void CState_Aerith_LadderUpFinished::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CState_Aerith_LadderUpFinished::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CState_Aerith_LadderUpFinished::Transition_State(CState* pNextState)
{
	__super::Transition_State(pNextState);

	m_pActor.lock()->Get_PhysXControllerCom().lock()->Enable_Gravity(true);
	m_pActor.lock()->Get_PhysXControllerCom().lock()->Set_EnableSimulation(true);
}

bool CState_Aerith_LadderUpFinished::isValid_NextState(CState* state)
{
	return true;
}

void CState_Aerith_LadderUpFinished::Free()
{
}
