#include "stdafx.h"
#include "Aerith/State/State_List_Aerith.h"

#include "Aerith/Weapon/Aerith_NormalBullet.h"
#include "PartObject.h"

CState_Aerith_SpecialAttack03::CState_Aerith_SpecialAttack03(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine)
	: BASE(pActor, pStatemachine)
{
}

HRESULT CState_Aerith_SpecialAttack03::Initialize_State(CState* pPreviousState)
{
	__super::Initialize_State(pPreviousState);

	GET_SINGLE(CCamera_Manager)->Set_ThirdCam_OffsetType(OFFSET_TYPE::TYPE_ATTACK2);

	if (m_TargetInfo.pTarget.lock() == nullptr || m_TargetInfo.pTarget.expired())
		m_TargetInfo = GET_SINGLE(CClient_Manager)->Get_TargetMonster(AERITH);

	if (m_TargetInfo.pTarget.lock() != nullptr)
		m_pActor_TransformCom.lock()->Look_At_OnLand(m_TargetInfo.vTargetPos);

	m_pActor_ModelCom.lock()->Set_Animation("Battle00|B_SAttack01_2", 1.15f, false, true, 2.f);

	return S_OK;
}

void CState_Aerith_SpecialAttack03::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
	
	// 특정 프레임에 투사체 3개 생성
	if (m_pActor_ModelCom.lock()->IsAnimation_Frame_Once(5.f)
		|| m_pActor_ModelCom.lock()->IsAnimation_Frame_Once(8.f)
		|| m_pActor_ModelCom.lock()->IsAnimation_Frame_Once(11.f))
	{
		Create_Bullet(fTimeDelta);
	}

	// 애니메이션 끝나면 돌아감
	if (m_pActor_ModelCom.lock()->IsAnimation_Finished())
	{
		m_pStateMachineCom.lock()->Enter_State<CState_Aerith_Idle>();
		GET_SINGLE(CCamera_Manager)->Set_ThirdCam_OffsetType(OFFSET_TYPE::TYPE_ATTACK1);
	}
}

void CState_Aerith_SpecialAttack03::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CState_Aerith_SpecialAttack03::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CState_Aerith_SpecialAttack03::Transition_State(CState* pNextState)
{
	__super::Transition_State(pNextState);

	static_cast<CState_Player*>(pNextState)->Set_TargetInfo(m_TargetInfo);
}

bool CState_Aerith_SpecialAttack03::isValid_NextState(CState* state)
{
	return true;
}

void CState_Aerith_SpecialAttack03::Free()
{
}

void CState_Aerith_SpecialAttack03::Create_Bullet(_cref_time fTimeDelta)
{
	shared_ptr<CAerith_NormalBullet> pBullet = { nullptr };
	m_pGameInstance->Add_CloneObject(m_pGameInstance->Get_CreateLevelIndex(), L_OBJECT,
		TEXT("Prototype_GameObject_Aerith_NormalBullet"), nullptr, &pBullet);
	pBullet->Set_Owner(m_pActor);
	pBullet->Set_StatusComByOwner("Aerith_SpecialAttack");
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
