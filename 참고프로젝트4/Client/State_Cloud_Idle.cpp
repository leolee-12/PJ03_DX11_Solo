#include "stdafx.h"
#include "State_List_Cloud.h"

CState_Cloud_Idle::CState_Cloud_Idle(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine)
	:CState_Cloud(pActor, pStatemachine)
{
	m_bInputBattleMode = true;
}

HRESULT CState_Cloud_Idle::Initialize_State(CState* pPreviousState)
{
	__super::Initialize_State(pPreviousState);

	auto pMainFSM = m_pStateMachineCom.lock()->Get_RefStateMachine("UpperBody").lock();
	if (pMainFSM->Is_State<CStateUpperBody_Cloud_Guard>())
	{
		m_pActor_ModelCom.lock()->Set_Animation("Battle00|B_Guard01_1", 1.f, true);
	}
	else
	{
		if(m_eCurBattleMode == BATTLE_MODE::NETURAL)
			m_pActor_ModelCom.lock()->Set_Animation(m_strCurBattleMode + "Idle06_1", 1.f, true);
		else
			m_pActor_ModelCom.lock()->Set_Animation(m_strCurBattleMode + "Idle01_1", 1.f, true);
	}
	
	m_pActor_TransformCom.lock()->Set_Move_AnimationPosition(false);
	
	m_fAccTime = 0.f;

	STATE_CLOUD::Reset_VirtualKey();
	return S_OK;
}

void CState_Cloud_Idle::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);

	m_fAccTime += fTimeDelta;
	if (m_fAccTime >= 5.f)
	{
		GET_SINGLE(CClient_Manager)->Set_BattleMode(false);
		GET_SINGLE(CCamera_Manager)->Set_ThirdCam_OffsetType(OFFSET_TYPE::TYPE_LONG);
	}
		
	if (m_eCurBattleMode != BATTLE_MODE::NETURAL)
	{
		if (STATE_CLOUD::Action_Key_Input(m_pGameInstance, m_pStateMachineCom, m_bPlayable))
			return;
	}

	if (m_bBattleModeChange)
	{
		m_pActor_ModelCom.lock()->Set_Animation(m_strCurBattleMode + "Idle01_1", 1.f, true);
		GET_SINGLE(CClient_Manager)->Set_BattleMode(true);
		m_fAccTime = 0.f;
	}

	if (m_DirKeyInfo.bDirKey_EventOn && Get_DirKey_Input_Enable())
	{
		_float fAngle = m_pActor_TransformCom.lock()->Check_Look_On_BaseDir(m_DirKeyInfo.fRadianFromBaseDir);
		_float fDegree = XMConvertToDegrees(fAngle);

		_bool bLShift = m_pGameInstance->Check_KeyState(DIK_LSHIFT, KEY_PRESS);
		if (m_eCurBattleMode == BATTLE_MODE::BRAVE)
			bLShift = false;

		m_pActor_TransformCom.lock()->Set_Move_AnimationPosition(true);
		
		if (m_pActor_ModelCom.lock()->Get_CurrentAnimationName().find("Run") == string::npos && m_pActor_ModelCom.lock()->Get_CurrentAnimationName().find("Walk") == string::npos)
		{
			if (InRange(fAngle, 0.35f * PI, 0.65f * PI, "[)"))
			{
				bLShift ? m_pActor_ModelCom.lock()->Set_Animation(m_strCurBattleMode + "Run01_0_R90", 2.0f, false) : m_pActor_ModelCom.lock()->Set_Animation(m_strCurBattleMode + "Walk01_0_R90", 2.0f, false);
			}
			else if (InRange(fAngle, 1.35f * PI, 1.65f * PI, "[]"))
			{
				bLShift ? m_pActor_ModelCom.lock()->Set_Animation(m_strCurBattleMode + "Run01_0_L90", 2.0f, false) : m_pActor_ModelCom.lock()->Set_Animation(m_strCurBattleMode + "Walk01_0_L90", 2.0f, false);
			}
			else if (InRange(fAngle, 0.75f * PI, 1.f * PI, "[)"))
			{
				bLShift ? m_pActor_ModelCom.lock()->Set_Animation(m_strCurBattleMode + "Run01_0_R180", 2.0f, false) : m_pActor_ModelCom.lock()->Set_Animation(m_strCurBattleMode + "Walk01_0_R180", 2.0f, false);
			}
			else if (InRange(fAngle, 1.f * PI, 1.25f * PI, "[)"))
			{
				bLShift ? m_pActor_ModelCom.lock()->Set_Animation(m_strCurBattleMode + "Run01_0_L180", 2.0f, false) : m_pActor_ModelCom.lock()->Set_Animation(m_strCurBattleMode + "Walk01_0_L180", 2.0f, false);
			}
			else
			{
				bLShift ? m_pActor_ModelCom.lock()->Set_Animation(m_strCurBattleMode + "Run01_0", 1.3f, false) : m_pActor_ModelCom.lock()->Set_Animation(m_strCurBattleMode + "Walk01_0", 1.3f, false);
				m_bIsTurn = true;
			}
		}
		
		
		bLShift ? m_pStateMachineCom.lock()->Wait_State<CState_Cloud_Run>() : m_pStateMachineCom.lock()->Wait_State<CState_Cloud_Walk>();
		Set_DirKey_Input_Enable(false);
	}

	if (m_bIsTurn)
		Turn_Dir(fTimeDelta * 1.5f);
}

void CState_Cloud_Idle::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (m_pGameInstance->Key_DownEx(DIK_0,DIK_MD_LCTRL))
		m_pStateMachineCom.lock()->Set_State<CState_Cloud_Dance>();
}

void CState_Cloud_Idle::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CState_Cloud_Idle::Transition_State(CState* pNextState)
{
	__super::Transition_State(pNextState);

	m_bIsTurn = false;
}

bool CState_Cloud_Idle::isValid_NextState(CState* state)
{
	if (m_pActor_ModelCom.lock()->IsCurrentAnimation(m_strCurBattleMode + "Idle01_1") || m_pActor_ModelCom.lock()->IsCurrentAnimation(m_strCurBattleMode + "Idle06_1"))
		return true;

	if (m_pActor_ModelCom.lock()->IsAnimation_Finished())
		return true;
	else
		return false;

	return true;
}


void CState_Cloud_Idle::Free()
{
}
