#include "stdafx.h"
#include "..\Public\Play_Goku.h"

#include "RenderInstance.h"
#include "GameInstance.h"

#include "AttackObject.h"
#include "AttackObject_Grab.h"
#include "AttackObject_CommandGrab.h"
#include "AttackObject_Ranged.h"

#include "AttackObject_Energy.h"

#include "AttackObject_Reflect.h"

#include "UI_Manager.h"
#include "iostream"

#include "Effect.h"
#include "BattleInterface.h"


#include "Main_Camera.h"

#include "BoneEffectObject.h"
#include "Effect_Manager.h"	

#include "Effect_Layer.h"
#include "Animation.h"

#include "QTE_Manager.h"

#include "Map_Manager.h"
#include "Particle_Manager.h"

#include"Level_GamePlay.h"

CPlay_Goku::CPlay_Goku(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCharacter{ pDevice, pContext }
{

}

CPlay_Goku::CPlay_Goku(const CPlay_Goku& Prototype)
	: CCharacter{ Prototype }
{

}

HRESULT CPlay_Goku::Initialize_Prototype()
{
	//m_pFrameEvent = CFrameEvent_Manager::Get_Instance()->Get_pFrameEventMap();
	CFrameEvent_Manager::Get_Instance()->Initalize_NameMap();
	CFrameEvent_Manager::Get_Instance()->LoadFile2("../Bin/FrameEventData/EventData_Goku.txt");
	CFrameEvent_Manager::Get_Instance()->LoadFile2("../Bin/FrameEventData/EventData_Goku_ss3.txt");

	return S_OK;
}

HRESULT CPlay_Goku::Initialize(void* pArg)
{

	m_fAIrGrabEndAnimationPositon = 29.99f;

	m_eCharacterIndex = PLAY_GOKU;
	m_iFallAnimationIndex = ANIME_JUMP_DOWN;
	m_iIdleAnimationIndex = ANIME_IDLE;
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


	m_iNextAnimation.first = ANIME_IDLE;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(-2.f + (m_iPlayerTeam * 2), 0.f, 0.f, 1.f));

	//m_eChaseSoundIndex = (_short)CSound_Manager::SOUND_KEY_NAME::Chase_Attack_21;

	m_tAttackMap.Initalize(this);
	Character_DESC* pDesc = static_cast<Character_DESC*>(pArg);
	m_strName = "GOKU_" + to_string(m_iPlayerTeam)+"_" + to_string(pDesc->ePlayerSlot);
	m_RendererDesc.strName = m_strName;
	LIGHT_DESC			LightDesc{};

	LightDesc.eType = LIGHT_DESC::TYPE_DIRECTIONAL;

	LightDesc.vDirection = _float4(-0.15f, -0.7f, 0.5f, 0.f);
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.0f, 1.0f);
	LightDesc.vAmbient = _float4(0.5f, 0.5f, 0.5f, 1.f);
	LightDesc.vSpecular = _float4(0.f, 0.f, 0.f, 1.f);
	LightDesc.pPlayerDirection = &m_iLookDirection;
	LightDesc.strName = m_strName;
	//LightDesc.vAuraColor = _float4(15.07f, 1.53333f, 0.5f, 1.89f);
	LightDesc.vAuraColor = _float4(0.f,0.f,0.f,0.f);

	m_fAuraColor = _float4(15.07f, 1.53333f, 0.5f, 1.89f);

	if (FAILED(m_pRenderInstance->Add_Player_Light(m_strName, LightDesc, _float4(1.5f, 1.4f, 1.17647f, 1.f), &m_bChase)))
		return E_FAIL;


	LIGHT_DESC* pLight_Desc = m_pRenderInstance->Get_LightDesc(CLight_Manager::LIGHT_PLAYER, 0, m_strName);
	//pLight_Desc->vAuraColor = _float4(15.07f, 1.53333f, 0.5f, 1.89f);
	pLight_Desc->vAuraColor = _float4(0.f,0.f,0.f,0.f);

	/*
	빛 각자 생성해주기
	*/


	//m_pModelCom->SetUp_Animation(16, true);
	m_pModelCom->SetUp_Animation(0, true);
	m_pModelCom->Play_Animation(0.f);

	//MoveCommandPatterns.push_back({ hadoukenPattern, ShowInputBuffer});

	MoveCommandPatterns.push_back({ Command_236Attack, 66 });
	MoveCommandPatterns.push_back({ Command_236Attack_Extra, 66 });

	MoveCommandPatterns.push_back({ Command_LightAttack, ANIME_ATTACK_LIGHT1 });
	MoveCommandPatterns.push_back({ Command_MediumAttack, ANIME_ATTACK_MEDIUM });
	MoveCommandPatterns.push_back({ Command_HeavyAttack, ANIME_ATTACK_HEAVY });


	MoveCommandPatternsFunction.push_back({ Command_236UltimateAttack, bind(&CGoku_MeleeAttack::Attack_236Ultimate, &m_tAttackMap) });
	MoveCommandPatternsFunction.push_back({ Command_236UltimateAttack_Side, bind(&CGoku_MeleeAttack::Attack_236Ultimate_Side, &m_tAttackMap) });
	MoveCommandPatternsFunction.push_back({ Command_236UltimateAttack_Side_Extra, bind(&CGoku_MeleeAttack::Attack_236Ultimate_Side, &m_tAttackMap) });




	MoveCommandPatternsFunction.push_back({ Command_236Attack,  bind(&CGoku_MeleeAttack::Attack_236, &m_tAttackMap) });
	MoveCommandPatternsFunction.push_back({ Command_236Attack_Extra,  bind(&CGoku_MeleeAttack::Attack_236, &m_tAttackMap) });

	MoveCommandPatternsFunction.push_back({ Command_236Attack_Heavy,  bind(&CGoku_MeleeAttack::Attack_236_Heavy, &m_tAttackMap) });
	MoveCommandPatternsFunction.push_back({ Command_236Attack_Heavy_Extra,  bind(&CGoku_MeleeAttack::Attack_236_Heavy, &m_tAttackMap) });


	MoveCommandPatternsFunction.push_back({ Command_214Attack,  bind(&CGoku_MeleeAttack::Attack_214, &m_tAttackMap) });
	MoveCommandPatternsFunction.push_back({ Command_214Attack_Extra,  bind(&CGoku_MeleeAttack::Attack_214, &m_tAttackMap) });

	MoveCommandPatternsFunction.push_back({ Command_236Special,  bind(&CGoku_MeleeAttack::Attack_236Special, &m_tAttackMap) });
	MoveCommandPatternsFunction.push_back({ Command_236Special_Side,  bind(&CGoku_MeleeAttack::Attack_236Special_Side, &m_tAttackMap) });
	MoveCommandPatternsFunction.push_back({ Command_236Special_Side_Extra,  bind(&CGoku_MeleeAttack::Attack_236Special_Side, &m_tAttackMap) });

	


	MoveCommandPatternsFunction.push_back({ Command_214FinalAttack, bind(&CGoku_MeleeAttack::Attack_214Final, &m_tAttackMap) });


	MoveCommandPatternsFunction.push_back({ Command_Crouch_LightAttack, bind(&CGoku_MeleeAttack::Attack_Crouch_Light, &m_tAttackMap) });
	MoveCommandPatternsFunction.push_back({ Command_Crouch_MediumAttack, bind(&CGoku_MeleeAttack::Attack_Crouch_Medium, &m_tAttackMap) });
	MoveCommandPatternsFunction.push_back({ Command_Crouch_HeavyAttack, bind(&CGoku_MeleeAttack::Attack_Crouch_Heavy, &m_tAttackMap) });


	MoveCommandPatternsFunction.push_back({ Command_Crouch_MediumAttack_Extra, bind(&CGoku_MeleeAttack::Attack_Crouch_Medium, &m_tAttackMap) });
	MoveCommandPatternsFunction.push_back({ Command_Crouch_HeavyAttack_Extra, bind(&CGoku_MeleeAttack::Attack_Crouch_Heavy, &m_tAttackMap) });



	//위에서 부터 확인하므로 간단한 커맨드가 아래로 가야함
	MoveCommandPatternsFunction.push_back({ Command_LightAttack, bind(&CGoku_MeleeAttack::Attack_Light, &m_tAttackMap) });
	MoveCommandPatternsFunction.push_back({ Command_MediumAttack, bind(&CGoku_MeleeAttack::Attack_Medium, &m_tAttackMap) });
	MoveCommandPatternsFunction.push_back({ Command_HeavyAttack, bind(&CGoku_MeleeAttack::Attack_Heavy, &m_tAttackMap) });
	MoveCommandPatternsFunction.push_back({ Command_SpecialAttack, bind(&CGoku_MeleeAttack::Attack_Special, &m_tAttackMap) });


	MoveCommandPatternsFunction.push_back({ Command_HeavyAttack_Extra, bind(&CGoku_MeleeAttack::Attack_Heavy, &m_tAttackMap) });
	MoveCommandPatternsFunction.push_back({ Command_MediumAttack_Extra, bind(&CGoku_MeleeAttack::Attack_Medium, &m_tAttackMap) });


	MoveCommandPatternsFunction.push_back({ Command_Grab, bind(&CGoku_MeleeAttack::Attack_Grab, &m_tAttackMap) });


	//MoveCommandPatternsFunction.push_back({ Command_BackDash, bind(&CGoku_MeleeAttack::BackDash, &m_tAttackMap) });
	//MoveCommandPatternsFunction.push_back({ Command_Forward, bind(&CGoku_MeleeAttack::ForwardDash, &m_tAttackMap) });

	MoveCommandPatternsFunction_Exactly.push_back({ Command_BackDash, bind(&CGoku_MeleeAttack::BackDash, &m_tAttackMap) });
	MoveCommandPatternsFunction_Exactly.push_back({ Command_Forward, bind(&CGoku_MeleeAttack::ForwardDash, &m_tAttackMap) });


	MoveCommandPatternsFunction_Exactly.push_back({ Command_Reflect, bind(&CGoku_MeleeAttack::Reflect, &m_tAttackMap) });
	MoveCommandPatternsFunction_Exactly.push_back({ Command_BenishingAttack, bind(&CGoku_MeleeAttack::Attack_Benishing, &m_tAttackMap) });

	

	m_eCharacterID = CUI_Define::PLAYER_ID::GOKU;

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



	//LPlayer1, RPlayer1이 아니면  ss1오공 상태로 시작
	if (m_ePlayerSlot == CUI_Define::PLAYER_SLOT::LPLAYER1 || m_ePlayerSlot == CUI_Define::PLAYER_SLOT::RPLAYER1)
	{
		;
		m_bAura = false;
	}
	else
	{
		m_bNormalGoku = false;
	}

	//Set_bAura(false);
	{	
		//LIGHT_DESC* pLight_DescTemp = m_pRenderInstance->Get_LightDesc(CLight_Manager::LIGHT_PLAYER, 0, m_strName);
		//pLight_DescTemp->vAuraColor = m_fAuraColor;
		m_bAura = false;
	}

	return S_OK;
}

