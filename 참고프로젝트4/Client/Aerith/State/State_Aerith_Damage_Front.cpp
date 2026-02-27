#include "stdafx.h"
#include "Aerith/State/State_list_Aerith.h"

CState_Aerith_Damage_Front::CState_Aerith_Damage_Front(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine)
	: BASE(pActor, pStatemachine)
{
}

HRESULT CState_Aerith_Damage_Front::Initialize_State(CState* pPreviousState)
{
	__super::Initialize_State(pPreviousState);


	weak_ptr< CAnimationComponent> pAnimationComp;

	switch (m_iDamageType)
	{
	case 5:
		pAnimationComp = m_pActor_ModelCom.lock()->AnimationComp();
		pAnimationComp.lock()->Set_ADD_Animation(0, L"Battle00|B_Dmg01_Add", L"C_Spine_a", { L"C_Spine_c" }
		, 4, 1.f, 1.f, false);
	case 0:
		if (typeid(*pPreviousState) != typeid(CState_Aerith_Damage_Back) && pPreviousState != this)
			m_pStateMachineCom.lock()->Set_CurState(pPreviousState);
		break;
	case 10:
		m_pActor_ModelCom.lock()->Set_Animation("Battle00|B_DmgF01", 1.3f, false, true, 2.f);
		break;
	case 15:
		m_pActor_ModelCom.lock()->Set_Animation("Battle00|B_DmgF02", 1.3f, false, true, 2.f);
		break;
	case 20:
		m_pActor_ModelCom.lock()->Set_Animation("Battle00|B_DmgF03_0", 1.3f, false, true, 2.f);
		break;

	default:
		break;
	}
	return S_OK;
}

void CState_Aerith_Damage_Front::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
	
	switch (m_iDamageType)
	{
	case 0:
	case 5:
		m_pStateMachineCom.lock()->Enter_State<CState_Aerith_Idle>();
		break;
	case 10:
	case 15:
		if (m_pActor_ModelCom.lock()->IsAnimation_Finished())
			m_pStateMachineCom.lock()->Enter_State<CState_Aerith_Idle>();
		break;
	case 20:
		if (m_pActor_ModelCom.lock()->IsAnimation_Finished("Battle00|B_DmgF03_0"))
			m_pActor_ModelCom.lock()->Set_Animation("Battle00|B_DmgF03_1", 1.3f, false, true, 2.f, false, false);
		else if (m_pActor_ModelCom.lock()->IsAnimation_Finished("Battle00|B_DmgF03_1"))
			m_pActor_ModelCom.lock()->Set_Animation("Battle00|B_DmgF03_2", 1.3f, false, true, 2.f, false, false);
		else if (m_pActor_ModelCom.lock()->IsAnimation_Finished("Battle00|B_DmgF03_2"))
			m_pStateMachineCom.lock()->Enter_State<CState_Aerith_Idle>();
		break;
	default:
		break;
	}

}

void CState_Aerith_Damage_Front::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CState_Aerith_Damage_Front::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CState_Aerith_Damage_Front::Transition_State(CState* pNextState)
{
	__super::Transition_State(pNextState);
}

bool CState_Aerith_Damage_Front::isValid_NextState(CState* state)
{
	return true;
}

void CState_Aerith_Damage_Front::Free()
{
}
