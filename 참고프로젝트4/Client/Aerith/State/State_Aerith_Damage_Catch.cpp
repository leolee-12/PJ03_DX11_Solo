#include "stdafx.h"
#include "Aerith/State/State_List_Aerith.h"

#include "Sound_Manager.h"

CState_Aerith_Damage_Catch::CState_Aerith_Damage_Catch(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine)
	: BASE(pActor, pStatemachine)
{
}

HRESULT CState_Aerith_Damage_Catch::Initialize_State(CState* pPreviousState)
{
	__super::Initialize_State(pPreviousState);

	m_pActor_ModelCom.lock()->Set_Animation("Battle00|B_DmgCatch02_0", 1.3f, false, true, 2.f);

	m_pActor_TransformCom.lock()->Set_Move_AnimationPosition(false);

	return S_OK;
}

void CState_Aerith_Damage_Catch::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
	
	// 상태 이상
	if (m_pActor_ModelCom.lock()->IsAnimation_Finished("Battle00|B_DmgCatch02_0"))
		m_pActor_ModelCom.lock()->Set_Animation("Battle00|B_DmgCatch02_1", 1.3f, true, true, 2.f);

	// 정상화됨
	if (m_bIsNormal)
		m_pActor_ModelCom.lock()->Set_Animation("Battle00|B_DmgCatch02_2", 1.3f, false, true, 2.f);

	// 애니메이션 종료시 돌아감
	if (m_pActor_ModelCom.lock()->IsAnimation_Finished("Battle00|B_DmgCatch02_2"))
		m_pStateMachineCom.lock()->Enter_State<CState_Aerith_Idle>();
}

void CState_Aerith_Damage_Catch::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CState_Aerith_Damage_Catch::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CState_Aerith_Damage_Catch::Transition_State(CState* pNextState)
{
	__super::Transition_State(pNextState);
	m_bIsNormal = false;

	m_pActor_TransformCom.lock()->Set_Move_AnimationPosition(true);
}

bool CState_Aerith_Damage_Catch::isValid_NextState(CState* state)
{
	return true;
}

void CState_Aerith_Damage_Catch::Free()
{
}
