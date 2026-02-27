#include "stdafx.h"
#include "Aerith/State/State_List_Aerith.h"

CState_Aerith_Walk::CState_Aerith_Walk(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine)
	: BASE(pActor, pStatemachine)
{
	m_bInputBattleMode = true;
}

HRESULT CState_Aerith_Walk::Initialize_State(CState* pPreviousState)
{
	__super::Initialize_State(pPreviousState);

	Turn_Dir();
	if(typeid(*pPreviousState) == typeid(CState_Aerith_Run))
		m_pActor_ModelCom.lock()->Set_Animation(m_strCurBattleMode + "B_Walk01_1", 1.0f, true, true, 1.5f);
	else
		m_pActor_ModelCom.lock()->Set_Animation(m_strCurBattleMode + "B_Walk01_1", 1.0f, true, false);

	m_pActor_TransformCom.lock()->Set_Move_AnimationPosition(false);

	return S_OK;
}

void CState_Aerith_Walk::Priority_Tick(_cref_time fTimeDelta)
{ 
	__super::Priority_Tick(fTimeDelta);

	if (STATE_AERITH::Action_Key_Input(m_pGameInstance,m_pStateMachineCom, m_bPlayable))
		return;


	_bool bKeyInputEnable = Get_DirKey_Input_Enable();

	if ((!m_bPlayable && STATE_AERITH::eVirtualMoveKey == STATE_AERITH::VIRTUAL_MOVEKEY_NONE) || (!m_DirKeyInfo.bDirKey_EventOn && bKeyInputEnable))
	{
		m_pActor_ModelCom.lock()->Set_Animation(m_strCurBattleMode + "B_Walk01_2r", 1.5f, false);
		m_pStateMachineCom.lock()->Wait_State<CState_Aerith_Idle>();

		Set_DirKey_Input_Enable(false);
	}
	else if ((!m_bPlayable && (STATE_AERITH::eVirtualMoveKey == STATE_AERITH::RUN || STATE_AERITH::eVirtualMoveKey == STATE_AERITH::WALK)) || bKeyInputEnable)
	{
		Turn_Dir(fTimeDelta * 2.f);
		m_pActor_TransformCom.lock()->Go_Straight(fTimeDelta * 0.6f);

		if ((!m_bPlayable && (STATE_AERITH::eVirtualMoveKey == STATE_AERITH::RUN)) || (m_bPlayable && m_pGameInstance->Check_KeyState(DIK_LSHIFT, KEY_PRESS) && m_eCurBattleMode == BATTLE_MODE::ASSERT))
		{
			m_pActor_ModelCom.lock()->Set_Animation("Battle00|B_Run01_0", 2.0f, false);
			m_pStateMachineCom.lock()->Wait_State<CState_Aerith_Run>();
		}
	}
}

void CState_Aerith_Walk::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CState_Aerith_Walk::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CState_Aerith_Walk::Transition_State(CState* pNextState)
{
	__super::Transition_State(pNextState);
}

bool CState_Aerith_Walk::isValid_NextState(CState* state)
{
	if (!m_pActor_ModelCom.lock()->IsCurrentAnimation(m_strCurBattleMode + "B_Walk01_1"))
		return m_pActor_ModelCom.lock()->IsAnimation_Finished();
	else
		return true;

	return true;
}

void CState_Aerith_Walk::Free()
{
}
