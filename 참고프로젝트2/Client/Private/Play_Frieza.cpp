#include "stdafx.h"
#include "..\Public\Play_Frieza.h"

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


#include "BattleInterface.h"


#include "Main_Camera.h"

#include "BoneEffectObject.h"
#include "Effect_Manager.h"	

#include "Effect_Layer.h"
#include "Animation.h"
#include "Frieza_Metal.h"

#include "Opening_Kririn.h"
#include "Particle_Manager.h"

#include "Map_Manager.h"

#include"Level_GamePlay.h"

CPlay_Frieza::CPlay_Frieza(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCharacter{ pDevice, pContext }
{

}

CPlay_Frieza::CPlay_Frieza(const CPlay_Frieza& Prototype)
	: CCharacter{ Prototype }
{

}

HRESULT CPlay_Frieza::Initialize_Prototype()
{
	//m_pFrameEvent = CFrameEvent_Manager::Get_Instance()->Get_pFrameEventMap();
	CFrameEvent_Manager::Get_Instance()->Initalize_NameMap();
	CFrameEvent_Manager::Get_Instance()->LoadFile2("../Bin/FrameEventData/EventData_Frieza.txt");

	return S_OK;
}

HRESULT CPlay_Frieza::Initialize(void* pArg)
{

	m_ChaseEffectName = TEXT("FZ_BurstR");
	m_fAIrGrabEndAnimationPositon = 29.99f;

	m_eCharacterIndex = PLAY_FRN;
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
	m_iAttack_AirUpper = { ANIME_ATTACK_AIR3 };

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

	m_eChaseSoundIndex = (_short)CSound_Manager::SOUND_KEY_NAME::Frieza_Air_Chase;

	m_tAttackMap.Initalize(this);

	Character_DESC* pDesc = static_cast<Character_DESC*>(pArg);
	m_strName = "FRN_" + to_string(m_iPlayerTeam) + "_" + to_string(pDesc->ePlayerSlot);
	m_RendererDesc.strName = m_strName;
	LIGHT_DESC			LightDesc{};

	LightDesc.eType = LIGHT_DESC::TYPE_DIRECTIONAL;

	LightDesc.vDirection = _float4(-0.06f, -0.07f, 0.1f, 0.f);
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.0f, 1.0f);
	LightDesc.vAmbient = _float4(0.5f, 0.5f, 0.5f, 1.f);
	LightDesc.vSpecular = _float4(0.f, 0.f, 0.f, 1.f);
	//LightDesc.vAuraColor = _float4(16.76, 1.5333, 27.86, 5.490);
	LightDesc.pPlayerDirection = &m_iLookDirection;
	LightDesc.strName = m_strName;


	LightDesc.vAuraColor = _float4(0.f, 0.f, 0.f, 0.f);
	m_fAuraColor = _float4(16.76, 1.5333, 27.86, 5.490);


	if (FAILED(m_pRenderInstance->Add_Player_Light(m_strName, LightDesc, _float4(1.7802f, 1.30980f, 2.f, 1.f), &m_bChase)))
		return E_FAIL;
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


	MoveCommandPatternsFunction.push_back({ Command_236UltimateAttack, bind(&CFrieza_MeleeAttack::Attack_236Ultimate, &m_tAttackMap) });



	MoveCommandPatternsFunction.push_back({ Command_236Attack,  bind(&CFrieza_MeleeAttack::Attack_236, &m_tAttackMap) });
	MoveCommandPatternsFunction.push_back({ Command_236Attack_Extra,  bind(&CFrieza_MeleeAttack::Attack_236, &m_tAttackMap) });


	MoveCommandPatternsFunction.push_back({ Command_236Attack_Heavy,  bind(&CFrieza_MeleeAttack::Attack_236_Heavy, &m_tAttackMap) });
	MoveCommandPatternsFunction.push_back({ Command_236Attack_Heavy_Extra,  bind(&CFrieza_MeleeAttack::Attack_236_Heavy, &m_tAttackMap) });

	MoveCommandPatternsFunction.push_back({ Command_214Attack,  bind(&CFrieza_MeleeAttack::Attack_214, &m_tAttackMap) });
	MoveCommandPatternsFunction.push_back({ Command_214Attack_Extra,  bind(&CFrieza_MeleeAttack::Attack_214, &m_tAttackMap) });

	MoveCommandPatternsFunction.push_back({ Command_236Special,  bind(&CFrieza_MeleeAttack::Attack_236Special, &m_tAttackMap) });




	MoveCommandPatternsFunction.push_back({ Command_214FinalAttack, bind(&CFrieza_MeleeAttack::Attack_214Final, &m_tAttackMap) });


	MoveCommandPatternsFunction.push_back({ Command_Crouch_LightAttack, bind(&CFrieza_MeleeAttack::Attack_Crouch_Light, &m_tAttackMap) });
	MoveCommandPatternsFunction.push_back({ Command_Crouch_MediumAttack, bind(&CFrieza_MeleeAttack::Attack_Crouch_Medium, &m_tAttackMap) });
	MoveCommandPatternsFunction.push_back({ Command_Crouch_HeavyAttack, bind(&CFrieza_MeleeAttack::Attack_Crouch_Heavy, &m_tAttackMap) });


	MoveCommandPatternsFunction.push_back({ Command_Crouch_MediumAttack_Extra, bind(&CFrieza_MeleeAttack::Attack_Crouch_Medium, &m_tAttackMap) });
	MoveCommandPatternsFunction.push_back({ Command_Crouch_HeavyAttack_Extra, bind(&CFrieza_MeleeAttack::Attack_Crouch_Heavy, &m_tAttackMap) });



	//위에서 부터 확인하므로 간단한 커맨드가 아래로 가야함
	MoveCommandPatternsFunction.push_back({ Command_LightAttack, bind(&CFrieza_MeleeAttack::Attack_Light, &m_tAttackMap) });
	MoveCommandPatternsFunction.push_back({ Command_MediumAttack, bind(&CFrieza_MeleeAttack::Attack_Medium, &m_tAttackMap) });
	MoveCommandPatternsFunction.push_back({ Command_HeavyAttack, bind(&CFrieza_MeleeAttack::Attack_Heavy, &m_tAttackMap) });
	MoveCommandPatternsFunction.push_back({ Command_SpecialAttack, bind(&CFrieza_MeleeAttack::Attack_Special, &m_tAttackMap) });


	MoveCommandPatternsFunction.push_back({ Command_HeavyAttack_Extra, bind(&CFrieza_MeleeAttack::Attack_Heavy, &m_tAttackMap) });
	MoveCommandPatternsFunction.push_back({ Command_MediumAttack_Extra, bind(&CFrieza_MeleeAttack::Attack_Medium, &m_tAttackMap) });


	MoveCommandPatternsFunction.push_back({ Command_Grab, bind(&CFrieza_MeleeAttack::Attack_Grab, &m_tAttackMap) });


	//MoveCommandPatternsFunction.push_back({ Command_BackDash, bind(&CFrieza_MeleeAttack::BackDash, &m_tAttackMap) });
	//MoveCommandPatternsFunction.push_back({ Command_Forward, bind(&CFrieza_MeleeAttack::ForwardDash, &m_tAttackMap) });

	MoveCommandPatternsFunction_Exactly.push_back({ Command_BackDash, bind(&CFrieza_MeleeAttack::BackDash, &m_tAttackMap) });
	MoveCommandPatternsFunction_Exactly.push_back({ Command_Forward, bind(&CFrieza_MeleeAttack::ForwardDash, &m_tAttackMap) });


	MoveCommandPatternsFunction_Exactly.push_back({ Command_Reflect, bind(&CFrieza_MeleeAttack::Reflect, &m_tAttackMap) });


	MoveCommandPatternsFunction_Exactly.push_back({ Command_LowBenishingAttack, bind(&CFrieza_MeleeAttack::Attack_LowBenishing, &m_tAttackMap) });

	MoveCommandPatternsFunction_Exactly.push_back({ Command_BenishingAttack, bind(&CFrieza_MeleeAttack::Attack_Benishing, &m_tAttackMap) });

	MoveCommandPatternsFunction_Exactly.push_back({ Command_Crouch_Crouch_SpecialAttack, bind(&CFrieza_MeleeAttack::Attack_Crouch_Crouch_Special, &m_tAttackMap) });
	MoveCommandPatternsFunction_Exactly.push_back({ Command_Crouch_SpecialAttack, bind(&CFrieza_MeleeAttack::Attack_Crouch_Speical, &m_tAttackMap) });

	MoveCommandPatternsFunction_Exactly.push_back({ Command_Transform, bind(&CFrieza_MeleeAttack::Frieza_Transform, &m_tAttackMap) });



	m_eCharacterID = CUI_Define::PLAYER_ID::FRIEZA;

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


	CFrieza_Metal::Frieza_Metal_DESC tMetalDesc{};
	/* 머리 */
	tMetalDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	tMetalDesc.pSocketBoneMatrix = m_pModelCom->Get_BoneMatrixPtr("G_eyes_base");
	tMetalDesc.vPosition = _float4(0.0f, 0.0883401930f, 0.0f, 1.f);
	tMetalDesc.vScale = _float3(0.25f, 0.25f, 1.0f);
	tMetalDesc.pLookDirection = &m_iLookDirection;
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Frieza_Metal"),
		TEXT("Layer_Frieza_Metal"), &tMetalDesc)))
		return E_FAIL;

	/* 오른쪽 어깨 */
	tMetalDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	tMetalDesc.pSocketBoneMatrix = m_pModelCom->Get_BoneMatrixPtr("GP_shldrHalf_R");
	tMetalDesc.vPosition = _float4(0.0f, 0.f, 0.0f, 1.f);
	tMetalDesc.vScale = _float3(0.178303823f, 0.150784373f, 1.0f);
	tMetalDesc.pLookDirection = &m_iLookDirection;
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Frieza_Metal"),
		TEXT("Layer_Frieza_Metal"), &tMetalDesc)))
		return E_FAIL;

	/* 왼쪽 어깨 */
	tMetalDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	tMetalDesc.pSocketBoneMatrix = m_pModelCom->Get_BoneMatrixPtr("GP_shldrHalf_L");
	tMetalDesc.vPosition = _float4(-0.00833033770f, 0.f, -0.0199901238f, 1.f);
	tMetalDesc.vScale = _float3(0.174982980f, 0.139942512f, 1.0f);
	tMetalDesc.pLookDirection = &m_iLookDirection;
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Frieza_Metal"),
		TEXT("Layer_Frieza_Metal"), &tMetalDesc)))
		return E_FAIL;

	/* 몸통 */
	tMetalDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	tMetalDesc.pSocketBoneMatrix = m_pModelCom->Get_BoneMatrixPtr("GP_chest");
	tMetalDesc.vPosition = _float4(-0.00499127991f, -0.0133360298f, -0.140040994f, 1.f);
	tMetalDesc.vScale = _float3(0.0910967141f, 0.111640006f, 1.0f);
	tMetalDesc.pLookDirection = &m_iLookDirection;
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Frieza_Metal"),
		TEXT("Layer_Frieza_Metal"), &tMetalDesc)))
		return E_FAIL;

	/* 오른쪽 다리 */
	//tMetalDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	//tMetalDesc.pSocketBoneMatrix = m_pModelCom->Get_BoneMatrixPtr("GP_calf_mid_R");
	//tMetalDesc.vPosition = _float4(0.0199940223f, -0.0383511372f, -0.0100060701f, 1.f);
	//tMetalDesc.vScale = _float3(0.0566315502f, 0.281670630f, 1.0f);
	//tMetalDesc.pLookDirection = &m_iLookDirection;
	//if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Frieza_Metal"),
	//	TEXT("Layer_Frieza_Metal"), &tMetalDesc)))
	//	return E_FAIL;

	//tMetalDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	//tMetalDesc.pSocketBoneMatrix = m_pModelCom->Get_BoneMatrixPtr("GP_calf_mid_L");
	//tMetalDesc.vPosition = _float4(0.0199940223f, -0.0383511372f, -0.0100060701f, 1.f);
	//tMetalDesc.vScale = _float3(0.0566315502f, 0.281670630f, 1.0f);
	//tMetalDesc.pLookDirection = &m_iLookDirection;
	//if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Frieza_Metal"),
	//	TEXT("Layer_Frieza_Metal"), &tMetalDesc)))
	//	return E_FAIL;

	return S_OK;
}

