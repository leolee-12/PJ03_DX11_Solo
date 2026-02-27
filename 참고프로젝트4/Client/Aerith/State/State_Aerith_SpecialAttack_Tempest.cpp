#include "stdafx.h"
#include "Aerith/State/State_List_Aerith.h"

#include "Aerith/Weapon/Aerith_TempestBullet.h"
#include "PartObject.h"

CState_Aerith_SpecialAttack_Tempest::CState_Aerith_SpecialAttack_Tempest(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine)
	: BASE(pActor, pStatemachine)
{
}

HRESULT CState_Aerith_SpecialAttack_Tempest::Initialize_State(CState* pPreviousState)
{
	__super::Initialize_State(pPreviousState);

	if (m_TargetInfo.pTarget.lock() == nullptr || m_TargetInfo.pTarget.expired())
		m_TargetInfo = GET_SINGLE(CClient_Manager)->Get_TargetMonster(AERITH);

	if (m_TargetInfo.pTarget.lock() != nullptr)
		m_pActor_TransformCom.lock()->Look_At_OnLand(m_TargetInfo.vTargetPos);
	
	m_pActor_ModelCom.lock()->Set_Animation("Battle00|B_SAttack01_3", 1.15f, false, true, 2.f,false);

	return S_OK;
}

void CState_Aerith_SpecialAttack_Tempest::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);

	// 딱 한번만 투사체 생성
	if (m_pActor_ModelCom.lock()->IsAnimation_Frame_Once(30.f))
	{
		Create_Tempest(fTimeDelta);
	}

	// 상태 전환
	if (m_pActor_ModelCom.lock()->IsAnimation_Finished())
		m_pStateMachineCom.lock()->Enter_State<CState_Aerith_Idle>();
}

void CState_Aerith_SpecialAttack_Tempest::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CState_Aerith_SpecialAttack_Tempest::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CState_Aerith_SpecialAttack_Tempest::Transition_State(CState* pNextState)
{
	__super::Transition_State(pNextState);

	static_cast<CState_Player*>(pNextState)->Set_TargetInfo(m_TargetInfo);
}

bool CState_Aerith_SpecialAttack_Tempest::isValid_NextState(CState* state)
{
	return true;
}

void CState_Aerith_SpecialAttack_Tempest::Free()
{
}

void CState_Aerith_SpecialAttack_Tempest::Create_Tempest(_cref_time fTimeDelta)
{
	shared_ptr<CAerith_TempestBullet> pBullet = { nullptr };
	m_pGameInstance->Add_CloneObject(m_pGameInstance->Get_CreateLevelIndex(), L_OBJECT,
		TEXT("Prototype_GameObject_Aerith_TempestBullet"), nullptr, &pBullet);
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
