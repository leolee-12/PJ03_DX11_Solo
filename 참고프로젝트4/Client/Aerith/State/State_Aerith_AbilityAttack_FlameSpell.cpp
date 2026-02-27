#include "stdafx.h"
#include "Aerith/State/State_List_Aerith.h"

#include "Aerith/Weapon/Aerith_List_Weapon.h"
#include "PartObject.h"

CState_Aerith_AbilityAttack_FlameSpell::CState_Aerith_AbilityAttack_FlameSpell(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine)
	: BASE(pActor, pStatemachine)
{
	
}

HRESULT CState_Aerith_AbilityAttack_FlameSpell::Initialize_State(CState* pPreviousState)
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

	// 화염구에 적절한 애니메이션 설정 필요
	m_pActor_ModelCom.lock()->Set_Animation("Battle00|B_MagAttack02_0", 1.15f, false, true, 2.f);

	/*shared_ptr< CAttackCollider> pCollider;
	m_pGameInstance->Add_CloneObject<CAttackCollider>(m_pGameInstance->Get_CreateLevelIndex(), L_EFFECT, L"Prototype_GameObject_AttackCollider", nullptr, &pCollider);
	m_pAttackCollider = pCollider;
	m_pAttackCollider.lock()->Get_TransformCom().lock()->Set_Scaling(4.f, 2.f, 4.f);*/

	return S_OK;
}

void CState_Aerith_AbilityAttack_FlameSpell::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);

	
	if (m_pActor_ModelCom.lock()->IsCurrentAnimation("Battle00|B_MagAttack02_0"))
	{
		SubState_0(fTimeDelta);
	}
	else if (m_pActor_ModelCom.lock()->IsCurrentAnimation("Battle00|B_MagAttack02_1"))
	{
		SubState_1(fTimeDelta);
	}
	else if (m_pActor_ModelCom.lock()->IsCurrentAnimation("Battle00|B_MagAttack02_2"))
	{
		SubState_2(fTimeDelta);
	}
}

void CState_Aerith_AbilityAttack_FlameSpell::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CState_Aerith_AbilityAttack_FlameSpell::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CState_Aerith_AbilityAttack_FlameSpell::Transition_State(CState* pNextState)
{
	__super::Transition_State(pNextState);
}

bool CState_Aerith_AbilityAttack_FlameSpell::isValid_NextState(CState* state)
{
	return true;
}

void CState_Aerith_AbilityAttack_FlameSpell::Free()
{
}

void CState_Aerith_AbilityAttack_FlameSpell::SubState_0(_cref_time fTimeDelta)
{
	if (m_pActor_ModelCom.lock()->IsAnimation_Finished())
		m_pActor_ModelCom.lock()->Set_Animation("Battle00|B_MagAttack02_1", 1.15f, false, true, 2.f);
}

void CState_Aerith_AbilityAttack_FlameSpell::SubState_1(_cref_time fTimeDelta)
{
	if (m_pActor_ModelCom.lock()->IsAnimation_Finished())
		m_pActor_ModelCom.lock()->Set_Animation("Battle00|B_MagAttack02_2", 1.4f, false, true, 2.f, false, false);
}

void CState_Aerith_AbilityAttack_FlameSpell::SubState_2(_cref_time fTimeDelta)
{
	// 특정 프레임에 화염구 생성코드
	if (m_pActor_ModelCom.lock()->IsAnimation_Frame_Once(6.f))
	{
		Create_FlameBall(fTimeDelta);
	}

	if (m_pActor_ModelCom.lock()->IsAnimation_Finished())
		m_pStateMachineCom.lock()->Enter_State<CState_Aerith_Idle>();
}

void CState_Aerith_AbilityAttack_FlameSpell::Create_FlameBall(_cref_time fTimeDelta)
{
	shared_ptr<CAerith_FlameBall> pBullet = { nullptr };
	m_pGameInstance->Add_CloneObject(m_pGameInstance->Get_CreateLevelIndex(), L_OBJECT,
		TEXT("Prototype_GameObject_Aerith_FlameBall"), nullptr, &pBullet);
	pBullet->Set_Owner(m_pActor);
	auto pPart = m_pActor.lock()->Find_PartObject(TEXT("Part_Weapon"));
	if (pPart)
	{
		_matrix SocketMatrix = pPart->Get_WorldMatrixFromBone(TEXT("R_RodAGimmickS_End"));
		_matrix ActorMatrix = m_pActor.lock()->Get_TransformCom().lock()->Get_WorldFloat4x4();
		//SocketMatrix *= ActorMatrix;

		pBullet->Get_TransformCom().lock()->Set_Look_Manual(ActorMatrix.r[2]);
		pBullet->Get_TransformCom().lock()->Set_Position(fTimeDelta, SocketMatrix.r[3]);
	}
}
