#include "stdafx.h"
#include "..\Public\Goku_MeleeAttack.h"

#include "RenderInstance.h"
#include "GameInstance.h"
#include "Input.h"

#include "Play_Goku.h"

#include "Effect_Manager.h"

#include "AttackObject.h"
#include "Effect.h"
#include "Effect_Layer.h"
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

void CGoku_MeleeAttack::Initalize(CPlay_Goku* pPlayer)
{
	m_pPlayer = pPlayer;
	m_pPlayerAnimationIndex = m_pPlayer->Get_pAnimationIndex();


	m_pbAttackCount = m_pPlayer->Get_pbAttackCount();
	m_piCountGroundSpecial = m_pPlayer->Get_piSpecialCount();

	m_pGameInstance = CGameInstance::Get_Instance();
	m_pEffect_Manager = CEffect_Manager::Get_Instance();


	//	Safe_AddRef(m_pEffect_Manager);
}

void CGoku_MeleeAttack::Attack_Light()
{

	this;
	_bool bDebug = false;


	if (m_pPlayer->Check_bCurAnimationisGroundMove())
	{
		m_pPlayer->Set_Animation(CPlay_Goku::ANIME_ATTACK_LIGHT1);
		
	}

	else if (*m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_LIGHT1)
	{
		m_pPlayer->Set_NextAnimation(CPlay_Goku::ANIME_ATTACK_LIGHT2,1.f);
	}

	else if (*m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_LIGHT2)
	{
		m_pPlayer->Set_NextAnimation(CPlay_Goku::ANIME_ATTACK_LIGHT3, 1.f);
	}

	else if (*m_pPlayerAnimationIndex == CPlay_Goku::ANIME_JUMP_UP || *m_pPlayerAnimationIndex == CPlay_Goku::ANIME_JUMP_DOWN)
	{
		m_pPlayer->Set_Animation(CPlay_Goku::ANIME_ATTACK_AIR1);
	}
	else if (*m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_AIR1)
	{
		
		m_pPlayer->Set_NextAnimation(CPlay_Goku::ANIME_ATTACK_AIR2,0.5f);
		
		if (m_pPlayer->Get_fGravityTime() > 0.135)
		{
			m_pPlayer->Set_bNextAnimationGravityEvent();
		}
	}
	else if (*m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_AIR2)
	{
		m_pPlayer->Set_NextAnimation(CPlay_Goku::ANIME_ATTACK_AIR3,0.5f);
		if (m_pPlayer->Get_fGravityTime() > 0.135)
		{
			//m_pPlayer->Set_ForcedGravityTime_LittleUp();
			m_pPlayer->Set_bNextAnimationGravityEvent();
		}
	}

	
	else if (*m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_CROUCH_LIGHT)
	{
		m_pPlayer->Set_Animation(CPlay_Goku::ANIME_ATTACK_LIGHT1);
	}
	else if (*m_pPlayerAnimationIndex == CPlay_Goku::ANIME_FORWARD_DASH || *m_pPlayerAnimationIndex == CPlay_Goku::ANIME_FORWARD_DASH_END)
	{
		m_pPlayer->Set_Animation(CPlay_Goku::ANIME_ATTACK_LIGHT1);
	}
}

void CGoku_MeleeAttack::Attack_Medium()
{


	if (m_pPlayer->Check_bCurAnimationisGroundMove(*m_pPlayerAnimationIndex))
	{
		m_pPlayer->Set_Animation(CPlay_Goku::ANIME_ATTACK_MEDIUM);
		m_pbAttackCount[CPlay_Goku::COUNT_ATTACK_MEDIUM] = false;

		m_pbAttackCount[CPlay_Goku::COUNT_ATTACK_MEDIUM];
		m_pbAttackCount[CPlay_Goku::COUNT_ATTACK_CROUCH_MEDUIM];
		m_pbAttackCount;

		_bool bDebug = true;
	}
	
	else if(*m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_LIGHT1 || *m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_LIGHT2
		|| *m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_CROUCH_LIGHT || *m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_CROUCH_MEDUIM)
	{

		//서서 중공격 횟수가 남아있으면 일단 사용
		if (m_pbAttackCount[CPlay_Goku::COUNT_ATTACK_MEDIUM] == true)
		{
			m_pPlayer->Set_NextAnimation(CPlay_Goku::ANIME_ATTACK_MEDIUM, 0.5f);
			m_pbAttackCount[CPlay_Goku::COUNT_ATTACK_MEDIUM] = false;
		}

	}

	else if (*m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_MEDIUM)
	{
		//서서 중공격 중에 또 누를 시 횟수가 있으면  앉아 중공격 자동 사용
		if (m_pbAttackCount[CPlay_Goku::COUNT_ATTACK_CROUCH_MEDUIM] == true)
		{
			m_pPlayer->Set_NextAnimation(CPlay_Goku::ANIME_ATTACK_CROUCH_MEDUIM,0.5f);
			m_pbAttackCount[CPlay_Goku::COUNT_ATTACK_CROUCH_MEDUIM] = false;
		}
	}

	
	else if (*m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_AIR1)
	{
		m_pPlayer->Set_NextAnimation(CPlay_Goku::ANIME_ATTACK_AIR2, 0.5f);
	}
	
	else if (*m_pPlayerAnimationIndex == CPlay_Goku::ANIME_JUMP_UP || *m_pPlayerAnimationIndex == CPlay_Goku::ANIME_JUMP_DOWN)
	{
		m_pPlayer->Set_Animation(CPlay_Goku::ANIME_ATTACK_AIR2);
	}


	//다음애니메이션으로 설정하는 방식이지만 약공격이 빨라서 이상함
	//if (m_pPlayer->Check_bCurAnimationisGroundMove(*m_pPlayerAnimationIndex))
	//{
	//	m_pPlayer->Set_Animation(CPlay_Goku::ANIME_ATTACK_MEDIUM);
	//}
	//else if(*m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_LIGHT1 || *m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_LIGHT2
	//	|| *m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_CROUCH_LIGHT || *m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_CROUCH_MEDUIM)
	//{
	//	m_pPlayer->Set_NextAnimation(CPlay_Goku::ANIME_ATTACK_MEDIUM,0.5f);
	//}
	//
	//else if (*m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_AIR1)
	//{
	//	m_pPlayer->Set_NextAnimation(CPlay_Goku::ANIME_ATTACK_AIR2, 0.5f);
	//}

	
	
}

