#include "stdafx.h"
#include "..\Public\Frieza_MeleeAttack.h"

#include "RenderInstance.h"
#include "GameInstance.h"
#include "Input.h"

#include "Play_Frieza.h"

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

void CFrieza_MeleeAttack::Initalize(CPlay_Frieza* pPlayer)
{
	m_pPlayer = pPlayer;
	m_pPlayerAnimationIndex = m_pPlayer->Get_pAnimationIndex();


	m_pbAttackCount = m_pPlayer->Get_pbAttackCount();
	m_piCountGroundSpecial = m_pPlayer->Get_piSpecialCount();
	m_piCountAirSpecial = m_pPlayer->Get_piSAirpecialCount();

	m_pGameInstance = CGameInstance::Get_Instance();
	m_pEffect_Manager = CEffect_Manager::Get_Instance();


	//	Safe_AddRef(m_pEffect_Manager);
}

void CFrieza_MeleeAttack::Attack_Light()
{

	this;
	_bool bDebug = false;


	if (m_pPlayer->Check_bCurAnimationisGroundMove())
	{
		m_pPlayer->Set_Animation(CPlay_Frieza::ANIME_ATTACK_LIGHT1);
		
	}

	else if (*m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_ATTACK_LIGHT1)
	{
		m_pPlayer->Set_NextAnimation(CPlay_Frieza::ANIME_ATTACK_LIGHT2,1.f);
	}

	else if (*m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_ATTACK_LIGHT2)
	{
		m_pPlayer->Set_NextAnimation(CPlay_Frieza::ANIME_ATTACK_LIGHT3, 1.f);
	}

	else if (*m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_JUMP_UP || *m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_JUMP_DOWN)
	{
		m_pPlayer->Set_Animation(CPlay_Frieza::ANIME_ATTACK_AIR1);
	}
	else if (*m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_ATTACK_AIR1)
	{
		
		m_pPlayer->Set_NextAnimation(CPlay_Frieza::ANIME_ATTACK_AIR2,0.5f);
		
		if (m_pPlayer->Get_fGravityTime() > 0.135)
		{
			m_pPlayer->Set_bNextAnimationGravityEvent();
		}
	}
	else if (*m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_ATTACK_AIR2)
	{
		m_pPlayer->Set_NextAnimation(CPlay_Frieza::ANIME_ATTACK_AIR3,0.5f);
		if (m_pPlayer->Get_fGravityTime() > 0.135)
		{
			//m_pPlayer->Set_ForcedGravityTime_LittleUp();
			m_pPlayer->Set_bNextAnimationGravityEvent();
		}
	}

	
	else if (*m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_ATTACK_CROUCH_LIGHT)
	{
		m_pPlayer->Set_Animation(CPlay_Frieza::ANIME_ATTACK_LIGHT1);
	}
	else if (*m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_FORWARD_DASH || *m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_FORWARD_DASH_END)
	{
		m_pPlayer->Set_Animation(CPlay_Frieza::ANIME_ATTACK_LIGHT1);
	}
}

void CFrieza_MeleeAttack::Attack_Medium()
{


	if (m_pPlayer->Check_bCurAnimationisGroundMove(*m_pPlayerAnimationIndex) || *m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_FORWARD_DASH || *m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_FORWARD_DASH_END)
	{
		m_pPlayer->Set_Animation(CPlay_Frieza::ANIME_ATTACK_MEDIUM);
		m_pbAttackCount[CPlay_Frieza::COUNT_ATTACK_MEDIUM] = false;

	}
	
	else if(*m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_ATTACK_LIGHT1 || *m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_ATTACK_LIGHT2
		|| *m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_ATTACK_CROUCH_LIGHT || *m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_ATTACK_CROUCH_MEDUIM)
	{

		//서서 중공격 횟수가 남아있으면 일단 사용
		if (m_pbAttackCount[CPlay_Frieza::COUNT_ATTACK_MEDIUM] == true)
		{
			m_pPlayer->Set_NextAnimation(CPlay_Frieza::ANIME_ATTACK_MEDIUM, 0.5f);
			m_pbAttackCount[CPlay_Frieza::COUNT_ATTACK_MEDIUM] = false;
		}

	}

	else if (*m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_ATTACK_MEDIUM)
	{
		//서서 중공격 중에 또 누를 시 횟수가 있으면  앉아 중공격 자동 사용
		if (m_pbAttackCount[CPlay_Frieza::COUNT_ATTACK_CROUCH_MEDUIM] == true)
		{
			m_pPlayer->Set_NextAnimation(CPlay_Frieza::ANIME_ATTACK_CROUCH_MEDUIM,0.1f);
			m_pbAttackCount[CPlay_Frieza::COUNT_ATTACK_CROUCH_MEDUIM] = false;
		}
	}

	
	else if (*m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_ATTACK_AIR1)
	{
		m_pPlayer->Set_NextAnimation(CPlay_Frieza::ANIME_ATTACK_AIR2, 0.5f);
	}
	
	else if (*m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_JUMP_UP || *m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_JUMP_DOWN)
	{
		m_pPlayer->Set_Animation(CPlay_Frieza::ANIME_ATTACK_AIR2);
	}


	//다음애니메이션으로 설정하는 방식이지만 약공격이 빨라서 이상함
	//if (m_pPlayer->Check_bCurAnimationisGroundMove(*m_pPlayerAnimationIndex))
	//{
	//	m_pPlayer->Set_Animation(CPlay_Frieza::ANIME_ATTACK_MEDIUM);
	//}
	//else if(*m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_ATTACK_LIGHT1 || *m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_ATTACK_LIGHT2
	//	|| *m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_ATTACK_CROUCH_LIGHT || *m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_ATTACK_CROUCH_MEDUIM)
	//{
	//	m_pPlayer->Set_NextAnimation(CPlay_Frieza::ANIME_ATTACK_MEDIUM,0.5f);
	//}
	//
	//else if (*m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_ATTACK_AIR1)
	//{
	//	m_pPlayer->Set_NextAnimation(CPlay_Frieza::ANIME_ATTACK_AIR2, 0.5f);
	//}

	
	
}