void CPlay_Frieza::Player_Update(_float fTimeDelta)
{
	__super::Player_Update(fTimeDelta);


	Update_Tag_In(fTimeDelta);


	if (m_bPlaying == false)
		return;

	if (m_pGameInstance->Key_Down(DIK_TAB))
	{
		m_bDebugInputLock = !m_bDebugInputLock;
	}


	if (m_bDebugInputLock)
		return;

	Update_Opening(fTimeDelta);

	//cout << m_fAccStunTime << " / " << m_fMaxStunTime << endl;



	//사망시 업데이트 처리.  return때문에
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


			//m_bFinalSkillRoundEnd
			//if (m_bMotionPlaying == false)
			//{
			//
			//
			//	if (iAnimationIndex == m_iDyingStandingAnimationIndex || iAnimationIndex == m_iBound_Ground)
			//	{
			//		m_fAccDyingTime += fTimeDelta;
			//		if (m_fAccDyingTime > 2.f)
			//		{
			//			CBattleInterface_Manager::Get_Instance()->Check_NextRoundFromDeathCharacter(m_iPlayerTeam, Get_NewCharacterslot());
			//			//Tag_In(m_ePlayerSlot);
			//
			//
			//		}
			//	}
			//
			//}

			cout << m_fAccDyingTime << endl;
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

				if(m_bFinalSkillRoundEndSolo ==false)
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
					//시작 애니메이션 끝부분  프리저 끝 466
					Set_CurrentAnimationPositionJump(445.99f);
				}
			}
			else if (m_pModelCom->m_iCurrentAnimationIndex == ANIME_GOKU_CINEMATIC_01)
			{
				if (m_bMotionPlaying == false)
				{
					//Set_AnimationMoveXZ(false);
					//CBattleInterface_Manager::Get_Instance()->Character_Opening_EndForCharacter(m_iPlayerTeam);
					////static_cast<CCharacter*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Model_Opening")))->m_bDead = true;
					////static_cast<COpening_Kririn*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Model_Opening")))->Set_Delete();

					Character_CinematicEnd();


				}
				else if (m_pGameInstance->Key_Down(DIK_RETURN))
				{
					//시작 애니메이션 끝부분  프리저 끝  1170
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



	Update214ReturnEvent(fTimeDelta);

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


			//cout << m_pModelCom->m_iCurrentAnimationIndex << endl;
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

	if (m_bAnimationLock == false)
		Move(fTimeDelta);











	m_pColliderCom->Update(m_pTransformCom->Get_State(CTransform::STATE_POSITION));




	if (Check_bWall())
	{
		Move_ForWall();
	}


	Tag_KeyCheck();


	if (m_pGameInstance->Key_Down(DIK_8))
	{
		ShowInputBuffer();
		m_pModelCom->m_fCurrentAnimPosition;
		_float fHeight = Get_fHeight();
		_bool bDebugf = true;
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



	if (m_pGameInstance->Key_Down(DIK_INSERT))
	{
		CBattleInterface_Manager::Get_Instance()->Gain_KiGuage(50, m_iPlayerTeam);
	}
}

void CPlay_Frieza::Camera_Update(_float fTimeDelta)
{

}

void CPlay_Frieza::Update(_float fTimeDelta)
{
	__super::Player_Update(fTimeDelta);

}

void CPlay_Frieza::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
	m_pRenderInstance->Add_RenderObject(CRenderer::RG_SHADOWOBJ, this);
}

HRESULT CPlay_Frieza::Render(_float fTimeDelta)
{

	////백업
	//if (FAILED(Bind_ShaderResources()))
	//	return E_FAIL;
	//
	//_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();
	//
	//for (size_t i = 0; i < iNumMeshes; i++)
	//{
	//	/* 모델이 가지고 있는 머테리얼 중 i번째 메시가 사용해야하는 머테리얼구조체의 aiTextureType_DIFFUSE번째 텍스쳐를 */
	//	/* m_pShaderCom에 있는 g_DiffuseTexture변수에 던져. */
	//	if (m_iPlayerTeam == 1)
	//	{
	//		if (FAILED(m_pModelCom->Bind_MaterialSRV(m_pShaderCom, aiTextureType_DIFFUSE, "g_DiffuseTexture", i)))
	//			return E_FAIL;
	//	}
	//	else
	//	{
	//		if (FAILED(m_p2PTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", 0)))
	//			return E_FAIL;
	//	}
	//	 //m_pModelCom->Bind_MaterialSRV(m_pShaderCom, aiTextureType_NORMALS, "g_NormalTexture", i);
	//
	//	/* 모델이 가지고 있는 뼈들 중에서 현재 렌더링할려고 했던 i번째ㅑ 메시가 사용하는 뼈들을 배열로 만들어서 쉐이더로 던져준다.  */
	//	m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i);
	//
	//	if (FAILED(m_pShaderCom->Begin(0)))
	//		return E_FAIL;
	//
	//
	//	if (FAILED(m_pModelCom->Render(i)))
	//		return E_FAIL;
	//}

	if (m_bInvisible == true)
		return S_OK;

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;


	

	if (m_iPlayerTeam == 1)
	{
		if (FAILED(m_p1PTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", 0)))
			return E_FAIL;
	}
	else
	{
		if (FAILED(m_p2PTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", 0)))
			return E_FAIL;

	}
	if (FAILED(m_pDecalTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DecalTexture", 0)))
		return E_FAIL;

	if (m_bGoldFrieza == true)
	{
		if (FAILED(m_pGold_BaseTexture->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", 0)))
			return E_FAIL;
		if (FAILED(m_pGold_OutLineTexture->Bind_ShaderResource(m_pShaderCom, "g_OutLineTexture", 0)))
			return E_FAIL;
	}
	


	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();
	/* Main MeshIndex : 0 */
	/* DramaticCamera MeshIndex : 1 */
	/* Shadow MeshIndex : 2 */
	/* Decal MeshIndex : 3 */
	/* Detail?? MeshIndex : 4 */
	/* 쓰레기 : 5 */

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (i == 1 || i == 2 || i == 5)
			continue;

		_uint iPassIndex = { 5 };
		if (i == 3)
			iPassIndex = 4;

		_float fMeshIndex = i * 0.1f;
		m_pShaderCom->Bind_RawValue("g_MeshIndex", &fMeshIndex, sizeof(_float));

		m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i);

		if (FAILED(m_pShaderCom->Begin(iPassIndex)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;

	}


	//corlorChange Test
	//for (size_t i = 0; i < iNumMeshes; i++)
	//{
	//	/* 모델이 가지고 있는 머테리얼 중 i번째 메시가 사용해야하는 머테리얼구조체의 aiTextureType_DIFFUSE번째 텍스쳐를 */
	//	/* m_pShaderCom에 있는 g_DiffuseTexture변수에 던져. */
	//	if (FAILED(m_pModelCom->Bind_MaterialSRV(m_pShaderCom, (aiTextureType)m_iPlayerTeam, "g_DiffuseTexture", i)))
	//		return E_FAIL;
	//	// m_pModelCom->Bind_MaterialSRV(m_pShaderCom, aiTextureType_NORMALS, "g_NormalTexture", i);
	//
	//	/* 모델이 가지고 있는 뼈들 중에서 현재 렌더링할려고 했던 i번째ㅑ 메시가 사용하는 뼈들을 배열로 만들어서 쉐이더로 던져준다.  */
	//	m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i);
	//
	//	if (FAILED(m_pShaderCom->Begin(0)))
	//		return E_FAIL;
	//
	//	if (FAILED(m_pModelCom->Render(i)))
	//		return E_FAIL;
	//}
#ifdef _DEBUG
	m_pColliderCom->Render(fTimeDelta);
#endif // DEBUG

	return S_OK;
}

HRESULT CPlay_Frieza::Shadow_Render(_float fTimeDelta)
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
void CPlay_Frieza::AttackNextMoveCheck()
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

void CPlay_Frieza::AnimeEndNextMoveCheck()
{

	Set_Animation(m_iNextAnimation.first);

	m_iNextAnimation.first = ANIME_IDLE;
	m_iNextAnimation.second = 1000.f;


}



void CPlay_Frieza::Set_Animation(_uint iAnimationIndex, _bool bloof)
{

	if (iAnimationIndex == m_iIdleAnimationIndex)
		m_pModelCom->SetUp_Animation(iAnimationIndex, true);
	else
		m_pModelCom->SetUp_Animation(iAnimationIndex, bloof);

}

*/


void CPlay_Frieza::KeyTest()
{

}



HRESULT CPlay_Frieza::Ready_Components()
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


	/* 모델 근육질은 아직 */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Play_Frieza"), TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* 아웃라인 및 명암 처리 텍스쳐 (골드버전은 따로 존재함) */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_FRN_ilm"), TEXT("Com_OutLine_Base_Texture"),
		reinterpret_cast<CComponent**>(&m_pOutLineCom))))
		return E_FAIL;
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_FRG_ilm"), TEXT("Com_OutLine_Gold_Texture"),
		reinterpret_cast<CComponent**>(&m_pGold_OutLineTexture))))
		return E_FAIL;

	/* 표정 텍스쳐 (애니메이션) */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_FRN_decal"),
		TEXT("Com_DecalTexture"), reinterpret_cast<CComponent**>(&m_pDecalTextureCom))))
		return E_FAIL;
	/* 디테일 텍스쳐 (바디 메쉬에 쓰임) */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_FRN_detail"),
		TEXT("Com_DetailTexture"), reinterpret_cast<CComponent**>(&m_pDetailTextureCom))))
		return E_FAIL;

	/* For.Team Diffuse_Gold */
	if (m_iPlayerTeam == 1)
	{
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_FRN_base_1P"),
			TEXT("Com_Base_1P_Texture"), reinterpret_cast<CComponent**>(&m_p1PTextureCom))))
			return E_FAIL;
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_FRG_base_1P"),
			TEXT("Com_Gold_1P_Texture"), reinterpret_cast<CComponent**>(&m_pGold_BaseTexture))))
			return E_FAIL;
	}
	else
	{
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_FRN_base_1P"),
			TEXT("Com_Base_2P_Texture"), reinterpret_cast<CComponent**>(&m_p2PTextureCom))))
			return E_FAIL;
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_FRG_base_1P"),
			TEXT("Com_Gold_2P_Texture"), reinterpret_cast<CComponent**>(&m_pGold_BaseTexture))))
			return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_FRN_metal"),
		TEXT("Com_Metal_Texture"), reinterpret_cast<CComponent**>(&m_pMetalTexture))))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlay_Frieza::Bind_ShaderResources()
{

	if (FAILED(__super::Bind_ShaderResources()))
		return E_FAIL;

	return S_OK;
}

_bool CPlay_Frieza::Check_bCurAnimationisGroundMove(_uint iAnimation)
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

_bool CPlay_Frieza::Check_bCurAnimationisAttack(_uint iAnimation)
{

	return false;
}

_bool CPlay_Frieza::Check_bCurAnimationisAirAttack(_uint iAnimation)
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

_bool CPlay_Frieza::Check_bCurAnimationisHalfGravityStop(_uint iAnimation)
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

_short CPlay_Frieza::Check_bCurAnimationisCanChase()
{
	_uint 	iModelIndex = m_pModelCom->m_iCurrentAnimationIndex;



	//공격중 체이스는 속도 좀 빠르게
	if (m_bAttackBackEvent && (iModelIndex == ANIME_ATTACK_HEAVY || iModelIndex == ANIME_ATTACK_CROUCH_HEAVY || iModelIndex == ANIME_ATTACK_SPECIAL_AIR || iModelIndex == ANIME_ATTACK_LIGHT3 ||
		iModelIndex == ANIME_ATTACK_CROUCH_SPECIAL))
	{
		return 10;
	}

	else if (m_bAttackBackEvent && (iModelIndex == ANIME_ATTACK_CROUCH_CROUCH_SPECIAL))
	{
		return 15;
	}


	else if (Check_bCurAnimationisGroundMove() || Check_bCurAnimationisAirMove())
	{
		return 1;
	}


	return 0;

}

void CPlay_Frieza::Reset_AttackCount()
{

	__super::Reset_AttackCount();

	for (size_t i = 0; i < COUNT_END; i++)
		m_bAttackCount[i] = true;

	m_iCountGroundSpecial = 0;

	//감소하는방식.
	m_iCountAirSpecial = 5;


}

void CPlay_Frieza::Gravity(_float fTimeDelta)
{

	//if (m_bAttackGravity && m_pModelCom->m_iCurrentAnimationIndex == ANIME_ATTACK_236_AIR)
	//{
	//	if (Get_fHeight() > 0)
	//	{
	//
	//	
	//		_float fGravity = (-0.7f * (2 * m_fGravityTime - m_fJumpPower) * (2 * m_fGravityTime - m_fJumpPower) + 4) * 0.1;
	//
	//		Add_Move({ m_fImpuse.x *fTimeDelta, -fGravity });
	//	}
	//
	//	if (m_fGravityTime * 2.f < m_fJumpPower)
	//	{
	//		m_fGravityTime += fTimeDelta;
	//
	//		//일시정지해서 여기서 넘어가면 값이 확 높아질 수 있음.  예외처리해서 시간값 한번 더?
	//	}
	//
	//}

	if (m_pModelCom->m_iCurrentAnimationIndex == ANIME_ATTACK_SPECIAL_AIR || m_pModelCom->m_iCurrentAnimationIndex == ANIME_ATTACK_SPECIAL_AIR2)
	{
		return;
	}


	__super::Gravity(fTimeDelta);

	//프리저는 공중 강공, 공중 어퍼가 똑같은 기술이 나가서 중력 처리가 안되므로 여기서 해결해야함
	if (m_pModelCom->m_iCurrentAnimationIndex == ANIME_ATTACK_AIR3)
	{
		m_fGravityTime += fTimeDelta;
		_float fGravity = (-0.7f * (2 * m_fGravityTime - m_fJumpPower) * (2 * m_fGravityTime - m_fJumpPower) + 4) * 0.1;

		Add_Move({ m_fImpuse.x * fTimeDelta, -fGravity });
	}
	else if (m_bAttackGravity && (m_pModelCom->m_iCurrentAnimationIndex == ANIME_ATTACK_CROUCH_CROUCH_SPECIAL))
	{
		m_fGravityTime += fTimeDelta;
		_float fGravity = (-0.7f * (2 * m_fGravityTime - m_fJumpPower) * (2 * m_fGravityTime - m_fJumpPower) + 4) * 0.1;

		Add_Move({ m_fImpuse.x * fTimeDelta, -fGravity });
	}
	else if (m_pModelCom->m_iCurrentAnimationIndex == ANIME_214_FINAL && m_pModelCom->m_fCurrentAnimPosition < 124)
	{
		Add_Move({ m_fImpuse.x * fTimeDelta, m_fImpuse.y * fTimeDelta });
	}
	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_float fHeight = XMVectorGetY(vPos);
	_ushort iAnimationIndex = m_pModelCom->m_iCurrentAnimationIndex;


	// iAnimationIndex == ANIME_ATTACK_236_SPECIAL_AIR || iAnimationIndex == ANIME_ATTACK_236_SPECIAL_AIR_DOWNSIDE || 


	if (iAnimationIndex == ANIME_ATTACK_AIR1 || iAnimationIndex == ANIME_ATTACK_AIR2 || iAnimationIndex == ANIME_ATTACK_AIR3 ||
		iAnimationIndex == ANIME_ATTACK_SPECIAL_AIR || iAnimationIndex == ANIME_ATTACK_CROUCH_CROUCH_SPECIAL)
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
		}

	}

	else if (iAnimationIndex == ANIME_214_FINAL && m_pModelCom->m_fCurrentAnimPosition < 124)
	{
		if (fHeight <= 0)
		{
			Set_CurrentAnimationPositionJump(124.999);

			//m_pTransformCom-

			if (m_p214FinalEffect != nullptr)
			{
				m_p214FinalEffect->m_bIsDoneAnim = true;
				m_p214FinalEffect = nullptr;
			}


			Character_Make_Effect(TEXT("Crash_Smoke"));
			m_pRenderInstance->Get_Instance()->Switch_BlackOut(false);

			//CEffect_Layer* pEffect =Character_Make_Effect(TEXT("Crash_Smoke"));
			//pEffect->Set_Copy_Layer_Scaled({ 2.f, 2.f, 1.f });

			//Character_Make_Effect(TEXT("Smoke04"),{1.f,0.f},true);
			//if(m_iLookDirection == 1)
			//	Character_Make_Effect(TEXT("Smoke03_Five_Dir"));
			//else
			//	Character_Make_Effect(TEXT("Smoke03_Five_Dir_Rotated_Right"));

				

		}
	}

}

