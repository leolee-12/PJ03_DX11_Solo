#include "stdafx.h"
#include "State_List_Sweeper.h"
#include "GameInstance.h"
#include "Effect_Group.h"

#include "Particle.h"
#include "Bullet.h"
#include "StatusComp.h"

CState_Sweeper_Bombing::CState_Sweeper_Bombing(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine)
	:CState_Sweeper(pActor, pStatemachine)
{
}

HRESULT CState_Sweeper_Bombing::Initialize_State(CState* pPreviousState)
{
	__super::Initialize_State(pPreviousState);

	m_iAttackCount = 0;

	m_pActor_ModelCom.lock()->Set_Animation("Main|B_Idle03_0", 1.f, false);
	m_pActor_TransformCom.lock()->Set_Move_AnimationPosition(false);

	DynPtrCast<IStatusInterface>(m_pActor.lock())->Get_StatusCom().lock()->Update_ActionPower(5);

	return S_OK;
}

void CState_Sweeper_Bombing::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);

	if (m_pActor_ModelCom.lock()->IsAnimation_Finished("Main|B_Idle03_0"))
	{
		m_pStateMachineCom.lock()->Enter_State<CState_Sweeper_Walk>();
	}
}

void CState_Sweeper_Bombing::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (m_pActor_ModelCom.lock()->IsAnimation_Frame_Once(41) || m_pActor_ModelCom.lock()->IsAnimation_Frame_Once(45) || m_pActor_ModelCom.lock()->IsAnimation_Frame_Once(49) || m_pActor_ModelCom.lock()->IsAnimation_Frame_Once(53) || m_pActor_ModelCom.lock()->IsAnimation_Frame_Once(57))
	{
		shared_ptr<CBullet> pBullet;
		m_pGameInstance->Add_CloneObject(m_pGameInstance->Get_CreateLevelIndex(), L_EFFECT, TEXT("Prototype_GameObject_Sweeper_Bombing"), nullptr, &pBullet);
		pBullet->Set_Owner(m_pActor.lock());
		pBullet->Set_StatusComByOwner("Sweeper_Flame");

		_matrix MuzzleMatrix = m_pActor.lock()->Get_ModelCom().lock()->Get_BoneTransformMatrixWithParents(TEXT("L_VFXMuzzleA_a"));

		//pBullet->Get_TransformCom().lock()->Rotation(_float3(0.f, 0.f, 0.f), XMConvertToRadians(90.f));
		pBullet->Get_TransformCom().lock()->Set_Position(fTimeDelta, MuzzleMatrix.r[3]);
		pBullet->Get_TransformCom().lock()->Set_Look_Manual(-MuzzleMatrix.r[2]);
	}
	else if (m_pActor_ModelCom.lock()->IsAnimation_Frame_Once(42) || m_pActor_ModelCom.lock()->IsAnimation_Frame_Once(46) || m_pActor_ModelCom.lock()->IsAnimation_Frame_Once(50) || m_pActor_ModelCom.lock()->IsAnimation_Frame_Once(54) || m_pActor_ModelCom.lock()->IsAnimation_Frame_Once(58))
	{
		shared_ptr<CBullet> pBullet;
		m_pGameInstance->Add_CloneObject(m_pGameInstance->Get_CreateLevelIndex(), L_EFFECT, TEXT("Prototype_GameObject_Sweeper_Bombing"), nullptr, &pBullet);
		pBullet->Set_Owner(m_pActor.lock());
		pBullet->Set_StatusComByOwner("Sweeper_Flame");

		_matrix MuzzleMatrix = m_pActor.lock()->Get_ModelCom().lock()->Get_BoneTransformMatrixWithParents(TEXT("R_VFXMuzzleA_a"));

		//pBullet->Get_TransformCom().lock()->Rotation(_float3(0.f, 0.f, 0.f), XMConvertToRadians(90.f));
		pBullet->Get_TransformCom().lock()->Set_Position(fTimeDelta, MuzzleMatrix.r[3]);
		pBullet->Get_TransformCom().lock()->Set_Look_Manual(-MuzzleMatrix.r[2]);
	}
}

void CState_Sweeper_Bombing::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CState_Sweeper_Bombing::Transition_State(CState* pNextState)
{
	__super::Transition_State(pNextState);


}

bool CState_Sweeper_Bombing::isValid_NextState(CState* state)
{
	if (m_pActor_ModelCom.lock()->IsAnimation_Finished())
		return true;
	else
		return false;

	return true;
}

void CState_Sweeper_Bombing::Free()
{
}
