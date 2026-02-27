#include "stdafx.h"
#include "State_list_Cloud.h"

#include "WeaponPart.h"

CState_Cloud_AssertAttack01::CState_Cloud_AssertAttack01(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine)
	:CState_Cloud(pActor, pStatemachine)
{
}

HRESULT CState_Cloud_AssertAttack01::Initialize_State(CState* pPreviousState)
{
	__super::Initialize_State(pPreviousState);
	if(m_bPlayable || !m_TargetInfo.pTarget.lock())
		m_TargetInfo = GET_SINGLE(CClient_Manager)->Get_TargetMonster(CLOUD);

	if (m_TargetInfo.pTarget.lock() != nullptr)
	{
		m_pActor_TransformCom.lock()->Look_At_OnLand(m_TargetInfo.vTargetPos);
		GET_SINGLE(CCamera_Manager)->Get_ThirdPersonCamera(CLOUD).lock()->Turn_To_Look(m_pActor_TransformCom.lock()->Get_State(CTransform::STATE_LOOK));

		if (InRange(m_TargetInfo.fDistance, 0.f, 3.f) || m_TargetInfo.fDistance >= 12.f)
		{
			m_pActor_ModelCom.lock()->Set_Animation("Battle00|B_NAttack01_0", 1.15f, false, true, 2.f);
			m_iAttackType = 0;
			TurnOn_MotionBlur();
		}
		else if (InRange(m_TargetInfo.fDistance, 3.f, 6.f))
		{
			m_pActor_ModelCom.lock()->Set_Animation("Battle00|B_NAttack01_1", 1.15f, false, true, 2.f);
			m_iAttackType = 1;
			TurnOn_MotionBlur();
		}
		else if (InRange(m_TargetInfo.fDistance, 6.f, 12.f))
		{
			m_pActor_ModelCom.lock()->Set_Animation("Battle00|B_NAttack01_1_0", 1.15f, false, true, 2.f);
			m_iAttackType = 2;
			TurnOn_MotionBlur();
		}
	}
	else
	{
		m_pActor_ModelCom.lock()->Set_Animation("Battle00|B_NAttack01_0", 1.15f, false, true, 2.f);
		m_iAttackType = 0;
	}

	// 무기설정
	auto pWeapon = DynPtrCast<CWeaponPart>(m_pActor.lock()->Find_PartObject(TEXT("Part_Weapon")));
	if (pWeapon) { pWeapon->Set_SkillName("Cloud_NormalAttack1"); }

	// 입력설정
	m_bMousePress = true;
	m_bMouseDown = false;

	TurnOn_WeaponEffect();

	m_bNextAttack = !m_bPlayable && (STATE_CLOUD::eVirtualActionKey == STATE_CLOUD::ASSERT_ATTACK2 || STATE_CLOUD::eVirtualActionKey == STATE_CLOUD::ASSERT_ATTACK3 || STATE_CLOUD::eVirtualActionKey == STATE_CLOUD::ASSERT_ATTACK4 || STATE_CLOUD::eVirtualActionKey == STATE_CLOUD::ASSERT_ATTACK5);


	return S_OK;
}

void CState_Cloud_AssertAttack01::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
	

	if (m_bMousePress)
		m_bMousePress = !m_pGameInstance->Mouse_Up(DIM_LB);
	if (!m_bMouseDown)
		m_bMouseDown = m_pGameInstance->Mouse_Down(DIM_LB);

	switch (m_iAttackType)
	{
	case 0:
		TurnOnOff_Weapon(3.f, 14.f,true);
		TurnOff_MotionBlur(7.f);
	if((m_bMousePress|| m_bMouseDown || m_bNextAttack) && m_pActor_ModelCom.lock()->IsAnimation_Range(13.f,20.f))
		m_pStateMachineCom.lock()->Enter_State<CState_Cloud_AssertAttack02>();
	else if (m_pActor_ModelCom.lock()->IsAnimation_Finished())
		m_pStateMachineCom.lock()->Enter_State<CState_Cloud_Idle>();
	
	break;

	case 1:
		TurnOnOff_Weapon(10.f, 20.f,true);
		TurnOff_MotionBlur(15.f);

	if ((m_bMousePress || m_bMouseDown || m_bNextAttack) && m_pActor_ModelCom.lock()->IsAnimation_Range(20.f, 30.f))
		m_pStateMachineCom.lock()->Enter_State<CState_Cloud_AssertAttack02>();
	else if (m_pActor_ModelCom.lock()->IsAnimation_Finished())
		m_pStateMachineCom.lock()->Enter_State<CState_Cloud_Idle>();
		
	break;

	case 2:
		if (m_pActor_ModelCom.lock()->IsAnimation_Finished("Battle00|B_NAttack01_1_0"))
			m_pActor_ModelCom.lock()->Set_Animation("Battle00|B_NAttack01_1_1", 1.15f, false, false, 2.f, false, false);
		else if (m_pActor_ModelCom.lock()->IsAnimation_Finished("Battle00|B_NAttack01_1_1"))
			m_pActor_ModelCom.lock()->Set_Animation("Battle00|B_NAttack01_1_2", 1.15f, false, false, 2.f, false, false);
		else if (m_pActor_ModelCom.lock()->IsCurrentAnimation("Battle00|B_NAttack01_1_2"))
		{
			TurnOnOff_Weapon(1.f, 8.f,true);
			TurnOff_MotionBlur(3.f);
			if ((m_bMousePress || m_bMouseDown || m_bNextAttack) && m_pActor_ModelCom.lock()->IsAnimation_Range(6.f, 14.f))
				m_pStateMachineCom.lock()->Enter_State<CState_Cloud_AssertAttack02>();
			else if (m_pActor_ModelCom.lock()->IsAnimation_Finished("Battle00|B_NAttack01_1_2"))
				m_pStateMachineCom.lock()->Enter_State<CState_Cloud_Idle>();
		}
		break;

	default:
		break;
	}
}

void CState_Cloud_AssertAttack01::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CState_Cloud_AssertAttack01::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CState_Cloud_AssertAttack01::Transition_State(CState* pNextState)
{
	__super::Transition_State(pNextState);

	static_pointer_cast<CCharacter>(m_pActor.lock())->TurnOff_Weapon();
	static_cast<CState_Player*>(pNextState)->Set_TargetInfo(m_TargetInfo);

	TurnOff_WeaponEffect();
	TurnOff_MotionBlur();
}

bool CState_Cloud_AssertAttack01::isValid_NextState(CState* state)
{
	return true;
}

void CState_Cloud_AssertAttack01::Free()
{
}