void CPlay_Frieza::AttackEvent(_int iAttackEvent, _int AddEvent)
{
	switch (m_pModelCom->m_iCurrentAnimationIndex)
	{
	case Client::CPlay_Frieza::ANIME_ATTACK_LIGHT1:
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
	case Client::CPlay_Frieza::ANIME_ATTACK_LIGHT2:
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
	}
	break;
	case Client::CPlay_Frieza::ANIME_ATTACK_LIGHT3:
	{
		//잡기판정
		if (iAttackEvent == 0)
		{
			CAttackObject_CommandGrab::ATTACK_COMMANDGRAB_DESC Desc{};
			if (m_iPlayerTeam == 1)
				Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_1P_Melee_Attack;
			else
				Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_2P_Melee_Attack;
			Desc.ColliderDesc.pMineGameObject = this;
			Desc.ColliderDesc.vExtents = { 1.f,1.f,1.f };


			_vector vEnemyPos = m_pEnemy->Get_vPosition();
			_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

			_vector vLength = vEnemyPos - vMyPos;

			//적 위치에 생성
			//Desc.ColliderDesc.vCenter = { XMVectorGetX(vLength),XMVectorGetY(vLength),0.f };

			//가로로 직선
			Desc.ColliderDesc.vCenter = {7.f * m_iLookDirection,0.4f,0.f };
			Desc.ColliderDesc.vExtents = { 8.f,1.f,1.f };

			//Desc.ColliderDesc.pTransform = m_pTransformCom;
			//Desc.fhitCharacter_Impus = { 0.3f * m_iLookDirection,0 };
			Desc.fhitCharacter_StunTime = 1.f;
			Desc.iDamage = 100 * Get_DamageScale();;
			Desc.fLifeTime = 0.2f;
			Desc.ihitCharacter_Motion = { HitMotion::HIT_WALLBOUNCE };

			//Desc.ihitCharacter_Motion = { HitMotion::HIT_LIGHT };
			//Desc.ihitCharacter_Motion = { HitMotion::HIT_CROUCH_MEDIUM };

			Desc.iTeam = m_iPlayerTeam;
			Desc.fAnimationLockTime = 0.3f;
			Desc.pOwner = this;

			Desc.eAttackType = ATTACKTYPE_HIGH;

			Desc.fDistance = { XMVectorGetX(vLength),XMVectorGetY(vLength) + 0.2f };
			Desc.fhitCharacter_Impus = { 0.f,2.5f };
			//Desc.fGrabAnimationPosition = 40.f;
			//Desc.fGrabAnimationPosition = 25.f;
			Desc.iGainAttackStep = 0;
			Desc.bOwnerNextAnimation = true;

			Desc.iGrabAnimationIndex = ANIME_ATTACK_LIGHT3;
			Desc.bOwnerNextAnimation = false;
			Desc.bGrabedGravity = true;
			Desc.fForcedGravityTime = 0.1f;


			Desc.iCallAttackBackIndex = 1001;


			//Desc.iVirtualCameraindex = CMain_Camera::VIRTUAL_CAMERA_21_GRAB_SPECIAL;dd

			m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack_CommandGrab"), TEXT("Layer_AttackObject"), &Desc);

			Character_Create_Distortion({ 1.f,0.f,0.f }, { 7.f*m_iLookDirection,0.f }, { 10.f,0.7f }, 0.07f);
		}
		else if (iAttackEvent == 1001)
		{
			CMain_Camera* mainCamera = static_cast<CMain_Camera*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Main_Camera")));
			if (Get_iDirection() == 1)
			{
				mainCamera->Play(CMain_Camera::VIRTUAL_CAMERA::VIRTUAL_CAMERA_FRIEZA_LIGHT_FINAL, 0, this, m_pEnemy, true);
			}
			else if (Get_iDirection() == -1)
			{
				mainCamera->Play(CMain_Camera::VIRTUAL_CAMERA::VIRTUAL_CAMERA_FRIEZA_LIGHT_FINAL, 1, this, m_pEnemy, true);
			}

			m_pEnemy->Set_bForcedAura(true);
			m_pEnemy->Set_bAura(true);
			m_pEnemy->Set_fAuraColor({ 6.76f, 1.5333f, 27.86f, 5.490f });
	
		}
		// AttackBack 실패시 55로 이동  성공시 애니메이션 속도 조절
		else if (iAttackEvent == 1)
		{

			if (m_bAttackBackEvent == false)
			{
				Set_CurrentAnimationPositionJump(54.99f);
			}
			else
			{
				m_pModelCom->Get_pCurrentAnimation()->m_fTickPerSecond = 20.f;
			}

		}
		else if (iAttackEvent == 101)
		{
			Set_AnimationStopWithoutMe(2.f);
		}
		//적 상승, 애니메이션 2초 정지?
		else if (iAttackEvent == 2)
		{
			//Set_AnimationStopWithoutMe(1.f);
			Set_AnimationStop(1.f);

			CMain_Camera* mainCamera = static_cast<CMain_Camera*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Main_Camera")));
			if (Get_iDirection() == 1)
			{
				mainCamera->Play(CMain_Camera::VIRTUAL_CAMERA::VIRTUAL_CAMERA_FRIEZA_LIGHT_FINAL, 2, this);
			}
			else if (Get_iDirection() == -1)
			{
				mainCamera->Play(CMain_Camera::VIRTUAL_CAMERA::VIRTUAL_CAMERA_FRIEZA_LIGHT_FINAL, 3, this, nullptr, true);
			}
		}

		//폭발  Position 40
		else if (iAttackEvent == 3)
		{
			CAttackObject::ATTACK_DESC Desc{};

			if (m_iPlayerTeam == 1)
				Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_1P_Melee_Attack;
			else
				Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_2P_Melee_Attack;
			Desc.ColliderDesc.pMineGameObject = this;
			Desc.ColliderDesc.vExtents = { 0.2f,1.f,1.f };


			//center를 상대 좌표로 할게 아니라 나~상대 까지의 거리로 해야함

			_vector vEnemyPos = m_pEnemy->Get_vPosition();
			_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

			_vector vLength = vEnemyPos - vMyPos;

			Desc.ColliderDesc.vCenter = { XMVectorGetX(vLength),XMVectorGetY(vLength),0.f };


			Desc.fhitCharacter_StunTime = 0.4f;
			Desc.iDamage = 800 * Get_DamageScale();;
			Desc.fLifeTime = 0.2f;
			//Desc.ihitCharacter_Motion = { HitMotion::HIT_KNOCK_AWAY_UP_GRAVITY };
			//Desc.fhitCharacter_Impus = { 0.5f * m_iLookDirection,8.f };

			//Desc.ihitCharacter_Motion = { HitMotion::HIT_KNOCK_AWAY_LEFTDOWN };
			Desc.ihitCharacter_Motion = { HitMotion::HIT_SPIN_AWAY_LEFTUP };


			Desc.fhitCharacter_Impus = { 3.f * m_iLookDirection, 1.f };

			Desc.iTeam = m_iPlayerTeam;
			Desc.fAnimationLockTime = 0.1f;
			Desc.bGrabbedEnd = true;
			Desc.pOwner = this;
			Desc.bCameraZoom = false;
			Desc.bOnwerHitNoneStop = true;

			m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack"), TEXT("Layer_AttackObject"), &Desc);


			CEffect_Layer* pEffect = m_pEnemy->Character_Make_Effect(TEXT("BurstJ3-Hit01"));
			pEffect->Set_Copy_Layer_Scaled({ 2.f, 2.f, 2.f });

			m_pEnemy->Set_bAura(false);
			m_pEnemy->Set_bAura(true);
			m_pEnemy->Set_bAura(false);
			m_pEnemy->Set_bForcedAura(false);

		}

	}
	break;
	case Client::CPlay_Frieza::ANIME_ATTACK_MEDIUM:
	{

		//거리가 가까우면 바로 공격 직전으로
		if (iAttackEvent == 1)
		{
			//_float fEnemyPosX = m_pEnemy->Get_fPositionX();

			_float fLength = m_pEnemy->Get_fPositionX() - Get_fPositionX();

			if (abs(fLength) < 1.1f)
			{
				Set_CurrentAnimationPositionJump(40.f);
			}

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
			Desc.ColliderDesc.vExtents = { 0.6f,0.6f,1.f };
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

	}
	break;
	case Client::CPlay_Frieza::ANIME_ATTACK_HEAVY:
	{
		m_bGrab_Air = false;

		if (iAttackEvent == 0)
		{



			CAttackObject_CommandGrab::ATTACK_COMMANDGRAB_DESC Desc{};
			if (m_iPlayerTeam == 1)
				Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_1P_Melee_Attack;
			else
				Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_2P_Melee_Attack;




			Desc.ColliderDesc.vCenter = { 1.5f * m_iLookDirection,0.4f,0.f };


			Desc.ColliderDesc.vExtents = { 1.f,1.0f,1.f };
			if (m_bGrabDraw)
			{
				m_bGrabDraw = false;
				Desc.ColliderDesc.vExtents = { 2.f,2.0f,1.f };
			}

			Desc.ColliderDesc.pMineGameObject = this;

			Desc.fhitCharacter_StunTime = 10.f;
			Desc.iDamage = 400;
			Desc.fLifeTime = 0.2f;
			Desc.ihitCharacter_Motion = { HitMotion::HIT_LIGHT };
			Desc.iTeam = m_iPlayerTeam;
			Desc.fAnimationLockTime = 0.7f;
			//Desc.bOnwerHitNoneStop = true;
			Desc.pOwner = this;
			Desc.bCameraZoom = false;

			Desc.eAttackType = ATTACKTYPE_HIGH;


			//_float fLength = m_pEnemy->Get_fPositionX() - Get_fPositionX();
			//Desc.fDistance = { fLength,0.f };


			//Desc.fDistance = { 1.3f * m_iLookDirection,0.1f };
			Desc.fDistance = { 2.3f * m_iLookDirection,0.1f };


			Desc.iGainAttackStep = 0;
			Desc.iGainKiAmount = 0;

			Desc.bOwnerNextAnimation = false;

			Desc.iGrabAnimationIndex = ANIME_ATTACK_HEAVY;

			//Desc.iVirtualCameraindex = CMain_Camera::VIRTUAL_CAMERA_HIT_1_ULTIMATE;
			//Desc.ianimationIndex = 1;

			Desc.fCameraShakeDuration = 1.f;
			Desc.fCameraShakeMagnitude = 0.2f;
			Desc.iVirtualCameraindex = CMain_Camera::VIRTUAL_CAMERA_FRIEZA_HEAVY;

			m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack_CommandGrab"), TEXT("Layer_AttackObject"), &Desc);


			//m_pAttackHeavyEffect = Character_Make_Effect(TEXT("FZ_BurstK"), { 0.4f,-0.2f });
			m_pAttackHeavyEffect = Character_Make_Effect(TEXT("FZ_BurstK"), { 1.4f,-0.2f });


		}
		else if (iAttackEvent == 1)
		{
			if (m_pAttackHeavyEffect != nullptr)
			{
				m_pAttackHeavyEffect->m_bIsDoneAnim = true;
				m_pAttackHeavyEffect = nullptr;
			}

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

			Desc.bGrabbedEnd = true;

			Desc.fhitCharacter_Impus = { 20.f * m_iLookDirection,0 };
			Desc.fhitCharacter_StunTime = 0.35f;
			Desc.iDamage = 150 * Get_DamageScale();;
			Desc.fLifeTime = 2.f;


			Desc.bExplosion = false;



			Desc.iTeam = m_iPlayerTeam;

			Desc.bCameraZoom = false;
			Desc.fAnimationLockTime = 0.f;

			Desc.pOwner = this;

			//Desc.eAttackType = { ATTACKTYPE_HIGH };
			//Desc.fStartOffset = { 0.2f * m_iLookDirection, 0.9f };
			//Desc.fRanged_Impus_NoneDirection = { 13.f,0.f };
			Desc.fRanged_Impus_NoneDirection = { 21.f,0.f };

			Desc.iDirection = m_iLookDirection;
			Desc.eRangeColor = CAttackObject_Ranged::RANGED_LIGHT_PURPLE;

			Desc.iGainKiAmount = 1;

			//Desc.ihitCharacter_Motion = { HitMotion::HIT_KNOCK_AWAY_LEFT };
			//Desc.fStartOffset = { 0.2f * m_iLookDirection +0.2f,1.7f };
			//Desc.strEffectName = TEXT("FZ_BurstK-01");
			//m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack_Ranged"), TEXT("Layer_AttackObject"), &Desc);
			//
			//
			//Desc.fStartOffset = { 0.2f * m_iLookDirection +0.7f,1.5f };
			//Desc.ihitCharacter_Motion = { HitMotion::HIT_KNOCK_AWAY_LEFT };
			//Desc.strEffectName = TEXT("FZ_BurstK-02");
			//m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack_Ranged"), TEXT("Layer_AttackObject"), &Desc);
			//
			//Desc.fStartOffset = { 0.2f * m_iLookDirection -0.3f,0.7f };
			//Desc.ihitCharacter_Motion = { HitMotion::HIT_KNOCK_AWAY_LEFT };
			//Desc.strEffectName = TEXT("FZ_BurstK-03");
			//m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack_Ranged"), TEXT("Layer_AttackObject"), &Desc);
			//
			//
			//Desc.fStartOffset = { 0.2f * m_iLookDirection + 0.2f,0.7f };
			//Desc.ihitCharacter_Motion = { HitMotion::HIT_KNOCK_AWAY_LEFT };
			//Desc.strEffectName = TEXT("FZ_BurstK-04");
			//m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack_Ranged"), TEXT("Layer_AttackObject"), &Desc);
			//
			//Desc.fStartOffset = { 0.2f * m_iLookDirection + 0.9f,0.6f };
			//Desc.ihitCharacter_Motion = { HitMotion::HIT_KNOCK_AWAY_LEFT };
			//Desc.strEffectName = TEXT("FZ_BurstK-05");
			//m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack_Ranged"), TEXT("Layer_AttackObject"), &Desc);
			//
			//Desc.fStartOffset = { 0.2f * m_iLookDirection - 0.3f,0.4f };
			//Desc.ihitCharacter_Motion = { HitMotion::HIT_KNOCK_AWAY_LEFT };
			//Desc.strEffectName = TEXT("FZ_BurstK-06");
			//m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack_Ranged"), TEXT("Layer_AttackObject"), &Desc);

			Desc.ihitCharacter_Motion = { HitMotion::HIT_KNOCK_AWAY_LEFT };
			Desc.fStartOffset = { 1.4f * m_iLookDirection ,1.7f };
			Desc.strEffectName = TEXT("FZ_BurstK-01");
			m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack_Ranged"), TEXT("Layer_AttackObject"), &Desc);


			Desc.fStartOffset = { 1.9f * m_iLookDirection ,1.5f };
			Desc.ihitCharacter_Motion = { HitMotion::HIT_KNOCK_AWAY_LEFT };
			Desc.strEffectName = TEXT("FZ_BurstK-02");
			m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack_Ranged"), TEXT("Layer_AttackObject"), &Desc);

			Desc.fStartOffset = { 0.9f * m_iLookDirection,0.7f };
			Desc.ihitCharacter_Motion = { HitMotion::HIT_KNOCK_AWAY_LEFT };
			Desc.strEffectName = TEXT("FZ_BurstK-03");
			m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack_Ranged"), TEXT("Layer_AttackObject"), &Desc);


			Desc.fStartOffset = { 1.4f * m_iLookDirection ,0.7f };
			Desc.ihitCharacter_Motion = { HitMotion::HIT_KNOCK_AWAY_LEFT };
			Desc.strEffectName = TEXT("FZ_BurstK-04");
			m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack_Ranged"), TEXT("Layer_AttackObject"), &Desc);

			Desc.fStartOffset = { 2.1f * m_iLookDirection,0.6f };
			Desc.ihitCharacter_Motion = { HitMotion::HIT_KNOCK_AWAY_LEFT };
			Desc.strEffectName = TEXT("FZ_BurstK-05");
			m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack_Ranged"), TEXT("Layer_AttackObject"), &Desc);

			Desc.fStartOffset = { 0.9f * m_iLookDirection,0.4f };
			Desc.ihitCharacter_Motion = { HitMotion::HIT_KNOCK_AWAY_LEFT };
			Desc.strEffectName = TEXT("FZ_BurstK-06");
			m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack_Ranged"), TEXT("Layer_AttackObject"), &Desc);


		}

	}
	break;
	case Client::CPlay_Frieza::ANIME_ATTACK_SPECIAL:
	{
		if (iAttackEvent == 0)
		{
			CAttackObject_Ranged::ATTACK_RANGED_DESC Desc{};
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
			Desc.fLifeTime = 2.f;






			Desc.ihitCharacter_Motion = { HitMotion::HIT_LIGHT };
			Desc.fhitCharacter_Impus = { 0.2f * m_iLookDirection,0 };


			Desc.iTeam = m_iPlayerTeam;

			Desc.bCameraZoom = false;
			Desc.fAnimationLockTime = 0.1f;
			Desc.bOnwerHitNoneStop = true;

			Desc.pOwner = this;

			//Desc.eAttackType = { ATTACKTYPE_HIGH };


			const _float4x4* pMatrix = m_pModelCom->Get_BoneMatrixPtr("GD_fng_b3_L");
			Desc.fStartOffset = { pMatrix->_41 * m_iLookDirection,pMatrix->_42 };
			//Desc.fStartOffset = { 0.2f * m_iLookDirection, 0.9f };


			Desc.fRanged_Impus_NoneDirection = { 30.f,0.f };
			Desc.iDirection = m_iLookDirection;
			Desc.eRangeColor = CAttackObject_Ranged::RANGED_LIGHT_PURPLE;
			Desc.strEffectName = TEXT("FZ_BurstJ-02");

			Desc.bExplosion = false;
			Desc.iGainKiAmount = 3;

			Desc.bPierce = true;

			m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack_Ranged"), TEXT("Layer_AttackObject"), &Desc);

			Character_Make_BoneEffect("GD_fng_b3_L", TEXT("FZ_BurstJ-01"));

		}
		else if (iAttackEvent == 1)
		{
			// != 연산 안되서 이어붙힘
			//특수버튼 말고 다른거 누르면 종료.

			m_pGameInstance->Play_Sound(CSound_Manager::SOUND_KEY_NAME::Frieza_J_Attack_SFX, false, 1.f);

			if (inputBuffer.size() == 0)
			{
				inputBuffer.push_back(CInput(MOVEKEY_NEUTRAL, ATTACK_NONE));
			}

			if (!((inputBuffer.back() == CInput(MOVEKEY_NEUTRAL, ATTACK_SPECIAL)) || (inputBuffer.back() == CInput(MOVEKEY_NEUTRAL, ATTACK_NONE))))
			{
				m_pModelCom->Get_pCurrentAnimation()->m_fTickPerSecond = 120.f;
				Set_CurrentAnimationPositionJump(80.f);
			}
			else
			{
				CAttackObject_Ranged::ATTACK_RANGED_DESC Desc{};
				if (m_iPlayerTeam == 1)
					Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_1P_Ranged_Attack;
				else
					Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_2P_Ranged_Attack;
				Desc.ColliderDesc.pMineGameObject = this;
				Desc.ColliderDesc.vExtents = { 0.2f,0.2f,1.f };
				Desc.ColliderDesc.vCenter = { 0.f,0.f,0.f };









				//Desc.ihitCharacter_Motion = { HitMotion::HIT_SPIN_AWAY_LEFTUP };
				//Desc.fhitCharacter_Impus = { 8.f * m_iLookDirection, 0.3f };
				//Desc.fhitCharacter_StunTime = 0.1f;


				//막타면 판정 다르게

				if (m_pModelCom->m_fCurrentAnimPosition > 69)
				{
					Desc.iDamage = 500 * Get_DamageScale();
					Desc.ihitCharacter_Motion = { HitMotion::HIT_SPIN_AWAY_LEFTUP };
					Desc.fhitCharacter_Impus = { 4.f * m_iLookDirection,1.f };
				}
				else
				{
					Desc.iDamage = 300 * Get_DamageScale();
					Desc.ihitCharacter_Motion = { HitMotion::HIT_LIGHT };
					Desc.fhitCharacter_Impus = { 0.2f * m_iLookDirection,0 };
				}
				Desc.fhitCharacter_StunTime = 0.35f;
				Desc.fLifeTime = 2.f;

				Desc.iTeam = m_iPlayerTeam;

				Desc.bCameraZoom = false;
				Desc.fAnimationLockTime = 0.f;

				Desc.pOwner = this;

				//Desc.eAttackType = { ATTACKTYPE_HIGH };
				//Desc.fStartOffset = { 0.2f * m_iLookDirection, 0.9f };


				const _float4x4* pMatrix = m_pModelCom->Get_BoneMatrixPtr("GD_fng_b3_L");
				Desc.fStartOffset = { pMatrix->_41 * m_iLookDirection,pMatrix->_42 };

				Desc.fRanged_Impus_NoneDirection = { 30.f,0.f };
				Desc.iDirection = m_iLookDirection;
				Desc.eRangeColor = CAttackObject_Ranged::RANGED_LIGHT_PURPLE;
				Desc.strEffectName = TEXT("FZ_BurstJ-02");

				Desc.iGainKiAmount = 3;
				Desc.bExplosion = false;

				Desc.bPierce = true;
				m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack_Ranged"), TEXT("Layer_AttackObject"), &Desc);

				Character_Make_BoneEffect("GD_fng_b3_L", TEXT("FZ_BurstJ-01"));
			}
		}
	}
	break;
	case Client::CPlay_Frieza::ANIME_ATTACK_AIR1:
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
			Desc.ColliderDesc.vExtents = { 0.4f,0.4f,1.f };
			Desc.ColliderDesc.vCenter = { 0.9f * m_iLookDirection,1.0f,0.f };
		}


		if (abs(m_fImpuse.x) < 0.3)
			Desc.fhitCharacter_Impus = { m_fImpuse.x * m_iLookDirection,0 };
		else
			Desc.fhitCharacter_Impus = { 0.3f * m_iLookDirection,0 };

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

		m_pGameInstance->Play_Group_Sound(CSound_Manager::SOUND_GROUP_KEY::Light_Attack_Frieza, false, 1.f);
		m_pGameInstance->Play_Group_Sound(CSound_Manager::SOUND_GROUP_KEY::LIGHT_ATTACK_Hit_SFX, false, 1.f);
	}
	break;
	case Client::CPlay_Frieza::ANIME_ATTACK_AIR2:
	{
		if (iAttackEvent == 0)
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


			if (abs(m_fImpuse.x) < 0.2)
				Desc.fhitCharacter_Impus = { m_fImpuse.x * m_iLookDirection,0 };
			else
				Desc.fhitCharacter_Impus = { 0.2f * m_iLookDirection,0 };

			Desc.fhitCharacter_StunTime = 0.4f;

			if (Get_fHeight() > 2)
				Desc.fhitCharacter_StunTime = 0.6f;


			Desc.iDamage = 700 * Get_DamageScale();
			Desc.fLifeTime = 0.1f;
			Desc.ihitCharacter_Motion = { HitMotion::HIT_LIGHT };
			Desc.iTeam = m_iPlayerTeam;
			//Desc.fAnimationLockTime = 0.04f;
			Desc.fAnimationLockTime = 0.11f;

			//Desc.bOwnerGravityTimeReset = true;
			Desc.pOwner = this;
			Desc.eAttackType = { ATTACKTYPE_MIDDLE };
			Desc.iGainKiAmount = 3;

			Desc.fForcedGravityTime = m_fGravityTime - 0.08f;

			m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack"), TEXT("Layer_AttackObject"), &Desc);

			m_pGameInstance->Play_Group_Sound(CSound_Manager::SOUND_GROUP_KEY::Light_Attack_Frieza, false, 1.f);
			m_pGameInstance->Play_Group_Sound(CSound_Manager::SOUND_GROUP_KEY::LIGHT_ATTACK_Hit_SFX, false, 1.f);
		}
		else if (iAttackEvent == 1)
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
				Desc.fAnimationLockTime = 0.11f;

				//Desc.bOwnerGravityTimeReset = true;
				Desc.pOwner = this;
				Desc.eAttackType = { ATTACKTYPE_MIDDLE };
				Desc.iGainKiAmount = 3;
				//Desc.fForcedGravityTime = m_fGravityTime;
				Desc.fForcedGravityTime = m_fGravityTime - 0.08f;

				Desc.iGainAttackStep = 0;

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
				Desc.iGainKiAmount = 0;

				Desc.bCameraZoom = false;
				m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack"), TEXT("Layer_AttackObject"), &Desc);
				Character_Create_Distortion({ 1.f,0.f,0.f }, { 0.f,0.f }, { 1.f,1.f }, 0.2f);

			}
		}


	}
	break;
	case Client::CPlay_Frieza::ANIME_ATTACK_AIR3:
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

		m_pGameInstance->Play_Sound(CSound_Manager::SOUND_KEY_NAME::Frieza_Heavy_Attack, false, 1.f);
	}
	break;
	case Client::CPlay_Frieza::ANIME_ATTACK_SPECIAL_AIR:
	{
		//Position 45. 공격
		if (iAttackEvent == 0)
		{
			m_pGameInstance->Play_Sound(CSound_Manager::SOUND_KEY_NAME::Frieza_J_Attack, false, 1.f);
			m_pGameInstance->Play_Sound(CSound_Manager::SOUND_KEY_NAME::Frieza_J_Attack_SFX, false, 1.f);

			CAttackObject::ATTACK_DESC Desc{};
			if (m_iPlayerTeam == 1)
				Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_1P_Melee_Attack;
			else
				Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_2P_Melee_Attack;
			Desc.ColliderDesc.pMineGameObject = this;
			Desc.ColliderDesc.vExtents = { 1.f,1.f,1.f };
			Desc.ColliderDesc.vCenter = { 1.0f * m_iLookDirection,0.8f,0.f };


			//막타면
			if (m_iCountAirSpecial == 0)
			{
				Desc.iDamage = 600 * Get_DamageScale();
				Desc.ihitCharacter_Motion = { HitMotion::HIT_KNOCK_AWAY_LEFT };
				Desc.fAnimationLockTime = 0.3f;
				Desc.fhitCharacter_Impus = { 20.f * m_iLookDirection,0 };
			}
			else
			{
				Desc.iDamage = 500 * Get_DamageScale();
				Desc.ihitCharacter_Motion = { HitMotion::HIT_SPIN_AWAY_LEFTUP };
				Desc.bOnwerHitNoneStop = true; //막타 아닐떈 안멈춤
				Desc.fAnimationLockTime = 0.25f;
				Desc.fhitCharacter_Impus = { 3.f * m_iLookDirection,0 };

			}
			//Desc.fhitCharacter_Impus = { 20.f * m_iLookDirection,0 };
			//Desc.iDamage = 500 * Get_DamageScale();
			//Desc.ihitCharacter_Motion = { HitMotion::HIT_KNOCK_AWAY_LEFT };
			//Desc.bOnwerHitNoneStop = true;

			Desc.fhitCharacter_StunTime = 1.0f;
			///Desc.iDamage = 30000 * Get_DamageScale();;
			Desc.fLifeTime = 0.2f;
			Desc.iTeam = m_iPlayerTeam;
			//Desc.fAnimationLockTime = 0.25f;
			Desc.bGrabbedEnd = true;
			Desc.pOwner = this;
			Desc.iGainKiAmount = 2;

			Desc.bCameraZoom = false;
			m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack"), TEXT("Layer_AttackObject"), &Desc);


			Character_Make_Effect(TEXT("FZ_WJ"), { 0.7f,1.4f });
		}
		else if (iAttackEvent == 1)
		{

		}
		else if (iAttackEvent == 2)
		{

		}
		else if (iAttackEvent == 3)
		{

		}
	}
	break;
	case Client::CPlay_Frieza::ANIME_ATTACK_SPECIAL_AIR2:
	{
		//공격
		if (iAttackEvent == 0)
		{
			CAttackObject::ATTACK_DESC Desc{};
			if (m_iPlayerTeam == 1)
				Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_1P_Melee_Attack;
			else
				Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_2P_Melee_Attack;
			Desc.ColliderDesc.pMineGameObject = this;
			Desc.ColliderDesc.vExtents = { 1.f,1.f,1.f };
			Desc.ColliderDesc.vCenter = { 1.0f * m_iLookDirection,0.8f,0.f };

			Desc.fhitCharacter_StunTime = 1.0f;


			//막타면
			if (m_iCountAirSpecial == 0)
			{
				Desc.iDamage = 600 * Get_DamageScale();
				Desc.ihitCharacter_Motion = { HitMotion::HIT_KNOCK_AWAY_LEFT };
				Desc.fAnimationLockTime = 0.3f;
				Desc.fhitCharacter_Impus = { 20.f * m_iLookDirection,0 };
			}
			else
			{
				Desc.iDamage = 500 * Get_DamageScale();
				Desc.ihitCharacter_Motion = { HitMotion::HIT_SPIN_AWAY_LEFTUP };
				Desc.bOnwerHitNoneStop = true; //막타 아닐떈 안멈춤
				Desc.fAnimationLockTime = 0.25f;
				Desc.fhitCharacter_Impus = { 3.f * m_iLookDirection,0 };

			}
			///Desc.iDamage = 30000 * Get_DamageScale();
			Desc.fLifeTime = 0.2f;
			Desc.iTeam = m_iPlayerTeam;
			Desc.bGrabbedEnd = true;
			Desc.pOwner = this;
			Desc.iGainKiAmount = 2;


			Desc.bCameraZoom = false;
			m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack"), TEXT("Layer_AttackObject"), &Desc);


		}

		//Position 40에서 종료
		else if (iAttackEvent == 1)
		{
			Set_Animation(ANIME_JUMP_DOWN);
		}

		//공격 맞췄을 때 한번 더 누르면 여기로 옴 Position 45
		else if (iAttackEvent == 2)
		{
			//재진입 방지
			m_bAttackBackEvent = false;

		}

		//Position 55.  SPECIAL1의 Position 40으로 이동
		else if (iAttackEvent == 3)
		{
			Set_Animation(ANIME_ATTACK_SPECIAL_AIR);
			Set_CurrentAnimationPositionJump(40.f);
		}
	}
	break;
	case Client::CPlay_Frieza::ANIME_ATTACK_CROUCH_LIGHT:
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

		m_pGameInstance->Play_Group_Sound(CSound_Manager::SOUND_GROUP_KEY::Light_Attack_Frieza, false, 1.f);
		m_pGameInstance->Play_Group_Sound(CSound_Manager::SOUND_GROUP_KEY::LIGHT_ATTACK_Hit_SFX, false, 1.f);
	}
	break;
	case Client::CPlay_Frieza::ANIME_ATTACK_CROUCH_MEDUIM:
	{
		if (iAttackEvent == 0)
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

			Desc.fhitCharacter_Impus = { 2.f * m_iLookDirection,0.f };

			Desc.fhitCharacter_StunTime = 1.f;
			Desc.iDamage = 600 * Get_DamageScale();
			Desc.fLifeTime = 0.1f;
			Desc.ihitCharacter_Motion = { HitMotion::HIT_CROUCH_MEDIUM };
			Desc.iTeam = m_iPlayerTeam;
			Desc.fAnimationLockTime = 0.1f;
			//Desc.bOwnerGravityTimeReset = true;
			Desc.pOwner = this;
			Desc.iGainKiAmount = 4;

			Desc.eAttackType = { ATTACKTYPE_LOW };

			m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack"), TEXT("Layer_AttackObject"), &Desc);

			m_pGameInstance->Play_Group_Sound(CSound_Manager::SOUND_GROUP_KEY::LIGHT_ATTACK_Hit_SFX, false, 1.f);
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
			Desc.ColliderDesc.vCenter = { 0.9f * m_iLookDirection,0.8f,0.f };
			//Desc.ColliderDesc.pTransform = m_pTransformCom;
			//Desc.fhitCharacter_Impus = { 1.f * m_iLookDirection,0.3f };
			//Desc.fhitCharacter_Impus = { 1.5f * m_iLookDirection,0.3f };
			//Desc.ColliderDesc.vExtents = { 0.7f,0.8f,1.f };

			Desc.ColliderDesc.vExtents = { 0.7f,0.5f,1.f };
			Desc.ColliderDesc.vCenter = { 0.7f * m_iLookDirection,0.3f,0.f };

			Desc.fhitCharacter_Impus = { 4.f * m_iLookDirection,0.f };

			Desc.fhitCharacter_StunTime = 1.f;
			Desc.iDamage = 600 * Get_DamageScale();
			Desc.fLifeTime = 0.1f;
			Desc.ihitCharacter_Motion = { HitMotion::HIT_SPIN_AWAY_LEFTUP };
			Desc.iTeam = m_iPlayerTeam;
			Desc.fAnimationLockTime = 0.1f;
			//Desc.bOwnerGravityTimeReset = true;
			Desc.pOwner = this;
			Desc.iGainKiAmount = 4;

			Desc.iGainAttackStep = 0;

			Desc.eAttackType = { ATTACKTYPE_LOW };

			m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack"), TEXT("Layer_AttackObject"), &Desc);

			m_pGameInstance->Play_Group_Sound(CSound_Manager::SOUND_GROUP_KEY::Light_Attack_Frieza, false, 1.f);
			m_pGameInstance->Play_Group_Sound(CSound_Manager::SOUND_GROUP_KEY::LIGHT_ATTACK_Hit_SFX, false, 1.f);
		}
	}
	break;
	case Client::CPlay_Frieza::ANIME_ATTACK_CROUCH_HEAVY:
	{
		CAttackObject::ATTACK_DESC Desc{};
		if (m_iPlayerTeam == 1)
			Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_1P_Melee_Attack;
		else
			Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_2P_Melee_Attack;
		Desc.ColliderDesc.pMineGameObject = this;
		//Desc.ColliderDesc.vExtents = { 1.5f,1.3f,1.f };
		//Desc.ColliderDesc.vCenter = { 2.f * m_iLookDirection,1.2f,0.f };

		Desc.ColliderDesc.vExtents = { 1.5f,1.0f,1.f };
		Desc.ColliderDesc.vCenter = { 2.f * m_iLookDirection,1.0f,0.f };


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


		//CEffect_Layer* pEffect =Character_Make_Effect(TEXT("FZ_SDO-02"), { 2.f,0.f });

		Character_Make_Effect(TEXT("FZ_SDU-02"), { 2.f,0.f });
		//CEffect_Layer* pEffect = Character_Make_Effect(TEXT("FZ_SDU-02"), { 2.f,0.f });
		//pEffect->Set_Copy_Layer_Scaled({ 0.5f,0.5f,1.f });

	}
	break;
	case Client::CPlay_Frieza::ANIME_ATTACK_CROUCH_SPECIAL:
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

		Desc.fhitCharacter_Impus = { 3.f * m_iLookDirection,5.f };
		Desc.fhitCharacter_StunTime = 0.8f;


		Desc.iDamage = 600 * Get_DamageScale();;
		Desc.fLifeTime = 5.f;
		Desc.ihitCharacter_Motion = { HitMotion::HIT_LIGHT };

		Desc.iTeam = m_iPlayerTeam;

		Desc.bCameraZoom = false;
		Desc.fAnimationLockTime = 0.05f;
		Desc.fForcedGravityTime = 0.f;
		Desc.pOwner = this;
		Desc.bPierce = true;

		//Desc.eAttackType = { ATTACKTYPE_HIGH };

		const _float4x4* pMatrix = m_pModelCom->Get_BoneMatrixPtr("G_eyes_base");
		Desc.fStartOffset = { pMatrix->_41 * m_iLookDirection,pMatrix->_42 - 0.15f };


		Desc.fRanged_Impus_NoneDirection = { 24.f,15.f };  // 1:1.569 비율

		Desc.iDirection = m_iLookDirection;

		Desc.eRangeColor = CAttackObject_Ranged::RANGED_LIGHT_NONE;
		Desc.strEffectName = TEXT("FZ_SJ-02");
		Desc.fEffectRotationDegree = 30.f;
		Desc.iGainKiAmount = 7;
		Desc.bExplosion = false;

		Desc.bOnwerHitNoneStop = true;

		m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack_Ranged"), TEXT("Layer_AttackObject"), &Desc);

		m_pGameInstance->Play_Sound(CSound_Manager::SOUND_KEY_NAME::Frieza_J_Attack, false, 1.f);
		m_pGameInstance->Play_Sound(CSound_Manager::SOUND_KEY_NAME::Frieza_J_Attack_SFX, false, 1.f);

		Character_Make_BoneEffect("G_eye_L", TEXT("FZ_SJ-01"));
	}
	break;
	case Client::CPlay_Frieza::ANIME_ATTACK_CROUCH_CROUCH_SPECIAL:
	{
		if (iAttackEvent == 0)
		{
			//Set_AnimationStopWithoutMe(0.3f);
			//샤샤샥

			Set_AnimationStop(0.2f);

			Character_Create_Distortion({ 0.f,1.f,0.f });

			m_pTransformCom->Set_State(CTransform::STATE_POSITION, { m_pEnemy->Get_fPositionX() + m_iLookDirection * 1.5f,0.5f,0.f,1.f });
			FlipDirection();

			m_bAttackGravity = false;
			m_bInvisible = true;


			CMain_Camera* mainCamera = static_cast<CMain_Camera*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Main_Camera")));
			mainCamera->StartCameraShake(0.3f, 0.1f);

			Set_CurrentAnimationPositionJump(1.f);

		}
		else if (iAttackEvent == 1)
		{

			m_bAttackGravity = true;

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

			//Desc.fhitCharacter_Impus = { 3.f * m_iLookDirection,3.f };
			//Desc.fhitCharacter_StunTime = 0.6f;
			//Desc.ihitCharacter_Motion = { HitMotion::HIT_CROUCH_MEDIUM };


			//CROUCH_MEDIUM은 x값만 받음
			Desc.fhitCharacter_Impus = { 1.f * m_iLookDirection,1.f };
			Desc.fhitCharacter_StunTime = 0.7f;
			Desc.ihitCharacter_Motion = { HitMotion::HIT_SPIN_AWAY_LEFTUP };



			Desc.iDamage = 600 * Get_DamageScale();;
			Desc.fLifeTime = 5.f;


			Desc.iTeam = m_iPlayerTeam;

			Desc.bCameraZoom = false;
			Desc.fAnimationLockTime = 0.05f;

			Desc.pOwner = this;

			//Desc.eAttackType = { ATTACKTYPE_HIGH };
			Desc.fStartOffset = { 0.6f * m_iLookDirection, 0.f };
			//Desc.fRanged_Impus_NoneDirection = { 7.845f,-5.f };  // 1:1.569 비율
			Desc.fRanged_Impus_NoneDirection = { 15.69f,-10.f };  // 1:1.569 비율


			Desc.iDirection = m_iLookDirection;

			Desc.eRangeColor = CAttackObject_Ranged::RANGED_LIGHT_PURPLE;
			Desc.strEffectName = TEXT("FZ_SJ-02");
			Desc.fEffectRotationDegree = -45.f;
			Desc.iGainKiAmount = 7;

			Desc.bOnwerHitNoneStop = true;

			m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack_Ranged"), TEXT("Layer_AttackObject"), &Desc);
		}

		//,순간이동 직후 처리  Position : 5
		else if (iAttackEvent == 101)
		{
			//중복처리 방지가 필요한데
			//fabs(CurValue - TargetValue) <= 0.1;
			m_bInvisible = false;
			Character_Create_Distortion({ 0.f,1.f,0.f }, { 0.f,0.f,0.f }, { 0.7f,0.7f });
			//Set_ForcedGravityTime_LittleUp();
			Set_ForcedGravityDown();

		}

	}
	break;
	case Client::CPlay_Frieza::ANIME_SPARKING:
		break;
	case Client::CPlay_Frieza::ANIME_REFLECT:
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
			Desc.iGainKiAmount = 15;

			//m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack_Reflect"), TEXT("Layer_AttackObject"), &Desc);
			m_pReflectObject = m_pGameInstance->Add_GameObject_ToLayer_AndGet(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack_Reflect"), TEXT("Layer_AttackObject"), &Desc);


			m_bReflect = true;
		}
		if (iAttackEvent == 1)
		{
			if (m_bAttackBackEvent)
			{
				Set_Animation(CPlay_Frieza::ANIME_IDLE);

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
		break;
	case Client::CPlay_Frieza::ANIME_GOKU_CINEMATIC_01:
		break;
	case Client::CPlay_Frieza::ANIME_GOKU_CINEMATIC_02:
		break;
	case Client::CPlay_Frieza::ANIME_GOKU_CINEMATIC_03:
		break;
	case Client::CPlay_Frieza::ANIME_GRAB:
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
			Desc.ColliderDesc.vCenter = { 0.3f * m_iLookDirection,0.7f,0.f };
			//Desc.ColliderDesc.pTransform = m_pTransformCom;
			//Desc.fhitCharacter_Impus = { 0.3f * m_iLookDirection,0 };
			Desc.fhitCharacter_StunTime = 1.f;
			Desc.iDamage = 100 * Get_DamageScale();;
			Desc.fLifeTime = 0.2f;
			Desc.ihitCharacter_Motion = { HitMotion::HIT_MEDIUM };
			Desc.iTeam = m_iPlayerTeam;
			Desc.fAnimationLockTime = 0.1f;
			Desc.pOwner = this;

			Desc.iGainAttackStep = 2;


			if (m_bGrab_Air)
			{
				Desc.eAttackType = ATTACKTYPE_GRAB_AIR;
				Desc.fDistance = { 1.5f * m_iLookDirection,-0.3f };
			}
			else
			{
				Desc.eAttackType = ATTACKTYPE_GRAB_GROUND;
				Desc.fDistance = { 1.f * m_iLookDirection,0.f };

			}
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
			Desc.iDamage = 150 * Get_DamageScale();
			Desc.fLifeTime = 0.1f;
			Desc.ihitCharacter_Motion = { HitMotion::HIT_LIGHT };
			Desc.iTeam = m_iPlayerTeam;
			Desc.fAnimationLockTime = 0.f;
			Desc.iGainAttackStep = 0;
			Desc.pOwner = this;
			Desc.iGainKiAmount = 1;

			Desc.bDrawNoneStop = true;

			Desc.iGainAttackStep = 0;

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
			Desc.iDamage = 150 * Get_DamageScale();
			Desc.fLifeTime = 0.1f;
			Desc.ihitCharacter_Motion = { HitMotion::HIT_LIGHT };
			Desc.iTeam = m_iPlayerTeam;
			Desc.fAnimationLockTime = 0.f;
			Desc.iGainAttackStep = 0;
			Desc.pOwner = this;
			Desc.iGainKiAmount = 1;
			Desc.iGainAttackStep = 0;

			Desc.bDrawNoneStop = true;


			m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack"), TEXT("Layer_AttackObject"), &Desc);
			m_pGameInstance->Play_Group_Sound(CSound_Manager::SOUND_GROUP_KEY::LIGHT_ATTACK_Hit_SFX, false, 1.f);
		}
		break;

		}
	}
	break;
	case Client::CPlay_Frieza::ANIME_ATTACK_236:
	{
		if (iAttackEvent == 0)
		{
			CAttackObject::ATTACK_DESC Desc{};
			if (m_iPlayerTeam == 1)
				Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_1P_Melee_Attack;
			else
				Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_2P_Melee_Attack;
			Desc.ColliderDesc.pMineGameObject = this;
			//Desc.ColliderDesc.vExtents = { 1.5f,1.3f,1.f };
			//Desc.ColliderDesc.vCenter = { 2.f * m_iLookDirection,1.2f,0.f };

			Desc.ColliderDesc.vExtents = { 1.5f,3.0f,1.f };
			Desc.ColliderDesc.vCenter = { 4.f * m_iLookDirection,1.5f,0.f };


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
			Desc.fCameraShakeDuration = 0.5f;
			Desc.fCameraShakeMagnitude = 0.3f;
			m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack"), TEXT("Layer_AttackObject"), &Desc);

			if (m_bHeavySkill)
				Desc.bOnwerHitNoneStop = true;


			//CEffect_Layer* pEffect = Character_Make_Effect(TEXT("FZ_SDU-01"));
			//CEffect_Layer* pEffect = Character_Make_Effect(TEXT("FZ_SDU-01"),{0.f,0.5f});
			//pEffect->Set_Copy_Layer_Rotation({ 0.f,0.f,270.f });

			Character_Make_BoneEffect("GD_hand_R", TEXT("FZ_SDU-01"));

			CEffect_Layer* pEffect1 = Character_Make_Effect(TEXT("FZ_SDU-02"), { 3.7f,0.f });
			CEffect_Layer* pEffect2 = Character_Make_Effect(TEXT("FZ_SDU-02"), { 4.4f,0.f }, true);

			pEffect1->Set_Copy_Layer_Scaled({ 1.2f,1.2f,1.f });
			//pEffect2->Set_Copy_Layer_Scaled({ 1.0f,1.2f,1.f });

			//CEffect_Layer* pEffect = Character_Make_Effect(TEXT("FZ_SDU-02"), { 2.f,0.f });
		//pEffect->Set_Copy_Layer_Scaled({ 0.5f,0.5f,1.f });

		}

		//강공격용
		else if (iAttackEvent == 1)
		{
			if (m_bHeavySkill)
			{
				CAttackObject::ATTACK_DESC Desc{};
				if (m_iPlayerTeam == 1)
					Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_1P_Melee_Attack;
				else
					Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_2P_Melee_Attack;
				Desc.ColliderDesc.pMineGameObject = this;
				//Desc.ColliderDesc.vExtents = { 1.5f,1.3f,1.f };
				//Desc.ColliderDesc.vCenter = { 2.f * m_iLookDirection,1.2f,0.f };

				Desc.ColliderDesc.vExtents = { 1.5f,3.0f,1.f };
				Desc.ColliderDesc.vCenter = { 6.f * m_iLookDirection,1.5f,0.f };


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
				Desc.fCameraShakeDuration = 0.5f;
				Desc.fCameraShakeMagnitude = 0.3f;
				m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack"), TEXT("Layer_AttackObject"), &Desc);


				//CEffect_Layer* pEffect = Character_Make_Effect(TEXT("FZ_SDU-01"));
				//CEffect_Layer* pEffect = Character_Make_Effect(TEXT("FZ_SDU-01"),{0.f,0.5f});
				//pEffect->Set_Copy_Layer_Rotation({ 0.f,0.f,270.f });


					//CEffect_Layer* pEffect = Character_Make_Effect(TEXT("FZ_SDU-01"));
					//CEffect_Layer* pEffect = Character_Make_Effect(TEXT("FZ_SDU-01"),{0.f,0.5f});
					//pEffect->Set_Copy_Layer_Rotation({ 0.f,0.f,270.f });

				CEffect_Layer* pEffect1 = Character_Make_Effect(TEXT("FZ_SDU-02"), { 5.7f,0.f });
				CEffect_Layer* pEffect2 = Character_Make_Effect(TEXT("FZ_SDU-02"), { 6.4f,0.f }, true);
			}
		}
	}
	break;
	case Client::CPlay_Frieza::ANIME_ATTACK_236_SPECIAL:
	{
		// Position : 40 본인 애니메이션정지, 잡기판정 생성
		if (iAttackEvent == 0)
		{

			CAttackObject_CommandGrab::ATTACK_COMMANDGRAB_DESC Desc{};
			if (m_iPlayerTeam == 1)
				Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_1P_Melee_Attack;
			else
				Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_2P_Melee_Attack;
			Desc.ColliderDesc.pMineGameObject = this;
			Desc.ColliderDesc.vExtents = { 0.7f,0.7f,1.f };
			Desc.ColliderDesc.vCenter = { 0.8f * m_iLookDirection,0.f,0.f };
			//Desc.fDistance = { 1.f * m_iLookDirection, -0.3f };
			Desc.fDistance = { 0.8f * m_iLookDirection, -0.8f };



			//Desc.ColliderDesc.pTransform = m_pTransformCom;
			//Desc.fhitCharacter_Impus = { 0.3f * m_iLookDirection,0 };
			Desc.fhitCharacter_StunTime = 20.f;
			Desc.iDamage = 300 * Get_DamageScale();
			Desc.iDamage = 0;
			Desc.fLifeTime = 0.2f;
			Desc.ihitCharacter_Motion = { HitMotion::HIT_WALLBOUNCE };
			Desc.iTeam = m_iPlayerTeam;
			Desc.fAnimationLockTime = 0.3f;
			Desc.pOwner = this;

			Desc.eAttackType = ATTACKTYPE_HIGH;
			Desc.iGrabAnimationIndex = ANIME_ATTACK_236_SPECIAL;
			Desc.bOwnerNextAnimation = false;


			Desc.iGainKiAmount = 2;
			Desc.iGainAttackStep = 0;
			Desc.iVirtualCameraindex = (_uint)CMain_Camera::VIRTUAL_CAMERA_FRIEZA_LIGHT_FINAL;

			m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack_CommandGrab"), TEXT("Layer_AttackObject"), &Desc);

			//m_p236SpecialEffect=Character_Make_BoneEffect("GD_fist_R", TEXT("FZ_SDJ"));
			//m_p236SpecialEffect = Character_Make_Effect(TEXT("FZ_SDJ"),{0.4f,-0.3f});
			m_p236SpecialEffect = Character_Make_Effect(TEXT("FZ_SDJ"), { 0.7f,-0.1f });



		}

		// Position : 45 잡기기술 AttackBack 확인
		else if (iAttackEvent == 1)
		{
			if (m_bAttackBackEvent)
			{
				Set_AnimationStopWithoutMe(1.0f);
				Set_AnimationStop(0.8f);


				CAttackObject_Energy::ATTACK_RANGED_DESC Desc{};
				if (m_iPlayerTeam == 1)
					Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_1P_Energy_Attack;
				else
					Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_2P_Energy_Attack;
				Desc.ColliderDesc.pMineGameObject = this;
				Desc.ColliderDesc.vExtents = { 0.8f,0.8f,1.f };
				Desc.ColliderDesc.vCenter = { 0.2f * m_iLookDirection,0.f,0.f };
				//Desc.ColliderDesc.pTransform = m_pTransformCom;
				Desc.fhitCharacter_Impus = { 0.f,0.f };
				Desc.fhitCharacter_StunTime = 10.f;
				//Desc.iDamage = 120 * Get_DamageScale();

				Desc.iDamage = 60 * Get_DamageScale(true);

				Desc.fLifeTime = 2.f;
				Desc.ihitCharacter_Motion = { HitMotion::HIT_WALLBOUNCE };
				Desc.iTeam = m_iPlayerTeam;
				Desc.fAnimationLockTime = 0.0f;
				Desc.pOwner = this;
				Desc.bCameraZoom = false;

				Desc.fStartOffset = { 0.f,0.f };
				Desc.fMoveSpeedNoneDirection = { 5.f,0.f };

				Desc.iAttackCount = 10;
				Desc.iPlayerDirection = m_iLookDirection;		//
				Desc.iGainHitCount = 2;
				Desc.iGainAttackStep = 0;
				Desc.iGainKiAmount = 0;
				Desc.fAttackDelayTime = 0.05f;
				Desc.bOnwerHitNoneStop = true;



				Desc.eEnergyColor = CAttackObject_Energy::ENERGY_LIGHT_NONE;

				Desc.fColliderfCY = 2.f;


				Desc.bHeightCorrection = false;

				CMain_Camera* mainCamera = static_cast<CMain_Camera*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Main_Camera")));
				if (Get_iDirection() == 1)
				{
					mainCamera->Play(CMain_Camera::VIRTUAL_CAMERA::VIRTUAL_CAMERA_FRIEZA_DOWN_FORWARD_J, 0, this);
				}
				else if (Get_iDirection() == -1)
				{
					mainCamera->Play(CMain_Camera::VIRTUAL_CAMERA::VIRTUAL_CAMERA_FRIEZA_DOWN_FORWARD_J, 1, this, nullptr, true);
				}

				mainCamera->StartCameraShake(0.7f, 0.2f);

				m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack_Energy"), TEXT("Layer_AttackObject"), &Desc);


			}


		}

		//Position 60 : 발사
		else if (iAttackEvent == 2)
		{


			CAttackObject_Ranged::ATTACK_RANGED_DESC Desc{};
			//Desc.ColliderDesc.width = 1.0;
			//Desc.ColliderDesc.height = 1.0;
			if (m_iPlayerTeam == 1)
				Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_1P_Ranged_Attack;
			else
				Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_2P_Ranged_Attack;
			Desc.ColliderDesc.pMineGameObject = this;
			Desc.ColliderDesc.vExtents = { 0.8f,0.8f,1.f };
			Desc.ColliderDesc.vCenter = { 0.f,0.f,0.f };

			Desc.bGrabbedEnd = true;

			Desc.fhitCharacter_StunTime = 0.35f;
			Desc.iDamage = 1020 * Get_DamageScale();;
			Desc.fLifeTime = 5.f;





			Desc.iTeam = m_iPlayerTeam;

			Desc.bCameraZoom = false;
			Desc.fAnimationLockTime = 0.f;

			Desc.pOwner = this;

			//Desc.eAttackType = { ATTACKTYPE_HIGH };
			Desc.fStartOffset = { 0.2f * m_iLookDirection, 0.9f };

			//잡기 성공시 땅에 꽂힘
			if (m_bAttackBackEvent)
			{
				Desc.bGroundSmash = true;
				//Desc.fRanged_Impus_NoneDirection = { 7.f,-5.f };
				//Desc.fRanged_Impus_NoneDirection = { 14.f,-25.f };

				//어차피 필중
				//Desc.fRanged_Impus_NoneDirection = { 10.f,-15.f };
				Desc.fRanged_Impus_NoneDirection = { 10.f,-15.f };

				Desc.fhitCharacter_Impus = { 10.f * m_iLookDirection, -15.f };
				Desc.ihitCharacter_Motion = { HitMotion::HIT_KNOCK_AWAY_LEFTDOWN };
				Desc.fStartOffset = { -0.2f,-0.3f };
				//Desc.fStartOffset = { 0.f,-0.3f };


			}

			//else if (Get_fHeight() > 1)
			else if (Get_fHeight() > 2)
			{
				//Desc.fRanged_Impus_NoneDirection = { 7.f,-5.f };
				Desc.fRanged_Impus_NoneDirection = { 10.f,-15.f };
				Desc.fhitCharacter_Impus = { 15.f * m_iLookDirection, -15.f };

				Desc.ihitCharacter_Motion = { HitMotion::HIT_KNOCK_AWAY_LEFTDOWN };
				//Desc.fStartOffset = { -0.2f,-0.3f };
				Desc.fStartOffset = { 0.5f * m_iLookDirection,-0.3f };

				Desc.bGroundExplosionEffect = true;
			}
			else
			{
				Desc.fRanged_Impus_NoneDirection = { 15.f,0.f };
				Desc.fhitCharacter_Impus = { 5.f * m_iLookDirection,5.f };
				Desc.ihitCharacter_Motion = { HitMotion::HIT_SPIN_AWAY_LEFTUP };
				//Desc.fStartOffset = { -0.2f,0.f };
				//Desc.fStartOffset = { 0.3f * m_iLookDirection,-0.3f };
				Desc.fStartOffset = { 0.5f * m_iLookDirection,-0.3f };
			}




			Desc.iDirection = m_iLookDirection;
			Desc.eRangeColor = CAttackObject_Ranged::RANGED_LIGHT_PURPLE;


			//	Desc.ColliderDesc.vCenter = { -0.2f,-0.3f,0.f };

			//Desc.fStartOffset = { -0.2f,-0.3f};

			Desc.strEffectName = TEXT("FZ_SDJ_BIG");

			if (m_p236SpecialEffect != nullptr)
			{
				m_p236SpecialEffect->m_bIsDoneAnim = true;
				m_p236SpecialEffect = nullptr;
			}

			m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack_Ranged"), TEXT("Layer_AttackObject"), &Desc);

		}
	}
	break;
	case Client::CPlay_Frieza::ANIME_ATTACK_214:
	{
		if (iAttackEvent == 1)
		{
			m_DeleteLayer.push_back(Character_Make_BoneEffect("GD_fist_R", TEXT("FZ_SAJ-01_Right_Hand")));
			m_DeleteLayer.push_back(Character_Make_BoneEffect("GD_fist_L", TEXT("FZ_SAJ-01")));

		}
		else if (iAttackEvent == 0)
		{

			for (auto pEffect : m_DeleteLayer)
			{
				if (pEffect != nullptr)
					pEffect->m_bIsDoneAnim = true;
			}
			m_DeleteLayer.clear();


			m_bAcc214Attack = true;
			m_i214AttackPreviousDirection = m_iLookDirection;
			//Position 60 기원참 던지기.  처음 이펙트 생성은 melee에서?   어차피 던지는거랑 준비자세 둘이 다른이펙트. (각도가 다름)


			CAttackObject_Ranged::ATTACK_RANGED_DESC Desc{};

			if (m_iPlayerTeam == 1)
				Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_1P_Ranged_Attack;
			else
				Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_2P_Ranged_Attack;
			Desc.ColliderDesc.pMineGameObject = this;
			Desc.ColliderDesc.vExtents = { 0.2f,0.2f,1.f };


			Desc.bGrabbedEnd = true;

			//끌려가는것같아서 마음에 들지만 벽에 팅기는게 싫음.  
			//Desc.fhitCharacter_Impus = { 20.f * m_iLookDirection,0 };
			//Desc.ihitCharacter_Motion = { HitMotion::HIT_KNOCK_AWAY_LEFT };


			//괜찮지만 원작반영 한번 해보자
			//Desc.fhitCharacter_Impus = { 20.f * m_iLookDirection,0 };
			//Desc.ihitCharacter_Motion = { HitMotion::HIT_KNOCK_AWAY_LEFT_NONEBOUNDE };
			//Desc.bGroundSmash = true;


			//Desc.fhitCharacter_Impus = { 7.f * m_iLookDirection,1.f };
			Desc.fhitCharacter_Impus = { 7.f * m_iLookDirection,0.3f };

			Desc.ihitCharacter_Motion = { HitMotion::HIT_SPIN_AWAY_LEFTUP };
			Desc.fForcedGravityTime = 0.f;

			Desc.bHitNoGravity = true;
			Desc.fMaxNoNoGravitySafeTime = 1.9f;

			Desc.fhitCharacter_StunTime = 10.f;
			Desc.iDamage = 600 * Get_DamageScale();;
			Desc.fLifeTime = 2.f;


			Desc.bExplosion = false;



			Desc.iTeam = m_iPlayerTeam;

			Desc.bCameraZoom = false;
			Desc.fAnimationLockTime = 0.f;

			Desc.pOwner = this;

			//Desc.eAttackType = { ATTACKTYPE_HIGH };
			//Desc.fRanged_Impus_NoneDirection = { 13.f,0.f };
			Desc.fRanged_Impus_NoneDirection = { 21.f,0.f };

			Desc.iDirection = m_iLookDirection;
			Desc.eRangeColor = CAttackObject_Ranged::RANGED_LIGHT_PURPLE;

			Desc.bPierce = true;

			Desc.strEffectName = TEXT("FZ_SAJ-Basic");
			Desc.ColliderDesc.vCenter = { -0.2f,-0.3f,0.f };

			Desc.fStartOffset = { 0.4f * m_iLookDirection, 0.9f };


			m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack_Ranged"), TEXT("Layer_AttackObject"), &Desc);

			Desc.fStartOffset = { -0.4f * m_iLookDirection, 0.9f };

			m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack_Ranged"), TEXT("Layer_AttackObject"), &Desc);

		}

	}
	break;
	case Client::CPlay_Frieza::ANIME_ULTIMATE:
	{

		//,본인포함 약간 시간정지, 컷씬
		if (iAttackEvent == 0)
		{

			Set_bAura(true);
			if (Get_fHeight() < 2)
			{
				Add_Move({ 0.f,2.f });
			}

			Set_AnimationStopWithoutMe(1.5f);
			//Set_AnimationStop(2.f);

			CMain_Camera* mainCamera = static_cast<CMain_Camera*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Main_Camera")));
			if (Get_iDirection() == 1)
			{
				mainCamera->Play(CMain_Camera::VIRTUAL_CAMERA::VIRTUAL_CAMERA_FRIEZA_ULTIMATE_1, 0, this);
			}
			else if (Get_iDirection() == -1)
			{
				mainCamera->Play(CMain_Camera::VIRTUAL_CAMERA::VIRTUAL_CAMERA_FRIEZA_ULTIMATE_1, 1, this, nullptr, true);
			}


			//FZ_SDO-01

			m_p236UltimateEffect = Character_Make_BoneEffect("GD_fng_b3_R", TEXT("FZ_SDO-01"));
			m_pRenderInstance->Get_Instance()->Switch_BlackOut(true);


		}
		//,이펙트생성. 손가락 끝을 따라다님?
		else if (iAttackEvent == 1)
		{
			Set_AnimationStop(1.f);


			_vector position = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			_vector resultPosition = XMVectorAdd(position, XMVectorSet(0.f, 2.f, 0.f, 0.f));
			_float3 resultFloat3{};
			XMStoreFloat3(&resultFloat3, resultPosition);
			CParticle_Manager::Get_Instance()->Play(CParticle_Manager::FREIZA_ULTIMATE_1_PARTICLE, resultFloat3);
		}
		//,정지끝, 구체 떨어짐. 이거 땅에 떨어지거나 맞을때까지 정지인데
		else if (iAttackEvent == 2)
		{
			if (m_p236UltimateEffect != nullptr)
			{
				m_p236UltimateEffect->m_bIsDoneAnim = true;
				m_p236UltimateEffect = nullptr;
			}

			CAttackObject_Ranged::ATTACK_RANGED_DESC Desc{};
			//Desc.ColliderDesc.width = 1.0;
			//Desc.ColliderDesc.height = 1.0;
			if (m_iPlayerTeam == 1)
				Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_1P_Ranged_Attack;
			else
				Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_2P_Ranged_Attack;
			Desc.ColliderDesc.pMineGameObject = this;
			//Desc.ColliderDesc.vExtents = { 0.8f,0.8f,1.f };
			//Desc.ColliderDesc.vExtents = { 1.3f,1.3f,1.f };
			Desc.ColliderDesc.vExtents = { 1.0f,1.0f,1.f };



			Desc.bGrabbedEnd = true;

			Desc.fhitCharacter_StunTime = 0.35f;
			Desc.iDamage = 2080 * Get_DamageScale(true);
			Desc.fLifeTime = 5.f;





			Desc.iTeam = m_iPlayerTeam;

			Desc.bCameraZoom = false;
			Desc.fAnimationLockTime = 0.5f;
			Desc.bOnwerHitNoneStop = true;

			Desc.pOwner = this;

			//Desc.eAttackType = { ATTACKTYPE_HIGH };
			Desc.fStartOffset = { 1.3f * m_iLookDirection, 0.9f };

			Desc.iGainKiAmount = 0;

			//Desc.bGroundSmash = true;
			//Desc.fRanged_Impus_NoneDirection = { 7.f,-5.f };
			//Desc.fRanged_Impus_NoneDirection = { 14.f,-25.f };

			Desc.fRanged_Impus_NoneDirection = { 10.f,-15.f };
			//Desc.fRanged_Impus_NoneDirection = { 15.f,-15.f };


			Desc.fhitCharacter_Impus = { 15.f * m_iLookDirection, -15.f };
			Desc.ihitCharacter_Motion = { HitMotion::HIT_HEAVY_DOWN };


			Desc.iDirection = m_iLookDirection;
			Desc.eRangeColor = CAttackObject_Ranged::RANGED_LIGHT_PURPLE;
			Desc.bGrabbedEnd = true;

			Desc.eAttackGrade = { GRADE_ULTIMATE };

			Desc.ColliderDesc.vCenter = { -0.2f,-0.3f,0.f };


			Desc.iCallAttackBackIndex = 1001;

			Desc.strEffectName = TEXT("FZ_SDO-01_BIG");
			Desc.strHitEffectName = TEXT("FZ_SDO-02_BIG");
			Desc.bExplosionEffectisHitEffect = true;
			Desc.bGroundExplosionEffect = true;
			//Character_Make_BoneEffect("GD_fng_b3_R", TEXT("FZ_SDO-02"));

			m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack_Ranged"), TEXT("Layer_AttackObject"), &Desc);


			Set_bAura(false);
			m_pRenderInstance->Get_Instance()->Switch_BlackOut(false);

		}
		else if (iAttackEvent == 1001)
		{
			_vector position = static_cast<CTransform*>(m_pEnemy->Get_Component(TEXT("Com_Transform")))->Get_State(CTransform::STATE_POSITION);
			_vector resultPosition = XMVectorAdd(position, XMVectorSet(0.f, 0.f, 0.f, 0.f));
			_float3 resultFloat3{};
			XMStoreFloat3(&resultFloat3, resultPosition);
			CParticle_Manager::Get_Instance()->Play(CParticle_Manager::FREIZA_ULTIMATE_1_HIT_PARTICLE, resultFloat3);


			//if (m_pEnemy->Get_iHP() < 2080 * Get_DamageScale(true))
			if (m_pEnemy->Get_iHP() <=0)
			{
				CMap_Manager::Get_Instance()->PlayerCall_EastFinish(CMap_Manager::EAST_SPHERE, 2.f);
				m_pEnemy->Set_FinalSkillRoundEnd(true, 0);
				//캐릭터 MaxDeath 도 처리
				CUI_Manager::Get_Instance()->CutSceneUI(false);

			}
		}
	}
	break;
	case Client::CPlay_Frieza::ANIME_214_FINAL:
	{

		//카메라 컷씬 및 가속도 초기화
		if (iAttackEvent == 0)
		{
			Set_fImpulse({ 0.f , 0.f });

			Set_AnimationStopWithoutMe(2.f);

			CMain_Camera* mainCamera = static_cast<CMain_Camera*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Main_Camera")));
			if (Get_iDirection() == 1)
			{
				mainCamera->Play(CMain_Camera::VIRTUAL_CAMERA::VIRTUAL_CAMERA_FRIEZA_ULTIMATE_3, 0, this);
			}
			else if (Get_iDirection() == -1)
			{
				mainCamera->Play(CMain_Camera::VIRTUAL_CAMERA::VIRTUAL_CAMERA_FRIEZA_ULTIMATE_3, 1, this, nullptr, true);
			}

			//Character_Make_Effect(TEXT("FZ_SAO-01"));

			//Character_Make_BoneEffect("G_root", TEXT("FZ_SAO-01"));
			Character_Make_BoneEffect("G_chest", TEXT("FZ_SAO-01"));
			m_pRenderInstance->Get_Instance()->Switch_BlackOut(true);

		}

		//준비?
		else if (iAttackEvent == 1)
		{
			//Character_Make_BoneEffect("G_root", TEXT("FZ_SAO-02"));
			Character_Make_BoneEffect("G_chest", TEXT("FZ_SAO-02"));

			_vector position = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			_vector resultPosition = XMVectorAdd(position, XMVectorSet(0.f, 1.f, 0.f, 0.f));
			_float3 resultFloat3{};
			XMStoreFloat3(&resultFloat3, resultPosition);
			CParticle_Manager::Get_Instance()->Play(CParticle_Manager::FREIZA_ULTIMATE_3_PARTICLE, resultFloat3);
		}

		//공격시작
		else if (iAttackEvent == 2)
		{
			m_p214FinalEffect = Character_Make_BoneEffect("G_head", TEXT("FZ_SAO-03"));
			m_p214FinalEffect->Set_Copy_Layer_Rotation({ 0.f,0.f,-45.f });

			Set_fImpulse({ 10.f * m_iLookDirection, -15.f });


			CAttackObject::ATTACK_DESC Desc{};

			if (m_iPlayerTeam == 1)
				Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_1P_Melee_Attack;
			else
				Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_2P_Melee_Attack;
			Desc.ColliderDesc.pMineGameObject = this;
			Desc.ColliderDesc.vCenter = { -0.3f * m_iLookDirection,0.8f,0.f };
			Desc.ColliderDesc.vExtents = { 1.5f,1.5f,0.2f };


			Desc.fhitCharacter_Impus = { m_fImpuse };


			Desc.fhitCharacter_StunTime = 50.f;
			Desc.iDamage = 4200 * Get_DamageScale(true);
			//Desc.fLifeTime = 0.3f;  //어쩌지 현재높이로부 -13/s로 움직였을때 땅에 닿을때까지의 시간 
			Desc.fLifeTime = Get_fHeight() / 13.f;
			Desc.eAttackGrade = { GRADE_ULTIMATE };

			Desc.ihitCharacter_Motion = { HitMotion::HIT_KNOCK_AWAY_LEFTDOWN };
			Desc.iTeam = m_iPlayerTeam;
			Desc.fAnimationLockTime = 0.1f;
			Desc.pOwner = this;
			Desc.bGroundSmash = true;
			Desc.bCameraZoom = false;
			Desc.iGainAttackStep = 0;
			Desc.bGrabbedEnd = true;

			Desc.iCallAttackBackIndex = 1001;

			m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack"), TEXT("Layer_AttackObject"), &Desc);
		}
		else if (iAttackEvent == 1001)
		{
			_vector position = static_cast<CTransform*>(m_pEnemy->Get_Component(TEXT("Com_Transform")))->Get_State(CTransform::STATE_POSITION);
			_vector resultPosition = XMVectorAdd(position, XMVectorSet(0.f, 0.f, 0.f, 0.f));
			_float3 resultFloat3{};
			XMStoreFloat3(&resultFloat3, resultPosition);
			CParticle_Manager::Get_Instance()->Play(CParticle_Manager::FREIZA_ULTIMATE_3_HIT_PARTICLE, resultFloat3);
		}
	}
	break;
	case Client::CPlay_Frieza::ANIME_214_FINAL_2:
	{
		//풀파워 용인데 어쩐다냐

	}
	break;
	case Client::CPlay_Frieza::ANIME_WAKEUP_FINAL:
	{
		//Position 0 : 
		if (iAttackEvent == 0)
		{
			//0.6초면 조금 움직일 수 있음
			Set_AnimationStopWithoutMe(0.55f);

			CMain_Camera* mainCamera = static_cast<CMain_Camera*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Main_Camera")));
			mainCamera->Play(CMain_Camera::VIRTUAL_CAMERA::VIRTUAL_CAMERA_FRIEZA_FALL_REFLECT, 0, this);
			mainCamera->StartCameraShake(1.f, 0.1f);

			m_pGameInstance->Play_Sound(CSound_Manager::SOUND_KEY_NAME::Frieza_Reflect, false, 1.f);
		}

		//Position 25 자세잡음, 시간정지, 사운드추가?
		else if (iAttackEvent == 1)
		{
			Set_AnimationStopWithoutMe(0.4f);
			Set_AnimationStop(0.6f);

			Character_Make_BoneEffect("GD_fist_L", TEXT("FZ_Down_R-01"));

		}

		//Position 50 발사
		else if (iAttackEvent == 2)
		{
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
			Desc.iDamage = 520 * Get_DamageScale(true);
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

			CMain_Camera* mainCamera = static_cast<CMain_Camera*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Main_Camera")));
			mainCamera->StartCameraShake(1.f, 0.2f);

			m_pGameInstance->Play_Sound(CSound_Manager::SOUND_KEY_NAME::Frieza_Reflect_SFX, false, 1.f);
			m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack_Energy"), TEXT("Layer_AttackObject"), &Desc);


			//Character_Make_BoneEffect("GD_fist_L", TEXT("FZ_Down_R-03"));
			//Character_Make_BoneEffect("GD_fist_L", TEXT("FZ_Down_R-04"));


			//CEffect_Layer* pEffect =Character_Make_Effect(TEXT("FZ_Down_R-04"), { 0.599f,1.075f });

			{
				CEffect_Layer* pEffect = Character_Make_BoneEffect("GD_fist_L", TEXT("FZ_Down_R-03"));
				pEffect->Set_Copy_Layer_Rotation({ 0.f,0.f,25.f });
			}
			{
				CEffect_Layer* pEffect = Character_Make_BoneEffect("GD_fist_L", TEXT("FZ_Down_R-04"));
				pEffect->Set_Copy_Layer_Rotation({ 0.f,0.f,25.f });
			}


			//Character_Make_BoneEffect("GD_fist_L", TEXT("FZ_Down_R-02"));
			Character_Make_Effect(TEXT("FZ_Down_R-02"));
		}
	}
	break;
	case Client::CPlay_Frieza::ANIME_TRANSFORM_FINAL:
	{

		//Position 0 : 컷씬, 적 시간정지. SM
		if (iAttackEvent == 0)
		{
			CMain_Camera* mainCamera = static_cast<CMain_Camera*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Main_Camera")));

			Set_AnimationStopWithoutMe(5.f);

			mainCamera->Play(CMain_Camera::VIRTUAL_CAMERA::VIRTUAL_CAMERA_FRIEZA_GOLDEN, 0, this);

			//FZ_SAR-01
			m_pRenderInstance->Get_Instance()->Switch_BlackOut(true);


		}

		//Position 7 이펙트 생성
		else if (iAttackEvent == 101)
		{
			Character_Make_Effect(TEXT("FZ_SAR-01"));
		}
		//Position 15 : //,이펙트,화면가리기,  433cs(index 70)로 연계?  일단 Position 20까지 안가고 여기서 처리.
		else if (iAttackEvent == 1)
		{
			Set_Animation(CPlay_Frieza::ANIME_TRANSFORM_FINAL_CUTSCENE);
			Set_CurrentAnimationPositionJump(100.f);
			m_pModelCom->Get_pCurrentAnimation()->m_fTickPerSecond = 25;

			m_bGoldFrieza = true;

			CRenderInstance::Get_Instance()->Start_AllWhiteOut(1.f, 2.f);

			/*if (Get_iDirection() == 1)
			{
				mainCamera->Play(CMain_Camera::VIRTUAL_CAMERA::VIRTUAL_CAMERA_FRIEZA_GOLDEN, 0, this);
			}
			else if (Get_iDirection() == -1)
			{
				mainCamera->Play(CMain_Camera::VIRTUAL_CAMERA::VIRTUAL_CAMERA_FRIEZA_GOLDEN, 2, this, nullptr, true);
			}*/
			m_pRenderInstance->Get_Instance()->Switch_BlackOut(false);

		}


		//Position 20 : 모델변경, 카메라 컷씬지속?
		else if (iAttackEvent == 2)
		{

		}
	}
	break;
	case Client::CPlay_Frieza::ANIME_TRANSFORM_FINAL_CUTSCENE:
	{
		//필요한가?

		//,변신3필 컷씬? 이펙트로 가리다가 보이는건 120부터
		//if (iAttackEvent == 0)
		//{
		//
		//}
		//else if (iAttackEvent == 1)
		//{
		//
		//}
		//else if (iAttackEvent == 2)
		//{
		//
		//}
	}
	break;
	case Client::CPlay_Frieza::ANIME_IDLE:
		break;
	case Client::CPlay_Frieza::ANIME_JUMP_UP:
		break;
	case Client::CPlay_Frieza::ANIME_JUMP_DOWN:
		break;
	case Client::CPlay_Frieza::ANIME_FORWARD_WALK:
		break;
	case Client::CPlay_Frieza::ANIME_BACK_WALK:
		break;
	case Client::CPlay_Frieza::ANIME_FORWARD_DASH:
		break;
	case Client::CPlay_Frieza::ANIME_BACK_DASH:
		break;
	case Client::CPlay_Frieza::ANIME_FORWARD_DASH_END:
		break;
	case Client::CPlay_Frieza::ANIME_BACK_DASH_END:
		break;
	case Client::CPlay_Frieza::ANIME_CROUCH_START:
		break;
	case Client::CPlay_Frieza::ANIME_CROUCHING:
		break;
	case Client::CPlay_Frieza::ANIME_CHASE:
		break;
	case Client::CPlay_Frieza::ANIME_GUARD_GROUND:
		break;
	case Client::CPlay_Frieza::ANIME_GUARD_CROUCH:
		break;
	case Client::CPlay_Frieza::ANIME_GUARD_AIR:
		break;
	case Client::CPlay_Frieza::ANIME_HIT_LIGHT:
		break;
	case Client::CPlay_Frieza::ANIME_HIT_LIGHT_FRONT:
		break;
	case Client::CPlay_Frieza::ANIME_HIT_CROUCH:
		break;
	case Client::CPlay_Frieza::ANIE_HIT_AIR:
		break;
	case Client::CPlay_Frieza::ANIME_HIT_FALL:
		break;
	case Client::CPlay_Frieza::ANIME_HIT_BOUND_DOWN:
		break;
	case Client::CPlay_Frieza::ANIME_LAYUP:
		break;
	case Client::CPlay_Frieza::ANIME_DIE_STAND:
		break;
	case Client::CPlay_Frieza::ANIME_HIT_HEAVY_AWAY_SPIN_UP:
		break;
	case Client::CPlay_Frieza::ANIME_HIT_HEAVY_AWAY_SPIN_LEFTUP:
		break;
	case Client::CPlay_Frieza::ANIME_HIT_HEAVY_AWAY_SPIN_LEFT:
		break;
	case Client::CPlay_Frieza::ANIME_HIT_HEAVY_AWAY_LEFT:
		break;
	case Client::CPlay_Frieza::ANIME_HIT_HEAVY_AWAY_UP:
		break;
	case Client::CPlay_Frieza::ANIME_HIT_HEAVY_AWAY_LEFTDOWN:
		break;
	case Client::CPlay_Frieza::ANIME_HIT_WALLBOUNCE:
		break;
	case Client::CPlay_Frieza::ANIME_BREAK_FALL_GROUND:
		break;
	case Client::CPlay_Frieza::ANIME_BREAK_FALL_AIR:
		break;
	case Client::CPlay_Frieza::ANIME_GRAB_READY:
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
	case ANIME_START_DEFAULT:
	{
		if (iAttackEvent == 0)
		{
			//하얀 필터?

		}
		if (iAttackEvent == 1)
		{
			//FZ_Opening
			//CEffect_Layer* pEffect =
			Character_Make_BoneEffect("G_root", TEXT("FZ_Opening"));
			//pEffect->Set_Copy_Layer_Rotation({ 0.f,30.f,0.f });
			//pEffect->Set_Copy_Layer_Position

			Character_Make_BoneEffect("G_root", TEXT("21_Opening"));
			//CEffect_Layer* pEffect = Character_Make_BoneEffect("G_root", TEXT("21_WSDO-01"));
			//pEffect->Set_Copy_Layer_Rotation({ 0.f,90.f,0.f });
		}

	}
	break;

	default:
		break;
	}

}

