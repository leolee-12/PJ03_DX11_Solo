#include "stdafx.h"
#include "State_List_Sweeper.h"


CState_Sweeper_Run::CState_Sweeper_Run(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine)
	:CState_Sweeper(pActor, pStatemachine)
{
}

HRESULT CState_Sweeper_Run::Initialize_State(CState* pPreviousState)
{
	__super::Initialize_State(pPreviousState);

	m_pActor_ModelCom.lock()->Set_Animation("Main|B_Run01_0", 1.f, false);
	return S_OK;
}

void CState_Sweeper_Run::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);

		if (m_pActor_ModelCom.lock()->Get_CurrentAnimationName() != "Main|B_Run01_2")
	{
		m_pActor_TransformCom.lock()->Go_Straight(fTimeDelta * 1.5f);
	}

	m_pActor_TransformCom.lock()->Look_At_OnLand(m_vTargetPos);

	if (m_pActor_ModelCom.lock()->IsAnimation_Finished("Main|B_Run01_0"))
	{
		m_pActor_ModelCom.lock()->Set_Animation("Main|B_Run01_1", 1.f, true);
	}

	if (m_pActor_ModelCom.lock()->IsAnimation_Finished("Main|B_Run01_2"))
	{
		_int iRandom = rand() % 3;

		if (iRandom == 0)
			m_pStateMachineCom.lock()->Enter_State<CState_Sweeper_LShoot>();
		else if(iRandom ==1)
			m_pStateMachineCom.lock()->Enter_State<CState_Sweeper_RShoot>();
		else
			m_pStateMachineCom.lock()->Enter_State<CState_Sweeper_MachineGun>();
	}

}

void CState_Sweeper_Run::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (m_fDistance <= 7.f && m_pActor_ModelCom.lock()->Get_CurrentAnimationName() != "Main|B_Run01_2")
	{
		m_pActor_ModelCom.lock()->Set_Animation("Main|B_Run01_2", 1.f, false);
	}


}

void CState_Sweeper_Run::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CState_Sweeper_Run::Transition_State(CState* pNextState)
{
	__super::Transition_State(pNextState);
}

bool CState_Sweeper_Run::isValid_NextState(CState* state)
{
	return true;
}

void CState_Sweeper_Run::Free()
{
}