void CGoku_MeleeAttack::Attack_Heavy()
{
	//if (m_pPlayer->Check_bCurAnimationisGroundMove() || *m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_MEDIUM ||
	//	*m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_LIGHT1 || *m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_LIGHT2 ||
	//	*m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_CROUCH_LIGHT || *m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_CROUCH_MEDUIM ||
	//	*m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_SPECIAL)
	//{
	//	m_pPlayer->Set_Animation(CPlay_Goku::ANIME_ATTACK_HEAVY);
	//	//m_pPlayer->Set_NextAnimation(CPlay_Goku::ANIME_ATTACK_HEAVY, 0.5f);
	//}
	//
	//else if (*m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_AIR2)
	//{
	//	m_pPlayer->Set_NextAnimation(CPlay_Goku::ANIME_ATTACK_AIR3, 0.5f);
	//}
	

	if (m_pPlayer->Check_bCurAnimationisGroundMove() || *m_pPlayerAnimationIndex == CPlay_Goku::ANIME_FORWARD_DASH || *m_pPlayerAnimationIndex == CPlay_Goku::ANIME_FORWARD_DASH_END)
	{
		m_pPlayer->Set_Animation(CPlay_Goku::ANIME_ATTACK_HEAVY);
		//m_pPlayer->Set_NextAnimation(CPlay_Goku::ANIME_ATTACK_HEAVY, 0.5f);
	}
	else if (*m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_MEDIUM ||
		*m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_LIGHT1 || *m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_LIGHT2 ||
		*m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_CROUCH_LIGHT || *m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_CROUCH_MEDUIM ||
		*m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_SPECIAL)
	{
		m_pPlayer->Set_NextAnimation(CPlay_Goku::ANIME_ATTACK_HEAVY, 0.5f);
	}
	else if (*m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_AIR2)
	{
		m_pPlayer->Set_NextAnimation(CPlay_Goku::ANIME_ATTACK_AIR3, 0.5f);
	}
	else if (*m_pPlayerAnimationIndex == CPlay_Goku::ANIME_JUMP_UP  || *m_pPlayerAnimationIndex == CPlay_Goku::ANIME_JUMP_DOWN)
	{
		m_pPlayer->Set_Animation(CPlay_Goku::ANIME_ATTACK_AIR3);
	}
}

void CGoku_MeleeAttack::Attack_Special()
{
	if (m_pPlayer->Check_bCurAnimationisGroundMove(*m_pPlayerAnimationIndex))
	{
		m_pPlayer->Set_Animation(CPlay_Goku::ANIME_ATTACK_SPECIAL);
		//(*m_piCountGroundSpecial)++;
	} 
	else if(*m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_LIGHT1 || *m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_LIGHT2 || *m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_MEDIUM ||
			*m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_CROUCH_LIGHT || *m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_CROUCH_MEDUIM)
	{
		//m_pPlayer->Set_NextAnimation(CPlay_Goku::ANIME_ATTACK_SPECIAL, 0.5f);
		m_pPlayer->Set_NextAnimation(CPlay_Goku::ANIME_ATTACK_SPECIAL, 0.5f);
		//(*m_piCountGroundSpecial)++;

	}

	else if (*m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_SPECIAL)
	{
		_ushort iCount = *m_piCountGroundSpecial;

		
		//0에선 더할 수 없으니 5대신 4,    4발째일때까지 다음공격 가능하니 이하 대신 미만.
		if (iCount < 4)
		{

			//m_pPlayer->Set_NextAnimation(CPlay_Goku::ANIME_ATTACK_SPECIAL, 0.5f);
			
			//홀짝 판별 비트연산 (최적화)  true일때 홀수
			if (iCount & 1)  //홀수만큼 썻으니 왼손
			{

				//if (m_pPlayer->CompareNextAnimation(CPlay_Goku::ANIME_ATTACK_SPECIAL, 10.f) == false)

				if (m_pPlayer->CompareNextAnimation(CPlay_Goku::ANIME_IDLE))
				{
					//m_pPlayer->Set_NextAnimation(CPlay_Goku::ANIME_ATTACK_SPECIAL, 0.5f, 10.f);
					m_pPlayer->Set_NextAnimation(CPlay_Goku::ANIME_ATTACK_SPECIAL, 0.5f, 10.f);

					//cout << "TEST1" << endl;
					(*m_piCountGroundSpecial)++;
				}
				
			}
			else
			{
				//if (m_pPlayer->CompareNextAnimation(CPlay_Goku::ANIME_ATTACK_SPECIAL, 55.f) == false)
				if (m_pPlayer->CompareNextAnimation(CPlay_Goku::ANIME_IDLE))
				{
					m_pPlayer->Set_NextAnimation(CPlay_Goku::ANIME_ATTACK_SPECIAL, 0.5f, 55);
					//cout << "TEST2" << endl;

					(*m_piCountGroundSpecial)++;
				}

			}


		}
		else
		{
			(*m_piCountGroundSpecial)++;
		}
	}


	//else if (*m_pPlayerAnimationIndex == CPlay_Goku::ANIME_JUMP_UP || *m_pPlayerAnimationIndex == CPlay_Goku::ANIME_JUMP_DOWN ||
	//	*m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_AIR1 || *m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_AIR2)
	//{
	//	m_pPlayer->Set_Animation(CPlay_Goku::ANIME_ATTACK_SPECIAL_AIR);
	//}


	else if (*m_pPlayerAnimationIndex == CPlay_Goku::ANIME_JUMP_UP || *m_pPlayerAnimationIndex == CPlay_Goku::ANIME_JUMP_DOWN)
	{
		m_pPlayer->Set_Animation(CPlay_Goku::ANIME_ATTACK_SPECIAL_AIR);
	

	}
	else if(*m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_AIR1 || *m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_AIR2)
	{
		m_pPlayer->Set_NextAnimation(CPlay_Goku::ANIME_ATTACK_SPECIAL_AIR,0.5f);


	}
	

}

void CGoku_MeleeAttack::Attack_Grab()
{
	
	if(m_pbAttackCount[CPlay_Goku::COUNT_ATTACK_GRAB])
	{

		if (m_pPlayer->Check_bCurAnimationisGroundMove())
		{
			m_pPlayer->Set_Grab(false);
			m_pPlayer->Set_Animation(CPlay_Goku::ANIME_GRAB_READY);
			m_pPlayer->Set_NextAnimation(CPlay_Goku::ANIME_GRAB, 3.f, 5.f);
			m_pPlayer->Set_GrabLoofCount(2);

			_uint iEffectCount = 0;
			CEffect_Layer* pLayer = m_pPlayer->Character_Make_Effect(TEXT("DIR_K"));
			for (auto& iter : pLayer->m_MixtureEffects)
			{
				if (iEffectCount == 1)
					iter->m_vColor = _float4(0.f, 255.f, 0.f, 0.3f);
				iEffectCount++;
			}

		}

		else if (*m_pPlayerAnimationIndex == CPlay_Goku::ANIME_JUMP_DOWN || *m_pPlayerAnimationIndex == CPlay_Goku::ANIME_JUMP_UP)
		{
			m_pPlayer->Set_Grab(true);
			m_pPlayer->Set_Animation(CPlay_Goku::ANIME_GRAB_READY);
			m_pPlayer->Set_NextAnimation(CPlay_Goku::ANIME_GRAB, 3.f, 5.f);
			m_pPlayer->Set_GrabLoofCount(2);

			_uint iEffectCount = 0;
			CEffect_Layer* pLayer = m_pPlayer->Character_Make_Effect(TEXT("DIR_K"));
			for (auto& iter : pLayer->m_MixtureEffects)
			{
				if(iEffectCount == 1)
					iter->m_vColor = _float4(0.f, 255.f, 0.f, 0.2f);
				iEffectCount++;
			}

		}
	}

}

