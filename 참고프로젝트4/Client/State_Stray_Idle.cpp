#include "stdafx.h"
#include "State_List_Stray.h"

CState_Stray_Idle::CState_Stray_Idle(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine)
	:CState_Stray(pActor, pStatemachine)
{
}

HRESULT CState_Stray_Idle::Initialize_State(CState* pPreviousState)
{
	__super::Initialize_State(pPreviousState);

	m_pActor_ModelCom.lock()->Set_Animation("Main|B_Idle01_0", 1.f, false);
	m_pActor_TransformCom.lock()->Set_Move_AnimationPosition(false);

	return S_OK;
}

void CState_Stray_Idle::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);

  	if (m_pActor_ModelCom.lock()->IsAnimation_Finished("Main|B_Idle01_2") && InRange(m_fDistance, 0.f, 15.f, "[]"))
		m_pStateMachineCom.lock()->Enter_State<CState_Stray_Shoot>();

	if (m_pActor_ModelCom.lock()->IsAnimation_Finished("Main|B_Idle01_0"))
	{
		m_pActor_ModelCom.lock()->Set_Animation("Main|B_Idle01_1", 2.f, false);
	}

	else if (m_pActor_ModelCom.lock()->IsAnimation_Finished("Main|B_Idle01_1"))
	{
		m_pActor_ModelCom.lock()->Set_Animation("Main|B_Idle01_2", 2.f, false);
	}

	else if (m_pActor_ModelCom.lock()->IsAnimation_Finished("Main|B_Idle01_2"))
	{
		m_pActor_ModelCom.lock()->Set_Animation("Main|B_Idle01_0", 2.f, false);
	}

}

void CState_Stray_Idle::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);


}

void CState_Stray_Idle::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CState_Stray_Idle::Transition_State(CState* pNextState)
{
	__super::Transition_State(pNextState);
}

bool CState_Stray_Idle::isValid_NextState(CState* state)
{
	return true;
}

void CState_Stray_Idle::Free()
{
}
