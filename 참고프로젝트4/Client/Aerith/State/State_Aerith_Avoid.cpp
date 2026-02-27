#include "stdafx.h"
#include "Aerith/State/State_list_Aerith.h"

CState_Aerith_Avoid::CState_Aerith_Avoid(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine)
	: BASE(pActor, pStatemachine)
{
}

HRESULT CState_Aerith_Avoid::Initialize_State(CState* pPreviousState)
{
	__super::Initialize_State(pPreviousState);
	 

	m_iAvoidType = Random({ 0 });
	
	m_iAvoidType == 0 ? m_pActor_ModelCom.lock()->Set_Animation("Battle00|B_Avoid01", 1.3f, false, true, 2.f)
		: m_pActor_ModelCom.lock()->Set_Animation("Battle00|B_Avoid02", 1.3f, false, true, 2.f);

	return S_OK;
}

void CState_Aerith_Avoid::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
	
	if (m_pActor_ModelCom.lock()->IsAnimation_Finished())
		m_pStateMachineCom.lock()->Enter_State<CState_Aerith_Idle>();
}

void CState_Aerith_Avoid::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CState_Aerith_Avoid::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CState_Aerith_Avoid::Transition_State(CState* pNextState)
{
	__super::Transition_State(pNextState);
}

bool CState_Aerith_Avoid::isValid_NextState(CState* state)
{
	return true;
}

void CState_Aerith_Avoid::Free()
{
}