void CGoku_MeleeAttack::Attack_236()
{

	m_pPlayer->Set_bHeavySkill(false);
	//공중인지 아닌지 상태 받고,  사용중인게 필살기가 아닌지 구분.  피격중은 키 lock을 해버릴것

	if (m_pPlayer->Check_bCurAnimationisGroundMove() || *m_pPlayerAnimationIndex == CPlay_Goku::ANIME_FORWARD_DASH || *m_pPlayerAnimationIndex == CPlay_Goku::ANIME_FORWARD_DASH_END)
	{
		m_pPlayer->Set_Animation(CPlay_Goku::ANIME_ATTACK_236);
		m_pPlayer->Set_fImpulse(2.f *m_pPlayer->Get_iDirection());
		m_pPlayer->Set_bAttackGravity(false);


		//m_pPlayer->Teleport_ToEnemy(1.f, 0.5f);
		//m_pPlayer->FlipDirection();
	} 
	
	else if(*m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_MEDIUM ||  *m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_HEAVY ||
		*m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_LIGHT1 || *m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_LIGHT2  || *m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_LIGHT3 ||
		*m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_SPECIAL || *m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_CROUCH_LIGHT || *m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_CROUCH_MEDUIM)
	{
		m_pPlayer->Set_NextAnimation(CPlay_Goku::ANIME_ATTACK_236,0.5f);
		m_pPlayer->Set_fImpulse(2.f * m_pPlayer->Get_iDirection());
		m_pPlayer->Set_bAttackGravity(false);

	}
	
	
	else if (*m_pPlayerAnimationIndex == CPlay_Goku::ANIME_JUMP_UP || *m_pPlayerAnimationIndex == CPlay_Goku::ANIME_JUMP_DOWN)
	{
		m_pPlayer->Set_Animation(CPlay_Goku::ANIME_ATTACK_236_AIR);
		//m_pPlayer->Set_fImpulse(2.f * m_pPlayer->Get_iDirection());
		m_pPlayer->Set_fImpulse({ 2.f * m_pPlayer->Get_iDirection(),-1.f });
		m_pPlayer->Set_bAttackGravity(false);
		
		//m_pPlayer->Set_ForcedGravityDown();
		m_pPlayer->Set_fGravityTime(0.265f);

	}
	else if(*m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_AIR1  || *m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_AIR2 || *m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_SPECIAL_AIR)
	{
		m_pPlayer->Set_NextAnimation(CPlay_Goku::ANIME_ATTACK_236_AIR, 0.5f);
		//m_pPlayer->Set_fImpulse(2.f * m_pPlayer->Get_iDirection());
		m_pPlayer->Set_fImpulse({ 2.f * m_pPlayer->Get_iDirection(),-1.f });

		m_pPlayer->Set_bAttackGravity(false);
		//m_pPlayer->Set_ForcedGravityDown();

		m_pPlayer->Set_fGravityTime(0.265f);

	}


	


}

void CGoku_MeleeAttack::Attack_236_Heavy()
{

	_bool bHaveKi = false;

	//기 2칸 있을 때
	if (CBattleInterface_Manager::Get_Instance()->Use_KiRealGuage(50, m_pPlayer->Get_iPlayerTeam()))
	{
		//m_pPlayer->Set_Animation(CPlay_Goku::ANIME_236_POSE_HEAVY);
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

		if (m_pPlayer->Check_bCurAnimationisGroundMove() || *m_pPlayerAnimationIndex == CPlay_Goku::ANIME_FORWARD_DASH || *m_pPlayerAnimationIndex == CPlay_Goku::ANIME_FORWARD_DASH_END)
		{
			m_pPlayer->Set_Animation(CPlay_Goku::ANIME_ATTACK_236);
			m_pPlayer->Set_fImpulse(2.f * m_pPlayer->Get_iDirection());
			m_pPlayer->Set_bAttackGravity(false);


			m_pPlayer->Set_AnimationStop(0.1f);
			m_pPlayer->Character_Create_Distortion({ 1.f,0.f,0.f }, {0.f,0.f});
			//m_pPlayer->Set_bInivisible(true);

			//_float fEnemyPosX = m_pPlayer->Get_pEnemy()->Get_fPositionX();

			m_pPlayer->Character_Make_Effect(TEXT("DIR_K"));


			//if (abs(m_pPlayer->Get_pEnemy()->Get_fPositionX()) < 10)
			//{
			//	m_pPlayer->Teleport_ToEnemy(2.f, 0.f);
			//	m_pPlayer->Add_Move({ 0.f,-m_pPlayer->Get_fHeight() });
			//
			//	m_pPlayer->FlipDirection();
			//}
			//else
			//	m_pPlayer->Teleport_ToEnemy(-2.f, 0.2f);
		}

		else if (m_pPlayer->Get_bAttackBackEvent() && (*m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_MEDIUM || *m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_HEAVY ||
			*m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_LIGHT1 || *m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_LIGHT2 || *m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_LIGHT3 ||
			*m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_SPECIAL || *m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_CROUCH_LIGHT || *m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_CROUCH_MEDUIM))
		{
			///m_pPlayer->Set_NextAnimation(CPlay_Goku::ANIME_ATTACK_236, 0.5f);
			m_pPlayer->Set_Animation(CPlay_Goku::ANIME_ATTACK_236,false);
			m_pPlayer->Set_fImpulse(2.f * m_pPlayer->Get_iDirection());
			m_pPlayer->Set_bAttackGravity(false);
			m_pPlayer->Character_Make_Effect(TEXT("DIR_K"));

			m_pPlayer->Set_AnimationStop(0.1f);
			m_pPlayer->Character_Create_Distortion({ 1.f,0.f,0.f }, { 0.f,0.f });
			//m_pPlayer->Set_bInivisible(true);
			//if (abs(m_pPlayer->Get_pEnemy()->Get_fPositionX()) < 10)
			//{
			//	m_pPlayer->Teleport_ToEnemy(2.f, 0.f);
			//	m_pPlayer->Add_Move({ 0.f,-m_pPlayer->Get_fHeight() });
			//
			//	m_pPlayer->FlipDirection();
			//}
			//else
			//	m_pPlayer->Teleport_ToEnemy(-2.f, 0.2f);
		}


		else if (*m_pPlayerAnimationIndex == CPlay_Goku::ANIME_JUMP_UP || *m_pPlayerAnimationIndex == CPlay_Goku::ANIME_JUMP_DOWN)
		{
			m_pPlayer->Set_Animation(CPlay_Goku::ANIME_ATTACK_236_AIR);
			//m_pPlayer->Set_fImpulse(2.f * m_pPlayer->Get_iDirection());
			m_pPlayer->Set_fImpulse({ 2.f * m_pPlayer->Get_iDirection(),-1.f });
			m_pPlayer->Set_bAttackGravity(false);

			//m_pPlayer->Set_ForcedGravityDown();
			m_pPlayer->Set_fGravityTime(0.265f);

			m_pPlayer->Character_Make_Effect(TEXT("DIR_K"));

			m_pPlayer->Set_AnimationStop(0.1f);
			m_pPlayer->Character_Create_Distortion({ 1.f,0.f,0.f }, { 0.f,0.f });
			//m_pPlayer->Set_bInivisible(true);
			//if (abs(m_pPlayer->Get_pEnemy()->Get_fPositionX()) < 10)
			//{
			//	m_pPlayer->Teleport_ToEnemy(2.f, 0.2f);
			//	m_pPlayer->FlipDirection();
			//}
			//else
			//	m_pPlayer->Teleport_ToEnemy(-2.f, 0.2f);

		}
		else if (m_pPlayer->Get_bAttackBackEvent() && (*m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_AIR1 || *m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_AIR2 || *m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_SPECIAL_AIR))
		{
			//m_pPlayer->Set_NextAnimation(CPlay_Goku::ANIME_ATTACK_236_AIR, 0.5f);
			m_pPlayer->Set_Animation(CPlay_Goku::ANIME_ATTACK_236_AIR, false);
			//m_pPlayer->Set_fImpulse(2.f * m_pPlayer->Get_iDirection());
			m_pPlayer->Set_fImpulse({ 2.f * m_pPlayer->Get_iDirection(),-1.f });

			m_pPlayer->Set_bAttackGravity(false);
			//m_pPlayer->Set_ForcedGravityDown();

			m_pPlayer->Set_fGravityTime(0.265f);
			m_pPlayer->Character_Make_Effect(TEXT("DIR_K"));

			//m_pPlayer->Set_AnimationStop(0.3f);
			m_pPlayer->Character_Create_Distortion({ 1.f,0.f,0.f }, { 0.f,0.f });
			//m_pPlayer->Set_bInivisible(true);
	
			//if(abs(m_pPlayer->Get_pEnemy()->Get_fPositionX())<10)
			//{
			//	m_pPlayer->Teleport_ToEnemy(2.f, 0.2f);
			//	m_pPlayer->FlipDirection();
			//}
			//else
			//	m_pPlayer->Teleport_ToEnemy(-2.f, 0.2f);

		}

		//사용 못했으면
		else
		{
			CBattleInterface_Manager::Get_Instance()->Gain_KiGuage(25, m_pPlayer->Get_iPlayerTeam());
			m_pPlayer->Set_bHeavySkill(false);
		}
	}
}

