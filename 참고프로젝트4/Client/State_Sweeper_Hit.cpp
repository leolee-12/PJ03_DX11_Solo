#include "stdafx.h"
#include "State_List_Sweeper.h"


CState_Sweeper_Hit::CState_Sweeper_Hit(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine)
	:CState_Sweeper(pActor, pStatemachine)
{
}

HRESULT CState_Sweeper_Hit::Initialize_State(CState* pPreviousState)
{
	__super::Initialize_State(pPreviousState);
	m_pActor_ModelCom.lock()->Set_Animation("Main|B_DmgL01", 1.f, false);
	m_pActor_TransformCom.lock()->Set_Move_AnimationPosition(true);
	return S_OK;
}

void CState_Sweeper_Hit::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);

	if (m_pActor_ModelCom.lock()->IsAnimation_Finished())
		m_pStateMachineCom.lock()->Enter_State<CState_Sweeper_Idle>();
}

void CState_Sweeper_Hit::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CState_Sweeper_Hit::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CState_Sweeper_Hit::Transition_State(CState* pNextState)
{
	__super::Transition_State(pNextState);
}

bool CState_Sweeper_Hit::isValid_NextState(CState* state)
{
	return true;
}

void CState_Sweeper_Hit::Free()
{
}
