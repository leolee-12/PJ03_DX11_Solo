#include "stdafx.h"
#include "State_List_Guard_Hound.h"


CState_Guard_Hound_Idle::CState_Guard_Hound_Idle(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine)
	:CState_Monster(pActor, pStatemachine)
{
}

HRESULT CState_Guard_Hound_Idle::Initialize_State(CState* pPreviousState)
{
	__super::Initialize_State(pPreviousState);

	m_pActor_ModelCom.lock()->Set_Animation("Main|B_Idle01_1",1.f,true);
	m_pActor_TransformCom.lock()->Set_Move_AnimationPosition(true);

	return S_OK;
}

void CState_Guard_Hound_Idle::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);

	if (XMVectorGetX(XMVector3Length(m_vPos - m_vTargetPos)) <= 10.0f)
	{
		m_pStateMachineCom.lock()->Enter_State<CState_Guard_Hound_Run>();
	}

}

void CState_Guard_Hound_Idle::Tick(_cref_time fTimeDelta) 
{
	__super::Tick(fTimeDelta);
}

void CState_Guard_Hound_Idle::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CState_Guard_Hound_Idle::Transition_State(CState* pNextState)
{
	__super::Transition_State(pNextState);
}

bool CState_Guard_Hound_Idle::isValid_NextState(CState* state)
{
	return true;
}

void CState_Guard_Hound_Idle::Free()
{
}
