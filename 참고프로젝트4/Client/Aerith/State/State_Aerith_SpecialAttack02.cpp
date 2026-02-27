#include "stdafx.h"
#include "Aerith/State/State_List_Aerith.h"

CState_Aerith_SpecialAttack02::CState_Aerith_SpecialAttack02(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine)
	: BASE(pActor, pStatemachine)
{
}

HRESULT CState_Aerith_SpecialAttack02::Initialize_State(CState* pPreviousState)
{
	__super::Initialize_State(pPreviousState);

	if (m_TargetInfo.pTarget.lock() == nullptr || m_TargetInfo.pTarget.expired())
		m_TargetInfo = GET_SINGLE(CClient_Manager)->Get_TargetMonster(AERITH);

	if (m_TargetInfo.pTarget.lock() != nullptr)
		m_pActor_TransformCom.lock()->Look_At_OnLand(m_TargetInfo.vTargetPos);
	
	m_pActor_ModelCom.lock()->Set_Animation("Battle00|B_SAttack01_1", 1.15f, true, true, 2.f,false);

	m_bMouseRelease = false;
	m_bMouseDown = false;
	m_fChargeGauge.Reset();
	m_bTempest = false;

	return S_OK;
}

void CState_Aerith_SpecialAttack02::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);

	if (!m_bMouseRelease)
		m_bMouseRelease = m_pGameInstance->Key_Up(DIK_Q);
	if (!m_bMouseDown)
		m_bMouseDown = m_pGameInstance->Key_Pressing(DIK_Q);

	// Â÷Â¡ ÀÌÆåÆ® »ý¼º
	if (m_fChargeGauge.Increase_MaxOnce(fTimeDelta))
	{
		m_bTempest = true;
		// ÀÌÆåÆ®
	}

	// Â÷Â¡¼¦
	if (m_bMouseRelease)
		m_pStateMachineCom.lock()->Enter_State<CState_Aerith_SpecialAttack_Tempest>();
}

void CState_Aerith_SpecialAttack02::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CState_Aerith_SpecialAttack02::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CState_Aerith_SpecialAttack02::Transition_State(CState* pNextState)
{
	__super::Transition_State(pNextState);

	static_cast<CState_Player*>(pNextState)->Set_TargetInfo(m_TargetInfo);
}

bool CState_Aerith_SpecialAttack02::isValid_NextState(CState* state)
{
	return true;
}

void CState_Aerith_SpecialAttack02::Free()
{
}
