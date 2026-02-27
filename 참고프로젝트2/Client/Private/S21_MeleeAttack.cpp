#include "stdafx.h"
#include "..\Public\S21_MeleeAttack.h"

#include "RenderInstance.h"
#include "GameInstance.h"
#include "Effect_Manager.h"
#include "Input.h"

#include "Play_21.h"

#include "BattleInterface.h"


#include "Effect.h"

void CS21_MeleeAttack::Initalize(CPlay_21* pPlayer)
{
	m_pPlayer = pPlayer;
	m_pPlayerAnimationIndex = m_pPlayer->Get_pAnimationIndex();


	m_pbAttackCount = m_pPlayer->Get_pbAttackCount();
	//m_piCountGroundSpecial = m_pPlayer->Get_piSpecialCount();

	m_pGameInstance = CGameInstance::Get_Instance();
	m_pEffect_Manager = CEffect_Manager::Get_Instance();

}

void CS21_MeleeAttack::Attack_Light()
{

	this;
	_bool bDebug = false;


	if (m_pPlayer->Check_bCurAnimationisGroundMove())
	{
		m_pPlayer->Set_Animation(CPlay_21::ANIME_ATTACK_LIGHT1);

	}

	else if (*m_pPlayerAnimationIndex == CPlay_21::ANIME_ATTACK_LIGHT1)
	{
		m_pPlayer->Set_NextAnimation(CPlay_21::ANIME_ATTACK_LIGHT2, 1.f);
	}

	else if (*m_pPlayerAnimationIndex == CPlay_21::ANIME_ATTACK_LIGHT2)
	{
		m_pPlayer->Set_NextAnimation(CPlay_21::ANIME_ATTACK_LIGHT3, 1.f);
	}

	else if (*m_pPlayerAnimationIndex == CPlay_21::ANIME_JUMP_UP || *m_pPlayerAnimationIndex == CPlay_21::ANIME_JUMP_DOWN)
	{
		m_pPlayer->Set_Animation(CPlay_21::ANIME_ATTACK_AIR1);
	}
	else if (*m_pPlayerAnimationIndex == CPlay_21::ANIME_ATTACK_AIR1)
	{

		m_pPlayer->Set_NextAnimation(CPlay_21::ANIME_ATTACK_AIR2, 0.5f);

		if (m_pPlayer->Get_fGravityTime() > 0.135)
		{
			m_pPlayer->Set_bNextAnimationGravityEvent();
		}
	}
	else if (*m_pPlayerAnimationIndex == CPlay_21::ANIME_ATTACK_AIR2)
	{
		m_pPlayer->Set_NextAnimation(CPlay_21::ANIME_ATTACK_AIR3, 0.5f);
		if (m_pPlayer->Get_fGravityTime() > 0.135)
		{
			//m_pPlayer->Set_ForcedGravityTime_LittleUp();
			m_pPlayer->Set_bNextAnimationGravityEvent();
		}
	}


	else if (*m_pPlayerAnimationIndex == CPlay_21::ANIME_ATTACK_CROUCH_LIGHT)
	{
		m_pPlayer->Set_Animation(CPlay_21::ANIME_ATTACK_LIGHT1);
	}
	else if (*m_pPlayerAnimationIndex == CPlay_21::ANIME_FORWARD_DASH || *m_pPlayerAnimationIndex == CPlay_21::ANIME_FORWARD_DASH_END)
	{
		m_pPlayer->Set_Animation(CPlay_21::ANIME_ATTACK_LIGHT1);
	}
}

