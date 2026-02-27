#include "stdafx.h"
#include "State_List_Soldier_Gun.h"
#include "GameInstance.h"

CState_Soldier_Gun_Idle::CState_Soldier_Gun_Idle(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine)
	:CState_Soldier_Gun(pActor, pStatemachine)
{
}

HRESULT CState_Soldier_Gun_Idle::Initialize_State(CState* pPreviousState)
{
	__super::Initialize_State(pPreviousState);

	m_pActor_ModelCom.lock()->Set_Animation("Main|N_Idle02_0", 1.f, false);
	m_pActor_TransformCom.lock()->Set_Move_AnimationPosition(true);

	return S_OK;
}

void CState_Soldier_Gun_Idle::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);

	if (m_pActor_ModelCom.lock()->IsAnimation_Finished())
	{
		_int iRandom = m_pGameInstance->RandomInt(1, 5);

		switch (iRandom)
		{
		case 1:
			m_pActor_ModelCom.lock()->Set_Animation("Main|N_Idle02_0", 1.f, false);
			break;
		case 2:
			m_pActor_ModelCom.lock()->Set_Animation("Main|N_Idle03_0", 1.f, false);
			break;
		case 3:
			m_pActor_ModelCom.lock()->Set_Animation("Main|N_Idle04_0", 1.f, false);
			break;
		case 4:
			m_pActor_ModelCom.lock()->Set_Animation("Main|N_Idle07", 1.f, false);
			break;
		case 5:
			m_pActor_ModelCom.lock()->Set_Animation("Main|N_Idle08", 1.f, false);
			break;
		}
	}



	if (XMVectorGetX(XMVector3Length(m_vPos - m_vTargetPos)) <= 15.0f)
	{
		m_pStateMachineCom.lock()->Enter_State<CState_Soldier_Gun_Run>();
	}

}

void CState_Soldier_Gun_Idle::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CState_Soldier_Gun_Idle::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CState_Soldier_Gun_Idle::Transition_State(CState* pNextState)
{
	__super::Transition_State(pNextState);
}

bool CState_Soldier_Gun_Idle::isValid_NextState(CState* state)
{
	return true;
}

void CState_Soldier_Gun_Idle::Free()
{
}
