#include "stdafx.h"
#include "State_List_Soldier_Gun.h"
#include "GameInstance.h"
#include "Effect_Group.h"
#include "Particle.h"

#include "Security_Soldier.h"
#include "Bullet.h"


CState_Soldier_Gun_Shoot::CState_Soldier_Gun_Shoot(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine)
	:CState_Soldier_Gun(pActor, pStatemachine)
{
}

HRESULT CState_Soldier_Gun_Shoot::Initialize_State(CState* pPreviousState)
{
	__super::Initialize_State(pPreviousState);

	m_iAttackCount = 0;

	m_pActor_ModelCom.lock()->Set_Animation("Main|B_AtkShot01_0", 1.f, false);
	m_pActor_TransformCom.lock()->Set_Move_AnimationPosition(false);

	if (m_pActor_ModelCom.lock()->Get_PrevAnimationName() == "Main|N_Run01_1")
	{
		m_pActor_ModelCom.lock()->Set_Animation("Main|N_Run01_1", 1.f, false);
		m_pActor_TransformCom.lock()->Set_Move_AnimationPosition(false);
	}

	return S_OK;
}

void CState_Soldier_Gun_Shoot::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);

	m_pActor_TransformCom.lock()->Look_At_OnLand(m_vTargetPos);

	_int iBullet = dynamic_pointer_cast<CSecurity_Soldier>(m_pActor.lock())->Get_BulletCount();

	cout << iBullet << endl;

	if (iBullet <= 0 && m_pActor_ModelCom.lock()->Get_CurrentAnimationName() != "Main|B_Reload01")
	{
		m_pActor_ModelCom.lock()->Set_Animation("Main|B_Reload01",1.f,false);
	}

	if (m_pActor_ModelCom.lock()->IsAnimation_Finished("Main|B_Reload01"))
	{
		dynamic_pointer_cast<CSecurity_Soldier>(m_pActor.lock())->Set_BulletCount(20);

		m_pActor_ModelCom.lock()->Set_Animation("Main|B_AtkGrenadeS01_0", 1.5f, false);

	}

	if (m_pActor_ModelCom.lock()->Get_CurrentAnimationName() == "Main|N_Run01_1")
	{
		m_pActor_ModelCom.lock()->Set_Animation("Main|B_AtkShot01_0", 1.f, false);
	}

	if (m_pActor_ModelCom.lock()->IsAnimation_Finished("Main|B_AtkShot01_0"))
	{
		m_pActor_ModelCom.lock()->Set_Animation("Main|B_AtkGrenadeS01_0", 1.5f, false);
	}
	if (m_iAttackCount >= 2 && m_pActor_ModelCom.lock()->IsAnimation_Finished())
	{
		m_pStateMachineCom.lock()->Wait_State<CState_Soldier_Gun_Run>();
	}
	else if (m_pActor_ModelCom.lock()->IsAnimation_Finished("Main|B_AtkGrenadeS01_0"))
	{
		m_pActor_ModelCom.lock()->Set_Animation("Main|B_AtkGrenadeS01_0", 1.5f, false);

		m_iAttackCount++;
	}

}

void CState_Soldier_Gun_Shoot::Tick(_cref_time fTimeDelta)
{
	if (m_pActor_ModelCom.lock()->Get_CurrentAnimationName() == "Main|N_Run01_1")
	{
		m_pActor_TransformCom.lock()->Go_Target(m_vTargetPos, fTimeDelta, 1.f);
	}
	
	if (m_pActor_ModelCom.lock()->Get_CurrentAnimationName() == "Main|B_AtkGrenadeS01_0" && m_pActor_ModelCom.lock()->IsAnimation_Frame_Once(5))
	{
		shared_ptr<CBullet> pBullet;
		m_pGameInstance->Add_CloneObject(m_pGameInstance->Get_CreateLevelIndex(), L_EFFECT, TEXT("Prototype_GameObject_Soldier_Bullet"), nullptr, &pBullet);
		pBullet->Set_Owner(m_pActor.lock());
		pBullet->Set_StatusComByOwner("Soldier_Gun");
		dynamic_pointer_cast<CSecurity_Soldier>(m_pActor.lock())->DiscountBullet();
	}
	__super::Tick(fTimeDelta);
}

void CState_Soldier_Gun_Shoot::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CState_Soldier_Gun_Shoot::Transition_State(CState* pNextState)
{
	__super::Transition_State(pNextState);


}

bool CState_Soldier_Gun_Shoot::isValid_NextState(CState* state)
{
	if (m_pActor_ModelCom.lock()->IsAnimation_Finished())
		return true;
	else
		return false;

	return true;
}

void CState_Soldier_Gun_Shoot::Free()
{
}
