#include "stdafx.h"
#include "State_list_Cloud.h"

#include "WeaponPart.h"

CState_Cloud_AssertAttack05::CState_Cloud_AssertAttack05(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine)
	:CState_Cloud(pActor, pStatemachine)
{
}

HRESULT CState_Cloud_AssertAttack05::Initialize_State(CState* pPreviousState)
{
	__super::Initialize_State(pPreviousState);

	if (m_TargetInfo.pTarget.lock() == nullptr || m_TargetInfo.pTarget.expired())
		m_TargetInfo = GET_SINGLE(CClient_Manager)->Get_TargetMonster(CLOUD);

	if (m_TargetInfo.pTarget.lock() != nullptr || m_TargetInfo.pTarget.expired())
		m_pActor_TransformCom.lock()->Look_At_OnLand(m_TargetInfo.vTargetPos);

	m_pActor_ModelCom.lock()->Set_Animation("Battle00|B_NAttack05_0", 1.15f, false, true, 2.f);

	// 무기설정
	auto pWeapon = DynPtrCast<CWeaponPart>(m_pActor.lock()->Find_PartObject(TEXT("Part_Weapon")));
	if (pWeapon) { pWeapon->Set_SkillName("Cloud_NormalAttack5"); }

	TurnOn_WeaponEffect();

	return S_OK;
}

void CState_Cloud_AssertAttack05::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);

	TurnOnOff_Weapon(1.f, 28.f,true);

	if (m_pActor_ModelCom.lock()->IsAnimation_Finished())
	{
		m_pStateMachineCom.lock()->Enter_State<CState_Cloud_Idle>();
		GET_SINGLE(CCamera_Manager)->Set_ThirdCam_OffsetType(OFFSET_TYPE::TYPE_ATTACK1);
	}
}

void CState_Cloud_AssertAttack05::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CState_Cloud_AssertAttack05::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CState_Cloud_AssertAttack05::Transition_State(CState* pNextState)
{
	__super::Transition_State(pNextState);

	static_cast<CState_Player*>(pNextState)->Set_TargetInfo(m_TargetInfo);

	TurnOff_Weapon();

	TurnOff_WeaponEffect();
}

bool CState_Cloud_AssertAttack05::isValid_NextState(CState* state)
{
	return true;
}

void CState_Cloud_AssertAttack05::Free()
{
}