void CGoku_MeleeAttack::Attack_214()
{
	


	if (m_pPlayer->Check_bCurAnimationisGroundMove() || *m_pPlayerAnimationIndex == CPlay_Goku::ANIME_FORWARD_DASH || *m_pPlayerAnimationIndex == CPlay_Goku::ANIME_FORWARD_DASH_END)
	{
		m_pPlayer->Set_Animation(CPlay_Goku::ANIME_ATTACK_214);
		m_pPlayer->Set_fImpulse(2.5f * m_pPlayer->Get_iDirection());
		m_pPlayer->Set_bAttackGravity(false);
		m_pPlayer->Set_ForcedGravityDown();


	}
	else if (*m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_MEDIUM ||
		*m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_LIGHT1 || *m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_LIGHT2 || *m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_LIGHT3 ||
		*m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_HEAVY || *m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_SPECIAL ||
		*m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_CROUCH_LIGHT || *m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_CROUCH_MEDUIM)
	{
		m_pPlayer->Set_NextAnimation(CPlay_Goku::ANIME_ATTACK_214, 0.5f);
		m_pPlayer->Set_fImpulse(2.5f * m_pPlayer->Get_iDirection());
		m_pPlayer->Set_bAttackGravity(false);
		m_pPlayer->Set_ForcedGravityDown();
	}


	else if (*m_pPlayerAnimationIndex == CPlay_Goku::ANIME_JUMP_UP || *m_pPlayerAnimationIndex == CPlay_Goku::ANIME_JUMP_DOWN)
	{
		m_pPlayer->Set_Animation(CPlay_Goku::ANIME_ATTACK_214);
		m_pPlayer->Set_fImpulse(2.5f * m_pPlayer->Get_iDirection());
		m_pPlayer->Set_bAttackGravity(false);
		m_pPlayer->Set_CurrentAnimationPositionJump(20.f);
		m_pPlayer->Set_ForcedGravityDown();

	}
	else if (*m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_AIR1 || *m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_AIR2 || *m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_SPECIAL_AIR)
	{

		if (m_pPlayer->Get_bAttackBackEvent())
		{
			m_pPlayer->Set_Animation(CPlay_Goku::ANIME_ATTACK_214);
			m_pPlayer->Set_fImpulse(2.5f * m_pPlayer->Get_iDirection());
			m_pPlayer->Set_bAttackGravity(false);
			m_pPlayer->Set_CurrentAnimationPositionJump(20.f);
			m_pPlayer->Set_ForcedGravityDown();
		}
		else
		{
			m_pPlayer->Set_NextAnimation(CPlay_Goku::ANIME_ATTACK_214, 0.5f, 20.f);
			m_pPlayer->Set_fImpulse(2.5f * m_pPlayer->Get_iDirection());
			m_pPlayer->Set_bAttackGravity(false);
			m_pPlayer->Set_ForcedGravityDown();
		}

	}


}

void CGoku_MeleeAttack::Attack_236Special()
{

	//if (m_pPlayer->Check_bCurAnimationisGroundMove() ||		*m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_MEDIUM || *m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_HEAVY ||
	//	*m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_LIGHT1 || *m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_LIGHT2 || *m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_LIGHT3 )
	//{
	//	m_pPlayer->Set_Animation(CPlay_Goku::ANIME_ATTACK_236_SPECIAL);
	//}
	//
	//else if (*m_pPlayerAnimationIndex == CPlay_Goku::ANIME_JUMP_UP || *m_pPlayerAnimationIndex == CPlay_Goku::ANIME_JUMP_DOWN ||
	//	*m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_AIR1 || *m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_AIR2)
	//{
	//	m_pPlayer->Set_Animation(CPlay_Goku::ANIME_ATTACK_236_SPECIAL_AIR);
	//}


	if (m_pPlayer->Check_bCurAnimationisGroundMove() || *m_pPlayerAnimationIndex == CPlay_Goku::ANIME_FORWARD_DASH || *m_pPlayerAnimationIndex == CPlay_Goku::ANIME_FORWARD_DASH_END)
	{
		m_pPlayer->Set_Animation(CPlay_Goku::ANIME_ATTACK_236_SPECIAL);
		m_pPlayer->Set_fImpulse(2.5f * m_pPlayer->Get_iDirection());

		m_pPlayer->Set_UltimateKamehameha(false);

	} 
	
	else if(*m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_MEDIUM || *m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_HEAVY ||
		*m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_LIGHT1 || *m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_LIGHT2 || *m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_LIGHT3 ||
		*m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_SPECIAL)
	{
		m_pPlayer->Set_NextAnimation(CPlay_Goku::ANIME_ATTACK_236_SPECIAL,3.5f);
		m_pPlayer->Set_fImpulse(2.5f * m_pPlayer->Get_iDirection());
		m_pPlayer->Set_UltimateKamehameha(false);

	}

	else if (*m_pPlayerAnimationIndex == CPlay_Goku::ANIME_JUMP_UP || *m_pPlayerAnimationIndex == CPlay_Goku::ANIME_JUMP_DOWN)
	{
		m_pPlayer->Set_Animation(CPlay_Goku::ANIME_ATTACK_236_SPECIAL_AIR);
		m_pPlayer->Set_fImpulse(2.5f * m_pPlayer->Get_iDirection());
		m_pPlayer->Set_UltimateKamehameha(false);


	} 
	else if(*m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_AIR1 || *m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_AIR2 ||
		*m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_SPECIAL_AIR)
	{
		m_pPlayer->Set_NextAnimation(CPlay_Goku::ANIME_ATTACK_236_SPECIAL_AIR, 3.5f);
		m_pPlayer->Set_fImpulse(2.5f * m_pPlayer->Get_iDirection());
		m_pPlayer->Set_UltimateKamehameha(false);


	}

}

