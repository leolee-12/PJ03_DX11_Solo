#include "stdafx.h"
#include "Aerith/State/State_List_Aerith.h"

#include "Aerith/Weapon/Aerith_List_Weapon.h"
#include "PartObject.h"

CState_Aerith_AbilityAttack_RayOfJudge::CState_Aerith_AbilityAttack_RayOfJudge(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine)
	: BASE(pActor, pStatemachine)
{

	// 노티파이에 공격 이벤트 연결
	auto pModelCom = pActor->Get_ModelCom().lock();
	if (pModelCom)
	{
		pModelCom->Get_AnimationComponent()
			->Regist_EventToNotify("Battle00|B_AtkBurst05_0", "Event", "Laser", 
				MakeDelegate(this, &CState_Aerith_AbilityAttack_RayOfJudge::Create_Ray));
	}
}

HRESULT CState_Aerith_AbilityAttack_RayOfJudge::Initialize_State(CState* pPreviousState)
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

	// 레이저 전용 애니메이션 필요
	m_pActor_ModelCom.lock()->Set_Animation("Battle00|B_AtkBurst05_0", 1.15f, false, true, 2.f);

	/*shared_ptr<CAttackCollider> pCollider;
	m_pGameInstance->Add_CloneObject<CAttackCollider>(m_pGameInstance->Get_CreateLevelIndex(), L_EFFECT, L"Prototype_GameObject_AttackCollider", nullptr, &pCollider);
	m_pAttackCollider = pCollider;
	m_pAttackCollider.lock()->Get_TransformCom().lock()->Set_Scaling(4.f, 2.f, 4.f);*/

	return S_OK;
}

void CState_Aerith_AbilityAttack_RayOfJudge::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);

	// 노티파이에 이펙트 붙이기
	// 발사부분 + 실제 레이저
	// 레이객체 방향 지정 기능이 여기에서 쓰임.
	/*if (m_pActor_ModelCom.lock()->IsAnimation_Frame_Once(10.f))
	{
		Create_Ray();
	}*/

	/*if (m_pActor_ModelCom.lock()->IsAnimation_Range(88.f, 91.f))
	{
		_float3 vOffset = m_pAttackCollider.lock()->Get_PhysXColliderLocalOffset();
		m_pAttackCollider.lock()->Set_PhysXColliderLocalOffset({ 0.f, vOffset.y, 1.f });

		m_pAttackCollider.lock()->Get_PhysXColliderCom().lock()->WakeUp();
	}
	else if (m_pActor_ModelCom.lock()->IsAnimation_Range(92.f, 95.f))
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
		m_pAttackCollider.lock()->Get_TransformCom().lock()->Set_Look( m_pActor_TransformCom.lock()->Get_State(CTransform::STATE_LOOK));
	}*/

	if (m_pActor_ModelCom.lock()->IsAnimation_Finished())
		m_pStateMachineCom.lock()->Enter_State<CState_Aerith_Idle>();
}

void CState_Aerith_AbilityAttack_RayOfJudge::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CState_Aerith_AbilityAttack_RayOfJudge::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CState_Aerith_AbilityAttack_RayOfJudge::Transition_State(CState* pNextState)
{
	__super::Transition_State(pNextState);
	/*if (!m_pAttackCollider.expired())
	{
		m_pAttackCollider.lock()->Set_Dead();
		m_pAttackCollider.lock() = nullptr;
	}*/
}

bool CState_Aerith_AbilityAttack_RayOfJudge::isValid_NextState(CState* state)
{
	return true;
}

void CState_Aerith_AbilityAttack_RayOfJudge::Free()
{
}

void CState_Aerith_AbilityAttack_RayOfJudge::Create_Ray()
{
	shared_ptr<CAerith_RayOfJudge> pBullet = { nullptr };
	m_pGameInstance->Add_CloneObject(m_pGameInstance->Get_CreateLevelIndex(), L_OBJECT,
		TEXT("Prototype_GameObject_Aerith_RayOfJudge"), nullptr, &pBullet);
	pBullet->Set_Owner(m_pActor);
	auto pPart = m_pActor.lock()->Find_PartObject(TEXT("Part_Weapon"));
	if (pPart)
	{
		_matrix SocketMatrix = pPart->Get_WorldMatrixFromBone(TEXT("R_RodAGimmickS_End"));
		_matrix ActorMatrix = m_pActor.lock()->Get_TransformCom().lock()->Get_WorldFloat4x4();
		//SocketMatrix *= ActorMatrix;

		pBullet->Get_TransformCom().lock()->Set_Look_Manual(ActorMatrix.r[2]);
		pBullet->Get_TransformCom().lock()->Set_Position(0.f, SocketMatrix.r[3]);
	}
}

