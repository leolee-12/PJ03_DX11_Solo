#include "stdafx.h"
#include "State_list_Cloud.h"
#include "StatusComp.h"

CStateUpperBody_Cloud_Guard::CStateUpperBody_Cloud_Guard(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine)
	:BASE(pActor, pStatemachine)
{
}

HRESULT CStateUpperBody_Cloud_Guard::Initialize_State(CState* pPreviousState)
{
	__super::Initialize_State(pPreviousState);

	auto pMainFSM = m_pStateMachineCom.lock()->Get_RefStateMachine("Main").lock();
	m_strCurBattleMode = DynCast<CState_Cloud*>(pMainFSM->Get_CurState())->Get_CurrentBattleModeName();
	
	// 상체 애니메이션을 재생시킨다. 시작뼈 + 어디까지 적용할지에 대한 뼈들(여러개 가능)
	m_pActor_ModelCom.lock()->Get_AnimationComponent()
		->Set_MaskAnim(0, L"Battle00|B_Guard01_1"
						, TEXT("C_Spine_b"), { TEXT("C_FaceBase_b") }
						, UINT_MAX, 1.f, true, true, FLT_MAX);	

	DynPtrCast<CCharacter>(m_pActor.lock())->Get_StatusCom().lock()->Set_Guard(true);

	return S_OK;
}

void CStateUpperBody_Cloud_Guard::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);

	if (m_pGameInstance->Key_Down(DIK_T))
	{
		auto pMainFSM = m_pStateMachineCom.lock()->Get_RefStateMachine("Main").lock();
		pMainFSM->Set_State<CState_Cloud_GuardRelease>();
	}

	if (m_pActor_ModelCom.lock()->IsAnimation_Finished())
	{
		dynamic_pointer_cast<CCharacter>(m_pActor.lock())->Dead_DissolveType(L"EM_MonsterDead_Num2500");
	}
}

void CStateUpperBody_Cloud_Guard::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CStateUpperBody_Cloud_Guard::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CStateUpperBody_Cloud_Guard::Transition_State(CState* pNextState)
{
	__super::Transition_State(pNextState);

	m_pActor_ModelCom.lock()->Get_AnimationComponent()
		->Exit_MaskAnim(0, FLT_MAX);

}

bool CStateUpperBody_Cloud_Guard::isValid_NextState(CState* state)
{
	return true;
}

void CStateUpperBody_Cloud_Guard::Free()
{
}
