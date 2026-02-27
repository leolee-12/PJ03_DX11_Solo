#include "stdafx.h"
#include "..\Public\Hit_MeleeAttack.h"

#include "RenderInstance.h"
#include "GameInstance.h"
#include "Input.h"

#include "Play_Hit.h"

#include "Effect_Manager.h"

#include "AttackObject.h"
#include "Effect.h"

#include "BattleInterface.h"
//vector<CInput> Command_236Attack =
//{
//	{MOVEKEY_DOWN, ATTACK_NONE},
//	{MOVEKEY_DOWN_RIGHT, ATTACK_NONE},
//	{MOVEKEY_RIGHT, ATTACK_NONE},
//	{MOVEKEY_NEUTRAL, ATTACK_LIGHT}
//};
//vector<CInput> Command_236Attack_Extra =
//{
//	{MOVEKEY_DOWN, ATTACK_NONE},
//	{MOVEKEY_DOWN_RIGHT, ATTACK_NONE},
//	{MOVEKEY_RIGHT, ATTACK_NONE},
//	{MOVEKEY_RIGHT, ATTACK_LIGHT}
//};
//vector<CInput> Command_LightAttack ={	{MOVEKEY_NEUTRAL, ATTACK_LIGHT}};
//vector<CInput> Command_MediumAttack ={	{MOVEKEY_NEUTRAL, ATTACK_MEDIUM}};
//vector<CInput> Command_HeavyAttack ={	{MOVEKEY_NEUTRAL, ATTACK_HEAVY}};
//vector<CInput> Command_SpecialAttack ={	{MOVEKEY_NEUTRAL, ATTACK_SPECIAL}};


#include <iostream>

void CHit_MeleeAttack::Initalize(CPlay_Hit* pPlayer)
{
	m_pPlayer = pPlayer;
	m_pPlayerAnimationIndex = m_pPlayer->Get_pAnimationIndex();


	m_pbAttackCount = m_pPlayer->Get_pbAttackCount();
	//m_piCountGroundSpecial = m_pPlayer->Get_piSpecialCount();

	m_pGameInstance = CGameInstance::Get_Instance();
	m_pEffect_Manager = CEffect_Manager::Get_Instance();

	m_pbCounterPose = static_cast<CPlay_Hit*>(m_pPlayer)->Get_pbCounterPose();

	//	Safe_AddRef(m_pEffect_Manager);
}

void CHit_MeleeAttack::Attack_Light()
{

	this;
	_bool bDebug = false;


	if (m_pPlayer->Check_bCurAnimationisGroundMove())
	{
		m_pPlayer->Set_Animation(CPlay_Hit::ANIME_ATTACK_LIGHT1);
		
	}

	else if (*m_pPlayerAnimationIndex == CPlay_Hit::ANIME_ATTACK_LIGHT1)
	{
		m_pPlayer->Set_NextAnimation(CPlay_Hit::ANIME_ATTACK_LIGHT2,1.f);
	}

	else if (*m_pPlayerAnimationIndex == CPlay_Hit::ANIME_ATTACK_LIGHT2)
	{
		m_pPlayer->Set_NextAnimation(CPlay_Hit::ANIME_ATTACK_LIGHT3, 1.f);
	}

	else if (*m_pPlayerAnimationIndex == CPlay_Hit::ANIME_JUMP_UP || *m_pPlayerAnimationIndex == CPlay_Hit::ANIME_JUMP_DOWN)
	{
		m_pPlayer->Set_Animation(CPlay_Hit::ANIME_ATTACK_AIR1);
	}
	else if (*m_pPlayerAnimationIndex == CPlay_Hit::ANIME_ATTACK_AIR1)
	{
		
		m_pPlayer->Set_NextAnimation(CPlay_Hit::ANIME_ATTACK_AIR2,0.5f);
		
		if (m_pPlayer->Get_fGravityTime() > 0.135)
		{
			m_pPlayer->Set_bNextAnimationGravityEvent();
		}
	}
	else if (*m_pPlayerAnimationIndex == CPlay_Hit::ANIME_ATTACK_AIR2)
	{
		m_pPlayer->Set_NextAnimation(CPlay_Hit::ANIME_ATTACK_AIR3,0.5f);
		if (m_pPlayer->Get_fGravityTime() > 0.135)
		{
			//m_pPlayer->Set_ForcedGravityTime_LittleUp();
			m_pPlayer->Set_bNextAnimationGravityEvent();
		}
	}

	
	//1프레임 차이로 쓰러지려는거 건지는 문제가 있음
	else if (*m_pPlayerAnimationIndex == CPlay_Hit::ANIME_ATTACK_CROUCH_LIGHT && m_pPlayer->Get_bAttackBackEvent())
	{
		//m_pPlayer->Set_Animation(CPlay_Hit::ANIME_ATTACK_LIGHT1);
		m_pPlayer->Set_NextAnimation(CPlay_Hit::ANIME_ATTACK_LIGHT1, 0.5f);

	}
	else if (*m_pPlayerAnimationIndex == CPlay_Hit::ANIME_FORWARD_DASH || *m_pPlayerAnimationIndex == CPlay_Hit::ANIME_FORWARD_DASH_END)
	{
		m_pPlayer->Set_Animation(CPlay_Hit::ANIME_ATTACK_LIGHT1);
	}



	//else  if (*m_pPlayerAnimationIndex == CPlay_Hit::ANIME_236_POSE && m_pPlayer->Get_b236Posing())
	//{
	//	m_pPlayer->Set_Animation(CPlay_Hit::ANIME_236_POSE_LIGHT);
	//	m_pbAttackCount[CPlay_Hit::COUNT_ATTACK_236_LIGHT] = false;
	//}

	else  if (*m_pPlayerAnimationIndex == CPlay_Hit::ANIME_236_POSE && m_pPlayer->Get_b236Posing() && m_pbAttackCount[CPlay_Hit::COUNT_ATTACK_236_LIGHT])
	{
		m_pPlayer->Set_Animation(CPlay_Hit::ANIME_236_POSE_LIGHT);
		m_pbAttackCount[CPlay_Hit::COUNT_ATTACK_236_LIGHT] = false;
		m_pPlayer->Set_b236Special(false);
	}

	

}

