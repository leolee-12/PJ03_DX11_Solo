#include "stdafx.h"
#include "State_List_Stray.h"
#include "Stray_Laser.h"
#include "Stray_Charge.h"
#include "GameInstance.h"

#include "Client_Manager.h"
#include "PhysX_Manager.h"

CState_Stray_Shoot::CState_Stray_Shoot(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine)
	:CState_Stray(pActor, pStatemachine)
{
}

HRESULT CState_Stray_Shoot::Initialize_State(CState* pPreviousState)
{
	__super::Initialize_State(pPreviousState);

	m_iAttackCount = 0;

	m_pActor_ModelCom.lock()->Set_Animation("Main|B_AtkLaser01_0", 1.f, false);
	m_pActor_TransformCom.lock()->Set_Move_AnimationPosition(false);


	shared_ptr<CStray_Charge> pBullet = { nullptr };
	m_pGameInstance->Add_CloneObject(m_pGameInstance->Get_CreateLevelIndex(), L_EFFECT, TEXT("Prototype_GameObject_Stray_Charge"), nullptr, &pBullet);
	pBullet->Set_Owner(m_pActor);
	_matrix MuzzleMatrix = m_pActor.lock()->Get_ModelCom().lock()->Get_BoneTransformMatrixWithParents(TEXT("C_VFXMuzzleA_a"));
	pBullet->Get_TransformCom().lock()->Set_Position(1.f, MuzzleMatrix.r[3]);

	return S_OK;
}

void CState_Stray_Shoot::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);

	m_pActor_TransformCom.lock()->Look_At(m_vTargetPos);

	if (m_pActor_ModelCom.lock()->IsAnimation_Finished("Main|B_AtkLaser01_0"))
	{
		m_pActor_ModelCom.lock()->Set_Animation("Main|B_AtkLaser01_1", 1.f, false);
	}

	if (m_pActor_ModelCom.lock()->IsAnimation_Finished("Main|B_AtkLaser01_1"))
	{
		m_pActor_ModelCom.lock()->Set_Animation("Main|B_AtkLaser01_2", 1.f, false);
	}

	if (m_pActor_ModelCom.lock()->IsAnimation_Finished("Main|B_AtkLaser01_2"))
	{
		m_pStateMachineCom.lock()->Enter_State<CState_Stray_Idle>();
	}
}

void CState_Stray_Shoot::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (m_pActor_ModelCom.lock()->Get_CurrentAnimationName() == "Main|B_AtkLaser01_2" && m_pActor_ModelCom.lock()->IsAnimation_Frame_Once(1))
	{
		shared_ptr<CStray_Laser> pBullet = { nullptr };
		m_pGameInstance->Add_CloneObject(m_pGameInstance->Get_CreateLevelIndex(), L_EFFECT, TEXT("Prototype_GameObject_Stray_Laser"), nullptr, &pBullet);
		pBullet->Set_Owner(m_pActor);

		_matrix MuzzleMatrix = m_pActor.lock()->Get_ModelCom().lock()->Get_BoneTransformMatrixWithParents(TEXT("C_VFXMuzzleA_a"));
		_float3 vLook = XMVector3Normalize(MuzzleMatrix.r[0]);
		_float3 vPos = MuzzleMatrix.r[3] + vLook * 0.5f;
		
		pBullet->Get_TransformCom().lock()->Set_Look_Manual(MuzzleMatrix.r[0]);
		pBullet->Get_TransformCom().lock()->Set_Position(fTimeDelta, vPos);
	}
}

void CState_Stray_Shoot::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CState_Stray_Shoot::Transition_State(CState* pNextState)
{
	__super::Transition_State(pNextState);
}

bool CState_Stray_Shoot::isValid_NextState(CState* state)
{
	return true;
}

void CState_Stray_Shoot::Free()
{
}