void CPlay_Frieza::Play_Sound(_uint SoundName, _bool bisLoof, _float fvolume)
{
	m_pGameInstance->Play_Sound((CSound_Manager::SOUND_KEY_NAME)SoundName, bisLoof, fvolume);
}

void CPlay_Frieza::Play_Group_Sound(_uint groupKey, _bool loop, _float volume)
{
	m_pGameInstance->Play_Group_Sound((CSound_Manager::SOUND_GROUP_KEY)groupKey, loop, volume);
}

void CPlay_Frieza::Play_Sound_Stop(_uint SoundName)
{
	m_pGameInstance->Stop_Sound((CSound_Manager::SOUND_KEY_NAME)SoundName);
}



void CPlay_Frieza::Add_YellowLight()
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

void CPlay_Frieza::Add_BlueLight()
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

_float CPlay_Frieza::Get_DamageScale(_bool bUltimate)
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
		//fDamageScale *= 0.2f; //곱연산 .  15%->16%   너무 작은가 싶지만 원작반영.
		fDamageScale += 0.1f;
	}


	if (m_bGoldFrieza)
	{
		fDamageScale *= 1.1f;
	}

	//return fDamageScale;
	return fDamageScale * 0.8f;
}

void CPlay_Frieza::Set_BreakFall_Ground()
{


	Set_bRedHP(false);
	Reset_AttackStep();




	Set_Animation(m_iBreakFall_Ground, 2.f);
	Set_NextAnimation(m_iIdleAnimationIndex, 2.f);


	if (inputBuffer.size() == 0)
	{
		inputBuffer.push_back(CInput(MOVEKEY_NEUTRAL, ATTACK_NONE));
	}


	DirectionInput iMoveKey = inputBuffer.back().direction;

	if (iMoveKey == MOVEKEY_UP || iMoveKey == MOVEKEY_UP_LEFT)
	{
		Set_fImpulse({ -3.f * m_iLookDirection,1.f });
		Set_ForcveGravityTime(0.f);
	}
	else if (iMoveKey == MOVEKEY_RIGHT)
	{
		//Set_fImpulse({ 0.f , 0.3f });
		Set_fImpulse({ 0.f , 0.1f });

		Set_ForcedGravityTime_LittleUp();
	}

	else if (iMoveKey == MOVEKEY_DOWN)
	{
		Set_Animation(m_iBound_Ground, 2.f);
		Set_NextAnimation(m_iLayUp, 2.f);
	}

	else //if (iMoveKey == MOVEKEY_LEFT)
	{
		Set_fImpulse({ -5.f * m_iLookDirection, 0.f });
		Set_ForcedGravityTime_LittleUp();
	}


}

