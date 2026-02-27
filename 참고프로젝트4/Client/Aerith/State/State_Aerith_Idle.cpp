#include "stdafx.h"
#include "Aerith/State/State_List_Aerith.h"

CState_Aerith_Idle::CState_Aerith_Idle(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine)
	: BASE(pActor, pStatemachine)
{
	m_bInputBattleMode = true;
}

HRESULT CState_Aerith_Idle::Initialize_State(CState* pPreviousState)
{
	__super::Initialize_State(pPreviousState);

	m_pActor_ModelCom.lock()->Set_Animation(m_strCurBattleMode + "B_Idle01_1", 1.f, true);
	m_pActor_TransformCom.lock()->Set_Move_AnimationPosition(false);
	
	m_fAccTime = 0.f;

	STATE_AERITH::Reset_VirtualKey();
	return S_OK;
}

void CState_Aerith_Idle::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);

	m_fAccTime += fTimeDelta;
	if (m_fAccTime >= 5.f)
	{
		GET_SINGLE(CClient_Manager)->Set_BattleMode(false);
		GET_SINGLE(CCamera_Manager)->Set_ThirdCam_OffsetType(OFFSET_TYPE::TYPE_LONG);
	}
		
	if (STATE_AERITH::Action_Key_Input(m_pGameInstance, m_pStateMachineCom,m_bPlayable))
		return;

	if (m_bBattleModeChange)
	{
		m_pActor_ModelCom.lock()->Set_Animation(m_strCurBattleMode + "B_Idle01_1", 1.f, true);
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
		
		if (InRange(fAngle, 0.35f * PI, 0.65f * PI, "[)"))
		{
			bLShift ?  m_pActor_ModelCom.lock()->Set_Animation("Battle00|B_Run01_0_R90", 2.0f, false) : m_pActor_ModelCom.lock()->Set_Animation(m_strCurBattleMode + "B_Walk01_0_R90", 2.0f, false);
		}
		else if (InRange(fAngle, 1.35f * PI, 1.65f * PI, "[]"))
		{
			bLShift ? m_pActor_ModelCom.lock()->Set_Animation("Battle00|B_Run01_0_L90", 2.0f, false) : m_pActor_ModelCom.lock()->Set_Animation(m_strCurBattleMode + "B_Walk01_0_L90", 2.0f, false);
		}
		else if (InRange(fAngle, 0.75f * PI, 1.f * PI, "[)"))
		{
			bLShift ? m_pActor_ModelCom.lock()->Set_Animation("Battle00|B_Run01_0_R180", 2.0f, false) : m_pActor_ModelCom.lock()->Set_Animation(m_strCurBattleMode + "B_Walk01_0_R180", 2.0f, false);
		}
		else if (InRange(fAngle, 1.f * PI, 1.25f * PI, "[)"))
		{
			bLShift ? m_pActor_ModelCom.lock()->Set_Animation("Battle00|B_Run01_0_L180", 2.0f, false) : m_pActor_ModelCom.lock()->Set_Animation(m_strCurBattleMode + "B_Walk01_0_L180", 2.0f, false);
		}
		else
		{
			bLShift ? m_pActor_ModelCom.lock()->Set_Animation("Battle00|B_Run01_0", 1.3f, false) : m_pActor_ModelCom.lock()->Set_Animation(m_strCurBattleMode + "B_Walk01_0", 1.3f, false);
			m_bIsTurn = true;
		}
		
		bLShift ? m_pStateMachineCom.lock()->Wait_State<CState_Aerith_Run>() : m_pStateMachineCom.lock()->Wait_State<CState_Aerith_Walk>();
		Set_DirKey_Input_Enable(false);
	}

	if (m_bIsTurn)
		Turn_Dir(fTimeDelta * 1.5f);
}

void CState_Aerith_Idle::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CState_Aerith_Idle::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CState_Aerith_Idle::Transition_State(CState* pNextState)
{
	__super::Transition_State(pNextState);

	m_bIsTurn = false;
}

bool CState_Aerith_Idle::isValid_NextState(CState* state)
{
	if (m_pActor_ModelCom.lock()->IsCurrentAnimation(m_strCurBattleMode + "B_Idle01_1"))
		return true;

	if (m_pActor_ModelCom.lock()->IsAnimation_Finished())
		return true;
	else
		return false;

	return true;
}


void CState_Aerith_Idle::Free()
{
}