void CHit_MeleeAttack::Attack_Medium()
{


	if (m_pPlayer->Check_bCurAnimationisGroundMove(*m_pPlayerAnimationIndex))
	{
		m_pPlayer->Set_Animation(CPlay_Hit::ANIME_ATTACK_MEDIUM);
		m_pbAttackCount[CPlay_Hit::COUNT_ATTACK_MEDIUM] = false;


	}

	else if (m_pPlayer->Get_bAttackBackEvent() && (*m_pPlayerAnimationIndex == CPlay_Hit::ANIME_ATTACK_LIGHT1 || *m_pPlayerAnimationIndex == CPlay_Hit::ANIME_ATTACK_LIGHT2
		|| *m_pPlayerAnimationIndex == CPlay_Hit::ANIME_ATTACK_CROUCH_LIGHT || *m_pPlayerAnimationIndex == CPlay_Hit::ANIME_ATTACK_CROUCH_MEDUIM))
	{
		//서서 중공격 횟수가 남아있으면 일단 사용
		if (m_pbAttackCount[CPlay_Hit::COUNT_ATTACK_MEDIUM] == true)
		{
			m_pPlayer->Set_Animation(CPlay_Hit::ANIME_ATTACK_MEDIUM, 0.5f);
			m_pbAttackCount[CPlay_Hit::COUNT_ATTACK_MEDIUM] = false;
		}

	}
	else if(*m_pPlayerAnimationIndex == CPlay_Hit::ANIME_ATTACK_LIGHT1 || *m_pPlayerAnimationIndex == CPlay_Hit::ANIME_ATTACK_LIGHT2
		|| *m_pPlayerAnimationIndex == CPlay_Hit::ANIME_ATTACK_CROUCH_LIGHT || *m_pPlayerAnimationIndex == CPlay_Hit::ANIME_ATTACK_CROUCH_MEDUIM)
	{

		//서서 중공격 횟수가 남아있으면 일단 사용
		if (m_pbAttackCount[CPlay_Hit::COUNT_ATTACK_MEDIUM] == true)
		{
			m_pPlayer->Set_NextAnimation(CPlay_Hit::ANIME_ATTACK_MEDIUM, 0.5f);
			m_pbAttackCount[CPlay_Hit::COUNT_ATTACK_MEDIUM] = false;
		}

	}

	else if (*m_pPlayerAnimationIndex == CPlay_Hit::ANIME_ATTACK_MEDIUM && m_pPlayer->Get_bAttackBackEvent())
	{
		//서서 중공격 중에 또 누를 시 횟수가 있으면  앉아 중공격 자동 사용
		if (m_pbAttackCount[CPlay_Hit::COUNT_ATTACK_AUTO_MEDIUMTOHEAVY] == true)
		{
			m_pPlayer->Set_NextAnimation(CPlay_Hit::ANIME_ATTACK_HEAVY,0.5f);
			m_pbAttackCount[CPlay_Hit::COUNT_ATTACK_AUTO_MEDIUMTOHEAVY] = false;
		}
	}

	
	else if (*m_pPlayerAnimationIndex == CPlay_Hit::ANIME_ATTACK_AIR1)
	{
		m_pPlayer->Set_NextAnimation(CPlay_Hit::ANIME_ATTACK_AIR2, 0.5f);
	}
	
	else if (*m_pPlayerAnimationIndex == CPlay_Hit::ANIME_JUMP_UP || *m_pPlayerAnimationIndex == CPlay_Hit::ANIME_JUMP_DOWN)
	{
		m_pPlayer->Set_Animation(CPlay_Hit::ANIME_ATTACK_AIR2);
	}


	//다음애니메이션으로 설정하는 방식이지만 약공격이 빨라서 이상함
	//if (m_pPlayer->Check_bCurAnimationisGroundMove(*m_pPlayerAnimationIndex))
	//{
	//	m_pPlayer->Set_Animation(CPlay_Hit::ANIME_ATTACK_MEDIUM);
	//}
	//else if(*m_pPlayerAnimationIndex == CPlay_Hit::ANIME_ATTACK_LIGHT1 || *m_pPlayerAnimationIndex == CPlay_Hit::ANIME_ATTACK_LIGHT2
	//	|| *m_pPlayerAnimationIndex == CPlay_Hit::ANIME_ATTACK_CROUCH_LIGHT || *m_pPlayerAnimationIndex == CPlay_Hit::ANIME_ATTACK_CROUCH_MEDUIM)
	//{
	//	m_pPlayer->Set_NextAnimation(CPlay_Hit::ANIME_ATTACK_MEDIUM,0.5f);
	//}
	//
	//else if (*m_pPlayerAnimationIndex == CPlay_Hit::ANIME_ATTACK_AIR1)
	//{
	//	m_pPlayer->Set_NextAnimation(CPlay_Hit::ANIME_ATTACK_AIR2, 0.5f);
	//}

	else  if (*m_pPlayerAnimationIndex == CPlay_Hit::ANIME_236_POSE && m_pPlayer->Get_b236Posing())
	{
		m_pPlayer->Set_Animation(CPlay_Hit::ANIME_236_POSE_MEDIUM);
	}
	
}

