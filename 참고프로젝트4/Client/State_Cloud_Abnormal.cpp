#include "stdafx.h"
#include "State_list_Cloud.h"

CState_Cloud_Abnormal::CState_Cloud_Abnormal(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine)
	:CState_Cloud(pActor, pStatemachine)
{
}

HRESULT CState_Cloud_Abnormal::Initialize_State(CState* pPreviousState)
{
	__super::Initialize_State(pPreviousState);

	m_pActor_ModelCom.lock()->Set_Animation("Battle00|B_Abnormal01_0", 1.3f, false, true, 2.f);

	return S_OK;
}

void CState_Cloud_Abnormal::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);

	if (m_pActor_ModelCom.lock()->IsAnimation_Finished("Battle00|B_Abnormal01_0"))
		m_pActor_ModelCom.lock()->Set_Animation("Battle00|B_Abnormal01_1", 1.3f, true, true, 2.f);

	if (m_bIsNormal)
		m_pActor_ModelCom.lock()->Set_Animation("Battle00|B_Abnormal01_2", 1.3f, false, true, 2.f);

	if (m_pActor_ModelCom.lock()->IsAnimation_Finished("Battle00|B_Abnormal01_2"))
		m_pStateMachineCom.lock()->Enter_State<CState_Cloud_Idle>();
}

void CState_Cloud_Abnormal::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CState_Cloud_Abnormal::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CState_Cloud_Abnormal::Transition_State(CState* pNextState)
{
	__super::Transition_State(pNextState);
	m_bIsNormal = false;
}

bool CState_Cloud_Abnormal::isValid_NextState(CState* state)
{
	return true;
}

void CState_Cloud_Abnormal::Free()
{
}
