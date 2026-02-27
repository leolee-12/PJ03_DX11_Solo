#include "stdafx.h"
#include "State_list_Cloud.h"

#include "Cloud_Weapon.h"

#include "WeaponPart.h"

CState_Cloud_LimitAttack::CState_Cloud_LimitAttack(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine)
	:CState_Cloud(pActor, pStatemachine)
{
}

HRESULT CState_Cloud_LimitAttack::Initialize_State(CState* pPreviousState)
{
	__super::Initialize_State(pPreviousState);

	Check_UseCommand();

	//임시 가까운 몬스터로 타겟팅 (선택으로 변경하기)
	if (m_bPlayable || !m_TargetInfo.pTarget.lock())
		m_TargetInfo = GET_SINGLE(CClient_Manager)->Get_TargetMonster(CLOUD);

	if (m_TargetInfo.pTarget.lock() != nullptr)
	{
		m_pActor_TransformCom.lock()->Look_At_OnLand(m_TargetInfo.vTargetPos);
		GET_SINGLE(CCamera_Manager)->Get_ThirdPersonCamera(CLOUD).lock()->Turn_To_Look(m_pActor_TransformCom.lock()->Get_State(CTransform::STATE_LOOK));
		m_TargetInfo.pTarget.lock()->Get_TransformCom().lock()->Look_At_OnLand(m_pActor_TransformCom.lock()->Get_State(CTransform::STATE_POSITION));
	}
	m_pActor_ModelCom.lock()->Set_Animation("Battle00|B_AtkLimit02_0", 1.3f, false, true, 2.f);

	// 무기설정
	auto pWeapon = DynPtrCast<CWeaponPart>(m_pActor.lock()->Find_PartObject(TEXT("Part_Weapon")));
	if (pWeapon) { pWeapon->Set_SkillName("Cloud_Limit_ClimHazzard_1"); }

	if (!m_bTrailCreate)
	{
		m_bTrailCreate = true;
		Create_Clo();
	}// 처음만 트레일 생성

	GET_SINGLE(CCamera_Manager)->Call_CutSceneCamera("Cloud_Limit_Following", 3, m_pActor.lock(), 0.f);

	return S_OK;
}

void CState_Cloud_LimitAttack::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
	
	TurnOnOff_Weapon(70.f, 82.f,true);
	TurnOnOff_Weapon(92.f, 94.f, true);
	TurnOnOff_Weapon(102.f, 104.f,true);
	TurnOnOff_Weapon(114.f, 116.f,true);
	TurnOnOff_Weapon(139.f, 146.f,true);
	TurnOnOff_Weapon(160.f, 170.f,true);

	//TurnOnOff_Weapon(40.f, 180.f);

	if (m_pActor_ModelCom.lock()->IsAnimation_Frame_Once(25.f))
		Activate_Clo(true);

	if (m_TargetInfo.pTarget.lock() != nullptr)
	{
		GET_SINGLE(CClient_Manager)->Update_TargetInfo(m_pActor, m_TargetInfo);

		if(m_TargetInfo.fDistance <= 0.5f)
			m_pActor_TransformCom.lock()->Set_Move_AnimationPosition(false);
	}
	if (m_pActor_ModelCom.lock()->IsAnimation_Finished())
		m_pStateMachineCom.lock()->Enter_State<CState_Cloud_Idle>();
}

void CState_Cloud_LimitAttack::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);

	// 상시 이펙트 On
	if(m_pActor_ModelCom.lock()->IsAnimation_Frame_Once(25.f))
		DynPtrCast<CCloud_Weapon>(m_pActor.lock()->Find_PartObject(TEXT("Part_Weapon")))->Set_AscensionMode(true);
}

void CState_Cloud_LimitAttack::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CState_Cloud_LimitAttack::Transition_State(CState* pNextState)
{
	__super::Transition_State(pNextState);

	// 상시 이펙트 Off
	DynPtrCast<CCloud_Weapon>(m_pActor.lock()->Find_PartObject(TEXT("Part_Weapon")))->Set_AscensionMode(false);

	m_pActor_TransformCom.lock()->Set_Move_AnimationPosition(true);

	Activate_Clo(false);
}

bool CState_Cloud_LimitAttack::isValid_NextState(CState* state)
{
	return true;
}

void CState_Cloud_LimitAttack::Create_Clo()
{
	for (_uint i = 0; i < 2; i++)
	{
		if (m_pTrail[i] == nullptr)
		{
			m_pTrail[i] = GET_SINGLE(CEffect_Manager)->Create_Effect<CTrail_Buffer>(L"ET_Cloud_Ascension_Trail1",
				m_pActor.lock()->Find_PartObject(TEXT("Part_Weapon")));

			if (m_pTrail[i])
			{
				m_pTrail[i]->Trail_Pos_Reset();
				m_pTrail[i]->TurnOff_State(OBJSTATE::Active);
			}
		}
	}
}

void CState_Cloud_LimitAttack::Activate_Clo(_bool bActivate)
{
	for (_uint i = 0; i < 2; i++)
	{
		if (m_pTrail[i])
		{
			if (bActivate)
			{
				if (!m_pTrail[i]->IsState(OBJSTATE::Active))
				{
					m_pTrail[i]->TurnOn_State(OBJSTATE::Active);
				}
			}
			else {
				if (m_pTrail[i]->IsState(OBJSTATE::Active))
				{
					m_pTrail[i]->Trail_Pos_Reset();
					m_pTrail[i]->TurnOff_State(OBJSTATE::Active);
				}
			}
		}
	}
}

void CState_Cloud_LimitAttack::Clo_Anim_Range(_float fMin, _float fMax)
{
	if (m_pActor_ModelCom.lock()->IsAnimation_Range(fMin, fMax))
	{
		Activate_Clo(true);
	}

	if (m_pActor_ModelCom.lock()->IsAnimation_Frame_Once(fMax + 1.f))
	{
		Activate_Clo(false);
	}
}

void CState_Cloud_LimitAttack::Free()
{
}
