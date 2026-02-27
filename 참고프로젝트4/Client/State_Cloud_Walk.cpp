#include "stdafx.h"
#include "State_list_Cloud.h"

CState_Cloud_Walk::CState_Cloud_Walk(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine)
	:CState_Cloud(pActor, pStatemachine)
{
	m_bInputBattleMode = true;
}

HRESULT CState_Cloud_Walk::Initialize_State(CState* pPreviousState)
{
	__super::Initialize_State(pPreviousState);

	Turn_Dir();
	if(typeid(*pPreviousState) == typeid(CState_Cloud_Run))
		m_pActor_ModelCom.lock()->Set_Animation(m_strCurBattleMode + "Walk01_1", 1.0f, true, true,1.5f);
	else
		m_pActor_ModelCom.lock()->Set_Animation(m_strCurBattleMode + "Walk01_1", 1.0f, true, false);

	m_pActor_TransformCom.lock()->Set_Move_AnimationPosition(false);

	return S_OK;
}

void CState_Cloud_Walk::Priority_Tick(_cref_time fTimeDelta)
{ 
	__super::Priority_Tick(fTimeDelta);

	if (m_eCurBattleMode != BATTLE_MODE::NETURAL)
	{
		if (STATE_CLOUD::Action_Key_Input(m_pGameInstance,m_pStateMachineCom,m_bPlayable))
			return;
	}


	_bool bKeyInputEnable = Get_DirKey_Input_Enable();

	if ((!m_bPlayable && STATE_CLOUD::eVirtualMoveKey == STATE_CLOUD::VIRTUAL_MOVEKEY_NONE) || (!m_DirKeyInfo.bDirKey_EventOn && bKeyInputEnable))
	{
		m_pActor_ModelCom.lock()->Set_Animation(m_strCurBattleMode + "Walk01_2r", 1.5f, false);
		m_pStateMachineCom.lock()->Wait_State<CState_Cloud_Idle>();

		Set_DirKey_Input_Enable(false);
	}
	else if ((!m_bPlayable && (STATE_CLOUD::eVirtualMoveKey == STATE_CLOUD::RUN || STATE_CLOUD::eVirtualMoveKey == STATE_CLOUD::WALK)) || bKeyInputEnable)
	{
		Turn_Dir(fTimeDelta * 2.f);
		_float fSpeed = m_eCurBattleMode == BATTLE_MODE::ASSERT ? 0.6f : 0.4f;
		m_pActor_TransformCom.lock()->Go_Straight(fTimeDelta * fSpeed);

		if (m_bBattleModeChange)
		{
			m_pActor_ModelCom.lock()->Set_Animation(m_strCurBattleMode + "Walk01_1", 1.f, true);
			GET_SINGLE(CClient_Manager)->Set_BattleMode(true);
		}


		if ((!m_bPlayable && (STATE_CLOUD::eVirtualMoveKey == STATE_CLOUD::RUN)) || (m_bPlayable && m_pGameInstance->Check_KeyState(DIK_LSHIFT, KEY_PRESS) && m_eCurBattleMode != BATTLE_MODE::BRAVE))
		{
			m_pActor_ModelCom.lock()->Set_Animation(m_strCurBattleMode + "Run01_0", 2.0f, false);
			m_pStateMachineCom.lock()->Wait_State<CState_Cloud_Run>();
		}
	}
}

void CState_Cloud_Walk::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CState_Cloud_Walk::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CState_Cloud_Walk::Transition_State(CState* pNextState)
{
	__super::Transition_State(pNextState);
}

bool CState_Cloud_Walk::isValid_NextState(CState* state)
{
	if (!m_pActor_ModelCom.lock()->IsCurrentAnimation(m_strCurBattleMode + "Walk01_1"))
		return m_pActor_ModelCom.lock()->IsAnimation_Finished();
	else
		return true;

	return true;
}

void CState_Cloud_Walk::Free()
{
}