void CFrieza_MeleeAttack::Attack_Heavy()
{
	//if (m_pPlayer->Check_bCurAnimationisGroundMove() || *m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_ATTACK_MEDIUM ||
	//	*m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_ATTACK_LIGHT1 || *m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_ATTACK_LIGHT2 ||
	//	*m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_ATTACK_CROUCH_LIGHT || *m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_ATTACK_CROUCH_MEDUIM ||
	//	*m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_ATTACK_SPECIAL)
	//{
	//	m_pPlayer->Set_Animation(CPlay_Frieza::ANIME_ATTACK_HEAVY);
	//	//m_pPlayer->Set_NextAnimation(CPlay_Frieza::ANIME_ATTACK_HEAVY, 0.5f);
	//}
	//
	//else if (*m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_ATTACK_AIR2)
	//{
	//	m_pPlayer->Set_NextAnimation(CPlay_Frieza::ANIME_ATTACK_AIR3, 0.5f);
	//}
	

	if (m_pPlayer->Check_bCurAnimationisGroundMove() || *m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_FORWARD_DASH || *m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_FORWARD_DASH_END)
	{
		m_pPlayer->Set_Animation(CPlay_Frieza::ANIME_ATTACK_HEAVY);
		//m_pPlayer->Set_NextAnimation(CPlay_Frieza::ANIME_ATTACK_HEAVY, 0.5f);
	}
	else if (*m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_ATTACK_MEDIUM ||
		*m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_ATTACK_LIGHT1 || *m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_ATTACK_LIGHT2 ||
		*m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_ATTACK_CROUCH_LIGHT || *m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_ATTACK_CROUCH_MEDUIM ||
		*m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_ATTACK_SPECIAL)
	{
		m_pPlayer->Set_NextAnimation(CPlay_Frieza::ANIME_ATTACK_HEAVY, 0.5f);
	}
	else if (m_pPlayer->Get_bAttackBackEvent() && *m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_ATTACK_SPECIAL)
	{
		m_pPlayer->Set_Animation(CPlay_Frieza::ANIME_ATTACK_CROUCH_HEAVY);
	}


	else if (*m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_ATTACK_AIR2)
	{
		m_pPlayer->Set_NextAnimation(CPlay_Frieza::ANIME_ATTACK_AIR3, 0.5f);
	}
	else if (*m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_JUMP_UP  || *m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_JUMP_DOWN)
	{
		m_pPlayer->Set_Animation(CPlay_Frieza::ANIME_ATTACK_AIR3);
	}
}

