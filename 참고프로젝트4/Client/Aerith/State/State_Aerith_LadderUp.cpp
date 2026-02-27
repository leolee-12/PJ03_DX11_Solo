#include "stdafx.h"
#include "Aerith/State/State_List_Aerith.h"

CState_Aerith_LadderUp::CState_Aerith_LadderUp(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine)
	: BASE(pActor, pStatemachine)
{
}

HRESULT CState_Aerith_LadderUp::Initialize_State(CState* pPreviousState)
{
	__super::Initialize_State(pPreviousState);

	m_pActor.lock()->Get_PhysXControllerCom().lock()->Enable_Gravity(false);
	m_pActor.lock()->Get_PhysXControllerCom().lock()->Set_EnableSimulation(false);

	if (typeid(*pPreviousState) != typeid(CState_Aerith_LadderDown))
	{		
		m_pActor_ModelCom.lock()->Set_Animation("Ladder|N_LadderU01_0", 1.3f, false, false);
	}
	else
	{
		m_pActor_ModelCom.lock()->Set_Animation("Ladder|N_LadderU01_1", 1.3f, true, false);
	}

	return S_OK;
}

void CState_Aerith_LadderUp::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);

	if (m_pGameInstance->Key_Pressing(DIK_W))
	{
		if (m_pActor_ModelCom.lock()->IsAnimation_Finished("Ladder|N_LadderU01_0"))
		{
			m_pActor_ModelCom.lock()->Set_Animation("Ladder|N_LadderU01_1", 1.3f, false, false);
		}
		else if (!m_pActor_ModelCom.lock()->IsCurrentAnimation("Ladder|N_LadderU01_0"))
			m_pActor_ModelCom.lock()->Set_Animation("Ladder|N_LadderU01_1", 1.3f, true, false);
	}
	else if (m_pGameInstance->Key_Pressing(DIK_S))
	{
		m_pStateMachineCom.lock()->Enter_State<CState_Aerith_LadderDown>();
	}
	else
	{
		if(!m_pActor_ModelCom.lock()->IsCurrentAnimation("Ladder|N_LadderU01_0"))
			m_pActor_ModelCom.lock()->Set_Animation("Ladder|N_Ladder01_1", 1.3f, true, true, 2.f);
	}

	if (m_pGameInstance->Key_Down(DIK_O))
	{
		m_pStateMachineCom.lock()->Enter_State<CState_Aerith_LadderUpFinished>();
	}
}

void CState_Aerith_LadderUp::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CState_Aerith_LadderUp::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CState_Aerith_LadderUp::Transition_State(CState* pNextState)
{
	__super::Transition_State(pNextState);
}

bool CState_Aerith_LadderUp::isValid_NextState(CState* state)
{
	if (typeid(*state) == typeid(CState_Aerith_LadderUpFinished) || typeid(*state) == typeid(CState_Aerith_LadderDown))
		return true;
	else
		return false;
}

void CState_Aerith_LadderUp::Free()
{
}