void CPlay_Frieza::Update214ReturnEvent(_float fTimeDelta)
{



	if (m_bAcc214Attack == false)
		return;

	if (m_bGrab || m_bGrabbed)
	{
		m_fAcc214AttackTime = 0.f;
		m_bAcc214Attack = false;

		return;
	}

	m_fAcc214AttackTime += fTimeDelta;

	//if (m_fAcc214AttackTime > 1.5f)
	if (m_fAcc214AttackTime > 1.0f)
	{
		m_fAcc214AttackTime = 0.f;
		m_bAcc214Attack = false;


		CAttackObject_Ranged::ATTACK_RANGED_DESC Desc{};

		//if (m_iPlayerTeam == 1)
		//	Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_1P_Ranged_Attack;
		//else
		//	Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_2P_Ranged_Attack;


		Desc.ColliderDesc.pMineGameObject = this;
		Desc.ColliderDesc.vExtents = { 0.2f,0.2f,1.f };


		Desc.bGrabbedEnd = true;

		//끌려가는것같아서 마음에 들지만 벽에 팅기는게 싫음.  
		//Desc.fhitCharacter_Impus = { 20.f * m_iLookDirection,0 };
		//Desc.ihitCharacter_Motion = { HitMotion::HIT_KNOCK_AWAY_LEFT };

		Desc.fhitCharacter_Impus = { -7.f * m_i214AttackPreviousDirection,0.5 };
		Desc.ihitCharacter_Motion = { HitMotion::HIT_KNOCK_AWAY_LEFT_NONEBOUNDE };
		Desc.bGroundSmash = true;


		Desc.fhitCharacter_StunTime = 0.35f;
		Desc.iDamage = 600 * Get_DamageScale();;
		Desc.fLifeTime = 2.f;

		Desc.iOnwerDirection = -m_i214AttackPreviousDirection;

		Desc.bExplosion = false;



		Desc.iTeam = m_iPlayerTeam;

		Desc.bCameraZoom = false;
		Desc.fAnimationLockTime = 0.f;

		Desc.pOwner = this;

		//Desc.eAttackType = { ATTACKTYPE_HIGH };
		//Desc.fRanged_Impus_NoneDirection = { 13.f,0.f };
		Desc.fRanged_Impus_NoneDirection = { 21.f,0.f };

		//반대로 갈것
		Desc.iDirection = -m_i214AttackPreviousDirection;

		Desc.eRangeColor = CAttackObject_Ranged::RANGED_LIGHT_PURPLE;

		Desc.bPierce = true;


		Desc.ColliderDesc.vCenter = { -0.2f,-0.3f,0.f };


		//Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_1P_Ranged_Attack;
		//
		//Desc.fStartOffset = { Get_fPositionX() + 20 * m_i214AttackPreviousDirection, 0.9f };
		//
		//m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack_Ranged"), TEXT("Layer_AttackObject"), &Desc);
		//
		//Desc.fStartOffset = { Get_fPositionX() + 16 * m_i214AttackPreviousDirection, 0.9f };
		//
		//m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack_Ranged"), TEXT("Layer_AttackObject"), &Desc);
		//
		//
		////자해 버전
		//
		//Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_2P_Ranged_Attack;
		//m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack_Ranged"), TEXT("Layer_AttackObject"), &Desc);
		//Desc.fStartOffset = { Get_fPositionX() + 20 * m_i214AttackPreviousDirection, 0.9f };
		//m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack_Ranged"), TEXT("Layer_AttackObject"), &Desc);

		Desc.fCameraShakeDuration = 0.5f;
		Desc.fCameraShakeMagnitude = 0.2f;

		Desc.strEffectName = TEXT("FZ_SAJ-01");;

		if (m_iPlayerTeam == 1)
			Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_1P_Ranged_Attack;
		else
			Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_2P_Ranged_Attack;


		//0.9였음
		Desc.fStartOffset = { Get_fPositionX() + 19 * m_i214AttackPreviousDirection, 0.8f - Get_fHeight() };

		m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack_Ranged"), TEXT("Layer_AttackObject"), &Desc);



		//자해 버전
		if (m_iPlayerTeam == 1)
			Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_2P_Ranged_Attack;
		else
			Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_1P_Ranged_Attack;

		Desc.fStartOffset = { Get_fPositionX() + 20 * m_i214AttackPreviousDirection, 0.8f - Get_fHeight() };
		m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack_Ranged"), TEXT("Layer_AttackObject"), &Desc);



	}



}

