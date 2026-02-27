#include "stdafx.h"
#include "Aerith/State/State_List_Aerith.h"

CState_Aerith_LadderDown::CState_Aerith_LadderDown(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine)
	: BASE(pActor, pStatemachine)
{
}

HRESULT CState_Aerith_LadderDown::Initialize_State(CState* pPreviousState)
{
	__super::Initialize_State(pPreviousState);

	m_pActor.lock()->Get_PhysXControllerCom().lock()->Enable_Gravity(false);
	m_pActor.lock()->Get_PhysXControllerCom().lock()->Set_EnableSimulation(false);

	if (typeid(*pPreviousState) != typeid(CState_Aerith_LadderUp))
	{
		m_pActor_ModelCom.lock()->Set_Animation("Ladder|N_LadderD01_0", 1.3f, false, false);
	}
	else
	{
		m_pActor_ModelCom.lock()->Set_Animation("Ladder|N_LadderD02_0", 1.3f, false, false);
	}
	
	return S_OK;
}

void CState_Aerith_LadderDown::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);

	if (m_pGameInstance->Key_Pressing(DIK_S))
	{
		if (m_pActor_ModelCom.lock()->IsAnimation_Finished("Ladder|N_LadderD01_0"))
		{
			m_pActor_TransformCom.lock()->Set_Look(-1.f * m_pActor_TransformCom.lock()->Get_State(CTransform::STATE_LOOK));
			m_pActor_ModelCom.lock()->Set_Animation("Ladder|N_LadderD02_0", 1.3f, false, false);
		}
		else if (m_pActor_ModelCom.lock()->IsAnimation_Finished("Ladder|N_LadderD02_0"))
			m_pActor_ModelCom.lock()->Set_Animation("Ladder|N_LadderD02_1", 1.3f, true, false);
		else if(!m_pActor_ModelCom.lock()->IsCurrentAnimation("Ladder|N_LadderD01_0") && !m_pActor_ModelCom.lock()->IsCurrentAnimation("Ladder|N_LadderD02_1") && !m_pActor_ModelCom.lock()->IsCurrentAnimation("Ladder|N_LadderD02_0"))
			m_pActor_ModelCom.lock()->Set_Animation("Ladder|N_LadderD02_0", 1.3f, true, false);
	}
	else if (m_pGameInstance->Key_Pressing(DIK_W))
	{
		m_pStateMachineCom.lock()->Enter_State<CState_Aerith_LadderUp>();
	}
	else
	{
		if (!m_pActor_ModelCom.lock()->IsCurrentAnimation("Ladder|N_LadderD02_2") && !m_pActor_ModelCom.lock()->IsCurrentAnimation("Ladder|N_LadderD01_0") && !m_pActor_ModelCom.lock()->IsCurrentAnimation("Ladder|N_Ladder01_1"))
			m_pActor_ModelCom.lock()->Set_Animation("Ladder|N_LadderD02_2", 1.3f, false, false);
		else if (m_pActor_ModelCom.lock()->IsAnimation_Finished("Ladder|N_LadderD02_2"))
			m_pActor_ModelCom.lock()->Set_Animation("Ladder|N_Ladder01_1", 1.3f, true, true, 2.f);
	}

	if (m_pActor_ModelCom.lock()->IsCurrentAnimation("Ladder|N_LadderD02_1"))
	{
		m_pActor.lock()->Get_PhysXControllerCom().lock()->Enable_Gravity(true);
		m_pActor_TransformCom.lock()->Set_Move_AnimationPosition(false);
	}
	else
	{
		m_pActor.lock()->Get_PhysXControllerCom().lock()->Enable_Gravity(false);
		m_pActor_TransformCom.lock()->Set_Move_AnimationPosition(true);
	}

	if (m_pGameInstance->Key_Down(DIK_O))
	{
			m_pStateMachineCom.lock()->Enter_State<CState_Aerith_LadderDownFinished>();
	}
}

void CState_Aerith_LadderDown::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CState_Aerith_LadderDown::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CState_Aerith_LadderDown::Transition_State(CState* pNextState)
{
	__super::Transition_State(pNextState);
}

bool CState_Aerith_LadderDown::isValid_NextState(CState* state)
{
	if (typeid(*state) == typeid(CState_Aerith_LadderDownFinished) || typeid(*state) == typeid(CState_Aerith_LadderUp))
		return true;
	else
		return false;
}

void CState_Aerith_LadderDown::Free()
{
}
