#include "stdafx.h"
#include "State_list_Cloud.h"
#include "AnimMapObject.h"
#include "Data_Manager.h"

CState_Cloud_LeverDown::CState_Cloud_LeverDown(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine)
	:CState_Cloud(pActor, pStatemachine)
{
}

HRESULT CState_Cloud_LeverDown::Initialize_State(CState* pPreviousState)
{
	__super::Initialize_State(pPreviousState);

	m_pActor_ModelCom.lock()->Set_Animation("Common|CMN_PC0000_00_LeverD01_0", 1.3f, false, true, 2.f);

	return S_OK;
}

void CState_Cloud_LeverDown::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);

	if (m_pActor_ModelCom.lock()->IsAnimation_Finished("Common|CMN_PC0000_00_LeverD01_0"))
	{
		auto pObject = m_pGameInstance->Get_Object(m_pGameInstance->Get_CreateLevelIndex(), L_ANIMOBJECT, 2);
		pObject->Get_ModelCom().lock()->Set_Animation("Lever|N_Down_1", 1.f, false);
		dynamic_pointer_cast<CAnimMapObject>(pObject)->Set_IsAnimPlayLever(true);

		m_pActor_ModelCom.lock()->Set_Animation("Common|CMN_PC0000_00_LeverD01_2", 1.3f, false, true, 2.f);
	}

	if (m_pActor_ModelCom.lock()->IsAnimation_Finished("Common|CMN_PC0000_00_LeverD01_2"))
		m_pStateMachineCom.lock()->Enter_State<CState_Cloud_Idle>();
}

void CState_Cloud_LeverDown::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);

}

void CState_Cloud_LeverDown::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CState_Cloud_LeverDown::Transition_State(CState* pNextState)
{
	__super::Transition_State(pNextState);
}

bool CState_Cloud_LeverDown::isValid_NextState(CState* state)
{
	return true;
}

void CState_Cloud_LeverDown::Free()
{
}