void CGoku_MeleeAttack::Attack_236Special_Side()
{


	if (m_pPlayer->Check_bCurAnimationisGroundMove())
	{
		m_pPlayer->Set_Animation(CPlay_Goku::ANIME_ATTACK_236_SPECIAL_UPSIDE);
		m_pPlayer->Set_UltimateKamehameha(false);

	}

	else if (*m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_MEDIUM || *m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_HEAVY ||
		*m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_LIGHT1 || *m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_LIGHT2 || *m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_LIGHT3 ||
		*m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_SPECIAL || *m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_236_SPECIAL || *m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_236_SPECIAL_UPSIDE)
	{
		m_pPlayer->Set_NextAnimation(CPlay_Goku::ANIME_ATTACK_236_SPECIAL_UPSIDE, 3.5f);	
		m_pPlayer->Set_UltimateKamehameha(false);

	}

	else if (*m_pPlayerAnimationIndex == CPlay_Goku::ANIME_JUMP_UP || *m_pPlayerAnimationIndex == CPlay_Goku::ANIME_JUMP_DOWN)
	{
		m_pPlayer->Set_Animation(CPlay_Goku::ANIME_ATTACK_236_SPECIAL_AIR_DOWNSIDE);
		m_pPlayer->Set_UltimateKamehameha(false);
	}
	else if (*m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_AIR1 || *m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_AIR2 ||
		*m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_SPECIAL_AIR )
	{
		m_pPlayer->Set_NextAnimation(CPlay_Goku::ANIME_ATTACK_236_SPECIAL_AIR_DOWNSIDE, 3.f);
		m_pPlayer->Set_UltimateKamehameha(false);
	}
	




}

void CGoku_MeleeAttack::Attack_236Ultimate()
{


	//필살기는 모션을 끊고 쓰는것처럼 보이는데 자세히 보면 기술마다 다름
	// 에너지파는 끊고 쓰고,   공격은 타격판정 난 이후? 

	if (m_pPlayer->Check_bCurAnimationisGroundMove() || *m_pPlayerAnimationIndex == CPlay_Goku::ANIME_FORWARD_DASH || *m_pPlayerAnimationIndex == CPlay_Goku::ANIME_FORWARD_DASH_END)
	{
		if (CBattleInterface_Manager::Get_Instance()->Use_KiGuage(1, m_pPlayer->Get_iPlayerTeam()))
		{
			m_pPlayer->Set_Animation(CPlay_Goku::ANIME_ATTACK_236_SPECIAL);
			m_pPlayer->Set_UltimateKamehameha(true);
		}
		//m_pPl
		//StopMacro(1.f);
		//m_pPlayer->Set_AnimationStop(1.f);
	}

	else if ((*m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_MEDIUM || *m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_HEAVY ||
		*m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_LIGHT1 || *m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_LIGHT2 || *m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_LIGHT3 ||
		*m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_SPECIAL || *m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_236  ||
		*m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_236_SPECIAL || *m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_236_SPECIAL_UPSIDE) )
	{
		if (CBattleInterface_Manager::Get_Instance()->Use_KiGuage(1, m_pPlayer->Get_iPlayerTeam()))
		{
			m_pPlayer->Set_NextAnimation(CPlay_Goku::ANIME_ATTACK_236_SPECIAL, 3.5f);
			//m_pPlayer->Set_Animation(CPlay_Goku::ANIME_ATTACK_236_SPECIAL);
			m_pPlayer->Set_UltimateKamehameha(true);
		}
	}

	else if (*m_pPlayerAnimationIndex == CPlay_Goku::ANIME_JUMP_UP || *m_pPlayerAnimationIndex == CPlay_Goku::ANIME_JUMP_DOWN)
	{
		if (CBattleInterface_Manager::Get_Instance()->Use_KiGuage(1, m_pPlayer->Get_iPlayerTeam()))
		{
			m_pPlayer->Set_Animation(CPlay_Goku::ANIME_ATTACK_236_SPECIAL_AIR);
			m_pPlayer->Set_UltimateKamehameha(true);
		}
	}
	else if (*m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_AIR1 || *m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_AIR2 ||
		*m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_SPECIAL_AIR || *m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_236_AIR ||
		*m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_236_SPECIAL_AIR || *m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_236_SPECIAL_AIR_DOWNSIDE)
	{
		if (CBattleInterface_Manager::Get_Instance()->Use_KiGuage(1, m_pPlayer->Get_iPlayerTeam()))
		{
			m_pPlayer->Set_NextAnimation(CPlay_Goku::ANIME_ATTACK_236_SPECIAL_AIR, 3.5f);
			//m_pPlayer->Set_Animation(CPlay_Goku::ANIME_ATTACK_236_SPECIAL_AIR);
			m_pPlayer->Set_UltimateKamehameha(true);
		}
	}


	//214 애니메이션은 지상,공중 구분이 없는데 어떻게 처리할것인가
	//높이 기준이 일단 0. 바뀔수도 있음

	else if (*m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_214)
	{
		if(m_pPlayer->Get_fHeight() > CPlay_Goku::fGroundHeight)
		{
			if (CBattleInterface_Manager::Get_Instance()->Use_KiGuage(1, m_pPlayer->Get_iPlayerTeam()))
			{
				m_pPlayer->Set_NextAnimation(CPlay_Goku::ANIME_ATTACK_236_SPECIAL_AIR, 3.5f);
				//m_pPlayer->Set_Animation(CPlay_Goku::ANIME_ATTACK_236_SPECIAL_AIR);
				m_pPlayer->Set_UltimateKamehameha(true);
			}
		}
		else
		{
			if (CBattleInterface_Manager::Get_Instance()->Use_KiGuage(1, m_pPlayer->Get_iPlayerTeam()))
			{
				m_pPlayer->Set_NextAnimation(CPlay_Goku::ANIME_ATTACK_236_SPECIAL, 3.5f);
				//m_pPlayer->Set_Animation(CPlay_Goku::ANIME_ATTACK_236_SPECIAL);
				m_pPlayer->Set_UltimateKamehameha(true);
			}

		}
	}


}

