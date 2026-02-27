#include "stdafx.h"
#include "Character.h"
#include "State_List_Sweeper.h"
#include "GameInstance.h"
#include "Effect_Group.h"

#include "StatusComp.h"
#include "Particle.h"
#include "Bullet.h"


CState_Sweeper_Rush::CState_Sweeper_Rush(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine)
	:CState_Sweeper(pActor, pStatemachine)
{
}

HRESULT CState_Sweeper_Rush::Initialize_State(CState* pPreviousState)
{
	__super::Initialize_State(pPreviousState);

	m_iAttackCount = 0;
	m_pActor_ModelCom.lock()->Set_Animation("Main|B_AtkCharge01_0", 1.3f, false);
	m_pActor_TransformCom.lock()->Set_Move_AnimationPosition(false);

	DynPtrCast<IStatusInterface>(m_pActor.lock())->Get_StatusCom().lock()->Update_ActionPower(10);
	
	return S_OK;
}

void CState_Sweeper_Rush::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);

	if (m_pActor_ModelCom.lock()->IsAnimation_Finished("Main|B_AtkCharge01_0"))
	{
		m_pActor_ModelCom.lock()->Set_Animation("Main|B_AtkCharge01_1", 1.f, false);
	}

	if (m_pActor_ModelCom.lock()->IsAnimation_Finished("Main|B_AtkCharge01_2"))
	{
		m_pStateMachineCom.lock()->Enter_State<CState_Sweeper_Walk>();
	}

	if (m_pActor_ModelCom.lock()->IsAnimation_Finished("Main|B_AtkCharge01_1"))
	{
		if (m_iAttackCount >= 3)
			m_pActor_ModelCom.lock()->Set_Animation("Main|B_AtkCharge01_2", 1.f, false);
		else
			m_pActor_ModelCom.lock()->Set_Animation("Main|B_AtkCharge01_1", 1.f, false);

		m_iAttackCount++;
	}
}

void CState_Sweeper_Rush::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (m_pActor_ModelCom.lock()->Get_CurrentAnimationName() == "Main|B_AtkCharge01_1")
	{
		m_pActor_TransformCom.lock()->Go_Straight(fTimeDelta * 4.f);
		static_pointer_cast<CCharacter>(m_pActor.lock())->TurnOn_Weapon(L"Part_Rush");
	}
	else
	{
		static_pointer_cast<CCharacter>(m_pActor.lock())->TurnOff_Weapon(L"Part_Rush");
	}

}

void CState_Sweeper_Rush::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CState_Sweeper_Rush::Transition_State(CState* pNextState)
{
	__super::Transition_State(pNextState);


}

bool CState_Sweeper_Rush::isValid_NextState(CState* state)
{
	if (m_pActor_ModelCom.lock()->IsAnimation_Finished())
		return true;
	else
		return false;

	return true;
}

void CState_Sweeper_Rush::Free()
{
}
