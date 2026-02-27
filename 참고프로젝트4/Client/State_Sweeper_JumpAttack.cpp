#include "stdafx.h"
#include "State_List_Sweeper.h"
#include "GameInstance.h"
#include "Effect_Group.h"

#include "Particle.h"
#include "Bullet.h"


CState_Sweeper_JumpAttack::CState_Sweeper_JumpAttack(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine)
	:CState_Sweeper(pActor, pStatemachine)
{
}

HRESULT CState_Sweeper_JumpAttack::Initialize_State(CState* pPreviousState)
{
	__super::Initialize_State(pPreviousState);

	m_iAttackCount = 0;

	m_pActor_ModelCom.lock()->Set_Animation("Main|B_AtkShot01_0", 2.f, false);
	m_pActor_TransformCom.lock()->Set_Move_AnimationPosition(false);

	if (m_pActor_ModelCom.lock()->Get_PrevAnimationName() == "Main|N_Run01_1")
	{
		m_pActor_ModelCom.lock()->Set_Animation("Main|N_Run01_1", 1.f, false);
		m_pActor_TransformCom.lock()->Set_Move_AnimationPosition(false);
	}

	return S_OK;
}

void CState_Sweeper_JumpAttack::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);

	m_pActor_TransformCom.lock()->Look_At_OnLand(m_vTargetPos);

	if (m_pActor_ModelCom.lock()->Get_CurrentAnimationName() == "Main|N_Run01_1")
	{
		m_pActor_ModelCom.lock()->Set_Animation("Main|B_AtkShot01_0", 2.f, false);
	}

	if (m_pActor_ModelCom.lock()->IsAnimation_Finished("Main|B_AtkShot01_0"))
	{
		m_pActor_ModelCom.lock()->Set_Animation("Main|B_AtkGrenadeS01_0", 1.f, false);
	}
	if (m_pActor_ModelCom.lock()->Get_CurrentAnimationName() == "Main|B_AtkGrenadeS01_0"&&m_pActor_ModelCom.lock()->IsAnimation_Range(5,6))
	{
		shared_ptr<CBullet> pBullet;
		m_pGameInstance->Add_CloneObject(m_pGameInstance->Get_CreateLevelIndex(), L_EFFECT, TEXT("Prototype_GameObject_Soldier_Bullet"), nullptr, &pBullet);
		pBullet->Set_Owner(m_pActor.lock());
		pBullet->Set_StatusComByOwner("Sweeper_Flame");

	}
	if (m_pActor_ModelCom.lock()->IsAnimation_Finished("Main|B_AtkGrenadeS01_0"))
	{
		m_pActor_ModelCom.lock()->Set_Animation("Main|B_AtkGrenadeS01_0", 1.f, false);



		m_iAttackCount++;
	}
}

void CState_Sweeper_JumpAttack::Tick(_cref_time fTimeDelta)
{
	if (m_pActor_ModelCom.lock()->Get_CurrentAnimationName() == "Main|N_Run01_1")
	{
		m_pActor_TransformCom.lock()->Go_Target(m_vTargetPos, fTimeDelta, 1.f);
	}

	__super::Tick(fTimeDelta);
}

void CState_Sweeper_JumpAttack::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CState_Sweeper_JumpAttack::Transition_State(CState* pNextState)
{
	__super::Transition_State(pNextState);


}

bool CState_Sweeper_JumpAttack::isValid_NextState(CState* state)
{
	if (m_pActor_ModelCom.lock()->IsAnimation_Finished())
		return true;
	else
		return false;

	return true;
}

void CState_Sweeper_JumpAttack::Free()
{
}
