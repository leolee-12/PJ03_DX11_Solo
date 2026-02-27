#include "stdafx.h"
#include "State_List_MonoDrive.h"


CState_MonoDrive_Idle::CState_MonoDrive_Idle(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine)
	:CState_MonoDrive(pActor, pStatemachine)
{
}

HRESULT CState_MonoDrive_Idle::Initialize_State(CState* pPreviousState)
{
	__super::Initialize_State(pPreviousState);

	m_pActor_ModelCom.lock()->Set_Animation("Main|B_Idle01_0",0.7f,false);
	m_pActor_TransformCom.lock()->Set_Move_AnimationPosition(false);

	return S_OK;
}

void CState_MonoDrive_Idle::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);

	if (InRange(m_fDistance, 0.f, 8.f, "[]"))
	{
		m_pStateMachineCom.lock()->Enter_State<CState_MonoDrive_Walk>();
	}

	if (m_pActor_ModelCom.lock()->IsAnimation_Finished("Main|B_Idle01_0"))
	{
		m_pActor_ModelCom.lock()->Set_Animation("Main|B_Idle01_1", 1.f, false);
	}

}

void CState_MonoDrive_Idle::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CState_MonoDrive_Idle::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CState_MonoDrive_Idle::Transition_State(CState* pNextState)
{
	__super::Transition_State(pNextState);
}

bool CState_MonoDrive_Idle::isValid_NextState(CState* state)
{
	return true;
}

void CState_MonoDrive_Idle::Free()
{
}
