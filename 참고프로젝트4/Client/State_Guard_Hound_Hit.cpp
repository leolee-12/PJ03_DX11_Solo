#include "stdafx.h"
#include "State_List_Guard_Hound.h"


CState_Guard_Hound_Hit::CState_Guard_Hound_Hit(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine)
	:CState_Monster(pActor, pStatemachine)
{
}

HRESULT CState_Guard_Hound_Hit::Initialize_State(CState* pPreviousState)
{
	__super::Initialize_State(pPreviousState);
	m_pActor_ModelCom.lock()->Set_Animation("Main|B_DmgF01", 1.f, false);
	m_pActor_TransformCom.lock()->Set_Move_AnimationPosition(true);
	return S_OK;
}

void CState_Guard_Hound_Hit::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
	if(m_pActor_ModelCom.lock()->IsAnimation_Finished())
		m_pStateMachineCom.lock()->Enter_State<CState_Guard_Hound_Run>();
}

void CState_Guard_Hound_Hit::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CState_Guard_Hound_Hit::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CState_Guard_Hound_Hit::Transition_State(CState* pNextState)
{
	__super::Transition_State(pNextState);
}

bool CState_Guard_Hound_Hit::isValid_NextState(CState* state)
{
	return true;
}

void CState_Guard_Hound_Hit::Free()
{
}
