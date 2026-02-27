#include "stdafx.h"
#include "Aerith/State/State_list_Aerith.h"

CState_Aerith_Guard::CState_Aerith_Guard(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine)
	: BASE(pActor, pStatemachine)
{
}

HRESULT CState_Aerith_Guard::Initialize_State(CState* pPreviousState)
{
	__super::Initialize_State(pPreviousState);

	m_pActor_ModelCom.lock()->Set_Animation("Battle00|B_Guard01_0", 1.3f, false, true, 2.f);

	return S_OK;
}

void CState_Aerith_Guard::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
	
	if (m_pActor_ModelCom.lock()->IsAnimation_Finished("Battle00|B_Guard01_0"))
		m_pActor_ModelCom.lock()->Set_Animation("Battle00|B_Guard01_1", 1.3f, true, true, 2.f);

	// °¡µå
	if (m_pGameInstance->Key_Down(DIK_T))
		m_pActor_ModelCom.lock()->Set_Animation("Battle00|B_Guard01_2", 1.3f, false, true, 2.f);

	if (m_pActor_ModelCom.lock()->IsAnimation_Finished("Battle00|B_Guard01_2"))
		m_pStateMachineCom.lock()->Enter_State<CState_Aerith_Idle>();
}

void CState_Aerith_Guard::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CState_Aerith_Guard::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CState_Aerith_Guard::Transition_State(CState* pNextState)
{
	__super::Transition_State(pNextState);
}

bool CState_Aerith_Guard::isValid_NextState(CState* state)
{
	return true;
}

void CState_Aerith_Guard::Free()
{
}