void CHit_MeleeAttack::Attack_Heavy()
{
	//if (m_pPlayer->Check_bCurAnimationisGroundMove() || *m_pPlayerAnimationIndex == CPlay_Hit::ANIME_ATTACK_MEDIUM ||
	//	*m_pPlayerAnimationIndex == CPlay_Hit::ANIME_ATTACK_LIGHT1 || *m_pPlayerAnimationIndex == CPlay_Hit::ANIME_ATTACK_LIGHT2 ||
	//	*m_pPlayerAnimationIndex == CPlay_Hit::ANIME_ATTACK_CROUCH_LIGHT || *m_pPlayerAnimationIndex == CPlay_Hit::ANIME_ATTACK_CROUCH_MEDUIM ||
	//	*m_pPlayerAnimationIndex == CPlay_Hit::ANIME_ATTACK_SPECIAL)
	//{
	//	m_pPlayer->Set_Animation(CPlay_Hit::ANIME_ATTACK_HEAVY);
	//	//m_pPlayer->Set_NextAnimation(CPlay_Hit::ANIME_ATTACK_HEAVY, 0.5f);
	//}
	//
	//else if (*m_pPlayerAnimationIndex == CPlay_Hit::ANIME_ATTACK_AIR2)
	//{
	//	m_pPlayer->Set_NextAnimation(CPlay_Hit::ANIME_ATTACK_AIR3, 0.5f);
	//}
	

	if (m_pPlayer->Check_bCurAnimationisGroundMove() || *m_pPlayerAnimationIndex == CPlay_Hit::ANIME_FORWARD_DASH || *m_pPlayerAnimationIndex == CPlay_Hit::ANIME_FORWARD_DASH_END)
	{
		m_pPlayer->Set_Animation(CPlay_Hit::ANIME_ATTACK_HEAVY);
		//m_pPlayer->Set_NextAnimation(CPlay_Hit::ANIME_ATTACK_HEAVY, 0.5f);
	}
	else if (*m_pPlayerAnimationIndex == CPlay_Hit::ANIME_ATTACK_MEDIUM ||
		*m_pPlayerAnimationIndex == CPlay_Hit::ANIME_ATTACK_LIGHT1 || *m_pPlayerAnimationIndex == CPlay_Hit::ANIME_ATTACK_LIGHT2 ||
		*m_pPlayerAnimationIndex == CPlay_Hit::ANIME_ATTACK_CROUCH_LIGHT || *m_pPlayerAnimationIndex == CPlay_Hit::ANIME_ATTACK_CROUCH_MEDUIM ||
		*m_pPlayerAnimationIndex == CPlay_Hit::ANIME_ATTACK_SPECIAL)
	{
		m_pPlayer->Set_NextAnimation(CPlay_Hit::ANIME_ATTACK_HEAVY, 0.5f);
	}
	else if (*m_pPlayerAnimationIndex == CPlay_Hit::ANIME_ATTACK_AIR2)
	{
		m_pPlayer->Set_NextAnimation(CPlay_Hit::ANIME_ATTACK_AIR3, 0.5f);
	}
	else if (*m_pPlayerAnimationIndex == CPlay_Hit::ANIME_JUMP_UP  || *m_pPlayerAnimationIndex == CPlay_Hit::ANIME_JUMP_DOWN)
	{
		m_pPlayer->Set_Animation(CPlay_Hit::ANIME_ATTACK_AIR3);
	}



	else  if (*m_pPlayerAnimationIndex == CPlay_Hit::ANIME_236_POSE && m_pPlayer->Get_b236Posing())
	{

		//if(CBattleInterface_Manager::Get_Instance()->Use_KiGuage(1,m_pPlayer->Get_iPlayerTeam()))
		//	m_pPlayer->Set_Animation(CPlay_Hit::ANIME_236_POSE_HEAVY);

		if (CBattleInterface_Manager::Get_Instance()->Use_KiRealGuage(50, m_pPlayer->Get_iPlayerTeam()))
			m_pPlayer->Set_Animation(CPlay_Hit::ANIME_236_POSE_HEAVY);

		else if ((CBattleInterface_Manager::Get_Instance()->Get_KiNumber(m_pPlayer->Get_iPlayerTeam()) != 0))
		{
			CBattleInterface_Manager::Get_Instance()->Use_KiGuage(1, m_pPlayer->Get_iPlayerTeam());
			CBattleInterface_Manager::Get_Instance()->Gain_KiGuage(25, m_pPlayer->Get_iPlayerTeam());
			m_pPlayer->Set_Animation(CPlay_Hit::ANIME_236_POSE_HEAVY);

		}
	}

}

void CHit_MeleeAttack::Attack_Special()
{
	if (m_pPlayer->Check_bCurAnimationisGroundMove(*m_pPlayerAnimationIndex))
	{
		m_pPlayer->Set_Animation(CPlay_Hit::ANIME_ATTACK_SPECIAL);
		//*m_pbCounterPose = true;

	}

	else  if (*m_pPlayerAnimationIndex == CPlay_Hit::ANIME_236_POSE && m_pPlayer->Get_b236Posing())
	{
		m_pPlayer->Set_Animation(CPlay_Hit::ANIME_236_SPECIAL_RIGHT);
		m_pbAttackCount[CPlay_Hit::COUNT_ATTACK_236SPECIAL_RIGHT] = false;
		m_pPlayer->Set_b236Special(true);

	}
	
	//아래약->서서특  되지 않도록 이거 먼저 처리해야함
	else if (m_pPlayer->Get_b236Sepcial() && m_pPlayer->Get_bAttackBackEvent() && m_pbAttackCount[CPlay_Hit::COUNT_ATTACK_236SPECIAL_RIGHT] && (*m_pPlayerAnimationIndex == CPlay_Hit::ANIME_236_SPECIAL_DOWN || *m_pPlayerAnimationIndex == CPlay_Hit::ANIME_236_SPECIAL_UP))
	{
		m_pPlayer->Set_Animation(CPlay_Hit::ANIME_236_SPECIAL_RIGHT);
		m_pbAttackCount[CPlay_Hit::COUNT_ATTACK_236SPECIAL_RIGHT] = false;

	}

	else if (m_pPlayer->Get_bAttackBackEvent() && ( *m_pPlayerAnimationIndex == CPlay_Hit::ANIME_ATTACK_LIGHT1 || *m_pPlayerAnimationIndex == CPlay_Hit::ANIME_ATTACK_LIGHT2 || *m_pPlayerAnimationIndex == CPlay_Hit::ANIME_ATTACK_MEDIUM ||
		*m_pPlayerAnimationIndex == CPlay_Hit::ANIME_ATTACK_CROUCH_LIGHT || *m_pPlayerAnimationIndex == CPlay_Hit::ANIME_ATTACK_CROUCH_MEDUIM))
	{
		m_pPlayer->Set_Animation(CPlay_Hit::ANIME_ATTACK_SPECIAL);
		//*m_pbCounterPose = true;

	}

	else if (*m_pPlayerAnimationIndex == CPlay_Hit::ANIME_JUMP_UP || *m_pPlayerAnimationIndex == CPlay_Hit::ANIME_JUMP_DOWN)
	{
		m_pPlayer->Set_Animation(CPlay_Hit::ANIME_ATTACK_SPECIAL_AIR);
		//*m_pbCounterPose = true;

	}
	else if (m_pPlayer->Get_bAttackBackEvent() && (*m_pPlayerAnimationIndex == CPlay_Hit::ANIME_ATTACK_AIR1 || *m_pPlayerAnimationIndex == CPlay_Hit::ANIME_ATTACK_AIR2))
	{
		m_pPlayer->Set_Animation(CPlay_Hit::ANIME_ATTACK_SPECIAL_AIR);
		//*m_pbCounterPose = true;
	}




}

