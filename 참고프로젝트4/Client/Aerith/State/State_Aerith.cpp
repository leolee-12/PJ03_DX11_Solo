#include "stdafx.h"
#include "Aerith/State/State_Aerith.h"
#include "GameInstance.h"
#include "Player.h"

#include "PartObject.h"

CState_Aerith::CState_Aerith(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine)
	: BASE(pActor, pStatemachine)
{
}

HRESULT CState_Aerith::Initialize_State(CState* pPreviousState)
{
	__super::Initialize_State(pPreviousState);
	Update_BattleModeName();

	m_pPartObject = m_pActor.lock()->Find_PartObject(TEXT("Part_Weapon"));

	return S_OK;
}

void CState_Aerith::Priority_Tick(_cref_time fTimeDelta) 
{
	__super::Priority_Tick(fTimeDelta);
	Update_BattleModeName();
}

void CState_Aerith::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CState_Aerith::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

_matrix CState_Aerith::Get_PartObjectBoneMatrix(const wstring& strBoneName)
{
	if (m_pPartObject.expired())
		return XMMatrixIdentity();
	
	return m_pPartObject.lock()->Get_WorldMatrixWithParent();
}

void CState_Aerith::Update_BattleModeName()
{
	if (m_bInputBattleMode && m_pGameInstance->Key_Down(DIK_Q))
	{
		if (m_strCurBattleMode == m_strBattleMode[0])
		{
			GET_SINGLE(CClient_Manager)->Set_AssertMode(false);
			m_strCurBattleMode = m_strBattleMode[1];
			m_eCurBattleMode = BATTLE_MODE::BRAVE;
		}
		else
		{
			GET_SINGLE(CClient_Manager)->Set_AssertMode(true);
			m_strCurBattleMode = m_strBattleMode[0];
			m_eCurBattleMode = BATTLE_MODE::ASSERT;
		}
		m_bBattleModeChange = true;
	} 
	else
	{
		if (GET_SINGLE(CClient_Manager)->Get_AssertMode())
		{
			m_strCurBattleMode = m_strBattleMode[0];
			m_eCurBattleMode = BATTLE_MODE::ASSERT;
		}
		else
		{
			m_strCurBattleMode = m_strBattleMode[1];
			m_eCurBattleMode = BATTLE_MODE::BRAVE;
		}
		m_bBattleModeChange = false;
	}

}

void CState_Aerith::Transition_State(CState* pNextState)
{
	__super::Transition_State(pNextState);

	if (!GET_SINGLE(CClient_Manager)->IsPlayable(AERITH))
	{
		string strStatename = string(typeid(*pNextState).name());
		if (strStatename == string("class Client::CState_Aerith_Idle"))
			static_pointer_cast<CPlayer>(m_pActor.lock())->Set_State_AIMode(true);
	}
	m_bNextAttack = false;
}

bool CState_Aerith::isValid_NextState(CState* state)
{
	return true;
}

void CState_Aerith::Free()
{
}