void CS21_MeleeAttack::Attack_Medium()
{


	if (m_pPlayer->Check_bCurAnimationisGroundMove(*m_pPlayerAnimationIndex))
	{
		m_pPlayer->Set_Animation(CPlay_21::ANIME_ATTACK_MEDIUM);
		m_pbAttackCount[CPlay_21::COUNT_ATTACK_MEDIUM] = false;

		m_pbAttackCount[CPlay_21::COUNT_ATTACK_MEDIUM];
		m_pbAttackCount[CPlay_21::COUNT_ATTACK_CROUCH_MEDUIM];
		m_pbAttackCount;

		_bool bDebug = true;
	}

	else if (*m_pPlayerAnimationIndex == CPlay_21::ANIME_ATTACK_LIGHT1 || *m_pPlayerAnimationIndex == CPlay_21::ANIME_ATTACK_LIGHT2
		|| *m_pPlayerAnimationIndex == CPlay_21::ANIME_ATTACK_CROUCH_LIGHT || *m_pPlayerAnimationIndex == CPlay_21::ANIME_ATTACK_CROUCH_MEDUIM)
	{

		//서서 중공격 횟수가 남아있으면 일단 사용
		if (m_pbAttackCount[CPlay_21::COUNT_ATTACK_MEDIUM] == true)
		{
			m_pPlayer->Set_NextAnimation(CPlay_21::ANIME_ATTACK_MEDIUM, 0.5f);
			m_pbAttackCount[CPlay_21::COUNT_ATTACK_MEDIUM] = false;
		}

	}

	else if (*m_pPlayerAnimationIndex == CPlay_21::ANIME_ATTACK_MEDIUM)
	{
		//서서 중공격 중에 또 누를 시 횟수가 있으면  앉아 중공격 자동 사용
		if (m_pbAttackCount[CPlay_21::COUNT_ATTACK_CROUCH_MEDUIM] == true)
		{
			m_pPlayer->Set_NextAnimation(CPlay_21::ANIME_ATTACK_CROUCH_MEDUIM, 0.5f);
			m_pbAttackCount[CPlay_21::COUNT_ATTACK_CROUCH_MEDUIM] = false;
		}
	}


	else if (*m_pPlayerAnimationIndex == CPlay_21::ANIME_ATTACK_AIR1)
	{
		m_pPlayer->Set_NextAnimation(CPlay_21::ANIME_ATTACK_AIR2, 0.5f);
	}

	else if (*m_pPlayerAnimationIndex == CPlay_21::ANIME_JUMP_UP || *m_pPlayerAnimationIndex == CPlay_21::ANIME_JUMP_DOWN)
	{
		m_pPlayer->Set_Animation(CPlay_21::ANIME_ATTACK_AIR2);
	}


	/*


	if (m_pPlayer->Check_bCurAnimationisGroundMove(*m_pPlayerAnimationIndex))
	{
		m_pPlayer->Set_Animation(CPlay_21::ANIME_ATTACK_MEDIUM);
		m_pbAttackCount[CPlay_21::COUNT_ATTACK_MEDIUM] = false;

		m_pbAttackCount[CPlay_21::COUNT_ATTACK_MEDIUM];
		m_pbAttackCount[CPlay_21::COUNT_ATTACK_CROUCH_MEDUIM];
		m_pbAttackCount;

		_bool bDebug = true;
	}
	
	else if(*m_pPlayerAnimationIndex == CPlay_21::ANIME_ATTACK_LIGHT1 || *m_pPlayerAnimationIndex == CPlay_21::ANIME_ATTACK_LIGHT2
		|| *m_pPlayerAnimationIndex == CPlay_21::ANIME_ATTACK_CROUCH_LIGHT || *m_pPlayerAnimationIndex == CPlay_21::ANIME_ATTACK_CROUCH_MEDUIM)
	{

		//서서 중공격 횟수가 남아있으면 일단 사용
		if (m_pbAttackCount[CPlay_21::COUNT_ATTACK_MEDIUM] == true)
		{
			m_pPlayer->Set_NextAnimation(CPlay_21::ANIME_ATTACK_MEDIUM, 0.5f);
			m_pbAttackCount[CPlay_21::COUNT_ATTACK_MEDIUM] = false;

			m_pbAttackCount[CPlay_21::COUNT_ATTACK_MEDIUM];
			m_pbAttackCount[CPlay_21::COUNT_ATTACK_CROUCH_MEDUIM];
			m_pbAttackCount;

			_bool bDebug = true;


		}

	}

	else if (*m_pPlayerAnimationIndex == CPlay_21::ANIME_ATTACK_MEDIUM)
	{
		//서서 중공격 중에 또 누를 시 횟수가 있으면  앉아 중공격 자동 사용
		if (m_pbAttackCount[CPlay_21::COUNT_ATTACK_CROUCH_MEDUIM] == true)
		{
			m_pPlayer->Set_NextAnimation(CPlay_21::ANIME_ATTACK_CROUCH_MEDUIM,0.5f);
			m_pbAttackCount[CPlay_21::COUNT_ATTACK_CROUCH_MEDUIM] = false;
		}
	}

	
	else if (*m_pPlayerAnimationIndex == CPlay_21::ANIME_ATTACK_AIR1)
	{
		m_pPlayer->Set_NextAnimation(CPlay_21::ANIME_ATTACK_AIR2, 0.5f);
	}
	
	else if (*m_pPlayerAnimationIndex == CPlay_21::ANIME_JUMP_UP || *m_pPlayerAnimationIndex == CPlay_21::ANIME_JUMP_DOWN)
	{
		m_pPlayer->Set_Animation(CPlay_21::ANIME_ATTACK_AIR2);
	}

	*/
	
	
	
}

