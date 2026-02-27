#include "stdafx.h"
#include "State_Cloud.h"
#include "GameInstance.h"
#include "Player.h"
#include "Cloud_Weapon.h"

CState_Cloud::CState_Cloud(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine)
	:CState_Player(pActor, pStatemachine)
{
}

HRESULT CState_Cloud::Initialize_State(CState* pPreviousState)
{
	__super::Initialize_State(pPreviousState);
	Update_BattleModeName();

	return S_OK;
}

void CState_Cloud::Priority_Tick(_cref_time fTimeDelta) 
{
	__super::Priority_Tick(fTimeDelta);
	Update_BattleModeName();
}

void CState_Cloud::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CState_Cloud::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CState_Cloud::Update_BattleModeName()
{
	if (m_bPlayable)
	{
		{
			if (GET_SINGLE(CClient_Manager)->Get_BattleMode_Natural())
			{
				if (m_eCurBattleMode != BATTLE_MODE::NETURAL)
				{
					static_pointer_cast<CCloud_Weapon>(m_pActor.lock()->Find_PartObject(L"Part_Weapon"))->Set_SocketBoneType(CCloud_Weapon::BACK);
					m_eCurBattleMode = BATTLE_MODE::NETURAL;
					m_bBattleModeChange = true;
				}
				else
					m_bBattleModeChange = false;

			}
			else if (m_eCurBattleMode == BATTLE_MODE::NETURAL)
			{
				m_eCurBattleMode = BATTLE_MODE::ASSERT;
				m_bBattleModeChange = true;
				static_pointer_cast<CCloud_Weapon>(m_pActor.lock()->Find_PartObject(L"Part_Weapon"))->Set_SocketBoneType(CCloud_Weapon::HAND);
			}
			else
				m_bBattleModeChange = false;
		}

		if (m_eCurBattleMode != BATTLE_MODE::NETURAL && m_bInputBattleMode)
		{
			BATTLE_MODE eChangeMode = BATTLE_MODE::ASSERT;
			if (GET_SINGLE(CClient_Manager)->Get_AssertMode())
			{
				eChangeMode = BATTLE_MODE::ASSERT;
			}
			else
			{
				eChangeMode = BATTLE_MODE::BRAVE;
			}

			if(eChangeMode != m_eCurBattleMode)
				m_bBattleModeChange = true;
			else
				m_bBattleModeChange = false;

			m_eCurBattleMode = eChangeMode;
			
		} 
		

		switch (m_eCurBattleMode)
		{
		case BATTLE_MODE::ASSERT:
			m_strCurBattleMode = m_strBattleMode[0];
			break;
		case BATTLE_MODE::BRAVE:
			m_strCurBattleMode = m_strBattleMode[1];
			break;
		case BATTLE_MODE::NETURAL:
			m_strCurBattleMode = m_strBattleMode[2];
			break;
		default:
			break;
		}


	}

}

void CState_Cloud::Transition_State(CState* pNextState)
{
	__super::Transition_State(pNextState);

	if (!GET_SINGLE(CClient_Manager)->IsPlayable(CLOUD))
	{
		string strStatename = string(typeid(*pNextState).name());
		if ( strStatename == string("class Client::CState_Cloud_Idle"))
			static_pointer_cast<CPlayer>(m_pActor.lock())->Set_State_AIMode(true);
	}
	m_bNextAttack = false;

	 DynCast<CState_Cloud*>(pNextState)->Set_CurrentBattleModeName(m_strCurBattleMode);
	 DynCast<CState_Cloud*>(pNextState)->Set_CurrentBattleMode(m_eCurBattleMode);
}

bool CState_Cloud::isValid_NextState(CState* state)
{
	return true;
}

void CState_Cloud::Free()
{
}
