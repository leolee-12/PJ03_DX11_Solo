#include "stdafx.h"
#include "Client_Manager.h"
#include "GameInstance.h"
#include "Cloud.h"
#include "Level_Tool.h"

#include "UI_Manager.h"
#include "UI_LockOnMarker.h"
#include "StatusComp.h"
#include "Camera_Manager.h"

#include "Light.h"

IMPLEMENT_SINGLETON(CClient_Manager)

CClient_Manager::CClient_Manager()
{
}

HRESULT CClient_Manager::Initialize_Client(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	m_pDevice = pDevice;
	m_pContext = pContext;
	Safe_AddRef(m_pGameInstance = GI());

	Register_ClientKey();

	return S_OK;
}

void CClient_Manager::Register_ClientKey()
{
	m_pGameInstance->Register_Key(DIK_UP);
	m_pGameInstance->Register_Key(DIK_DOWN);
	m_pGameInstance->Register_Key(DIK_LEFT);
	m_pGameInstance->Register_Key(DIK_RIGHT);
	m_pGameInstance->Register_Key(DIK_W);
	m_pGameInstance->Register_Key(DIK_A);
	m_pGameInstance->Register_Key(DIK_S);
	m_pGameInstance->Register_Key(DIK_D);
	m_pGameInstance->Register_Key(DIK_E); //UI-상호작용 키
	m_pGameInstance->Register_Key(DIK_UP);
	m_pGameInstance->Register_Key(DIK_LSHIFT);
	m_pGameInstance->Register_Key(DIK_F);
	m_pGameInstance->Register_Key(DIK_Z); //플레이어&UI-캐릭터 교체
	m_pGameInstance->Register_Key(DIK_RETURN);
	m_pGameInstance->Register_Key(DIK_ESCAPE);
	m_pGameInstance->Register_Key(DIK_SPACE);
	m_pGameInstance->Register_Key(DIK_B); //플레이어&UI-전투모드 및 비전투모드 설정
	m_pGameInstance->Register_Key(DIK_G); //플레이어&UI-LockOn 고정/해제 키
	m_pGameInstance->Register_Key(DIK_H);
	m_pGameInstance->Register_Key(DIK_J);
	m_pGameInstance->Register_Key(DIK_M); //UI- MainPopup Open 키
	m_pGameInstance->Register_Key(DIK_MINUS); //UI-체력 감소 테스트키
	m_pGameInstance->Register_Key(DIK_X);	//UI- 커서 이동
	m_pGameInstance->Register_Key(DIK_C); //UI- 커서 이동
	m_pGameInstance->Register_Key(DIK_Q);

	//For Debug Test
	m_pGameInstance->Register_Key(DIK_K);
	m_pGameInstance->Register_Key(DIK_L);
	m_pGameInstance->Register_Key(DIK_COMMA);
}

void CClient_Manager::Tick_Client()
{
	Update_TargetLockOn();

	if (m_bDirKey_Input_Enable)
		Input_DirKey();

	if (m_bKey_Input_Enable)
		Input_Key();


	if (m_pGameInstance->Key_Down(DIK_5) && m_pGameInstance->Get_CreateLevelIndex() != LEVEL_TOOL)
	{
		Switching_AttackMode();
	}

	if (!GET_SINGLE(CUI_Manager)->Get_WaitMode() && !GET_SINGLE(CCamera_Manager)->Get_CheckSwitching() && m_pGameInstance->Get_CreateLevelIndex() != LEVEL_TOOL) {
		if (m_bAttackMode && m_pGameInstance->Key_Down(DIK_Z))
		{
			Switching_Player();
		}
	}

	if ((m_pGameInstance->Get_CreateLevelIndex() != LEVEL_TOOL) &&
		(m_pGameInstance->Key_Down(DIK_COMMA)))
	{
		Set_Player_Full_Limit();
	}

#ifdef _DEBUG
	if (m_pGameInstance->Key_Down(DIK_BACKSPACE))
	{
		Add_PlayerGIL();
	}
#endif

	if (m_pGameInstance->Key_Down(DIK_Q))
	{
		if (!m_bBattleMode_Natural)
		{
			m_bisAssertMode = !m_bisAssertMode;
		}
	}

	if (m_pGameInstance->Key_Down(DIK_9))
		m_bBattleMode_Natural = !m_bBattleMode_Natural;
}