void CS21_MeleeAttack::Attack_Heavy()
{
	//if (m_pPlayer->Check_bCurAnimationisGroundMove() || *m_pPlayerAnimationIndex == CPlay_21::ANIME_ATTACK_MEDIUM ||
	//	*m_pPlayerAnimationIndex == CPlay_21::ANIME_ATTACK_LIGHT1 || *m_pPlayerAnimationIndex == CPlay_21::ANIME_ATTACK_LIGHT2 ||
	//	*m_pPlayerAnimationIndex == CPlay_21::ANIME_ATTACK_CROUCH_LIGHT || *m_pPlayerAnimationIndex == CPlay_21::ANIME_ATTACK_CROUCH_MEDUIM ||
	//	*m_pPlayerAnimationIndex == CPlay_21::ANIME_ATTACK_SPECIAL)
	//{
	//	m_pPlayer->Set_Animation(CPlay_21::ANIME_ATTACK_HEAVY);
	//	//m_pPlayer->Set_NextAnimation(CPlay_21::ANIME_ATTACK_HEAVY, 0.5f);
	//}
	//
	//else if (*m_pPlayerAnimationIndex == CPlay_21::ANIME_ATTACK_AIR2)
	//{
	//	m_pPlayer->Set_NextAnimation(CPlay_21::ANIME_ATTACK_AIR3, 0.5f);
	//}
	

	if (m_pPlayer->Check_bCurAnimationisGroundMove() || *m_pPlayerAnimationIndex == CPlay_21::ANIME_FORWARD_DASH || *m_pPlayerAnimationIndex == CPlay_21::ANIME_FORWARD_DASH_END)
	{
		m_pPlayer->Set_Animation(CPlay_21::ANIME_ATTACK_HEAVY);
		//m_pPlayer->Set_NextAnimation(CPlay_21::ANIME_ATTACK_HEAVY, 0.5f);
	}
	else if (*m_pPlayerAnimationIndex == CPlay_21::ANIME_ATTACK_MEDIUM ||
		*m_pPlayerAnimationIndex == CPlay_21::ANIME_ATTACK_LIGHT1 || *m_pPlayerAnimationIndex == CPlay_21::ANIME_ATTACK_LIGHT2 ||
		*m_pPlayerAnimationIndex == CPlay_21::ANIME_ATTACK_CROUCH_LIGHT || *m_pPlayerAnimationIndex == CPlay_21::ANIME_ATTACK_CROUCH_MEDUIM ||
		*m_pPlayerAnimationIndex == CPlay_21::ANIME_ATTACK_SPECIAL)
	{
		m_pPlayer->Set_NextAnimation(CPlay_21::ANIME_ATTACK_HEAVY, 0.5f);
	}
	else if (*m_pPlayerAnimationIndex == CPlay_21::ANIME_ATTACK_AIR2)
	{
		m_pPlayer->Set_NextAnimation(CPlay_21::ANIME_ATTACK_AIR3, 0.5f);
	}
	else if (*m_pPlayerAnimationIndex == CPlay_21::ANIME_JUMP_UP  || *m_pPlayerAnimationIndex == CPlay_21::ANIME_JUMP_DOWN)
	{
		m_pPlayer->Set_Animation(CPlay_21::ANIME_ATTACK_AIR3);
	}
}

void CS21_MeleeAttack::Attack_Special()
{
	if (m_pPlayer->Check_bCurAnimationisGroundMove(*m_pPlayerAnimationIndex))
	{
		m_pPlayer->Set_Animation(CPlay_21::ANIME_ATTACK_SPECIAL);
		//(*m_piCountGroundSpecial)++;
	} 
	else if(*m_pPlayerAnimationIndex == CPlay_21::ANIME_ATTACK_LIGHT1 || *m_pPlayerAnimationIndex == CPlay_21::ANIME_ATTACK_LIGHT2 || *m_pPlayerAnimationIndex == CPlay_21::ANIME_ATTACK_MEDIUM)
	{
		//m_pPlayer->Set_NextAnimation(CPlay_21::ANIME_ATTACK_SPECIAL, 0.5f);
		m_pPlayer->Set_NextAnimation(CPlay_21::ANIME_ATTACK_SPECIAL, 0.5f);
		//(*m_piCountGroundSpecial)++;

	}


	else if (*m_pPlayerAnimationIndex == CPlay_21::ANIME_JUMP_UP || *m_pPlayerAnimationIndex == CPlay_21::ANIME_JUMP_DOWN ||
		*m_pPlayerAnimationIndex == CPlay_21::ANIME_ATTACK_CROUCH_LIGHT || *m_pPlayerAnimationIndex == CPlay_21::ANIME_ATTACK_CROUCH_MEDUIM)
	{
		m_pPlayer->Set_Animation(CPlay_21::ANIME_ATTACK_CROUCH_SPECIAL);
		//16부터
		CModel* pModel = static_cast<CModel*>(m_pPlayer->Get_Component(TEXT("Com_Model")));
		pModel->m_fCurrentAnimPosition = 16.f;
		pModel->Update_FrameIndex();
		//m_pPlayer->Set_fImpulse({ m_pPlayer->Get_iDirection() * 1.f,0.f });

		if (m_pPlayer->Get_fHeight() < 1)
		{
			m_pPlayer->Set_fImpulse({ m_pPlayer->Get_iDirection() * 1.f,0.5f });
		}
		else
		{
			m_pPlayer->Set_fImpulse({ m_pPlayer->Get_iDirection() * 1.f,0.f });
		}
		m_pPlayer->Set_fGravityTime(0.07f);


	}
	else if (m_pPlayer->Get_bAttackBackEvent() && (*m_pPlayerAnimationIndex == CPlay_21::ANIME_ATTACK_AIR1 || *m_pPlayerAnimationIndex == CPlay_21::ANIME_ATTACK_AIR2))
	{
		//m_pPlayer->Set_NextAnimation(CPlay_21::ANIME_ATTACK_CROUCH_SPECIAL, 0.5f, 16.f);
		m_pPlayer->Set_Animation(CPlay_21::ANIME_ATTACK_CROUCH_SPECIAL);
		m_pPlayer->Set_CurrentAnimationPositionJump(16.f);

		if (m_pPlayer->Get_fHeight() < 1)
		{
			m_pPlayer->Set_fImpulse({ m_pPlayer->Get_iDirection() * 1.f,0.5f });
		}
		else
		{
			m_pPlayer->Set_fImpulse({ m_pPlayer->Get_iDirection() * 1.f,0.f });
		}

		m_pPlayer->Set_fGravityTime(0.07f);
	}
	//else if( m_pPlayer->Get_bAttackBackEvent() && (*m_pPlayerAnimationIndex == CPlay_21::ANIME_ATTACK_AIR1 || *m_pPlayerAnimationIndex == CPlay_21::ANIME_ATTACK_AIR2))
	//{
	//	m_pPlayer->Set_NextAnimation(CPlay_21::ANIME_ATTACK_CROUCH_SPECIAL,0.5f,16.f);
	//
	//	if (m_pPlayer->Get_fHeight() < 1)
	//	{
	//		m_pPlayer->Set_fImpulse({ m_pPlayer->Get_iDirection() * 1.f,0.5f });
	//	}
	//	else
	//	{
	//		m_pPlayer->Set_fImpulse({ m_pPlayer->Get_iDirection() * 1.f,0.f });
	//	}
	//
	//	m_pPlayer->Set_fGravityTime(0.07f);
	//}
	
	else if (*m_pPlayerAnimationIndex == CPlay_21::ANIME_CROUCHING)
	{

	}

}

