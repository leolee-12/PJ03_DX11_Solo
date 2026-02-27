#include "stdafx.h"
#include "State_List_Sweeper.h"
#include "GameInstance.h"
#include "Effect_Manager.h"
#include "Data_Manager.h"

#include "Particle.h"
#include "Bullet.h"
#include "StatusComp.h"


CState_Sweeper_Flame::CState_Sweeper_Flame(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine)
	:CState_Sweeper(pActor, pStatemachine)
{
}

HRESULT CState_Sweeper_Flame::Initialize_State(CState* pPreviousState)
{
	__super::Initialize_State(pPreviousState);

	m_bFlame = true;
	m_iAttackCount = 0;
	m_pActor_ModelCom.lock()->Set_Animation("Main|B_AtkJump01_0", 1.f, false);
	m_pActor_TransformCom.lock()->Set_Move_AnimationPosition(false);

	DynPtrCast<IStatusInterface>(m_pActor.lock())->Get_StatusCom().lock()->Update_ActionPower(5);

	return S_OK;
}

void CState_Sweeper_Flame::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);


	if (m_pActor_ModelCom.lock()->Get_CurrentAnimationName() == ("Main|B_AtkJump01_0") && m_pActor_ModelCom.lock()->IsAnimation_Frame_Once(18))
	{
		if (m_iAttackCount >= 4)
		{
			m_pActor_ModelCom.lock()->Set_Animation("Main|B_AtkSmoke01", 1.2f, false);
		}
		else
			m_pActor_ModelCom.lock()->Get_AnimationComponent()->Set_AnimTrackPos(12);

		m_iAttackCount++;
	}

	if (m_pActor_ModelCom.lock()->IsAnimation_Finished("Main|B_AtkSmoke01"))
	{
		m_pStateMachineCom.lock()->Enter_State<CState_Sweeper_Walk>();
	}
}

void CState_Sweeper_Flame::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (m_pActor_ModelCom.lock()->Get_CurrentAnimationName() == "Main|B_AtkSmoke01" && m_pActor_ModelCom.lock()->IsAnimation_Range(31, 58))
	{
		if (m_fTimeChecker.Update(fTimeDelta))
		{
			shared_ptr<CBullet> pBullet;
			m_pGameInstance->Add_CloneObject(m_pGameInstance->Get_CreateLevelIndex(),
				L_EFFECT, TEXT("Prototype_GameObject_Sweeper_Flame"), nullptr, &pBullet);
			pBullet->Set_Owner(m_pActor.lock());
			pBullet->Set_StatusComByOwner("Sweeper_Flame");

			_matrix MuzzleMatrix = m_pActor.lock()->Get_ModelCom().lock()->Get_BoneTransformMatrixWithParents(TEXT("C_VFXMuzzleA_a"));

			_vector vLook = XMVector3Normalize(MuzzleMatrix.r[2]);
			_vector vPos = vLook * -2.6f + MuzzleMatrix.r[3];

			pBullet->Get_TransformCom().lock()->Set_Look_Manual(-MuzzleMatrix.r[2]);
			pBullet->Get_TransformCom().lock()->Set_Position(fTimeDelta, vPos);

			GET_SINGLE(CEffect_Manager)->Create_Bone_Effect_One_Time<CParticle>(L"Sweeper_FireGas",
				L"C_VFXMuzzleA_a", m_pActor.lock());
		}
	}
}

void CState_Sweeper_Flame::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CState_Sweeper_Flame::Transition_State(CState* pNextState)
{
	__super::Transition_State(pNextState);
}

bool CState_Sweeper_Flame::isValid_NextState(CState* state)
{
	if (m_pActor_ModelCom.lock()->IsAnimation_Finished())
		return true;
	else
		return false;

	return true;
}

void CState_Sweeper_Flame::Free()
{
}
