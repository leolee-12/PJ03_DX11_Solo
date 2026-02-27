#include "stdafx.h"
#include "State_List_MonoDrive.h"


CState_MonoDrive_Hit::CState_MonoDrive_Hit(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine)
	:CState_MonoDrive(pActor, pStatemachine)
{
}

HRESULT CState_MonoDrive_Hit::Initialize_State(CState* pPreviousState)
{
	__super::Initialize_State(pPreviousState);
	m_pActor_ModelCom.lock()->Set_Animation("Main|B_DmgF01", 1.f, false);
	m_pActor_TransformCom.lock()->Set_Move_AnimationPosition(true);
	return S_OK;
}

void CState_MonoDrive_Hit::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);

	if (m_pActor_ModelCom.lock()->IsAnimation_Finished())
		m_pStateMachineCom.lock()->Enter_State<CState_MonoDrive_Idle>();
}

void CState_MonoDrive_Hit::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CState_MonoDrive_Hit::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CState_MonoDrive_Hit::Transition_State(CState* pNextState)
{
	__super::Transition_State(pNextState);
}

bool CState_MonoDrive_Hit::isValid_NextState(CState* state)
{
	return true;
}

void CState_MonoDrive_Hit::Free()
{
}
