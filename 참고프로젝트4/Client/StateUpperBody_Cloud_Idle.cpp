#include "stdafx.h"
#include "State_List_Cloud.h"
#include "StatusComp.h"

CStateUpperBody_Cloud_Idle::CStateUpperBody_Cloud_Idle(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine)
	:BASE(pActor, pStatemachine)
{
}

HRESULT CStateUpperBody_Cloud_Idle::Initialize_State(CState* pPreviousState)
{
	__super::Initialize_State(pPreviousState);

	// 페이드 아웃 효과로 블렌드 시키며 애니메이션을 종료
	m_pActor_ModelCom.lock()->Get_AnimationComponent()->Exit_MaskAnim(0);

	DynPtrCast<CCharacter>(m_pActor.lock())->Get_StatusCom().lock()->Set_Guard(false);

	return S_OK;
}

void CStateUpperBody_Cloud_Idle::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
}

void CStateUpperBody_Cloud_Idle::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CStateUpperBody_Cloud_Idle::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CStateUpperBody_Cloud_Idle::Transition_State(CState* pNextState)
{
	__super::Transition_State(pNextState);
}

bool CStateUpperBody_Cloud_Idle::isValid_NextState(CState* state)
{
	return true;
}


void CStateUpperBody_Cloud_Idle::Free()
{
}
