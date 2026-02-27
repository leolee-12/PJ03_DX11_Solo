#include "stdafx.h"
#include "State_list_Cloud.h"

CState_Cloud_LadderUp::CState_Cloud_LadderUp(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine)
	:CState_Cloud(pActor, pStatemachine)
{
}

HRESULT CState_Cloud_LadderUp::Initialize_State(CState* pPreviousState)
{
	__super::Initialize_State(pPreviousState);

	m_pActor.lock()->Get_PhysXControllerCom().lock()->Enable_Gravity(false);
	m_pActor.lock()->Get_PhysXControllerCom().lock()->Set_EnableSimulation(false);

	if (typeid(*pPreviousState) != typeid(CState_Cloud_LadderDown))
	{		
		m_pActor_ModelCom.lock()->Set_Animation("Ladder|N_LadderU01_0", 1.3f, false, false);
	}
	else
	{
		m_pActor_ModelCom.lock()->Set_Animation("Ladder|N_LadderU01_1", 1.3f, true, false);
	}

	return S_OK;
}

void CState_Cloud_LadderUp::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);

	if (m_pGameInstance->Key_Pressing(DIK_W))
	{
		if (m_pActor_ModelCom.lock()->IsAnimation_Finished("Ladder|N_LadderU01_0"))
		{
			m_pActor_ModelCom.lock()->Set_Animation("Ladder|N_LadderU01_1", 1.3f, false, false);
		}
		else if (!m_pActor_ModelCom.lock()->IsCurrentAnimation("Ladder|N_LadderU01_0") && !m_pActor_ModelCom.lock()->IsCurrentAnimation("Ladder|N_LadderU01_1"))
			m_pActor_ModelCom.lock()->Set_Animation("Ladder|N_LadderU01_1", 1.3f, true, false);
	}
	else if (m_pGameInstance->Key_Pressing(DIK_S))
	{
		if (!m_pActor_ModelCom.lock()->IsCurrentAnimation("Ladder|N_LadderU01_0"))
			m_pStateMachineCom.lock()->Enter_State<CState_Cloud_LadderDown>();
	}
	else
	{
		if(!m_pActor_ModelCom.lock()->IsCurrentAnimation("Ladder|N_LadderU01_0"))
			m_pActor_ModelCom.lock()->Set_Animation("Ladder|N_Ladder01_1", 1.3f, true, true, 2.f);
	}

	if (m_pGameInstance->Key_Down(DIK_O))
	{
		m_pStateMachineCom.lock()->Enter_State<CState_Cloud_LadderUpFinished>();
	}
}

void CState_Cloud_LadderUp::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CState_Cloud_LadderUp::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CState_Cloud_LadderUp::Transition_State(CState* pNextState)
{
	__super::Transition_State(pNextState);
}

bool CState_Cloud_LadderUp::isValid_NextState(CState* state)
{
	if (typeid(*state) == typeid(CState_Cloud_LadderUpFinished) || typeid(*state) == typeid(CState_Cloud_LadderDown))
		return true;
	else
		return false;
}

void CState_Cloud_LadderUp::Free()
{
}