void CGoku_MeleeAttack::Attack_236Ultimate_Side()
{


	if (m_pPlayer->Check_bCurAnimationisGroundMove() || *m_pPlayerAnimationIndex == CPlay_Goku::ANIME_FORWARD_DASH || *m_pPlayerAnimationIndex == CPlay_Goku::ANIME_FORWARD_DASH_END)
	{
		if (CBattleInterface_Manager::Get_Instance()->Use_KiGuage(1, m_pPlayer->Get_iPlayerTeam()))
		{
			m_pPlayer->Set_Animation(CPlay_Goku::ANIME_ATTACK_236_SPECIAL_UPSIDE);
			m_pPlayer->Set_UltimateKamehameha(true);
		}
	}

	else if (*m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_MEDIUM || *m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_HEAVY ||
		*m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_LIGHT1 || *m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_LIGHT2 || *m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_LIGHT3 ||
		*m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_SPECIAL || *m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_236 ||
		*m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_236_SPECIAL || *m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_236_SPECIAL_UPSIDE)
	{
		if (CBattleInterface_Manager::Get_Instance()->Use_KiGuage(1, m_pPlayer->Get_iPlayerTeam()))
		{
			m_pPlayer->Set_NextAnimation(CPlay_Goku::ANIME_ATTACK_236_SPECIAL_UPSIDE, 3.5f);
			//m_pPlayer->Set_Animation(CPlay_Goku::ANIME_ATTACK_236_SPECIAL);
			m_pPlayer->Set_UltimateKamehameha(true);
		}
	}

	else if (*m_pPlayerAnimationIndex == CPlay_Goku::ANIME_JUMP_UP || *m_pPlayerAnimationIndex == CPlay_Goku::ANIME_JUMP_DOWN)
	{
		if (CBattleInterface_Manager::Get_Instance()->Use_KiGuage(1, m_pPlayer->Get_iPlayerTeam()))
		{
			m_pPlayer->Set_Animation(CPlay_Goku::ANIME_ATTACK_236_SPECIAL_AIR_DOWNSIDE);
			m_pPlayer->Set_UltimateKamehameha(true);
		}
	}
	else if (*m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_AIR1 || *m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_AIR2 ||
		*m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_SPECIAL_AIR || *m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_236_AIR ||
		*m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_236_SPECIAL_AIR || *m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_236_SPECIAL_AIR_DOWNSIDE)
	{
		if (CBattleInterface_Manager::Get_Instance()->Use_KiGuage(1, m_pPlayer->Get_iPlayerTeam()))
		{
			m_pPlayer->Set_NextAnimation(CPlay_Goku::ANIME_ATTACK_236_SPECIAL_AIR_DOWNSIDE, 3.5f);
			//m_pPlayer->Set_Animation(CPlay_Goku::ANIME_ATTACK_236_SPECIAL_AIR);
			m_pPlayer->Set_UltimateKamehameha(true);
		}
	}


	//214 애니메이션은 지상,공중 구분이 없는데 어떻게 처리할것인가
	//높이 기준이 일단 0. 바뀔수도 있음

	else if (*m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_214)
	{
		if (CBattleInterface_Manager::Get_Instance()->Use_KiGuage(1, m_pPlayer->Get_iPlayerTeam()))
		{
			if (m_pPlayer->Get_fHeight() > CPlay_Goku::fGroundHeight)
			{
				m_pPlayer->Set_NextAnimation(CPlay_Goku::ANIME_ATTACK_236_SPECIAL_AIR_DOWNSIDE, 3.5f);
				//m_pPlayer->Set_Animation(CPlay_Goku::ANIME_ATTACK_236_SPECIAL_AIR);

			}
			else
			{
				m_pPlayer->Set_NextAnimation(CPlay_Goku::ANIME_ATTACK_236_SPECIAL_UPSIDE, 3.5f);
				//m_pPlayer->Set_Animation(CPlay_Goku::ANIME_ATTACK_236_SPECIAL);

			}
		}
	}

}

void CGoku_MeleeAttack::Attack_214Final()
{

	
	if (m_pPlayer->Check_bCurAnimationisGroundMove())
	{
		if(CBattleInterface_Manager::Get_Instance()->Use_KiGuage(3, m_pPlayer->Get_iPlayerTeam()))
		{
			CEffect_Layer::COPY_DESC tDesc{};
//			tDesc.pPlayertMatrix = m_pPlayer->Get_pTransformMatrix();//m_pTransformCom->Get_WorldMatrixPtr();
			/*tDesc.pPlayertMatrix = m_pPlayer->Character_Make_BoneEffect()
			tDesc.pTransformCom = static_cast<CTransform*>(m_pPlayer->Get_Component(TEXT("Com_Transform")));
			m_pEffect_Manager->Copy_Layer(TEXT("EnergieSAO-01"), &tDesc);*/


			//Character로 이사감
			//CEffect_Layer* pEffectLayer = m_pPlayer->Character_Make_BoneEffect("G_root", TEXT("EnergieSAO-01"));

			//레이어 안에 이펙트 껐다 켰다
			//pEffectLayer->Find_Effect(L"EnergieSAO-01_01")->m_bIsSpriteEnd = true;
			//pEffectLayer->Find_Effect(L"EnergieSAO-01_02")->m_bIsSpriteEnd = true;
			//pEffectLayer->Find_Effect(L"EnergieSAO-01_03")->m_bIsSpriteEnd = true;
			
			m_pPlayer->Set_Animation(CPlay_Goku::ANIME_FINAL_START);
			m_pPlayer->Set_AnimationStopWithoutMe(2.f);

			//이전에 만들어진 모든 Attack Object들 2초간 정지.  이거 본인은 이 직후에 만들어질테니 괜찮음
			for (auto pAttackObject : m_pGameInstance->Get_Layer(LEVEL_GAMEPLAY, TEXT("Layer_AttackObject")))
			{
				static_cast<CAttackObject*>(pAttackObject)->Set_UpdateStop(2.f);
			}

			m_pPlayer->Set_bFinalSkillQTE(false);
		}
	}

	//이거 빗맞음
	//else if (m_pPlayer->Get_bAttackBackEvent() && (*m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_HEAVY))
	//{
	//	if (CBattleInterface_Manager::Get_Instance()->Use_KiGuage(3, m_pPlayer->Get_iPlayerTeam()))
	//	{
	//		CEffect_Layer::COPY_DESC tDesc{};
	//		//			tDesc.pPlayertMatrix = m_pPlayer->Get_pTransformMatrix();//m_pTransformCom->Get_WorldMatrixPtr();
	//					/*tDesc.pPlayertMatrix = m_pPlayer->Character_Make_BoneEffect()
	//					tDesc.pTransformCom = static_cast<CTransform*>(m_pPlayer->Get_Component(TEXT("Com_Transform")));
	//					m_pEffect_Manager->Copy_Layer(TEXT("EnergieSAO-01"), &tDesc);*/
	//		CEffect_Layer* pEffectLayer = m_pPlayer->Character_Make_BoneEffect("G_root", TEXT("EnergieSAO-01"));
	//
	//		//레이어 안에 이펙트 껐다 켰다
	//		//pEffectLayer->Find_Effect(L"EnergieSAO-01_01")->m_bIsSpriteEnd = true;
	//		//pEffectLayer->Find_Effect(L"EnergieSAO-01_02")->m_bIsSpriteEnd = true;
	//		//pEffectLayer->Find_Effect(L"EnergieSAO-01_03")->m_bIsSpriteEnd = true;
	//
	//		m_pPlayer->Set_Animation(CPlay_Goku::ANIME_FINAL_START);
	//		m_pPlayer->Set_AnimationStopWithoutMe(2.f);
	//
	//		//이전에 만들어진 모든 Attack Object들 2초간 정지.  이거 본인은 이 직후에 만들어질테니 괜찮음
	//		for (auto pAttackObject : m_pGameInstance->Get_Layer(LEVEL_GAMEPLAY, TEXT("Layer_AttackObject")))
	//		{
	//			static_cast<CAttackObject*>(pAttackObject)->Set_UpdateStop(2.f);
	//		}
	//
	//		m_pPlayer->Set_bFinalSkillQTE(false);
	//	}
	//}

	else if (m_pPlayer->Get_bAttackBackEvent() && (*m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_236))
	{
		if (CBattleInterface_Manager::Get_Instance()->Use_KiGuage(3, m_pPlayer->Get_iPlayerTeam()))
		{
			CEffect_Layer::COPY_DESC tDesc{};
			//			tDesc.pPlayertMatrix = m_pPlayer->Get_pTransformMatrix();//m_pTransformCom->Get_WorldMatrixPtr();
						/*tDesc.pPlayertMatrix = m_pPlayer->Character_Make_BoneEffect()
						tDesc.pTransformCom = static_cast<CTransform*>(m_pPlayer->Get_Component(TEXT("Com_Transform")));
						m_pEffect_Manager->Copy_Layer(TEXT("EnergieSAO-01"), &tDesc);*/
			CEffect_Layer* pEffectLayer = m_pPlayer->Character_Make_BoneEffect("G_root", TEXT("EnergieSAO-01"));

			//레이어 안에 이펙트 껐다 켰다
			//pEffectLayer->Find_Effect(L"EnergieSAO-01_01")->m_bIsSpriteEnd = true;
			//pEffectLayer->Find_Effect(L"EnergieSAO-01_02")->m_bIsSpriteEnd = true;
			//pEffectLayer->Find_Effect(L"EnergieSAO-01_03")->m_bIsSpriteEnd = true;

			m_pPlayer->Set_Animation(CPlay_Goku::ANIME_FINAL_START);
			m_pPlayer->Set_AnimationStopWithoutMe(2.f);

			//이전에 만들어진 모든 Attack Object들 2초간 정지.  이거 본인은 이 직후에 만들어질테니 괜찮음
			for (auto pAttackObject : m_pGameInstance->Get_Layer(LEVEL_GAMEPLAY, TEXT("Layer_AttackObject")))
			{
				static_cast<CAttackObject*>(pAttackObject)->Set_UpdateStop(2.f);
			}

			m_pPlayer->Set_bFinalSkillQTE(false);
		}
	}
}