_bool CPlay_Frieza::Get_bGoldenFrieza()
{
	return m_bGoldFrieza;
}

void CPlay_Frieza::Character_CinematicEnd()
{
	Set_AnimationMoveXZ(false);
	//CBattleInterface_Manager::Get_Instance()->Character_Opening_EndForCharacter(m_iPlayerTeam);

	CCharacter* pKririn = static_cast<CCharacter*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Model_Opening")));

	if (pKririn != nullptr)
		pKririn->Destory();

	//static_cast<COpening_Kririn*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Model_Opening")))->Set_Delete();

	//CMain_Camera* mainCamera = static_cast<CMain_Camera*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Main_Camera")));
	//mainCamera->Set_Virtual_Camera(CMain_Camera::VIRTUAL_CAMERA::VIRTUAL_CAMERA_NORMAL);

}



CPlay_Frieza* CPlay_Frieza::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPlay_Frieza* pInstance = new CPlay_Frieza(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CPlay_Frieza"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPlay_Frieza::Clone(void* pArg)
{
	CPlay_Frieza* pInstance = new CPlay_Frieza(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CPlay_Frieza"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlay_Frieza::Free()
{
	__super::Free();


	//Safe_Release(m_pShaderCom);
	//Safe_Release(m_pModelCom);

	//Safe_Release(m_pModelCom_Opening);
	Safe_Release(m_pModelCom_Skill);
	Safe_Release(m_p2PTextureCom);
	Safe_Release(m_pDecalTextureCom);
	Safe_Release(m_pGold_BaseTexture);
	Safe_Release(m_pGold_OutLineTexture);
	Safe_Release(m_pMetalTexture);
}
