#include "stdafx.h"
#include "State_list_Cloud.h"

CState_Cloud_GuardRelease::CState_Cloud_GuardRelease(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine)
	:CState_Cloud(pActor, pStatemachine)
{
}

HRESULT CState_Cloud_GuardRelease::Initialize_State(CState* pPreviousState)
{
	__super::Initialize_State(pPreviousState);

	m_pActor_ModelCom.lock()->Set_Animation("Battle00|B_Guard01_2", 1.3f, false, true, 2.f);

	auto pUpperBodyFSM = m_pStateMachineCom.lock()->Get_RefStateMachine("UpperBody").lock();
	pUpperBodyFSM->Enter_State<CStateUpperBody_Cloud_Idle>();

	return S_OK;
}

void CState_Cloud_GuardRelease::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);

	// 애니메이션 끝나면 복귀
	if (m_pActor_ModelCom.lock()->IsAnimation_Finished("Battle00|B_Guard01_2"))
	{
		m_pStateMachineCom.lock()->Enter_State<CState_Cloud_Idle>();
	}
}

void CState_Cloud_GuardRelease::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CState_Cloud_GuardRelease::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CState_Cloud_GuardRelease::Transition_State(CState* pNextState)
{
	__super::Transition_State(pNextState);
}

bool CState_Cloud_GuardRelease::isValid_NextState(CState* state)
{
	return true;
}

void CState_Cloud_GuardRelease::Free()
{
}
