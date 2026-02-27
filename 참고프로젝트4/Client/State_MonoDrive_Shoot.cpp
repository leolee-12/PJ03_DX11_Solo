#include "stdafx.h"
#include "State_List_MonoDrive.h"
#include "GameInstance.h"
#include "Effect_Group.h"

#include "Particle.h"


CState_MonoDrive_Shoot::CState_MonoDrive_Shoot(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine)
	:CState_MonoDrive(pActor, pStatemachine)
{
}

HRESULT CState_MonoDrive_Shoot::Initialize_State(CState* pPreviousState)
{
	__super::Initialize_State(pPreviousState);
	m_pActor.lock()->Get_PhysXControllerCom().lock()->Enable_Gravity(false);
	m_pActor_ModelCom.lock()->Set_Animation("Main|B_Magic01_0", 1.f, false);
	m_pActor_TransformCom.lock()->Set_Move_AnimationPosition(true);

	return S_OK;
}

void CState_MonoDrive_Shoot::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);

	m_pActor_TransformCom.lock()->Look_At_OnLand(m_vTargetPos);

	if (m_pActor_ModelCom.lock()->Get_CurrentAnimationName() == "Main|B_Magic01_1")
		m_pActor_TransformCom.lock()->Go_Up(fTimeDelta * 0.3f);

	if (m_pActor_ModelCom.lock()->IsAnimation_Finished("Main|B_Magic01_0"))
	{
		m_pActor_ModelCom.lock()->Set_Animation("Main|B_Magic01_1", 1.f, false);
	}
	if (m_pActor_ModelCom.lock()->IsAnimation_Finished("Main|B_Magic01_1"))
	{
		m_pActor_ModelCom.lock()->Set_Animation("Main|B_Magic01", 1.f, false);
	}

	if (m_pActor_ModelCom.lock()->IsAnimation_Finished("Main|B_Magic01"))
	{
		m_pStateMachineCom.lock()->Enter_State<CState_MonoDrive_Walk>();
	}
}

void CState_MonoDrive_Shoot::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (m_pActor_ModelCom.lock()->Get_CurrentAnimationName() == "Main|B_Magic01"&&m_pActor_ModelCom.lock()->IsAnimation_Frame_Once(56))
	{
		shared_ptr<CGameObject> pBullet;
		m_pGameInstance->Add_CloneObject(m_pGameInstance->Get_CreateLevelIndex(), L_EFFECT, TEXT("Prototype_GameObject_MonoDrive_FireBall"), nullptr, &pBullet);
		pBullet->Set_Owner(m_pActor.lock());

		_matrix MuzzleMatrix = m_pActor.lock()->Get_ModelCom().lock()->Get_BoneTransformMatrixWithParents(TEXT("C_Eye_a"));

		pBullet->Get_TransformCom().lock()->Set_Look_Manual(-MuzzleMatrix.r[2]);
		pBullet->Get_TransformCom().lock()->Set_Position(fTimeDelta, MuzzleMatrix.r[3]);
	}
}

void CState_MonoDrive_Shoot::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CState_MonoDrive_Shoot::Transition_State(CState* pNextState)
{
	__super::Transition_State(pNextState);
}

bool CState_MonoDrive_Shoot::isValid_NextState(CState* state)
{
	if (m_pActor_ModelCom.lock()->IsAnimation_Finished())
		return true;
	else
		return false;

	return true;
}

void CState_MonoDrive_Shoot::Free()
{
}
