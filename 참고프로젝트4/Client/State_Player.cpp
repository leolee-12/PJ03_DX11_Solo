#include "stdafx.h"
#include "State_Player.h"
#include "GameInstance.h"
#include "Camera_Manager.h"
#include "Client_Manager.h"
#include "UI_Command.h"
#include "Player.h"

CState_Player::CState_Player(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine)
	:CState(pActor, pStatemachine)
{
	CCharacter* pChr = DynCast<CCharacter*>(pActor.get());
	if (pChr == nullptr)
		assert(false);

	m_pStatusCom = pChr->Get_StatusCom();
}

HRESULT CState_Player::Initialize_State(CState* pPreviousState)
{
	__super::Initialize_State(pPreviousState);
		
	return S_OK;
}

void CState_Player::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
		
	m_bPlayable = GET_SINGLE(CClient_Manager)->IsPlayable(static_pointer_cast<CPlayer>(m_pActor.lock())->Get_PlayerType());

	if(m_bPlayable)
		m_DirKeyInfo = GET_SINGLE(CClient_Manager)->Get_DirKeyInfo();
	if (GET_SINGLE(CCamera_Manager)->Get_CamType() != CCamera_Manager::CAMERATYPE::CAM_THIRD)
		m_DirKeyInfo.bDirKey_EventOn = false;
}

void CState_Player::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CState_Player::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CState_Player::Transition_State(CState* pNextState)
{
	__super::Transition_State(pNextState);

	m_pActor_TransformCom.lock()->Set_Move_AnimationPosition(true);

	if (m_bPlayable)
		GET_SINGLE(CClient_Manager)->Set_DirKey_Input_Enable(true);
	static_cast<CState_Player*>(pNextState)->Set_DirKeyInfo(m_DirKeyInfo);
	static_cast<CState_Player*>(pNextState)->Set_TargetInfo(m_TargetInfo);
}

bool CState_Player::isValid_NextState(CState* state)
{
	return true;
}

_bool CState_Player::Get_DirKey_Input_Enable()
{
	return m_bPlayable ?  GET_SINGLE(CClient_Manager)->Get_DirKey_Input_Enable() : false;
}

void CState_Player::Set_DirKey_Input_Enable(_bool bEnable)
{
	if(m_bPlayable)	
		GET_SINGLE(CClient_Manager)->Set_DirKey_Input_Enable(bEnable);
}

_float CState_Player::DirToRadian(_int iDir)
{
	if (iDir & ECast(UP))
	{
		if (iDir & ECast(LEFT))
		{
			return XMConvertToRadians(315.f);
		}
		else if (iDir & ECast(RIGHT))
		{
			m_DirKeyInfo.bDirKey_EventOn = true;
			m_DirKeyInfo.fRadianFromBaseDir = XMConvertToRadians(45.f);
			m_DirKeyInfo.iDirKey = UP | RIGHT;
		}
		else
		{
			m_DirKeyInfo.bDirKey_EventOn = true;
			m_DirKeyInfo.fRadianFromBaseDir = XMConvertToRadians(0.f);
			m_DirKeyInfo.iDirKey = UP;
		}
	}
	else if (iDir & ECast(DOWN))
	{
		if (iDir & ECast(LEFT))
		{
			m_DirKeyInfo.bDirKey_EventOn = true;
			m_DirKeyInfo.fRadianFromBaseDir = XMConvertToRadians(225.f);
			m_DirKeyInfo.iDirKey = DOWN | LEFT;
		}
		else if (iDir & ECast(RIGHT))
		{
			m_DirKeyInfo.bDirKey_EventOn = true;
			m_DirKeyInfo.fRadianFromBaseDir = XMConvertToRadians(135.f);
			m_DirKeyInfo.iDirKey = DOWN | RIGHT;
		}
		else
		{
			m_DirKeyInfo.bDirKey_EventOn = true;
			m_DirKeyInfo.fRadianFromBaseDir = XMConvertToRadians(180.f);
			m_DirKeyInfo.iDirKey = DOWN;
		}
	}
	else if (iDir & ECast(RIGHT))
	{
		m_DirKeyInfo.bDirKey_EventOn = true;
		m_DirKeyInfo.fRadianFromBaseDir = XMConvertToRadians(90.f);
		m_DirKeyInfo.iDirKey = RIGHT;
	}
	else if (iDir & ECast(LEFT))
	{
		m_DirKeyInfo.bDirKey_EventOn = true;
		m_DirKeyInfo.fRadianFromBaseDir = XMConvertToRadians(270.f);
		m_DirKeyInfo.iDirKey = LEFT;
	}	

	return m_DirKeyInfo.fRadianFromBaseDir;
}

