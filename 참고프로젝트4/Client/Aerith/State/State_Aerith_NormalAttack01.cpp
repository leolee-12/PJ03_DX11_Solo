#include "stdafx.h"
#include "Aerith/State/State_List_Aerith.h"

#include "Aerith/Weapon/Aerith_NormalBullet.h"
#include "PartObject.h"

CState_Aerith_NormalAttack01::CState_Aerith_NormalAttack01(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine)
	: BASE(pActor, pStatemachine)
{
	// 노티파이에 공격 이벤트 연결
	auto pModelCom = pActor->Get_ModelCom().lock();
	if (pModelCom)
	{
		pModelCom->Get_AnimationComponent()
			->Regist_EventToNotify("Battle00|B_NAttack01_0", "Event", "Shoot1",
				MakeDelegate(this, &CState_Aerith_NormalAttack01::Create_Bullet));
		pModelCom->Get_AnimationComponent()
			->Regist_EventToNotify("Battle00|B_NAttack01_0", "Event", "Shoot2",
				MakeDelegate(this, &CState_Aerith_NormalAttack01::Create_Bullet));
	}
}

HRESULT CState_Aerith_NormalAttack01::Initialize_State(CState* pPreviousState)
{
	__super::Initialize_State(pPreviousState);

	if (m_bPlayable || !m_TargetInfo.pTarget.lock())
		m_TargetInfo = GET_SINGLE(CClient_Manager)->Get_TargetMonster(AERITH);

	if (m_TargetInfo.pTarget.lock() != nullptr)
	{
		m_pActor_TransformCom.lock()->Look_At_OnLand(m_TargetInfo.vTargetPos);
		GET_SINGLE(CCamera_Manager)->Get_ThirdPersonCamera(AERITH).lock()->Turn_To_Look(m_pActor_TransformCom.lock()->Get_State(CTransform::STATE_LOOK));
	}

	m_pActor_ModelCom.lock()->Set_Animation("Battle00|B_NAttack01_0", 1.15f, false, true, 2.f);

	m_bMousePress = true;
	m_bMouseDown = false;

	m_bNextAttack = !m_bPlayable && (STATE_AERITH::eVirtualActionKey == STATE_AERITH::NORMAL_ATTACK2 || STATE_AERITH::eVirtualActionKey == STATE_AERITH::NORMAL_ATTACK3 || STATE_AERITH::eVirtualActionKey == STATE_AERITH::NORMAL_ATTACK4 || STATE_AERITH::eVirtualActionKey == STATE_AERITH::NORMAL_ATTACK5);

	return S_OK;
}

void CState_Aerith_NormalAttack01::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
	
	// 입력 + 입력 딜레이(이거 안 넣으면 자동으로 2번 공격까지 이어짐.
	if (!m_bMouseDown && m_pActor_ModelCom.lock()->IsAnimation_UpTo(6.f))
		m_bMouseDown = m_pGameInstance->Mouse_Pressing(DIM_LB);

	if (!m_bPlayable)	
		m_bMouseDown = false;	

	// 연계
	if (m_bNextAttack || (m_bMouseDown && m_pActor_ModelCom.lock()->IsAnimation_Range(24.f, 25.f)))
		m_pStateMachineCom.lock()->Enter_State<CState_Aerith_NormalAttack02>();
	else if (m_pActor_ModelCom.lock()->IsAnimation_Finished("Battle00|B_NAttack01_0"))
		m_pStateMachineCom.lock()->Enter_State<CState_Aerith_Idle>();
}

void CState_Aerith_NormalAttack01::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CState_Aerith_NormalAttack01::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CState_Aerith_NormalAttack01::Transition_State(CState* pNextState)
{
	__super::Transition_State(pNextState);

	//static_pointer_cast<CCharacter>(m_pActor.lock())->TurnOff_Weapon();
	static_cast<CState_Player*>(pNextState)->Set_TargetInfo(m_TargetInfo);
}

bool CState_Aerith_NormalAttack01::isValid_NextState(CState* state)
{
	return true;
}

void CState_Aerith_NormalAttack01::Free()
{
}

void CState_Aerith_NormalAttack01::Create_Bullet()
{
	shared_ptr<CAerith_NormalBullet> pBullet = { nullptr };
	m_pGameInstance->Add_CloneObject(m_pGameInstance->Get_CreateLevelIndex(), L_OBJECT,
		TEXT("Prototype_GameObject_Aerith_NormalBullet"), nullptr, &pBullet);
	pBullet->Set_Owner(m_pActor);
	pBullet->Set_StatusComByOwner("Aerith_NormalAttack1");
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