void CHit_MeleeAttack::Attack_Crouch_Speical()
{

	 if (*m_pPlayerAnimationIndex == CPlay_Hit::ANIME_236_POSE && m_pPlayer->Get_b236Posing())
	{
		m_pPlayer->Set_Animation(CPlay_Hit::ANIME_236_SPECIAL_DOWN);
		m_pbAttackCount[CPlay_Hit::COUNT_ATTACK_236SPECIAL_DOWN] = false;
		m_pPlayer->Set_b236Special(true);

		}

	else if (m_pPlayer->Get_b236Sepcial() && m_pPlayer->Get_bAttackBackEvent() && m_pbAttackCount[CPlay_Hit::COUNT_ATTACK_236SPECIAL_DOWN] && (*m_pPlayerAnimationIndex == CPlay_Hit::ANIME_236_SPECIAL_UP || *m_pPlayerAnimationIndex == CPlay_Hit::ANIME_236_SPECIAL_RIGHT))
	{
		m_pPlayer->Set_Animation(CPlay_Hit::ANIME_236_SPECIAL_DOWN);
		m_pbAttackCount[CPlay_Hit::COUNT_ATTACK_236SPECIAL_DOWN] = false;

	}

}

void CHit_MeleeAttack::Attack_Up_Speical()
{
	if (*m_pPlayerAnimationIndex == CPlay_Hit::ANIME_236_POSE && m_pPlayer->Get_b236Posing())
	{
		m_pPlayer->Set_Animation(CPlay_Hit::ANIME_236_SPECIAL_UP);
		m_pPlayer->Set_b236Special(true);

		m_pbAttackCount[CPlay_Hit::COUNT_ATTACK_236SPECIAL_UP] = false;
	}


	else if (m_pPlayer->Get_b236Sepcial() && m_pPlayer->Get_bAttackBackEvent() && m_pbAttackCount[CPlay_Hit::COUNT_ATTACK_236SPECIAL_UP] && (*m_pPlayerAnimationIndex == CPlay_Hit::ANIME_236_SPECIAL_DOWN || *m_pPlayerAnimationIndex == CPlay_Hit::ANIME_236_SPECIAL_RIGHT))
	{
		m_pPlayer->Set_Animation(CPlay_Hit::ANIME_236_SPECIAL_UP);
		m_pbAttackCount[CPlay_Hit::COUNT_ATTACK_236SPECIAL_UP] = false;

	}



}

void CHit_MeleeAttack::Attack_Grab()
{
	
	if(m_pbAttackCount[CPlay_Hit::COUNT_ATTACK_GRAB])
	{

		if (m_pPlayer->Check_bCurAnimationisGroundMove())
		{
			m_pPlayer->Set_Grab(false);
			m_pPlayer->Set_Animation(CPlay_Hit::ANIME_GRAB_READY);
			m_pPlayer->Set_NextAnimation(CPlay_Hit::ANIME_GRAB, 3.f, 5.f);
			m_pPlayer->Set_GrabLoofCount(2);

			CEffect_Layer* pLayer = m_pPlayer->Character_Make_Effect(TEXT("DIR_K"));
			_uint iEffectCount = 0;
			for (auto& iter : pLayer->m_MixtureEffects)
			{
				if (iEffectCount == 1)
					iter->m_vColor = _float4(0.f, 255.f, 0.f, 0.3f);
				iEffectCount++;
			}
		}

		else if (*m_pPlayerAnimationIndex == CPlay_Hit::ANIME_JUMP_DOWN || *m_pPlayerAnimationIndex == CPlay_Hit::ANIME_JUMP_UP)
		{
			m_pPlayer->Set_Grab(true);
			m_pPlayer->Set_Animation(CPlay_Hit::ANIME_GRAB_READY);
			m_pPlayer->Set_NextAnimation(CPlay_Hit::ANIME_GRAB, 3.f, 5.f);
			m_pPlayer->Set_GrabLoofCount(2);

			CEffect_Layer* pLayer = m_pPlayer->Character_Make_Effect(TEXT("DIR_K"));
			_uint iEffectCount = 0;
			for (auto& iter : pLayer->m_MixtureEffects)
			{
				if (iEffectCount == 1)
					iter->m_vColor = _float4(0.f, 255.f, 0.f, 0.3f);
				iEffectCount++;
			}

		}
	}

}

