#include "stdafx.h"
#include "Aerith/State/State_List_Aerith.h"

#include "Aerith/Weapon/Aerith_NormalBullet.h"
#include "PartObject.h"

CState_Aerith_NormalAttack05::CState_Aerith_NormalAttack05(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine)
	: BASE(pActor, pStatemachine)
{
	// 노티파이에 공격 이벤트 연결
	auto pModelCom = pActor->Get_ModelCom().lock();
	if (pModelCom)
	{
		pModelCom->Get_AnimationComponent()
			->Regist_EventToNotify("Battle00|B_NAttack05_0", "Event", "Shoot1",
				MakeDelegate(this, &DERIVED::Create_Bullet));
		pModelCom->Get_AnimationComponent()
			->Regist_EventToNotify("Battle00|B_NAttack05_0", "Event", "Shoot2",
				MakeDelegate(this, &DERIVED::Create_Bullet));
		pModelCom->Get_AnimationComponent()
			->Regist_EventToNotify("Battle00|B_NAttack05_0", "Event", "Shoot3",
				MakeDelegate(this, &DERIVED::Create_Bullet));
		pModelCom->Get_AnimationComponent()
			->Regist_EventToNotify("Battle00|B_NAttack05_0", "Event", "Shoot4",
				MakeDelegate(this, &DERIVED::Create_Bullet));
	}
}

HRESULT CState_Aerith_NormalAttack05::Initialize_State(CState* pPreviousState)
{
	__super::Initialize_State(pPreviousState);

	if (m_TargetInfo.pTarget.lock() == nullptr || m_TargetInfo.pTarget.expired())
		m_TargetInfo = GET_SINGLE(CClient_Manager)->Get_TargetMonster(AERITH);

	if (m_TargetInfo.pTarget.lock() != nullptr)
		m_pActor_TransformCom.lock()->Look_At_OnLand(m_TargetInfo.vTargetPos);

	m_pActor_ModelCom.lock()->Set_Animation("Battle00|B_NAttack05_0", 1.15f, false, true, 2.f);

	return S_OK;
}

void CState_Aerith_NormalAttack05::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);

	if (m_pActor_ModelCom.lock()->IsAnimation_Finished())
	{
		m_pStateMachineCom.lock()->Enter_State<CState_Aerith_Idle>();
		GET_SINGLE(CCamera_Manager)->Set_ThirdCam_OffsetType(OFFSET_TYPE::TYPE_ATTACK1);
	}
}

void CState_Aerith_NormalAttack05::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CState_Aerith_NormalAttack05::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CState_Aerith_NormalAttack05::Transition_State(CState* pNextState)
{
	__super::Transition_State(pNextState);

	static_cast<CState_Player*>(pNextState)->Set_TargetInfo(m_TargetInfo);
}

bool CState_Aerith_NormalAttack05::isValid_NextState(CState* state)
{
	return true;
}

void CState_Aerith_NormalAttack05::Free()
{
}

void CState_Aerith_NormalAttack05::Create_Bullet()
{
	shared_ptr<CAerith_NormalBullet> pBullet = { nullptr };
	m_pGameInstance->Add_CloneObject(m_pGameInstance->Get_CreateLevelIndex(), L_OBJECT,
		TEXT("Prototype_GameObject_Aerith_NormalBullet"), nullptr, &pBullet);
	pBullet->Set_Owner(m_pActor);
	pBullet->Set_StatusComByOwner("Aerith_NormalAttack5");
	auto pPart = m_pActor.lock()->Find_PartObject(TEXT("Part_Weapon"));
	if (pPart)
	{
		_matrix SocketMatrix = pPart->Get_WorldMatrixFromBone(TEXT("L_RodAGimmickS_End"));
		_matrix ActorMatrix = m_pActor.lock()->Get_TransformCom().lock()->Get_WorldFloat4x4();
		//SocketMatrix *= ActorMatrix;

		pBullet->Get_TransformCom().lock()->Set_Look_Manual(ActorMatrix.r[2]);
		pBullet->Get_TransformCom().lock()->Set_Position(0.f, SocketMatrix.r[3]);
	}
}