void CPlay_Goku::Player_Update(_float fTimeDelta)
{
	__super::Player_Update(fTimeDelta);


	//if (m_bOnlyCutSceneNoMove)
	//{
	//	Character_Play_Animation(fTimeDelta);
	//	return;
	//}

	//cout << m_pModelCom->m_fCurrentAnimPosition << endl;

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

	//cout << m_fAccStunTime << " / " << m_fMaxStunTime << endl;



	//사망시 업데이트 처리.  return때문에
	if (m_bDying)
	{
		if(m_bAnimationLock == true)
			Update_AnimationLock(fTimeDelta);
		else
		{
			
			Character_Play_Animation(fTimeDelta);
	
			if(Get_fHeight()>0)
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
			if (m_bCinematic_NoMoveXZ)
				Character_Play_Animation_NoXZ(fTimeDelta);
			else
				Character_Play_Animation(fTimeDelta);

			if (m_pModelCom->m_iCurrentAnimationIndex == m_iStartAnimatonIndex)
			{
				if (m_bMotionPlaying == false)
				{
					if (m_fAccfirstOpeningTime >= 0.5f)
					{
						m_fAccfirstOpeningTime = 0.f;
						CBattleInterface_Manager::Get_Instance()->Character_Opening_EndForCharacter(m_iPlayerTeam);

						if (m_pFinalAura != nullptr)
						{
							m_pFinalAura->m_bIsDoneAnim = true;
							m_pFinalAura = nullptr;
						}
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
				else if(m_pGameInstance->Key_Down(DIK_RETURN))
				{
					//시작 애니메이션 끝부분  오공 끝 370
					m_bNormalGoku = false;
					Set_CurrentAnimationPositionJump(369.99f);
				}
			}
			else if (m_pModelCom->m_iCurrentAnimationIndex == ANIME_GOKU_CINEMATIC_01)
			{
				if (m_bMotionPlaying == false)
				{
					Set_AnimationMoveXZ(false);
					CBattleInterface_Manager::Get_Instance()->Character_Opening_EndForCharacter(m_iPlayerTeam);
				}
				else if (m_pGameInstance->Key_Down(DIK_RETURN))
				{
					//시작 애니메이션 끝부분  오공 끝 1170
					m_bNormalGoku = false;
					Set_CurrentAnimationPositionJump(1169.99f);
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
				//이쪽으로 접근 하지 않는다?
				if (m_bStun == false)
				{
					m_fAccChaseTime = 0.f;

					//상대의 높이가 높으면 더 위로
					//if (m_pModelCom->m_fCurrentAnimPosition > 60)
					//{
					//
					//	m_fGravityTime = 0.05f;
					//}
					//else
					//	m_fGravityTime = 0.185f;


					m_fGravityTime = 0.185;// -(m_pModelCom->m_fCurrentAnimPosition) * 0.01;


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
		{
			

			Character_Play_Animation(fTimeDelta);
			
			

		 }
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

		//여기로 옮기면 2단점프가 안됨
		//AttckCancleJump();
		//Move(fTimeDelta);
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


	//Animation Lock 상태에서도 움직이는 문제 있음

	if(m_bAnimationLock == false)
		Move(fTimeDelta);











	m_pColliderCom->Update(m_pTransformCom->Get_State(CTransform::STATE_POSITION));



	
	if (Check_bWall())
	{
		Move_ForWall();
	}


	Tag_KeyCheck();


	
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
	
	if (m_pGameInstance->Key_Down(DIK_INSERT))
	{
		CBattleInterface_Manager::Get_Instance()->Gain_KiGuage(50, m_iPlayerTeam);
	}
}

void CPlay_Goku::Camera_Update(_float fTimeDelta)
{
	
}

void CPlay_Goku::Update(_float fTimeDelta)
{
	__super::Player_Update(fTimeDelta);
}

void CPlay_Goku::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
	m_pRenderInstance->Add_RenderObject(CRenderer::RG_SHADOWOBJ, this);
}

HRESULT CPlay_Goku::Render(_float fTimeDelta)
{
	if (m_bInvisible == true)
		return S_OK;


	if (m_bAlwaysss3Test)
		Set_bAura(true);

	if(m_bForcedAura)
		Set_bAura(true);

	

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
		_uint iPassIndex = { 0 };

		if (m_bNormalGoku)
		{
			// 0: 쓰레기
			// 1 :기본 몸 
			// 2: 큐브?
			// 3: decal  제외
			// 4 : decal? 등딱지
			if (i == 0 || i == 2 || i == 3)
				continue;
			if (i == 4)
				iPassIndex = 4;
		}
		else
		{
			if (i == 1 || i == 2 || i == 4)
				continue;
			if (i == 3)
				iPassIndex = 4;
		}

		/* 모델이 가지고 있는 머테리얼 중 i번째 메시가 사용해야하는 머테리얼구조체의 aiTextureType_DIFFUSE번째 텍스쳐를 */
		/* m_pShaderCom에 있는 g_DiffuseTexture변수에 던져. */
		
	

		if (i == 0)
		{
			if (FAILED(m_p2PTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", 0)))
				return E_FAIL;
		}
		else
		{
			if (m_bNormalGoku)
			{
				
				if (FAILED(m_pOpeningTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", 0)))
					return E_FAIL;
				if (FAILED(m_pOpeningDecalTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DecalTexture", 0)))
					return E_FAIL;
				if (FAILED(m_pOpeningOutLineTextureCom->Bind_ShaderResource(m_pShaderCom, "g_OutLineTexture", 0)))
					return E_FAIL;
				
			}
			else
			{
				if (FAILED(m_p2PTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", 0)))
					return E_FAIL;
				/* 오프닝 관련 패스 인덱스 다 던져주기 */
				//m_pOpeningOutLineTextureCom
				if (FAILED(m_pDecalTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DecalTexture", 0)))
					return E_FAIL;
				
			}
		}

	   /* 모델이 가지고 있는 뼈들 중에서 현재 렌더링할려고 했던 i번째ㅑ 메시가 사용하는 뼈들을 배열로 만들어서 쉐이더로 던져준다.  */
		m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i);

		if (FAILED(m_pShaderCom->Begin(iPassIndex)))
			return E_FAIL;

		//이게 왜 됨?
		if (m_bNormalGoku)
		{
			if (FAILED(m_pModelCom_Opening->Render(i)))
				return E_FAIL;
		}
		else if (m_bAlwaysss3Test)
		{
			if (FAILED(m_pModelCom_Skill->Render(i)))
				return E_FAIL;
		}
		else if (m_bFinalSkillss3 == false)
		{
			if (FAILED(m_pModelCom->Render(i)))
				return E_FAIL;
		}
		else
		{
			if (FAILED(m_pModelCom_Skill->Render(i)))
				return E_FAIL;
		}
	}

#ifdef _DEBUG
	m_pColliderCom->Render(fTimeDelta);
#endif // DEBUG

	return S_OK;
}

HRESULT CPlay_Goku::Shadow_Render(_float fTimeDelta)
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_ShadowTransform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_ShadowTransform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	//if (FAILED(m_pModelCom->Bind_MaterialSRV(m_pShaderCom, aiTextureType_DIFFUSE, "g_DiffuseTexture", 2)))
	//	return E_FAIL;

	if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", 2)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(2)))
		return E_FAIL;

	if (FAILED(m_pModelCom->Render(2)))
		return E_FAIL;

	return S_OK;
}



//공용 처리 가능해서 Character로 이사가고 백업.
/*
void CPlay_Goku::AttackNextMoveCheck()
{

	if (m_iNextAnimation.first != m_iIdleAnimationIndex)
	{
		Set_Animation(m_iNextAnimation.first);

		m_iNextAnimation.first = m_iIdleAnimationIndex;
		m_iNextAnimation.second = 1000.f;

		if (m_fNextAnimationCurrentPosition != 0)
		{
			m_pModelCom->CurrentAnimationPositionJump(m_fNextAnimationCurrentPosition);
			m_fNextAnimationCurrentPosition = 0.f;
		}
	}


}

void CPlay_Goku::AnimeEndNextMoveCheck()
{

	Set_Animation(m_iNextAnimation.first);

	m_iNextAnimation.first = ANIME_IDLE;
	m_iNextAnimation.second = 1000.f;


}



void CPlay_Goku::Set_Animation(_uint iAnimationIndex, _bool bloof)
{

	if (iAnimationIndex == m_iIdleAnimationIndex)
		m_pModelCom->SetUp_Animation(iAnimationIndex, true);
	else
		m_pModelCom->SetUp_Animation(iAnimationIndex, bloof);

}

*/


void CPlay_Goku::KeyTest()
{

}



HRESULT CPlay_Goku::Ready_Components()
{
	/* Com_Shader */
	//if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
	//	TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
	//	return E_FAIL;
	//
	///* Com_Model */
	//if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_untitled"),
	//	TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
	//	return E_FAIL;


	CBounding_AABB::BOUNDING_AABB_DESC ColliderDesc{};
	if (m_iPlayerTeam == 1)
		ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_1P_BODY;
	else
		ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_2P_BODY;
	ColliderDesc.pMineGameObject = this;
	ColliderDesc.vCenter = { 0.f,0.8f,0.f };
	ColliderDesc.vExtents = { 0.5f,0.7f,1.f };


	//Com_Collider
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_AABB"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
		return E_FAIL;

	m_pGameInstance->Add_ColliderObject(ColliderDesc.colliderGroup, m_pColliderCom);


	/* Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Play_Goku"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Play_Goku_Final"),
		TEXT("Com_Model_Sub"), reinterpret_cast<CComponent**>(&m_pModelCom_Skill))))
		return E_FAIL;


	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Play_Goku_Opening"),
		TEXT("Com_Model_Sub2"), reinterpret_cast<CComponent**>(&m_pModelCom_Opening))))
		return E_FAIL;


	
	//if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_BlackGoku"), 
	//	TEXT("Com_Model_Black"), reinterpret_cast<CComponent**>(&m_pBlackGokuModelCom))))
	//	return E_FAIL;
	
	/* Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_OutLine"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pOutLineCom))))
		return E_FAIL;

	if (m_iPlayerTeam == 1)
	{
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_GKS_base"),
			TEXT("Com_1PTexture"), reinterpret_cast<CComponent**>(&m_p2PTextureCom))))
			return E_FAIL;

		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_GKN_base_1P"),
			TEXT("Com_Opening_Texture"), reinterpret_cast<CComponent**>(&m_pOpeningTextureCom))))
			return E_FAIL;
		
	}
	else
	{
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_GKS_base"),
			TEXT("Com_2PTexture"), reinterpret_cast<CComponent**>(&m_p2PTextureCom))))
			return E_FAIL;

		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_GKN_base_1P"),
			TEXT("Com_Opening_Texture"), reinterpret_cast<CComponent**>(&m_pOpeningTextureCom))))
			return E_FAIL;

	}	

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_GKS_decal"), 
		TEXT("Com_DecalTexture"), reinterpret_cast<CComponent**>(&m_pDecalTextureCom))))
		return E_FAIL;
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_GKN_decal"),
		TEXT("Com_OpeningDecalTexture"), reinterpret_cast<CComponent**>(&m_pOpeningDecalTextureCom))))
		return E_FAIL;
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_GKN_ilm"),
		TEXT("Com_OpeningOutLineTexture"), reinterpret_cast<CComponent**>(&m_pOpeningOutLineTextureCom))))
		return E_FAIL;
	
	/*

	
	*/

	return S_OK;
}

HRESULT CPlay_Goku::Bind_ShaderResources()
{

	if (FAILED(__super::Bind_ShaderResources()))
		return E_FAIL;

	return S_OK;
}

_bool CPlay_Goku::Check_bCurAnimationisGroundMove(_uint iAnimation)
{
	_uint iModelIndex = iAnimation;

	if (iAnimation == 1000)
		iModelIndex = m_pModelCom->m_iCurrentAnimationIndex;



	if (iModelIndex == ANIME_IDLE || iModelIndex == ANIME_FORWARD_WALK || iModelIndex == ANIME_BACK_WALK || iModelIndex == ANIME_CROUCH_START || iModelIndex == ANIME_CROUCHING
		|| iModelIndex == ANIME_FORWARD_DASH)
	{
		return true;
	}

	return false;
}

_bool CPlay_Goku::Check_bCurAnimationisAttack(_uint iAnimation)
{

	return false;
}

_bool CPlay_Goku::Check_bCurAnimationisAirAttack(_uint iAnimation)
{
	_uint iModelIndex = iAnimation;

	if (iAnimation == 1000)
		iModelIndex = m_pModelCom->m_iCurrentAnimationIndex;



	if (iModelIndex == ANIME_ATTACK_AIR1 || iModelIndex == ANIME_ATTACK_AIR2 || iModelIndex == ANIME_ATTACK_AIR3 || iModelIndex == ANIME_ATTACK_SPECIAL_AIR)
	{
		return true;
	}

	return false;
}

_bool CPlay_Goku::Check_bCurAnimationisHalfGravityStop(_uint iAnimation)
{
	_uint iModelIndex = iAnimation;

	if (iAnimation == 1000)
		iModelIndex = m_pModelCom->m_iCurrentAnimationIndex;



	if (iModelIndex == ANIME_ATTACK_SPECIAL_AIR)
	{
		return true;
	}

	return false;
}

_short CPlay_Goku::Check_bCurAnimationisCanChase()
{
	_uint 	iModelIndex = m_pModelCom->m_iCurrentAnimationIndex;



	//공격중 체이스는 속도 좀 빠르게
	if(m_bAttackBackEvent && (iModelIndex == ANIME_ATTACK_HEAVY || iModelIndex == ANIME_ATTACK_CROUCH_HEAVY || iModelIndex == ANIME_ATTACK_UPPER_AIR ||
		iModelIndex == ANIME_ATTACK_SPECIAL_AIR || iModelIndex == ANIME_ATTACK_LIGHT3))
	{
		return 10;
	}


	else if (Check_bCurAnimationisGroundMove() || Check_bCurAnimationisAirMove())
	{
		return 1;
	}


	return 0;

}

void CPlay_Goku::Reset_AttackCount()
{

	__super::Reset_AttackCount();

	for (size_t i = 0; i < COUNT_END; i++)
		m_bAttackCount[i] = true;

	m_iCountGroundSpecial = 0;

}

void CPlay_Goku::Gravity(_float fTimeDelta)
{

	if (m_bAttackGravity && m_pModelCom->m_iCurrentAnimationIndex == ANIME_ATTACK_236_AIR)
	{
		if (Get_fHeight() > 0)
		{

		
			_float fGravity = (-0.7f * (2 * m_fGravityTime - m_fJumpPower) * (2 * m_fGravityTime - m_fJumpPower) + 4) * 0.1;

			Add_Move({ m_fImpuse.x *fTimeDelta, -fGravity });
		}

		if (m_fGravityTime * 2.f < m_fJumpPower)
		{
			m_fGravityTime += fTimeDelta;

			//일시정지해서 여기서 넘어가면 값이 확 높아질 수 있음.  예외처리해서 시간값 한번 더?
		}

	}

	
	__super::Gravity(fTimeDelta);


	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_float fHeight = XMVectorGetY(vPos);
	_ushort iAnimationIndex = m_pModelCom->m_iCurrentAnimationIndex;


	// iAnimationIndex == ANIME_ATTACK_236_SPECIAL_AIR || iAnimationIndex == ANIME_ATTACK_236_SPECIAL_AIR_DOWNSIDE || 


	if (iAnimationIndex == ANIME_ATTACK_236_AIR || iAnimationIndex == ANIME_ATTACK_AIR1 || iAnimationIndex == ANIME_ATTACK_AIR2 || iAnimationIndex == ANIME_ATTACK_AIR3 ||
		iAnimationIndex == ANIME_ATTACK_SPECIAL_AIR)
	{

	


		if (fHeight == 0)
		{

			//m_pModelCom->SetUp_Animation(m_iIdleAnimationIndex, true);
			Set_Animation(m_iIdleAnimationIndex);


			Set_fGravityTime(0.f);
			Set_fJumpPower(0.f);
			Set_fImpulse(0.f);
			m_bAriDashEnable = true;
			Set_bAttackGravity(true);

			//m_pGameInstance->Play_Sound(CSound_Manager::SOUND_KEY_NAME::)
		}

	}



}

void CPlay_Goku::AttackEvent(_int iAttackEvent, _int AddEvent)
{

	switch (m_pModelCom->m_iCurrentAnimationIndex)
	{
	case Client::CPlay_Goku::ANIME_ATTACK_LIGHT1:
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
		Desc.iDamage = 400 * Get_DamageScale();
		Desc.fLifeTime = 0.1f;
		Desc.ihitCharacter_Motion = { HitMotion::HIT_LIGHT };
		Desc.iTeam = m_iPlayerTeam;
		Desc.fAnimationLockTime = 0.1f;
		Desc.pOwner = this;

		m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack"), TEXT("Layer_AttackObject"), &Desc);
	}
	break;

	case Client::CPlay_Goku::ANIME_ATTACK_LIGHT2:
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
		Desc.fhitCharacter_StunTime = 0.15f;


		Desc.iDamage = 700 * Get_DamageScale();
		//Desc.iDamage = 4200 * Get_DamageScale();
		Desc.fLifeTime = 0.1f;
		Desc.ihitCharacter_Motion = { HitMotion::HIT_LIGHT };
		Desc.iTeam = m_iPlayerTeam;
		Desc.fAnimationLockTime = 0.1f;
		Desc.pOwner = this;
		Desc.iGainKiAmount = 7;

		m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack"), TEXT("Layer_AttackObject"), &Desc);


		//
		// 
		// 
		// 
		// 
		// 
		// ->Copy_Layer(TEXT("Smoke03_Stop"), m_pTransformCom->Get_WorldMatrixPtr());
		Character_Make_Effect(TEXT("Smoke03_Stop"), {}, true);
		
	}
	break;
	case Client::CPlay_Goku::ANIME_ATTACK_LIGHT3:
	{

		CAttackObject::ATTACK_DESC Desc{};

		if (m_iPlayerTeam == 1)
			Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_1P_Melee_Attack;
		else
			Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_2P_Melee_Attack;
		Desc.ColliderDesc.pMineGameObject = this;
		Desc.ColliderDesc.vCenter = { 0.9f * m_iLookDirection,0.8f,0.f };
		Desc.ColliderDesc.vExtents = { 0.3f,0.5f,0.2f };



		//Desc.fhitCharacter_Impus = { 6.f * m_iLookDirection,5.f };
		//Desc.fhitCharacter_Impus = { 6.f * m_iLookDirection,2.f };

		//Desc.fhitCharacter_Impus = { 5.f * m_iLookDirection,3.f };
		//Desc.fhitCharacter_StunTime = 0.6f;

		Desc.fhitCharacter_Impus = { 4.f * m_iLookDirection,2.f };
		//Desc.fhitCharacter_StunTime = 0.6f;
		Desc.fhitCharacter_StunTime = 0.8f;  //교대까지 가능하려면 0.8초


		Desc.iDamage = 1000 * Get_DamageScale();
		Desc.fLifeTime = 0.1f;
		Desc.ihitCharacter_Motion = { HitMotion::HIT_SPIN_AWAY_LEFTUP };
		Desc.iTeam = m_iPlayerTeam;
		Desc.fAnimationLockTime = 0.7f;
		Desc.pOwner = this;
		Desc.iGainKiAmount = 10;

		m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack"), TEXT("Layer_AttackObject"), &Desc);

		//CEffect_Manager::Get_Instance()->Copy_Layer(TEXT("Kick_Wind"), m_pTransformCom->Get_WorldMatrixPtr());
		Character_Make_Effect(TEXT("Kick_Wind"));

	}
	break;
	case Client::CPlay_Goku::ANIME_ATTACK_MEDIUM:
	{
		CAttackObject::ATTACK_DESC Desc{};
		//Desc.ColliderDesc.width = 0.7;
		//Desc.ColliderDesc.height = 0.8;
		if (m_iPlayerTeam == 1)
			Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_1P_Melee_Attack;
		else
			Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_2P_Melee_Attack;
		Desc.ColliderDesc.pMineGameObject = this;
		Desc.ColliderDesc.vExtents = { 0.4f,0.6f,1.f };
		Desc.ColliderDesc.vCenter = { 0.5f * m_iLookDirection,0.7f,0.f };
		Desc.fhitCharacter_Impus = { 0.3f * m_iLookDirection,0 };
		Desc.fhitCharacter_StunTime = 0.4f;
		//Desc.fhitCharacter_StunTime = 0.55f;

		Desc.iDamage = 700 * Get_DamageScale();;
		Desc.fLifeTime = 0.15f;
		Desc.ihitCharacter_Motion = { HitMotion::HIT_MEDIUM };
		Desc.iTeam = m_iPlayerTeam;
		Desc.fAnimationLockTime = 0.1f;
		Desc.pOwner = this;
		Desc.iGainKiAmount = 7;

		m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack"), TEXT("Layer_AttackObject"), &Desc);
	}
	break;
	case Client::CPlay_Goku::ANIME_ATTACK_HEAVY:
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
		///Desc.iDamage = 30000 * Get_DamageScale();;
		Desc.fLifeTime = 0.2f;
		Desc.ihitCharacter_Motion = { HitMotion::HIT_KNOCK_AWAY_LEFT };
		Desc.iTeam = m_iPlayerTeam;
		Desc.fAnimationLockTime = 0.7f;
		Desc.bGrabbedEnd = true;
		Desc.pOwner = this;
		Desc.iGainKiAmount = 10;

		//Desc.bCameraZoom = false;

		m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack"), TEXT("Layer_AttackObject"), &Desc);


		Character_Make_Effect(TEXT("Ring_Dust_Right"), { 0.7f,0.f });

		//Character_Make_Effect(TEXT("Ring_Dust_Right"), { 0.4f,0.f });
	
		

		//Ring_Dust

	}
	break;
	case Client::CPlay_Goku::ANIME_ATTACK_SPECIAL:
	//if(iAttackEvent == 0)
	{
		//CAttackObject::ATTACK_DESC Desc{};

		CAttackObject_Ranged::ATTACK_RANGED_DESC Desc{};
		//Desc.ColliderDesc.width = 1.0;
		//Desc.ColliderDesc.height = 1.0;
		if (m_iPlayerTeam == 1)
			Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_1P_Ranged_Attack;
		else
			Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_2P_Ranged_Attack;
		Desc.ColliderDesc.pMineGameObject = this;
		Desc.ColliderDesc.vExtents = { 0.2f,0.2f,1.f };
		Desc.ColliderDesc.vCenter = { 0.f,0.f,0.f };


		Desc.fhitCharacter_Impus = { 0.3f * m_iLookDirection,0 };
		Desc.fhitCharacter_StunTime = 0.35f;
		Desc.iDamage = 300 * Get_DamageScale();;
		Desc.fLifeTime = 5.f;




		if (m_iCountGroundSpecial >=5)
		{
			Desc.ihitCharacter_Motion = { HitMotion::HIT_SPIN_AWAY_LEFTUP };
			//Desc.fhitCharacter_Impus = { 5.f * m_iLookDirection, 0.3f };
			Desc.fhitCharacter_Impus = { 8.f * m_iLookDirection, 0.3f };
			Desc.fhitCharacter_StunTime = 0.1f;
		}
		else
		{
			Desc.ihitCharacter_Motion = { HitMotion::HIT_LIGHT };
			Desc.fhitCharacter_Impus = { 0.2f * m_iLookDirection,0 };
		}

		Desc.iTeam = m_iPlayerTeam;

		Desc.bCameraZoom = false;
		Desc.fAnimationLockTime = 0.f;

		Desc.pOwner = this;

		//Desc.eAttackType = { ATTACKTYPE_HIGH };
		Desc.fStartOffset = { 0.2f * m_iLookDirection, 0.9f };
		//Desc.fRanged_Impus_NoneDirection = { 9.f,0.f }; 
		Desc.fRanged_Impus_NoneDirection = { 15.f,0.f };

		Desc.iDirection = m_iLookDirection;
		Desc.eRangeColor = CAttackObject_Ranged::RANGED_LIGHT_YELLOW;
		Desc.strEffectName = TEXT("BurstJ-03");
		//Desc.strEffectName = TEXT("Parrying_Ball");


		Desc.iGainKiAmount = 3;


		m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack_Ranged"), TEXT("Layer_AttackObject"), &Desc);

		//Character_Make_Effect(TEXT("BurstJ-03"));
	}
	//else if (iAttackEvent == 1)
	//{
	//	CAttackObject_Ranged::ATTACK_RANGED_DESC Desc{};
	//	//Desc.ColliderDesc.width = 1.0;
	//	//Desc.ColliderDesc.height = 1.0;
	//	if (m_iPlayerTeam == 1)
	//		Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_1P_Ranged_Attack;
	//	else
	//		Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_2P_Ranged_Attack;
	//	Desc.ColliderDesc.pMineGameObject = this;
	//	Desc.ColliderDesc.vExtents = { 0.2f,0.2f,1.f };
	//	Desc.ColliderDesc.vCenter = { 0.f,0.f,0.f };
	//
	//
	//	Desc.fhitCharacter_Impus = { 0.3f * m_iLookDirection,0 };
	//	Desc.fhitCharacter_StunTime = 0.5f;
	//	Desc.iDamage = 300 * Get_DamageScale();;
	//	Desc.fLifeTime = 5.f;
	//	Desc.ihitCharacter_Motion = { HitMotion::HIT_KNOCK_AWAY_LEFT };
	//
	//	Desc.iTeam = m_iPlayerTeam;
	//
	//	Desc.bCameraZoom = false;
	//	Desc.fAnimationLockTime = 0.f;
	//
	//	Desc.pOwner = this;
	//
	//	//Desc.eAttackType = { ATTACKTYPE_HIGH };
	//	Desc.fStartOffset = { 0.2f * m_iLookDirection, 0.9f };
	//	Desc.fRanged_Impus_NoneDirection = { 6.f,0.f };  // 1:1.569 비율
	//
	//	Desc.iDirection = m_iLookDirection;
	//
	//	m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack_Ranged"), TEXT("Layer_AttackObject"), &Desc);
	//}
		break;
	case Client::CPlay_Goku::ANIME_ATTACK_AIR1:
	{

		CAttackObject::ATTACK_DESC Desc{};
		//Desc.ColliderDesc.width = 0.7;
		//Desc.ColliderDesc.height = 0.8;
		if (m_iPlayerTeam == 1)
			Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_1P_Melee_Attack;
		else
			Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_2P_Melee_Attack;
		Desc.ColliderDesc.pMineGameObject = this;
		
		if(Get_fHeight()>3)  //공중에선 더 맞추기 쉽도록 보정
		{
			Desc.ColliderDesc.vExtents = { 0.7f,0.8f,1.f };
			Desc.ColliderDesc.vCenter = { 0.9f * m_iLookDirection,0.8f,0.f };
		}
		else
		{
			Desc.ColliderDesc.vExtents = { 0.4f,0.4f,1.f };
			Desc.ColliderDesc.vCenter = { 0.9f * m_iLookDirection,1.0f,0.f };
		}

		//Desc.fhitCharacter_Impus = { m_fImpuse.x * 0.9f,0 };
		 
		
		Desc.fhitCharacter_Impus = { m_fImpuse.x,0 };

		//if (abs(m_fImpuse.x) < 0.3)
		//	Desc.fhitCharacter_Impus = { m_fImpuse.x * m_iLookDirection,0 };
		//else
		//	Desc.fhitCharacter_Impus = { 0.3f * m_iLookDirection,0 };

		Desc.fhitCharacter_StunTime = 0.6f;
		Desc.iDamage = 400 * Get_DamageScale();
		Desc.fLifeTime = 0.1f;
		Desc.ihitCharacter_Motion = { HIT_LIGHT };
		Desc.iTeam = m_iPlayerTeam;
		//Desc.fAnimationLockTime = 0.04f;
		Desc.fAnimationLockTime = 0.07f;

		//Desc.bOwnerGravityTimeReset = true;
		Desc.pOwner = this;
		Desc.eAttackType = { ATTACKTYPE_MIDDLE };

		m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack"), TEXT("Layer_AttackObject"), &Desc);

		m_pGameInstance->Play_Group_Sound(CSound_Manager::SOUND_GROUP_KEY::LIGHT_ATTACK_Goku, false, 1.f);
		m_pGameInstance->Play_Group_Sound(CSound_Manager::SOUND_GROUP_KEY::LIGHT_ATTACK_Hit_SFX, false, 1.f);
	}
	break;
	case Client::CPlay_Goku::ANIME_ATTACK_AIR2:
	{
		//Position 31 에서 공격
		//0에서 PLAY_GOKU,GKS261_body,0,AnimSpeedChange,0.025000,120


		if (iAttackEvent == 0)
		{
			m_bBenishingAttack = false;
		}

		else if(iAttackEvent == 1)
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
				if (Get_fHeight() > 3)  //공중에선 더 맞추기 쉽도록 보정
				{
					Desc.ColliderDesc.vExtents = { 0.7f,0.8f,1.f };
					Desc.ColliderDesc.vCenter = { 0.9f * m_iLookDirection,0.8f,0.f };
				}
				else
				{
					Desc.ColliderDesc.vExtents = { 0.6f,0.5f,1.f };
					Desc.ColliderDesc.vCenter = { 0.9f * m_iLookDirection,1.0f,0.f };
				}
				//Desc.ColliderDesc.pTransform = m_pTransformCom;
				//Desc.fhitCharacter_Impus = { 0.3f * m_iLookDirection,0 };

				Desc.fhitCharacter_Impus = { m_fImpuse.x  ,0 };

				//if (abs(m_fImpuse.x) < 0.3)
				//	Desc.fhitCharacter_Impus = { m_fImpuse.x * m_iLookDirection,0 };
				//else
				//	Desc.fhitCharacter_Impus = { 0.3f * m_iLookDirection,0 };

				Desc.fhitCharacter_StunTime = 0.4f;

				if (Get_fHeight() > 2)
					Desc.fhitCharacter_StunTime = 0.6f;


				Desc.iDamage = 700 * Get_DamageScale();
				Desc.fLifeTime = 0.1f;
				Desc.ihitCharacter_Motion = { HitMotion::HIT_LIGHT };
				Desc.iTeam = m_iPlayerTeam;
				//Desc.fAnimationLockTime = 0.04f;
				Desc.fAnimationLockTime = 0.07f;

				//Desc.bOwnerGravityTimeReset = true;
				Desc.pOwner = this;
				Desc.eAttackType = { ATTACKTYPE_MIDDLE };
				Desc.iGainKiAmount = 7;


				m_pGameInstance->Play_Group_Sound(CSound_Manager::SOUND_GROUP_KEY::LIGHT_ATTACK_Goku, false, 1.f);
				m_pGameInstance->Play_Group_Sound(CSound_Manager::SOUND_GROUP_KEY::LIGHT_ATTACK_Hit_SFX, false, 1.f);

				m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack"), TEXT("Layer_AttackObject"), &Desc);
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
				Desc.ColliderDesc.vCenter = { 0.7f * m_iLookDirection,0.8f,0.f };

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
				Desc.iGainKiAmount = 0;

				Desc.bCameraZoom = false;

				m_pGameInstance->Play_Group_Sound(CSound_Manager::SOUND_GROUP_KEY::LIGHT_ATTACK_Goku, false, 1.f);
				m_pGameInstance->Play_Group_Sound(CSound_Manager::SOUND_GROUP_KEY::LIGHT_ATTACK_Hit_SFX, false, 1.f);

				m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack"), TEXT("Layer_AttackObject"), &Desc);

				
				Character_Create_Distortion({ 1.f,0.f,0.f }, { 0.f,0.f }, { 1.f,1.f },0.2f);

			}
		}
	}
	break;
	case Client::CPlay_Goku::ANIME_ATTACK_AIR3:
	{
		CAttackObject::ATTACK_DESC Desc{};
		//Desc.ColliderDesc.width = 1.0;
		//Desc.ColliderDesc.height = 1.0;
		if (m_iPlayerTeam == 1)
			Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_1P_Melee_Attack;
		else
			Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_2P_Melee_Attack;
		Desc.ColliderDesc.pMineGameObject = this;
		
		if (Get_fHeight() > 3)  //공중에선 더 맞추기 쉽도록 보정
		{
			Desc.ColliderDesc.vExtents = { 1.f,1.f,1.f };
			Desc.ColliderDesc.vCenter = { 1.0f * m_iLookDirection,0.8f,0.f };
		}
		else
		{
			Desc.ColliderDesc.vExtents = { 0.4f,0.8f,1.f };
			Desc.ColliderDesc.vCenter = { 0.7f * m_iLookDirection,0.8f,0.f };
		}
		//Desc.ColliderDesc.pTransform = m_pTransformCom;
		Desc.fhitCharacter_Impus = { 3.f * m_iLookDirection,-20.f };
		Desc.fhitCharacter_StunTime = 0.9f;
		Desc.iDamage = 850 * Get_DamageScale();;
		Desc.fLifeTime = 0.2f;
		//Desc.ihitCharacter_Motion = { HitMotion::HIT_KNOCK_AWAY_LEFT };
		Desc.ihitCharacter_Motion = { HitMotion::HIT_KNOCK_AWAY_LEFTDOWN };

		Desc.iTeam = m_iPlayerTeam;


		//높이가 5 이상이면 내려찍히는 판정.  디폴트는 false이므로  else 처리 안함
		//if(Get_fHeight() > 5)

		if (m_bGrab_Air)
		{
			Desc.bGroundSmash = true;
			Desc.fAnimationLockTime = 0.7f;
			m_bGrab_Air = false;

		}
		else if (Get_fHeight() > 3)
		{
			Desc.bGroundSmash = true;
			Desc.fAnimationLockTime = 0.7f;
		}
		else
		{
			Desc.bCameraZoom = false;
			Desc.fAnimationLockTime = 0.1f;
		}

		Desc.bGrabbedEnd = true;
		Desc.pOwner = this;
		Desc.eAttackType = { ATTACKTYPE_MIDDLE };
		Desc.iGainKiAmount = 10;

		m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack"), TEXT("Layer_AttackObject"), &Desc);
	}
	break;
	case Client::CPlay_Goku::ANIME_ATTACK_SPECIAL_AIR:
	{
		if (iAttackEvent == 0)
		{
			//{		
			//	CBoneEffectObject::BONE_EFFECT_DESC EffectDesc;
			//	EffectDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
			//	EffectDesc.pSocketBoneMatrix = m_pModelCom->Get_BoneMatrixPtr("GD_fist_R");
			//	//EffectDesc.strEffectName = TEXT("Smoke05");
			//	EffectDesc.strEffectName = TEXT("BurstU-2");
			//
			//
			//	//m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Bone_Effect_Object"), TEXT("Layer_BoneEffectObject"), &EffectDesc);
			//
			//	m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Bone_Effect_Object"), TEXT("Layer_BoneEffectObject"), &EffectDesc);
			//	
			//
			//}

			//Character_Make_BoneEffect("GD_fist_R", TEXT("BurstU-3_02"));jjwj
			Character_Make_BoneEffect("GD_fist_R", TEXT("BurstU-3_01"));

			//BurstU-3_02

		}

		if(iAttackEvent == 1) //공격
		{
			CAttackObject_Ranged::ATTACK_RANGED_DESC Desc{};
			//Desc.ColliderDesc.width = 1.0;
			//Desc.ColliderDesc.height = 1.0;
			if (m_iPlayerTeam == 1)
				Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_1P_Ranged_Attack;
			else
				Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_2P_Ranged_Attack;
			Desc.ColliderDesc.pMineGameObject = this;
			Desc.ColliderDesc.vExtents = { 0.5f,0.5f,1.f };
			Desc.ColliderDesc.vCenter = { 0.f,0.f,0.f };

			Desc.fhitCharacter_Impus = { 3.f * m_iLookDirection,2.f };
			Desc.fhitCharacter_StunTime = 0.4f;

			if (Get_fHeight() > 3)
				Desc.fhitCharacter_StunTime = 0.8f;

			Desc.iDamage = 600 * Get_DamageScale();;
			Desc.fLifeTime = 5.f;
			Desc.ihitCharacter_Motion = { HitMotion::HIT_LIGHT };

			Desc.iTeam = m_iPlayerTeam;

			Desc.bCameraZoom = false;
			Desc.fAnimationLockTime = 0.05f;

			Desc.pOwner = this;

			//Desc.eAttackType = { ATTACKTYPE_HIGH };
			Desc.fStartOffset = { 0.6f * m_iLookDirection, 0.f };
			Desc.fRanged_Impus_NoneDirection = { 7.845f,-5.f };  // 1:1.569 비율

			Desc.iDirection = m_iLookDirection;

			Desc.eRangeColor = CAttackObject_Ranged::RANGED_LIGHT_YELLOW;
			Desc.strEffectName = TEXT("BurstJ-03_Rotated_Left");
			Desc.iGainKiAmount = 7;

			Desc.bOnwerHitNoneStop = true;
			Desc.fEffectRotationDegree = 340.f;

			m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack_Ranged"), TEXT("Layer_AttackObject"), &Desc);
		}

		

	}
	break;
	case Client::CPlay_Goku::ANIME_ATTACK_CROUCH_LIGHT:
	{

		m_pGameInstance->Play_Group_Sound(CSound_Manager::SOUND_GROUP_KEY::LIGHT_ATTACK_Goku, false, 1.f);
		m_pGameInstance->Play_Group_Sound(CSound_Manager::SOUND_GROUP_KEY::LIGHT_ATTACK_Hit_SFX, false, 1.f);

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
		{
			{
				//CBoneEffectObject::BONE_EFFECT_DESC EffectDesc;
				//EffectDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
				//EffectDesc.pSocketBoneMatrix = m_pModelCom->Get_BoneMatrixPtr("GD_fist_R");
				//
				//EffectDesc.strEffectName = TEXT("BurstU-2");
				//
				//m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Bone_Effect_Object"), TEXT("Layer_BoneEffectObject"), &EffectDesc);


				


				//Body 기준으로 Offset, 좌우반전 주고 이펙트 생성.  성공
				//Character_Make_BoneEffect("GD_fist_R", TEXT("BurstU-2"), { 0.f,-0.5f });
				
				
				
				//뼈에 이펙트 붙히고 Offset주기. 성공
				//Character_Make_BoneEffect_Offset("GD_fist_R", TEXT("BurstU-2"), { 0.f,-0.3f });
				
				
				//뼈에 이펙트 붙히고 Offset,좌우반전 테스트.  성공
				//Character_Make_BoneEffect_Offset("GD_fist_R", TE
			}

		}
	}
	break;
	case Client::CPlay_Goku::ANIME_ATTACK_CROUCH_MEDUIM:
	{

		//0 : 공격 전 연기  1: 공격판정생성 2: 공격 직후 연기
		if (iAttackEvent == 0)
		{
			Character_Make_Effect(TEXT("Smoke03"));
		}
		else if (iAttackEvent == 2)
		{
			Character_Make_Effect(TEXT("Smoke03_Stop"));
		}

		if(iAttackEvent == 1)
		{
			CAttackObject::ATTACK_DESC Desc{};
			//Desc.ColliderDesc.width = 0.7;
			//Desc.ColliderDesc.height = 0.8;
			if (m_iPlayerTeam == 1)
				Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_1P_Melee_Attack;
			else
				Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_2P_Melee_Attack;
			Desc.ColliderDesc.pMineGameObject = this;
			Desc.ColliderDesc.vCenter = { 0.9f * m_iLookDirection,0.8f,0.f };
			//Desc.ColliderDesc.pTransform = m_pTransformCom;
			//Desc.fhitCharacter_Impus = { 1.f * m_iLookDirection,0.3f };
			//Desc.fhitCharacter_Impus = { 1.5f * m_iLookDirection,0.3f };
			//Desc.ColliderDesc.vExtents = { 0.7f,0.8f,1.f };

			Desc.ColliderDesc.vExtents = { 0.7f,0.5f,1.f };
			Desc.ColliderDesc.vCenter = { 0.7f * m_iLookDirection,0.3f,0.f };

			Desc.fhitCharacter_StunTime = 1.f;
			Desc.iDamage = 700 * Get_DamageScale();
			Desc.fLifeTime = 0.1f;
			Desc.ihitCharacter_Motion = { HitMotion::HIT_CROUCH_MEDIUM };
			Desc.iTeam = m_iPlayerTeam;
			Desc.fAnimationLockTime = 0.1f;
			//Desc.bOwnerGravityTimeReset = true;
			Desc.pOwner = this;
			Desc.iGainKiAmount = 7;

			Desc.eAttackType = { ATTACKTYPE_LOW };

			m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack"), TEXT("Layer_AttackObject"), &Desc);

			m_pGameInstance->Play_Group_Sound(CSound_Manager::SOUND_GROUP_KEY::LIGHT_ATTACK_Goku, false, 1.f);
			m_pGameInstance->Play_Group_Sound(CSound_Manager::SOUND_GROUP_KEY::LIGHT_ATTACK_Hit_SFX, false, 1.f);
		}
	}
	break;
	case Client::CPlay_Goku::ANIME_ATTACK_CROUCH_HEAVY:
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
		//Desc.fhitCharacter_Impus = { 0.2f * m_iLookDirection, 10.f };  //원래 가속도.
		Desc.fhitCharacter_Impus = { 0.6f * m_iLookDirection, 10.f };  //원래 가속도.

		//Desc.fhitCharacter_Impus = { 0.2f * m_iLookDirection, 3.f };  //공중 테스트용 임시

		Desc.fhitCharacter_StunTime = 1.0f;
		Desc.iDamage = 1000 * Get_DamageScale();;
		Desc.fLifeTime = 0.3f;
		Desc.ihitCharacter_Motion = { HitMotion::HIT_KNOCK_AWAY_UP };
		Desc.iTeam = m_iPlayerTeam;
		Desc.fAnimationLockTime = 0.5f;
		Desc.pOwner = this;
		Desc.iGainKiAmount = 10;

		m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack"), TEXT("Layer_AttackObject"), &Desc);


		//Character_Make_Effect(TEXT("Ring_Dust"), { m_iLookDirection * 0.7f,0.5f });
		//Character_Make_Effect(TEXT("Ring_Dust"), { m_iLookDirection * 0.4f,0.9f });
		Character_Make_Effect(TEXT("Ring_Dust"), {0.4f,0.9f });

		m_pGameInstance->Play_Sound(CSound_Manager::SOUND_KEY_NAME::Goku_Heavy_Attack, false, 1.f);
	}
	break;
	case Client::CPlay_Goku::ANIME_ATTACK_UPPER_AIR:
	{
		CAttackObject::ATTACK_DESC Desc{};
		//Desc.ColliderDesc.width = 1.0;
		//Desc.ColliderDesc.height = 1.0;
		if (m_iPlayerTeam == 1)
			Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_1P_Melee_Attack;
		else
			Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_2P_Melee_Attack;
		Desc.ColliderDesc.pMineGameObject = this;

		Desc.ColliderDesc.vExtents = { 0.5f,0.7f,1.f };
		Desc.ColliderDesc.vCenter = { 0.7f * m_iLookDirection,0.8f,0.f };

		//Desc.ColliderDesc.pTransform = m_pTransformCom;
		//Desc.fhitCharacter_Impus = { 0.f, 6.f }; //너무 높음
		Desc.fhitCharacter_Impus = { 0.f, 4.f };

		//Desc.fhitCharacter_StunTime = 0.6f;
		Desc.fhitCharacter_StunTime = 0.8f;

		Desc.iDamage = 1000 * Get_DamageScale();;
		Desc.fLifeTime = 0.2f;
		Desc.ihitCharacter_Motion = { HitMotion::HIT_KNOCK_AWAY_UP };
		Desc.iTeam = m_iPlayerTeam;
		Desc.fAnimationLockTime = 0.08f;
		Desc.pOwner = this;

		if (Get_fHeight() > 2)
		{
			Desc.bCameraZoom = false;
			//Desc.ColliderDesc.vCenter = { 1.0f * m_iLookDirection,0.8f,0.f };
			//Desc.ColliderDesc.vCenter = { 1.0f * m_iLookDirection,0.8f,0.f };
			//Desc.fhitCharacter_Impus = { 0.2f * m_iLookDirection, 6.f };
			Desc.fhitCharacter_Impus = { 0.4f * m_iLookDirection, 5.f };

		}
		else
		{
			Desc.fhitCharacter_Impus = { 0.2f * m_iLookDirection, 10.f };  //원래 가속도.
			Desc.bCameraZoom = true;
			Desc.fAnimationLockTime = 0.5f;
		}

		Desc.iGainKiAmount = 10;


		m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack"), TEXT("Layer_AttackObject"), &Desc);





		Character_Make_Effect(TEXT("Ring_Dust"),{m_iLookDirection*1.2f,0.7f});
	}
	break;
	case Client::CPlay_Goku::ANIME_ATTACK_236:
	{
		//강공격 버전용 
		if (iAttackEvent == 0)
		{
		
			if (m_bHeavySkill)
			{
				m_bInvisible = true;
				m_pModelCom->Get_pCurrentAnimation()->m_fTickPerSecond = 25.f;
			}
		}
		//강공격 버전용
		else if (iAttackEvent == 10)
		{
			if (m_bHeavySkill)
			{
				Set_AnimationStop(0.1f);


				m_bInvisible = false;
				m_pModelCom->Get_pCurrentAnimation()->m_fTickPerSecond = 85.6f;
			

				if (abs(m_pEnemy->Get_fPositionX()) < 10)
				{
					Teleport_ToEnemy(2.f, 0.f);
					Add_Move({ 0.f,-Get_fHeight() });

					FlipDirection();
				}
				else
					Teleport_ToEnemy(-2.f, 0.2f);


				Character_Create_Distortion({ 1.f,0.f,0.f, }, { 0.f,0.f });
				CEffect_Layer::COPY_DESC tDesc{};
				tDesc.pPlayertMatrix = m_pModelCom->Get_BoneMatrixPtr("G_head");
				tDesc.pTransformCom = m_pTransformCom;
				tDesc.m_isPlayerDirRight = m_iLookDirection;
				m_p236ChaseAura = CEffect_Manager::Get_Instance()->Copy_Layer_AndGet(m_ChaseEffectName, &tDesc);

			}
		}
		
		else if (iAttackEvent == 1)
		{
			CAttackObject::ATTACK_DESC Desc{};
			//Desc.ColliderDesc.width = 1.0;
			//Desc.ColliderDesc.height = 1.3;
			if (m_iPlayerTeam == 1)
				Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_1P_Melee_Attack;
			else
				Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_2P_Melee_Attack;
			Desc.ColliderDesc.pMineGameObject = this;
			//Desc.ColliderDesc.vExtents = { 1.f,1.3f,1.f };
			Desc.ColliderDesc.vExtents = { 0.5f,0.6f,1.f };

			Desc.ColliderDesc.vCenter = { 1.0f * m_iLookDirection,0.7f,0.f };
			//Desc.ColliderDesc.pTransform = m_pTransformCom;
			Desc.fhitCharacter_Impus = { 3.f * m_iLookDirection,-20.f };
			Desc.fhitCharacter_StunTime = 10.0f;
			Desc.iDamage = 1200 * Get_DamageScale();;
			Desc.fLifeTime = 0.2f;
			//Desc.ihitCharacter_Motion = { HitMotion::HIT_KNOCK_AWAY_LEFT };
			Desc.ihitCharacter_Motion = { HitMotion::HIT_HEAVY_DOWN };
			Desc.iTeam = m_iPlayerTeam;
			Desc.bGroundSmash = true;
			Desc.fAnimationLockTime = 0.5f;
			Desc.pOwner = this;
			Desc.eAttackType = { ATTACKTYPE_MIDDLE };

			Desc.iGainKiAmount = 10;

			m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack"), TEXT("Layer_AttackObject"), &Desc);


			if (m_p236ChaseAura != nullptr)
			{
				m_p236ChaseAura->m_bIsDoneAnim = true;
				m_p236ChaseAura = nullptr;
			}
		}
	}
	break;
	case Client::CPlay_Goku::ANIME_ATTACK_236_AIR:
	{

		//강공격 버전용
		if (iAttackEvent == 20)
		{
			//if(m_bHeavySkill)
			//{
			//	Set_AnimationStop(0.1f);
			//	Character_Create_Distortion({ 0.f,1.f,0.f, }, { 0.f,0.f });
			//	Character_Make_Effect(TEXT("DIR_K"));
			//	m_bInvisible = false;
			//}
			//Set_bInivisible(false);

			if (m_bHeavySkill)
			{
				m_bInvisible = true;
				m_pModelCom->Get_pCurrentAnimation()->m_fTickPerSecond = 25.f;
			}

		}
		else if (iAttackEvent == 123)
		{
			if (m_bHeavySkill)
			{
				Set_AnimationStop(0.1f);


				m_bInvisible = false;
				m_pModelCom->Get_pCurrentAnimation()->m_fTickPerSecond = 180.6f;


				if (abs(m_pEnemy->Get_fPositionX()) < 10)
				{
					Teleport_ToEnemy(2.f, 0.2f);

					FlipDirection();
				}
				else
					Teleport_ToEnemy(-2.f, 0.2f);


				Character_Create_Distortion({ 1.f,0.f,0.f, }, { 0.f,0.f });


				//m_p236ChaseAura = Character_Make_BoneEffect("G")

				CEffect_Layer::COPY_DESC tDesc{};
				tDesc.pPlayertMatrix = m_pModelCom->Get_BoneMatrixPtr("G_head");
				tDesc.pTransformCom = m_pTransformCom;
				tDesc.m_isPlayerDirRight = m_iLookDirection;
				m_p236ChaseAura = CEffect_Manager::Get_Instance()->Copy_Layer_AndGet(m_ChaseEffectName, &tDesc);

				m_p236ChaseAura->Set_Copy_Layer_Scaled({ 1.f,0.7f,1.f });

			}
		}
		if(iAttackEvent == 0)
		{

			CAttackObject::ATTACK_DESC Desc{};
			//Desc.ColliderDesc.width = 1.0;
			//Desc.ColliderDesc.height = 1.3;
			if (m_iPlayerTeam == 1)
				Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_1P_Melee_Attack;
			else
				Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_2P_Melee_Attack;
			Desc.ColliderDesc.pMineGameObject = this;
			//Desc.ColliderDesc.vExtents = { 1.f,1.3f,1.f };
			Desc.ColliderDesc.vExtents = { 0.6f,0.7f,1.f };
			Desc.ColliderDesc.vCenter = { 1.0f * m_iLookDirection,0.6f,0.f };
			//Desc.ColliderDesc.pTransform = m_pTransformCom;
			Desc.fhitCharacter_Impus = { 3.f * m_iLookDirection,-20.f };
			Desc.fhitCharacter_StunTime = 2.5f;
			Desc.iDamage = 1200 * Get_DamageScale();;
			Desc.fLifeTime = 0.15f;
			//Desc.ihitCharacter_Motion = { HitMotion::HIT_KNOCK_AWAY_LEFT };
			Desc.ihitCharacter_Motion = { HitMotion::HIT_HEAVY_DOWN };
			Desc.iTeam = m_iPlayerTeam;
			Desc.bGroundSmash = true;
			Desc.fAnimationLockTime = 0.5f;
			Desc.pOwner = this;
			Desc.eAttackType = { ATTACKTYPE_MIDDLE };

			Desc.iGainKiAmount = 10;

			m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack"), TEXT("Layer_AttackObject"), &Desc);


			if (m_p236ChaseAura != nullptr)
			{
				m_p236ChaseAura->m_bIsDoneAnim = true;
				m_p236ChaseAura = nullptr;
			}
		}
		else if (iAttackEvent == 1)
		{
			m_bAttackGravity = true;
			//Set_bAttackGravity(true);
			//Set_ForcedGravityDown();

			Set_ForcveGravityTime(0.295f);

			//Set_fImpulse({ m_iLookDirection * 2.f,0 });
		}
	}
	break;
	case Client::CPlay_Goku::ANIME_ATTACK_214:
	{

		//지상 사용시 구름 이펙트
		if (iAttackEvent == 2)
		{
			//Character_Make_Effect(TEXT("Smoke03"));
			//Character_Make_Effect(TEXT("Smoke_Run"));
			Character_Make_Effect(TEXT("Smoke03_Stop"), {m_iLookDirection*0.2f,0.f},true);
			
		}

		else if (iAttackEvent == 0)
		{
			CAttackObject::ATTACK_DESC Desc{};
			//Desc.ColliderDesc.width = 0.7;
			//Desc.ColliderDesc.height = 0.8;
			if (m_iPlayerTeam == 1)
				Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_1P_Melee_Attack;
			else
				Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_2P_Melee_Attack;
			Desc.ColliderDesc.pMineGameObject = this;
			Desc.ColliderDesc.vExtents = { 0.4f,0.4f,1.f };
			Desc.ColliderDesc.vCenter = { 0.9f * m_iLookDirection,0.8f,0.f };
			//Desc.ColliderDesc.pTransform = m_pTransformCom;
			//Desc.fhitCharacter_Impus = { 0.3f * m_iLookDirection,0 };
			Desc.fhitCharacter_Impus = { m_fImpuse.x,0 };

			Desc.fhitCharacter_StunTime = 0.3f;
			Desc.iDamage = 500 * Get_DamageScale();
			Desc.fLifeTime = 0.1f;
			Desc.ihitCharacter_Motion = { HitMotion::HIT_LIGHT };
			Desc.iTeam = m_iPlayerTeam;
			Desc.fAnimationLockTime = 0.04f;
			//Desc.bOwnerGravityTimeReset = true;
			Desc.pOwner = this;

			Desc.fCameraShakeDuration = 0.5f;
			Desc.fCameraShakeMagnitude = 0.1f;

			m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack"), TEXT("Layer_AttackObject"), &Desc);
		}
		else if (iAttackEvent == 1)
		{
			CAttackObject::ATTACK_DESC Desc{};
			//Desc.ColliderDesc.width = 0.7;
			//Desc.ColliderDesc.height = 0.8;
			if (m_iPlayerTeam == 1)
				Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_1P_Melee_Attack;
			else
				Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_2P_Melee_Attack;
			Desc.ColliderDesc.pMineGameObject = this;
			Desc.ColliderDesc.vExtents = { 0.4f,0.4f,1.f };
			Desc.ColliderDesc.vCenter = { 0.9f * m_iLookDirection,0.8f,0.f };
			//Desc.ColliderDesc.pTransform = m_pTransformCom;
			//Desc.fhitCharacter_Impus = { 0.3f * m_iLookDirection,0 };
			Desc.fhitCharacter_Impus = { m_fImpuse.x * 3.f,0 };

			Desc.fhitCharacter_StunTime = 0.5f;
			Desc.iDamage = 500 * Get_DamageScale();
			Desc.fLifeTime = 0.1f;
			Desc.ihitCharacter_Motion = { HitMotion::HIT_MEDIUM };
			Desc.iTeam = m_iPlayerTeam;
		
			if(Get_fHeight() >1)
				Desc.fAnimationLockTime = 0.2;
			else
				Desc.fAnimationLockTime = 0.07f;

			//Desc.bOwnerGravityTimeReset = true;
			Desc.pOwner = this;
			//Desc.bGainAttackStep = false;		//2타 떄리는 공격이므로 step은 한번만.
			Desc.iGainAttackStep = 0;

			Desc.fCameraShakeDuration = 0.5f;
			Desc.fCameraShakeMagnitude = 0.1f;

			Desc.iGainKiAmount = 7;

			m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack"), TEXT("Layer_AttackObject"), &Desc);
		}
	}

	break;
	case Client::CPlay_Goku::ANIME_ATTACK_236_SPECIAL:
	{
		if (iAttackEvent == 0)
		{

			if (m_bUltimateKamehameha)
			{
				m_pRenderInstance->Get_Instance()->Switch_BlackOut(true);
				Set_bAura(true);

				//Set_AnimationStopWithoutMe(2.f);
				//
				////이전에 만들어진 모든 Attack Object들 2초간 정지.  이거 본인은 이 직후에 만들어질테니 괜찮음
				//for (auto pAttackObject : m_pGameInstance->Get_Layer(LEVEL_GAMEPLAY, TEXT("Layer_AttackObject")))
				//{
				//	static_cast<CAttackObject*>(pAttackObject)->Set_UpdateStop(2.f);
				//}
				//
				//Set_NoEventAnmationLoof(25.0f, 40.f, 2.f);


				_float fStopTime = 1.8f;

				Set_AnimationStopWithoutMe(fStopTime);
				for (auto pAttackObject : m_pGameInstance->Get_Layer(LEVEL_GAMEPLAY, TEXT("Layer_AttackObject")))
				{
					static_cast<CAttackObject*>(pAttackObject)->Set_UpdateStop(fStopTime);
				}

				CMain_Camera* main_Camera = static_cast<CMain_Camera*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Main_Camera")));
				main_Camera->Play(CMain_Camera::VIRTUAL_CAMERA_SON_ENERGY, 0, this);
				main_Camera->StartCameraShake(2.f, 0.1f);

				Set_AnimationStop(fStopTime);

				//Character_Make_Effect(TEXT("Aura11_Yellow"));
				Set_LoofAnimationCreate(TEXT("Aura11_Yellow"), 2.6f, 0.3f);


	
				//Set_LoofAnimationCreate(TEXT("EnergieSDO-01"), 2.f, 0.3f, { -0.5f,-0.2f });
				//Set_LoofAnimationCreate(TEXT("EnergieSDO-02"), 2.f, 0.3f, { -0.5f,-0.2f });

				Character_Make_Effect(TEXT("EnergieSDO-01"), { -0.5f,-0.2f });
				Character_Make_Effect(TEXT("EnergieSDO-02"), { -0.5f,-0.2f });


			}
			else
			{
				//Character_Make_BoneEffect("GD_fist_R", TEXT("Energie-01"));

				//Character_Make_Effect(TEXT("Energie-01"), { -0.6f,-0.2f });



			}
			Character_Make_Effect(TEXT("Energie-01"), { -0.6f,-0.2f });

			m_pGameInstance->Play_Sound(CSound_Manager::SOUND_KEY_NAME::Goku_Energy_Fire_SFX, false, 1.f);
		}
		else if (iAttackEvent == 1)
		{

			Add_BlueLight();

			if (m_bUltimateKamehameha)
			{
				Set_bAura(true);
				//attack obejct이던 시절 백업

				//CAttackObject::ATTACK_DESC Desc{};
				//if (m_iPlayerTeam == 1)
				//	Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_1P_Melee_Attack;
				//else
				//	Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_2P_Melee_Attack;
				//Desc.ColliderDesc.pMineGameObject = this;
				//Desc.ColliderDesc.vExtents = { 20.f,1.3f,1.f };
				//Desc.ColliderDesc.vCenter = { 10.2f * m_iLookDirection,1.2f,0.f };
				////Desc.ColliderDesc.pTransform = m_pTransformCom;
				//Desc.fhitCharacter_Impus = { 20.f * m_iLookDirection,0 };
				//Desc.fhitCharacter_StunTime = 3.0f;
				//Desc.iDamage = 2214 * Get_DamageScale();;
				//Desc.fLifeTime = 0.7f;
				//Desc.ihitCharacter_Motion = { HitMotion::HIT_KNOCK_AWAY_LEFT };
				//Desc.iTeam = m_iPlayerTeam;
				////Desc.bGroundSmash = true;
				//Desc.fAnimationLockTime = 0.0f;
				//Desc.pOwner = this;
				//Desc.eAttackGrade = { GRADE_ULTIMATE };
				//Desc.bCameraZoom = false;
				//
				//m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack"), TEXT("Layer_AttackObject"), &Desc);


				CAttackObject_Energy::ATTACK_RANGED_DESC Desc{};
				if (m_iPlayerTeam == 1)
					Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_1P_Energy_Attack;
				else
					Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_2P_Energy_Attack;
				Desc.ColliderDesc.pMineGameObject = this;
				//Desc.ColliderDesc.vExtents = { 0.5f,0.8f,1.f };
				Desc.ColliderDesc.vExtents = { 0.f,0.f,0.f };
				Desc.ColliderDesc.vCenter = { -0.6f *m_iLookDirection,0.f,0.f };
				//Desc.ColliderDesc.pTransform = m_pTransformCom;
				Desc.fhitCharacter_Impus = { 20.f * m_iLookDirection,0 };
				Desc.fhitCharacter_StunTime = 3.0f;
				Desc.iDamage = 220 * Get_DamageScale(true);
				Desc.fLifeTime = 1.2f;
				Desc.ihitCharacter_Motion = { HitMotion::HIT_KNOCK_AWAY_LEFT };
				Desc.iTeam = m_iPlayerTeam;
				//Desc.bGroundSmash = true;
				Desc.fAnimationLockTime = 0.0f;
				Desc.pOwner = this;
				Desc.eAttackGrade = { GRADE_ULTIMATE };
				Desc.bCameraZoom = false;

				Desc.fStartOffset = { 0.5f * m_iLookDirection,0.5f };
				//Desc.fMoveSpeedNoneDirection = { 25.f,0.f };
				Desc.fMoveSpeedNoneDirection = { 90.f,0.f };


				Desc.iAttackCount = 10;
				Desc.iPlayerDirection = m_iLookDirection;		//
				Desc.iGainHitCount = 2;

				Desc.eEnergyColor = CAttackObject_Energy::ENERGY_LIGHT_BLUE;

				Desc.fColliderfCY = 1.2f;
				Desc.iGainKiAmount = 0;

				m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack_Energy"), TEXT("Layer_AttackObject"), &Desc);

				m_pGameInstance->Play_Sound(CSound_Manager::SOUND_KEY_NAME::Goku_Energy_Fire_SFX, false, 1.f);


				CEffect_Manager::Get_Instance()->Copy_Layer(TEXT("BackGround_Dust"));


				Character_Make_Effect(TEXT("Energie-02"), { 0.7f,0.9f });
				Character_Make_Effect(TEXT("Energie-03"), { 0.7f,0.9f });

				//CEffect_Layer* pEffect = Character_Make_Effect(TEXT("Energie-02"), { 0.7f,0.9f });
				//pEffect->Set_Copy_Layer_Scaled({ 1.3f,1.3f,1.3f });
				//
				//pEffect = Character_Make_Effect(TEXT("Energie-03"), { 0.7f,0.9f });
				//pEffect->Set_Copy_Layer_Scaled({ 1.3f,1.3f,1.3f });

			}
			else
			{
				//CAttackObject::ATTACK_DESC Desc{};
				////Desc.ColliderDesc.width = 20.0;
				////Desc.ColliderDesc.height = 1.3;
				//
				//if (m_iPlayerTeam == 1)
				//	Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_1P_Melee_Attack;
				//else
				//	Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_2P_Melee_Attack;
				//Desc.ColliderDesc.pMineGameObject = this;
				//Desc.ColliderDesc.vExtents = { 20.f,1.3f,1.f };
				//Desc.ColliderDesc.vCenter = { 10.2f * m_iLookDirection,1.2f,0.f };
				////Desc.ColliderDesc.pTransform = m_pTransformCom;
				//Desc.fhitCharacter_Impus = { 20.f * m_iLookDirection,0 };
				//Desc.fhitCharacter_StunTime = 3.0f;
				//Desc.iDamage = 1100 * Get_DamageScale();;
				//Desc.fLifeTime = 0.7f;
				//Desc.ihitCharacter_Motion = { HitMotion::HIT_KNOCK_AWAY_LEFT };
				//Desc.iTeam = m_iPlayerTeam;
				////Desc.bGroundSmash = true;
				//Desc.fAnimationLockTime = 0.0f;
				//Desc.pOwner = this;
				//Desc.bCameraZoom = false;
				//
				//m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack"), TEXT("Layer_AttackObject"), &Desc);


				CAttackObject_Energy::ATTACK_RANGED_DESC Desc{};
				if (m_iPlayerTeam == 1)
					Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_1P_Energy_Attack;
				else
					Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_2P_Energy_Attack;
				Desc.ColliderDesc.pMineGameObject = this;

				//Desc.ColliderDesc.vExtents = { 0.5f,0.8f,1.f };
				//Desc.ColliderDesc.vCenter = { 0.2f * m_iLookDirection,1.2f,0.f };

				Desc.ColliderDesc.vExtents = { 0.f,0.f,0.f };
				Desc.ColliderDesc.vCenter = { -1.f * m_iLookDirection,0.f,0.f };

				//Desc.ColliderDesc.vExtents = { 0.f,0.f,0.f };
				//Desc.ColliderDesc.vCenter = { -0.6f * m_iLookDirection,0.f,0.f };

				//Desc.ColliderDesc.pTransform = m_pTransformCom;
				Desc.fhitCharacter_Impus = { 20.f * m_iLookDirection,0 };
				Desc.fhitCharacter_StunTime = 3.0f;
				Desc.iDamage = 260 * Get_DamageScale();;
				Desc.fLifeTime = 1.2f;
				Desc.ihitCharacter_Motion = { HitMotion::HIT_KNOCK_AWAY_LEFT };
				//Desc.ihitCharacter_Motion = { HitMotion::HIT_SPIN_AWAY_LEFTUP };


				Desc.iTeam = m_iPlayerTeam;
				//Desc.bGroundSmash = true;
				Desc.fAnimationLockTime = 0.0f;
				Desc.pOwner = this;
				//Desc.eAttackGrade = { GRADE_ULTIMATE };
				Desc.bCameraZoom = false;

				Desc.fStartOffset = { 0.5f * m_iLookDirection,0.5f };
				//Desc.fMoveSpeedNoneDirection = { 25.f,0.f };
				//Desc.fMoveSpeedNoneDirection = { 50.f,0.f };
				Desc.fMoveSpeedNoneDirection = { 90.f,0.f };



				Desc.iAttackCount = 5;
				Desc.iPlayerDirection = m_iLookDirection;		//
				Desc.iGainHitCount = 1;
				Desc.iGainKiAmount = 2;

				Desc.eEnergyColor = CAttackObject_Energy::ENERGY_LIGHT_BLUE;


				//m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("CAttackObject_Energy"), TEXT("Layer_AttackObject"), &Desc);

				m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack_Energy"), TEXT("Layer_AttackObject"), &Desc);
			

				Character_Make_Effect(TEXT("Energie-02"), { 0.7f,0.9f });
				Character_Make_Effect(TEXT("Energie-03"), { 0.7f,0.9f });

			}
		}
		else if (iAttackEvent == 101)
		{
			Set_bAura(false);
			m_pRenderInstance->Get_Instance()->Switch_BlackOut(false);
		}
	}
	

	break;
	case Client::CPlay_Goku::ANIME_ATTACK_236_SPECIAL_UPSIDE:

		if (iAttackEvent == 0)
		{

			if (m_bUltimateKamehameha)
			{
				Set_bAura(true);
				_float fStopTime = 1.8f;

				Set_AnimationStopWithoutMe(fStopTime);
				for (auto pAttackObject : m_pGameInstance->Get_Layer(LEVEL_GAMEPLAY, TEXT("Layer_AttackObject")))
				{
					static_cast<CAttackObject*>(pAttackObject)->Set_UpdateStop(fStopTime);
				}

				CMain_Camera* main_Camera = static_cast<CMain_Camera*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Main_Camera")));
				main_Camera->StartCameraShake(fStopTime, 0.1f);
				main_Camera->Play(CMain_Camera::VIRTUAL_CAMERA_SON_ENERGY, 0, this);
				main_Camera->StartCameraShake(2.f, 0.1f);

				Set_AnimationStop(fStopTime);


				Set_LoofAnimationCreate(TEXT("Aura11_Yellow"), 2.6f, 0.3f);

				//BackGround_Dust
				Character_Make_Effect(TEXT("BackGround_Dust"));
			}
		}
		else if(iAttackEvent == 1)
		{

			if (m_bUltimateKamehameha)
			{
				Set_bAura(true);
				CAttackObject_Energy::ATTACK_RANGED_DESC Desc{};
				if (m_iPlayerTeam == 1)
					Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_1P_Energy_Attack;
				else
					Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_2P_Energy_Attack;
				Desc.ColliderDesc.pMineGameObject = this;
				//Desc.ColliderDesc.vExtents = { 0.5f,0.8f,1.f };
				//Desc.ColliderDesc.vCenter = { 0.2f * m_iLookDirection,1.2f,0.f };
				Desc.ColliderDesc.vExtents = { 0.0f,0.0f,0.f };
				Desc.ColliderDesc.vCenter = { -0.3f * m_iLookDirection,0.f,0.f };
				//Desc.ColliderDesc.pTransform = m_pTransformCom;
				//Desc.fhitCharacter_Impus = { 18.75f * m_iLookDirection,10.f };
				Desc.fhitCharacter_Impus = { 6.f * m_iLookDirection,3.f };

				Desc.fhitCharacter_StunTime = 3.0f;
				Desc.iDamage = 220 * Get_DamageScale(true);
				Desc.fLifeTime = 1.2f; 
				Desc.ihitCharacter_Motion = { HitMotion::HIT_SPIN_AWAY_LEFTUP };
				Desc.iTeam = m_iPlayerTeam;
				//Desc.bGroundSmash = true;
				Desc.fAnimationLockTime = 0.0f;
				Desc.pOwner = this;
				//Desc.eAttackGrade = { GRADE_ULTIMATE };
				Desc.bCameraZoom = false;

				Desc.fStartOffset = { 0.5f * m_iLookDirection,0.5f };
				//Desc.fMoveSpeedNoneDirection = { 25.f,10.f };
				//Desc.fMoveSpeedNoneDirection = { 75.f,30.f };
				Desc.fMoveSpeedNoneDirection = { 75.f,40.f };



				Desc.iAttackCount = 10;
				Desc.iPlayerDirection = m_iLookDirection;		//
				Desc.iGainHitCount = 2;

				Desc.eEnergyColor = CAttackObject_Energy::ENERGY_LIGHT_BLUE;

				Desc.fColliderfCY = 1.2f;
				Desc.iGainKiAmount = 0;

				//m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("CAttackObject_Energy"), TEXT("Layer_AttackObject"), &Desc);

				m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack_Energy"), TEXT("Layer_AttackObject"), &Desc);




				Character_Make_Effect(TEXT("Energie-02_Up"), { 0.9f,1.3f });
				Character_Make_Effect(TEXT("Energie-03_Up"), { 0.9f,1.3f });


			}
			else
			{

				CAttackObject_Energy::ATTACK_RANGED_DESC Desc{};
				if (m_iPlayerTeam == 1)
					Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_1P_Energy_Attack;
				else
					Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_2P_Energy_Attack;
				Desc.ColliderDesc.pMineGameObject = this;
				Desc.ColliderDesc.vExtents = { 0.0f,0.0f,0.f };
				Desc.ColliderDesc.vCenter = { -0.5f * m_iLookDirection,0.f,0.f };
				//Desc.ColliderDesc.pTransform = m_pTransformCom;
				//Desc.fhitCharacter_Impus = { 18.75f * m_iLookDirection,10.f };
				//Desc.fhitCharacter_Impus = { 12.f * m_iLookDirection,6.f };
				Desc.fhitCharacter_Impus = { 6.f * m_iLookDirection,3.f };


				//Desc.fhitCharacter_Impus = { 48.75f * m_iLookDirection,10.f };

				Desc.fhitCharacter_StunTime = 3.0f;
				Desc.iDamage = 260 * Get_DamageScale();;
				Desc.fLifeTime = 1.2f;
				Desc.ihitCharacter_Motion = { HitMotion::HIT_SPIN_AWAY_LEFTUP };
				Desc.iTeam = m_iPlayerTeam;
				//Desc.bGroundSmash = true;
				Desc.fAnimationLockTime = 0.0f;
				Desc.pOwner = this;
				//Desc.eAttackGrade = { GRADE_ULTIMATE };
				Desc.bCameraZoom = false;

				Desc.fStartOffset = { 0.5f *m_iLookDirection,0.5f };
				//Desc.fMoveSpeedNoneDirection = { 25.f,10.f };
				//Desc.fMoveSpeedNoneDirection = { 75.f,30.f };
				Desc.fMoveSpeedNoneDirection = { 75.f,40.f };


				Desc.iAttackCount = 5;
				Desc.iPlayerDirection = m_iLookDirection;		//
				Desc.iGainHitCount = 2;

				Desc.eEnergyColor = CAttackObject_Energy::ENERGY_LIGHT_BLUE;

				Desc.fColliderfCY = 1.2f;
				Desc.iGainKiAmount = 2;

				//m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("CAttackObject_Energy"), TEXT("Layer_AttackObject"), &Desc);

				m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack_Energy"), TEXT("Layer_AttackObject"), &Desc);




				//Character_Make_Effect(TEXT("Energie-02_Up"));
				//Character_Make_Effect(TEXT("Energie-03_Up"));

				Character_Make_Effect(TEXT("Energie-02_Up"), { 0.9f,1.3f });
				Character_Make_Effect(TEXT("Energie-03_Up"), { 0.9f,1.3f });
			}

		}
		else if (iAttackEvent == 101)
		{
			Set_bAura(false);
			m_pRenderInstance->Get_Instance()->Switch_BlackOut(false);


		}
		break;
	case Client::CPlay_Goku::ANIME_ATTACK_236_SPECIAL_AIR:
		if (iAttackEvent == 0)
		{

			if (m_bUltimateKamehameha)
			{

				_float fStopTime = 1.8f;


				Set_AnimationStopWithoutMe(fStopTime);
				for (auto pAttackObject : m_pGameInstance->Get_Layer(LEVEL_GAMEPLAY, TEXT("Layer_AttackObject")))
				{
					static_cast<CAttackObject*>(pAttackObject)->Set_UpdateStop(fStopTime);
				}

				CMain_Camera* main_Camera = static_cast<CMain_Camera*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Main_Camera")));
				main_Camera->StartCameraShake(fStopTime, 0.1f);
				main_Camera->Play(CMain_Camera::VIRTUAL_CAMERA_SON_ENERGY, 0, this);
				main_Camera->StartCameraShake(2.f, 0.1f);

				Set_AnimationStop(fStopTime);



				Set_LoofAnimationCreate(TEXT("Aura11_Yellow"), 2.6f, 0.3f);


				Character_Make_Effect(TEXT("BackGround_Dust"));

				Character_Make_Effect(TEXT("EnergieSDO-01"), { -0.4f,-0.07f });
				Character_Make_Effect(TEXT("EnergieSDO-02"), { -0.4f,-0.07f });

				//Character_Make_BoneEffect("GD_fist_R", TEXT("EnergieSDO-01"));
				//Character_Make_BoneEffect("GD_fist_R", TEXT("EnergieSDO-02"));


			}



		}
		else if (iAttackEvent == 1)
		{

			Add_BlueLight();

			if (m_bUltimateKamehameha)
			{


				CAttackObject_Energy::ATTACK_RANGED_DESC Desc{};
				if (m_iPlayerTeam == 1)
					Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_1P_Energy_Attack;
				else
					Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_2P_Energy_Attack;
				Desc.ColliderDesc.pMineGameObject = this;
				//Desc.ColliderDesc.vExtents = { 0.5f,0.8f,1.f };
				//Desc.ColliderDesc.vCenter = { 0.2f * m_iLookDirection,1.6f,0.f };
				Desc.ColliderDesc.vExtents = { 0.f,0.f,0.f };
				Desc.ColliderDesc.vCenter = { -0.5f * m_iLookDirection,0.f,0.f };


				Desc.fhitCharacter_Impus = { 20.f * m_iLookDirection,0 };
				Desc.fhitCharacter_StunTime = 3.0f;
				Desc.iDamage = 220 * Get_DamageScale(true);
				Desc.fLifeTime = 1.2f;
				Desc.ihitCharacter_Motion = { HitMotion::HIT_KNOCK_AWAY_LEFT };
				Desc.iTeam = m_iPlayerTeam;
				//Desc.bGroundSmash = true;
				Desc.fAnimationLockTime = 0.0f;
				Desc.pOwner = this;
				Desc.eAttackGrade = { GRADE_ULTIMATE };
				Desc.bCameraZoom = false;

				Desc.fStartOffset = { 0.5f,0.5f };
				Desc.fMoveSpeedNoneDirection = { 25.f,0.f };

				Desc.iAttackCount = 10;
				Desc.iPlayerDirection = m_iLookDirection;		//
				Desc.iGainHitCount = 2;

				Desc.eEnergyColor = CAttackObject_Energy::ENERGY_LIGHT_BLUE;


				Desc.fColliderfCY = 1.2f;
				Desc.iGainKiAmount = 0;

				m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack_Energy"), TEXT("Layer_AttackObject"), &Desc);



				CEffect_Manager::Get_Instance()->Copy_Layer(TEXT("BackGround_Dust"));

				Character_Make_Effect(TEXT("Energie-02"), { 0.7f,1.1f });
				Character_Make_Effect(TEXT("Energie-03"), { 0.7f,1.1f });
			}
			else
			{
				CAttackObject_Energy::ATTACK_RANGED_DESC Desc{};
				if (m_iPlayerTeam == 1)
					Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_1P_Energy_Attack;
				else
					Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_2P_Energy_Attack;
				Desc.ColliderDesc.pMineGameObject = this;
				//Desc.ColliderDesc.vExtents = { 0.5f,0.8f,1.f };
				//Desc.ColliderDesc.vCenter = { 0.2f * m_iLookDirection,1.6f,0.f };
				Desc.ColliderDesc.vExtents = { 0.f,0.f,0.f };
				Desc.ColliderDesc.vCenter = { -0.5f * m_iLookDirection,0.f,0.f };

				//Desc.ColliderDesc.pTransform = m_pTransformCom;
				Desc.fhitCharacter_Impus = { 20.f * m_iLookDirection,0 };
				Desc.fhitCharacter_StunTime = 3.0f;
				Desc.iDamage = 260 * Get_DamageScale();;
				Desc.fLifeTime = 1.2f;
				Desc.ihitCharacter_Motion = { HitMotion::HIT_KNOCK_AWAY_LEFT };
				//Desc.ihitCharacter_Motion = { HitMotion::HIT_NONE };


				Desc.iTeam = m_iPlayerTeam;
				//Desc.bGroundSmash = true;
				Desc.fAnimationLockTime = 0.0f;
				Desc.pOwner = this;
				//Desc.eAttackGrade = { GRADE_ULTIMATE };
				Desc.bCameraZoom = false;

				Desc.fStartOffset = { 0.5f,0.5f };
				Desc.fMoveSpeedNoneDirection = { 25.f,0.f };

				Desc.iAttackCount = 5;
				Desc.iPlayerDirection = m_iLookDirection;		//
				Desc.iGainHitCount = 5;
				Desc.iGainKiAmount = 2;

				Desc.eEnergyColor = CAttackObject_Energy::ENERGY_LIGHT_BLUE;


				//m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("CAttackObject_Energy"), TEXT("Layer_AttackObject"), &Desc);

				m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack_Energy"), TEXT("Layer_AttackObject"), &Desc);

				Character_Make_Effect(TEXT("Energie-02"), { 0.7f,1.1f });
				Character_Make_Effect(TEXT("Energie-03"), { 0.7f,1.1f });
			}
		}
		else if (iAttackEvent == 101)
		{
			Set_bAura(false);
			m_pRenderInstance->Get_Instance()->Switch_BlackOut(false);
		}
		break;
	case Client::CPlay_Goku::ANIME_ATTACK_236_SPECIAL_AIR_DOWNSIDE:
		if (iAttackEvent == 0)
		{

			if (m_bUltimateKamehameha)
			{

				_float fStopTime = 1.8f;


				Set_AnimationStopWithoutMe(fStopTime);
				for (auto pAttackObject : m_pGameInstance->Get_Layer(LEVEL_GAMEPLAY, TEXT("Layer_AttackObject")))
				{
					static_cast<CAttackObject*>(pAttackObject)->Set_UpdateStop(fStopTime);
				}

				CMain_Camera* main_Camera = static_cast<CMain_Camera*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Main_Camera")));
				main_Camera->StartCameraShake(fStopTime, 0.1f);
				main_Camera->Play(CMain_Camera::VIRTUAL_CAMERA_SON_ENERGY, 0, this);
				main_Camera->StartCameraShake(2.f, 0.1f);

				Set_AnimationStop(fStopTime);



				Set_LoofAnimationCreate(TEXT("Aura11_Yellow"), 2.6f, 0.3f);

				Character_Make_Effect(TEXT("BackGround_Dust"));


			}
			


		}
		if (iAttackEvent == 1)
		{
			if (m_bUltimateKamehameha)
			{
				CAttackObject_Energy::ATTACK_RANGED_DESC Desc{};
				if (m_iPlayerTeam == 1)
					Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_1P_Energy_Attack;
				else
					Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_2P_Energy_Attack;
				Desc.ColliderDesc.pMineGameObject = this;

				Desc.ColliderDesc.vExtents = { 0.0f,0.0f,0.f };
				Desc.ColliderDesc.vCenter = { -0.5f * m_iLookDirection,0.f,0.f };


				//Desc.fhitCharacter_Impus = { 18.75f * m_iLookDirection,-10.f };
				Desc.fhitCharacter_Impus = { 6.f * m_iLookDirection,-3.f };

				Desc.fhitCharacter_StunTime = 3.0f;
				Desc.iDamage = 220 * Get_DamageScale(true);;
				Desc.fLifeTime = 1.2f;
				Desc.ihitCharacter_Motion = { HitMotion::HIT_KNOCK_AWAY_LEFTDOWN };
				Desc.iTeam = m_iPlayerTeam;
				Desc.fAnimationLockTime = 0.0f;
				Desc.pOwner = this;
				Desc.eAttackGrade = { GRADE_ULTIMATE };
				Desc.bCameraZoom = false;
				Desc.bGroundSmash = true;

				//Desc.fStartOffset = { 0.5f * m_iLookDirection,0.5f };

				Desc.fMoveSpeedNoneDirection = { 75.f,-40.f };



				Desc.iAttackCount = 10;
				Desc.iPlayerDirection = m_iLookDirection;		//
				Desc.iGainHitCount = 2;

				Desc.eEnergyColor = CAttackObject_Energy::ENERGY_LIGHT_BLUE;

				Desc.fColliderfCY = 1.2f;
				Desc.iGainKiAmount = 0;

				//m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("CAttackObject_Energy"), TEXT("Layer_AttackObject"), &Desc);

				m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack_Energy"), TEXT("Layer_AttackObject"), &Desc);




				Character_Make_Effect(TEXT("Energie-02_Down"), { 0.f,0.7f });
				Character_Make_Effect(TEXT("Energie-03_Down"), { 0.f,0.7f });
			}
			else
			{
				CAttackObject_Energy::ATTACK_RANGED_DESC Desc{};
				if (m_iPlayerTeam == 1)
					Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_1P_Energy_Attack;
				else
					Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_2P_Energy_Attack;
				Desc.ColliderDesc.pMineGameObject = this;
				Desc.ColliderDesc.vExtents = { 0.0f,0.0f,0.f };
				Desc.ColliderDesc.vCenter = { -0.5f * m_iLookDirection,0.f,0.f };
				//Desc.ColliderDesc.pTransform = m_pTransformCom;
				//Desc.fhitCharacter_Impus = { 18.75f * m_iLookDirection,10.f };
				Desc.fhitCharacter_Impus = { 6.f * m_iLookDirection,0.f };

				//Desc.fhitCharacter_Impus = { 48.75f * m_iLookDirection,10.f };

				Desc.fhitCharacter_StunTime = 3.0f;
				Desc.iDamage = 260 * Get_DamageScale();;
				Desc.fLifeTime = 1.2f;
				Desc.ihitCharacter_Motion = { HitMotion::HIT_SPIN_AWAY_LEFTUP };
				Desc.iTeam = m_iPlayerTeam;
				//Desc.bGroundSmash = true;
				Desc.fAnimationLockTime = 0.0f;
				Desc.pOwner = this;
				//Desc.eAttackGrade = { GRADE_ULTIMATE };
				Desc.bCameraZoom = false;

				//Desc.fStartOffset = { 0.5f * m_iLookDirection,0.5f };
				Desc.fStartOffset = { 0.f,0.5f };

				Desc.fMoveSpeedNoneDirection = { 75.f,-40.f };


				Desc.iAttackCount = 5;
				Desc.iPlayerDirection = m_iLookDirection;		//
				Desc.iGainHitCount = 2;
				Desc.iGainKiAmount = 2;

				Desc.eEnergyColor = CAttackObject_Energy::ENERGY_LIGHT_BLUE;

				Desc.fColliderfCY = 1.2f;

				//m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("CAttackObject_Energy"), TEXT("Layer_AttackObject"), &Desc);

				m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack_Energy"), TEXT("Layer_AttackObject"), &Desc);



				Character_Make_Effect(TEXT("Energie-02_Down"), { 0.f,0.7f });
				Character_Make_Effect(TEXT("Energie-03_Down"), { 0.f,0.7f });
			}
		}
		else if (iAttackEvent == 101)
		{
			Set_bAura(false);
			m_pRenderInstance->Get_Instance()->Switch_BlackOut(false);
		}
		break;
	case Client::CPlay_Goku::ANIME_FINAL_START:
	{

		if (iAttackEvent == 0)
		{
			if (m_pEnemy->Get_bStun() == true || m_pEnemy->Check_bCurAnimationisGroundSmash())
			{
				Character_Start_QTE(CQTE_Manager::QTE_ID_HIT);
			}
			m_pFinalAura =Character_Make_BoneEffect("G_root", TEXT("EnergieSAO-01"));


			Set_bAura(true);
			m_pRenderInstance->Get_Instance()->Switch_BlackOut(true);

		}

	}
		break;
	case Client::CPlay_Goku::ANIME_FINAL_ELBO:
	{
			if(iAttackEvent == 0)
			{
				CEffect_Layer::COPY_DESC tDesc{};
				tDesc.pPlayertMatrix = Get_pTransformMatrix();//m_pTransformCom->Get_WorldMatrixPtr();
				//m_pEffect_Manager->Copy_Layer(TEXT("EnergieSAO-01"), &tDesc);


				CAttackObject_CommandGrab::ATTACK_COMMANDGRAB_DESC Desc{};
				if (m_iPlayerTeam == 1)
					Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_1P_Melee_Attack;
				else
					Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_2P_Melee_Attack;
				Desc.ColliderDesc.pMineGameObject = this;
				//Desc.ColliderDesc.vExtents = { 1.4f,2.2f,1.f };
				Desc.ColliderDesc.vExtents = { 0.8f,0.6f,1.f };
				Desc.ColliderDesc.vCenter = { 0.3f,0.7f,0.f };
				//Desc.ColliderDesc.pTransform = m_pTransformCom;
				//Desc.fhitCharacter_Impus = { 0.3f * m_iLookDirection,0 };
				Desc.fhitCharacter_StunTime = 20.f;

				//if (m_pEnemy->Get_iHP() > 500)
				//	Desc.iDamage = 500 * Get_DamageScale(true);
				//else
				//	Desc.iDamage = 0;

				Desc.iDamage = 500 * Get_DamageScale(true);
				
				Desc.fLifeTime = 0.2f;
				Desc.ihitCharacter_Motion = { HitMotion::HIT_LIGHT };
				Desc.iTeam = m_iPlayerTeam;
				Desc.fAnimationLockTime = 0.1f;
				Desc.pOwner = this;

				Desc.eAttackType = ATTACKTYPE_HIGH;
				Desc.eAttackGrade = GRADE_ULTIMATE;

				Desc.fDistance = { 0.8f * m_iLookDirection,0.f };
				Desc.fForcedGravityTime = 0.f;
				//Desc.fGrabAnimationPosition = 40.f;
				//Desc.fGrabAnimationPosition = 25.f;
				Desc.iGainAttackStep = 2;
				Desc.iGrabAnimationIndex = ANIME_FINAL_ELBO;
				Desc.iOnwerNextAnimationIndex = ANIME_FINAL_UPPER;


				Desc.iVirtualCameraindex = CMain_Camera::VIRTUAL_CAMERA_SON_ULTIMATE;
				Desc.ianimationIndex = 1;
				Desc.fCameraShakeDuration = 0.5f;
				Desc.fCameraShakeMagnitude = 0.2f;
				Desc.iGainKiAmount = 0;

				Desc.fForcedGravityTime = 0.f;

				Desc.bOwnerNextAnimation = true;

				Desc.iCallAttackBackIndex = 1001;
				m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack_CommandGrab"), TEXT("Layer_AttackObject"), &Desc);
			}

			//못맞추면 여기로 옴
			if (iAttackEvent == 1)
			{
				Set_bAura(false);
				if (m_pFinalAura != nullptr)
				{
					m_pFinalAura->m_bIsDoneAnim = true;
					m_pFinalAura = nullptr;
				}
				//cout << "FINAL_ELBO_FAIL" << endl;

				//QTE가 켜져있다면 꺼버리기
				if (m_bCreateQTE)
				{
					Character_Start_QTE(CQTE_Manager::QTE_ID_HIT);
					m_bCreateQTE = false;
				}
				m_pRenderInstance->Get_Instance()->Switch_BlackOut(false);

			}
			else if (iAttackEvent == 1001)
			{
				m_pEnemy->Set_UnDying(true);
			}
	}
	break;
	case Client::CPlay_Goku::ANIME_FINAL_UPPER:
	{
		
		if (iAttackEvent == 1001)
		{
			m_pEnemy->Set_UnDying(true);
		}

		else if (iAttackEvent == 0)
		{
			CEffect_Layer::COPY_DESC tDesc{};
			tDesc.pPlayertMatrix = Get_pTransformMatrix();//m_pTransformCom->Get_WorldMatrixPtr();
			//m_pEffect_Manager->Copy_Layer(TEXT("EnergieSAO-01"), &tDesc);

			CAttackObject::ATTACK_DESC Desc{};
			if (m_iPlayerTeam == 1)
				Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_1P_Melee_Attack;
			else
				Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_2P_Melee_Attack;
			Desc.ColliderDesc.pMineGameObject = this;
			Desc.ColliderDesc.vExtents = { 1.5f,1.3f,1.f };
			Desc.ColliderDesc.vCenter = { 0.2f * m_iLookDirection,1.2f,0.f };


			//Desc.ColliderDesc.pTransform = m_pTransformCom;
			//Desc.fhitCharacter_Impus = { 0.5f * m_iLookDirection, 3.f };  //원래 가속도.
			Desc.fhitCharacter_Impus = { 0.5f * m_iLookDirection, 10.f };  //원래 가속도.

			//Desc.fhitCharacter_Impus = { 0.2f * m_iLookDirection, 3.f };  //공중 테스트용 임시

			Desc.fhitCharacter_StunTime = 20.0f;
			//Desc.iDamage = 400 * Get_DamageScale(true);

			Desc.iDamage = 500 * Get_DamageScale(true);

			Desc.fLifeTime = 0.3f;
			Desc.ihitCharacter_Motion = { HitMotion::HIT_KNOCK_AWAY_UP };
			Desc.iTeam = m_iPlayerTeam;
			Desc.fAnimationLockTime = 0.5f;
			Desc.pOwner = this;
			Desc.fForcedGravityTime = 0.f;

			Desc.bGrabbedEnd = true;
			Desc.iGainAttackStep = 0;
			Desc.fForcedGravityTime = 0.f;
			Desc.iGainKiAmount = 0;

			m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack"), TEXT("Layer_AttackObject"), &Desc);
		}
		
	}
	break;

	case Client::CPlay_Goku::ANIME_FINAL_SMASH:
	{
		CEffect_Layer::COPY_DESC tDesc{};
		tDesc.pPlayertMatrix = Get_pTransformMatrix();//m_pTransformCom->Get_WorldMatrixPtr();
		//m_pEffect_Manager->Copy_Layer(TEXT("EnergieSAO-01"), &tDesc);

		CAttackObject::ATTACK_DESC Desc{};
		if (m_iPlayerTeam == 1)
			Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_1P_Melee_Attack;
		else
			Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_2P_Melee_Attack;
		Desc.ColliderDesc.pMineGameObject = this;
		Desc.ColliderDesc.vExtents = { 1.5f,1.3f,1.f };
		Desc.ColliderDesc.vCenter = { 0.2f * m_iLookDirection,1.2f,0.f };


		//Desc.ColliderDesc.pTransform = m_pTransformCom;
		Desc.fhitCharacter_Impus = { 0.2f * m_iLookDirection, -8.f };  //원래 가속도.
		//Desc.fhitCharacter_Impus = { 0.2f * m_iLookDirection, 3.f };  //공중 테스트용 임시

		Desc.fhitCharacter_StunTime = 20.0f;
		Desc.iDamage = 600 * Get_DamageScale(true);;
		Desc.fLifeTime = 0.3f;
		Desc.ihitCharacter_Motion = { HitMotion::HIT_KNOCK_AWAY_LEFTDOWN };
		Desc.iTeam = m_iPlayerTeam;
		Desc.fAnimationLockTime = 0.5f;
		Desc.pOwner = this;
		Desc.iGainAttackStep = 0;
		Desc.iGainKiAmount = 0;

		Desc.bGrabbedEnd = true;

		m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack"), TEXT("Layer_AttackObject"), &Desc);
	}
	break;
	case Client::CPlay_Goku::ANIME_FINAL_KAMEHAMEHA:
	{

		//모델변경 테스트  Position0
		if (iAttackEvent == 3)
		{
			//m_iQTE = true;

	
			if (m_iQTE == 1) // 1
			{
				m_bFinalSkillss3 = true;
				m_iQTE = -1;
				m_pModelCom_Skill->SetUp_Animation(0,false,0);
				m_pModelCom_Skill->CurrentAnimationPositionJump(0.1f);

				m_bAlwaysss3Test = true;
			}
		}

		//위치 고정용 빈거
		else if (iAttackEvent == 0)
		{
			

			CAttackObject_CommandGrab::ATTACK_COMMANDGRAB_DESC Desc{};
			if (m_iPlayerTeam == 1)
				Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_1P_Melee_Attack;
			else
				Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_2P_Melee_Attack;
			Desc.ColliderDesc.pMineGameObject = this;
			Desc.ColliderDesc.vExtents = { 4.f,4.f,1.f };
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



			//Desc.fDistance = { 0.8f * m_iLookDirection,0.f };
			Desc.fForcedGravityTime = 0.f;
			//Desc.fGrabAnimationPosition = 40.f;
			//Desc.fGrabAnimationPosition = 25.f;
			Desc.iGainAttackStep = 0;
			Desc.iGrabAnimationIndex = ANIME_FINAL_KAMEHAMEHA;
			Desc.iOnwerNextAnimationIndex = ANIME_FINAL_KAMEHAMEHA;
			Desc.bOwnerNextAnimation = false;
			Desc.bForcedHit = true;
			Desc.iOnwerDirection = 0; //맞아도 뒤집지 않음
			Desc.iGainKiAmount = 0;

			Desc.iGainHitCount = 0;

			m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack_CommandGrab"), TEXT("Layer_AttackObject"), &Desc);

		}
		else  if (iAttackEvent == 1)//에너지파 생성
		{
			

			m_pEnemy->Set_UnDying(false);

			CAttackObject_Energy::ATTACK_RANGED_DESC Desc{};
			if (m_iPlayerTeam == 1)
				Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_1P_Energy_Attack;
			else
				Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_2P_Energy_Attack;
			Desc.ColliderDesc.pMineGameObject = this;
			Desc.ColliderDesc.vExtents = { 0.5f,0.8f,1.f };
			Desc.ColliderDesc.vCenter = { 0.2f * m_iLookDirection,1.2f,0.f };
			//Desc.ColliderDesc.pTransform = m_pTransformCom;
			Desc.fhitCharacter_Impus = { 20.f * m_iLookDirection,0.1f };
			Desc.fhitCharacter_StunTime = 10.f;
			//Desc.iDamage = 120 * Get_DamageScale();

			//if(m_bFinalSkillss3)
			if (m_bAlwaysss3Test)
				Desc.iDamage = 220 * Get_DamageScale(true);
			else
				Desc.iDamage = 200 * Get_DamageScale(true);
			
			if (m_pEnemy->Get_iHP() < Desc.iDamage * 20)
			{
				CMap_Manager::Get_Instance()->PlayerCall_EastFinish(CMap_Manager::EAST_LASER, 1.f);
				CUI_Manager::Get_Instance()->CutSceneUI(false);
				m_pEnemy->Set_FinalSkillRoundEnd(true, 0);
				//캐릭터 MaxDeath 도 처리

			}

			Desc.fLifeTime = 6.f;
			Desc.ihitCharacter_Motion = { HitMotion::HIT_KNOCK_AWAY_LEFT };
			Desc.iTeam = m_iPlayerTeam;
			//Desc.bGroundSmash = true;
			Desc.fAnimationLockTime = 0.0f;
			Desc.pOwner = this;
			Desc.eAttackGrade = { GRADE_ULTIMATE };
			Desc.bCameraZoom = false;

			Desc.fStartOffset = { 0.5f,0.5f };
			Desc.fMoveSpeedNoneDirection = { 50.f,0.f };

			Desc.iAttackCount = 28;
			Desc.iPlayerDirection = m_iLookDirection;		//
			Desc.iGainHitCount = 2;
			Desc.iGainAttackStep = 0;
			//Desc.fAttackDelayTime = 0.04f;  //꽤 자주 풀히트 못함
			//Desc.fAttackDelayTime = 0.03f; //종종 풀히트 못함
			Desc.fAttackDelayTime = 0.02f; //QTE 성공했을때만 못함?

			Desc.eEnergyColor = CAttackObject_Energy::ENERGY_LIGHT_BLUE;

			Desc.fColliderfCY = 1.2f;

			//Desc.bGrabbedEnd = true;
			Desc.bCameraZoom = false;
			Desc.iGainKiAmount = 0;

			m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack_Energy"), TEXT("Layer_AttackObject"), &Desc);


			Character_Make_Effect(TEXT("EnergieSAO-03"), { 0.7f,0.9f });
			//Character_Make_Effect(TEXT("Energie-03"), { 0.7f,0.9f });	
		}
		else  if (iAttackEvent == 2) //고정풀기
		{
			CAttackObject::ATTACK_DESC Desc{};
			if (m_iPlayerTeam == 1)
				Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_1P_Melee_Attack;
			else
				Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_2P_Melee_Attack;
			Desc.ColliderDesc.pMineGameObject = this;
			Desc.ColliderDesc.vExtents = { 20.f,20.f,1.f };
			Desc.ColliderDesc.vCenter = { 0.3f *m_iLookDirection,0.7f,0.f };
			//Desc.ColliderDesc.pTransform = m_pTransformCom;
			//Desc.fhitCharacter_Impus = { 20.f * m_iLookDirection,0 };
			Desc.fhitCharacter_Impus = { 40.f * m_iLookDirection,5.f };

			Desc.fhitCharacter_StunTime = 10.f;
			Desc.iDamage = 1;
			Desc.fLifeTime = 0.2f;
			Desc.ihitCharacter_Motion = { HitMotion::HIT_KNOCK_AWAY_LEFT };
			Desc.iTeam = m_iPlayerTeam;
			Desc.fAnimationLockTime = 0.1f;
			Desc.pOwner = this;


			Desc.eAttackType = ATTACKTYPE_HIGH;



			//Desc.fDistance = { 0.8f * m_iLookDirection,0.f };
			Desc.fForcedGravityTime = 0.f;
			//Desc.fGrabAnimationPosition = 40.f;
			//Desc.fGrabAnimationPosition = 25.f;
			Desc.iGainAttackStep = 0;
			Desc.bOwnerNextAnimation = false;
			Desc.iGainKiAmount = 0;

			Desc.iGainHitCount = 0;
			Desc.bGrabbedEnd = true;
			Desc.bCameraZoom = false;

			m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack"), TEXT("Layer_AttackObject"), &Desc);


			m_iQTE = -1;
			m_bFinalSkillss3 = false;


			if(m_bAlwaysss3Test == false)
				Set_bAura(false);

			m_pRenderInstance->Get_Instance()->Switch_BlackOut(false);

		}

	}
	break;
	case Client::CPlay_Goku::ANIME_REFLECT:
	{
		if(iAttackEvent == 0)
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
			Desc.iGainKiAmount = 15;

			//m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack_Reflect"), TEXT("Layer_AttackObject"), &Desc);
			m_pReflectObject = m_pGameInstance->Add_GameObject_ToLayer_AndGet(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack_Reflect"), TEXT("Layer_AttackObject"), &Desc);


			m_bReflect = true;
		}
		if (iAttackEvent == 1)
		{
			if (m_bAttackBackEvent)
			{
				Set_Animation(CPlay_Goku::ANIME_IDLE);

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
				if (m_pReflectObject->m_iGameObjectData != 0)
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
	case Client::CPlay_Goku::ANIME_IDLE:
		break;
	case Client::CPlay_Goku::ANIME_FORWARD_WALK:
		break;
	case Client::CPlay_Goku::ANIME_BACK_WALK:
		break;
	case Client::CPlay_Goku::ANIME_FORWARD_DASH:
		break;
	case Client::CPlay_Goku::ANIME_BACK_DASH:
		break;
	case Client::CPlay_Goku::ANIME_FORWARD_DASH_END:
		break;
	case Client::CPlay_Goku::ANIME_BACK_DASH_END:
		break;
	case Client::CPlay_Goku::ANIME_CROUCH_START:
		break;
	case Client::CPlay_Goku::ANIME_CROUCHING:
		break;
	case Client::CPlay_Goku::ANIME_JUMP_UP:
		break;
	case Client::CPlay_Goku::ANIME_JUMP_DOWN:
		break;
	case Client::CPlay_Goku::ANIME_CHASE:
		break;

	case Client::CPlay_Goku::ANIME_GRAB:
	{

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
			Desc.ColliderDesc.vExtents = { 1.2f,1.0f,1.f };
			Desc.ColliderDesc.vCenter = { 0.3f* m_iLookDirection,0.7f,0.f };
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

			Desc.fDistance = { 1.f * m_iLookDirection,0.f };
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
			Desc.iGainKiAmount = 1;

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
			Desc.iGainKiAmount = 1;

			Desc.bDrawNoneStop = true;


			m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack"), TEXT("Layer_AttackObject"), &Desc);
			m_pGameInstance->Play_Group_Sound(CSound_Manager::SOUND_GROUP_KEY::LIGHT_ATTACK_Hit_SFX, false, 1.f);
		}
		break;

		}

	}

	case CPlay_Goku::ANIME_BREAK_FALL_AIR:
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
	case ANIME_START_DEFAULT:
	{
		if (iAttackEvent == 3)
		{
			m_pFinalAura = Character_Make_BoneEffect("G_root", TEXT("EnergieSAO-01"));
		}

		else if (iAttackEvent == 4)
		{

			CRenderInstance::Get_Instance()->Start_AllWhiteOut(1.f, 2.f);

			//하얀필터
			m_bNormalGoku = false;

			//for (auto& iter : m_pFinalAura->m_MixtureEffects)
			//{
			//	iter->m_iChangePassIndex = 10;
			//}
		}
		else if (iAttackEvent == 5)
		{
			//확대 이후
			for (auto& iter : m_pFinalAura->m_MixtureEffects)
			{
				iter->m_iChangePassIndex = 10;
			}
		}
	}
	break;
	case ANIME_GOKU_CINEMATIC_01:
	{

		//크리링폭발
		if (iAttackEvent == 2)
		{
			//CEffect_Layer* pEffect = Character_Make_Effect(TEXT("BurstJ3-Hit01"), { 0.f,12.5f });
			//pEffect->Set_Copy_Layer_Scaled({ 3.f,3.f,3.f });

			//CEffect_Layer* pEffect = Character_Make_Effect(TEXT("BurstJ3-Hit01"), { 0.f,12.0f });
			//pEffect->Set_Copy_Layer_Scaled({ 4.f,4.f,4.f });
			//pEffect->m_fTickPerSecond *= 0.7f;


			//CEffect_Layer* pEffect = Character_Make_Effect(TEXT("BurstJ3-Hit01"), { 0.f,10.0f });
			CEffect_Layer* pEffect = Character_Make_Effect(TEXT("BurstJ3-Hit01"), { 0.f,8.0f });
			pEffect->Set_Copy_Layer_Scaled({ 4.f,4.f,4.f });
			pEffect->m_fTickPerSecond *= 0.7f;

			CBattleInterface_Manager::Get_Instance()->SpecialKiGain(7, m_iPlayerTeam);
			CBattleInterface_Manager::Get_Instance()->SpecialKiGain(5, 2);



		}

		//분노 bAura
		else if (iAttackEvent == 3)
		{
			Set_bAura(true);
		}

		//변신 전 아우라 Positon 840
		else if (iAttackEvent == 0)
		{
			m_pFinalAura = Character_Make_BoneEffect("G_root", TEXT("EnergieSAO-01"));

			

			//m_pFinalAura = Character_Make_Effect(TEXT("EnergieSAO-01"),{-12.f,0.f});

			
			CEffect_Layer* pEffect = Character_Make_BoneEffect("G_root", TEXT("Smoke03_Five_Dir"));			
			pEffect->Set_Copy_Layer_Scaled({ 2.f,1.f,2.f });
			//pEffect->m_fTickPerSecond *= 0.2f;

			pEffect = Character_Make_BoneEffect("G_root", TEXT("Smoke03_Five_Dir_Rotated_Right"));
			pEffect->Set_Copy_Layer_Scaled({ 2.f,1.f,2.f });
			//pEffect->m_fTickPerSecond *= 0.2f;

			//pEffect = Character_Make_BoneEffect("G_root", TEXT("EF_EFFECT"));
			//
			//_int NumCount = 0;
			//for (auto& iter : pEffect->m_MixtureEffects)
			//{
			//	iter->m_bIsSpriteEnd = true;
			//
			//	if(NumCount == 8 || NumCount == 9)
			//		iter->m_bIsSpriteEnd = false;
			//
			//	++NumCount;
			//}
			//pEffect->Set_Copy_Layer_Rotation({ 0.f,90.f,0.f });

		}
		//프리저 보고있는 동안 변신  Position 1001
		else if (iAttackEvent == 1)
		{
			m_bNormalGoku = false;
			for (auto& iter : m_pFinalAura->m_MixtureEffects)
			{
				iter->m_iChangePassIndex = 10;
			}
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

void CPlay_Goku::Play_Sound(_uint SoundName, _bool bisLoof, _float fvolume)
{
	m_pGameInstance->Play_Sound((CSound_Manager::SOUND_KEY_NAME)SoundName, bisLoof, fvolume);
}

void CPlay_Goku::Play_Group_Sound(_uint groupKey, _bool loop, _float volume)
{
	m_pGameInstance->Play_Group_Sound((CSound_Manager::SOUND_GROUP_KEY)groupKey, loop, volume);
}

void CPlay_Goku::Play_Sound_Stop(_uint SoundName)
{
	m_pGameInstance->Stop_Sound((CSound_Manager::SOUND_KEY_NAME)SoundName);
}

void CPlay_Goku::Set_UltimateKamehameha(_bool bUltimate)
{
	m_bUltimateKamehameha = bUltimate;
	
}

void CPlay_Goku::Add_YellowLight()
{
	LIGHT_DESC			LightDesc{};

	ZeroMemory(&LightDesc, sizeof(LIGHT_DESC));
	LightDesc.eType = LIGHT_DESC::TYPE_POINT;
	LightDesc.vPosition = _float4(0.f, 0.f, 0.f, 1.f);
	LightDesc.fRange = 30.f;
	LightDesc.vDiffuse = _float4(1.2f, 1.15f, 0.7f, 1.0f);
	//LightDesc.vDiffuse = _float4(1.0f, 0.f, 0.f, 1.f);
	LightDesc.vAmbient = _float4(0.1f, 0.1f, 0.1f, 1.f);
	LightDesc.vSpecular = _float4(1.0f, 0.95f, 0.45f, 1.f);

	LightDesc.fAccTime = 0.f;
	LightDesc.fLifeTime = 1.f;
	LightDesc.strName = "Explosion";
	if (FAILED(m_pRenderInstance->Add_Effect_Light(LightDesc.strName, LightDesc)))
		return;
}

void CPlay_Goku::Add_BlueLight()
{
	LIGHT_DESC			LightDesc{};

	ZeroMemory(&LightDesc, sizeof(LIGHT_DESC));
	LightDesc.eType = LIGHT_DESC::TYPE_POINT;
	LightDesc.vPosition = _float4(0.f, 0.f, 0.f, 1.f);
	LightDesc.fRange = 30.f;

	LightDesc.vDiffuse = _float4(0.9f, 1.1f, 1.7f, 1.0f); // 파란빛 계열로 변경

	LightDesc.vAmbient = _float4(0.1f, 0.1f, 0.1f, 1.f);
	LightDesc.vSpecular = _float4(1.0f, 0.95f, 0.45f, 1.f);

	LightDesc.fAccTime = 0.f;
	LightDesc.fLifeTime = 2.7f;
	LightDesc.strName = "Ray";
	if (FAILED(m_pRenderInstance->Add_Effect_Light(LightDesc.strName, LightDesc)))
		return;
}

_float CPlay_Goku::Get_DamageScale(_bool bUltimate)
{
	//깡으로 더하는건 쉬운데 1히트당 1이 아닌데 따로 더해도 되나  //스파킹도 있는데 받는쪽에서 더하는게 아니라 때리는쪽에 더해야하는거 아님?
	//뎀감비율
	//Step Count	0	1	2	3	4	5	6	7	8	9	10	11	12	13	14	15	16	17 +
	//Next Hit		0%	10%	20% 30% 40% 50% 60% 70% 70% 70% 70% 75% 75% 75% 80% 80% 80% 85%
	//데미지비율    1.0 0.9 0.8 0              0.3   

	_uint iAttackStepCount;
	if (m_iPlayerTeam == 1)
		iAttackStepCount = CBattleInterface_Manager::Get_Instance()->Get_HitAttackStep(2);
	else
		iAttackStepCount = CBattleInterface_Manager::Get_Instance()->Get_HitAttackStep(1);


	_float fDamageScale;// = 1.f;

	if (iAttackStepCount <= 7)
	{
		fDamageScale = 1.0f - iAttackStepCount * 0.1f;
	}

	else if (iAttackStepCount <= 10)
	{
		fDamageScale = 0.3f;
	}

	else if (iAttackStepCount <= 13)
	{
		fDamageScale = 0.25f;
	}

	else if (iAttackStepCount <= 16)
	{
		fDamageScale = 0.2f;
	}
	else
	{
		fDamageScale = 0.15f;
	}



	//필살기의 경우 최소 35%는 보장
	if (bUltimate)
	{
		if (fDamageScale < 0.35f)
			fDamageScale = 0.35f;

	}


	if (m_bSparking)
	{
		//fDamageScale += 0.2f;   //합연산. 너무 큰가?  15%->35%
		//fDamageScale *= 1.2f;	  //곱연산 .  15%->16%   너무 작은가 싶지만 원작반영.
		fDamageScale += 0.1f;
	}

	if (m_bAlwaysss3Test)
	{
		fDamageScale *= 1.05f;
	}

	//return fDamageScale;
	return fDamageScale * 0.7f;
}

void CPlay_Goku::Character_CinematicEnd()
{

	
	Set_AnimationMoveXZ(false);
	m_bNormalGoku = false;

	if (m_pFinalAura != nullptr)
	{
		m_pFinalAura->m_bIsDoneAnim = true;
		m_pFinalAura = nullptr;
	}
}

void CPlay_Goku::HitStopEffect()
{
	if (m_p236ChaseAura != nullptr)
	{
		m_p236ChaseAura->m_bIsDoneAnim = true;
		m_p236ChaseAura = nullptr;
	}

	if (m_pFinalAura != nullptr)
	{
		m_pFinalAura->m_bIsDoneAnim = true;
		m_pFinalAura = nullptr;
	}
	
}


CPlay_Goku* CPlay_Goku::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPlay_Goku* pInstance = new CPlay_Goku(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CPlay_Goku"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPlay_Goku::Clone(void* pArg)
{
	CPlay_Goku* pInstance = new CPlay_Goku(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CPlay_Goku"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlay_Goku::Free()
{
	__super::Free();


	//Safe_Release(m_pShaderCom);
	//Safe_Release(m_pModelCom);
	//Safe_Release(m_pBlackGokuModelCom);
	Safe_Release(m_pOpeningDecalTextureCom);
	Safe_Release(m_pOpeningOutLineTextureCom);
	Safe_Release(m_pModelCom_Opening);
	Safe_Release(m_pModelCom_Skill);
	Safe_Release(m_p2PTextureCom);
	Safe_Release(m_pDecalTextureCom);
	Safe_Release(m_pOpeningTextureCom);

}
