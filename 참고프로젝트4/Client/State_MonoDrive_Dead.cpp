#include "stdafx.h"
#include "State_List_MonoDrive.h"

#include "UI_Manager.h"

CState_MonoDrive_Dead::CState_MonoDrive_Dead(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine)
	:CState_MonoDrive(pActor, pStatemachine)
{
}

HRESULT CState_MonoDrive_Dead::Initialize_State(CState* pPreviousState)
{
	__super::Initialize_State(pPreviousState);

	m_pActor_ModelCom.lock()->Set_Animation("Main|B_Die01_0", 1.f, false);
	m_pActor_TransformCom.lock()->Set_Move_AnimationPosition(false);

	m_pActor.lock()->TurnOn_State(OBJSTATE::DeadAnim);

	GET_SINGLE(CUI_Manager)->Make_EventLogPopUp(0, 0, "MonoDrive");
	GET_SINGLE(CUI_Manager)->Make_EventLogPopUp(1, 7);
	GET_SINGLE(CUI_Manager)->Make_EventLogPopUp(2, 11);

	return S_OK;
}

void CState_MonoDrive_Dead::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);

	if (m_pActor_ModelCom.lock()->IsAnimation_Finished())
	{
		dynamic_pointer_cast<CCharacter>(m_pActor.lock())->Dead_DissolveType(L"EM_MonsterDead_Num2500");
	}
}

void CState_MonoDrive_Dead::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CState_MonoDrive_Dead::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CState_MonoDrive_Dead::Transition_State(CState* pNextState)
{
	__super::Transition_State(pNextState);
}

bool CState_MonoDrive_Dead::isValid_NextState(CState* state)
{
	return true;
}

void CState_MonoDrive_Dead::Free()
{
}
