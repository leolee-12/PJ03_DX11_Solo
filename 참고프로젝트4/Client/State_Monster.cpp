#include "stdafx.h"
#include "State_Monster.h"
#include "GameInstance.h"
#include "Camera_Manager.h"
#include "Client_Manager.h"
#include "StatusComp.h"

CState_Monster::CState_Monster(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine)
	:CState(pActor, pStatemachine)
{
}

HRESULT CState_Monster::Initialize_State(CState* pPreviousState)
{
	__super::Initialize_State(pPreviousState);

	//auto& pDesc = GET_SINGLE(CClient_Manager)->Find_TargetPlayer(m_pActor);
	//m_pTarget = pDesc.pTarget.lock();
	//m_fDistance = pDesc.fDistance;
	//m_vTargetPos = pDesc.vTargetPos;
	//m_vDirToTarget = pDesc.vDirToTarget;

	return S_OK;
}

void CState_Monster::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);

	auto& pDesc = GET_SINGLE(CClient_Manager)->Find_TargetPlayer(m_pActor);
	m_pTarget = pDesc.pTarget.lock();
	m_fDistance = pDesc.fDistance;
	m_vTargetPos = pDesc.vTargetPos;
	m_vDirToTarget = pDesc.vDirToTarget;
	m_vPos = m_pActor_TransformCom.lock()->Get_State(CTransform::STATE_POSITION);
}

void CState_Monster::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);
	
	
}

void CState_Monster::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CState_Monster::Transition_State(CState* pNextState)
{
	__super::Transition_State(pNextState);

	DynPtrCast<IStatusInterface>(m_pActor.lock())->Get_StatusCom().lock()->Reset_ActionPower();

}

bool CState_Monster::isValid_NextState(CState* state)
{
	return true;
}

void CState_Monster::Free()
{
}