void CS21_MeleeAttack::Attack_Grab()
{
	if (m_pbAttackCount[CPlay_21::COUNT_ATTACK_GRAB])
	{

		if (m_pPlayer->Check_bCurAnimationisGroundMove())
		{
			m_pPlayer->Set_Grab(false);
			m_pPlayer->Set_Animation(CPlay_21::ANIME_GRAB_READY);
			m_pPlayer->Set_NextAnimation(CPlay_21::ANIME_GRAB, 3.f, 5.f);
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

		else if (*m_pPlayerAnimationIndex == CPlay_21::ANIME_JUMP_DOWN || *m_pPlayerAnimationIndex == CPlay_21::ANIME_JUMP_UP)
		{
			m_pPlayer->Set_Grab(true);
			m_pPlayer->Set_Animation(CPlay_21::ANIME_GRAB_READY);
			m_pPlayer->Set_NextAnimation(CPlay_21::ANIME_GRAB, 3.f, 5.f);
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

void CS21_MeleeAttack::Attack_236()
{

	//공중인지 아닌지 상태 받고,  사용중인게 필살기가 아닌지 구분.  피격중은 키 lock을 해버릴것

	if (m_pPlayer->Check_bCurAnimationisGroundMove() || *m_pPlayerAnimationIndex == CPlay_21::ANIME_FORWARD_DASH || *m_pPlayerAnimationIndex == CPlay_21::ANIME_FORWARD_DASH_END)
	{
		m_pPlayer->Set_Animation(CPlay_21::ANIME_ATTACK_236);
	} 
	
	else if(*m_pPlayerAnimationIndex == CPlay_21::ANIME_ATTACK_MEDIUM ||  *m_pPlayerAnimationIndex == CPlay_21::ANIME_ATTACK_HEAVY ||
		*m_pPlayerAnimationIndex == CPlay_21::ANIME_ATTACK_LIGHT1 || *m_pPlayerAnimationIndex == CPlay_21::ANIME_ATTACK_LIGHT2  || *m_pPlayerAnimationIndex == CPlay_21::ANIME_ATTACK_LIGHT3 ||
		*m_pPlayerAnimationIndex == CPlay_21::ANIME_ATTACK_SPECIAL)
	{
		m_pPlayer->Set_NextAnimation(CPlay_21::ANIME_ATTACK_236,0.5f);
	}
	
	


}

void CS21_MeleeAttack::Attack_214()
{
	


	if (m_pPlayer->Check_bCurAnimationisGroundMove() || *m_pPlayerAnimationIndex == CPlay_21::ANIME_FORWARD_DASH || *m_pPlayerAnimationIndex == CPlay_21::ANIME_FORWARD_DASH_END)
	{
		CTransform* pTrasnform = static_cast<CTransform*>(m_pPlayer->Get_Component(TEXT("Com_Transform")));
		pTrasnform->Add_Move({ 0.f,1.f,0.f });
		m_pPlayer->Set_Animation(CPlay_21::ANIME_ATTACK_214);

		m_pPlayer->Set_fImpulse({ m_pPlayer->Get_iDirection() * 5.f, -15.f });
	}
	else if ( m_pPlayer->Get_bAttackBackEvent() &&
		(*m_pPlayerAnimationIndex == CPlay_21::ANIME_ATTACK_MEDIUM || *m_pPlayerAnimationIndex == CPlay_21::ANIME_ATTACK_CROUCH_MEDUIM ||
		*m_pPlayerAnimationIndex == CPlay_21::ANIME_ATTACK_LIGHT1 || *m_pPlayerAnimationIndex == CPlay_21::ANIME_ATTACK_LIGHT2 || *m_pPlayerAnimationIndex == CPlay_21::ANIME_ATTACK_LIGHT3 ||
		*m_pPlayerAnimationIndex == CPlay_21::ANIME_ATTACK_HEAVY || *m_pPlayerAnimationIndex == CPlay_21::ANIME_ATTACK_SPECIAL))
	{
		CTransform* pTrasnform = static_cast<CTransform*>(m_pPlayer->Get_Component(TEXT("Com_Transform")));
		//pTrasnform->Add_Move({ 0.f,1.f,0.f });
		pTrasnform->Add_Move({m_pPlayer->Get_iDirection() * 0.2f,0.8f,0.f });

		//m_pPlayer->Set_NextAnimation(CPlay_21::ANIME_ATTACK_214, 0.5f);
		m_pPlayer->Set_Animation(CPlay_21::ANIME_ATTACK_214);
		m_pPlayer->Set_fImpulse({ m_pPlayer->Get_iDirection() * 5.f, -15.f });

	}


	else if (*m_pPlayerAnimationIndex == CPlay_21::ANIME_JUMP_UP || *m_pPlayerAnimationIndex == CPlay_21::ANIME_JUMP_DOWN)
	{
		m_pPlayer->Set_Animation(CPlay_21::ANIME_ATTACK_214);
		m_pPlayer->Set_fImpulse({ m_pPlayer->Get_iDirection() * 5.f, -15.f });

	}


	//AttackBack을 사용한 버전. 자연스럽지만 커맨드 입력이 빡세짐.      문제있나?

	else if (m_pPlayer->Get_bAttackBackEvent() && (*m_pPlayerAnimationIndex == CPlay_21::ANIME_ATTACK_AIR1 || *m_pPlayerAnimationIndex == CPlay_21::ANIME_ATTACK_AIR2 || *m_pPlayerAnimationIndex == CPlay_21::ANIME_ATTACK_CROUCH_SPECIAL))
	{
		m_pPlayer->Set_Animation(CPlay_21::ANIME_ATTACK_214);
	
	
		CTransform* pTrasnform = static_cast<CTransform*>(m_pPlayer->Get_Component(TEXT("Com_Transform")));
		//pTrasnform->Add_Move({ 0.f,0.7f,0.f });
		pTrasnform->Add_Move({ m_pPlayer->Get_iDirection()*0.5f,0.7f,0.f});
		m_pPlayer->Set_fImpulse({ m_pPlayer->Get_iDirection() * 9.f, -15.f });
		//테스트

		//너무 안보임
		//m_pPlayer->Set_CurrentAnimationPositionJump(27.f);
	
		m_pPlayer->Set_CurrentAnimationPositionJump(24.f);


		m_pPlayer->Set_ForcedGravityDown();
	}


	//AttackBack을 이용하지 않은 NextAnimation 버전.  기탄중에 Impus 때문에 문제가 생길 수 있음

	//else if (*m_pPlayerAnimationIndex == CPlay_21::ANIME_ATTACK_AIR1 || *m_pPlayerAnimationIndex == CPlay_21::ANIME_ATTACK_AIR2 || *m_pPlayerAnimationIndex == CPlay_21::ANIME_ATTACK_CROUCH_SPECIAL)
	//{
	//	//m_pPlayer->Set_NextAnimation(CPlay_21::ANIME_ATTACK_214, 0.5f, 20.f);
	//	m_pPlayer->Set_NextAnimation(CPlay_21::ANIME_ATTACK_214, 0.5f, 26.f);
	//
	//	m_pPlayer->Set_fImpulse({ m_pPlayer->Get_iDirection() *9.f, -15.f });
	//
	//	CTransform* pTrasnform = static_cast<CTransform*>(m_pPlayer->Get_Component(TEXT("Com_Transform")));
	//	pTrasnform->Add_Move({ 0.f,0.7f,0.f });
	//	//pTrasnform->Add_Move({ m_pPlayer->Get_iDirection()*0.5f,0.7f,0.f});
	//
	//	//테스트
	//	//m_pPlayer->Set_CurrentAnimationPositionJump(27.f);
	//
	//	m_pPlayer->Set_ForcedGravityDown();
	//}



}

void CS21_MeleeAttack::Attack_236Special()
{



	if (m_pPlayer->Check_bCurAnimationisGroundMove() || *m_pPlayerAnimationIndex == CPlay_21::ANIME_FORWARD_DASH || *m_pPlayerAnimationIndex == CPlay_21::ANIME_FORWARD_DASH_END)
	{
		m_pPlayer->Set_Animation(CPlay_21::ANIME_ATTACK_236_SPECIAL);

		//디버그코드 체크
		//m_pPlayer->Set_NextAnimation(CPlay_21::ANIME_ATTACK_236_SPECIAL_SUCCES, 10.5f);

	}

	else if (*m_pPlayerAnimationIndex == CPlay_21::ANIME_ATTACK_MEDIUM || *m_pPlayerAnimationIndex == CPlay_21::ANIME_ATTACK_HEAVY ||
		*m_pPlayerAnimationIndex == CPlay_21::ANIME_ATTACK_LIGHT1 || *m_pPlayerAnimationIndex == CPlay_21::ANIME_ATTACK_LIGHT2 || *m_pPlayerAnimationIndex == CPlay_21::ANIME_ATTACK_LIGHT3 ||
		*m_pPlayerAnimationIndex == CPlay_21::ANIME_ATTACK_SPECIAL)
	{
		m_pPlayer->Set_NextAnimation(CPlay_21::ANIME_ATTACK_236_SPECIAL, 3.5f);
	}

} 


void CS21_MeleeAttack::Attack_236Ultimate()
{

	//필살기는 모션을 끊고 쓰는것처럼 보이는데 자세히 보면 기술마다 다름
	// 에너지파는 끊고 쓰고,   공격은 타격판정 난 이후? 

	if (m_pPlayer->Check_bCurAnimationisGroundMove() || 
		*m_pPlayerAnimationIndex == CPlay_21::ANIME_FORWARD_DASH || *m_pPlayerAnimationIndex == CPlay_21::ANIME_FORWARD_DASH_END || m_pPlayer->Check_bCurAnimationisAirMove())
	{
		if (CBattleInterface_Manager::Get_Instance()->Use_KiGuage(1, m_pPlayer->Get_iPlayerTeam()))
			m_pPlayer->Set_Animation(CPlay_21::ANIME_ATTACK_236_ULTIMATE);
	}

	//else if (*m_pPlayerAnimationIndex == CPlay_21::ANIME_ATTACK_MEDIUM || *m_pPlayerAnimationIndex == CPlay_21::ANIME_ATTACK_HEAVY ||
	//	*m_pPlayerAnimationIndex == CPlay_21::ANIME_ATTACK_LIGHT1 || *m_pPlayerAnimationIndex == CPlay_21::ANIME_ATTACK_LIGHT2 || *m_pPlayerAnimationIndex == CPlay_21::ANIME_ATTACK_LIGHT3 ||
	//	*m_pPlayerAnimationIndex == CPlay_21::ANIME_ATTACK_SPECIAL || *m_pPlayerAnimationIndex == CPlay_21::ANIME_ATTACK_236  ||
	//	*m_pPlayerAnimationIndex == CPlay_21::ANIME_ATTACK_236_ULTIMATE)

//else if (*m_pPlayerAnimationIndex == CPlay_21::ANIME_ATTACK_MEDIUM || *m_pPlayerAnimationIndex == CPlay_21::ANIME_ATTACK_HEAVY ||
//	*m_pPlayerAnimationIndex == CPlay_21::ANIME_ATTACK_LIGHT1 || *m_pPlayerAnimationIndex == CPlay_21::ANIME_ATTACK_LIGHT2 || *m_pPlayerAnimationIndex == CPlay_21::ANIME_ATTACK_LIGHT3 ||
//	*m_pPlayerAnimationIndex == CPlay_21::ANIME_ATTACK_SPECIAL || *m_pPlayerAnimationIndex == CPlay_21::ANIME_ATTACK_236 )
//{
//	m_pPlayer->Set_NextAnimation(CPlay_21::ANIME_ATTACK_236_ULTIMATE, 3.5f,0.f);
//	//m_pPlayer->Set_Animation(CPlay_21::ANIME_ATTACK_236_SPECIAL);
//}




	else if (m_pPlayer->Get_bAttackBackEvent() && (*m_pPlayerAnimationIndex == CPlay_21::ANIME_ATTACK_MEDIUM || *m_pPlayerAnimationIndex == CPlay_21::ANIME_ATTACK_HEAVY ||
		*m_pPlayerAnimationIndex == CPlay_21::ANIME_ATTACK_LIGHT1 || *m_pPlayerAnimationIndex == CPlay_21::ANIME_ATTACK_LIGHT2 || *m_pPlayerAnimationIndex == CPlay_21::ANIME_ATTACK_LIGHT3 ||
		*m_pPlayerAnimationIndex == CPlay_21::ANIME_ATTACK_SPECIAL || *m_pPlayerAnimationIndex == CPlay_21::ANIME_ATTACK_236 || *m_pPlayerAnimationIndex == CPlay_21::ANIME_ATTACK_214))
	{
		//m_pPlayer->Set_NextAnimation(CPlay_21::ANIME_ATTACK_236_ULTIMATE, 3.5f, 0.f);
		if (CBattleInterface_Manager::Get_Instance()->Use_KiGuage(1, m_pPlayer->Get_iPlayerTeam()))
			m_pPlayer->Set_Animation(CPlay_21::ANIME_ATTACK_236_ULTIMATE);
	}

	
	
	//else if (m_pPlayer->Get_bAttackBackEvent())
	//{
	//	m_pPlayer->Set_Animation(CPlay_21::ANIME_ATTACK_236_ULTIMATE, 3.5f);
	//}
	


}
void CS21_MeleeAttack::Attack_214Final()
{
	if (m_pPlayer->Check_bCurAnimationisGroundMove() || m_pPlayer->Check_bCurAnimationisAirMove())
	{
		if (CBattleInterface_Manager::Get_Instance()->Use_KiGuage(3, m_pPlayer->Get_iPlayerTeam()))
		{
			m_pPlayer->Set_Animation(CPlay_21::ANIME_FINAL_START);
		}
	}
}



void CS21_MeleeAttack::Attack_Crouch_Light()
{
	if (*m_pPlayerAnimationIndex == CPlay_21::ANIME_CROUCHING  || *m_pPlayerAnimationIndex == CPlay_21::ANIME_FORWARD_DASH || *m_pPlayerAnimationIndex == CPlay_21::ANIME_FORWARD_DASH_END)
	{
		m_pPlayer->Set_Animation(CPlay_21::ANIME_ATTACK_CROUCH_LIGHT);
	}
}

void CS21_MeleeAttack::Attack_Crouch_Medium()
{

	//횟수 없으면 컷
	if (m_pbAttackCount[CPlay_21::COUNT_ATTACK_CROUCH_MEDUIM] == false)
		return;




	if (*m_pPlayerAnimationIndex == CPlay_21::ANIME_CROUCHING || *m_pPlayerAnimationIndex == CPlay_21::ANIME_FORWARD_DASH || *m_pPlayerAnimationIndex == CPlay_21::ANIME_FORWARD_DASH_END)
	{
		m_pPlayer->Set_Animation(CPlay_21::ANIME_ATTACK_CROUCH_MEDUIM);
		m_pbAttackCount[CPlay_21::COUNT_ATTACK_CROUCH_MEDUIM] = false;
	}

	else if (*m_pPlayerAnimationIndex == CPlay_21::ANIME_ATTACK_CROUCH_LIGHT ||
		*m_pPlayerAnimationIndex == CPlay_21::ANIME_ATTACK_LIGHT1 || *m_pPlayerAnimationIndex == CPlay_21::ANIME_ATTACK_LIGHT2 ||
		*m_pPlayerAnimationIndex == CPlay_21::ANIME_ATTACK_MEDIUM)
	{
		m_pPlayer->Set_NextAnimation(CPlay_21::ANIME_ATTACK_CROUCH_MEDUIM, 1.f);
		m_pbAttackCount[CPlay_21::COUNT_ATTACK_CROUCH_MEDUIM] = false;
	}



	
}

void CS21_MeleeAttack::Attack_Crouch_Heavy()
{



	


	if (*m_pPlayerAnimationIndex == CPlay_21::ANIME_CROUCHING || *m_pPlayerAnimationIndex == CPlay_21::ANIME_FORWARD_DASH || *m_pPlayerAnimationIndex == CPlay_21::ANIME_FORWARD_DASH_END)
	{
		m_pPlayer->Set_Animation(CPlay_21::ANIME_ATTACK_CROUCH_HEAVY);
	}
	
	else if(*m_pPlayerAnimationIndex == CPlay_21::ANIME_ATTACK_MEDIUM || *m_pPlayerAnimationIndex == CPlay_21::ANIME_ATTACK_HEAVY ||
		*m_pPlayerAnimationIndex == CPlay_21::ANIME_ATTACK_LIGHT1 || *m_pPlayerAnimationIndex == CPlay_21::ANIME_ATTACK_LIGHT2 || *m_pPlayerAnimationIndex == CPlay_21::ANIME_ATTACK_LIGHT3 ||
		*m_pPlayerAnimationIndex == CPlay_21::ANIME_ATTACK_CROUCH_LIGHT || *m_pPlayerAnimationIndex == CPlay_21::ANIME_ATTACK_CROUCH_MEDUIM )
	{
		m_pPlayer->Set_NextAnimation(CPlay_21::ANIME_ATTACK_CROUCH_HEAVY, 0.5f);
	}

	else if (*m_pPlayerAnimationIndex == CPlay_21::ANIME_JUMP_UP || *m_pPlayerAnimationIndex == CPlay_21::ANIME_JUMP_DOWN)
	{
		m_pPlayer->Set_Animation(CPlay_21::ANIME_ATTACK_UPPER_AIR);
	} 
	else if(*m_pPlayerAnimationIndex == CPlay_21::ANIME_ATTACK_AIR1 || *m_pPlayerAnimationIndex == CPlay_21::ANIME_ATTACK_AIR2 || *m_pPlayerAnimationIndex == CPlay_21::ANIME_ATTACK_SPECIAL)
	{
		m_pPlayer->Set_NextAnimation(CPlay_21::ANIME_ATTACK_UPPER_AIR, 0.5f);
	}




}

void CS21_MeleeAttack::Attack_Crouch_Speical()
{

	if (*m_pPlayerAnimationIndex == CPlay_21::ANIME_CROUCHING || *m_pPlayerAnimationIndex == CPlay_21::ANIME_FORWARD_DASH || *m_pPlayerAnimationIndex == CPlay_21::ANIME_FORWARD_DASH_END)
	{
		m_pPlayer->Set_Animation(CPlay_21::ANIME_ATTACK_CROUCH_SPECIAL);

		if (m_pPlayer->Get_fHeight() < 1)
		{
			//m_pPlayer->Set_fImpulse({ m_pPlayer->Get_iDirection() * 0.5f,0.5f });
			m_pPlayer->Set_fImpulse({ m_pPlayer->Get_iDirection() * 0.5f,0.2f });

		}
		else
		{
			m_pPlayer->Set_fImpulse({ m_pPlayer->Get_iDirection() * 0.5f,0.f });
		}

		//m_pPlayer->Set_fGravityTime(0.07f);
		m_pPlayer->Set_fGravityTime(0.1f);


		m_pPlayer->Add_Move({ 0.f,0.05f });
	}

}


void CS21_MeleeAttack::BackDash()
{

	if (m_pPlayer->Check_bCurAnimationisGroundMove())
	{
		m_pPlayer->Set_Animation(CPlay_21::ANIME_BACK_DASH);
	}

	else if (m_pPlayer->Get_bAirDashEnable() && (*m_pPlayerAnimationIndex == CPlay_21::ANIME_JUMP_DOWN || *m_pPlayerAnimationIndex == CPlay_21::ANIME_JUMP_UP))
	{
		m_pPlayer->Set_Animation(CPlay_21::ANIME_BACK_DASH);
		m_pPlayer->Set_CurrentAnimationPositionJump(12.f);
		m_pPlayer->Set_fImpulse(m_pPlayer->Get_iDirection() * (-10.f));

		m_pPlayer->Set_bAirDashEnable(false);
		m_pPlayer->Set_ForcedGravityDown();
		m_pPlayer->Set_ForcveGravityTime(0.135f);


		m_pPlayer->Character_Make_Effect(TEXT("Moving_Line_Left"), { -0.3f,0.f });

		m_pGameInstance->Play_Sound(CSound_Manager::SOUND_KEY_NAME::Common_Back_Dash_SFX, false, 1.f);
	}

}

void CS21_MeleeAttack::ForwardDash()
{
	if (*m_pPlayerAnimationIndex == CPlay_21::ANIME_IDLE || *m_pPlayerAnimationIndex == CPlay_21::ANIME_FORWARD_WALK || *m_pPlayerAnimationIndex == CPlay_21::ANIME_BACK_WALK)
	{
		m_pPlayer->Set_Animation(CPlay_21::ANIME_FORWARD_DASH);
		m_pGameInstance->Play_Sound(CSound_Manager::SOUND_KEY_NAME::Common_Dash_Start_SFX, false, 1.f);

		CEffect_Layer::COPY_DESC tDesc{};
		tDesc.pPlayertMatrix = static_cast<CTransform*>(m_pPlayer->Get_Component(TEXT("Com_Transform")))->Get_WorldMatrixPtr();
		m_pEffect_Manager->Copy_Layer(TEXT("Smoke_Run"), &tDesc);

		m_pGameInstance->Play_Sound(CSound_Manager::SOUND_KEY_NAME::Common_Dash_Start_SFX, false, 1.f);

	}

	else if (m_pPlayer->Get_bAirDashEnable() && (*m_pPlayerAnimationIndex == CPlay_21::ANIME_JUMP_DOWN || *m_pPlayerAnimationIndex == CPlay_21::ANIME_JUMP_UP))
	{
		m_pPlayer->Set_Animation(CPlay_21::ANIME_FORWARD_DASH);
		//m_pPlayer->Set_CurrentAnimationPositionJump(4.f);
		m_pPlayer->Set_fImpulse(m_pPlayer->Get_iDirection() * (10.f));
		m_pPlayer->Set_bAirDashEnable(false);
		//m_pPlayer->Set_ForcedGravityDown();

		m_pPlayer->Set_ForcveGravityTime(0.255f);



		//m_pPlayer->Character_Make_Effect(TEXT("Moving_Line_Right"));
		//m_pPlayer->Character_Make_Effect(TEXT("Ring_Dust_Right"), { 0.9f,0.f });


		//m_pEffect_Manager->Copy_Layer(TEXT("Dash"), m_pPlayer->Get_pTransformMatrix());
		m_pPlayer->Character_Make_Effect(TEXT("Dash"), { 1.2f,0.f });

		m_pGameInstance->Play_Sound(CSound_Manager::SOUND_KEY_NAME::Common_Back_Dash_SFX, false, 1.f);
	}

	else if (m_pPlayer->Get_bSparking() && m_pPlayer->Get_bAirDashEnable() && m_pPlayer->Get_bAttackBackEvent() &&
		(*m_pPlayerAnimationIndex == CPlay_21::ANIME_ATTACK_CROUCH_SPECIAL || *m_pPlayerAnimationIndex == CPlay_21::ANIME_ATTACK_AIR1 || *m_pPlayerAnimationIndex == CPlay_21::ANIME_ATTACK_AIR2))
	{
		m_pPlayer->Set_Animation(CPlay_21::ANIME_FORWARD_DASH);
		//m_pPlayer->Set_CurrentAnimationPositionJump(4.f);
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

void CS21_MeleeAttack::Reflect()
{
	if (m_pPlayer->Check_bCurAnimationisGroundMove())
	{
		m_pPlayer->Set_Animation(CPlay_21::ANIME_REFLECT);

	}

	else if (m_pPlayer->Get_bAttackBackEvent() && m_pPlayer->Check_bCurAnimationisReflect())
	{
		m_pPlayer->Set_Animation(CPlay_21::ANIME_REFLECT);
		m_pPlayer->Set_CurrentAnimationPositionJump(0.f);

	}

}

void CS21_MeleeAttack::Attack_Benishing()
{
	if (CBattleInterface_Manager::Get_Instance()->Get_KiNumber(m_pPlayer->Get_iPlayerTeam()) == 0 || m_pbAttackCount[CPlay_21::COUNT_ATTACK_BENISHING] == false)
		return;


	if (m_pPlayer->Get_bAttackBackEvent() &&
		(*m_pPlayerAnimationIndex == CPlay_21::ANIME_ATTACK_AIR1 || *m_pPlayerAnimationIndex == CPlay_21::ANIME_ATTACK_AIR2 || *m_pPlayerAnimationIndex == CPlay_21::ANIME_ATTACK_AIR3 ||
			*m_pPlayerAnimationIndex == CPlay_21::ANIME_ATTACK_LIGHT3 ||
			*m_pPlayerAnimationIndex == CPlay_21::ANIME_ATTACK_HEAVY || *m_pPlayerAnimationIndex == CPlay_21::ANIME_ATTACK_CROUCH_HEAVY))
	{

		if (CBattleInterface_Manager::Get_Instance()->Use_KiGuage(1, m_pPlayer->Get_iPlayerTeam()))
		{
			m_pbAttackCount[CPlay_21::COUNT_ATTACK_BENISHING] = false;
			//m_pPlayer->Set_NextAnimation(CPlay_21::ANIME_ATTACK_AIR2,2.f,0.f);
			m_pPlayer->Set_Animation(CPlay_21::ANIME_ATTACK_AIR2);
			m_pPlayer->Set_CurrentAnimationPositionJump(24.99);
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
			m_pbAttackCount[CPlay_21::COUNT_ATTACK_BENISHING] = false;
			//m_pPlayer->Set_NextAnimation(CPlay_21::ANIME_ATTACK_AIR2,2.f,0.f);
			m_pPlayer->Set_Animation(CPlay_21::ANIME_ATTACK_AIR2);
			m_pPlayer->Set_CurrentAnimationPositionJump(24.99);
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


