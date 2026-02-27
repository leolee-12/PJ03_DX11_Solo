#include "stdafx.h"
#include "..\Public\Play_21.h"

#include "RenderInstance.h"
#include "GameInstance.h"

#include "UI_Manager.h"
#include "Main_Camera.h"
//#define ANIME_ATTACK_LIGHT1 43
//#define ANIME_ATTACK_LIGHT2 44
//#define ANIME_ATTACK_LIGHT3 47
//
//#define ANIME_ATTACK_MEDIUM 46
//
//#define ANIME_ATTACK_HEAVY 45
//
//#define ANIME_IDLE 0
//#define ANIME_FORWARD_WALK 9
//#define ANIME_BACK_WALK 10
//
//
//#define ANIME_ATTACK_236 66
//#define ANIME_ATTACK_236_Air 67
//
//#define ANIME_ATTACK_236_Air 67
//
//
//#define ANIME_JUMP_UP 6;
//#define ANIME_JUMP_DOWN 7;


#include "iostream"
#include "Animation.h"


#include "AttackObject_Grab.h"
#include "AttackObject_CommandGrab.h"
#include "Main_Camera.h"

#include "BattleInterface.h"
#include "AttackObject_Ranged.h"

#include "AttackObject_Reflect.h"

#include "Effect_Manager.h"
#include "Effect.h"
//#include "Effect_Layer.h"

#include "Map_Manager.h"
#include "Particle_Manager.h"

#include "Particle_Manager.h"

#include"Level_GamePlay.h"
CPlay_21::CPlay_21(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCharacter{ pDevice, pContext }
{

}

CPlay_21::CPlay_21(const CPlay_21& Prototype)
	: CCharacter{ Prototype }
{

}

HRESULT CPlay_21::Initialize_Prototype()
{
	//m_pFrameEvent = CFrameEvent_Manager::Get_Instance()->Get_pFrameEventMap();
	CFrameEvent_Manager::Get_Instance()->Initalize_NameMap();
	CFrameEvent_Manager::Get_Instance()->LoadFile2("../Bin/FrameEventData/EventData_21.txt");



	return S_OK;
}

HRESULT CPlay_21::Initialize(void* pArg)
{

	//Play_21_DESC* pDesc = static_cast<Play_21_DESC*>(pArg);
	//m_strModelName = pDesc->strModelName;

	//m_pFrameEvent = CFrameEvent_Manager::Get_Instance()->Get_pFrameEventMap();

	m_fAIrGrabEndAnimationPositon = 29.99f;

	m_eCharacterIndex = PLAY_21;
	m_iFallAnimationIndex = ANIME_JUMP_DOWN;
	m_iIdleAnimationIndex = ANIME_IDLE;
	m_iJumpAnimationIndex = ANIME_JUMP_UP;
	m_iStandingMidAttackAnimationIndex = ANIME_ATTACK_MEDIUM;

	m_iChaseAnimationIndex = ANIME_CHASE;
	m_iForwardDashAnimationIndex = ANIME_FORWARD_DASH;
	m_iForwardDashEndAnimationIndex = { ANIME_FORWARD_DASH_END };

	m_iCrouchAnimationIndex = { ANIME_CROUCHING };
	m_iBackWalkAnimationIndex = { ANIME_BACK_WALK };
	m_iForwardWalkAnimationIndex = { ANIME_FORWARD_WALK };
	m_iHit_Away_LeftAnimationIndex = { ANIME_HIT_HEAVY_AWAY_LEFT };
	m_iHit_Away_UpAnimationIndex = { ANIME_HIT_HEAVY_AWAY_UP };
	m_iHit_Away_LeftDownAnimationIndex = { ANIME_HIT_HEAVY_AWAY_LEFTDOWN };

	m_iHit_WallBouce = { ANIME_HIT_WALLBOUNCE };

	m_iHit_Air_LightAnimationIndex = { ANIE_HIT_AIR };
	m_iHit_Air_FallAnimationIndex = { ANIME_HIT_FALL };

	m_iHit_Air_Spin_LeftUp = { ANIME_HIT_HEAVY_AWAY_SPIN_LEFTUP };
	m_iHit_Air_Spin_Up = { ANIME_HIT_HEAVY_AWAY_SPIN_UP };

	m_iAttack_Air1 = { ANIME_ATTACK_AIR1 };
	m_iAttack_Air2 = { ANIME_ATTACK_AIR2 };
	m_iAttack_Air3 = { ANIME_ATTACK_AIR3 };
	m_iAttack_AirUpper = { ANIME_ATTACK_UPPER_AIR };

	m_iAttack_LightLast = { ANIME_ATTACK_LIGHT3 };
	m_iAttack_Heavy = { ANIME_ATTACK_HEAVY };
	m_iAttack_Crouch_Heavy = { ANIME_ATTACK_CROUCH_HEAVY };

	m_iBound_Ground = { ANIME_HIT_BOUND_DOWN };
	m_iLayUp = { ANIME_LAYUP };

	m_iGuard_GroundAnimationIndex = { ANIME_GUARD_GROUND };
	m_iGuard_CrouchAnimationIndex = { ANIME_GUARD_CROUCH };
	m_iGuard_AirAnimationIndex = { ANIME_GUARD_AIR };

	m_iGrabReadyAnimationIndex = { ANIME_GRAB_READY };
	m_iGrabAnimationIndex = { ANIME_GRAB };



	m_iJumpAnimationIndex = { ANIME_JUMP_UP };
	m_iHit_Stand_LightAnimationIndex = { ANIME_HIT_LIGHT };
	m_iHit_Stand_MediumAnimationIndex = { ANIME_HIT_LIGHT_FRONT };
	m_iHit_Crouch_AnimationIndex = { ANIME_HIT_CROUCH };
	m_iBreakFall_Ground = { ANIME_BREAK_FALL_GROUND };   //100
	m_iBreakFall_Air = { ANIME_BREAK_FALL_AIR };   //101


	m_iSparkingAnimationIndex = { ANIME_SPARKING };

	m_iDyingStandingAnimationIndex = { ANIME_DIE_STAND };

	m_iReflectAnimationIndex = { ANIME_REFLECT };

	m_iStartAnimatonIndex = { ANIME_START_DEFAULT }; //600cs 
	m_iWinAnimationIndex = { ANIME_WIN_DEFAULT };	//610cs
	m_iNextRound_RightHandAppear_Cutscene_AnimationIndex = { ANIME_NEWROUND_RIGHTHAND_APEEAR_CUTSCENE }; //620c
	m_iNextRound_RightHand_AnimationIndex = { ANIME_NEWROUND_RIGHTHAND };  //621cs ->631으로 연계

	m_iNextRound_LeftHand_Cutscene_AnimationIndex = { ANIME_NEWROUND_LEFTHAND_CUTSCENE };  //630 Durtaio
	m_iNextRound_LeftHand_AnimationIndex = { ANIME_NEWROUND_LEFTHAND };  //631 Durtaion 24




	m_ChaseEffectName = TEXT("21_BurstR");

	m_iNextAnimation.first = ANIME_IDLE;


	m_eChaseSoundIndex = (_short)CSound_Manager::SOUND_KEY_NAME::Chase_Attack_21;


	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Character_DESC* pDesc = static_cast<Character_DESC*>(pArg);
	m_strName = "S21_" + to_string(m_iPlayerTeam) + to_string(pDesc->ePlayerSlot);
	m_RendererDesc.strName = m_strName;

	LIGHT_DESC			LightDesc{};

	LightDesc.eType = LIGHT_DESC::TYPE_DIRECTIONAL;

	LightDesc.vDirection = _float4(-0.06f, -0.07f, 0.1f, 0.f);
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.0f, 1.0f);
	LightDesc.vAmbient = _float4(0.5f, 0.5f, 0.5f, 1.f);
	LightDesc.vSpecular = _float4(0.f, 0.f, 0.f, 1.f);
	//LightDesc.vAuraColor = _float4(4.073, 1.887, 5.265, 4.285);
	LightDesc.pPlayerDirection = &m_iLookDirection;
	LightDesc.strName = m_strName;

	LightDesc.vAuraColor = _float4(0.f, 0.f, 0.f, 0.f);
	m_fAuraColor = _float4(4.073, 1.887, 5.265, 4.285);



	if (FAILED(m_pRenderInstance->Add_Player_Light(m_strName, LightDesc, _float4(2.f, 1.10196f, 1.73333f, 1.f), &m_bChase)))
		return E_FAIL;

	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 0.f, 0.f, 1.f));


	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(-2.f + (m_iPlayerTeam * 2), 0.f, 0.f, 1.f));
	m_tAttackMap.Initalize(this);

	//m_pModelCom->SetUp_Animation(16, true);
	m_pModelCom->SetUp_Animation(ANIME_IDLE, true);
	m_pModelCom->Play_Animation(0.f);




	//MoveCommandPatterns.push_back({ hadoukenPattern, ShowInputBuffer});

	MoveCommandPatterns.push_back({ Command_236Attack, 66 });
	MoveCommandPatterns.push_back({ Command_236Attack_Extra, 66 });

	MoveCommandPatterns.push_back({ Command_LightAttack, ANIME_ATTACK_LIGHT1 });
	MoveCommandPatterns.push_back({ Command_MediumAttack, ANIME_ATTACK_MEDIUM });
	MoveCommandPatterns.push_back({ Command_HeavyAttack, ANIME_ATTACK_HEAVY });


	MoveCommandPatternsFunction.push_back({ Command_236UltimateAttack, bind(&CS21_MeleeAttack::Attack_236Ultimate, &m_tAttackMap) });
	//MoveCommandPatternsFunction.push_back({ Command_236UltimateAttack_Side, bind(&CS21_MeleeAttack::Attack_236Ultimate_Side, &m_tAttackMap) });




	MoveCommandPatternsFunction.push_back({ Command_236Attack,  bind(&CS21_MeleeAttack::Attack_236, &m_tAttackMap) });
	MoveCommandPatternsFunction.push_back({ Command_236Attack_Extra,  bind(&CS21_MeleeAttack::Attack_236, &m_tAttackMap) });

	MoveCommandPatternsFunction.push_back({ Command_214Attack,  bind(&CS21_MeleeAttack::Attack_214, &m_tAttackMap) });
	MoveCommandPatternsFunction.push_back({ Command_214Attack_Extra,  bind(&CS21_MeleeAttack::Attack_214, &m_tAttackMap) });

	MoveCommandPatternsFunction.push_back({ Command_236Special,  bind(&CS21_MeleeAttack::Attack_236Special, &m_tAttackMap) });
	//MoveCommandPatternsFunction.push_back({ Command_236Special_Side,  bind(&CS21_MeleeAttack::Attack_236Special_Side, &m_tAttackMap) });


	MoveCommandPatternsFunction.push_back({ Command_214FinalAttack, bind(&CS21_MeleeAttack::Attack_214Final, &m_tAttackMap) });


	MoveCommandPatternsFunction.push_back({ Command_Crouch_LightAttack, bind(&CS21_MeleeAttack::Attack_Crouch_Light, &m_tAttackMap) });
	MoveCommandPatternsFunction.push_back({ Command_Crouch_MediumAttack, bind(&CS21_MeleeAttack::Attack_Crouch_Medium, &m_tAttackMap) });
	MoveCommandPatternsFunction.push_back({ Command_Crouch_HeavyAttack, bind(&CS21_MeleeAttack::Attack_Crouch_Heavy, &m_tAttackMap) });


	MoveCommandPatternsFunction.push_back({ Command_Crouch_MediumAttack_Extra, bind(&CS21_MeleeAttack::Attack_Crouch_Medium, &m_tAttackMap) });
	MoveCommandPatternsFunction.push_back({ Command_Crouch_HeavyAttack_Extra, bind(&CS21_MeleeAttack::Attack_Crouch_Heavy, &m_tAttackMap) });



	//위에서 부터 확인하므로 간단한 커맨드가 아래로 가야함
	MoveCommandPatternsFunction.push_back({ Command_LightAttack, bind(&CS21_MeleeAttack::Attack_Light, &m_tAttackMap) });
	MoveCommandPatternsFunction.push_back({ Command_MediumAttack, bind(&CS21_MeleeAttack::Attack_Medium, &m_tAttackMap) });
	MoveCommandPatternsFunction.push_back({ Command_HeavyAttack, bind(&CS21_MeleeAttack::Attack_Heavy, &m_tAttackMap) });
	MoveCommandPatternsFunction.push_back({ Command_SpecialAttack, bind(&CS21_MeleeAttack::Attack_Special, &m_tAttackMap) });


	MoveCommandPatternsFunction.push_back({ Command_HeavyAttack_Extra, bind(&CS21_MeleeAttack::Attack_Heavy, &m_tAttackMap) });



	MoveCommandPatternsFunction.push_back({ Command_Grab, bind(&CS21_MeleeAttack::Attack_Grab, &m_tAttackMap) });


	MoveCommandPatternsFunction.push_back({ Command_BackDash, bind(&CS21_MeleeAttack::BackDash, &m_tAttackMap) });
	MoveCommandPatternsFunction.push_back({ Command_Forward, bind(&CS21_MeleeAttack::ForwardDash, &m_tAttackMap) });

	MoveCommandPatternsFunction.push_back({ Command_Crouch_SpecialAttack, bind(&CS21_MeleeAttack::Attack_Crouch_Speical, &m_tAttackMap) });

	MoveCommandPatternsFunction.push_back({ Command_Reflect, bind(&CS21_MeleeAttack::Reflect, &m_tAttackMap) });
	MoveCommandPatternsFunction_Exactly.push_back({ Command_BenishingAttack, bind(&CS21_MeleeAttack::Attack_Benishing, &m_tAttackMap) });

	m_eCharacterID = CUI_Define::PLAYER_ID::ANDROID21;
	CBattleInterface_Manager::Get_Instance()->Regist_Character(m_iPlayerTeam, this, m_ePlayerSlot);
	if (m_ePlayerSlot != CUI_Define::PLAYER_SLOT::LPLAYER1 && m_ePlayerSlot != CUI_Define::PLAYER_SLOT::RPLAYER1)
	{
		m_bPlaying = false;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(5.f * m_iPlayerTeam, 100.f, 0.f, 1.f));

		//캐릭터 사이즈에 맞게 각자 추가하느라 m_pColliderCom이 없음
		m_pColliderCom->Update(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	}
	else
		m_bPlaying = true;


	return S_OK;
}