void CGoku_MeleeAttack::Attack_Crouch_Light()
{
	if (*m_pPlayerAnimationIndex == CPlay_Goku::ANIME_CROUCHING  || *m_pPlayerAnimationIndex == CPlay_Goku::ANIME_FORWARD_DASH || *m_pPlayerAnimationIndex == CPlay_Goku::ANIME_FORWARD_DASH_END)
	{
		m_pPlayer->Set_Animation(CPlay_Goku::ANIME_ATTACK_CROUCH_LIGHT);
	}
}

void CGoku_MeleeAttack::Attack_Crouch_Medium()
{

	//횟수 없으면 컷
	if (m_pbAttackCount[CPlay_Goku::COUNT_ATTACK_CROUCH_MEDUIM] == false)
		return;




	if (*m_pPlayerAnimationIndex == CPlay_Goku::ANIME_CROUCHING || *m_pPlayerAnimationIndex == CPlay_Goku::ANIME_FORWARD_DASH || *m_pPlayerAnimationIndex == CPlay_Goku::ANIME_FORWARD_DASH_END)
	{
		m_pPlayer->Set_Animation(CPlay_Goku::ANIME_ATTACK_CROUCH_MEDUIM);
		m_pbAttackCount[CPlay_Goku::COUNT_ATTACK_CROUCH_MEDUIM] = false;
	}

	else if (*m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_CROUCH_LIGHT ||
		*m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_LIGHT1 || *m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_LIGHT2 ||
		*m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_MEDIUM)
	{
		m_pPlayer->Set_NextAnimation(CPlay_Goku::ANIME_ATTACK_CROUCH_MEDUIM, 1.f);
		m_pbAttackCount[CPlay_Goku::COUNT_ATTACK_CROUCH_MEDUIM] = false;
	}



	//이전에 나올것이 약공격이다보니 nextanimation을 사용하면 타이밍을 놓침
	//if (*m_pPlayerAnimationIndex == CPlay_Goku::ANIME_CROUCHING)
	//{
	//	m_pPlayer->Set_Animation(CPlay_Goku::ANIME_ATTACK_CROUCH_MEDUIM);
	//} 
	//else if(*m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_CROUCH_LIGHT ||
	//	*m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_LIGHT1 || *m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_LIGHT2 )
	//{
	//	m_pPlayer->Set_NextAnimation(CPlay_Goku::ANIME_ATTACK_CROUCH_MEDUIM,0.4f);
	//}

	
}

void CGoku_MeleeAttack::Attack_Crouch_Heavy()
{



	//if (*m_pPlayerAnimationIndex == CPlay_Goku::ANIME_CROUCHING || *m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_MEDIUM || *m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_HEAVY ||
	//	*m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_LIGHT1 || *m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_LIGHT2 || *m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_LIGHT3 ||
	//	*m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_CROUCH_LIGHT || *m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_CROUCH_MEDUIM)
	//{
	//	m_pPlayer->Set_Animation(CPlay_Goku::ANIME_ATTACK_CROUCH_HEAVY);
	//}
	//
	//else if (*m_pPlayerAnimationIndex == CPlay_Goku::ANIME_JUMP_UP || *m_pPlayerAnimationIndex == CPlay_Goku::ANIME_JUMP_DOWN ||
	//	*m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_AIR1 || *m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_AIR2)
	//{
	//	m_pPlayer->Set_Animation(CPlay_Goku::ANIME_ATTACK_UPPER_AIR);
	//}



	if (*m_pPlayerAnimationIndex == CPlay_Goku::ANIME_CROUCHING || *m_pPlayerAnimationIndex == CPlay_Goku::ANIME_FORWARD_DASH || *m_pPlayerAnimationIndex == CPlay_Goku::ANIME_FORWARD_DASH_END)
	{
		m_pPlayer->Set_Animation(CPlay_Goku::ANIME_ATTACK_CROUCH_HEAVY);
	}
	
	else if(*m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_MEDIUM || *m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_HEAVY ||
		*m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_LIGHT1 || *m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_LIGHT2 || *m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_LIGHT3 ||
		*m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_CROUCH_LIGHT || *m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_CROUCH_MEDUIM || *m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_SPECIAL)
	{
		m_pPlayer->Set_NextAnimation(CPlay_Goku::ANIME_ATTACK_CROUCH_HEAVY, 0.5f);
	}

	else if (*m_pPlayerAnimationIndex == CPlay_Goku::ANIME_JUMP_UP || *m_pPlayerAnimationIndex == CPlay_Goku::ANIME_JUMP_DOWN)
	{
		m_pPlayer->Set_Animation(CPlay_Goku::ANIME_ATTACK_UPPER_AIR);
		m_pPlayer->Set_fImpulse(1.6f * m_pPlayer->Get_iDirection());

	} 
	else if(*m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_AIR1 || *m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_AIR2 || *m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_SPECIAL_AIR)
	{
		m_pPlayer->Set_NextAnimation(CPlay_Goku::ANIME_ATTACK_UPPER_AIR, 0.5f);
		m_pPlayer->Set_fImpulse(1.6f * m_pPlayer->Get_iDirection());

	}




}