void CHit_MeleeAttack::Attack_236()
{

	if (m_pPlayer->Check_bCurAnimationisGroundMove())
	{
		m_pPlayer->Set_Animation(CPlay_Hit::ANIME_236_POSE);
		m_pPlayer->Reset_PoseTime();
	}

	else if (m_pPlayer->Get_bAttackBackEvent() && (
		*m_pPlayerAnimationIndex == CPlay_Hit::ANIME_ATTACK_LIGHT1 || *m_pPlayerAnimationIndex == CPlay_Hit::ANIME_ATTACK_LIGHT2 || *m_pPlayerAnimationIndex == CPlay_Hit::ANIME_ATTACK_LIGHT3 ||
		* m_pPlayerAnimationIndex == CPlay_Hit::ANIME_ATTACK_MEDIUM || *m_pPlayerAnimationIndex == CPlay_Hit::ANIME_ATTACK_HEAVY || *m_pPlayerAnimationIndex == CPlay_Hit::ANIME_ATTACK_CROUCH_MEDUIM ||
		*m_pPlayerAnimationIndex == CPlay_Hit::ANIME_ATTACK_CROUCH_HEAVY))
	{
		m_pPlayer->Set_Animation(CPlay_Hit::ANIME_236_POSE);
		m_pPlayer->Reset_PoseTime();
	}


	else if (m_pPlayer->Get_bAttackBackEvent() && m_pPlayer->Get_bSparking() && (
		*m_pPlayerAnimationIndex == CPlay_Hit::ANIME_236_POSE_LIGHT || *m_pPlayerAnimationIndex == CPlay_Hit::ANIME_236_POSE_MEDIUM || *m_pPlayerAnimationIndex == CPlay_Hit::ANIME_236_POSE_HEAVY 
		))
	{
		m_pPlayer->Set_Animation(CPlay_Hit::ANIME_236_POSE);
		m_pPlayer->Reset_PoseTime();
	}


}

void CHit_MeleeAttack::Attack_214()
{
	
	if (m_pPlayer->Check_bCurAnimationisGroundMove())
	{
		m_pPlayer->Set_Animation(CPlay_Hit::ANIME_214_POSE);
	}



}

void CHit_MeleeAttack::Attack_236Special()
{

	

}



void CHit_MeleeAttack::Attack_236Ultimate()
{
	if (m_pPlayer->Check_bCurAnimationisGroundMove())
	{
		if (CBattleInterface_Manager::Get_Instance()->Use_KiGuage(1, m_pPlayer->Get_iPlayerTeam()))
		{
			m_pPlayer->Set_Animation(CPlay_Hit::ANIME_236_ULTIMATE);
		}
	}

	else if (m_pPlayer->Get_bAttackBackEvent() && (*m_pPlayerAnimationIndex == CPlay_Hit::ANIME_ATTACK_MEDIUM || *m_pPlayerAnimationIndex == CPlay_Hit::ANIME_ATTACK_HEAVY ||
		*m_pPlayerAnimationIndex == CPlay_Hit::ANIME_ATTACK_LIGHT1 || *m_pPlayerAnimationIndex == CPlay_Hit::ANIME_ATTACK_LIGHT2 || *m_pPlayerAnimationIndex == CPlay_Hit::ANIME_ATTACK_LIGHT3 ||
		*m_pPlayerAnimationIndex == CPlay_Hit::ANIME_ATTACK_CROUCH_LIGHT || *m_pPlayerAnimationIndex == CPlay_Hit::ANIME_ATTACK_CROUCH_MEDUIM || *m_pPlayerAnimationIndex == CPlay_Hit::ANIME_ATTACK_CROUCH_HEAVY ||
		*m_pPlayerAnimationIndex == CPlay_Hit::ANIME_ATTACK_SPECIAL ||
		*m_pPlayerAnimationIndex == CPlay_Hit::ANIME_236_POSE_LIGHT || *m_pPlayerAnimationIndex == CPlay_Hit::ANIME_236_POSE_MEDIUM || *m_pPlayerAnimationIndex == CPlay_Hit::ANIME_236_POSE_HEAVY ||
		*m_pPlayerAnimationIndex == CPlay_Hit::ANIME_236_SPECIAL_RIGHT || *m_pPlayerAnimationIndex == CPlay_Hit::ANIME_236_SPECIAL_UP || *m_pPlayerAnimationIndex == CPlay_Hit::ANIME_236_SPECIAL_DOWN ||
		*m_pPlayerAnimationIndex == CPlay_Hit::ANIME_ATTACK_SPECIAL
		))
	{
		if (CBattleInterface_Manager::Get_Instance()->Use_KiGuage(1, m_pPlayer->Get_iPlayerTeam()))
		{
			m_pPlayer->Set_Animation(CPlay_Hit::ANIME_236_ULTIMATE);
		}
	}
	


}



void CHit_MeleeAttack::Attack_214Final()
{

	if (m_pPlayer->Check_bCurAnimationisGroundMove())
	{
		if (CBattleInterface_Manager::Get_Instance()->Use_KiGuage(3, m_pPlayer->Get_iPlayerTeam()))
		{
			m_pPlayer->Set_Animation(CPlay_Hit::ANIME_214_FINAL_START);
		}
	}

	if (m_pPlayer->Get_bAttackBackEvent())
	{
		if (*m_pPlayerAnimationIndex == CPlay_Hit::ANIME_236_ULTIMATE || *m_pPlayerAnimationIndex == CPlay_Hit::ANIME_214_FINAL_SUCESS == CPlay_Hit::ANIME_214_FINAL_START)
		{
			return;
		}

		if (CBattleInterface_Manager::Get_Instance()->Use_KiGuage(3, m_pPlayer->Get_iPlayerTeam()))
		{
			m_pPlayer->Set_Animation(CPlay_Hit::ANIME_214_FINAL_START);
		}
	}
	
}



void CHit_MeleeAttack::Attack_Crouch_Light()
{
	if (*m_pPlayerAnimationIndex == CPlay_Hit::ANIME_CROUCHING  || *m_pPlayerAnimationIndex == CPlay_Hit::ANIME_FORWARD_DASH || *m_pPlayerAnimationIndex == CPlay_Hit::ANIME_FORWARD_DASH_END)
	{
		m_pPlayer->Set_Animation(CPlay_Hit::ANIME_ATTACK_CROUCH_LIGHT);
		m_pPlayer->Set_b236Special(false);
	}

	if (*m_pPlayerAnimationIndex == CPlay_Hit::ANIME_ATTACK_LIGHT1 && m_pPlayer->Get_bAttackBackEvent())
	{
		m_pPlayer->Set_Animation(CPlay_Hit::ANIME_ATTACK_CROUCH_LIGHT);
		m_pPlayer->Set_b236Special(false);
	}
}