void CPlay_21::Player_Update(_float fTimeDelta)
{
	__super::Player_Update(fTimeDelta);


	Update_Tag_In(fTimeDelta);

	if (m_bPlaying == false)
		return;



	if (m_pGameInstance->Key_Down(DIK_TAB))
	{
		m_bDebugInputLock = !m_bDebugInputLock;
	}


	Update_Opening(fTimeDelta);

	if (m_bDebugInputLock)
		return;


	if (m_bDying)
	{
		if (m_bAnimationLock == true)
			Update_AnimationLock(fTimeDelta);
		else
		{
			Character_Play_Animation(fTimeDelta);

			if (Get_fHeight() > 0)
				Update_StunImpus(fTimeDelta);

			Gravity(fTimeDelta);

			if (Check_bWall())
			{
				Move_ForWall();
			}

			_uint iAnimationIndex = m_pModelCom->m_iCurrentAnimationIndex;

			m_fAccDyingTime += fTimeDelta;
			if (m_fAccDyingTime > m_fMaxDyingTime)
			{
				if (m_bFinalSkillRoundEndSolo)
				{
					//5초 한바퀴만 더 
					if (m_bSoloFinalEndCount == false)
					{
						m_fAccDyingTime = 0.f;
						m_bSoloFinalEndCount = true;
						//WIN UI 띄우기
						CUI_Manager::Get_Instance()->WinUI(LEVEL_GAMEPLAY);

						return;
					}
					else
					{
						//로비로 이동
						static_cast<CLevel_GamePlay*>(m_pGameInstance->Get_Level())->Change_Level_ForCharacter();

					}

				}
				else
				{
					m_bDestructiveFinish = false;
					CBattleInterface_Manager::Get_Instance()->Check_NextRoundFromDeathCharacter(m_iPlayerTeam, Get_NewCharacterslot());
					m_pColliderCom->Update(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
					m_bPlaying = false;
				}
			}
			else if (m_bDyingBlack && m_fAccDyingTime > m_fMaxDyingTime - 0.6f)
			{

				if (m_bFinalSkillRoundEndSolo == false)
					CRenderInstance::Get_Instance()->Switch_AllBlackOut();

				m_bDyingBlack = false;
			}
			else if (iAnimationIndex == m_iDyingStandingAnimationIndex)
			{
				Stun_Shake();
			}
		}

		return;
	}
	else
		Update_Dying(fTimeDelta);

	Update_FinalSkill(fTimeDelta);
	Update_LoofAnimationCreate(fTimeDelta);
	Update_PreviousXPosition();



	if (m_bGrabbed)
	{

		if (m_bAnimationLock && m_fMaxAnimationLock == 0.f)
		{
			m_bAnimationLock = false;
		}

		if (m_bGrabbedGravity && m_bAnimationLock == false)
		{
			if (Get_fHeight() > 0)
			{
				_float fGravity = (-0.7f * (2 * m_fGravityTime - m_fJumpPower) * (2 * m_fGravityTime - m_fJumpPower) + 4) * 0.1;
				//Add_Move({ 0,-fGravity });
				Add_Move({ m_fImpuse.x * fTimeDelta, -fGravity + m_fImpuse.y * fTimeDelta });
			}

			if (m_fGravityTime * 2.f < m_fJumpPower)
			{
				m_fGravityTime += fTimeDelta;

			}

		}
		if (m_bAnimationLock == false)
		{
			Character_Play_Animation(fTimeDelta);
			if (m_pModelCom->m_iCurrentAnimationIndex == m_iStartAnimatonIndex)
			{
				if (m_bMotionPlaying == false)
				{
					if (m_fAccfirstOpeningTime >= 0.5f)
					{
						m_fAccfirstOpeningTime = 0.f;
						CBattleInterface_Manager::Get_Instance()->Character_Opening_EndForCharacter(m_iPlayerTeam);
					}
					else
					{
						if (m_fAccfirstOpeningTime == 0)
						{
							CRenderInstance::Get_Instance()->Switch_AllBlackOut();
						}
						m_fAccfirstOpeningTime += fTimeDelta;
					}
				}
				else if (m_pGameInstance->Key_Down(DIK_RETURN))
				{
					//시작 애니메이션 끝부분  21호 끝 466
					Set_CurrentAnimationPositionJump(465.99);
				}
			}
		}
		else
		{
			Update_AnimationLock(fTimeDelta);
		}
		m_pColliderCom->Update(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		return;
	}



	Update_BeReflecting(fTimeDelta);


	pEnemyCheck();


	//방향전환 코드.  적 탐지가 추가된 이후엔  CCharacter로 옮기기
	if (Check_bCurAnimationisGroundMove() || m_pModelCom->m_iCurrentAnimationIndex == m_iJumpAnimationIndex || m_pModelCom->m_iCurrentAnimationIndex == m_iFallAnimationIndex)
	{
		CTransform* pEnemyTransform = static_cast<CTransform*>(m_pEnemy->Get_Component(TEXT("Com_Transform")));

		//적 방향의 X값 체크
		_float fX = XMVectorGetX(pEnemyTransform->Get_State(CTransform::STATE_POSITION) - m_pTransformCom->Get_State(CTransform::STATE_POSITION));


		//차이가 좁으면 반전 안함. 둘 다 벽에 붙어있을 때 대비.
		if (fabsf(fX) > 0.05)
		{

			if (fX > 0)
			{
				FlipDirection(1);
			}
			else
			{
				FlipDirection(-1);
			}

		}
	}




	//입력되어있는 커맨드 시간에 따라 휘발
	InputedCommandUpdate(fTimeDelta);

	//Chase == true거나  Stun==ture면 안받음
	if (m_bChase == false && m_bStun == false)   //선입력을 받아야 하는가?
	{
		if (InputCommand())
		{
			CheckAllCommands();
		}

	}
	else   //입력 불가 상태여도 선입력받음.  커맨드 체크는 안하고 입력만 받음
	{
		InputCommand();
	}





	if (m_bAnimationLock == false)
	{

		Sparking_ON(fTimeDelta);

		//추적 관련 코드.
		if (m_iPlayerTeam == 1)
		{
			if (m_pGameInstance->Key_Down(DIK_R))
			{
				Chase_Ready(fTimeDelta);
			}
		}
		else  //Team2
		{
			if (m_pGameInstance->Key_Down(DIK_PGDN))  //PageDown 키
			{
				Chase_Ready(fTimeDelta);
			}
		}


		if (m_bChase)
		{
			Chase2(fTimeDelta);

			if (m_bChase == false)
			{
				//평범하게 체이스가 끝났으면 이렇게.
				if (m_bStun == false)
				{
					m_fAccChaseTime = 0.f;
					m_fGravityTime = 0.185f;
					Set_Animation(ANIME_JUMP_DOWN, false);
				}
				else //맞아서 체이스가 끝났으면 
				{
					m_fAccChaseTime = 0.f;
					m_fGravityTime = 0.185f;
				}

			}

		}
		else
		{
			if (m_bChaseStoping)
			{
				m_bChaseStoping = false;
				Set_Animation(m_iFallAnimationIndex);
			}
		}

		if (m_bGrab)
		{
			Chase_Grab(fTimeDelta);
			Character_Attack_Grab(fTimeDelta);
		}


		if (m_bNoEventLoofAnimation)
		{
			Update_NoEventTime(fTimeDelta);

			if (m_bForcedEventLoofAnimation)
			{
				Update_ForcedEventAnimationLoof(fTimeDelta);
			}
			else
			{
				Update_NoEventAnimationLoof(fTimeDelta);
			}
		}
		else
			Character_Play_Animation(fTimeDelta);


		//이건 반복재생이 아닌데 모션이 끝난경우 (=움직임 자체가 멈췄을 경우),  추락 등 몇몇 애니메이션 제외
		if (m_bMotionPlaying == false)
		{

			//스턴 추가 전에 있던거
			//if(m_pModelCom->m_iCurrentAnimationIndex != ANIME_JUMP_DOWN)
			//	AnimeEndNextMoveCheck();



			//if (Check_bCurAnimationisAirHit() || Check_bCurAnimationisHitAway() || m_pModelCom->m_iCurrentAnimationIndex == m_iHit_Air_Spin_LeftUp)
			if (Check_bCurAnimationisAirHit() || Check_bCurAnimationisHitAway())
			{

				if (m_bHitGroundSmashed == false)
					Set_Animation(m_iHit_Air_FallAnimationIndex);

			}
			else if (m_pModelCom->m_iCurrentAnimationIndex == m_iHit_Air_Spin_LeftUp)
			{
				Set_Animation(m_iHit_Air_LightAnimationIndex);
				m_pModelCom->CurrentAnimationPositionJump(30.f);
			}

			else if (m_pModelCom->m_iCurrentAnimationIndex == m_iHit_WallBouce)
			{
				Set_Animation(m_iHit_Air_FallAnimationIndex);
			}
			else if (m_pModelCom->m_iCurrentAnimationIndex == m_iBound_Ground)
			{
				Set_Animation(ANIME_LAYUP);
				Set_CurrentAnimationPositionJump(5.f);
			}
			else if (m_bStun == false && m_pModelCom->m_iCurrentAnimationIndex != m_iFallAnimationIndex && m_pModelCom->m_iCurrentAnimationIndex != m_iHit_Air_LightAnimationIndex)
				AnimeEndNextMoveCheck();

		}

		//이거맞나?   m_bMotionPlaying 중인데 스턴은 끝났고 Hit Away 중인 경우
		else if (m_bMotionPlaying && Check_bCurAnimationisHitAway() && m_bStun == false)
		{
			Set_Animation(m_iHit_Air_FallAnimationIndex);

		}


		//애니메이션 lock 도중에는 발동 안해야함.   아래에 넣자니 
		if (m_bStun)
		{
			Stun_Shake();
			m_fAccStunTime += fTimeDelta;
			//cout << "Stun Time :" << m_fAccStunTime  <<" / " <<m_fMaxStunTime << endl;
			if (m_fAccStunTime > m_fMaxStunTime)
			{
				m_bStun = false;
				m_fAccStunTime = 0.f;

				if (Check_bCurAnimationisHitGround())
				{
					Reset_AttackStep();
				}

			}
			Update_StunImpus(fTimeDelta);
		}
		else
		{




			BreakFall_Air();


			if (m_pModelCom->m_iCurrentAnimationIndex == m_iIdleAnimationIndex && Get_fHeight() == 0)
				m_fImpuse = { 0.f,0.f };



		}
		Gravity(fTimeDelta);


	}
	else
	{
		Update_AnimationLock(fTimeDelta);

		//m_fAccAnimationLock += fTimeDelta;
		//if (m_fAccAnimationLock > m_fMaxAnimationLock)
		//{
		//	m_fAccAnimationLock = 0.f;
		//	m_bAnimationLock = false;
		//}
	}


	//중력 처리.    ANimation Lock의 영향을 받아야하나? 위로 옮겨봄
	//Gravity(fTimeDelta);

	//일부 공격 캔슬
	AttckCancleJump();

	Sparking_TimeCount(fTimeDelta);




	Move(fTimeDelta);











	m_pColliderCom->Update(m_pTransformCom->Get_State(CTransform::STATE_POSITION));


	_float fPosX = Get_fPositionX();

	//12(벽) 이상 넘어가지 못하게 
	//if (fPosX > 12)
	//{
	//	Add_Move({ 12.f - fPosX, 0.f });
	//}
	//else if (fPosX < -12)
	//{
	//	Add_Move({ -12.f - fPosX, 0.f });
	//}

	if (Check_bWall())
	{
		//(Get_fPositionX() < -12.f || Get_fPositionX() > 12.f || fabsf(Get_fPositionX() - m_pEnemy->Get_fPositionX()) > 8);
		Move_ForWall();
	}

	Tag_KeyCheck();



	if (m_pGameInstance->Key_Down(DIK_8))
	{
		ShowInputBuffer();
	}
	if (m_pGameInstance->Key_Down(DIK_1))
	{
		if (m_pGameInstance->Key_Pressing(DIK_A))
		{
			DebugPositionReset(-1);
		}
		else if (m_pGameInstance->Key_Pressing(DIK_D))
		{
			DebugPositionReset(1);
		}
		else
			DebugPositionReset();


	}


	if (m_pGameInstance->Key_Down(DIK_4))
	{
		Set_bFinalSkillQTE(true);
	}
	//cout << "iHP : " << m_iHP << endl;
	if (m_pGameInstance->Key_Down(DIK_INSERT))
	{
		CBattleInterface_Manager::Get_Instance()->Gain_KiGuage(50, m_iPlayerTeam);
	}

	Check_Ground();

}

void CPlay_21::Camera_Update(_float fTimeDelta)
{

}

void CPlay_21::Update(_float fTimeDelta)
{

	__super::Player_Update(fTimeDelta);


}

void CPlay_21::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
}

HRESULT CPlay_21::Render(_float fTimeDelta)
{
	if (m_bInvisible == true)
		return S_OK;

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	/* Main MeshIndex : 0 */
	/* DramaticCamera MeshIndex : 1 */
	/* Shadow MeshIndex : 2 */
	/* Decal MeshIndex : 3 */
	/* Detail?? MeshIndex : 4 */
	for (size_t i = 0; i < iNumMeshes; i++)
	{
		/*if (i == 1 || i == 2 || i == 4)
			continue;*/
			/*if (i > 1 && i != 4)
				continue;*/
		if (i > 1)
			continue;

		/* 모델이 가지고 있는 머테리얼 중 i번째 메시가 사용해야하는 머테리얼구조체의 aiTextureType_DIFFUSE번째 텍스쳐를 */
		/* m_pShaderCom에 있는 g_DiffuseTexture변수에 던져. */
		_uint iPassIndex = { 1 };
		if (i == 4)
			iPassIndex = 4;

		if (i < 1)
		{
			if (FAILED(m_p2PTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", 0)))
				return E_FAIL;
		}
		else
		{

			if (FAILED(m_p2PTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", 0)))
				return E_FAIL;
			if (FAILED(m_pDecalTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DecalTexture", 0)))
				return E_FAIL;

		}

		//

		 //m_pModelCom->Bind_MaterialSRV(m_pShaderCom, aiTextureType_NORMALS, "g_NormalTexture", i);

		/* 모델이 가지고 있는 뼈들 중에서 현재 렌더링할려고 했던 i번째ㅑ 메시가 사용하는 뼈들을 배열로 만들어서 쉐이더로 던져준다.  */
		m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i);

		if (FAILED(m_pShaderCom->Begin(iPassIndex)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}

#ifdef _DEBUG
	m_pColliderCom->Render(fTimeDelta);
#endif // DEBUG
	return S_OK;
}

/*
void CPlay_21::NextMoveCheck()
{

	//지금 당장 공격중에 체크했을때 IDLE이면 변경하지 않고 공격을 마저 진행함

	//점프,이동 중이 아니라는 뜻


	//if (m_pModelCom->m_iCurrentAnimationIndex < m_iNextAnimation.first)
	//{
	//	//Set_Animation(m_iNextAnimation.first);
	//	m_pModelCom->SetUp_Animation(m_iNextAnimation.first,false);
	//	m_iNextAnimation.first = ANIME_IDLE;
	//	m_iNextAnimation.second = 100;
	//
	//}



	//공격중인경우  로 바꾸자  *체크*
	//if (Check_bCurAnimationisGroundMove() == false)
	//{
	//	//다음 애니메이션도 공격이 아니면 Set
	//	//if (Check_bCurAnimationisMove(m_iNextAnimation.first))
	//	//{
	//	//	Set_Animation(m_iNextAnimation.first);
	//	//}
	//
	//	if (m_iNextAnimation.first != ANIME_IDLE)
	//	{
	//		Set_Animation(m_iNextAnimation.first);
	//	}
	//
	//}

	// 체크*
	//if(m_iNextAnimation.first != ANIME_IDLE)
	//{
	//	m_pModelCom->SetUp_Animation(m_iNextAnimation.first, false);
	//
	//	m_iNextAnimation.first = ANIME_IDLE;
	//	m_iNextAnimation.second = 100.f;
	//}


}
*/


/*
void CPlay_21::AttackNextMoveCheck()
{

	//if (m_iNextAnimation.first != ANIME_IDLE)
	//{
	//	Set_Animation(m_iNextAnimation.first);
	//
	//	m_iNextAnimation.first = ANIME_IDLE;
	//	m_iNextAnimation.second = 1000.f;
	//
	//}

	if (m_iNextAnimation.first != ANIME_IDLE)
	{
		Set_Animation(m_iNextAnimation.first);

		m_iNextAnimation.first = ANIME_IDLE;
		m_iNextAnimation.second = 1000.f;

		if (m_fNextAnimationCurrentPosition != 0)
		{
			m_pModelCom->CurrentAnimationPositionJump(m_fNextAnimationCurrentPosition);
			m_fNextAnimationCurrentPosition = 0.f;
		}
	}


}

void CPlay_21::AnimeEndNextMoveCheck()
{

	Set_Animation(m_iNextAnimation.first);

	m_iNextAnimation.first = ANIME_IDLE;
	m_iNextAnimation.second = 1000.f;




}

*/

void CPlay_21::Test_InputCommand()
{
	//if (m_pGameInstance->Key_Down(DIK_U))
	//{
	//	m_pModelCom->SetUp_Animation(ANIME_ATTACK_LIGHT1,false);
	//}
	//
	//if (m_pGameInstance->Key_Down(DIK_I))
	//{
	//	m_pModelCom->SetUp_Animation(ANIME_ATTACK_MEDIUM, false);
	//
	//}
	//
	//
	//if (m_pGameInstance->Key_Down(DIK_K))
	//{
	//	m_pModelCom->SetUp_Animation(ANIME_ATTACK_HEAVY, false);
	//
	//}


}

void CPlay_21::Update_FinalSkill(_float fTimeDelta)
{

	//if (m_pModelCom->m_iCurrentAnimationIndex == ANIME_FINAL_START && m_bAttackBackEvent && m_bDynamicMove ==false)
	//{
	//	m_bDynamicMove = true;
	//}
	//
	//if (m_bFinalSkillSucess == false)
	//	return;
	//
	//
	//m_fAccFinalSkillTime += fTimeDelta;
	//
	//
	//if (m_fAccFinalSkillTime > 3.f)
	//{
	//	m_bFinalSkillSucess = false;
	//	AttackEvent(5);
	//}
	//
	//else if (m_fAccFinalSkillTime > 0.5f)
	//{
	//
	//}

	if (m_pModelCom->m_iCurrentAnimationIndex == ANIME_FINAL_END)
	{
		m_bDynamicMove = false;
		return;
	}

	if (m_pModelCom->m_iCurrentAnimationIndex != ANIME_FINAL_START)
		return;

	if (m_pModelCom->m_iCurrentAnimationIndex == ANIME_FINAL_START && m_bAttackBackEvent && m_bDynamicMove == false)
	{
		m_bDynamicMove = true;
	}



	if (m_pModelCom->m_iCurrentAnimationIndex == ANIME_FINAL_START && m_pModelCom->m_fCurrentAnimPosition < 230)
		return;


	m_fAccFinalSkillTime += fTimeDelta;



	if (m_fAccFinalSkillTime > 0.4f)
	{
		//AttackEvent(8);
		//화면 가리는 이펙트 생성, 애니메이션 변경 등

		Set_Animation(ANIME_FINAL_END);
	}
	//else if (m_fAccFinalSkillTime > 0.3f)
	//{
	//
	////	Character_Make_Effect(TEXT("Smoke_Run"), { -0.2f,0.f }, true);
	//	Character_Make_Effect(TEXT("Smoke_Run"), { -0.2f,0.f });
	//}
	//
	//else if (m_fAccFinalSkillTime > 0.20f)
	//{
	//	Character_Make_Effect(TEXT("Smoke_Run"), { -0.2f,0.f });
	//
	//}
	//else if (m_fAccFinalSkillTime > 0.10f)
	//{
	//	Character_Make_Effect(TEXT("Smoke_Run"), { -0.2f,0.f });
	//
	//}



}




/*
void CPlay_21::Set_Animation(_uint iAnimationIndex)
{


	//if(iAnimationIndex !=ANIME_IDLE)
	//	m_pModelCom->SetUp_Animation(iAnimationIndex,false);
	//
	//else
	//	m_pModelCom->SetUp_Animation(ANIME_IDLE, true);



	//ver2

	//if (m_pModelCom->m_iCurrentAnimationIndex != ANIME_IDLE)
	//{
	//	m_iNextAnimation.first = iAnimationIndex;
	//	m_iNextAnimation.second = 0.5f;
	//	return;
	//}
	//
	//
	//
	//
	//_uint iDebug = iAnimationIndex;
	//if (iAnimationIndex != 0 && (iAnimationIndex != ANIME_FORWARD_WALK && iAnimationIndex != ANIME_BACK_WALK))
	//{
	//	_bool bDebug = true;
	//}
	//
	//if(iAnimationIndex != ANIME_IDLE)
	//	m_pModelCom->SetUp_Animation(iAnimationIndex, false);



	//if (iAnimationIndex != ANIME_IDLE)
	//	m_pModelCom->SetUp_Animation(iAnimationIndex, true);
	//else
	//	m_pModelCom->SetUp_Animation(iAnimationIndex, false);



	if (iAnimationIndex == ANIME_IDLE)
		m_pModelCom->SetUp_Animation(iAnimationIndex, true);
	else
		m_pModelCom->SetUp_Animation(iAnimationIndex, false);


}
*/

void CPlay_21::KeyTest()
{

}



HRESULT CPlay_21::Ready_Components()
{
	/* Com_Shader */
	//if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
	//	TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
	//	return E_FAIL;
	//
	///* Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Play_21"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* Com_Model */
	//if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, m_strModelName,
	//	TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
	//	return E_FAIL;

	CBounding_AABB::BOUNDING_AABB_DESC ColliderDesc{};
	if (m_iPlayerTeam == 1)
		ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_1P_BODY;
	else
		ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_2P_BODY;
	ColliderDesc.pMineGameObject = this;
	ColliderDesc.vCenter = { 0.f,0.9f,0.f };
	ColliderDesc.vExtents = { 0.5f,0.8f,1.f };


	//Com_Collider
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_AABB"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
		return E_FAIL;

	m_pGameInstance->Add_ColliderObject(ColliderDesc.colliderGroup, m_pColliderCom);

	/* Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_21OutLine"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pOutLineCom))))
		return E_FAIL;

	if (m_iPlayerTeam == 1)
	{
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_TON_base"), TEXT("Com_2PTexture"), reinterpret_cast<CComponent**>(&m_p2PTextureCom))))
			return E_FAIL;

	}
	else
	{

		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_TON_base"), TEXT("Com_2PTexture"), reinterpret_cast<CComponent**>(&m_p2PTextureCom))))
			return E_FAIL;
	}
	/* Com_Model */

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_TON_decal"), TEXT("Com_DecalTexture"), reinterpret_cast<CComponent**>(&m_pDecalTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlay_21::Bind_ShaderResources()
{
	if (FAILED(__super::Bind_ShaderResources()))
		return E_FAIL;

	return S_OK;
}

void CPlay_21::Check_Ground()
{

	//_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_float fHeight = XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	if (fHeight <= fGroundHeight)
	{
		if (m_pModelCom->m_iCurrentAnimationIndex == ANIME_ATTACK_214)
		{
			if (m_pModelCom->Get_CurrentAnimationPosition() < 55)
			{
				m_pModelCom->CurrentAnimationPositionJump(55.f);
				//m_pModelCom->m_Animations[m_pModelCom->m_iCurrentAnimationIndex]->m_fTickPerSecond = 90.f;
				m_fGravityTime = 0.f;

				Reset_AttackCount();
			}
		}


	}

}

_bool CPlay_21::Check_bCurAnimationisGroundMove(_uint iAnimation)
{
	_uint iModelIndex = iAnimation;

	if (iAnimation == 1000)
		iModelIndex = m_pModelCom->m_iCurrentAnimationIndex;


	//if (iModelIndex == ANIME_IDLE || (iModelIndex == ANIME_FORWARD_WALK || iModelIndex == ANIME_BACK_WALK) )
	//{
	//	return true;
	//}


	//대시 추가하느라 주석
	//if (iModelIndex == ANIME_IDLE || (iModelIndex == ANIME_FORWARD_WALK || iModelIndex == ANIME_BACK_WALK) || iModelIndex == ANIME_CROUCH_START || iModelIndex == ANIME_CROUCHING)
	//{
	//	return true;
	//}


	if (iModelIndex == ANIME_IDLE || iModelIndex == ANIME_FORWARD_WALK || iModelIndex == ANIME_BACK_WALK || iModelIndex == ANIME_CROUCH_START || iModelIndex == ANIME_CROUCHING
		|| iModelIndex == ANIME_FORWARD_DASH)
	{
		return true;
	}

	return false;
}

_bool CPlay_21::Check_bCurAnimationisAttack(_uint iAnimation)
{

	return false;
}

_bool CPlay_21::Check_bCurAnimationisAirAttack(_uint iAnimation)
{
	_uint iModelIndex = iAnimation;

	if (iAnimation == 1000)
		iModelIndex = m_pModelCom->m_iCurrentAnimationIndex;



	if (iModelIndex == ANIME_ATTACK_AIR1 || iModelIndex == ANIME_ATTACK_AIR2 || iModelIndex == ANIME_ATTACK_AIR3)// || iModelIndex == ANIME_ATTACK_SPECIAL_AIR)
	{
		return true;
	}

	return false;
}

_short CPlay_21::Check_bCurAnimationisCanChase()
{
	_uint 	iModelIndex = m_pModelCom->m_iCurrentAnimationIndex;



	//공격중 체이스는 속도 좀 빠르게
	//if (m_bAttackBackEvent && (iModelIndex == ANIME_ATTACK_HEAVY || iModelIndex == ANIME_ATTACK_CROUCH_HEAVY || iModelIndex == ANIME_ATTACK_LIGHT3 || (iModelIndex == ANIME_ATTACK_214)&&m_pModelCom->m_fCurrentAnimPosition >=55 ))
	//{
	//	return 10;
	//}

	if (m_bAttackBackEvent && (iModelIndex == ANIME_ATTACK_HEAVY || iModelIndex == ANIME_ATTACK_CROUCH_HEAVY || iModelIndex == ANIME_ATTACK_LIGHT3))
	{
		return 10;

	}
	//else if (m_bAttackBackEvent &&  (iModelIndex == ANIME_ATTACK_214) && m_pModelCom->m_fCurrentAnimPosition >= 55.1)
	else if (m_bAttackBackEvent && (iModelIndex == ANIME_ATTACK_214) && m_pModelCom->m_fCurrentAnimPosition >= 60)
	{
		//return 45;

		//return 45; //시작을 214로했을때만 맞음 한 5히트만 해도 연계안됨

		return 50; //딱 5,6히트까지만 되는듯


	}

	else if (Check_bCurAnimationisGroundMove() || Check_bCurAnimationisAirMove())
	{
		return 1;
	}


	return 0;
}

void CPlay_21::Reset_AttackCount()
{

	for (size_t i = 0; i < COUNT_END; i++)
		m_bAttackCount[i] = true;

	m_bChaseEnable = true;
	m_bDoubleJumpEnable = true;
}

void CPlay_21::Gravity(_float fTimeDelta)
{
	_ushort iAnimationIndex = m_pModelCom->m_iCurrentAnimationIndex;


	if (iAnimationIndex == ANIME_ATTACK_CROUCH_SPECIAL)
	{
		if (Get_fHeight() > 0)
		{
			_float fGravity = (-0.7f * (2 * m_fGravityTime - m_fJumpPower) * (2 * m_fGravityTime - m_fJumpPower) + 4) * 0.1;
			m_pTransformCom->Add_Move({ m_fImpuse.x * fTimeDelta,-fGravity + m_fImpuse.y * fTimeDelta,0 });
		}

	}

	//3필중에 가속도대로 이동
	else if (iAnimationIndex == ANIME_FINAL_START)
	{
		m_pTransformCom->Add_Move({ m_fImpuse.x * fTimeDelta, m_fImpuse.y * fTimeDelta,0 });

		//내려가는경우는 내려찍기때밖에 없음.  처음일수도 있긴 한데  일단 예외처리 함
		//높이 0 대신 1이나 0.3쯤으로 낮추고 미리 먼지깔아두는 방식도 좋을듯

		//땅에
		if (Get_fHeight() < 0.2f && m_pModelCom->m_fCurrentAnimPosition > 225 && m_pModelCom->m_fCurrentAnimPosition < 230)
		{


			Character_Make_Effect(TEXT("Smoke01"));
			Character_Make_Effect(TEXT("Smoke01_BackZ"));
			Character_Make_Effect(TEXT("Smoke02"));
			Character_Make_Effect(TEXT("Smoke02_Small"));
			Character_Make_Effect(TEXT("Smoke04"));

			CEffect_Layer::COPY_DESC pDesc{};

			_float4x4 fCamMat = {};

			XMStoreFloat4((_float4*)&fCamMat.m[3][0], m_pGameInstance->Get_CamPosition_Vector());

			pDesc.pPlayertMatrix = &fCamMat;

			CEffect_Manager::Get_Instance()->Copy_Layer(TEXT("21_WSDO-04"), &pDesc);

			Set_CurrentAnimationPositionJump(230.f);

			if (m_pAttack214AssultEffect_Layer != nullptr)
			{
				m_pAttack214AssultEffect_Layer->m_bIsDoneAnim = true;
				m_pAttack214AssultEffect_Layer = nullptr;
			}


		}
	}


	__super::Gravity(fTimeDelta);



	//_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	//_float fHeight = XMVectorGetY(vPos);


	if (iAnimationIndex == ANIME_ATTACK_214 && m_pModelCom->m_fCurrentAnimPosition > 30.f && m_pModelCom->m_fCurrentAnimPosition < 50.f)
	{
		Add_Move({ m_fImpuse.x * fTimeDelta , m_fImpuse.y * fTimeDelta });  //땅에 닿았을때 처리는 현재 GroundCheck 에 있음
	}

	if (iAnimationIndex == ANIME_ATTACK_AIR1 || iAnimationIndex == ANIME_ATTACK_AIR2 || iAnimationIndex == ANIME_ATTACK_AIR3 || iAnimationIndex == ANIME_ATTACK_CROUCH_SPECIAL)
	{
		//if (fHeight == 0)
		if (Get_fHeight() == 0)
		{

			//m_pModelCom->SetUp_Animation(m_iIdleAnimationIndex, true);
			Set_Animation(m_iIdleAnimationIndex);


			Set_fGravityTime(0.f);
			//Set_fJumpPower(0.f);
			Set_fImpulse(0.f);
			m_bAriDashEnable = true;
			Set_bAttackGravity(true);
		}

	}

}

void CPlay_21::AttackEvent(_int iAttackEvent, _int AddEvent)
{
	switch (m_pModelCom->m_iCurrentAnimationIndex)
	{
	case Client::CPlay_21::ANIME_ATTACK_LIGHT1:
	{
		CAttackObject::ATTACK_DESC Desc{};

		if (m_iPlayerTeam == 1)
			Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_1P_Melee_Attack;
		else
			Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_2P_Melee_Attack;
		Desc.ColliderDesc.pMineGameObject = this;
		Desc.ColliderDesc.vCenter = { 0.9f * m_iLookDirection,0.8f,0.f };
		Desc.ColliderDesc.vExtents = { 0.3f,0.5f,0.2f };



		Desc.fhitCharacter_Impus = { 0.3f * m_iLookDirection,0 };
		Desc.fhitCharacter_StunTime = 0.3f;
		Desc.iDamage = 400 * Get_DamageScale();
		Desc.fLifeTime = 0.1f;
		Desc.ihitCharacter_Motion = { HitMotion::HIT_LIGHT };
		Desc.iTeam = m_iPlayerTeam;
		Desc.fAnimationLockTime = 0.1f;
		Desc.pOwner = this;

		m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack"), TEXT("Layer_AttackObject"), &Desc);
	}
	break;
	case Client::CPlay_21::ANIME_ATTACK_LIGHT2:
	{

		CAttackObject::ATTACK_DESC Desc{};

		if (m_iPlayerTeam == 1)
			Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_1P_Melee_Attack;
		else
			Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_2P_Melee_Attack;
		Desc.ColliderDesc.pMineGameObject = this;
		Desc.ColliderDesc.vCenter = { 0.9f * m_iLookDirection,0.8f,0.f };
		Desc.ColliderDesc.vExtents = { 0.3f,0.5f,0.2f };



		Desc.fhitCharacter_Impus = { 0.3f * m_iLookDirection,0 };
		Desc.fhitCharacter_StunTime = 0.4f;
		//Desc.iDamage = 5200 * Get_DamageScale(); //700이었음
		Desc.iDamage = 700 * Get_DamageScale(); //700이었음
		Desc.fLifeTime = 0.1f;
		Desc.ihitCharacter_Motion = { HitMotion::HIT_LIGHT };
		Desc.iTeam = m_iPlayerTeam;
		Desc.fAnimationLockTime = 0.1f;
		Desc.pOwner = this;

		m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack"), TEXT("Layer_AttackObject"), &Desc);
	}
	break;
	case Client::CPlay_21::ANIME_ATTACK_LIGHT3:
	{

		CAttackObject::ATTACK_DESC Desc{};

		if (m_iPlayerTeam == 1)
			Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_1P_Melee_Attack;
		else
			Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_2P_Melee_Attack;
		Desc.ColliderDesc.pMineGameObject = this;
		Desc.ColliderDesc.vCenter = { 0.9f * m_iLookDirection,0.8f,0.f };
		Desc.ColliderDesc.vExtents = { 0.3f,0.5f,0.2f };


		Desc.fhitCharacter_Impus = { 4.f * m_iLookDirection,2.f };
		//Desc.fhitCharacter_StunTime = 0.6f;
		Desc.fhitCharacter_StunTime = 0.8f;

		Desc.iDamage = 1000 * Get_DamageScale();
		Desc.fLifeTime = 0.1f;
		Desc.ihitCharacter_Motion = { HitMotion::HIT_SPIN_AWAY_LEFTUP };
		Desc.iTeam = m_iPlayerTeam;
		Desc.fAnimationLockTime = 0.7f;
		Desc.pOwner = this;

		m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack"), TEXT("Layer_AttackObject"), &Desc);
	}
	break;
	case Client::CPlay_21::ANIME_ATTACK_MEDIUM:
	{
		CAttackObject::ATTACK_DESC Desc{};
		//Desc.ColliderDesc.width = 0.7;
		//Desc.ColliderDesc.height = 0.8;
		if (m_iPlayerTeam == 1)
			Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_1P_Melee_Attack;
		else
			Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_2P_Melee_Attack;
		Desc.ColliderDesc.pMineGameObject = this;
		//Desc.ColliderDesc.vExtents = { 0.7f,0.8f,1.f };
		Desc.ColliderDesc.vExtents = { 1.2f,0.4f,1.f };

		Desc.ColliderDesc.vCenter = { 0.9f * m_iLookDirection,0.8f,0.f };
		//Desc.ColliderDesc.pTransform = m_pTransformCom;
		Desc.fhitCharacter_Impus = { 0.3f * m_iLookDirection,0 };
		Desc.fhitCharacter_StunTime = 0.5f;
		Desc.iDamage = 700 * Get_DamageScale();;
		Desc.fLifeTime = 0.2f;
		Desc.ihitCharacter_Motion = { HitMotion::HIT_MEDIUM };
		Desc.iTeam = m_iPlayerTeam;
		Desc.fAnimationLockTime = 0.1f;
		Desc.pOwner = this;

		m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack"), TEXT("Layer_AttackObject"), &Desc);
	}
	break;
	case Client::CPlay_21::ANIME_ATTACK_HEAVY:
	{
		m_bGrab_Air = false;

		CAttackObject::ATTACK_DESC Desc{};
		/*	Desc.ColliderDesc.width = 1.0;
			Desc.ColliderDesc.height = 1.0;
			Desc.ColliderDesc.vCenter = { 1.0f * m_iLookDirection,0.8f,0.f };
			Desc.ColliderDesc.pTransform = m_pTransformCom;*/

		if (m_iPlayerTeam == 1)
			Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_1P_Melee_Attack;
		else
			Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_2P_Melee_Attack;
		Desc.ColliderDesc.pMineGameObject = this;
		Desc.ColliderDesc.vExtents = { 1.f,1.f,1.f };
		Desc.ColliderDesc.vCenter = { 1.0f * m_iLookDirection,0.8f,0.f };

		Desc.fhitCharacter_Impus = { 20.f * m_iLookDirection,0 };
		Desc.fhitCharacter_StunTime = 1.0f;
		Desc.iDamage = 1000 * Get_DamageScale();;
		Desc.fLifeTime = 0.2f;
		Desc.ihitCharacter_Motion = { HitMotion::HIT_KNOCK_AWAY_LEFT };
		Desc.iTeam = m_iPlayerTeam;
		Desc.fAnimationLockTime = 0.7f;
		Desc.bGrabbedEnd = true;
		Desc.pOwner = this;
		m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack"), TEXT("Layer_AttackObject"), &Desc);


		//거리가 멀긴 한데 보기엔 괜찮을지도
		//Character_Make_Effect(TEXT("Ring_Dust_Right"), { 1.5f,0.f });

		Character_Make_Effect(TEXT("Ring_Dust_Right"), { 1.2f,0.f });

	}
	break;
	case Client::CPlay_21::ANIME_ATTACK_SPECIAL:
	{
		CAttackObject_Ranged::ATTACK_RANGED_DESC Desc{};
		//Desc.ColliderDesc.width = 1.0;
		//Desc.ColliderDesc.height = 1.0;
		if (m_iPlayerTeam == 1)
			Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_1P_Ranged_Attack;
		else
			Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_2P_Ranged_Attack;
		Desc.ColliderDesc.pMineGameObject = this;
		Desc.ColliderDesc.vExtents = { 0.2f,0.2f,1.f };

		Desc.ColliderDesc.vCenter = { 0.f, 0.f, 0.f };


		Desc.fhitCharacter_Impus = { 0.3f * m_iLookDirection,0 };
		Desc.fhitCharacter_StunTime = 0.35f;
		Desc.iDamage = 300 * Get_DamageScale();;
		Desc.fLifeTime = 5.f;




		Desc.fhitCharacter_Impus = { 0.2f * m_iLookDirection,0 };


		Desc.iTeam = m_iPlayerTeam;

		Desc.bCameraZoom = false;
		Desc.fAnimationLockTime = 0.f;

		Desc.pOwner = this;

		//Desc.eAttackType = { ATTACKTYPE_HIGH };
		Desc.fStartOffset = { 0.2f * m_iLookDirection, 0.9f };
		//Desc.fRanged_Impus_NoneDirection = { 9.f,0.f };
		Desc.fRanged_Impus_NoneDirection = { 18.f,0.f };

		Desc.iDirection = m_iLookDirection;
		Desc.eRangeColor = CAttackObject_Ranged::RANGED_LIGHT_PINK;


		Desc.strEffectName = TEXT("21_BurstJ-01");

		//Desc.ColliderDesc.vCenter = { 0.2f,-0.2f,0.f };
		//Desc.ihitCharacter_Motion = { HitMotion::HIT_CROUCH_MEDIUM };
		//
		//m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack_Ranged"), TEXT("Layer_AttackObject"), &Desc);
		//
		//
		//Desc.ColliderDesc.vCenter = { -0.3f,0.1f,0.f };
		//Desc.ihitCharacter_Motion = { HitMotion::HIT_LIGHT };
		//m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack_Ranged"), TEXT("Layer_AttackObject"), &Desc);
		//
		//Desc.ColliderDesc.vCenter = { 0.f,0.f,0.f };
		//Desc.ihitCharacter_Motion = { HitMotion::HIT_LIGHT };
		//m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack_Ranged"), TEXT("Layer_AttackObject"), &Desc);



		_float4x4 pBoneMatrix = *m_pModelCom->Get_BoneMatrixPtr("GD_fist_L");


		//Desc.fStartOffset = { 0.2f *m_iLookDirection,1.1f };
		Desc.fStartOffset = { (pBoneMatrix._41 + 0.2f) * m_iLookDirection ,pBoneMatrix._42 - 0.2f };

		Desc.ihitCharacter_Motion = { HitMotion::HIT_CROUCH_MEDIUM };
		m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack_Ranged"), TEXT("Layer_AttackObject"), &Desc);
		Character_Make_Effect(TEXT("21_BurstJ-02"), { pBoneMatrix._41 + 0.2f ,pBoneMatrix._42 - 0.2f });


		//Desc.fStartOffset = { 0.3f * m_iLookDirection,0.8f };
		Desc.fStartOffset = { (pBoneMatrix._41 + 0.6f) * m_iLookDirection ,pBoneMatrix._42 - 0.5f };

		Desc.ihitCharacter_Motion = { HitMotion::HIT_LIGHT };
		m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack_Ranged"), TEXT("Layer_AttackObject"), &Desc);
		Character_Make_Effect(TEXT("21_BurstJ-02"), { pBoneMatrix._41 - 0.4f  ,pBoneMatrix._42 - 0.7f });


		//Desc.fStartOffset = { -0.4f * m_iLookDirection,0.8f };
		//Desc.fStartOffset = { (pBoneMatrix._41 - 0.1f) * m_iLookDirection ,pBoneMatrix._42 - 0.5f };
		Desc.fStartOffset = { (pBoneMatrix._41 - 0.1f) * m_iLookDirection ,pBoneMatrix._42 - 0.7f };

		Desc.ihitCharacter_Motion = { HitMotion::HIT_CROUCH_MEDIUM };
		m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack_Ranged"), TEXT("Layer_AttackObject"), &Desc);
		Character_Make_Effect(TEXT("21_BurstJ-02"), { pBoneMatrix._41 - 0.1f ,pBoneMatrix._42 - 0.5f });


		//Character_Make_Effect(TEXT("21_BurstJ-02"), { pBoneMatrix._41 + 0.1f ,pBoneMatrix._42 });


	}
	break;
	case Client::CPlay_21::ANIME_ATTACK_AIR1:
	{

		CAttackObject::ATTACK_DESC Desc{};
		//Desc.ColliderDesc.width = 0.7;
		//Desc.ColliderDesc.height = 0.8;
		if (m_iPlayerTeam == 1)
			Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_1P_Melee_Attack;
		else
			Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_2P_Melee_Attack;
		Desc.ColliderDesc.pMineGameObject = this;
		Desc.ColliderDesc.vExtents = { 0.7f,0.8f,1.f };
		Desc.ColliderDesc.vCenter = { 0.9f * m_iLookDirection,0.8f,0.f };

		Desc.fhitCharacter_Impus = { m_fImpuse.x,0 };
		//Desc.fhitCharacter_Impus = { m_fImpuse.x * 0.9f,0 };


		Desc.fhitCharacter_StunTime = 0.5f;
		Desc.iDamage = 400 * Get_DamageScale();
		Desc.fLifeTime = 0.1f;
		Desc.ihitCharacter_Motion = { HIT_LIGHT };
		Desc.iTeam = m_iPlayerTeam;
		Desc.fAnimationLockTime = 0.07f;
		//Desc.bOwnerGravityTimeReset = true;
		Desc.pOwner = this;
		Desc.eAttackType = { ATTACKTYPE_MIDDLE };

		m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack"), TEXT("Layer_AttackObject"), &Desc);

		m_pGameInstance->Play_Group_Sound(CSound_Manager::SOUND_GROUP_KEY::LIGHT_ATTACK_21, false, 1.f);
		m_pGameInstance->Play_Group_Sound(CSound_Manager::SOUND_GROUP_KEY::LIGHT_ATTACK_Hit_SFX, false, 1.f);
	}
	break;
	case Client::CPlay_21::ANIME_ATTACK_AIR2:
	{
		//평범한 공격
		if (m_bBenishingAttack == false)
		{

			CAttackObject::ATTACK_DESC Desc{};
			//Desc.ColliderDesc.width = 0.7;
			//Desc.ColliderDesc.height = 0.8;
			if (m_iPlayerTeam == 1)
				Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_1P_Melee_Attack;
			else
				Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_2P_Melee_Attack;
			Desc.ColliderDesc.pMineGameObject = this;
			Desc.ColliderDesc.vExtents = { 0.7f,0.8f,1.f };
			Desc.ColliderDesc.vCenter = { 0.9f * m_iLookDirection,0.8f,0.f };
			//Desc.ColliderDesc.pTransform = m_pTransformCom;
			//Desc.fhitCharacter_Impus = { 0.3f * m_iLookDirection,0 };
			Desc.fhitCharacter_Impus = { m_fImpuse.x * 0.4f ,0 };

			Desc.fhitCharacter_StunTime = 0.6f;
			Desc.iDamage = 700 * Get_DamageScale();
			Desc.fLifeTime = 0.1f;
			Desc.ihitCharacter_Motion = { HitMotion::HIT_LIGHT };
			Desc.iTeam = m_iPlayerTeam;
			Desc.fAnimationLockTime = 0.07f;
			//Desc.bOwnerGravityTimeReset = true;
			Desc.pOwner = this;
			Desc.eAttackType = { ATTACKTYPE_MIDDLE };

			m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack"), TEXT("Layer_AttackObject"), &Desc);

			m_pGameInstance->Play_Group_Sound(CSound_Manager::SOUND_GROUP_KEY::LIGHT_ATTACK_21, false, 1.f);
			m_pGameInstance->Play_Group_Sound(CSound_Manager::SOUND_GROUP_KEY::LIGHT_ATTACK_Hit_SFX, false, 1.f);
		}
		else //배니싱 공격
		{
			m_bInvisible = false;
			m_bBenishingAttack = false;
			m_pModelCom->Get_pCurrentAnimation()->m_fTickPerSecond = 120.f;

			Character_Make_Effect(TEXT("Moving_Line_Right"));

			CAttackObject::ATTACK_DESC Desc{};

			if (m_iPlayerTeam == 1)
				Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_1P_Melee_Attack;
			else
				Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_2P_Melee_Attack;

			Desc.ColliderDesc.pMineGameObject = this;
			Desc.ColliderDesc.vExtents = { 1.f,1.f,1.f };
			Desc.ColliderDesc.vCenter = { 1.0f * m_iLookDirection,0.8f,0.f };

			Desc.fhitCharacter_Impus = { 20.f * m_iLookDirection,0 };
			Desc.fhitCharacter_StunTime = 1.0f;
			Desc.iDamage = 1000 * Get_DamageScale();;
			///Desc.iDamage = 30000 * Get_DamageScale();;
			Desc.fLifeTime = 0.2f;
			Desc.ihitCharacter_Motion = { HitMotion::HIT_KNOCK_AWAY_LEFT };
			Desc.iTeam = m_iPlayerTeam;
			Desc.fAnimationLockTime = 0.3f;
			Desc.bGrabbedEnd = true;
			Desc.pOwner = this;
			Desc.iGainAttackStep = 2;

			Desc.bCameraZoom = false;
			m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack"), TEXT("Layer_AttackObject"), &Desc);

			Character_Create_Distortion({ 1.f,0.f,0.f }, { 0.f,0.f }, { 1.f,1.f }, 0.2f);
		}
	}
	break;
	case Client::CPlay_21::ANIME_ATTACK_AIR3:
	{

		CAttackObject::ATTACK_DESC Desc{};

		if (m_iPlayerTeam == 1)
			Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_1P_Melee_Attack;
		else
			Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_2P_Melee_Attack;
		Desc.ColliderDesc.pMineGameObject = this;
		Desc.ColliderDesc.vCenter = { 0.9f * m_iLookDirection,0.8f,0.f };
		Desc.ColliderDesc.vExtents = { 0.7f,0.8f,0.2f };


		Desc.fhitCharacter_Impus = { 4.f * m_iLookDirection,2.f };
		Desc.fhitCharacter_StunTime = 0.6f;


		Desc.iDamage = 1000 * Get_DamageScale();
		Desc.fLifeTime = 0.1f;
		if (m_bGrab_Air)
		{
			Desc.bGroundSmash = true;
			Desc.fAnimationLockTime = 0.7f;
			m_bGrab_Air = false;
			Desc.ihitCharacter_Motion = { HitMotion::HIT_KNOCK_AWAY_LEFTDOWN };
			Desc.fhitCharacter_Impus = { 3.f * m_iLookDirection,-20.f };
		}
		else if (Get_fHeight() > 0)
		{
			Desc.ihitCharacter_Motion = { HitMotion::HIT_SPIN_AWAY_LEFTUP };
			//Desc.bCameraZoom = false;
		}
		else
			Desc.ihitCharacter_Motion = { HitMotion::HIT_MEDIUM };

		Desc.iTeam = m_iPlayerTeam;
		Desc.fAnimationLockTime = 0.7f;
		Desc.pOwner = this;
		Desc.bGrabbedEnd = true;

		m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack"), TEXT("Layer_AttackObject"), &Desc);

		m_pGameInstance->Play_Sound(CSound_Manager::SOUND_KEY_NAME::Heavy_Attack_21, false, 1.f);
	}
	break;
	case Client::CPlay_21::ANIME_ATTACK_CROUCH_LIGHT:
	{

		CAttackObject::ATTACK_DESC Desc{};

		if (m_iPlayerTeam == 1)
			Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_1P_Melee_Attack;
		else
			Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_2P_Melee_Attack;
		Desc.ColliderDesc.pMineGameObject = this;
		Desc.ColliderDesc.vCenter = { 0.9f * m_iLookDirection,0.8f,0.f };
		Desc.ColliderDesc.vExtents = { 0.3f,0.5f,0.2f };



		Desc.fhitCharacter_Impus = { 0.3f * m_iLookDirection,0 };
		Desc.fhitCharacter_StunTime = 0.3f;
		Desc.iDamage = 400 * Get_DamageScale();
		Desc.fLifeTime = 0.1f;
		Desc.ihitCharacter_Motion = { HitMotion::HIT_LIGHT };
		Desc.iTeam = m_iPlayerTeam;
		Desc.fAnimationLockTime = 0.1f;
		Desc.pOwner = this;

		m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack"), TEXT("Layer_AttackObject"), &Desc);

		m_pGameInstance->Play_Group_Sound(CSound_Manager::SOUND_GROUP_KEY::LIGHT_ATTACK_21, false, 1.f);
		m_pGameInstance->Play_Group_Sound(CSound_Manager::SOUND_GROUP_KEY::LIGHT_ATTACK_Hit_SFX, false, 1.f);
	}

	break;
	case Client::CPlay_21::ANIME_ATTACK_CROUCH_MEDUIM:
	{

		CAttackObject::ATTACK_DESC Desc{};
		//Desc.ColliderDesc.width = 0.7;
		//Desc.ColliderDesc.height = 0.8;
		if (m_iPlayerTeam == 1)
			Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_1P_Melee_Attack;
		else
			Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_2P_Melee_Attack;
		Desc.ColliderDesc.pMineGameObject = this;

		//오공처럼 쓰던것
		//Desc.ColliderDesc.vCenter = { 0.7f * m_iLookDirection,0.3f,0.f };
		Desc.ColliderDesc.vCenter = { 2.5f * m_iLookDirection,0.3f,0.f };


		Desc.ColliderDesc.vExtents = { 0.7f,0.5f,1.f };
		Desc.fhitCharacter_Impus = { -2.5f * m_iLookDirection,0.3f };

		//Desc.ColliderDesc.pTransform = m_pTransformCom;
		//Desc.fhitCharacter_Impus = { 1.f * m_iLookDirection,0.3f };
		//Desc.fhitCharacter_Impus = { 1.5f * m_iLookDirection,0.3f };
		//Desc.ColliderDesc.vExtents = { 0.7f,0.8f,1.f };


		Desc.fhitCharacter_StunTime = 1.f;
		Desc.iDamage = 700 * Get_DamageScale();
		Desc.fLifeTime = 0.1f;
		Desc.ihitCharacter_Motion = { HitMotion::HIT_CROUCH_MEDIUM };
		Desc.iTeam = m_iPlayerTeam;
		Desc.fAnimationLockTime = 0.1f;
		//Desc.bOwnerGravityTimeReset = true;
		Desc.pOwner = this;

		Desc.eAttackType = { ATTACKTYPE_LOW };

		m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack"), TEXT("Layer_AttackObject"), &Desc);


		Character_Make_Effect(TEXT("Smoke03_Stop"), { 4.f,0.f }, true);

		//Character_Make_Effect(TEXT("Ring_Dust"), { 2.2f,0.1f },true);

		Character_Make_Effect(TEXT("Smoke02_Small"), { 2.7f,-0.3f }, true);
		//Character_Make_Effect(TEXT("Smoke02"), { 2.9f,-0.3f }, true);

		//Character_Make_Effect(TEXT("Smoke01"), { 2.9f,-0.3f });

	}
	break;

	case Client::CPlay_21::ANIME_ATTACK_CROUCH_HEAVY:
	{
		CAttackObject::ATTACK_DESC Desc{};
		//Desc.ColliderDesc.width = 2.0;
		//Desc.ColliderDesc.height = 1.3;
		//Desc.ColliderDesc.vCenter = { 0.4f * m_iLookDirection,1.2f,0.f };
		//Desc.ColliderDesc.width = 1.5;
		//Desc.ColliderDesc.height = 1.3;
		if (m_iPlayerTeam == 1)
			Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_1P_Melee_Attack;
		else
			Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_2P_Melee_Attack;
		Desc.ColliderDesc.pMineGameObject = this;
		Desc.ColliderDesc.vExtents = { 1.5f,1.3f,1.f };
		Desc.ColliderDesc.vCenter = { 0.2f * m_iLookDirection,1.2f,0.f };


		//Desc.ColliderDesc.pTransform = m_pTransformCom;
		Desc.fhitCharacter_Impus = { 0.3f * m_iLookDirection, 10.f };  //원래 가속도.
		//Desc.fhitCharacter_Impus = { 0.2f * m_iLookDirection, 3.f };  //공중 테스트용 임시

		Desc.fhitCharacter_StunTime = 1.0f;
		Desc.iDamage = 1000 * Get_DamageScale();;
		Desc.fLifeTime = 0.3f;
		Desc.ihitCharacter_Motion = { HitMotion::HIT_KNOCK_AWAY_UP };
		Desc.iTeam = m_iPlayerTeam;
		Desc.fAnimationLockTime = 0.5f;
		Desc.pOwner = this;


		if (Get_fHeight() > 2)
		{
			Desc.bCameraZoom = false;
			Desc.fhitCharacter_Impus = { 0.2f * m_iLookDirection, 5.f };
			Desc.fAnimationLockTime = 0.1f;
			Desc.fLifeTime = 0.1f;
		}

		m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack"), TEXT("Layer_AttackObject"), &Desc);


		Character_Make_Effect(TEXT("Ring_Dust"), { 0.4f,0.9f });

		m_pGameInstance->Play_Sound(CSound_Manager::SOUND_KEY_NAME::Heavy_Attack_21, false, 1.f);
	}
	//공중 어퍼랑 같은 모션임


	break;
	break;
	case Client::CPlay_21::ANIME_ATTACK_CROUCH_SPECIAL:
	{

		if (iAttackEvent == 0)
		{

			m_pGameInstance->Play_Sound(CSound_Manager::SOUND_KEY_NAME::J_Attack_21, false, 1.f);
			m_pGameInstance->Play_Sound(CSound_Manager::SOUND_KEY_NAME::J_Attack_Voice_21, false, 1.f);
			//공중 콤보가능하도록 속도 더 빠르게+중력은 강하게
			if (Get_fHeight() > 3)
			{
				m_pModelCom->m_Animations[m_pModelCom->m_iCurrentAnimationIndex]->m_fTickPerSecond = 131.f;
				Set_ForcedGravityDown();

			}
			else
			{
				m_pModelCom->m_Animations[m_pModelCom->m_iCurrentAnimationIndex]->m_fTickPerSecond = 60.1f;
				Set_ForcedGravityTime_LittleUp();
			}
		}

		else if (iAttackEvent == 1)
		{

			CAttackObject_Ranged::ATTACK_RANGED_DESC Desc{};

			if (m_iPlayerTeam == 1)
				Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_1P_Ranged_Attack;
			else
				Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_2P_Ranged_Attack;
			Desc.ColliderDesc.pMineGameObject = this;
			//Desc.ColliderDesc.vExtents = { 0.5f,0.5f,1.f };
			Desc.ColliderDesc.vExtents = { 0.2f,0.2f,1.f };

			Desc.ColliderDesc.vCenter = { 0.f,0.f,0.f };

			Desc.fhitCharacter_Impus = { 3.f * m_iLookDirection,2.f };
			Desc.fhitCharacter_StunTime = 0.4f;

			if (Get_fHeight() > 3)
				Desc.fhitCharacter_StunTime = 0.8f;

			Desc.iDamage = 250 * Get_DamageScale();;
			Desc.fLifeTime = 5.f;
			Desc.ihitCharacter_Motion = { HitMotion::HIT_LIGHT };

			Desc.iTeam = m_iPlayerTeam;

			Desc.bCameraZoom = false;
			Desc.fAnimationLockTime = 0.f;

			Desc.pOwner = this;

			//Desc.eAttackType = { ATTACKTYPE_HIGH };
			//Desc.fStartOffset = { 0.6f * m_iLookDirection, 0.f };
			Desc.fStartOffset = { 0.6f * m_iLookDirection, 0.4f };

			Desc.iDirection = m_iLookDirection;
			Desc.eRangeColor = CAttackObject_Ranged::RANGED_LIGHT_PINK;



			Desc.iGainAttackStep = 0;



			//Desc.fRanged_Impus_NoneDirection = { fMultiple ,-0.07f * fMultiple };



			Desc.strEffectName = TEXT("21_BurstJ-01");

			//Desc.fRanged_Impus_NoneDirection = { 7.8f, -0.6f };
			Desc.fRanged_Impus_NoneDirection = { 15.6f, -1.2f };

			Desc.fEffectRotationDegree = 4.35f;
			m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack_Ranged"), TEXT("Layer_AttackObject"), &Desc);


			//Desc.fRanged_Impus_NoneDirection = { 6.72f, -2.808f };
			Desc.fRanged_Impus_NoneDirection = { 13.44f, -5.616f };

			Desc.fEffectRotationDegree = 337.62;
			m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack_Ranged"), TEXT("Layer_AttackObject"), &Desc);


			//Desc.fRanged_Impus_NoneDirection = { 5.196f, -4.752f };
			Desc.fRanged_Impus_NoneDirection = { 10.392f, -9.504f };

			Desc.fEffectRotationDegree = 318.37;
			m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack_Ranged"), TEXT("Layer_AttackObject"), &Desc);

		}

		else if (iAttackEvent == 3)
		{
			Set_fImpulse({ -1.f * m_iLookDirection,0.f });
			Set_ForcedGravityTime_LittleUp();
			//Set_fGravityTime()
		}

	}
	break;
	case Client::CPlay_21::ANIME_ATTACK_236:
	{

		//시작. Position 5
		if (iAttackEvent == 0)
		{
			CEffect_Layer::COPY_DESC tDesc{};

			//tDesc.pPlayertMatrix = m_pModelCom->Get_BoneMatrixPtr("GD_fist_L");

			//_float4x4 BoneMatrix = *m_pModelCom->Get_BoneMatrixPtr("GD_fist_L");
			tDesc.pPlayertMatrix = m_pModelCom->Get_BoneMatrixPtr("GD_fist_L");
			tDesc.pTransformCom = m_pTransformCom;
			m_pAttack236ChargeEffect_Layer = CEffect_Manager::Get_Instance()->Copy_Layer_AndGet(TEXT("21_SDU-01"), &tDesc);


		}

		else if (iAttackEvent == 1) //공격
		{
			if (m_pAttack236ChargeEffect_Layer != nullptr)
			{
				for (auto& iter : m_pAttack236ChargeEffect_Layer->m_MixtureEffects)
					iter->m_bIsSpriteEnd = true;

				m_pAttack236ChargeEffect_Layer->m_bIsDoneAnim = true;
				m_pAttack236ChargeEffect_Layer = nullptr;
			}

			CAttackObject_Ranged::ATTACK_RANGED_DESC Desc{};
			//Desc.ColliderDesc.width = 1.0;
			//Desc.ColliderDesc.height = 1.0;
			if (m_iPlayerTeam == 1)
				Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_1P_Ranged_Attack;
			else
				Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_2P_Ranged_Attack;
			Desc.ColliderDesc.pMineGameObject = this;
			Desc.ColliderDesc.vExtents = { 0.4f,0.4f,1.f };
			Desc.ColliderDesc.vCenter = { 0.f,0.f,0.f };


			Desc.fhitCharacter_Impus = { 0.3f * m_iLookDirection,0 };
			Desc.fhitCharacter_StunTime = 0.35f;
			Desc.iDamage = 900 * Get_DamageScale();;
			Desc.fLifeTime = 5.f;


			Desc.ihitCharacter_Motion = { HitMotion::HIT_SPIN_AWAY_LEFTUP };
			Desc.fhitCharacter_Impus = { 3.f * m_iLookDirection,1.f };


			Desc.iTeam = m_iPlayerTeam;

			Desc.bCameraZoom = false;
			Desc.fAnimationLockTime = 0.f;

			Desc.pOwner = this;

			//Desc.eAttackType = { ATTACKTYPE_HIGH };
			Desc.fStartOffset = { 0.2f * m_iLookDirection, 0.9f };
			Desc.fRanged_Impus_NoneDirection = { 15.f,0.f };
			Desc.iDirection = m_iLookDirection;
			Desc.eRangeColor = CAttackObject_Ranged::RANGED_LIGHT_PINK;
			Desc.strEffectName = TEXT("21_SDU-01");

			m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack_Ranged"), TEXT("Layer_AttackObject"), &Desc);

			CEffect_Layer::COPY_DESC tDesc{};
			tDesc.pPlayertMatrix = m_pModelCom->Get_BoneMatrixPtr("GD_fist_L");
			tDesc.pTransformCom = m_pTransformCom;
			CEffect_Manager::Get_Instance()->Copy_Layer(TEXT("21_SDU-02"), &tDesc);

			//Character_Make_Effect(TEXT("21_SDU-02"));

			m_pGameInstance->Play_Sound(CSound_Manager::SOUND_KEY_NAME::Down_Forward_Light_21, false, 1.f);
			m_pGameInstance->Play_Sound(CSound_Manager::SOUND_KEY_NAME::J_Attack_21, false, 1.f);

		}

	}
	break;
	case Client::CPlay_21::ANIME_ATTACK_214:
	{


		//내려찍는중
		if (iAttackEvent == 0)
		{
			//높이가 아주 높고, 특정모션이면 더 앞으로 가도록
			//if ((m_pModelCom->m_iCurrentAnimationIndex == CPlay_21::ANIME_ATTACK_CROUCH_SPECIAL))
			{
				Set_fImpulse({ m_iLookDirection * 9.f, -15.f });
				//Set_fImpulse({ m_iLookDirection * 9.f, -30.f });

			}


			if (Get_fHeight() < 1)
				return;



			CAttackObject::ATTACK_DESC Desc{};

			if (m_iPlayerTeam == 1)
				Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_1P_Melee_Attack;
			else
				Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_2P_Melee_Attack;
			Desc.ColliderDesc.pMineGameObject = this;
			Desc.ColliderDesc.vCenter = { 0.3f * m_iLookDirection,0.8f,0.f };
			Desc.ColliderDesc.vExtents = { 0.8f,0.7f,0.2f };


			//캐릭터는 Look*2, -15 인데  오히려 좀 끌려오는 느낌으로?
			//Desc.fhitCharacter_Impus = { m_iLookDirection * 2.f, -15.f };

			//Desc.fhitCharacter_Impus = { m_iLookDirection * -1.f, -15.f };
			Desc.fhitCharacter_Impus = { m_fImpuse };


			Desc.fhitCharacter_StunTime = 50.f;	//일단잡기마냥 땅에 닿아야 풀리는 느낌 + 추가타로 풀어버리는 느낌


			Desc.iDamage = 100 * Get_DamageScale();
			//Desc.fLifeTime = 0.3f;  //어쩌지 현재높이로부 -13/s로 움직였을때 땅에 닿을때까지의 시간 
			Desc.fLifeTime = Get_fHeight() / 13.f;


			Desc.ihitCharacter_Motion = { HitMotion::HIT_KNOCK_AWAY_LEFTDOWN };
			Desc.iTeam = m_iPlayerTeam;
			Desc.fAnimationLockTime = 0.1f;
			Desc.pOwner = this;
			Desc.bGroundSmash = true;
			Desc.bCameraZoom = false;
			Desc.iGainAttackStep = 0;

			m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack"), TEXT("Layer_AttackObject"), &Desc);


			Character_Make_Effect(TEXT("21_SAU-03"));
			m_pAttack214AssultEffect_Layer = Character_Make_BoneEffect("G_head", TEXT("21_SAU-02"));

		}

		//땅에 닿아서 충격파  Position55.1
		else if (iAttackEvent == 1)
		{
			CAttackObject::ATTACK_DESC Desc{};

			if (m_iPlayerTeam == 1)
				Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_1P_Melee_Attack;
			else
				Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_2P_Melee_Attack;
			Desc.ColliderDesc.pMineGameObject = this;
			Desc.ColliderDesc.vCenter = { 0.5f * m_iLookDirection,0.8f,0.f };

			if (m_bAttackBackEvent == true)
				Desc.ColliderDesc.vExtents = { 1.0f,0.5f,0.2f };
			else
				Desc.ColliderDesc.vExtents = { 0.6f,0.5f,0.2f };

			Desc.fhitCharacter_Impus = { m_iLookDirection * 1.5f, 0.2f };
			//Desc.fhitCharacter_StunTime = 0.1f;	
			//Desc.fhitCharacter_StunTime = 0.35f;   // 문제없지만 어차피 콤보비례뎀감도 있으니 스턴 더줘도 될지도
			//Desc.fhitCharacter_StunTime = 0.45f;   // 땅에 닿으면 덜덜거림, 이젠 아님. 왜? 스턴시간감소때문에?
			Desc.fhitCharacter_StunTime = 0.47f;     //1콤보 기준으로는 연계됨.  4는되나?  10은?




			Desc.iDamage = 800 * Get_DamageScale();
			Desc.fLifeTime = 0.3f;
			Desc.ihitCharacter_Motion = { HitMotion::HIT_SPIN_AWAY_LEFTUP };
			Desc.iTeam = m_iPlayerTeam;
			Desc.fAnimationLockTime = 0.7f;
			Desc.pOwner = this;
			Desc.eAttackGrade = GRADE_ULTIMATE;
			Desc.bGroundSmash = false;
			//Desc.iGainAttackStep = 2;  

			m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack"), TEXT("Layer_AttackObject"), &Desc);

			//Character_Make_Effect(TEXT("21_SAU-01"));
			m_pAttack214GroundEffect_Layer = nullptr;
			m_pAttack214GroundEffect_Layer = Character_Make_Effect(TEXT("21_SAU-01"));


			if (m_pAttack214AssultEffect_Layer != nullptr)
			{
				m_pAttack214AssultEffect_Layer->m_bIsDoneAnim = true;
				m_pAttack214AssultEffect_Layer = nullptr;
			}


			Character_Make_Effect(TEXT("Hit_SAO-01"));


		}

		//57에 NextAnimation,   69에 Event2
		else if (iAttackEvent == 2)
		{
			if (m_bAttackBackEvent == false)
			{
				if (m_pAttack214GroundEffect_Layer != nullptr)
				{
					m_pAttack214GroundEffect_Layer->m_bIsDoneAnim = true;
					m_pAttack214GroundEffect_Layer = nullptr;
				}
			}

		}
	}
	break;
	case Client::CPlay_21::ANIME_ATTACK_236_SPECIAL:
	{

		CAttackObject_CommandGrab::ATTACK_COMMANDGRAB_DESC Desc{};
		if (m_iPlayerTeam == 1)
			Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_1P_Melee_Attack;
		else
			Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_2P_Melee_Attack;
		Desc.ColliderDesc.pMineGameObject = this;
		Desc.ColliderDesc.vExtents = { 1.4f,2.2f,1.f };
		Desc.ColliderDesc.vCenter = { 0.3f,0.7f,0.f };
		//Desc.ColliderDesc.pTransform = m_pTransformCom;
		//Desc.fhitCharacter_Impus = { 0.3f * m_iLookDirection,0 };
		Desc.fhitCharacter_StunTime = 1.f;
		Desc.iDamage = 100 * Get_DamageScale();;
		Desc.fLifeTime = 0.2f;
		Desc.ihitCharacter_Motion = { HitMotion::HIT_WALLBOUNCE };
		Desc.iTeam = m_iPlayerTeam;
		Desc.fAnimationLockTime = 0.1f;
		Desc.pOwner = this;

		Desc.eAttackType = ATTACKTYPE_COMMANDGRAB;

		Desc.fDistance = { 0.8f * m_iLookDirection,0.5f };
		//Desc.fGrabAnimationPosition = 40.f;
		//Desc.fGrabAnimationPosition = 25.f;
		Desc.iGainAttackStep = 0;
		Desc.bOwnerNextAnimation = true;

		Desc.iGrabAnimationIndex = ANIME_ATTACK_236_SPECIAL;
		Desc.iOnwerNextAnimationIndex = ANIME_ATTACK_236_SPECIAL_SUCCES;


		//이동하기 전에 이펙트 생기던 시절.
		//Desc.strHitEffectName = TEXT("21_SDJ-01");
		//Desc.fHitEffectOffset = { 0.7f,1.9f };
		//Desc.bHitEffectFlip = true;

		Desc.strHitEffectName = TEXT("21_SDJ-01");
		Desc.fHitEffectOffset = { 0.3f,1.3f };
		Desc.bHitEffectFlip = true;

		Desc.iVirtualCameraindex = CMain_Camera::VIRTUAL_CAMERA_21_GRAB_SPECIAL;
		m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack_CommandGrab"), TEXT("Layer_AttackObject"), &Desc);

		//236 잡기 컷신
		//static_cast<CMain_Camera*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Main_Camera")))->Play(CMain_Camera::VIRTUAL_CAMERA_21_GRAB_SPECIAL, 0, this);


	}
	break;
	case Client::CPlay_21::ANIME_ATTACK_236_SPECIAL_SUCCES:
	{
		if (iAttackEvent == 3)
		{
			CEffect_Layer* pEffect = Character_Make_BoneEffect("GD_hand_R", TEXT("21_SDJ-02"));

			pEffect->Set_Copy_Layer_Rotation({ 0.f,0.f,-70.f });
		}

		else if (iAttackEvent == 0)
		{
			CAttackObject::ATTACK_DESC Desc{};
			//Desc.ColliderDesc.width = 1.0;
			//Desc.ColliderDesc.height = 1.3;
			if (m_iPlayerTeam == 1)
				Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_1P_Melee_Attack;
			else
				Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_2P_Melee_Attack;
			Desc.ColliderDesc.pMineGameObject = this;
			Desc.ColliderDesc.vExtents = { 1.f,1.3f,1.f };
			Desc.ColliderDesc.vCenter = { 1.0f * m_iLookDirection,0.8f,0.f };
			//Desc.ColliderDesc.pTransform = m_pTransformCom;
			Desc.fhitCharacter_Impus = { 3.f * m_iLookDirection,-20.f };
			Desc.fhitCharacter_StunTime = 1.0f;
			Desc.iDamage = 1200 * Get_DamageScale();;
			Desc.fLifeTime = 0.2f;
			Desc.ihitCharacter_Motion = { HitMotion::HIT_WALLBOUNCE };
			Desc.iTeam = m_iPlayerTeam;
			//Desc.bGroundSmash = true;
			Desc.fAnimationLockTime = 0.5f;
			Desc.pOwner = this;
			Desc.eAttackType = { ATTACKTYPE_MIDDLE };
			//Desc.bCameraZoom = false;
			//Desc.bGrabbedEnd = false;
			//Desc.bCameraZoom = false;

			m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack"), TEXT("Layer_AttackObject"), &Desc);



		}
		if (iAttackEvent == 1)
		{
			CAttackObject::ATTACK_DESC Desc{};
			//Desc.ColliderDesc.width = 1.0;
			//Desc.ColliderDesc.height = 1.3;
			if (m_iPlayerTeam == 1)
				Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_1P_Melee_Attack;
			else
				Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_2P_Melee_Attack;
			Desc.ColliderDesc.pMineGameObject = this;
			Desc.ColliderDesc.vExtents = { 1.f,1.3f,1.f };
			Desc.ColliderDesc.vCenter = { 1.0f * m_iLookDirection,0.8f,0.f };
			//Desc.ColliderDesc.pTransform = m_pTransformCom;
			Desc.fhitCharacter_Impus = { 3.f * m_iLookDirection,-20.f };
			Desc.fhitCharacter_StunTime = 1.0f;
			Desc.iDamage = 100 * Get_DamageScale();;
			Desc.fLifeTime = 0.2f;
			Desc.ihitCharacter_Motion = { HitMotion::HIT_HEAVY_DOWN };
			Desc.iTeam = m_iPlayerTeam;
			Desc.bGroundSmash = true;
			Desc.fAnimationLockTime = 0.f;
			Desc.pOwner = this;
			Desc.eAttackType = { ATTACKTYPE_MIDDLE };

			Desc.bGrabbedEnd = true;
			//Desc.bCameraZoom = false;
			Desc.iGainAttackStep = 0;

			m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack"), TEXT("Layer_AttackObject"), &Desc);

			//Character_Make_BoneEffect("GD_hand_R", TEXT("21_SDJ-03"));

			Character_Make_Effect(TEXT("21_SDJ-03"), { 0.f,1.1f });

		}


		break;
	case Client::CPlay_21::ANIME_ATTACK_236_ULTIMATE:
	{
		if (iAttackEvent == 0) //시작, 시간정지연출
		{

			Character_Make_Effect(TEXT("21_SDO-01"));

			//CEffect_Layer::COPY_DESC tDesc{};
			//tDesc.pPlayertMatrix = m_pTransformCom->Get_WorldMatrixPtr();
			//m_pEffect_Manager->Copy_Layer(TEXT("21_SDO-01"), &tDesc);


			_float fDebug = m_pModelCom->m_fCurrentAnimPosition;


			_float fStopTime = 2.f;

			Set_AnimationStopWithoutMe(fStopTime);
			//Set_AnimationStop(fStopTime);
			Set_StopAllAttackObject(fStopTime);


			CMain_Camera* mainCamera = static_cast<CMain_Camera*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Main_Camera")));
			mainCamera->Play(CMain_Camera::VIRTUAL_CAMERA_21_ENERGY, 0, this);
			mainCamera->StartCameraShake(1.5f, 0.2f);
			//이펙트들

			if (m_bAura == false)
				Set_bAura(true);

			m_pRenderInstance->Get_Instance()->Switch_BlackOut(true);


		}
		else if (iAttackEvent == 1) // 손가락에 차지중. 다시보니 이펙트 말고는 넣을 필요 없을듯
		{
			//_float fStopTime = 1.f;
			//
			//Set_AnimationStopWithoutMe(fStopTime + 0.3f);
			//Set_AnimationStop(fStopTime);
			//Set_StopAllAttackObject(fStopTime);
			Character_Make_BoneEffect("GD_fng_b3_L", TEXT("21_SDO-02"));

		}
		else if (iAttackEvent == 2) //손뻗음. 레이저?
		{
			//Set_AnimationStopWithoutMe(0.f);


		}
		else if (iAttackEvent == 3)
		{
			CAttackObject::ATTACK_DESC Desc{};

			if (m_iPlayerTeam == 1)
				Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_1P_Melee_Attack;
			else
				Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_2P_Melee_Attack;
			Desc.ColliderDesc.pMineGameObject = this;
			Desc.ColliderDesc.vCenter = { 1.f * m_iLookDirection,-4.f,0.f };
			Desc.ColliderDesc.vExtents = { 20.f,6.f,0.2f };


			Desc.fhitCharacter_Impus = { 4.f * m_iLookDirection,2.f };
			Desc.fhitCharacter_StunTime = 1.2f;


			Desc.iDamage = 2200 * Get_DamageScale();
			Desc.fLifeTime = 0.1f;

			Desc.ihitCharacter_Motion = { HitMotion::HIT_SPIN_AWAY_LEFTUP };

			Desc.iGainHitCount = 20;
			Desc.iTeam = m_iPlayerTeam;
			Desc.fAnimationLockTime = 0.0f;
			Desc.pOwner = this;

			Desc.bCameraZoom = false;


			m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack"), TEXT("Layer_AttackObject"), &Desc);

			CMain_Camera* mainCamera = static_cast<CMain_Camera*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Main_Camera")));
			mainCamera->StartCameraShake(0.9f, 0.2f);

			//임시 이펙트


			//for (int i = 0; i < 10; i++)
			//{
			//
			//	Character_Make_Effect(TEXT("BurstJ3-Hit01"), { i*1.f + rand()%5 *0.1f,-2.f});
			//
			//}

		}
		else if (iAttackEvent == 4)
		{
			Character_Make_Effect(TEXT("21_SDO-03"), { 3.f ,-2.f });

		}
		else if (iAttackEvent == 5)
		{
			Character_Make_Effect(TEXT("21_SDO-03"), { 5.f ,-2.f });

		}
		else if (iAttackEvent == 6)
		{
			Character_Make_Effect(TEXT("21_SDO-03"), { 7.f ,-2.f });


			Set_bAura(false);
			m_pRenderInstance->Get_Instance()->Switch_BlackOut(false);

		}

	}
	break;
	case Client::CPlay_21::ANIME_REFLECT:
	{
		if (iAttackEvent == 0)
		{
			CAttackObject::ATTACK_DESC Desc{};

			if (m_iPlayerTeam == 1)
				Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_1P_REFLECT;
			else
				Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_2P_REFLECT;
			Desc.ColliderDesc.pMineGameObject = this;

			Desc.ColliderDesc.vCenter = { 0.f,0.8f,0.f };
			Desc.ColliderDesc.vExtents = { 1.f,1.f,0.2f };


			Desc.fhitCharacter_Impus = { 0,0 };
			Desc.fhitCharacter_StunTime = 0.f;
			Desc.iDamage = 0;
			Desc.fLifeTime = 1.f;
			Desc.ihitCharacter_Motion = { HitMotion::HIT_NONE };
			Desc.iTeam = m_iPlayerTeam;
			Desc.fAnimationLockTime = 0.f;
			Desc.pOwner = this;
			Desc.iGainHitCount = 0;
			Desc.iGainAttackStep = 0;

			Desc.bReflect = true;

			//m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack_Reflect"), TEXT("Layer_AttackObject"), &Desc);
			m_pReflectObject = m_pGameInstance->Add_GameObject_ToLayer_AndGet(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack_Reflect"), TEXT("Layer_AttackObject"), &Desc);


			m_bReflect = true;
		}
		if (iAttackEvent == 1)
		{
			if (m_bAttackBackEvent)
			{
				Set_Animation(CPlay_21::ANIME_IDLE);

				//if (m_pReflectObject != nullptr)
				//{
				//	//static_cast<CAttackObject_Reflect*>(m_pReflectObject)->Set_RemoteDestory();
				//	static_cast<CAttackObject_Reflect*>(m_pReflectObject)->Destory();
				//
				//	m_pReflectObject = nullptr;
				//}
			}

			if (m_pReflectObject != nullptr)
			{
				if (m_pReflectObject->m_iGameObjectData < 0)
				{
					m_pReflectObject = nullptr;
				}
				else
					static_cast<CAttackObject_Reflect*>(m_pReflectObject)->Set_RemoteDestory();

				m_pReflectObject = nullptr;
			}

			m_bReflect = false;
		}
	}
	break;

	case Client::CPlay_21::ANIME_FINAL_START:
	{
		if (iAttackEvent == 0)  // Position 0
		{
			//Character_Make_Effect(TEXT("21_WSDO-01"));
			//Character_Make_BoneEffect("GD_waist_scl", TEXT("21_WSDO-01"));
			Character_Make_BoneEffect("G_root", TEXT("21_WSDO-01"));


			m_bFinalSoundEnable = true;

			CMain_Camera* mainCamera = static_cast<CMain_Camera*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Main_Camera")));
			mainCamera->Play(CMain_Camera::VIRTUAL_CAMERA_21_ULTIMATE, 0, this);

			m_iFinalLoofCount = 10;
			m_iQTE = -1;
			m_bFinalSkillAdd = false;
			if (m_pEnemy->Get_bStun() == true || m_pEnemy->Check_bCurAnimationisGroundSmash())
			{
				Character_Start_QTE(2);

			}

			_float fStopTime = 1.2f;

			Set_fImpulse({ 0,0 });

			//상대가 Stun 상태이고 위치 차이가 크게 나면  Y위치 추적.  X는 기본적으로 추적하지 않으나 Y가 차이날때만? 일단 X추적 뺌.
			if (m_pEnemy->Get_bStun() == true)
			{
				_float fEnemyHegiht = m_pEnemy->Get_fHeight();
				_float fMyHeight = Get_fHeight();

				if (abs(fEnemyHegiht - fMyHeight) > 0.4f)
				{
					_vector vPos = Get_vPosition();
					vPos = XMVectorSetY(vPos, fEnemyHegiht);


					m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);

					fStopTime = 1.5f;
				}
			}

			Set_AnimationStopWithoutMe(fStopTime);

			m_bAttackBackEvent = false;


			Set_bAura(true);

			m_pRenderInstance->Get_Instance()->Switch_BlackOut(true);

		}
		else if (iAttackEvent == 1) //53에서 발생. 루프가 50이라 잘못하면 닿기도 전에 방향전환을 한다?
		{


			////속도 200버전.
			//if (m_bAttackBackEvent)
			//{
			//	FlipDirection();
			//	Teleport_ToEnemy(-10.f, -2.f);
			//	Set_fImpulse({ m_iLookDirection * 60.f, 7.f });
			//
			//}
			//else
			//	Set_fImpulse({ m_iLookDirection * 60.f, 0.f });

			//Character_Make_Effect(TEXT("21_WSDO-02"));
			//Character_Make_Effect(TEXT("21_WSDO-03"));

			Character_Make_BoneEffect("G_waist", TEXT("21_WSDO-03"));



			if (m_bAttackBackEvent)
			{
				FlipDirection();
				Teleport_ToEnemy(-10.f, -2.f);
				//Set_fImpulse({ m_iLookDirection * 80.f, 10.f });

				//Set_fImpulse({ m_iLookDirection * 80.f, 13.f });
				Set_fImpulse({ m_iLookDirection * 80.f, 15.f });



				CEffect_Layer::COPY_DESC tDesc{};

				tDesc.pPlayertMatrix = m_pModelCom->Get_BoneMatrixPtr("G_head");
				tDesc.pTransformCom = m_pTransformCom;
				tDesc.m_isPlayerDirRight = m_iLookDirection;

				if (m_pAttackFinalChaseEffect_Layer != nullptr)
				{
					m_pAttackFinalChaseEffect_Layer->m_bIsDoneAnim = true;
					m_pAttackFinalChaseEffect_Layer = nullptr;
				}
				m_pAttackFinalChaseEffect_Layer = CEffect_Manager::Get_Instance()->Copy_Layer_AndGet(m_ChaseEffectName, &tDesc);


			}
			else
				Set_fImpulse({ m_iLookDirection * 60.f, 0.f });

			CAttackObject_CommandGrab::ATTACK_COMMANDGRAB_DESC Desc{};
			if (m_iPlayerTeam == 1)
				Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_1P_Melee_Attack;
			else
				Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_2P_Melee_Attack;
			Desc.ColliderDesc.pMineGameObject = this;

			//Desc.ColliderDesc.vExtents = { 0.8f,0.6f,1.f };
			//Desc.ColliderDesc.vCenter = { 0.3f,0.7f,0.f };

			Desc.ColliderDesc.vExtents = { 0.3f,0.6f,1.f };
			Desc.ColliderDesc.vCenter = { 0.9f * m_iLookDirection,0.7f,0.f };


			//Desc.ColliderDesc.pTransform = m_pTransformCom;
			Desc.fhitCharacter_Impus = { 0.f,2.f };
			Desc.fhitCharacter_StunTime = 2000.f;
			Desc.fLifeTime = 0.6f;
			Desc.ihitCharacter_Motion = { HitMotion::HIT_SPIN_AWAY_UP };
			Desc.iTeam = m_iPlayerTeam;

			if (m_bAttackBackEvent == false)  //첫공격만 0.2초 시간정지
				Desc.fAnimationLockTime = 0.2f;

			else if (m_bAttackBackEvent == true)
				Desc.fAnimationLockTime = 0.f;

			Desc.pOwner = this;

			Desc.eAttackType = ATTACKTYPE_HIGH;
			Desc.eAttackGrade = GRADE_ULTIMATE;

			//Desc.fDistance = { 0.8f * m_iLookDirection,0.2f };
			Desc.fDistance = { 0.f,0.2f };

			Desc.fForcedGravityTime = 0.f;
			//Desc.fGrabAnimationPosition = 40.f;
			//Desc.fGrabAnimationPosition = 25.f;

			Desc.bGrabedGravity = true;
			Desc.fForcedGravityTime = 0.f;

			if (m_bAttackBackEvent == false) //첫공격
			{
				Desc.iGainAttackStep = 1;
				Desc.iDamage = 700 * Get_DamageScale(true);
				//Desc.ihitCharacter_Motion = { HitMotion::HIT_KNOCK_AWAY_LEFT };
				//Desc.iVirtualCameraindex = CMain_Camera::VIRTUAL_CAMERA_21_HEAVY;
				//Desc.fAnimationLockTime = 0.7f;

				m_pGameInstance->Play_Sound(CSound_Manager::SOUND_KEY_NAME::Ultimate_3_Dash_21_SFX, false, 1.f);

			}
			//else if (m_bFinalSkillAdd && m_iFinalLoofCount == 0)
			//{
			//	Desc.bGrabedGravity = false;
			//	Desc.fAnimationLockTime = 3.f;
			//	Desc.bOnwerHitNoneStop = true;
			//}
			else
			{
				Desc.iGainAttackStep = 0;
				Desc.iDamage = 200 * Get_DamageScale(true);
				Desc.fDistance = { 100.f,0.f };

				//적 카메라로 순간적으로 올라가야함
				CMain_Camera* mainCamera = static_cast<CMain_Camera*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Main_Camera")));
				mainCamera->Play(CMain_Camera::VIRTUAL_CAMERA_21_ULTIMATE, 1, this, m_pEnemy);

			}
			Desc.iGrabAnimationIndex = ANIME_FINAL_START;
			Desc.bOwnerNextAnimation = false;
			//Desc.iOnwerNextAnimationIndex = ANIME_FINAL_START;
			//Desc.iVirtualCameraindex = CMain_Camera::VIRTUAL_CAMERA_SON_ULTIMATE;

			//Desc.ianimationIndex = 1;

			//Desc.fCameraShakeDuration = 0.5f;
			//Desc.fCameraShakeMagnitude = 0.2f;

			Desc.strHitEffectName = TEXT("21_WSDO-02");

			Desc.fForcedGravityTime = 0.f;

			Desc.fCameraShakeDuration = 0.1f;
			Desc.fCameraShakeMagnitude = 0.1f;

			m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack_CommandGrab"), TEXT("Layer_AttackObject"), &Desc);

		}
		else if (iAttackEvent == 2) // Position 61   여기에 넣으려했던것들이 53으로 옮기면서 폐기예정
		{

		}
		else if (iAttackEvent == 3) //Position  115 돌진 후 정지모션.
		{

			//명중했으면 루프돌기
			if (m_bAttackBackEvent && m_iFinalLoofCount != 0)
			{

				m_pEnemy->Set_UnDying(true);

				//m_bFinalSkillSucess = true;

				Set_CurrentAnimationPositionJump(52.9);
				m_iFinalLoofCount--;

				if (m_bFinalSoundEnable)
				{

					//3필 올라가면서 소리 내야하는데 115 키프레임이 정확한 싱크임, 태욱이가 추가한댔음
					m_pGameInstance->Play_Sound(CSound_Manager::SOUND_KEY_NAME::Ultimate_3_Attacks_21_SFX, false, 1.f);
					m_pGameInstance->Play_Sound(CSound_Manager::SOUND_KEY_NAME::Ultimate_3_Middle_21, false, 1.f);
					m_bFinalSoundEnable = false;




				}
			}
			else if (m_bFinalSkillAdd == true && m_iFinalLoofCount == 0) //추가타 루프 종료시  올려차기로 이행
			{
				Set_fImpulse({ 0.f,0.f });
				Set_CurrentAnimationPositionJump(136.9);

				//Set_AnimationStopWithoutMe(10.f);


			}
			else if (m_bAttackBackEvent && m_iFinalLoofCount == 0 && m_bFinalSkillAdd == false) //루프 다 돌았으면   추가타 확인, 아닌경우 내려찍기로 이행
			{
				//일단 이동 정지?
				Set_fImpulse({ 0.f,0.f });
				//QTE 추가타로 miLoof, 올려차기 할지 고민


				//QTE 성공했으면 5번 더 하고 위쪽으로 감
				if (m_iQTE == 1)
				{
					m_bFinalSkillAdd = true;
					m_iFinalLoofCount = 5;
					Set_CurrentAnimationPositionJump(52.9);
				}
				else
				{
					//Teleport_ToEnemy(-1.f, 2.f);
					Set_CurrentAnimationPositionJump(224.9f);
					//Set_fImpulse({ m_iLookDirection*2.f,-18.f });
					//Set_fImpulse({ m_iLookDirection * 2.f,-25.f });

				}
			}
			else if (m_bAttackBackEvent == false)
			{
				Set_fImpulse({ 0.f,0.f });
			}
			//명중하지 않았으면 AttackEvnet 4 에서 끝내기
		}
		else if (iAttackEvent == 4) //Position 135   돌진 막히거나 빗나갔을 시 Jump로 연결하고 끝. 
		{
			//원래라면 가드해도 AttackBack이 뜨지만 잡기는 아님. 버그 터지면 확인
			if (m_bAttackBackEvent == false)
			{
				Set_Animation(ANIME_JUMP_DOWN);
				Set_bAura(false);

				if (m_bCreateQTE)
				{
					Character_Start_QTE(2);
					m_bCreateQTE = false;
				}
			}
		}
		else if (iAttackEvent == 5) //Position 137   올려차기 시작 직전?   135랑 겹쳐서 문제생길 수 있으니 확인
		{
			//적 타임 스톱 및 카메라 연출
			//플레이어는 속도 느리게 애니메이션에서 설정
			//AnimSpeedChange, 0.018, 40

			_float fStopTime = 0.9f;
			Set_AnimationStopWithoutMe(fStopTime);


			//Teleport_ToEnemy(-0.8f, -0.7f);
			Teleport_ToEnemy(-0.8f, -1.5f);


			Character_Make_Effect(TEXT("Moving_Line_Down"));

			//적 카메라로 순간적으로 올라가야함
			CMain_Camera* mainCamera = static_cast<CMain_Camera*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Main_Camera")));
			mainCamera->Play(CMain_Camera::VIRTUAL_CAMERA_21_ULTIMATE, 2, this);
			//mainCamera->StartCameraShake(1.f, 0.4f);

			if (m_pAttackFinalChaseEffect_Layer != nullptr)
			{
				m_pAttackFinalChaseEffect_Layer->m_bIsDoneAnim = true;
				m_pAttackFinalChaseEffect_Layer = nullptr;
			}

		}
		else if (iAttackEvent == 10) //Position 150   올려차기 
		{
			CAttackObject::ATTACK_DESC Desc{};

			if (m_iPlayerTeam == 1)
				Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_1P_Melee_Attack;
			else
				Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_2P_Melee_Attack;
			Desc.ColliderDesc.pMineGameObject = this;
			Desc.ColliderDesc.vCenter = { 1.3f * m_iLookDirection,1.3f,0.f };
			Desc.ColliderDesc.vExtents = { 0.3f,0.5f,0.2f };


			//Desc.fhitCharacter_Impus = { -4.f * m_iLookDirection, 18.f };
			Desc.fhitCharacter_Impus = { -1.f * m_iLookDirection, 18.f };

			//Desc.fhitCharacter_StunTime = 0.6f;
			Desc.fhitCharacter_StunTime = 50.f;

			Desc.iDamage = 800 * Get_DamageScale(true);
			Desc.fLifeTime = 0.1f;
			Desc.ihitCharacter_Motion = { HitMotion::HIT_SPIN_AWAY_LEFTUP };
			Desc.iTeam = m_iPlayerTeam;
			Desc.fAnimationLockTime = 0.f;
			Desc.pOwner = this;
			Desc.bCameraZoom = false;

			Desc.fForcedGravityTime = 0.f;
			Desc.iGainAttackStep = 0;

			m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack"), TEXT("Layer_AttackObject"), &Desc);

		}
		else if (iAttackEvent == 6) //Position 225   내려찍기 
		{
			Set_AnimationStopWithoutMe(0.2f);

			if (m_pAttackFinalChaseEffect_Layer != nullptr)
			{
				m_pAttackFinalChaseEffect_Layer->m_bIsDoneAnim = true;
				m_pAttackFinalChaseEffect_Layer = nullptr;
			}

			CEffect_Layer::COPY_DESC tDesc{};

			tDesc.pPlayertMatrix = m_pModelCom->Get_BoneMatrixPtr("G_calf_L");
			tDesc.pTransformCom = m_pTransformCom;
			tDesc.m_isPlayerDirRight = m_iLookDirection;

			if (m_pAttackFinalChaseEffect_Layer != nullptr)
			{
				m_pAttackFinalChaseEffect_Layer->m_bIsDoneAnim = true;
				m_pAttackFinalChaseEffect_Layer = nullptr;
			}

			m_pAttackFinalChaseEffect_Layer = CEffect_Manager::Get_Instance()->Copy_Layer_AndGet(m_ChaseEffectName, &tDesc);
			m_pAttackFinalChaseEffect_Layer->Set_Copy_Layer_Rotation({ 0.f,0.f,-45.f });

			if (m_bFinalSkillAdd == false)
			{
				//Teleport_ToEnemy(-1.f, 2.f);
				//Set_fImpulse({ m_iLookDirection * 2.f,-25.f });

				m_pAttackFinalChaseEffect_Layer->Set_Copy_Layer_Scaled({ 1.1f,1.1f,1.1f });
				Teleport_ToEnemy(-1.f, 4.f);
				Set_fImpulse({ m_iLookDirection * 2.f,-40.f });


			}
			else
			{
				//Set_AnimationStopWithoutMe(0.3f);
				//Teleport_ToEnemy(-1.f, 2.f);
				m_pAttackFinalChaseEffect_Layer->Set_Copy_Layer_Scaled({ 1.2f,1.2f,1.2f });

				//Teleport_ToEnemy(-0.3f, 0.5f);
				//Set_fImpulse({ m_iLookDirection * 2.f,-25.f });

				Teleport_ToEnemy(-0.3f, 1.0f);
				Set_fImpulse({ m_iLookDirection * 3.f,-50.f });


			}
			//Teleport_ToEnemy(0.f, 1.8f);
			//Teleport_ToEnemy(-0.f, 2.f);
			//Teleport_ToEnemy(-1.f, 0.5f);
			//Teleport_ToEnemy(-1.f, 1.5f);


			//Teleport_ToEnemy(-1.5f, 3.f);
			//Set_fImpulse({ m_iLookDirection * 2.f,-25.f });
			//Set_fImpulse({ m_iLookDirection * 4.f,-50.f });

			Update_PreviousXPosition();


			//m_bDynamicMove = false;

			//내려찍기용 공격이펙트 생성  일단 214꺼 그대로 가져옴
			Character_Make_Effect(TEXT("Moving_Line_Down"));

			if (true)
			{
				CAttackObject::ATTACK_DESC Desc{};

				if (m_iPlayerTeam == 1)
					Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_1P_Melee_Attack;
				else
					Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_2P_Melee_Attack;
				Desc.ColliderDesc.pMineGameObject = this;
				Desc.ColliderDesc.vCenter = { 0.3f * m_iLookDirection,0.8f,0.f };
				//Desc.ColliderDesc.vExtents = { 3.f,3.f,0.2f };
				Desc.ColliderDesc.vExtents = { 0.3f,0.3f,0.2f };



				//캐릭터는 Look*2, -15 인데  오히려 좀 끌려오는 느낌으로?
				//Desc.fhitCharacter_Impus = { m_iLookDirection * 2.f, -15.f };
				//Desc.fhitCharacter_Impus = { m_iLookDirection * -1.f, -15.f };
				Desc.fhitCharacter_Impus = { m_fImpuse };


				Desc.fhitCharacter_StunTime = 50.f;	//일단잡기마냥 땅에 닿아야 풀리는 느낌 + 추가타로 풀어버리는 느낌


				Desc.iDamage = 500 * Get_DamageScale(true);
				//Desc.fLifeTime = 0.3f;  //어쩌지 현재높이로부 -13/s로 움직였을때 땅에 닿을때까지의 시간 
				Desc.fLifeTime = Get_fHeight() / 13.f;


				Desc.ihitCharacter_Motion = { HitMotion::HIT_KNOCK_AWAY_LEFTDOWN };
				Desc.iTeam = m_iPlayerTeam;
				Desc.fAnimationLockTime = 0.3f;

				//Desc.fAnimationLockTime = 0.f;

				Desc.pOwner = this;
				Desc.bGroundSmash = true;
				Desc.bCameraZoom = false;
				Desc.iGainAttackStep = 0;
				//Desc.fForcedGravityTime = 0.15f;
				Desc.bGrabbedEnd = true;
				//Desc.bHitNoGravity = true;


				m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack"), TEXT("Layer_AttackObject"), &Desc);
			}

			if (false)
			{
				CAttackObject_CommandGrab::ATTACK_COMMANDGRAB_DESC Desc{};
				if (m_iPlayerTeam == 1)
					Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_1P_Melee_Attack;
				else
					Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_2P_Melee_Attack;
				Desc.ColliderDesc.pMineGameObject = this;

				//Desc.ColliderDesc.vExtents = { 0.8f,0.6f,1.f };
				//Desc.ColliderDesc.vCenter = { 0.3f,0.7f,0.f };

				Desc.ColliderDesc.vCenter = { 0.3f * m_iLookDirection,0.8f,0.f };
				//Desc.ColliderDesc.vExtents = { 3.f,3.f,0.2f };
				Desc.ColliderDesc.vExtents = { 0.1f,0.1f,0.2f };

				//Desc.ColliderDesc.pTransform = m_pTransformCom;
				Desc.fhitCharacter_Impus = { m_fImpuse };
				Desc.fhitCharacter_StunTime = 2000.f;
				Desc.fLifeTime = 0.6f;
				Desc.ihitCharacter_Motion = { HitMotion::HIT_KNOCK_AWAY_LEFTDOWN };
				Desc.iTeam = m_iPlayerTeam;

				Desc.fAnimationLockTime = 0.1f;


				Desc.pOwner = this;

				Desc.eAttackType = ATTACKTYPE_HIGH;
				Desc.eAttackGrade = GRADE_ULTIMATE;

				Desc.fDistance = { 0.f,-0.5f };

				//Desc.fForcedGravityTime = 0.f;
				//Desc.fForcedGravityTime = 0.255f;
				//Desc.bGrabedGravity = true;



				Desc.iGainAttackStep = 1;
				Desc.iDamage = 700 * Get_DamageScale(true);

				Desc.iGrabAnimationIndex = ANIME_FINAL_START;
				Desc.bOwnerNextAnimation = false;


				//Desc.fForcedGravityTime = 0.f;
				//Desc.bHitNoGravity = false;

				m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack_CommandGrab"), TEXT("Layer_AttackObject"), &Desc);
			}




			// 카메라 진동넣어야겠다
			CMain_Camera* mainCamera = static_cast<CMain_Camera*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Main_Camera")));
			mainCamera->StartCameraShake(0.5f, 0.2f);
		}
		else if (iAttackEvent == 201)
		{



		}
	}

	break;

	case Client::CPlay_21::ANIME_FINAL_END:
	{
		if (iAttackEvent == 0)
		{
			//카메라 컷씬
			//잡기로 상대방 고정
			//상대 플레이어 안보이게?



			CMain_Camera* main_Camera = static_cast<CMain_Camera*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Main_Camera")));
			//원래 방향
			if (Get_iDirection() == 1)
				main_Camera->Play(CMain_Camera::VIRTUAL_CAMERA_21_ULTIMATE, 4, this);
			//좌우 반전
			else
				main_Camera->Play(CMain_Camera::VIRTUAL_CAMERA_21_ULTIMATE, 5, this, nullptr, true);



			//안보이게 하는 대신에  고정 위치를 저 멀리로 보내버리고, 공격기술 범위를 엄청 크게 한 뒤에 잡기 풀때 다시 데려오는 방법도 있음
			//이때 맵밖으로 밀려나가는건 어떻게 처리?
			//아예 하늘 위로?  땅 밑은?


			//m_bDynamicMove = false;


			CAttackObject_CommandGrab::ATTACK_COMMANDGRAB_DESC Desc{};
			if (m_iPlayerTeam == 1)
				Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_1P_Melee_Attack;
			else
				Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_2P_Melee_Attack;
			Desc.ColliderDesc.pMineGameObject = this;
			Desc.ColliderDesc.vExtents = { 40.f,40.f,1.f };
			Desc.ColliderDesc.vCenter = { 0.3f * m_iLookDirection,0.7f,0.f };
			//Desc.ColliderDesc.pTransform = m_pTransformCom;
			//Desc.fhitCharacter_Impus = { 0.3f * m_iLookDirection,0 };
			Desc.fhitCharacter_StunTime = 20.f;
			Desc.iDamage = 0;
			Desc.fLifeTime = 0.2f;
			Desc.ihitCharacter_Motion = { HitMotion::HIT_NONE };
			Desc.iTeam = m_iPlayerTeam;
			Desc.fAnimationLockTime = 0.1f;
			Desc.pOwner = this;


			Desc.eAttackType = ATTACKTYPE_HIGH;



			//딱히 문제는 없어보임
			//Desc.fDistance = { 0.f ,100.f };

			//이것도 문제가 없어보이는데?
			Desc.fDistance = { 0.f ,-10.f };


			Desc.fForcedGravityTime = 0.f;
			Desc.iGainAttackStep = 0;
			Desc.iGrabAnimationIndex = ANIME_FINAL_END;
			Desc.bOwnerNextAnimation = false;
			Desc.bForcedHit = true;
			Desc.iOnwerDirection = 0; //맞아도 뒤집지 않음

			Desc.iGainHitCount = 0;

			m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack_CommandGrab"), TEXT("Layer_AttackObject"), &Desc);


		}

		//손 뻗고나서
		else if (iAttackEvent == 1)
		{

			//Character_Make_Effect(TEXT("21_WSDO-05"));

			//Character_Make_BoneEffect("GD_fist_L", TEXT("21_WSDO-05"));
			Character_Make_BoneEffect("GD_hand_L", TEXT("21_WSDO-05"));

			m_pEnemy->Set_UnDying(false);
			 

			_vector position = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			CParticle_Manager::Get_Instance()->Play(CParticle_Manager::ULTIMATE_3_21_PARTICLE, position);
		}
		else if (iAttackEvent == 2)
		{
			//공격이펙트로 화면가리기

			//Character_Make_Effect(TEXT("21_WSDO-05"));
			//Character_Make_Effect(TEXT("21_WSDO-06"));


			//Character_Make_Effect(TEXT("21_WSDO-06"));


			//EAST_SPHERE

			if (m_pEnemy->Get_iHP() < 3500 * Get_DamageScale(true))
			{
				CMap_Manager::Get_Instance()->PlayerCall_EastFinish(CMap_Manager::EAST_SPHERE, 1.f);
				m_pEnemy->Set_FinalSkillRoundEnd(true, 0);
				//캐릭터 MaxDeath 도 처리
				CUI_Manager::Get_Instance()->CutSceneUI(false);

			}

		}
		else if (iAttackEvent == 3)
		{
			//잡기로 다시 끌고오기
			CAttackObject_CommandGrab::ATTACK_COMMANDGRAB_DESC Desc{};
			if (m_iPlayerTeam == 1)
				Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_1P_Melee_Attack;
			else
				Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_2P_Melee_Attack;
			Desc.ColliderDesc.pMineGameObject = this;
			Desc.ColliderDesc.vExtents = { 20.f,30.f,1.f };
			Desc.ColliderDesc.vCenter = { 0.f,0.f,0.f };
			//Desc.ColliderDesc.pTransform = m_pTransformCom;
			//Desc.fhitCharacter_Impus = { 0.3f * m_iLookDirection,0 };
			Desc.fhitCharacter_StunTime = 20.f;
			Desc.iDamage = 0;
			Desc.fLifeTime = 0.2f;
			Desc.ihitCharacter_Motion = { HitMotion::HIT_NONE };
			Desc.iTeam = m_iPlayerTeam;
			Desc.fAnimationLockTime = 0.1f;
			Desc.pOwner = this;


			Desc.eAttackType = ATTACKTYPE_HIGH;



			//딱히 문제는 없어보임
			//Desc.fDistance = { 0.f ,100.f };

			//이것도 문제가 없어보이는데?
			Desc.fDistance = { m_iLookDirection * 0.3f , 0.5f };


			Desc.fForcedGravityTime = 0.f;
			Desc.iGainAttackStep = 0;
			Desc.iGrabAnimationIndex = ANIME_FINAL_END;
			Desc.bOwnerNextAnimation = false;
			Desc.bForcedHit = true;
			Desc.iOnwerDirection = 0; //맞아도 뒤집지 않음

			Desc.iGainHitCount = 0;

			m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack_CommandGrab"), TEXT("Layer_AttackObject"), &Desc);

			//m_pRenderInstance->Get_Instance()->Start_AllWhiteOut(1.f,1.f);

			if(m_pEnemy->Get_iHP() > 3500 * Get_DamageScale(true))
				m_pRenderInstance->Get_Instance()->Start_AllWhiteOut(2.f, 0.5f);


		}
		else if (iAttackEvent == 4)
		{
			//잡기 해제 + 데미지
			CAttackObject::ATTACK_DESC Desc{};
			/*	Desc.ColliderDesc.width = 1.0;
				Desc.ColliderDesc.height = 1.0;
				Desc.ColliderDesc.vCenter = { 1.0f * m_iLookDirection,0.8f,0.f };
				Desc.ColliderDesc.pTransform = m_pTransformCom;*/

			if (m_iPlayerTeam == 1)
				Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_1P_Melee_Attack;
			else
				Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_2P_Melee_Attack;
			Desc.ColliderDesc.pMineGameObject = this;
			Desc.ColliderDesc.vExtents = { 5.f,5.f,1.f };
			Desc.ColliderDesc.vCenter = { 0.f,0.f,0.f };

			Desc.fhitCharacter_Impus = { 0.f,-15.f };
			Desc.fhitCharacter_StunTime = 2.0f;
			//Desc.iDamage = 2200 * Get_DamageScale(true);
			Desc.iDamage = 3500 * Get_DamageScale(true);

			Desc.fLifeTime = 0.2f;
			Desc.ihitCharacter_Motion = { HitMotion::HIT_HEAVY_DOWN };
			Desc.bGroundSmash = true;
			Desc.iTeam = m_iPlayerTeam;
			Desc.fAnimationLockTime = 0.7f;
			Desc.bGrabbedEnd = true;
			Desc.pOwner = this;
			Desc.bCameraZoom = false;


			Desc.bOnwerHitNoneStop = true;

			m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack"), TEXT("Layer_AttackObject"), &Desc);

		

		}
		else if (iAttackEvent == 5)
		{

			Character_Make_Effect(TEXT("21_WSDO-06"));

			Character_Make_Effect(TEXT("21_WSDO-06"), {}, true);

			//Set_AnimationStop(0.7f);

			Set_bAura(false);


			CParticle_Manager::Get_Instance()->Stop(CParticle_Manager::ULTIMATE_3_21_PARTICLE);

			CEffect_Layer* pEffect = Character_Make_BoneEffect("G_root", TEXT("Smoke03_Five_Dir"));
			pEffect->Set_Copy_Layer_Scaled({ 2.f,1.f,2.f });
			//pEffect->m_fTickPerSecond *= 0.2f;

			pEffect = Character_Make_BoneEffect("G_root", TEXT("Smoke03_Five_Dir_Rotated_Right"));
			pEffect->Set_Copy_Layer_Scaled({ 2.f,1.f,2.f });

			m_pRenderInstance->Get_Instance()->Switch_BlackOut(false);

		}

	}
	break;

	case Client::CPlay_21::ANIME_IDLE:
		break;
	case Client::CPlay_21::ANIME_FORWARD_WALK:
		break;
	case Client::CPlay_21::ANIME_BACK_WALK:
		break;
	case Client::CPlay_21::ANIME_FORWARD_DASH:
		break;
	case Client::CPlay_21::ANIME_BACK_DASH:
		break;
	case Client::CPlay_21::ANIME_FORWARD_DASH_END:
		break;
	case Client::CPlay_21::ANIME_BACK_DASH_END:
		break;
	case Client::CPlay_21::ANIME_CROUCH_START:
		break;
	case Client::CPlay_21::ANIME_CROUCHING:
		break;
	case Client::CPlay_21::ANIME_JUMP_UP:
		break;
	case Client::CPlay_21::ANIME_JUMP_DOWN:
		break;
	case Client::CPlay_21::ANIME_CHASE:
		break;
	case Client::CPlay_21::ANIME_HIT_LIGHT:
		break;
	case Client::CPlay_21::ANIME_HIT_LIGHT_FRONT:
		break;
	case Client::CPlay_21::ANIME_HIT_CROUCH:
		break;
	case Client::CPlay_21::ANIE_HIT_AIR:
		break;
	case Client::CPlay_21::ANIME_HIT_FALL:
		break;
	case Client::CPlay_21::ANIME_HIT_BOUND_DOWN:
		break;
	case Client::CPlay_21::ANIME_HIT_HEAVY_AWAY_LEFT:
		break;
	case Client::CPlay_21::ANIME_HIT_HEAVY_AWAY_UP:
		break;
	case Client::CPlay_21::ANIME_HIT_HEAVY_AWAY_LEFTDOWN:
		break;
	case Client::CPlay_21::ANIME_HIT_HEAVY_AWAY_SPIN_UP:
		break;
	case Client::CPlay_21::ANIME_HIT_HEAVY_AWAY_SPIN_LEFTUP:
		break;
	case Client::CPlay_21::ANIME_HIT_HEAVY_AWAY_SPIN_LEFT:
		break;
	case Client::CPlay_21::ANIME_HIT_WALLBOUNCE:
		break;
	case Client::CPlay_21::ANIME_LAYUP:
		break;
	case Client::CPlay_21::ANIME_BREAK_FALL_GROUND:
		break;
	case Client::CPlay_21::ANIME_BREAK_FALL_AIR:
	{
		//위치 고정용 빈거
		if (iAttackEvent == 0)
		{
			if (m_bGrabDraw)
			{
				m_bGrabDraw = false;

				CAttackObject_CommandGrab::ATTACK_COMMANDGRAB_DESC Desc{};
				if (m_iPlayerTeam == 1)
					Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_1P_Melee_Attack;
				else
					Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_2P_Melee_Attack;

				Desc.ColliderDesc.pMineGameObject = this;
				Desc.ColliderDesc.vExtents = { 4.f,4.f,1.f };
				Desc.ColliderDesc.vCenter = { 0.3f,0.7f,0.f };
				//Desc.ColliderDesc.pTransform = m_pTransformCom;
				//Desc.fhitCharacter_Impus = { 0.3f * m_iLookDirection,0 };
				Desc.fhitCharacter_StunTime = 0.3f;
				Desc.iDamage = 0;
				Desc.fLifeTime = 0.2f;
				Desc.ihitCharacter_Motion = { HitMotion::HIT_NONE };
				Desc.iTeam = m_iPlayerTeam;
				Desc.fAnimationLockTime = 0.1f;
				Desc.pOwner = this;


				Desc.eAttackType = ATTACKTYPE_HIGH;

				Desc.bDrawNoneStop = true;
				Desc.bGrabbedEnd = true;

				Desc.fForcedGravityTime = 0.f;
				Desc.iGainAttackStep = 0;
				Desc.bOwnerNextAnimation = false;
				Desc.bForcedHit = true;
				Desc.iOnwerDirection = 0; //맞아도 뒤집지 않음

				Desc.iGainHitCount = 0;

				m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack_CommandGrab"), TEXT("Layer_AttackObject"), &Desc);

			}
		}

	}
	break;
	case Client::CPlay_21::ANIME_DIE_STAND:
		break;
	case Client::CPlay_21::ANIME_GUARD_GROUND:
		break;
	case Client::CPlay_21::ANIME_GUARD_CROUCH:
		break;
	case Client::CPlay_21::ANIME_GUARD_AIR:
		break;
	case Client::CPlay_21::ANIME_GRAB_READY:
		break;
	case Client::CPlay_21::ANIME_GRAB:
		switch (iAttackEvent)
		{
			//잡기 시작
		case 0:
		{
			CAttackObject_Grab::ATTACK_Grab_DESC Desc{};
			//Desc.ColliderDesc.width = 1.4f;
			//Desc.ColliderDesc.height = 2.2f;
			if (m_iPlayerTeam == 1)
				Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_1P_Melee_Attack;
			else
				Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_2P_Melee_Attack;
			Desc.ColliderDesc.pMineGameObject = this;
			//Desc.ColliderDesc.vExtents = { 1.4f,2.2f,1.f };

			Desc.ColliderDesc.vExtents = { 1.2f,1.0f,1.f };


			Desc.ColliderDesc.vCenter = { 0.3f * m_iLookDirection,0.7f,0.f };
			//Desc.ColliderDesc.pTransform = m_pTransformCom;
			//Desc.fhitCharacter_Impus = { 0.3f * m_iLookDirection,0 };
			Desc.fhitCharacter_StunTime = 1.f;
			Desc.iDamage = 700 * Get_DamageScale();;
			Desc.fLifeTime = 0.2f;
			Desc.ihitCharacter_Motion = { HitMotion::HIT_MEDIUM };
			Desc.iTeam = m_iPlayerTeam;
			Desc.fAnimationLockTime = 0.1f;
			Desc.pOwner = this;

			if (m_bGrab_Air)
				Desc.eAttackType = ATTACKTYPE_GRAB_AIR;
			else
				Desc.eAttackType = ATTACKTYPE_GRAB_GROUND;

			Desc.fDistance = { 1.2f * m_iLookDirection,0.f };
			//Desc.fGrabAnimationPosition = 40.f;
			Desc.fGrabAnimationPosition = 25.f;

			Desc.iOnwerNextAnimationIndex = m_iGrabAnimationIndex;

			m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack_Grab"), TEXT("Layer_AttackObject"), &Desc);
			m_pGameInstance->Play_Group_Sound(CSound_Manager::SOUND_GROUP_KEY::LIGHT_ATTACK_Hit_SFX, false, 1.f);
		}
		break;

		//난타
		case 1:
		case 3:
		{
			CAttackObject::ATTACK_DESC Desc{};

			if (m_iPlayerTeam == 1)
				Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_1P_Melee_Attack;
			else
				Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_2P_Melee_Attack;
			Desc.ColliderDesc.pMineGameObject = this;
			Desc.ColliderDesc.vCenter = { 0.9f * m_iLookDirection,0.8f,0.f };
			Desc.ColliderDesc.vExtents = { 0.3f,0.5f,0.2f };



			Desc.fhitCharacter_Impus = { 0.3f * m_iLookDirection,0 };
			Desc.fhitCharacter_StunTime = 1.f;
			Desc.iDamage = 80 * Get_DamageScale();
			Desc.fLifeTime = 0.1f;
			Desc.ihitCharacter_Motion = { HitMotion::HIT_LIGHT };
			Desc.iTeam = m_iPlayerTeam;
			Desc.fAnimationLockTime = 0.f;
			Desc.iGainAttackStep = 0;
			Desc.pOwner = this;
			Desc.bDrawNoneStop = true;

			m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack"), TEXT("Layer_AttackObject"), &Desc);
			m_pGameInstance->Play_Group_Sound(CSound_Manager::SOUND_GROUP_KEY::LIGHT_ATTACK_Hit_SFX, false, 1.f);
		}
		break;
		case 2:
		case 4:
		{
			CAttackObject::ATTACK_DESC Desc{};

			if (m_iPlayerTeam == 1)
				Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_1P_Melee_Attack;
			else
				Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_2P_Melee_Attack;
			Desc.ColliderDesc.pMineGameObject = this;
			Desc.ColliderDesc.vCenter = { 0.9f * m_iLookDirection,0.8f,0.f };
			Desc.ColliderDesc.vExtents = { 0.3f,0.5f,0.2f };


			Desc.fhitCharacter_Impus = { 0.3f * m_iLookDirection,0 };
			Desc.fhitCharacter_StunTime = 1.f;
			Desc.iDamage = 80 * Get_DamageScale();
			Desc.fLifeTime = 0.1f;
			Desc.ihitCharacter_Motion = { HitMotion::HIT_LIGHT };
			Desc.iTeam = m_iPlayerTeam;
			Desc.fAnimationLockTime = 0.f;
			Desc.iGainAttackStep = 0;
			Desc.pOwner = this;
			Desc.bDrawNoneStop = true;

			m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack"), TEXT("Layer_AttackObject"), &Desc);
			m_pGameInstance->Play_Group_Sound(CSound_Manager::SOUND_GROUP_KEY::LIGHT_ATTACK_Hit_SFX, false, 1.f);
		}
		break;

		}
		break;
	case ANIME_START_DEFAULT:
	{
		//빙글 돌 때 아우라
		if (iAttackEvent == 0)
		{
			//Character_Make_BoneEffect("G_root", TEXT("21_Aura"));

			//
			//CEffect_Layer* pEffect = Character_Make_BoneEffect("G_root", TEXT("21_Aura"));
			//if(pEffect != nullptr)
			//{
			//	pEffect->Set_Copy_Layer_Scaled({ 1.5f,1.5f,1.5f });
			//	//pEffect->Set_Copy_Layer_Position({ -1.f,0.f,0.f });
			//
			//}

			Character_Make_BoneEffect("G_root", TEXT("21_Opening"));

			CEffect_Layer* pEffect = Character_Make_BoneEffect("G_root", TEXT("Smoke03_Five_Dir"));
			pEffect->Set_Copy_Layer_Scaled({ 2.f,1.f,2.f });
			//pEffect->m_fTickPerSecond *= 0.2f;

			pEffect = Character_Make_BoneEffect("G_root", TEXT("Smoke03_Five_Dir_Rotated_Right"));
			pEffect->Set_Copy_Layer_Scaled({ 2.f,1.f,2.f });


			
			_vector resultPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION); //XMVectorAdd(position, XMVectorSet(0.f, 0.f, 0.f, 0.f));
			_float3 resultFloat3{};
			XMStoreFloat3(&resultFloat3, resultPosition);
			CParticle_Manager::Get_Instance()->Play(CParticle_Manager::FREIZA_ULTIMATE_1_HIT_PARTICLE, resultFloat3);

		}

	}
	break;

	case ANIME_NEWROUND_RIGHTHAND_APEEAR_CUTSCENE:
	{
		if (iAttackEvent == 2001)
		{
			Character_Make_Effect(TEXT("Start_Battle-01"), { -0.7f * m_iLookDirection,0.f });
		}
		else if (iAttackEvent == 2002)
		{
			//CEffect_Layer::COPY_DESC pDesc{};
			//_float4x4 fCamMat = {};
			//XMStoreFloat4((_float4*)&fCamMat.m[3][0], m_pGameInstance->Get_CamPosition_Vector());
			//pDesc.pPlayertMatrix = &fCamMat;
			//CEffect_Manager::Get_Instance()->Copy_Layer(TEXT("Start_Battle-04"), &pDesc);

			CRenderInstance::Get_Instance()->Start_AllWhiteOut(0.5f, 2.f);

			m_pEnemy->Set_AnimationStop(0.5f);
			Set_AnimationStop(0.5f);

		}
		else if (iAttackEvent == 2003)
		{
			Character_Make_Effect(TEXT("Start_Battle-02"), { -0.7f * m_iLookDirection,0.f });

			//Set_AnimationStop(1.5f);
			//m_pEnemy->Set_AnimationStop(1.5f);
		}
		else if (iAttackEvent == 2004)
		{
			CEffect_Layer::COPY_DESC pDesc{};

			_float4x4 fCamMat = {};

			XMStoreFloat4((_float4*)&fCamMat.m[3][0], m_pGameInstance->Get_CamPosition_Vector());

			pDesc.pPlayertMatrix = &fCamMat;

			CEffect_Manager::Get_Instance()->Copy_Layer(TEXT("Start_Battle-03"), &pDesc);
			CUI_Manager::Get_Instance()->CutSceneUI(true);


		}


	}
	break;

	default:
		break;
	}

	}
}

void CPlay_21::Play_Sound(_uint SoundName, _bool bisLoof, _float fvolume)
{
	m_pGameInstance->Play_Sound((CSound_Manager::SOUND_KEY_NAME)SoundName, bisLoof, fvolume);
}

void CPlay_21::Play_Group_Sound(_uint groupKey, _bool loop, _float volume)
{
	m_pGameInstance->Play_Group_Sound((CSound_Manager::SOUND_GROUP_KEY)groupKey, loop, volume);
}

void CPlay_21::Play_Sound_Stop(_uint SoundName)
{
	m_pGameInstance->Stop_Sound((CSound_Manager::SOUND_KEY_NAME)SoundName);
}


CPlay_21* CPlay_21::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPlay_21* pInstance = new CPlay_21(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CPlay_21"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPlay_21::Clone(void* pArg)
{
	CPlay_21* pInstance = new CPlay_21(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CPlay_21"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlay_21::Free()
{
	__super::Free();


	Safe_Release(m_pShaderCom);
	Safe_Release(m_p2PTextureCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pDecalTextureCom);
}
