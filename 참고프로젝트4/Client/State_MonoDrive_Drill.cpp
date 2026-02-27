#include "stdafx.h"
#include "GameInstance.h"
#include "State_List_MonoDrive.h"
#include "Character.h"

#include "Effect_Group.h"
#include "PhysX_Controller.h"

#include "Particle.h"


CState_MonoDrive_Drill::CState_MonoDrive_Drill(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine)
	:CState_MonoDrive(pActor, pStatemachine)
{
}

HRESULT CState_MonoDrive_Drill::Initialize_State(CState* pPreviousState)
{
	m_iAttackCount = 0;

	__super::Initialize_State(pPreviousState);
	m_pActor.lock()->Get_PhysXControllerCom().lock()->Enable_Gravity(false);
	m_pActor_ModelCom.lock()->Set_Animation("Main|B_AtkDrill01_0", 1.f, false);
	m_pActor_TransformCom.lock()->Set_Move_AnimationPosition(true);

	return S_OK;
}

void CState_MonoDrive_Drill::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);

	if(m_pActor_ModelCom.lock()->Get_CurrentAnimationName() != "Main|B_AtkDrill01_1")
		m_pActor_TransformCom.lock()->Look_At_OnLand(m_vTargetPos);

	if (m_pActor_ModelCom.lock()->Get_CurrentAnimationName() == "Main|B_AtkDrill01_1")
		m_pActor_TransformCom.lock()->Go_Straight(fTimeDelta*2.f);

	if (m_pActor_ModelCom.lock()->IsAnimation_Finished("Main|B_AtkDrill01_0"))
	{
		m_pActor_TransformCom.lock()->Set_Move_AnimationPosition(false);
		m_pActor_ModelCom.lock()->Set_Animation("Main|B_AtkDrill01_1", 1.f, false);
	}

	if (m_pActor_ModelCom.lock()->IsAnimation_Finished("Main|B_AtkDrill01_1"))
	{
		if(m_iAttackCount <=2)
			m_pActor_ModelCom.lock()->Set_Animation("Main|B_AtkDrill01_1", 1.f, false);
		else
		{
			m_pActor_TransformCom.lock()->Set_Move_AnimationPosition(true);
			m_pActor_ModelCom.lock()->Set_Animation("Main|B_AtkDrill01_2", 1.f, false);
		}
		++m_iAttackCount;
	}

	if (m_pActor_ModelCom.lock()->IsAnimation_Finished("Main|B_AtkDrill01_2"))
	{
		m_pStateMachineCom.lock()->Enter_State<CState_MonoDrive_Walk>();
	}
}

void CState_MonoDrive_Drill::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (m_pActor_ModelCom.lock()->Get_CurrentAnimationName() == "Main|B_AtkDrill01_1")
	{
		static_pointer_cast<CCharacter>(m_pActor.lock())->TurnOn_Weapon(L"Part_Drill");
	}
	if (m_pActor_ModelCom.lock()->Get_CurrentAnimationName() == "Main|B_AtkDrill01_2" && m_pActor_ModelCom.lock()->IsAnimation_UpTo(10))
	{
		static_pointer_cast<CCharacter>(m_pActor.lock())->TurnOff_Weapon(L"Part_Drill");
	}
}

void CState_MonoDrive_Drill::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CState_MonoDrive_Drill::Transition_State(CState* pNextState)
{
	__super::Transition_State(pNextState);
}

bool CState_MonoDrive_Drill::isValid_NextState(CState* state)
{
	if (m_pActor_ModelCom.lock()->IsAnimation_Finished())
		return true;
	else
		return false;

	return true;
}

void CState_MonoDrive_Drill::Free()
{
}