void CHit_MeleeAttack::Attack_Crouch_Medium()
{

	//횟수 없으면 컷
	//if (m_pbAttackCount[CPlay_Hit::COUNT_ATTACK_CROUCH_MEDUIM] == false)
	//	return;




	if (*m_pPlayerAnimationIndex == CPlay_Hit::ANIME_CROUCHING || *m_pPlayerAnimationIndex == CPlay_Hit::ANIME_FORWARD_DASH || *m_pPlayerAnimationIndex == CPlay_Hit::ANIME_FORWARD_DASH_END)
	{
		m_pPlayer->Set_Animation(CPlay_Hit::ANIME_ATTACK_CROUCH_MEDUIM);
		//m_pbAttackCount[CPlay_Hit::COUNT_ATTACK_CROUCH_MEDUIM] = false;
	}

	else if (*m_pPlayerAnimationIndex == CPlay_Hit::ANIME_ATTACK_CROUCH_LIGHT ||
		*m_pPlayerAnimationIndex == CPlay_Hit::ANIME_ATTACK_LIGHT1 || *m_pPlayerAnimationIndex == CPlay_Hit::ANIME_ATTACK_LIGHT2 ||
		*m_pPlayerAnimationIndex == CPlay_Hit::ANIME_ATTACK_MEDIUM)
	{
		m_pPlayer->Set_NextAnimation(CPlay_Hit::ANIME_ATTACK_CROUCH_MEDUIM, 1.f);
		//m_pbAttackCount[CPlay_Hit::COUNT_ATTACK_CROUCH_MEDUIM] = false;
	}



	//이전에 나올것이 약공격이다보니 nextanimation을 사용하면 타이밍을 놓침
	//if (*m_pPlayerAnimationIndex == CPlay_Hit::ANIME_CROUCHING)
	//{
	//	m_pPlayer->Set_Animation(CPlay_Hit::ANIME_ATTACK_CROUCH_MEDUIM);
	//} 
	//else if(*m_pPlayerAnimationIndex == CPlay_Hit::ANIME_ATTACK_CROUCH_LIGHT ||
	//	*m_pPlayerAnimationIndex == CPlay_Hit::ANIME_ATTACK_LIGHT1 || *m_pPlayerAnimationIndex == CPlay_Hit::ANIME_ATTACK_LIGHT2 )
	//{
	//	m_pPlayer->Set_NextAnimation(CPlay_Hit::ANIME_ATTACK_CROUCH_MEDUIM,0.4f);
	//}

	
}

void CHit_MeleeAttack::Attack_Crouch_Heavy()
{



	//if (*m_pPlayerAnimationIndex == CPlay_Hit::ANIME_CROUCHING || *m_pPlayerAnimationIndex == CPlay_Hit::ANIME_ATTACK_MEDIUM || *m_pPlayerAnimationIndex == CPlay_Hit::ANIME_ATTACK_HEAVY ||
	//	*m_pPlayerAnimationIndex == CPlay_Hit::ANIME_ATTACK_LIGHT1 || *m_pPlayerAnimationIndex == CPlay_Hit::ANIME_ATTACK_LIGHT2 || *m_pPlayerAnimationIndex == CPlay_Hit::ANIME_ATTACK_LIGHT3 ||
	//	*m_pPlayerAnimationIndex == CPlay_Hit::ANIME_ATTACK_CROUCH_LIGHT || *m_pPlayerAnimationIndex == CPlay_Hit::ANIME_ATTACK_CROUCH_MEDUIM)
	//{
	//	m_pPlayer->Set_Animation(CPlay_Hit::ANIME_ATTACK_CROUCH_HEAVY);
	//}
	//
	//else if (*m_pPlayerAnimationIndex == CPlay_Hit::ANIME_JUMP_UP || *m_pPlayerAnimationIndex == CPlay_Hit::ANIME_JUMP_DOWN ||
	//	*m_pPlayerAnimationIndex == CPlay_Hit::ANIME_ATTACK_AIR1 || *m_pPlayerAnimationIndex == CPlay_Hit::ANIME_ATTACK_AIR2)
	//{
	//	m_pPlayer->Set_Animation(CPlay_Hit::ANIME_ATTACK_UPPER_AIR);
	//}



	if (*m_pPlayerAnimationIndex == CPlay_Hit::ANIME_CROUCHING || *m_pPlayerAnimationIndex == CPlay_Hit::ANIME_FORWARD_DASH || *m_pPlayerAnimationIndex == CPlay_Hit::ANIME_FORWARD_DASH_END)
	{
		m_pPlayer->Set_Animation(CPlay_Hit::ANIME_ATTACK_CROUCH_HEAVY);
		m_pPlayer->Set_b236Special(false);
	}
	
	else if(*m_pPlayerAnimationIndex == CPlay_Hit::ANIME_ATTACK_MEDIUM || *m_pPlayerAnimationIndex == CPlay_Hit::ANIME_ATTACK_HEAVY ||
		*m_pPlayerAnimationIndex == CPlay_Hit::ANIME_ATTACK_LIGHT1 || *m_pPlayerAnimationIndex == CPlay_Hit::ANIME_ATTACK_LIGHT2 || *m_pPlayerAnimationIndex == CPlay_Hit::ANIME_ATTACK_LIGHT3 ||
		*m_pPlayerAnimationIndex == CPlay_Hit::ANIME_ATTACK_CROUCH_LIGHT || *m_pPlayerAnimationIndex == CPlay_Hit::ANIME_ATTACK_CROUCH_MEDUIM || *m_pPlayerAnimationIndex == CPlay_Hit::ANIME_ATTACK_SPECIAL)
	{
		m_pPlayer->Set_NextAnimation(CPlay_Hit::ANIME_ATTACK_CROUCH_HEAVY, 0.5f);
		m_pPlayer->Set_b236Special(false);
	}

	else if (*m_pPlayerAnimationIndex == CPlay_Hit::ANIME_JUMP_UP || *m_pPlayerAnimationIndex == CPlay_Hit::ANIME_JUMP_DOWN)
	{
		m_pPlayer->Set_Animation(CPlay_Hit::ANIME_ATTACK_UPPER_AIR);
		m_pPlayer->Set_fImpulse(1.6f * m_pPlayer->Get_iDirection());

	} 
	else if(*m_pPlayerAnimationIndex == CPlay_Hit::ANIME_ATTACK_AIR1 || *m_pPlayerAnimationIndex == CPlay_Hit::ANIME_ATTACK_AIR2 || *m_pPlayerAnimationIndex == CPlay_Hit::ANIME_ATTACK_SPECIAL_AIR)
	{
		m_pPlayer->Set_NextAnimation(CPlay_Hit::ANIME_ATTACK_UPPER_AIR, 0.5f);
		m_pPlayer->Set_fImpulse(1.6f * m_pPlayer->Get_iDirection());

	}




}

