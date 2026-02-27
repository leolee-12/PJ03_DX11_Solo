#include "stdafx.h"
#include "State_List_Stray.h"


CState_Stray_Hit::CState_Stray_Hit(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine)
	:CState_Stray(pActor, pStatemachine)
{
}

HRESULT CState_Stray_Hit::Initialize_State(CState* pPreviousState)
{
	__super::Initialize_State(pPreviousState);
	m_pActor_ModelCom.lock()->Set_Animation("Main|B_DmgL01", 1.f, false);
	m_pActor_TransformCom.lock()->Set_Move_AnimationPosition(true);
	return S_OK;
}

void CState_Stray_Hit::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);

	if (m_pActor_ModelCom.lock()->IsAnimation_Finished())
		m_pStateMachineCom.lock()->Enter_State<CState_Stray_Idle>();
}

void CState_Stray_Hit::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CState_Stray_Hit::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CState_Stray_Hit::Transition_State(CState* pNextState)
{
	__super::Transition_State(pNextState);
}

bool CState_Stray_Hit::isValid_NextState(CState* state)
{
	return true;
}

void CState_Stray_Hit::Free()
{
}