void CFrieza_MeleeAttack::Attack_Special()
{
	if (m_pPlayer->Check_bCurAnimationisGroundMove(*m_pPlayerAnimationIndex))
	{
		m_pPlayer->Set_Animation(CPlay_Frieza::ANIME_ATTACK_SPECIAL);
		//(*m_piCountGroundSpecial)++;
	} 
	else if(*m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_ATTACK_LIGHT1 || *m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_ATTACK_LIGHT2 || *m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_ATTACK_MEDIUM ||
			*m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_ATTACK_CROUCH_LIGHT || *m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_ATTACK_CROUCH_MEDUIM)
	{
		//m_pPlayer->Set_NextAnimation(CPlay_Frieza::ANIME_ATTACK_SPECIAL, 0.5f);
		m_pPlayer->Set_NextAnimation(CPlay_Frieza::ANIME_ATTACK_SPECIAL, 0.5f);
		//(*m_piCountGroundSpecial)++;

	}

	else if (*m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_ATTACK_SPECIAL)
	{
		//_ushort iCount = *m_piCountGroundSpecial;

		
		//0에선 더할 수 없으니 5대신 4,    4발째일때까지 다음공격 가능하니 이하 대신 미만.
		//if (iCount < 4)
		//{
		//
		//	//m_pPlayer->Set_NextAnimation(CPlay_Frieza::ANIME_ATTACK_SPECIAL, 0.5f);
		//	
		//	//홀짝 판별 비트연산 (최적화)  true일때 홀수
		//	if (iCount & 1)  //홀수만큼 썻으니 왼손
		//	{
		//		if (m_pPlayer->CompareNextAnimation(CPlay_Frieza::ANIME_IDLE))
		//		{
		//			//m_pPlayer->Set_NextAnimation(CPlay_Frieza::ANIME_ATTACK_SPECIAL, 0.5f, 10.f);
		//			m_pPlayer->Set_NextAnimation(CPlay_Frieza::ANIME_ATTACK_SPECIAL, 0.5f, 10.f);
		//
		//			//cout << "TEST1" << endl;
		//			(*m_piCountGroundSpecial)++;
		//		}
		//		
		//	}
		//	else
		//	{
		//		//if (m_pPlayer->CompareNextAnimation(CPlay_Frieza::ANIME_ATTACK_SPECIAL, 55.f) == false)
		//		if (m_pPlayer->CompareNextAnimation(CPlay_Frieza::ANIME_IDLE))
		//		{
		//			m_pPlayer->Set_NextAnimation(CPlay_Frieza::ANIME_ATTACK_SPECIAL, 0.5f, 55);
		//			//cout << "TEST2" << endl;
		//
		//			(*m_piCountGroundSpecial)++;
		//		}
		//
		//	}
		//
		//
		//}
		//else
		//{
		//	(*m_piCountGroundSpecial)++;
		//}
	}


	//else if (*m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_JUMP_UP || *m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_JUMP_DOWN ||
	//	*m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_ATTACK_AIR1 || *m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_ATTACK_AIR2)
	//{
	//	m_pPlayer->Set_Animation(CPlay_Frieza::ANIME_ATTACK_SPECIAL_AIR);
	//}


	else if (*m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_JUMP_UP || *m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_JUMP_DOWN)
	{
		if (*m_piCountAirSpecial != 0)
		{
			(*m_piCountAirSpecial)--;
			m_pPlayer->Set_Animation(CPlay_Frieza::ANIME_ATTACK_SPECIAL_AIR);
			//*m_piCountAirSpecial = 5;  이건 땅에 닿았을때 하자  풀리자마자 다시쏜다
		}


	}
	else if(*m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_ATTACK_AIR1 || *m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_ATTACK_AIR2)
	{
		if (*m_piCountAirSpecial != 0)
		{
			(*m_piCountAirSpecial)--;
			m_pPlayer->Set_NextAnimation(CPlay_Frieza::ANIME_ATTACK_SPECIAL_AIR, 0.5f);
			//*m_piCountAirSpecial = 5;  이건 땅에 닿았을때 하자  풀리자마자 다시쏜다

		}
	}
	else if (m_pPlayer->Get_bAttackBackEvent() && *m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_ATTACK_SPECIAL_AIR)
	{
		
		if (*m_piCountAirSpecial != 0)
		{
			(*m_piCountAirSpecial)--;
			m_pPlayer->Set_Animation(CPlay_Frieza::ANIME_ATTACK_SPECIAL_AIR2);
		}


		//m_pPlayer->Set_CurrentAnimationPositionJump()
	}
	else if (m_pPlayer->Get_bAttackBackEvent() && *m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_ATTACK_SPECIAL_AIR2)
	{
		//m_pPlayer->Set_Animation(CPlay_Frieza::ANIME_ATTACK_SPECIAL_AIR);
		if (*m_piCountAirSpecial != 0)
		{
			(*m_piCountAirSpecial)--;
			m_pPlayer->Set_CurrentAnimationPositionJump(44.99f);
			//m_pPlayer->Set_Animation(CPlay_Frieza::ANIME_ATTACK_SPECIAL_AIR2);
		}

	}

}