void CClient_Manager::Clear(_uint ILevelIndex)
{
}

void CClient_Manager::Register_Player(PLAYER_TYPE eType, weak_ptr<class CPlayer> pPlayer)
{
	m_pPlayer[eType] = pPlayer.lock();

	if (m_ePlayerType == eType)
		static_pointer_cast<CPlayer>(m_pPlayer[eType])->Set_State_AIMode(false);
	else
		static_pointer_cast<CPlayer>(m_pPlayer[eType])->Set_State_AIMode(true);
}

void CClient_Manager::Input_DirKey()
{
	if (m_pGameInstance->Check_KeyState(DIK_W, KEY_PRESS))
	{
		if (m_pGameInstance->Check_KeyState(DIK_A, KEY_PRESS))
		{
			m_DirKeyInfo.bDirKey_EventOn = true;
			m_DirKeyInfo.fRadianFromBaseDir = XMConvertToRadians(315.f);
			m_DirKeyInfo.iDirKey = UP | LEFT;
		}
		else if (m_pGameInstance->Check_KeyState(DIK_D, KEY_PRESS))
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
	else if (m_pGameInstance->Check_KeyState(DIK_S, KEY_PRESS))
	{
		if (m_pGameInstance->Check_KeyState(DIK_A, KEY_PRESS))
		{
			m_DirKeyInfo.bDirKey_EventOn = true;
			m_DirKeyInfo.fRadianFromBaseDir = XMConvertToRadians(225.f);
			m_DirKeyInfo.iDirKey = DOWN | LEFT;
		}
		else if (m_pGameInstance->Check_KeyState(DIK_D, KEY_PRESS))
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
	else if (m_pGameInstance->Check_KeyState(DIK_D, KEY_PRESS))
	{
		m_DirKeyInfo.bDirKey_EventOn = true;
		m_DirKeyInfo.fRadianFromBaseDir = XMConvertToRadians(90.f);
		m_DirKeyInfo.iDirKey = RIGHT;
	}
	else if (m_pGameInstance->Check_KeyState(DIK_A, KEY_PRESS))
	{
		m_DirKeyInfo.bDirKey_EventOn = true;
		m_DirKeyInfo.fRadianFromBaseDir = XMConvertToRadians(270.f);
		m_DirKeyInfo.iDirKey = LEFT;
	}
	else
		m_DirKeyInfo.bDirKey_EventOn = false;
}

void CClient_Manager::Input_Key()
{
}

TARGETINFO CClient_Manager::Find_TargetMonster(PLAYER_TYPE ePlayerType, _uint iCloseOrder)
{
	list<shared_ptr<CGameObject>>* pMonsterList = m_pGameInstance->Get_ObjectList(m_pGameInstance->Get_CreateLevelIndex(), L_MONSTER);
	map<_float, pair<_float3, weak_ptr<CGameObject>>> mapMonster;

	TARGETINFO TargetInfo = TARGETINFO();
	if (pMonsterList != nullptr && pMonsterList->size() != 0)
	{
		_float3 vPlayerPos = m_pPlayer[ePlayerType]->Get_TransformCom().lock()->Get_State(CTransform::STATE_POSITION);

		for (const auto& iter : *pMonsterList)
		{
			// ZeroHP 제외
			auto pStatusCom = DynPtrCast<IStatusInterface>(iter)->Get_StatusCom().lock();
			if (iter->IsState(OBJSTATE::Active) == false 
				|| (pStatusCom != nullptr && (pStatusCom->IsZeroHP())))
				continue;
			if (DynPtrCast<IStatusInterface>(iter)->Get_StatusCom().lock()->IsActionPowerEqual(20))
				continue;

			_float3 vTargetPos = iter->Get_TransformCom().lock()->Get_State(CTransform::STATE_POSITION);

			_float3 vPlayerToTarget = vTargetPos - vPlayerPos;
			_float vTargetControllerRadius = 0.f;
			if (iter->Get_PhysXControllerCom().lock())
				vTargetControllerRadius = iter->Get_PhysXControllerCom().lock()->Get_HalfWidth();
			_float fDistance = XMVectorGetX(XMVector3Length(vPlayerToTarget)) - vTargetControllerRadius;

			_bool bYDistance = ePlayerType == CLOUD ? fabs(vPlayerToTarget.y) <= 3.f : fabs(vPlayerToTarget.y) <= 6.f;
			if (fDistance <= m_fMaxDistanceToTarget && bYDistance)
				mapMonster.emplace(fDistance, make_pair(vPlayerToTarget, iter));
		}

		//Clamp(iCloseOrder, (_uint)0, (_uint)mapMonster.size() - 1);
		if (iCloseOrder < 0)
			iCloseOrder = (_uint)mapMonster.size() - 1;
		else if (iCloseOrder> (_uint)mapMonster.size() - 1)
			iCloseOrder = 0;

		_uint iCount = 0;

		for (const auto& iter : mapMonster)
		{
			if (iCount == iCloseOrder)
			{
				TargetInfo.pTarget = iter.second.second;
				TargetInfo.fDistance = iter.first;
				TargetInfo.vTargetPos = iter.second.second.lock()->Get_TransformCom().lock()->Get_State(CTransform::STATE_POSITION);
				TargetInfo.vDirToTarget = XMVector3Normalize(iter.second.first);
				m_iTargetMonsterIndex = iCount;
				if (m_ePlayerType == ePlayerType)
					m_TargetMonsterInfo = TargetInfo;
				return TargetInfo;
			}
			++iCount;
		}
	}

	if (m_ePlayerType == ePlayerType)
		m_TargetMonsterInfo = TargetInfo;
	return TargetInfo;
}

TARGETINFO CClient_Manager::Get_TargetMonster(PLAYER_TYPE ePlayerType)
{
	if (m_ePlayerType == ePlayerType)
	{
		if (!m_bLockOnEnable || !m_bisLockOnToMonster)
		{
			m_TargetMonsterInfo = TARGETINFO();
			return m_TargetMonsterInfo;
		}

		if ((m_TargetMonsterInfo.pTarget.lock() == nullptr))
		{
			Find_TargetMonster(ePlayerType);
			if (m_TargetMonsterInfo.pTarget.lock() != nullptr)
			{
				//m_bisLockOnToMonster = true;
				//GET_SINGLE(CUI_Manager)->TurnOn_LockOnMarker();
			}
		}
		return m_TargetMonsterInfo;
	}
	else
		return Find_TargetMonster(ePlayerType);

}

TARGETINFO CClient_Manager::Find_TargetPlayer(weak_ptr<CGameObject> pMonster, PLAYER_TYPE type)
{
	TARGETINFO TargetInfo;
	PLAYER_TYPE ePlayerType = type == PLAYER_END ? m_ePlayerType : type;

	if (m_pPlayer[ePlayerType] != nullptr)
	{
		_vector vMonsterPos = pMonster.lock()->Get_TransformCom().lock()->Get_State(CTransform::STATE_POSITION);
		_vector vTargetPos = m_pPlayer[ePlayerType]->Get_TransformCom().lock()->Get_State(CTransform::STATE_POSITION);

		_vector vMonsterToTarget = vTargetPos - vMonsterPos;
		_float fDistance = XMVectorGetX(XMVector3Length(vMonsterToTarget));

		TargetInfo.pTarget = m_pPlayer[ePlayerType];
		TargetInfo.fDistance = fDistance;
		TargetInfo.vTargetPos = vTargetPos;
		TargetInfo.vDirToTarget = XMVector3Normalize(vMonsterToTarget);

	}
	return TargetInfo;
}

TARGETINFO CClient_Manager::Update_TargetInfo(weak_ptr<CGameObject>pActor, TARGETINFO& TargetInfo)
{
	if (TargetInfo.pTarget.lock())
	{
		_float3 vTargetPos = TargetInfo.pTarget.lock()->Get_TransformCom().lock()->Get_State(CTransform::STATE_POSITION);
		_float3 vActorPos = pActor.lock()->Get_TransformCom().lock()->Get_State(CTransform::STATE_POSITION);
		_float3 vActorToTarget = vTargetPos - vActorPos;
		_float vTargetControllerRadius = 0.f;
		if (TargetInfo.pTarget.lock()->Get_PhysXControllerCom().lock())
			vTargetControllerRadius = TargetInfo.pTarget.lock()->Get_PhysXControllerCom().lock()->Get_HalfWidth();
		_float fDistance = XMVectorGetX(XMVector3Length(vActorToTarget)) - vTargetControllerRadius;

		TargetInfo.fDistance = fDistance;
		TargetInfo.vTargetPos = vTargetPos;
		TargetInfo.vDirToTarget = XMVector3Normalize(vActorToTarget);
	}
	return TargetInfo;
}

void CClient_Manager::Switching_Player()
{
	m_ePlayerType = m_ePlayerType == CLOUD ? AERITH : CLOUD;

	PLAYER_TYPE eAIPlayerType = m_ePlayerType == CLOUD ? AERITH : CLOUD;
	//m_bisSelectCloud = m_ePlayerType == CLOUD ? true : false;

	if (m_pPlayer[eAIPlayerType])
		static_pointer_cast<CPlayer>(m_pPlayer[eAIPlayerType])->Set_State_AIMode(true);
	if (m_pPlayer[m_ePlayerType])
		static_pointer_cast<CPlayer>(m_pPlayer[m_ePlayerType])->Set_State_AIMode(false);

	GET_SINGLE(CCamera_Manager)->Switching_ThirdCamera(eAIPlayerType, m_ePlayerType);

}

void CClient_Manager::Switching_AttackMode()
{
	m_bAttackMode = !m_bAttackMode;

	if (m_bAttackMode)
	{
		static_pointer_cast<CPlayer>(m_pPlayer[AERITH])->Set_State_AIMode(true);
	}
	else
	{
		if (m_ePlayerType == AERITH)
			Switching_Player();
		else
			static_pointer_cast<CPlayer>(m_pPlayer[AERITH])->Set_State_AIMode(true);
	}

}

void CClient_Manager::Set_PlayerPos(_float3 vCloudPos, _float3 vAerithPos)
{
	if (m_pPlayer[CLOUD] != nullptr)
	{
		m_pPlayer[CLOUD]->Get_TransformCom().lock()->Set_Position(1.f, vCloudPos);
	}

	if (m_pPlayer[AERITH] != nullptr)
	{
		m_pPlayer[AERITH]->Get_TransformCom().lock()->Set_Position(1.f, vAerithPos);
	}
}

void CClient_Manager::Register_Event_AdjustTimeDelta(weak_ptr<CBase> _pSubscriber, _float fDuration, _float fAdjustTimeDelta)
{
	if (CGameInstance::m_fAdjustTimeDelta != 1.f)
		return;

	m_fAdjustTimeDelta_Duration = fDuration;
	m_fAdjustTimeDelta = fAdjustTimeDelta;

	m_pGameInstance->Add_TickEvent(_pSubscriber.lock(), [&, this](_float fTimeDelta)->_bool
		{
			m_fAdjustTimeDelta_AccTime += fTimeDelta;

			if (m_fAdjustTimeDelta_AccTime < m_fAdjustTimeDelta_Duration)
			{
				CGameInstance::m_fAdjustTimeDelta = m_fAdjustTimeDelta;
				return true;
			}
			else
			{
				m_fAdjustTimeDelta_AccTime = 0.f;
				m_fAdjustTimeDelta_Duration = 0.f;
				CGameInstance::m_fAdjustTimeDelta = 1.f;
				return false;
			}
		});
}

void CClient_Manager::Set_AdjustTimeDelta(_float fAdjustTimeDelta)
{
	CGameInstance::m_fAdjustTimeDelta = fAdjustTimeDelta;
}


void CClient_Manager::Update_TargetLockOn()
{
	if (m_bisLockOnToMonster)
	{
		if (GET_SINGLE(CUI_Manager)->Is_LockOnMarker_StateOn())
			Find_TargetMonster(m_ePlayerType);
		else if (m_pGameInstance->Key_DownEx(DIK_TAB, DIK_MD_NONE))
			Find_TargetMonster(m_ePlayerType,++m_iTargetMonsterIndex);
		else if (m_pGameInstance->Key_DownEx(DIK_TAB, DIK_MD_LSHIFT))
			Find_TargetMonster(m_ePlayerType,--m_iTargetMonsterIndex);
		else if (m_TargetMonsterInfo.pTarget.lock() != nullptr)
		{
			//타겟 정보 업데이트
			_float3 vPlayerPos = m_pPlayer[m_ePlayerType]->Get_TransformCom().lock()->Get_State(CTransform::STATE_POSITION);

			m_TargetMonsterInfo.vTargetPos = m_TargetMonsterInfo.pTarget.lock()->Get_TransformCom().lock()->Get_State(CTransform::STATE_POSITION);
			m_TargetMonsterInfo.fDistance = XMVectorGetX(XMVector3Length(m_TargetMonsterInfo.vTargetPos - vPlayerPos));
			m_TargetMonsterInfo.vDirToTarget = XMVector3Normalize(m_TargetMonsterInfo.vTargetPos - vPlayerPos);

		}

		shared_ptr<CGameObject> pTarget = m_TargetMonsterInfo.pTarget.lock();
		if (pTarget != nullptr)
		{
			if ((pTarget->IsState(OBJSTATE::DeadAnim)|| m_TargetMonsterInfo.fDistance >= m_fMaxDistanceToTarget))
			{
				shared_ptr<CStatusComp> pStatusCom = DynPtrCast<IStatusInterface>(pTarget)->Get_StatusCom().lock();
				if (pStatusCom != nullptr && (pStatusCom->IsZeroHP()))
				{
					m_bisLockOnToMonster = false;
					m_TargetMonsterInfo = TARGETINFO();
					GET_SINGLE(CUI_Manager)->TurnOff_LockOnMarker();
					return;
				}
			}

			_float3 vLockOnPos = m_TargetMonsterInfo.vTargetPos;
			vLockOnPos.y += pTarget->Get_PhysXColliderLocalOffset().y + 0.2f;
			GET_SINGLE(CUI_Manager)->Set_LockOnMarker_Pos(vLockOnPos);
		}
		else
		{
			GET_SINGLE(CUI_Manager)->TurnOff_LockOnMarker();
		}
	}
}

void CClient_Manager::Add_Player_In_Layer(LEVEL eLevel,_float3 vCloudPos, _float3 vAerithPos)
{
	m_pGameInstance->Add_Object(eLevel, L_PLAYER, dynamic_pointer_cast<CGameObject>(m_pPlayer[CLOUD]));
	m_pGameInstance->Add_Object(eLevel, L_PLAYER, dynamic_pointer_cast<CGameObject>(m_pPlayer[AERITH]));

	Set_PlayerPos(vCloudPos,vAerithPos);
}

void CClient_Manager::TurnOn_MotionBlur(_bool bUseActorCenter, _float vBlurScale, _float2 vCenter, _float fBlurAmount)
{
	m_pGameInstance->Get_Renderer()->Set_MotionBlur(true);
	m_pGameInstance->Get_Renderer()->Set_MotionBlurScale(vBlurScale);
	m_pGameInstance->Get_Renderer()->Set_MotionBlurAmount(fBlurAmount);
	if (bUseActorCenter)
	{
		_float4 vActorCenter = m_pPlayer[m_ePlayerType]->Get_TransformCom().lock()->Get_State(CTransform::STATE_POSITION) + float3(0.f, 0.7f, 0.f);
		vActorCenter = XMVector3TransformCoord(vActorCenter, m_pGameInstance->Get_TransformMatrix(CPipeLine::TS_VIEW));
		vActorCenter = XMVector3TransformCoord(vActorCenter, m_pGameInstance->Get_TransformMatrix(CPipeLine::TS_PROJ));
		vActorCenter = (vActorCenter + _float2(1.f, 1.f)) / 2.f;

		m_pGameInstance->Get_Renderer()->Set_MotionBlurCenter(_float2(vActorCenter.x, vActorCenter.y));
	}
	else
		m_pGameInstance->Get_Renderer()->Set_MotionBlurCenter(vCenter);
}

void CClient_Manager::TurnOff_MotionBlur()
{
	if (GET_SINGLE(CUI_Manager)->Get_WaitMode())
	{
		TurnOn_MotionBlur(true, 0.03f);
	}
	else
	{
		m_pGameInstance->Get_Renderer()->Set_MotionBlur(false);
		m_pGameInstance->Get_Renderer()->Set_MotionBlurCenter(_float2(0.5f, 0.5f));
		m_pGameInstance->Get_Renderer()->Set_MotionBlurScale(0.125f);
		m_pGameInstance->Get_Renderer()->Set_MotionBlurAmount(64.f);
	}
}

void CClient_Manager::Register_Event_MotionBlur(weak_ptr<CBase> _pSubscriber, _float fDuration, _bool bUseActorCenter, _float vBlurScale, _float2 vCenter)
{
	m_fMotionBlur_Duration = fDuration;
	m_fMotionBlur_AccTime = 0.f;
	TurnOn_MotionBlur(bUseActorCenter, vBlurScale, vCenter);

	m_pGameInstance->Add_TickEvent(_pSubscriber.lock(), [&, this](_float fTimeDelta)->_bool
		{
			m_fMotionBlur_AccTime += fTimeDelta;

			if (m_fMotionBlur_AccTime < m_fMotionBlur_Duration)
			{
				return true;
			}
			else
			{
				TurnOff_MotionBlur();
				return false;
			}
		});
}

void CClient_Manager::TurnOn_RadialBlur(_float3 vCenter, _float Intensity)
{
	m_pGameInstance->Get_Renderer()->Set_RadialBlur(true);
	m_pGameInstance->Get_Renderer()->Set_RadialBlurWorldPos(vCenter);
	m_pGameInstance->Get_Renderer()->Set_RadialBlurIntensity(Intensity);
}

void CClient_Manager::TurnOff_RadialBlur()
{
	m_pGameInstance->Get_Renderer()->Set_RadialBlur(false);
	m_pGameInstance->Get_Renderer()->Set_RadialBlurWorldPos(_float3(0.f, 0.f, 0.f));
	m_pGameInstance->Get_Renderer()->Set_RadialBlurIntensity(0.f);
}

void CClient_Manager::Play_Game()
{
	CGameInstance::m_bPauseGame = false;
}

void CClient_Manager::Pause_Game()
{
	CGameInstance::m_bPauseGame = true;
}

void CClient_Manager::Switch_Game()
{
	CGameInstance::m_bPauseGame = !CGameInstance::m_bPauseGame;
}

void CClient_Manager::Set_Player_Full_Limit()
{
	m_pPlayer[m_ePlayerType]->Get_StatusCom().lock()->Add_LimitBreak(100.f);
}

void CClient_Manager::Free()
{
	Safe_Release(m_pGameInstance);
}