void CHit_MeleeAttack::BackDash()
{

	if (m_pPlayer->Check_bCurAnimationisGroundMove())
	{
		m_pPlayer->Set_Animation(CPlay_Hit::ANIME_BACK_DASH);
	}

	else if (m_pPlayer->Get_bAirDashEnable() && (*m_pPlayerAnimationIndex == CPlay_Hit::ANIME_JUMP_DOWN || *m_pPlayerAnimationIndex == CPlay_Hit::ANIME_JUMP_UP))
	{
		m_pPlayer->Set_Animation(CPlay_Hit::ANIME_BACK_DASH);
		m_pPlayer->Set_CurrentAnimationPositionJump(12.f);
		m_pPlayer->Set_fImpulse(m_pPlayer->Get_iDirection() *(-10.f));

		m_pPlayer->Set_bAirDashEnable(false);
		m_pPlayer->Set_ForcedGravityDown();
		m_pPlayer->Set_ForcveGravityTime(0.135f);


		//m_pPlayer->Character_Make_Effect(TEXT("Moving_Line_Left"),{1.f,0.f});
		m_pPlayer->Character_Make_Effect(TEXT("Moving_Line_Left"), { -0.3f,0.f });

		m_pGameInstance->Play_Sound(CSound_Manager::SOUND_KEY_NAME::Common_Back_Dash_SFX, false, 1.f);
	}

	//포즈 막바지에 갑자기 푸는거 금지 제대로 포즈중에 해야함
	else if (*m_pPlayerAnimationIndex == CPlay_Hit::ANIME_236_POSE && m_pPlayer->Get_b236Posing())
	{
		//m_pPlayer->Set_Animation(CPlay_Hit::ANIME_IDLE);
		//m_pPlayer->Set_b236Posing(false);
		m_pPlayer->Set_CurrentAnimationPositionJump(19.99);
	}

}

void CHit_MeleeAttack::ForwardDash()
{
	//if (m_pPlayer->Check_bCurAnimationisGroundMove())
	//지상대쉬
	if(*m_pPlayerAnimationIndex == CPlay_Hit::ANIME_IDLE || *m_pPlayerAnimationIndex == CPlay_Hit::ANIME_FORWARD_WALK || *m_pPlayerAnimationIndex == CPlay_Hit::ANIME_BACK_WALK)
	{
		m_pPlayer->Set_Animation(CPlay_Hit::ANIME_FORWARD_DASH);
		m_pGameInstance->Play_Sound(CSound_Manager::SOUND_KEY_NAME::Common_Dash_Start_SFX, false, 1.f);

		CEffect_Layer::COPY_DESC tDesc{};
		tDesc.pPlayertMatrix = static_cast<CTransform*>(m_pPlayer->Get_Component(TEXT("Com_Transform")))->Get_WorldMatrixPtr();
		m_pEffect_Manager->Copy_Layer(TEXT("Smoke_Run"), &tDesc);

		m_pGameInstance->Play_Sound(CSound_Manager::SOUND_KEY_NAME::Common_Dash_Start_SFX, false, 1.f);
	}

	else if (m_pPlayer->Get_bAirDashEnable()  && (*m_pPlayerAnimationIndex == CPlay_Hit::ANIME_JUMP_DOWN || *m_pPlayerAnimationIndex == CPlay_Hit::ANIME_JUMP_UP ))
	{
		m_pPlayer->Set_Animation(CPlay_Hit::ANIME_FORWARD_DASH);
		//m_pPlayer->Set_CurrentAnimationPositionJump(4.f);
		m_pPlayer->Set_fImpulse(m_pPlayer->Get_iDirection() * (10.f));
		m_pPlayer->Set_bAirDashEnable(false);
		//m_pPlayer->Set_ForcedGravityDown();

		m_pPlayer->Set_ForcveGravityTime(0.255f);

		//m_pPlayer->Character_Make_Effect(TEXT("Moving_Line_Right"));
		//m_pPlayer->Character_Make_Effect(TEXT("Ring_Dust_Right"), { 0.9f,0.f });

		//m_pPlayer->Character_Make_Effect(TEXT("Right_Wall_Crash"), { 0.6f,0.f });

		//m_pEffect_Manager->Copy_Layer(TEXT("Dash"), m_pPlayer->Get_pTransformMatrix());

		m_pPlayer->Character_Make_Effect(TEXT("Dash"), { 1.2f,0.f });
		
		m_pGameInstance->Play_Sound(CSound_Manager::SOUND_KEY_NAME::Common_Back_Dash_SFX, false, 1.f);
	}

	else if (m_pPlayer->Get_bSparking() && m_pPlayer->Get_bAirDashEnable() && m_pPlayer->Get_bAttackBackEvent() && 
		(*m_pPlayerAnimationIndex == CPlay_Hit::ANIME_ATTACK_AIR1 || *m_pPlayerAnimationIndex == CPlay_Hit::ANIME_ATTACK_AIR2))
	{
		m_pPlayer->Set_Animation(CPlay_Hit::ANIME_FORWARD_DASH);
		m_pPlayer->Set_fImpulse(m_pPlayer->Get_iDirection() * (10.f));
		m_pPlayer->Set_bAirDashEnable(false);
		//m_pPlayer->Set_ForcedGravityDown();

		m_pPlayer->Set_ForcveGravityTime(0.255f);

		//m_pPlayer->Character_Make_Effect(TEXT("Moving_Line_Right"));
		//m_pPlayer->Character_Make_Effect(TEXT("Ring_Dust_Right"), { 0.9f,0.f });

		CEffect_Layer::COPY_DESC tDesc{};
		tDesc.pPlayertMatrix = m_pPlayer->Get_pTransformMatrix();
		m_pEffect_Manager->Copy_Layer(TEXT("Dash"), &tDesc);
	}



}