void CState_Player::Turn_Dir()
{
	//if (GET_SINGLE(CCamera_Manager)->Get_CamType() == CCamera_Manager::CAMERATYPE::CAM_THIRD)
	{
		m_pActor_TransformCom.lock()->Rotate_On_BaseDir(m_DirKeyInfo.fRadianFromBaseDir);
	}	
	return;
}

_bool CState_Player::Turn_Dir(_cref_time fTimeDelta)
{
	//if (GET_SINGLE(CCamera_Manager)->Get_CamType() == CCamera_Manager::CAMERATYPE::CAM_THIRD)
	{
		return m_pActor_TransformCom.lock()->Rotate_On_BaseDir(fTimeDelta, m_DirKeyInfo.fRadianFromBaseDir);
	}	
	return false;
}

void CState_Player::TurnOn_Weapon()
{
	static_pointer_cast<CCharacter>(m_pActor.lock())->TurnOn_Weapon();
}

void CState_Player::TurnOff_Weapon()
{
	static_pointer_cast<CCharacter>(m_pActor.lock())->TurnOff_Weapon();
}

void CState_Player::TurnOnOff_Weapon(_float fFrameMin, _float fFrameMax, _bool bEffect)
{
	if (m_pActor_ModelCom.lock()->IsAnimation_Frame_Once(fFrameMin))
	{
		TurnOn_Weapon();
		if (bEffect)
			TurnOn_WeaponEffect();
	}
		
	else if (m_pActor_ModelCom.lock()->IsAnimation_Frame_Once(fFrameMax))
	{
		TurnOff_Weapon();
		if (bEffect)
			TurnOff_WeaponEffect();
	}
}

void CState_Player::TurnOn_WeaponEffect()
{
	static_pointer_cast<CCharacter>(m_pActor.lock())->TurnOn_PartEffect();
}

void CState_Player::TurnOff_WeaponEffect()
{
	static_pointer_cast<CCharacter>(m_pActor.lock())->TurnOff_PartEffect();
}


void CState_Player::TurnOn_MotionBlur(_float fFrame, _bool bUseActorCenter,_float vBlurScale, _float2 vCenter)
{
	PLAYER_TYPE eType = static_pointer_cast<CPlayer>(m_pActor.lock())->Get_PlayerType();
	if (!GET_SINGLE(CClient_Manager)->IsPlayable(eType))
		return;

	if (fFrame == -1.f || m_pActor_ModelCom.lock()->IsAnimation_Frame_Once(fFrame))
	{
		m_pGameInstance->Get_Renderer()->Set_MotionBlur(true);
		m_pGameInstance->Get_Renderer()->Set_MotionBlurScale(vBlurScale);
		if (bUseActorCenter)
		{
			_float4 vActorCenter = m_pActor_TransformCom.lock()->Get_State(CTransform::STATE_POSITION) + float3(0.f,0.7f,0.f);
			vActorCenter = XMVector3TransformCoord(vActorCenter, m_pGameInstance->Get_TransformMatrix(CPipeLine::TS_VIEW));
			vActorCenter = XMVector3TransformCoord(vActorCenter, m_pGameInstance->Get_TransformMatrix(CPipeLine::TS_PROJ));
			vActorCenter = (vActorCenter + _float2(1.f, 1.f)) / 2.f;

			m_pGameInstance->Get_Renderer()->Set_MotionBlurCenter(_float2(vActorCenter.x, vActorCenter.y));
		}
		else
			m_pGameInstance->Get_Renderer()->Set_MotionBlurCenter(vCenter);
	}
	
}

void CState_Player::TurnOff_MotionBlur(_float fFrame)
{
	PLAYER_TYPE eType = static_pointer_cast<CPlayer>(m_pActor.lock())->Get_PlayerType();
	if (!GET_SINGLE(CClient_Manager)->IsPlayable(eType))
		return;

	if (fFrame == -1.f || m_pActor_ModelCom.lock()->IsAnimation_Frame_Once(fFrame))
	{
		m_pGameInstance->Get_Renderer()->Set_MotionBlur(false);
		m_pGameInstance->Get_Renderer()->Set_MotionBlurCenter(_float2(0.5f, 0.5f));
		m_pGameInstance->Get_Renderer()->Set_MotionBlurScale(0.125f);
	}
}

void CState_Player::Check_UseCommand()
{
	CUI_Command::COMMAND_RESULT eCommand = (CUI_Command::COMMAND_RESULT)GET_SINGLE(CClient_Manager)->Get_Command_Result();
	_bool bUseCommand = (eCommand != CUI_Command::COMMAND_RESULT::CMDR_NONE && eCommand != CUI_Command::COMMAND_RESULT::C_CMDR_A_ATTACK && eCommand != CUI_Command::COMMAND_RESULT::C_CMDR_B_ATTACK);

	if(bUseCommand)
		GET_SINGLE(CClient_Manager)->Set_Command_Result(CUI_Command::COMMAND_RESULT::CMDR_NONE);
}

void CState_Player::Free()
{
}
