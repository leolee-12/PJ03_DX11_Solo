#include "stdafx.h"
#include "Aerith/State/State_List_Aerith.h"

#include "Aerith/Weapon/Aerith_List_Weapon.h"
#include "PartObject.h"

CState_Aerith_AbilityAttack_LustrousShield::CState_Aerith_AbilityAttack_LustrousShield(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine)
	: BASE(pActor, pStatemachine)
{
}

HRESULT CState_Aerith_AbilityAttack_LustrousShield::Initialize_State(CState* pPreviousState)
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

	m_pActor_ModelCom.lock()->Set_Animation("Battle00|B_AtkBurst04_0", 1.15f, false, true, 2.f);

	shared_ptr< CAttackCollider> pCollider;
	m_pGameInstance->Add_CloneObject<CAttackCollider>(m_pGameInstance->Get_CreateLevelIndex(), L_EFFECT, L"Prototype_GameObject_AttackCollider", nullptr, &pCollider);
	m_pAttackCollider = pCollider;
	m_pAttackCollider.lock()->Get_TransformCom().lock()->Set_Scaling(2.f, 2.f, 6.f);

	return S_OK;
}

void CState_Aerith_AbilityAttack_LustrousShield::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);

	// 방패 객체 특정프레임에 소환하도록 여기서 지정
	// 딱 한번만 투사체 생성
	if (m_pActor_ModelCom.lock()->IsAnimation_Frame_Once(10.f))
	{
		Create_Shield(fTimeDelta);
	}

	/*if (m_pActor_ModelCom.lock()->IsAnimation_Range(35.f, 38.f))
	{
		_float3 vOffset = m_pAttackCollider.lock()->Get_PhysXColliderLocalOffset();
		m_pAttackCollider.lock()->Set_PhysXColliderLocalOffset({ 0.f, vOffset.y, 2.f });

		m_pAttackCollider.lock()->Get_PhysXColliderCom().lock()->WakeUp();
	}
	else if (m_pActor_ModelCom.lock()->IsAnimation_Range(40.f, 43.f))
	{
		if (!m_pAttackCollider.expired())
		{
			m_pAttackCollider.lock()->Set_Dead();
			m_pAttackCollider.lock() = nullptr;
		}
	}
	if (m_pAttackCollider.lock())
	{
		m_pAttackCollider.lock()->Get_TransformCom().lock()->Set_State(CTransform::STATE_POSITION, m_pActor_TransformCom.lock()->Get_State(CTransform::STATE_POSITION));
		m_pAttackCollider.lock()->Get_TransformCom().lock()->Set_Look(m_pActor_TransformCom.lock()->Get_State(CTransform::STATE_LOOK));
	}*/

	if (m_pActor_ModelCom.lock()->IsAnimation_Finished())
		m_pStateMachineCom.lock()->Enter_State<CState_Aerith_Idle>();
}

void CState_Aerith_AbilityAttack_LustrousShield::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CState_Aerith_AbilityAttack_LustrousShield::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CState_Aerith_AbilityAttack_LustrousShield::Transition_State(CState* pNextState)
{
	__super::Transition_State(pNextState);
	if (!m_pAttackCollider.expired())
	{
		m_pAttackCollider.lock()->Set_Dead();
		m_pAttackCollider.lock() = nullptr;
	}
}

bool CState_Aerith_AbilityAttack_LustrousShield::isValid_NextState(CState* state)
{
	return true;
}

void CState_Aerith_AbilityAttack_LustrousShield::Free()
{
}

void CState_Aerith_AbilityAttack_LustrousShield::Create_Shield(_cref_time fTimeDelta)
{
	shared_ptr<CAerith_LustrousShield> pBullet = { nullptr };
	m_pGameInstance->Add_CloneObject(m_pGameInstance->Get_CreateLevelIndex(), L_OBJECT,
		TEXT("Prototype_GameObject_Aerith_LustrousShield"), nullptr, &pBullet);
	pBullet->Set_Owner(m_pActor);
	pBullet->Set_StatusComByOwner("Aerith_NormalAttack1");
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
