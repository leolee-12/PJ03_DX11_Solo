#include "stdafx.h"
#include "Aerith/State/State_List_Aerith.h"

CState_Aerith_SpecialAttack01::CState_Aerith_SpecialAttack01(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine)
	: BASE(pActor, pStatemachine)
{
}

HRESULT CState_Aerith_SpecialAttack01::Initialize_State(CState* pPreviousState)
{
	__super::Initialize_State(pPreviousState);

	if (m_bPlayable || !m_TargetInfo.pTarget.lock())
		m_TargetInfo = GET_SINGLE(CClient_Manager)->Get_TargetMonster(AERITH);

	if (m_TargetInfo.pTarget.lock() != nullptr)
	{
		m_pActor_TransformCom.lock()->Look_At_OnLand(m_TargetInfo.vTargetPos);
		GET_SINGLE(CCamera_Manager)->Get_ThirdPersonCamera(AERITH).lock()->Turn_To_Look(m_pActor_TransformCom.lock()->Get_State(CTransform::STATE_LOOK));
	}
	
	m_pActor_ModelCom.lock()->Set_Animation("Battle00|B_SAttack01_0", 1.15f, false, true, 2.f);

	m_bMousePress = true;
	m_bMouseDown = false;
	m_bMouseRelease = false;
	
	return S_OK;
}

void CState_Aerith_SpecialAttack01::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);

	if (!m_bMouseRelease)
		m_bMouseRelease = m_pGameInstance->Key_Up(DIK_Q);
	if (!m_bMouseDown && (m_pActor_ModelCom.lock()->IsAnimation_UpTo(6.f)))
		m_bMouseDown = m_pGameInstance->Key_Pressing(DIK_Q);

	if (!m_bPlayable)
		m_bMouseRelease = false;	

	// 애니메이션 끝나면 템페스트 대기로
	if ((!m_bPlayable && STATE_AERITH::eVirtualActionKey == STATE_AERITH::SPECIAL_ATTACK2) || (m_pActor_ModelCom.lock()->IsAnimation_Finished()))
		m_pStateMachineCom.lock()->Enter_State<CState_Aerith_SpecialAttack02>();
	else if ((!m_bPlayable && STATE_AERITH::eVirtualActionKey == STATE_AERITH::SPECIAL_ATTACK3) || (m_bMouseRelease && m_pActor_ModelCom.lock()->IsAnimation_Frame_Once(24.f)))
		m_pStateMachineCom.lock()->Enter_State<CState_Aerith_SpecialAttack03>();
}

void CState_Aerith_SpecialAttack01::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CState_Aerith_SpecialAttack01::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CState_Aerith_SpecialAttack01::Transition_State(CState* pNextState)
{
	__super::Transition_State(pNextState);

	//static_pointer_cast<CCharacter>(m_pActor.lock())->TurnOff_Weapon();
	static_cast<CState_Player*>(pNextState)->Set_TargetInfo(m_TargetInfo);
}

bool CState_Aerith_SpecialAttack01::isValid_NextState(CState* state)
{
	return true;
}

void CState_Aerith_SpecialAttack01::Free()
{
}
