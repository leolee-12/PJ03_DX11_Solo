#include "stdafx.h"
#include "State_List_Sweeper.h"
#include "GameInstance.h"
#include "Effect_Group.h"

#include "Particle.h"
#include "Bullet.h"


CState_Sweeper_RShoot::CState_Sweeper_RShoot(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine)
	:CState_Sweeper(pActor, pStatemachine)
{
}

HRESULT CState_Sweeper_RShoot::Initialize_State(CState* pPreviousState)
{
	__super::Initialize_State(pPreviousState);


	m_iAttackCount = 0;

	m_pActor_ModelCom.lock()->Set_Animation("Main|B_AtkMachinegunR01_0", 1.f, false);
	m_pActor_TransformCom.lock()->Set_Move_AnimationPosition(false);

	return S_OK;
}

void CState_Sweeper_RShoot::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);

	
	//사격 ~ IDLE로변경
	if (m_pActor_ModelCom.lock()->IsAnimation_Finished("Main|B_AtkMachinegunR01_2"))
	{
		m_pStateMachineCom.lock()->Enter_State<CState_Sweeper_Walk>();

	}

	//사격종료
	if (m_iAttackCount >= 8 && m_pActor_ModelCom.lock()->IsAnimation_Finished())
	{
		m_pActor_ModelCom.lock()->Set_Animation("Main|B_AtkMachinegunR01_2", 1.f, false);
	}
	//사격준비 ~ 사격
	if (m_pActor_ModelCom.lock()->IsAnimation_Finished("Main|B_AtkMachinegunR01_0"))
	{
		m_pActor_ModelCom.lock()->Set_Animation("Main|B_AtkMachinegunR01_1", 1.f, false);
	}


	//사격중
	if (m_pActor_ModelCom.lock()->IsAnimation_Finished("Main|B_AtkMachinegunR01_1"))
	{
		m_pActor_ModelCom.lock()->Set_Animation("Main|B_AtkMachinegunR01_1", 2.5f, false);

		m_iAttackCount++;
	}
}

void CState_Sweeper_RShoot::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);

	//총알생성
	if (m_pActor_ModelCom.lock()->Get_CurrentAnimationName() == "Main|B_AtkMachinegunR01_1" && m_pActor_ModelCom.lock()->IsAnimation_Frame_Once(5))
	{
		shared_ptr<CBullet> pBullet;
		m_pGameInstance->Add_CloneObject(m_pGameInstance->Get_CreateLevelIndex(), L_EFFECT, TEXT("Prototype_GameObject_Sweeper_Bullet"), nullptr, &pBullet);
		pBullet->Set_Owner(m_pActor.lock());
		pBullet->Set_StatusComByOwner("Sweeper_Flame");

		_matrix MuzzleMatrix = m_pActor.lock()->Get_ModelCom().lock()->Get_BoneTransformMatrixWithParents(TEXT("R_VFXMuzzleA_a"));

		_vector vLook = XMVector3Normalize(MuzzleMatrix.r[0]);
		_vector vPos = vLook * 2.2f + MuzzleMatrix.r[3];

		pBullet->Get_TransformCom().lock()->Set_Look_Manual(MuzzleMatrix.r[0]);
		pBullet->Get_TransformCom().lock()->Set_Position(fTimeDelta, vPos);
	}
}

void CState_Sweeper_RShoot::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CState_Sweeper_RShoot::Transition_State(CState* pNextState)
{
	__super::Transition_State(pNextState);


}

bool CState_Sweeper_RShoot::isValid_NextState(CState* state)
{
	if (m_pActor_ModelCom.lock()->IsAnimation_Finished())
		return true;
	else
		return false;

	return true;
}

void CState_Sweeper_RShoot::Free()
{
}
