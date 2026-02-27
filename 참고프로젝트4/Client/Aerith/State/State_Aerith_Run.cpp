#include "stdafx.h"
#include "Aerith/State/State_List_Aerith.h"

CState_Aerith_Run::CState_Aerith_Run(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine)
	: BASE(pActor, pStatemachine)
{
}

HRESULT CState_Aerith_Run::Initialize_State(CState* pPreviousState)
{
	__super::Initialize_State(pPreviousState);

	Turn_Dir();
	m_pActor_ModelCom.lock()->Set_Animation("Battle00|B_Run01_1", 1.3f, true, false);
	m_pActor_TransformCom.lock()->Set_Move_AnimationPosition(false);

	return S_OK;
}

void CState_Aerith_Run::Priority_Tick(_cref_time fTimeDelta)
{ 
	__super::Priority_Tick(fTimeDelta);

	if (STATE_AERITH::Action_Key_Input(m_pGameInstance, m_pStateMachineCom, m_bPlayable))
		return;

	_bool bKeyInputEnable = Get_DirKey_Input_Enable();


	if ((!m_bPlayable && STATE_AERITH::eVirtualMoveKey == STATE_AERITH::VIRTUAL_MOVEKEY_NONE) || (!m_DirKeyInfo.bDirKey_EventOn && bKeyInputEnable))
	{
		m_pActor_TransformCom.lock()->Set_Move_AnimationPosition(true);
		m_pActor_ModelCom.lock()->Set_Animation("Battle00|B_Run01_2l", 1.5f, false);
		m_pStateMachineCom.lock()->Wait_State<CState_Aerith_Idle>();

		Set_DirKey_Input_Enable(false);
	}
	else if ((!m_bPlayable && (STATE_AERITH::eVirtualMoveKey == STATE_AERITH::RUN || STATE_AERITH::eVirtualMoveKey == STATE_AERITH::WALK)) || bKeyInputEnable)
	{
		if ((!m_bPlayable && (STATE_AERITH::eVirtualMoveKey == STATE_AERITH::WALK)) || (m_bPlayable && bKeyInputEnable && !m_pGameInstance->Check_KeyState(DIK_LSHIFT, KEY_PRESS)))
		{
			m_pStateMachineCom.lock()->Enter_State<CState_Aerith_Walk>();
		}
		else
		{
			Turn_Dir(fTimeDelta * 2.f);
			m_pActor_TransformCom.lock()->Go_Straight(fTimeDelta * 2.0f);
		}

	}
}

void CState_Aerith_Run::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CState_Aerith_Run::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CState_Aerith_Run::Transition_State(CState* pNextState)
{
	__super::Transition_State(pNextState);
}

bool CState_Aerith_Run::isValid_NextState(CState* state)
{
	if (m_pActor_ModelCom.lock()->IsCurrentAnimation("Battle00|B_Run01_2"))
		return m_pActor_ModelCom.lock()->IsAnimation_Finished();
	else
		return true;

	return true;
}


void CState_Aerith_Run::Free()
{
}