void CHit_MeleeAttack::Reflect()
{
	if (m_pPlayer->Check_bCurAnimationisGroundMove())
	{
		m_pPlayer->Set_Animation(CPlay_Hit::ANIME_REFLECT);

	}

	else if (m_pPlayer->Get_bAttackBackEvent() && m_pPlayer->Check_bCurAnimationisReflect())
	{
		m_pPlayer->Set_Animation(CPlay_Hit::ANIME_REFLECT);
		m_pPlayer->Set_CurrentAnimationPositionJump(0.f);

	}

}

void CHit_MeleeAttack::Attack_Benishing()
{

	if (CBattleInterface_Manager::Get_Instance()->Get_KiNumber(m_pPlayer->Get_iPlayerTeam()) == 0 || m_pbAttackCount[CPlay_Hit::COUNT_ATTACK_BENISHING] == false)
		return;


	if (m_pPlayer->Get_bAttackBackEvent() &&
		(*m_pPlayerAnimationIndex == CPlay_Hit::ANIME_ATTACK_AIR1 || *m_pPlayerAnimationIndex == CPlay_Hit::ANIME_ATTACK_AIR2 || *m_pPlayerAnimationIndex == CPlay_Hit::ANIME_ATTACK_AIR3 ||
			*m_pPlayerAnimationIndex == CPlay_Hit::ANIME_ATTACK_MEDIUM || *m_pPlayerAnimationIndex == CPlay_Hit::ANIME_ATTACK_HEAVY || *m_pPlayerAnimationIndex == CPlay_Hit::ANIME_ATTACK_CROUCH_HEAVY ||
			*m_pPlayerAnimationIndex == CPlay_Hit::ANIME_236_POSE_LIGHT || *m_pPlayerAnimationIndex == CPlay_Hit::ANIME_236_POSE_MEDIUM || *m_pPlayerAnimationIndex == CPlay_Hit::ANIME_236_POSE_HEAVY ||
			*m_pPlayerAnimationIndex == CPlay_Hit::ANIME_236_SPECIAL_RIGHT || *m_pPlayerAnimationIndex == CPlay_Hit::ANIME_236_SPECIAL_UP || *m_pPlayerAnimationIndex == CPlay_Hit::ANIME_236_SPECIAL_DOWN))
	{

		if (CBattleInterface_Manager::Get_Instance()->Use_KiGuage(1, m_pPlayer->Get_iPlayerTeam()))
		{
			m_pbAttackCount[CPlay_Hit::COUNT_ATTACK_BENISHING] = false;
			//m_pPlayer->Set_NextAnimation(CPlay_Goku::ANIME_ATTACK_AIR2,2.f,0.f);
			m_pPlayer->Set_Animation(CPlay_Hit::ANIME_ATTACK_AIR1);
			m_pPlayer->Set_CurrentAnimationPositionJump(14.99);
			m_pPlayer->Set_AnimationStopWithoutMe(0.3f);
			m_pPlayer->Set_AnimationStop(0.3f);
			//샤샤샥
			m_pPlayer->Character_Make_Effect(TEXT("Moving_Line_Right"));
			m_pPlayer->Character_Create_Distortion({ 1.f,0.f,0.f }, { 0.f,0.f }, { 1.f,1.f }, 0.2f);
			m_pPlayer->Teleport_ToEnemy(1.5f, 0.3f);
			m_pPlayer->FlipDirection();

			m_pPlayer->Set_bBenishingAttack(true);
			m_pPlayer->Set_ForcedGravityDown();
			m_pPlayer->Set_bInivisible(true);
			m_pPlayer->Set_fImpulse({ 0.f,0.f });
		}

	}

	else if (m_pPlayer->Check_bCurAnimationisGroundMove() || m_pPlayer->Check_bCurAnimationisAirMove())
	{
		if (CBattleInterface_Manager::Get_Instance()->Use_KiGuage(1, m_pPlayer->Get_iPlayerTeam()))
		{
			m_pbAttackCount[CPlay_Hit::COUNT_ATTACK_BENISHING] = false;
			//m_pPlayer->Set_NextAnimation(CPlay_Goku::ANIME_ATTACK_AIR2,2.f,0.f);
			m_pPlayer->Set_Animation(CPlay_Hit::ANIME_ATTACK_AIR1);
			m_pPlayer->Set_CurrentAnimationPositionJump(14.99);
			m_pPlayer->Set_AnimationStopWithoutMe(0.3f);
			m_pPlayer->Set_AnimationStop(0.3f);
			//샤샤샥
			m_pPlayer->Character_Make_Effect(TEXT("Moving_Line_Right"));
			m_pPlayer->Character_Create_Distortion({ 1.f,0.f,0.f }, { 0.f,0.f }, { 1.f,1.f }, 0.2f);
			m_pPlayer->Teleport_ToEnemy(1.5f, 0.3f);
			m_pPlayer->FlipDirection();

			m_pPlayer->Set_bBenishingAttack(true);
			m_pPlayer->Set_ForcedGravityDown();
			m_pPlayer->Set_bInivisible(true);
			m_pPlayer->Set_fImpulse({ 0.f,0.f });
		}

	}
}

void CHit_MeleeAttack::StopMacro(_float fStopTime)
{
	m_pPlayer->Set_AnimationStopWithoutMe(fStopTime);

	//이전에 만들어진 모든 Attack Object들 2초간 정지.  이거 본인은 이 직후에 만들어질테니 괜찮음
	for (auto pAttackObject : m_pGameInstance->Get_Layer(LEVEL_GAMEPLAY, TEXT("Layer_AttackObject")))
	{
		static_cast<CAttackObject*>(pAttackObject)->Set_UpdateStop(fStopTime);
	}
}

