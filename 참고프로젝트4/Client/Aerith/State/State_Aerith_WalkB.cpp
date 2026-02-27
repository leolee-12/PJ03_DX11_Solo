#include "stdafx.h"
#include "Aerith/State/State_list_Aerith.h"

CState_Aerith_WalkB::CState_Aerith_WalkB(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine)
	:CState_Aerith(pActor, pStatemachine)
{
	m_bInputBattleMode = true;
}

HRESULT CState_Aerith_WalkB::Initialize_State(CState* pPreviousState)
{
	__super::Initialize_State(pPreviousState);

	Turn_Dir();

	m_pActor_ModelCom.lock()->Set_Animation(m_strBattleMode[0] + "B_WalkB01_0", 1.0f, false, true);

	m_pActor_TransformCom.lock()->Set_Move_AnimationPosition(true);

	return S_OK;
}

void CState_Aerith_WalkB::Priority_Tick(_cref_time fTimeDelta)
{ 
	__super::Priority_Tick(fTimeDelta);

	if (STATE_AERITH::Action_Key_Input(m_pGameInstance, m_pStateMachineCom, m_bPlayable))
		return;

	if (m_pActor_ModelCom.lock()->IsAnimation_Finished(m_strBattleMode[0] + "B_WalkB01_0"))
	{
		m_pActor_ModelCom.lock()->Set_Animation(m_strBattleMode[0] + "B_WalkB01_1", 1.0f, true, false);
		m_pActor_TransformCom.lock()->Set_Move_AnimationPosition(false);
	}

	if (m_pActor_ModelCom.lock()->IsCurrentAnimation(m_strBattleMode[0] + "B_WalkB01_1"))
	{
		if ((!m_bPlayable && STATE_AERITH::eVirtualMoveKey == STATE_AERITH::VIRTUAL_MOVEKEY_NONE))
		{
			m_pActor_ModelCom.lock()->Set_Animation(m_strBattleMode[0] + "B_WallkB01_2l", 1.5f, false);
			m_pStateMachineCom.lock()->Wait_State<CState_Aerith_Idle>();
			m_pActor_TransformCom.lock()->Set_Move_AnimationPosition(true);
		}
		else if (!m_bPlayable && (STATE_AERITH::eVirtualMoveKey == STATE_AERITH::WALKB))
		{
			Turn_Dir(fTimeDelta * 2.f);
			m_pActor_TransformCom.lock()->Go_Backward(fTimeDelta * 0.5f);
		}
	}
}

void CState_Aerith_WalkB::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CState_Aerith_WalkB::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CState_Aerith_WalkB::Transition_State(CState* pNextState)
{
	__super::Transition_State(pNextState);
}

bool CState_Aerith_WalkB::isValid_NextState(CState* state)
{
	if (m_pActor_ModelCom.lock()->IsCurrentAnimation(m_strCurBattleMode + "B_WallkB01_2l"))
		return m_pActor_ModelCom.lock()->IsAnimation_Finished();
	else
		return true;

	return true;
}

void CState_Aerith_WalkB::Free()
{
}
