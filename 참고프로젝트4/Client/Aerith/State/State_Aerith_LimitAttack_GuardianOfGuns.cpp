#include "stdafx.h"
#include "Aerith/State/State_List_Aerith.h"

#include "Aerith/Weapon/Aerith_GunArea.h"

CState_Aerith_LimitAttack_GuardianOfGuns::CState_Aerith_LimitAttack_GuardianOfGuns(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine)
	: BASE(pActor, pStatemachine)
{
}

HRESULT CState_Aerith_LimitAttack_GuardianOfGuns::Initialize_State(CState* pPreviousState)
{
	__super::Initialize_State(pPreviousState);

	Check_UseCommand();

	//임시 가까운 몬스터로 타겟팅 (선택으로 변경하기)
	m_TargetInfo = GET_SINGLE(CClient_Manager)->Get_TargetMonster(AERITH);

	if (m_TargetInfo.pTarget.lock() != nullptr)
	{
		m_pActor_TransformCom.lock()->Look_At_OnLand(m_TargetInfo.vTargetPos);
		GET_SINGLE(CCamera_Manager)->Get_ThirdPersonCamera(AERITH).lock()->Turn_To_Look(m_pActor_TransformCom.lock()->Get_State(CTransform::STATE_LOOK));
	}
	
	m_pActor_ModelCom.lock()->Set_Animation("Battle00|B_AtkLimit02_0", 1.15f, false, true, 2.f);

	GET_SINGLE(CCamera_Manager)->Call_CutSceneCamera("Aerith_Limit_Following", 0, m_pActor.lock());

	return S_OK;
}

void CState_Aerith_LimitAttack_GuardianOfGuns::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
	
	// 특정 프레임에 특정적을 포화하는 객체 생성
	// 딱 한번만 투사체 생성
	if (m_pActor_ModelCom.lock()->IsAnimation_Frame_Once(80.f))
	{
		Create_AttackArea(fTimeDelta);
	}

	if (m_pActor_ModelCom.lock()->IsAnimation_Finished())
		m_pStateMachineCom.lock()->Enter_State<CState_Aerith_Idle>();
}

void CState_Aerith_LimitAttack_GuardianOfGuns::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CState_Aerith_LimitAttack_GuardianOfGuns::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CState_Aerith_LimitAttack_GuardianOfGuns::Transition_State(CState* pNextState)
{
	__super::Transition_State(pNextState);
}

bool CState_Aerith_LimitAttack_GuardianOfGuns::isValid_NextState(CState* state)
{
	return true;
}

void CState_Aerith_LimitAttack_GuardianOfGuns::Free()
{
}

void CState_Aerith_LimitAttack_GuardianOfGuns::Create_AttackArea(_cref_time fTimeDelta)
{
	shared_ptr<CAerith_GunArea> pBullet = { nullptr };
	m_pGameInstance->Add_CloneObject(m_pGameInstance->Get_CreateLevelIndex(), L_OBJECT,
		TEXT("Prototype_GameObject_Aerith_GunArea"), nullptr, &pBullet);
	pBullet->Set_Owner(m_pActor);
	pBullet->Set_StatusComByOwner("Aerith_NormalAttack1");
	_matrix MuzzleMatrix = m_pActor.lock()->Get_ModelCom().lock()->
		Get_BoneTransformMatrixWithParents(TEXT("R_Weapon_a"));
	pBullet->Get_TransformCom().lock()->Set_Position(fTimeDelta, MuzzleMatrix.r[3]);

	_matrix ActorMatrix = m_pActor.lock()->Get_TransformCom().lock()->Get_WorldFloat4x4();
	pBullet->Get_TransformCom().lock()->Set_Look_Manual(ActorMatrix.r[2]);
}