void CFrieza_MeleeAttack::Attack_Grab()
{
	
	if (*m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_HIT_BOUND_DOWN)
	{
		m_pPlayer->Set_Animation(CPlay_Frieza::ANIME_WAKEUP_FINAL);
		m_pPlayer->Set_NextAnimation(CPlay_Frieza::ANIME_IDLE,10.f);
	}

	if(m_pbAttackCount[CPlay_Frieza::COUNT_ATTACK_GRAB])
	{

		if (m_pPlayer->Check_bCurAnimationisGroundMove())
		{
			m_pPlayer->Set_Grab(false);
			m_pPlayer->Set_Animation(CPlay_Frieza::ANIME_GRAB_READY);
			m_pPlayer->Set_NextAnimation(CPlay_Frieza::ANIME_GRAB, 3.f, 5.f);
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

		else if (*m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_JUMP_DOWN || *m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_JUMP_UP)
		{
			m_pPlayer->Set_Grab(true);
			m_pPlayer->Set_Animation(CPlay_Frieza::ANIME_GRAB_READY);
			m_pPlayer->Set_NextAnimation(CPlay_Frieza::ANIME_GRAB, 3.f, 5.f);
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

void CFrieza_MeleeAttack::Attack_236()
{

	m_pPlayer->Set_bHeavySkill(false);

	if (m_pPlayer->Check_bCurAnimationisGroundMove())
	{
		m_pPlayer->Set_Animation(CPlay_Frieza::ANIME_ATTACK_236);
	}

	else if (m_pPlayer->Get_bAttackBackEvent() &&
		(*m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_ATTACK_LIGHT1 || *m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_ATTACK_LIGHT2 || *m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_ATTACK_LIGHT3 ||
			*m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_ATTACK_MEDIUM || *m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_ATTACK_CROUCH_MEDUIM ||
			*m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_ATTACK_HEAVY || *m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_ATTACK_CROUCH_HEAVY)
		)
	{
		m_pPlayer->Set_Animation(CPlay_Frieza::ANIME_ATTACK_236);
	}


}

void CFrieza_MeleeAttack::Attack_214()
{
	if (m_pPlayer->Check_bCurAnimationisGroundMove())
	{
		m_pPlayer->Set_Animation(CPlay_Frieza::ANIME_ATTACK_214);
	}
	else if (m_pPlayer->Get_bAttackBackEvent() &&
		(*m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_ATTACK_LIGHT1 || *m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_ATTACK_LIGHT2 || *m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_ATTACK_LIGHT3 ||
			*m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_ATTACK_MEDIUM || *m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_ATTACK_CROUCH_MEDUIM ||
			*m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_ATTACK_HEAVY || *m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_ATTACK_CROUCH_HEAVY)
		)
	{
		m_pPlayer->Set_Animation(CPlay_Frieza::ANIME_ATTACK_214);
	}
	
}

void CFrieza_MeleeAttack::Attack_236_Heavy()
{

	_bool bHaveKi = false;

	//기 2칸 있을 때
	if (CBattleInterface_Manager::Get_Instance()->Use_KiRealGuage(50, m_pPlayer->Get_iPlayerTeam()))
	{
		//m_pPlayer->Set_Animation(CPlay_Frieza::ANIME_236_POSE_HEAVY);
		bHaveKi = true;
	}

	//2칸은 없는데 1줄 이상 있을 때
	else if ((CBattleInterface_Manager::Get_Instance()->Get_KiNumber(m_pPlayer->Get_iPlayerTeam()) != 0))
	{
		CBattleInterface_Manager::Get_Instance()->Use_KiGuage(1, m_pPlayer->Get_iPlayerTeam());
		CBattleInterface_Manager::Get_Instance()->Gain_KiGuage(25, m_pPlayer->Get_iPlayerTeam());

		bHaveKi = true;
	}

	//다 없으면 약공격 버전으로
	else
	{
		Attack_236();
		return;
	}


	if (bHaveKi)
	{
		m_pPlayer->Set_bHeavySkill(true);

		if (m_pPlayer->Check_bCurAnimationisGroundMove() || *m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_FORWARD_DASH || *m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_FORWARD_DASH_END)
		{
			m_pPlayer->Set_Animation(CPlay_Frieza::ANIME_ATTACK_236);
			
			m_pPlayer->Set_bHeavySkill(true);

			m_pPlayer->Character_Make_Effect(TEXT("DIR_K"));
		}

		else if (m_pPlayer->Get_bAttackBackEvent() && (*m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_ATTACK_MEDIUM || *m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_ATTACK_HEAVY ||
			*m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_ATTACK_LIGHT1 || *m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_ATTACK_LIGHT2 || *m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_ATTACK_LIGHT3 ||
			*m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_ATTACK_SPECIAL || *m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_ATTACK_CROUCH_LIGHT || *m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_ATTACK_CROUCH_MEDUIM))
		{
			m_pPlayer->Set_Animation(CPlay_Frieza::ANIME_ATTACK_236, false);

			m_pPlayer->Set_bHeavySkill(true);
			m_pPlayer->Character_Make_Effect(TEXT("DIR_K"));
		}
		
		//사용 못했으면
		else
		{
			CBattleInterface_Manager::Get_Instance()->Gain_KiGuage(25, m_pPlayer->Get_iPlayerTeam());
			m_pPlayer->Set_bHeavySkill(false);
		}
	}
}

void CFrieza_MeleeAttack::Attack_236Special()
{
	if (m_pPlayer->Check_bCurAnimationisGroundMove() || m_pPlayer->Check_bCurAnimationisAirMove())
	{
		m_pPlayer->Set_Animation(CPlay_Frieza::ANIME_ATTACK_236_SPECIAL);
	}

	else if (m_pPlayer->Get_bAttackBackEvent() &&
		(*m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_ATTACK_LIGHT1 || *m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_ATTACK_LIGHT2 || *m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_ATTACK_LIGHT3 ||
			*m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_ATTACK_MEDIUM || *m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_ATTACK_CROUCH_MEDUIM ||
			*m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_ATTACK_AIR1 || *m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_ATTACK_AIR2 || 
			*m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_ATTACK_SPECIAL_AIR || *m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_ATTACK_SPECIAL_AIR2)
		)
	{
		m_pPlayer->Set_Animation(CPlay_Frieza::ANIME_ATTACK_236_SPECIAL);
	}

	//	*m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_ATTACK_HEAVY || *m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_ATTACK_CROUCH_HEAVY)
	
}



void CFrieza_MeleeAttack::Attack_236Ultimate()
{

	if (CBattleInterface_Manager::Get_Instance()->Use_KiGuage(1, m_pPlayer->Get_iPlayerTeam()))
	{
		m_pPlayer->Set_Animation(CPlay_Frieza::ANIME_ULTIMATE);
	}

}

void CFrieza_MeleeAttack::Attack_214Final()
{
	if (CBattleInterface_Manager::Get_Instance()->Use_KiGuage(3, m_pPlayer->Get_iPlayerTeam()))
	{
		m_pPlayer->Set_Animation(CPlay_Frieza::ANIME_214_FINAL);
	}
}



void CFrieza_MeleeAttack::Attack_Crouch_Light()
{
	if (*m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_CROUCHING  || *m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_FORWARD_DASH || *m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_FORWARD_DASH_END)
	{
		m_pPlayer->Set_Animation(CPlay_Frieza::ANIME_ATTACK_CROUCH_LIGHT);
	}
}

void CFrieza_MeleeAttack::Attack_Crouch_Medium()
{

	//횟수 없으면 컷
	if (m_pbAttackCount[CPlay_Frieza::COUNT_ATTACK_CROUCH_MEDUIM] == false)
		return;




	if (*m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_CROUCHING || *m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_FORWARD_DASH || *m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_FORWARD_DASH_END)
	{
		m_pPlayer->Set_Animation(CPlay_Frieza::ANIME_ATTACK_CROUCH_MEDUIM);
		m_pbAttackCount[CPlay_Frieza::COUNT_ATTACK_CROUCH_MEDUIM] = false;
	}

	else if (*m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_ATTACK_CROUCH_LIGHT ||
		*m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_ATTACK_LIGHT1 || *m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_ATTACK_LIGHT2 ||
		*m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_ATTACK_MEDIUM)
	{
		m_pPlayer->Set_NextAnimation(CPlay_Frieza::ANIME_ATTACK_CROUCH_MEDUIM, 1.f);
		m_pbAttackCount[CPlay_Frieza::COUNT_ATTACK_CROUCH_MEDUIM] = false;
	}



	
}

void CFrieza_MeleeAttack::Attack_Crouch_Heavy()
{




	if (*m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_CROUCHING || *m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_FORWARD_DASH || *m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_FORWARD_DASH_END)
	{
		m_pPlayer->Set_Animation(CPlay_Frieza::ANIME_ATTACK_CROUCH_HEAVY);
	}
	
	else if(*m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_ATTACK_MEDIUM || *m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_ATTACK_HEAVY ||
		*m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_ATTACK_LIGHT1 || *m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_ATTACK_LIGHT2 || *m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_ATTACK_LIGHT3 ||
		*m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_ATTACK_CROUCH_LIGHT || *m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_ATTACK_CROUCH_MEDUIM || *m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_ATTACK_SPECIAL)
	{
		m_pPlayer->Set_NextAnimation(CPlay_Frieza::ANIME_ATTACK_CROUCH_HEAVY, 0.5f);
	}

	else if (*m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_JUMP_UP || *m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_JUMP_DOWN)
	{
		m_pPlayer->Set_Animation(CPlay_Frieza::ANIME_ATTACK_AIR3);
		//m_pPlayer->Set_fImpulse(1.6f * m_pPlayer->Get_iDirection());

	} 
	else if(*m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_ATTACK_AIR1 || *m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_ATTACK_AIR2 || *m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_ATTACK_SPECIAL_AIR)
	{
		m_pPlayer->Set_NextAnimation(CPlay_Frieza::ANIME_ATTACK_AIR3, 0.5f);
		//m_pPlayer->Set_fImpulse(1.6f * m_pPlayer->Get_iDirection());
		m_pbAttackCount[CPlay_Frieza::COUNT_ATTACK_CROUCH_MEDUIM] = false;
	}


	else if (m_pPlayer->Get_bAttackBackEvent() && *m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_ATTACK_SPECIAL)
	{
		m_pPlayer->Set_Animation(CPlay_Frieza::ANIME_ATTACK_CROUCH_HEAVY);
	}



}

void CFrieza_MeleeAttack::Attack_Crouch_Speical()
{
	if (m_pbAttackCount[CPlay_Frieza::COUNT_ATTACK_CROUCH_SPECIAL] == false)
		return;



	if (m_pPlayer->Check_bCurAnimationisGroundMove() || *m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_FORWARD_DASH || *m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_FORWARD_DASH_END)
	{
		m_pPlayer->Set_Animation(CPlay_Frieza::ANIME_ATTACK_CROUCH_SPECIAL);
		m_pbAttackCount[CPlay_Frieza::COUNT_ATTACK_CROUCH_SPECIAL] = false;

	}
	else if (m_pPlayer->Get_bAttackBackEvent() && (*m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_ATTACK_MEDIUM || *m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_ATTACK_HEAVY ||
		*m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_ATTACK_LIGHT1 || *m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_ATTACK_LIGHT2 || *m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_ATTACK_LIGHT3 ||
		*m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_ATTACK_CROUCH_LIGHT || *m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_ATTACK_CROUCH_MEDUIM || *m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_ATTACK_SPECIAL ||
		*m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_ATTACK_CROUCH_HEAVY)
		)
	{
		m_pPlayer->Set_Animation(CPlay_Frieza::ANIME_ATTACK_CROUCH_SPECIAL);
		m_pbAttackCount[CPlay_Frieza::COUNT_ATTACK_CROUCH_SPECIAL] = false;
	}
	


}

void CFrieza_MeleeAttack::Attack_Crouch_Crouch_Special()
{
	if(m_pPlayer->Check_bCurAnimationisAirMove() || m_pPlayer->Check_bCurAnimationisGroundMove())
		m_pPlayer->Set_Animation(CPlay_Frieza::ANIME_ATTACK_CROUCH_CROUCH_SPECIAL);

	else if (m_pPlayer->Get_bAttackBackEvent() &&  
		(*m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_ATTACK_HEAVY || *m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_ATTACK_SPECIAL || 
			*m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_ATTACK_SPECIAL_AIR || *m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_ATTACK_SPECIAL_AIR2
			))
	{
		m_pPlayer->Set_Animation(CPlay_Frieza::ANIME_ATTACK_CROUCH_CROUCH_SPECIAL);
	}
}

void CFrieza_MeleeAttack::BackDash()
{

	if (m_pPlayer->Check_bCurAnimationisGroundMove())
	{
		m_pPlayer->Set_Animation(CPlay_Frieza::ANIME_BACK_DASH);
	}

	else if (m_pPlayer->Get_bAirDashEnable() && (*m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_JUMP_DOWN || *m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_JUMP_UP))
	{
		m_pPlayer->Set_Animation(CPlay_Frieza::ANIME_BACK_DASH);
		m_pPlayer->Set_CurrentAnimationPositionJump(12.f);
		m_pPlayer->Set_fImpulse(m_pPlayer->Get_iDirection() *(-10.f));

		m_pPlayer->Set_bAirDashEnable(false);
		m_pPlayer->Set_ForcedGravityDown();
		m_pPlayer->Set_ForcveGravityTime(0.135f);


		//m_pPlayer->Character_Make_Effect(TEXT("Moving_Line_Left"),{1.f,0.f});
		m_pPlayer->Character_Make_Effect(TEXT("Moving_Line_Left"), { -0.3f,0.f });

		m_pGameInstance->Play_Sound(CSound_Manager::SOUND_KEY_NAME::Common_Back_Dash_SFX, false, 1.f);
	}
}

void CFrieza_MeleeAttack::ForwardDash()
{
	//if (m_pPlayer->Check_bCurAnimationisGroundMove())
	//지상대쉬
	if(*m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_IDLE || *m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_FORWARD_WALK || *m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_BACK_WALK)
	{
		m_pPlayer->Set_Animation(CPlay_Frieza::ANIME_FORWARD_DASH);
		m_pGameInstance->Play_Sound(CSound_Manager::SOUND_KEY_NAME::Common_Dash_Start_SFX, false, 1.f);

		CEffect_Layer::COPY_DESC tDesc{};
		tDesc.pPlayertMatrix = static_cast<CTransform*>(m_pPlayer->Get_Component(TEXT("Com_Transform")))->Get_WorldMatrixPtr();
		m_pEffect_Manager->Copy_Layer(TEXT("Smoke_Run"), &tDesc);

	}

	else if (m_pPlayer->Get_bAirDashEnable()  && (*m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_JUMP_DOWN || *m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_JUMP_UP ))
	{
		m_pPlayer->Set_Animation(CPlay_Frieza::ANIME_FORWARD_DASH);
		//m_pPlayer->Set_CurrentAnimationPositionJump(4.f);
		m_pPlayer->Set_fImpulse(m_pPlayer->Get_iDirection() * (10.f));
		m_pPlayer->Set_bAirDashEnable(false);
		//m_pPlayer->Set_ForcedGravityDown();

		m_pPlayer->Set_ForcveGravityTime(0.255f);

		//m_pPlayer->Character_Make_Effect(TEXT("Moving_Line_Right"));
		//m_pPlayer->Character_Make_Effect(TEXT("Ring_Dust_Right"), { 0.9f,0.f });

		//m_pEffect_Manager->Copy_Layer(TEXT("Dash"), m_pPlayer->Get_pTransformMatrix());

		//m_pPlayer->Character_Make_Effect(TEXT("Dash"), { 0.9f,0.f });
		m_pPlayer->Character_Make_Effect(TEXT("Dash"), { 1.2f,0.f });

		m_pGameInstance->Play_Sound(CSound_Manager::SOUND_KEY_NAME::Common_Back_Dash_SFX, false, 1.f);
	}

	else if (m_pPlayer->Get_bSparking() && m_pPlayer->Get_bAirDashEnable() && m_pPlayer->Get_bAttackBackEvent() && 
		(*m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_ATTACK_SPECIAL_AIR || *m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_ATTACK_AIR1 || *m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_ATTACK_AIR2))
	{
		m_pPlayer->Set_Animation(CPlay_Frieza::ANIME_FORWARD_DASH);
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

void CFrieza_MeleeAttack::Reflect()
{
	if (m_pPlayer->Check_bCurAnimationisGroundMove())
	{
		m_pPlayer->Set_Animation(CPlay_Frieza::ANIME_REFLECT);

	}

	else if (m_pPlayer->Get_bAttackBackEvent() && m_pPlayer->Check_bCurAnimationisReflect())
	{
		m_pPlayer->Set_Animation(CPlay_Frieza::ANIME_REFLECT);
		m_pPlayer->Set_CurrentAnimationPositionJump(0.f);

	}

}

void CFrieza_MeleeAttack::StopMacro(_float fStopTime)
{
	m_pPlayer->Set_AnimationStopWithoutMe(fStopTime);

	//이전에 만들어진 모든 Attack Object들 2초간 정지.  이거 본인은 이 직후에 만들어질테니 괜찮음
	for (auto pAttackObject : m_pGameInstance->Get_Layer(LEVEL_GAMEPLAY, TEXT("Layer_AttackObject")))
	{
		static_cast<CAttackObject*>(pAttackObject)->Set_UpdateStop(fStopTime);
	}
}

void CFrieza_MeleeAttack::Attack_Benishing()
{
	
	if(CBattleInterface_Manager::Get_Instance()->Get_KiNumber(m_pPlayer->Get_iPlayerTeam()) == 0 || m_pbAttackCount[CPlay_Frieza::COUNT_ATTACK_BENISHING] == false)
		return;


	if (m_pPlayer->Get_bAttackBackEvent() && 
		(*m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_ATTACK_AIR1 || *m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_ATTACK_AIR2 || *m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_ATTACK_AIR3 ||
			*m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_ATTACK_236 || *m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_ATTACK_214 ||
			*m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_ATTACK_LIGHT3 ||
			*m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_ATTACK_HEAVY || *m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_ATTACK_CROUCH_HEAVY) )
	{

		if(CBattleInterface_Manager::Get_Instance()->Use_KiGuage(1,m_pPlayer->Get_iPlayerTeam()))
		{		
			m_pbAttackCount[CPlay_Frieza::COUNT_ATTACK_BENISHING] = false;
			//m_pPlayer->Set_NextAnimation(CPlay_Frieza::ANIME_ATTACK_AIR2,2.f,0.f);
			m_pPlayer->Set_Animation(CPlay_Frieza::ANIME_ATTACK_AIR2);
			m_pPlayer->Set_CurrentAnimationPositionJump(44.99);
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
			m_pbAttackCount[CPlay_Frieza::COUNT_ATTACK_BENISHING] = false;
			//m_pPlayer->Set_NextAnimation(CPlay_Frieza::ANIME_ATTACK_AIR2,2.f,0.f);
			m_pPlayer->Set_Animation(CPlay_Frieza::ANIME_ATTACK_AIR2);
			m_pPlayer->Set_CurrentAnimationPositionJump(44.99);
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

void CFrieza_MeleeAttack::Attack_LowBenishing()
{
	if (m_pPlayer->Get_pEnemy()->Get_fHeight() > 1.5f)
		return;


	if (CBattleInterface_Manager::Get_Instance()->Get_KiNumber(m_pPlayer->Get_iPlayerTeam()) == 0 || m_pbAttackCount[CPlay_Frieza::COUNT_ATTACK_BENISHING] == false)
		return;

	

	if (m_pPlayer->Get_bAttackBackEvent() &&
		(*m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_ATTACK_AIR1 || *m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_ATTACK_AIR2 || *m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_ATTACK_AIR3 ||
			*m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_ATTACK_236 || *m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_ATTACK_214 ||
			*m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_ATTACK_LIGHT3 ||
			*m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_ATTACK_HEAVY || *m_pPlayerAnimationIndex == CPlay_Frieza::ANIME_ATTACK_CROUCH_HEAVY))
	{

		if (CBattleInterface_Manager::Get_Instance()->Use_KiGuage(1, m_pPlayer->Get_iPlayerTeam()))
		{
			m_pbAttackCount[CPlay_Frieza::COUNT_ATTACK_BENISHING] = false;
			//m_pPlayer->Set_NextAnimation(CPlay_Frieza::ANIME_ATTACK_AIR2,2.f,0.f);
			m_pPlayer->Set_Animation(CPlay_Frieza::ANIME_ATTACK_AIR2);
			m_pPlayer->Set_CurrentAnimationPositionJump(44.99);
			m_pPlayer->Set_AnimationStopWithoutMe(0.3f);
			m_pPlayer->Set_AnimationStop(0.3f);
			//샤샤샥
			m_pPlayer->Character_Make_Effect(TEXT("Moving_Line_Right"));

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
			m_pbAttackCount[CPlay_Frieza::COUNT_ATTACK_BENISHING] = false;
			//m_pPlayer->Set_NextAnimation(CPlay_Frieza::ANIME_ATTACK_AIR2,2.f,0.f);
			m_pPlayer->Set_Animation(CPlay_Frieza::ANIME_ATTACK_AIR2);
			m_pPlayer->Set_CurrentAnimationPositionJump(44.99);
			m_pPlayer->Set_AnimationStopWithoutMe(0.3f);
			m_pPlayer->Set_AnimationStop(0.3f);
			//샤샤샥
			m_pPlayer->Character_Make_Effect(TEXT("Moving_Line_Right"));

			m_pPlayer->Teleport_ToEnemy(1.5f, 0.3f);
			m_pPlayer->FlipDirection();

			m_pPlayer->Set_bBenishingAttack(true);
			m_pPlayer->Set_ForcedGravityDown();
			m_pPlayer->Set_bInivisible(true);
			m_pPlayer->Set_fImpulse({ 0.f,0.f });
		}
	}


}

void CFrieza_MeleeAttack::Frieza_Transform()
{

	if (m_pPlayer->Check_bCurAnimationisGroundMove() || m_pPlayer->Get_bGoldenFrieza() == false)
	{
		m_pPlayer->Set_Animation(CPlay_Frieza::ANIME_TRANSFORM_FINAL);
		//m_pPlayer->Set_AnimationStopWithoutMe(5.f);
	}
}

