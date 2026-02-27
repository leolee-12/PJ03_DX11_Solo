#include "stdafx.h"
#include "State_list_Cloud.h"

CState_Cloud_Guard::CState_Cloud_Guard(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine)
	:CState_Cloud(pActor, pStatemachine)
{
}

HRESULT CState_Cloud_Guard::Initialize_State(CState* pPreviousState)
{
	__super::Initialize_State(pPreviousState);

	m_pActor_ModelCom.lock()->Set_Animation("Battle00|B_Guard01_0", 1.f, false, false, 1.f);

	return S_OK;
}

void CState_Cloud_Guard::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
	
	// 가드 돌입
	if (m_pActor_ModelCom.lock()->IsAnimation_Finished("Battle00|B_Guard01_0"))
	{
		// UpperBody 상태 가드로 변경 후, Idle로 복귀
		shared_ptr<CStateMachine> pUpperBodyFSM = m_pStateMachineCom.lock()->Get_RefStateMachine("UpperBody").lock();
		//m_strCurBattleMode = DynCast<CState_Cloud*>(pUpperBodyFSM->Get_CurState())->Get_CurrentBattleModeName();
		pUpperBodyFSM->Enter_State<CStateUpperBody_Cloud_Guard>();

		m_pStateMachineCom.lock()->Enter_State<CState_Cloud_Idle>();
	}
}

void CState_Cloud_Guard::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CState_Cloud_Guard::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CState_Cloud_Guard::Transition_State(CState* pNextState)
{
	__super::Transition_State(pNextState);
}

bool CState_Cloud_Guard::isValid_NextState(CState* state)
{
	return true;
}

void CState_Cloud_Guard::Free()
{
}
