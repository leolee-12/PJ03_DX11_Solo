#include "stdafx.h"
#include "Aerith/State/State_List_Aerith.h"

CState_Aerith_BraveAttack04::CState_Aerith_BraveAttack04(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine)
	: BASE(pActor, pStatemachine)
{
}

HRESULT CState_Aerith_BraveAttack04::Initialize_State(CState* pPreviousState)
{
	__super::Initialize_State(pPreviousState);

	if (m_TargetInfo.pTarget.lock() == nullptr || m_TargetInfo.pTarget.expired())
		m_TargetInfo = GET_SINGLE(CClient_Manager)->Get_TargetMonster();

	if (m_TargetInfo.pTarget.lock() != nullptr)
		m_pActor_TransformCom.lock()->Look_At_OnLand(m_TargetInfo.vTargetPos);

	m_pActor_ModelCom.lock()->Set_Animation("Battle01|B_SAttack04_0  ", 1.15f, false, true, 2.f);


	return S_OK;
}

void CState_Aerith_BraveAttack04::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);

	TurnOnOff_Weapon(25.f, 30.f);

	if (m_pActor_ModelCom.lock()->IsAnimation_Finished())
		m_pStateMachineCom.lock()->Enter_State<CState_Aerith_Idle>();
}

void CState_Aerith_BraveAttack04::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CState_Aerith_BraveAttack04::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CState_Aerith_BraveAttack04::Transition_State(CState* pNextState)
{
	__super::Transition_State(pNextState);

	static_cast<CState_Player*>(pNextState)->Set_TargetInfo(m_TargetInfo);

	TurnOff_Weapon();
}

bool CState_Aerith_BraveAttack04::isValid_NextState(CState* state)
{
	return true;
}

void CState_Aerith_BraveAttack04::Free()
{
}