void CGoku_MeleeAttack::BackDash()
{

	if (m_pPlayer->Check_bCurAnimationisGroundMove())
	{
		m_pPlayer->Set_Animation(CPlay_Goku::ANIME_BACK_DASH);
	}

	else if (m_pPlayer->Get_bAirDashEnable() && (*m_pPlayerAnimationIndex == CPlay_Goku::ANIME_JUMP_DOWN || *m_pPlayerAnimationIndex == CPlay_Goku::ANIME_JUMP_UP))
	{
		m_pPlayer->Set_Animation(CPlay_Goku::ANIME_BACK_DASH);
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

void CGoku_MeleeAttack::ForwardDash()
{
	//if (m_pPlayer->Check_bCurAnimationisGroundMove())
	//지상대쉬
	if(*m_pPlayerAnimationIndex == CPlay_Goku::ANIME_IDLE || *m_pPlayerAnimationIndex == CPlay_Goku::ANIME_FORWARD_WALK || *m_pPlayerAnimationIndex == CPlay_Goku::ANIME_BACK_WALK)
	{
		m_pPlayer->Set_Animation(CPlay_Goku::ANIME_FORWARD_DASH);
		m_pGameInstance->Play_Sound(CSound_Manager::SOUND_KEY_NAME::Common_Dash_Start_SFX, false, 1.f);


		CEffect_Layer::COPY_DESC tDesc{};
		tDesc.pPlayertMatrix = static_cast<CTransform*>(m_pPlayer->Get_Component(TEXT("Com_Transform")))->Get_WorldMatrixPtr();
		m_pEffect_Manager->Copy_Layer(TEXT("Smoke_Run"), &tDesc);

		m_pGameInstance->Play_Sound(CSound_Manager::SOUND_KEY_NAME::Common_Dash_Start_SFX, false, 1.f);
	}

	else if (m_pPlayer->Get_bAirDashEnable()  && (*m_pPlayerAnimationIndex == CPlay_Goku::ANIME_JUMP_DOWN || *m_pPlayerAnimationIndex == CPlay_Goku::ANIME_JUMP_UP ))
	{
		m_pPlayer->Set_Animation(CPlay_Goku::ANIME_FORWARD_DASH);
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
		(*m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_SPECIAL_AIR || *m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_AIR1 || *m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_AIR2))
	{
		m_pPlayer->Set_Animation(CPlay_Goku::ANIME_FORWARD_DASH);
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

	else if (m_pPlayer->Get_bSparking() && m_pPlayer->Get_bAirDashEnable() && m_pPlayer->Get_bAttackBackEvent() && *m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_214)
	{
		m_pPlayer->Set_Animation(CPlay_Goku::ANIME_FORWARD_DASH);
		m_pPlayer->Set_fImpulse(m_pPlayer->Get_iDirection() * (10.f));
		m_pPlayer->Set_bAirDashEnable(false);
		//m_pPlayer->Set_ForcedGravityDown();

		m_pPlayer->Set_ForcedGravityTime_LittleUp();

		//m_pPlayer->Character_Make_Effect(TEXT("Moving_Line_Right"));
		//m_pPlayer->Character_Make_Effect(TEXT("Ring_Dust_Right"), { 0.9f,0.f });

		CEffect_Layer::COPY_DESC tDesc{};
		tDesc.pPlayertMatrix = m_pPlayer->Get_pTransformMatrix();

		m_pEffect_Manager->Copy_Layer(TEXT("Dash"), &tDesc);
	}


}

void CGoku_MeleeAttack::Reflect()
{
	if (m_pPlayer->Check_bCurAnimationisGroundMove())
	{
		m_pPlayer->Set_Animation(CPlay_Goku::ANIME_REFLECT);

	}

	else if (m_pPlayer->Get_bAttackBackEvent() && m_pPlayer->Check_bCurAnimationisReflect())
	{
		m_pPlayer->Set_Animation(CPlay_Goku::ANIME_REFLECT);
		m_pPlayer->Set_CurrentAnimationPositionJump(0.f);

	}

}

void CGoku_MeleeAttack::StopMacro(_float fStopTime)
{
	m_pPlayer->Set_AnimationStopWithoutMe(fStopTime);

	//이전에 만들어진 모든 Attack Object들 2초간 정지.  이거 본인은 이 직후에 만들어질테니 괜찮음
	for (auto pAttackObject : m_pGameInstance->Get_Layer(LEVEL_GAMEPLAY, TEXT("Layer_AttackObject")))
	{
		static_cast<CAttackObject*>(pAttackObject)->Set_UpdateStop(fStopTime);
	}
}

void CGoku_MeleeAttack::Attack_Benishing()
{
	
	if(CBattleInterface_Manager::Get_Instance()->Get_KiNumber(m_pPlayer->Get_iPlayerTeam()) == 0 || m_pbAttackCount[CPlay_Goku::COUNT_ATTACK_BENISHING] == false)
		return;


	if (m_pPlayer->Get_bAttackBackEvent() && 
		(*m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_AIR1 || *m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_AIR2 || *m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_AIR3 ||
			*m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_236 || *m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_236_AIR || *m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_214 ||
			*m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_LIGHT3 ||
			*m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_HEAVY || *m_pPlayerAnimationIndex == CPlay_Goku::ANIME_ATTACK_CROUCH_HEAVY) )
	{

		if(CBattleInterface_Manager::Get_Instance()->Use_KiGuage(1,m_pPlayer->Get_iPlayerTeam()))
		{		
			m_pbAttackCount[CPlay_Goku::COUNT_ATTACK_BENISHING] = false;
			//m_pPlayer->Set_NextAnimation(CPlay_Goku::ANIME_ATTACK_AIR2,2.f,0.f);
			m_pPlayer->Set_Animation(CPlay_Goku::ANIME_ATTACK_AIR2);
			m_pPlayer->Set_CurrentAnimationPositionJump(30.99);
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
			m_pbAttackCount[CPlay_Goku::COUNT_ATTACK_BENISHING] = false;
			//m_pPlayer->Set_NextAnimation(CPlay_Goku::ANIME_ATTACK_AIR2,2.f,0.f);
			m_pPlayer->Set_Animation(CPlay_Goku::ANIME_ATTACK_AIR2);
			m_pPlayer->Set_CurrentAnimationPositionJump(30.99);
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

