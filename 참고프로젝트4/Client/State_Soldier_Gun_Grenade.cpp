#include "stdafx.h"
#include "State_List_Soldier_Gun.h"
#include "GameInstance.h"
#include "Soldier_Grenade.h"
#include "Character.h"
#include "Effect_Group.h"

#include "Particle.h"


CState_Soldier_Gun_Grenade::CState_Soldier_Gun_Grenade(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine)
	:CState_Soldier_Gun(pActor, pStatemachine)
{
}

HRESULT CState_Soldier_Gun_Grenade::Initialize_State(CState* pPreviousState)
{
	__super::Initialize_State(pPreviousState);

	m_pActor_ModelCom.lock()->Set_Animation("Main|B_AtkHandGrenade02", 1.f, false);
	m_pActor_TransformCom.lock()->Set_Move_AnimationPosition(false);

	return S_OK;
}

void CState_Soldier_Gun_Grenade::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);

	m_pActor_TransformCom.lock()->Look_At_OnLand(m_vTargetPos);

	if (m_pActor_ModelCom.lock()->IsAnimation_Finished())
	{
		m_pStateMachineCom.lock()->Enter_State<CState_Soldier_Gun_Run>();
	}
}

void CState_Soldier_Gun_Grenade::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (m_pActor_ModelCom.lock()->IsAnimation_Frame_Once(25))
	{
		m_pGameInstance->Add_CloneObject(m_pGameInstance->Get_CreateLevelIndex(), L_OBJECT, TEXT("Prototype_GameObject_Soldier_Grenade"), nullptr, &m_pBullet);
		m_pBullet->Set_Owner(m_pActor.lock());

		_matrix MuzzleMatrix = m_pActor.lock()->Get_ModelCom().lock()->Get_BoneTransformMatrixWithParents(TEXT("L_Thumb_End"));

		m_pBullet->Get_TransformCom().lock()->Set_Position(fTimeDelta, MuzzleMatrix.r[3]);
		static_pointer_cast<CSoldier_Grenade>(m_pBullet)->Set_BoneName(L"L_Thumb_End");
	}
	if (m_pActor_ModelCom.lock()->IsAnimation_Frame_Once(43))
	{
		static_pointer_cast<CSoldier_Grenade>(m_pBullet)->Set_Move(true);
	}
}

void CState_Soldier_Gun_Grenade::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CState_Soldier_Gun_Grenade::Transition_State(CState* pNextState)
{
	__super::Transition_State(pNextState);
}

bool CState_Soldier_Gun_Grenade::isValid_NextState(CState* state)
{
	if (m_pActor_ModelCom.lock()->IsAnimation_Finished())
		return true;
	else
		return false;

	return true;
}

void CState_Soldier_Gun_Grenade::Free()
{
}
