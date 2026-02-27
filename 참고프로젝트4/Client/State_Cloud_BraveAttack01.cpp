#include "stdafx.h"
#include "State_list_Cloud.h"

#include "WeaponPart.h"

CState_Cloud_BraveAttack01::CState_Cloud_BraveAttack01(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine)
	:CState_Cloud(pActor, pStatemachine)
{
}

HRESULT CState_Cloud_BraveAttack01::Initialize_State(CState* pPreviousState)
{
	__super::Initialize_State(pPreviousState);

	if (m_bPlayable || !m_TargetInfo.pTarget.lock())
		m_TargetInfo = GET_SINGLE(CClient_Manager)->Get_TargetMonster(CLOUD);

	if (m_TargetInfo.pTarget.lock() != nullptr)
	{
		m_pActor_TransformCom.lock()->Look_At_OnLand(m_TargetInfo.vTargetPos);
		GET_SINGLE(CCamera_Manager)->Get_ThirdPersonCamera(CLOUD).lock()->Turn_To_Look(m_pActor_TransformCom.lock()->Get_State(CTransform::STATE_LOOK));

		m_pActor_ModelCom.lock()->Set_Animation("Battle01|B_SAttack01_1", 1.15f, false,true, 2.f);
		m_iAttackType = 1;
		TurnOn_MotionBlur();
	}
	else
	{
		m_pActor_ModelCom.lock()->Set_Animation("Battle01|B_SAttack01_0", 1.15f, false, true, 2.f);
		m_iAttackType = 0;
		TurnOn_MotionBlur();
	}

	m_bMousePress = true;
	m_bMouseDown = false;

	// 무기설정
	auto pWeapon = DynPtrCast<CWeaponPart>(m_pActor.lock()->Find_PartObject(TEXT("Part_Weapon")));
	if (pWeapon) { pWeapon->Set_SkillName("Cloud_BraveAttack1"); }

	TurnOn_WeaponEffect();

	m_bNextAttack = !m_bPlayable && (STATE_CLOUD::eVirtualActionKey == STATE_CLOUD::BRAVE_ATTACK2 || STATE_CLOUD::eVirtualActionKey == STATE_CLOUD::BRAVE_ATTACK3 || STATE_CLOUD::eVirtualActionKey == STATE_CLOUD::BRAVE_ATTACK4);

	return S_OK;
}

void CState_Cloud_BraveAttack01::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);



	if (m_bMousePress)
		m_bMousePress = !m_pGameInstance->Mouse_Up(DIM_LB);
	if(!m_bMouseDown)
		m_bMouseDown = m_pGameInstance->Mouse_Down(DIM_LB);

	switch (m_iAttackType)
	{
	case 0:
		TurnOff_MotionBlur(22.f);
		TurnOnOff_Weapon(21.f, 33.f,true);
		TurnOnOff_Weapon(34.f, 44.f,true);
		TurnOnOff_Weapon(49.f, 52.f,true);

	if (((m_bNextAttack && STATE_CLOUD::eVirtualActionKey == STATE_CLOUD::BRAVE_ATTACK4) || m_bMousePress) && m_pActor_ModelCom.lock()->IsAnimation_Range(36.f, 38.f))
		m_pStateMachineCom.lock()->Enter_State<CState_Cloud_BraveAttack04>();
	else if((m_bNextAttack || m_bMouseDown) && m_pActor_ModelCom.lock()->IsAnimation_Range(52.f,54.f))
		m_pStateMachineCom.lock()->Enter_State<CState_Cloud_BraveAttack02>();
	else if (m_pActor_ModelCom.lock()->IsAnimation_Finished())
		m_pStateMachineCom.lock()->Enter_State<CState_Cloud_Idle>();
	
	break;

	case 1:
		TurnOff_MotionBlur(22.f);
		TurnOnOff_Weapon(21.f, 28.f,true);
		TurnOnOff_Weapon(37.f, 42.f,true);

		if (((m_bNextAttack && STATE_CLOUD::eVirtualActionKey == STATE_CLOUD::BRAVE_ATTACK4) || m_bMousePress) && m_pActor_ModelCom.lock()->IsAnimation_Range(35.f, 37.f))
			m_pStateMachineCom.lock()->Enter_State<CState_Cloud_BraveAttack04>();
		else if ((m_bNextAttack || m_bMouseDown) && m_pActor_ModelCom.lock()->IsAnimation_Range(41.f, 43.f))
			m_pStateMachineCom.lock()->Enter_State<CState_Cloud_BraveAttack02>();
		else if (m_pActor_ModelCom.lock()->IsAnimation_Finished())
			m_pStateMachineCom.lock()->Enter_State<CState_Cloud_Idle>();
		
	break;

	default:
		break;
	}
}

void CState_Cloud_BraveAttack01::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CState_Cloud_BraveAttack01::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CState_Cloud_BraveAttack01::Transition_State(CState* pNextState)
{
	__super::Transition_State(pNextState);

	static_pointer_cast<CCharacter>(m_pActor.lock())->TurnOff_Weapon();
	static_cast<CState_Player*>(pNextState)->Set_TargetInfo(m_TargetInfo);

	TurnOff_WeaponEffect();
	TurnOff_MotionBlur();
}

bool CState_Cloud_BraveAttack01::isValid_NextState(CState* state)
{
	return true;
}

void CState_Cloud_BraveAttack01::Free()
{
}
