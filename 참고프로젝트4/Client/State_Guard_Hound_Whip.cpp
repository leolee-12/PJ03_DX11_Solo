#include "stdafx.h"
#include "State_List_Guard_Hound.h"
#include "Character.h"

CState_Guard_Hound_Whip::CState_Guard_Hound_Whip(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine)
	:CState_Guard_Hound(pActor, pStatemachine)
{
}

HRESULT CState_Guard_Hound_Whip::Initialize_State(CState* pPreviousState)
{
	__super::Initialize_State(pPreviousState);

	_int iRandom = rand() % 2;

	if (iRandom == 0)
		m_pActor_ModelCom.lock()->Set_Animation("Main|B_AtkDWhip01", 1.f, false);
	else
		m_pActor_ModelCom.lock()->Set_Animation("Main|B_AtkFwhip01", 1.f, false);

	m_pActor_TransformCom.lock()->Set_Move_AnimationPosition(false);

	return S_OK;
}

void CState_Guard_Hound_Whip::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);

	m_pActor_TransformCom.lock()->Look_At_OnLand(m_vTargetPos);
	
	if (m_pActor_ModelCom.lock()->IsAnimation_Finished())
	{
		m_pStateMachineCom.lock()->Enter_State<CState_Guard_Hound_Run>();
	}
}

void CState_Guard_Hound_Whip::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (m_pActor_ModelCom.lock()->Get_CurrentAnimationName() == "Main|B_AtkDWhip01" && m_pActor_ModelCom.lock()->IsAnimation_Range(28, 40))
	{
		static_pointer_cast<CCharacter>(m_pActor.lock())->TurnOn_Weapon(L"Part_Whip");
	}
	else if (m_pActor_ModelCom.lock()->Get_CurrentAnimationName() == "Main|B_AtkFwhip01" && m_pActor_ModelCom.lock()->IsAnimation_Range(38, 50))
	{
		static_pointer_cast<CCharacter>(m_pActor.lock())->TurnOn_Weapon(L"Part_Whip");
	}
	else
	{
		static_pointer_cast<CCharacter>(m_pActor.lock())->TurnOff_Weapon(L"Part_Whip");
	}
}

void CState_Guard_Hound_Whip::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CState_Guard_Hound_Whip::Transition_State(CState* pNextState)
{
	__super::Transition_State(pNextState);
}

bool CState_Guard_Hound_Whip::isValid_NextState(CState* state)
{
	return true;
}

void CState_Guard_Hound_Whip::Free()
{
}
