#include "Level_Capture.h"
#include "Game_API.h"
#include "Battle_AnimDef.h"
#include "PokemonData_Manager.h"
#include "RenderRule_Manager.h"
#include "Capture_Manager.h"
#include "Capture_Menu.h"
#include "Camera_Free.h"
#include "Player_Status.h"
#include "Actor_CaptureTarget.h"
#include "Body_Pokemon.h"
#include "MonsterBall.h"
#include "MapObject.h"

#include "GameInstance.h"

NS_BEGIN(Game_PKM)
static constexpr _uint CURRENT_LEVEL = ETOUI(LEVEL::CAPTURE);

static constexpr _float CAPTURE_INTRO_MESSAGE_HOLD = 0.5f;

static const _float3 CAPTURE_TARGET_POS = { 0.f, 0.f, 0.f };
static const _float3 CAPTURE_CAMERA_EYE = { 0.f, 3.282f, -4.345f };
static const _float3 CAPTURE_CAMERA_AT = { 0.f, 0.65f, 0.f };

static constexpr _float STAGE_DROP_DURATION = 0.45f;

static constexpr _float STAGE_SCENE_CAMERA_BACK_DISTANCE = 3.8f;
static constexpr _float STAGE_SCENE_CAMERA_HEIGHT_OFFSET = 0.65f;
static constexpr _float STAGE_SCENE_CAMERA_LOOK_UP_OFFSET = 0.35f;
static constexpr _float STAGE_SCENE_BALL_GROUND_CENTER_Y = 0.2f;
static constexpr _float STAGE_SCENE_BALL_AIR_CENTER_Y = 2.6f;

static constexpr _float STAGE_SHAKE_DURATION = 0.8f;
static constexpr _float STAGE_SHAKE_ANGLE_DEG = 20.f;

namespace
{
	static CPlayer_Status* Find_PlayerState(CGameInstance* pGameInstance)
	{
		if (nullptr == pGameInstance)
			return nullptr;

		const list<CGameObject*>* pObjects =
			pGameInstance->Get_ObjectList(ETOUI(LEVEL::STATIC), LAYER_PERSISTENT);

		if (nullptr == pObjects || pObjects->empty())
			return nullptr;

		return static_cast<CPlayer_Status*>(pObjects->front());
	}
}
NS_END

CLevel_Capture::CLevel_Capture(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const CAPTURE_ENV& tEnv)
	: CLevel{ pDevice, pContext }
	, m_tEnv{ tEnv }
{
}

HRESULT CLevel_Capture::Initialize()
{
	wchar_t szLog[160] = {};
	swprintf_s(szLog,
		L"[Level_Capture] Initialize: SpeciesID=%u, Level=%u, BallItemID=%u, ZoneID=%u\n",
		m_tEnv.iSpeciesID, m_tEnv.iLevel, m_tEnv.iInitialBallItemID, m_tEnv.iZoneID);
	OutputDebugStringW(szLog);

	m_pCaptureManager = CCapture_Manager::Create(m_tEnv);
	if (nullptr == m_pCaptureManager)
		return E_FAIL;

	if (FAILED(Ready_Lights()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera(LAYER_CAMERA)))
		return E_FAIL;

	if (FAILED(Ready_Layer_BackGround(LAYER_BACKGROUND)))
		return E_FAIL;

	if (FAILED(Ready_Layer_Battler(LAYER_MONSTER)))
		return E_FAIL;

	if (FAILED(Ready_Layer_Ball(LAYER_INTERACTABLE)))
		return E_FAIL;

	if (FAILED(Ready_Layer_UI(LAYER_UI)))
		return E_FAIL;

	if (CPlayer_Status* pPlayerState = Find_PlayerState(m_pGameInstance))
		pPlayerState->Mark_DexSeen(m_tEnv.iSpeciesID);

	m_pCaptureManager->Set_Combatants(m_pCaptureTarget, m_pMonsterBall);
	m_pCaptureManager->Begin();

	Reset_CaptureCameraPose();

	if (nullptr != m_pMonsterBall)
	{
		m_pMonsterBall->Reset();
		Update_IntroBallPose();
		m_pMonsterBall->Show();
	}

	Begin_CaptureIntroView();

	return S_OK;
}

void CLevel_Capture::Update(_float fTimeDelta)
{
	if (nullptr == m_pCaptureManager)
		return;

	/* 디버그: B 키로 Ball_Trail On/Off 토글 */
	if (nullptr != m_pMonsterBall && m_pGameInstance->Key_Down(DIK_B))
		m_pMonsterBall->Set_TrailEnabled(!m_pMonsterBall->Is_TrailEnabled());

	const CAPTURE_PHASE ePhaseBeforeUpdate = m_pCaptureManager->Get_Phase();

	/* AIMING 에서만 좌클릭 = 볼 던지기. 재던지기 시 볼이 DONE 이면 Reset 선행. */
	const _bool bAiming = CAPTURE_PHASE::AIMING == ePhaseBeforeUpdate;

	if (bAiming && m_pGameInstance->Mouse_Down(DIMB::LBUTTON))
	{
		Set_AimingCameraControl(false);

		if (nullptr != m_pMonsterBall
			&& CMonsterBall::BALL_STATE::DONE == m_pMonsterBall->Get_State())
		{
			m_pMonsterBall->Reset();
		}

		Update_AimPose();

		m_pCaptureManager->Try_Throw();

		if (nullptr != m_pMonsterBall)
			m_pMonsterBall->Launch();
	}
	else if (bAiming)
	{
		Update_AimPose();
	}

	m_pCaptureManager->Update(fTimeDelta);

	const CAPTURE_PHASE ePhaseAfterUpdate = m_pCaptureManager->Get_Phase();

	/* 충돌 즉시(ball_absorb VFX 와 같은 프레임) capture_hit 재생.
	   적중 플래그의 상승 엣지로 던지기당 1회만 울린다. */
	const _bool bDidHit = m_pCaptureManager->Did_Hit();
	if (false == m_bPrevDidHit && true == bDidHit)
	{
		m_pGameInstance->Play(L"SFX/capture_hit.wav", CHANNELID::SFX, 0.8f);

		/* 피격 위치에서 멈춰 그 자리에서 흡수되도록 이동 정지. */
		if (nullptr != m_pCaptureTarget)
			m_pCaptureTarget->Set_MoveActive(false);
	}
	m_bPrevDidHit = bDidHit;

	if (CAPTURE_PHASE::INTRO != ePhaseBeforeUpdate
		&& CAPTURE_PHASE::INTRO == ePhaseAfterUpdate)
	{
		Set_AimingCameraControl(false);
		Reset_CaptureCameraPose();

		if (nullptr != m_pMonsterBall)
		{
			m_pMonsterBall->Reset();
			Update_IntroBallPose();
			m_pMonsterBall->Show();
		}

		if (nullptr != m_pCaptureMenu)
			m_pCaptureMenu->Open(true);

		/* 재도전 시 중앙(누적시간 0)부터 종별 이동 재개. */
		if (nullptr != m_pCaptureTarget)
		{
			m_pCaptureTarget->Reset_Move();
			m_pCaptureTarget->Set_MoveActive(true);
		}
	}

	if (CAPTURE_PHASE::STAGE != ePhaseBeforeUpdate
		&& CAPTURE_PHASE::STAGE == ePhaseAfterUpdate)
	{
		/* 볼 안에 있는 동안(흡수~흔들기) 홈(중앙)에 둔다. break-out 등장 위치를 중앙으로. */
		if (nullptr != m_pCaptureTarget)
			m_pCaptureTarget->Reset_Move();

		if (nullptr != m_pMonsterBall)
			m_pMonsterBall->Hide();

		Begin_StageCamera();
	}

	if (CAPTURE_PHASE::DROP != ePhaseBeforeUpdate
		&& CAPTURE_PHASE::DROP == ePhaseAfterUpdate)
	{
		Apply_StageCameraPose();
		Begin_StageDrop();
	}

	if (CAPTURE_PHASE::SHAKE == ePhaseAfterUpdate)
	{
		const _int iShakeIndex = m_pCaptureManager->Get_ShakeIndex();

		if (m_iAppliedShakeIndex != iShakeIndex
			&& nullptr != m_pMonsterBall
			&& CMonsterBall::BALL_STATE::DONE == m_pMonsterBall->Get_State())
		{
			m_pMonsterBall->Begin_OneShake(STAGE_SHAKE_DURATION, STAGE_SHAKE_ANGLE_DEG);
			m_pGameInstance->Play(L"SFX/capture_shake.wav", CHANNELID::SFX, 0.8f);
			m_iAppliedShakeIndex = iShakeIndex;
		}
	}
	else
	{
		m_iAppliedShakeIndex = -1;
	}

	if (CAPTURE_PHASE::SUCCESS_VIEW != ePhaseBeforeUpdate
		&& CAPTURE_PHASE::SUCCESS_VIEW == ePhaseAfterUpdate)
	{
		m_pGameInstance->Play(L"SFX/capture_success.wav", CHANNELID::SFX, 0.8f);
		Begin_CaptureSuccessView();
	}

	/* break-out VFX(Begin_Appear -> ball_absorb)는 BREAK_VIEW 끝(=INTRO 복귀) 시점에 터진다.
	   capture_fail 도 그 순간에 맞춰 재생한다. (이전엔 실패 판정 즉시 울려 VFX 보다 앞서 들렸음) */
	if (CAPTURE_PHASE::BREAK_VIEW == ePhaseBeforeUpdate
		&& CAPTURE_PHASE::INTRO == ePhaseAfterUpdate)
	{
		m_pGameInstance->Play(L"SFX/capture_fail.wav", CHANNELID::SFX, 0.8f);
	}

	if (CAPTURE_PHASE::AIMING != ePhaseBeforeUpdate
		&& CAPTURE_PHASE::AIMING == ePhaseAfterUpdate)
	{
		Set_AimingCameraControl(true);
		Update_AimPose();
	}

	UI_Update_All(fTimeDelta);

	Tick_CaptureIntroView(fTimeDelta);
	Tick_CaptureSuccessView();

	if (m_pCaptureManager->Is_Done())
	{
		if (CAPTURE_RESULT::SUCCESS == m_pCaptureManager->Get_Result())
		{
			if (CPlayer_Status* pPlayerState = Find_PlayerState(m_pGameInstance))
			{
				pPlayerState->Acquire_Pokemon(
					m_tEnv.iSpeciesID,
					static_cast<_ubyte>(m_tEnv.iLevel),
					m_tEnv.iZoneID);
			}
		}

		/* Pop_Level 성공 시 본 레벨(=this) 이 즉시 Free 되므로
		   호출 후 어떤 멤버에도 접근하지 않고 곧바로 return. */
		if (FAILED(m_pGameInstance->Pop_Level()))
		{
			MSG_BOX("Failed to Exit Capture");
			return;
		}
		return;
	}
}

HRESULT CLevel_Capture::Render()
{
#ifdef _DEBUG
	if (nullptr != m_pCaptureManager)
	{
		_wstring strTitle = TEXT("Capture Level | Phase: ");
		strTitle += to_wstring(static_cast<_uint>(m_pCaptureManager->Get_Phase()));
		strTitle += TEXT(" | Result: ");
		strTitle += to_wstring(static_cast<_uint>(m_pCaptureManager->Get_Result()));
		SetWindowText(m_pGameInstance->Get_HWND(), strTitle.c_str());
	}
	else
	{
		SetWindowText(m_pGameInstance->Get_HWND(), TEXT("Capture Level (manager null)"));
	}
#endif

	return S_OK;
}

HRESULT CLevel_Capture::Ready_Lights()
{
	m_pGameInstance->Clear_Lights();

	LIGHT_DESC      LightDesc{};

	LightDesc.eType = LIGHT::DIRECTIONAL;
	LightDesc.vDiffuse = _float4(0.75f, 0.75f, 0.75f, 1.f);  // 중성 화이트 (RGB 균일), 살짝 감소
	LightDesc.vAmbient = _float4(0.70f, 0.70f, 0.70f, 1.f);  // 중성 화이트, 강화 -> 평평·푸른 끼 제거
	LightDesc.vSpecular = _float4(0.3f, 0.3f, 0.3f, 1.f);  // 변화 없음
	LightDesc.vDirection = _float4(0.5f, -0.5f, 0.5f, 0.f);

	if (FAILED(m_pGameInstance->Add_Light(LightDesc)))
		return E_FAIL;

	SHADOW_LIGHT_DESC ShadowDesc{};
	ShadowDesc.vEye = _float4(0.f, 8.f, 0.f, 1.f);
	ShadowDesc.vAt = _float4(5.f, 0.f, 5.f, 1.f);
	ShadowDesc.fFovy = XMConvertToRadians(60.f);
	ShadowDesc.fNear = 0.1f;
	ShadowDesc.fFar = 200.f;

	if (FAILED(m_pGameInstance->Set_ShadowLight(ShadowDesc)))
		return E_FAIL;

	m_pGameInstance->Set_UseShadow(true);

	return S_OK;
}

HRESULT CLevel_Capture::Ready_Layer_Camera(WNameID strLayerTag)
{
	CCamera_Free::CAMERA_FREE_DESC CameraDesc = {};

	CameraDesc.vEye = CAPTURE_CAMERA_EYE;
	CameraDesc.vAt = CAPTURE_CAMERA_AT;
	CameraDesc.fFovy = XMConvertToRadians(35.f);
	CameraDesc.fNear = 0.1f;
	CameraDesc.fFar = 500.f;
	CameraDesc.fSpeedPerSec = 20.f;
	CameraDesc.fRotationPerSec = XMConvertToRadians(180.f);
	CameraDesc.fMouseSensor = 0.03f;
	CameraDesc.bControlEnabled = false;

	CCamera_Free* pCamera = static_cast<CCamera_Free*>(
		m_pGameInstance->Clone_Prototype(
			PROTOTYPE::GAMEOBJECT, ETOUI(LEVEL::STATIC),
			PROTO_OBJ_CAMERA_FREE, &CameraDesc));

	if (nullptr == pCamera)
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_GameObject_Ex(CURRENT_LEVEL, strLayerTag, pCamera)))
	{
		Safe_Release(pCamera);
		return E_FAIL;
	}

	m_pCaptureCamera = pCamera;

	return S_OK;
}

HRESULT CLevel_Capture::Ready_Layer_BackGround(WNameID strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::GAMEPLAY), PROTO_OBJ_SKY, ETOUI(LEVEL::CAPTURE), strLayerTag)))
		return E_FAIL;

	CMapObject* pMap = static_cast<CMapObject*>(
		m_pGameInstance->Clone_Prototype(
			PROTOTYPE::GAMEOBJECT, ETOUI(LEVEL::GAMEPLAY),
			PROTO_MAP_ROAD01));

	if (nullptr == pMap)
		return E_FAIL;

	pMap->Get_Component<CTransform>(COM_TRANSFORM)->Set_State(STATE::POSITION, XMVectorSet(-20.f, 0.f, -5.f, 1.f));

	if (FAILED(m_pGameInstance->Add_GameObject_Ex(CURRENT_LEVEL, strLayerTag, pMap)))
	{
		Safe_Release(pMap);
		return E_FAIL;
	}


	return S_OK;
}

HRESULT CLevel_Capture::Ready_Layer_Battler(WNameID strLayerTag)
{
	/* CAPTURE_ENV.iSpeciesID 기반 모델 매핑 - PokemonData_Manager 조회. */
	const CPokemonData_Manager* pPokeDataMgr = CPokemonData_Manager::GetInstance();
	if (nullptr == pPokeDataMgr)
		return E_FAIL;

	const SPECIES_DATA* pSpecies = pPokeDataMgr->Find_Species(m_tEnv.iSpeciesID);
	if (nullptr == pSpecies || 0 == pSpecies->strModelTag)
		return E_FAIL;

	CRenderRule_Manager* pRuleManager = CRenderRule_Manager::GetInstance();
	if (nullptr == pRuleManager)
		return E_FAIL;

	CBody_Pokemon::BODY_POKEMON_DESC BodyDesc{};
	BodyDesc.strModelProtoTag = pSpecies->strModelTag;
	BodyDesc.strShaderProtoTag = PROTO_COM_SHADER_POKEMON;
	BodyDesc.iDefaultAnim = BattleAnim::Find_AnimIndex(pSpecies->strModelTag, ANIM_KIND::IDLE);
	BodyDesc.bLoop = true;
	BodyDesc.fScale = 1.f;
	BodyDesc.pRenderRule = pRuleManager->Find_PokemonRenderRule(pSpecies);
	if (nullptr == BodyDesc.pRenderRule)
		return E_FAIL;

	/* PM0043(좌우 달리기 패턴)만 루트모션 활성 - 달리기 애님의 이동을 위치에 반영. */
	if (43 == m_tEnv.iSpeciesID)
	{
		BodyDesc.bEnableRootMotion = true;
		BodyDesc.strRootMotionBoneName = "Origin";
	}

	CActor_CaptureTarget::ACTOR_CAPTURE_DESC TargetDesc{};
	TargetDesc.fRotationPerSec = XMConvertToRadians(720.f);   // RUN_TURN 이 진행 방향으로 회전하도록(Face_Direction)
	TargetDesc.iBodyProtoLevel = ETOUI(LEVEL::STATIC);
	TargetDesc.iComponentLevel = ETOUI(LEVEL::GAMEPLAY);
	TargetDesc.strBodyProtoTag = PROTO_OBJ_BODY_POKEMON;
	TargetDesc.pBodyDesc = &BodyDesc;
	TargetDesc.iSpeciesID = m_tEnv.iSpeciesID;
	TargetDesc.iLevel = m_tEnv.iLevel;
	TargetDesc.iInitialBallItemID = m_tEnv.iInitialBallItemID;
	TargetDesc.bCaughtBefore = false;
	TargetDesc.vSpawnPos = CAPTURE_TARGET_POS;

	/* Clone -> LookAt -> Add_GameObject_Ex 패턴.
	   Add_GameObject 와 달리 인스턴스 포인터를 잡을 수 있어 등록 전에 회전 적용 가능. */
	CActor_CaptureTarget* pTarget = static_cast<CActor_CaptureTarget*>(
		m_pGameInstance->Clone_Prototype(
			PROTOTYPE::GAMEOBJECT, ETOUI(LEVEL::GAMEPLAY),
			PROTO_OBJ_ACTOR_CAPTURE_TARGET, &TargetDesc));
	if (nullptr == pTarget)
		return E_FAIL;

	/* 카메라 방향을 정면으로 보도록 회전하되, 수평면으로 투영해 상하 기울기(피치)는 제거.
	   카메라 Eye 가 타깃보다 높아 그대로 LookAt 하면 타깃이 카메라 쪽으로 기울어진다.
	   좌표는 Ready_Layer_Camera 의 vEye 와 동일해야 함 - 수동 동기화. */
	const _vector vTargetPos = pTarget->Get_Transform()->Get_State(STATE::POSITION);
	const _vector vLookTarget = XMVectorSetW(
		XMVectorSetY(XMLoadFloat3(&CAPTURE_CAMERA_EYE), XMVectorGetY(vTargetPos)), 1.f);
	pTarget->Get_Transform()->LookAt(vLookTarget);

	if (FAILED(m_pGameInstance->Add_GameObject_Ex(CURRENT_LEVEL, strLayerTag, pTarget)))
	{
		Safe_Release(pTarget);
		return E_FAIL;
	}

	m_pCaptureTarget = pTarget;

	return S_OK;
}

HRESULT CLevel_Capture::Ready_Layer_Ball(WNameID strLayerTag)
{
	CMonsterBall::MONSTER_BALL_DESC BallDesc{};
	/* 카메라 vEye(-1.3, 3.2, -7.3) 기준 Y만 하강 - 화면 하단부 근사.
	   도착점은 vAt=(0,0,0) = CaptureTarget 위치. */
	BallDesc.vSpawnPos = _float3(-0.7f, 0.5f, -4.f);
	BallDesc.vTargetPos = _float3(0.f, 0.f, 0.f);
	BallDesc.fFlightDuration = 0.72f;
	BallDesc.fArcHeight = 0.75f;
	BallDesc.fImpactDuration = 0.5f;
	BallDesc.fRotationPerSec = XMConvertToRadians(720.f);

	/* Clone -> Add_GameObject_Ex 패턴 - 인스턴스 포인터 캐시 후 등록. */
	CMonsterBall* pBall = static_cast<CMonsterBall*>(
		m_pGameInstance->Clone_Prototype(
			PROTOTYPE::GAMEOBJECT, ETOUI(LEVEL::GAMEPLAY),
			PROTO_OBJ_MONSTER_BALL, &BallDesc));
	if (nullptr == pBall)
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_GameObject_Ex(CURRENT_LEVEL, strLayerTag, pBall)))
	{
		Safe_Release(pBall);
		return E_FAIL;
	}

	m_pMonsterBall = pBall;   // weak - 레이어가 owner
	return S_OK;
}

HRESULT CLevel_Capture::Ready_Layer_UI(WNameID strLayerTag)
{
	/* ===== 메뉴 시퀀스 (UI_Get.uiseq) ===== */
	CUISequence* pSeq{ nullptr };
	CUISequence::UISEQUENCE_DESC tDesc{};
	tDesc.strPath = "../../DataFiles/UI/UI_Get.uiseq";
	tDesc.iProtoLevel = ETOUI(LEVEL::STATIC);

	pSeq = static_cast<CUISequence*>(m_pGameInstance->Clone_Prototype(
		PROTOTYPE::GAMEOBJECT, ETOUI(LEVEL::STATIC), PROTO_UI_SEQUENCE, &tDesc));
	if (nullptr == pSeq)
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_GameObject_Ex(CURRENT_LEVEL, strLayerTag, pSeq)))
	{
		Safe_Release(pSeq);
		return E_FAIL;
	}

	{
		CUISequence::UISEQUENCE_DESC tMsgDesc{};
		tMsgDesc.strPath = "../../DataFiles/UI/UI_BattleMsg.uiseq";
		tMsgDesc.iProtoLevel = ETOUI(LEVEL::STATIC);

		CUISequence* pMsgSeq = static_cast<CUISequence*>(m_pGameInstance->Clone_Prototype(
			PROTOTYPE::GAMEOBJECT, ETOUI(LEVEL::STATIC), PROTO_UI_SEQUENCE, &tMsgDesc));
		if (nullptr == pMsgSeq)
			return E_FAIL;

		if (FAILED(m_pGameInstance->Add_GameObject_Ex(CURRENT_LEVEL, strLayerTag, pMsgSeq)))
		{
			Safe_Release(pMsgSeq);
			return E_FAIL;
		}

		CBattleMsg* pCaptureMsg = CBattleMsg::Create();
		if (nullptr == pCaptureMsg)
			return E_FAIL;

		if (FAILED(pCaptureMsg->Initialize(pMsgSeq)))
		{
			Safe_Release(pCaptureMsg);
			return E_FAIL;
		}

		pCaptureMsg->Close();

		if (FAILED(UI_Register(pCaptureMsg, ETOUI(LEVEL::CAPTURE))))
		{
			Safe_Release(pCaptureMsg);
			return E_FAIL;
		}

		m_pCaptureMsg = pCaptureMsg;      // weak - UI Hub owns
		Safe_Release(pCaptureMsg);
	}

	{
		CCapture_Menu* pMenu = CCapture_Menu::Create();
		if (nullptr == pMenu)
			return E_FAIL;

		if (FAILED(pMenu->Initialize(pSeq)))
		{
			Safe_Release(pMenu);
			return E_FAIL;
		}

		pMenu->Bind(m_pCaptureTarget);

		/* Activate: MENU 인덱스 -> 행동 라우팅.
			READY=메뉴 닫고 AIMING 진입 / BAG·HELP=미구현 로그 / RUN=Request_Run. */
		pMenu->Set_OnActivate([this](_int iIdx)
			{
				const CCapture_Menu::MENU eMenu = static_cast<CCapture_Menu::MENU>(iIdx);

					switch (eMenu)
					{
					case CCapture_Menu::MENU::READY:
						if (nullptr != m_pCaptureMenu)
							m_pCaptureMenu->Close();

						if (nullptr != m_pCaptureManager)
							m_pCaptureManager->Enter_Aiming();

						Set_AimingCameraControl(true);

						if (nullptr != m_pMonsterBall)
						{
							m_pMonsterBall->Reset();
							Update_AimPose();
							m_pMonsterBall->Show();
						}
						break;

					case CCapture_Menu::MENU::BAG:
						OutputDebugStringW(L"[CCapture_Menu] BAG 선택 - 미구현\n");
						break;

					case CCapture_Menu::MENU::HELP:
						OutputDebugStringW(L"[CCapture_Menu] HELP 선택 - 미구현\n");
						break;

					case CCapture_Menu::MENU::RUN:
						/* 도망간다 - Request_Run() 로 FAIL_RUN + DONE 전이.
						   Is_Done() 분기가 Pop_Level 실행. */
						m_pGameInstance->Play(L"SFX/run.wav", CHANNELID::SFX, 0.8f);

						if (nullptr != m_pCaptureManager)
							m_pCaptureManager->Request_Run();
						break;

					default:
						break;
					}
					});

		/* Cancel(ESC): "도망간다" 와 동일 동작 - Request_Run 으로 통합 라우팅.
		   Request_Run 이 FAIL_RUN + DONE 으로 전이 -> Is_Done() 분기가 Pop_Level 실행.
		   m_bExitRequested 경로는 단위 δ 에서 제거 예정. */
		pMenu->Set_OnCancel([this]()
			{
				if (nullptr != m_pCaptureManager)
					m_pCaptureManager->Request_Run();
			});

		if (FAILED(UI_Register(pMenu, ETOUI(LEVEL::CAPTURE))))
		{
			Safe_Release(pMenu);
			return E_FAIL;
		}

		m_pCaptureMenu = pMenu;  // weak - UI Hub owns
		Safe_Release(pMenu);
	}

	/* 캡처 진입 메시지가 먼저 출력되므로 메뉴는 닫힌 상태로 시작한다. */
	if (nullptr != m_pCaptureMenu)
		m_pCaptureMenu->Close();

	/* ===== 커서 시퀀스 - Hub 가 단일 인스턴스로 공유 =====
	   다른 UI 시퀀스 등록을 모두 마친 뒤 마지막에 추가해 최상위에 그려지도록 함. */
	{
		CUISequence::UISEQUENCE_DESC tCursorDesc{};
		tCursorDesc.strPath = "../../DataFiles/UI/UI_Cursor.uiseq";
		tCursorDesc.iProtoLevel = ETOUI(LEVEL::STATIC);

		CUISequence* pCursorSeq = static_cast<CUISequence*>(m_pGameInstance->Clone_Prototype(
			PROTOTYPE::GAMEOBJECT, ETOUI(LEVEL::STATIC), PROTO_UI_SEQUENCE, &tCursorDesc));
			if (nullptr == pCursorSeq)
				return E_FAIL;

		if (FAILED(m_pGameInstance->Add_GameObject_Ex(CURRENT_LEVEL, LAYER_UI, pCursorSeq)))
		{
			Safe_Release(pCursorSeq);
			return E_FAIL;
		}

		UI_Set_Cursor_Sequence(pCursorSeq);  // Hub 에 weak 주입
		m_pCursorSeq = pCursorSeq;           // 레벨도 weak (Add_GameObject_Ex 가 owner)
	}

	return S_OK;
}

void CLevel_Capture::Set_AimingCameraControl(_bool bEnabled)
{
	if (nullptr != m_pCaptureCamera)
		m_pCaptureCamera->Set_ControlEnabled(bEnabled);
}

void CLevel_Capture::Update_AimPose()
{
	if (nullptr == m_pCaptureCamera || nullptr == m_pMonsterBall)
		return;

	CTransform* pCamTransform = m_pCaptureCamera->Get_Transform();
	if (nullptr == pCamTransform)
		return;

	_vector vCamPos = pCamTransform->Get_State(STATE::POSITION);
	_vector vLook = pCamTransform->Get_State(STATE::LOOK);
	_vector vUp = pCamTransform->Get_State(STATE::UP);

	if (XMVectorGetX(XMVector3LengthSq(vLook)) <= 0.000001f)
		return;

	vLook = XMVector3Normalize(vLook);
	vUp = XMVector3Normalize(vUp);

	constexpr _float AIM_START_FORWARD = 0.65f;
	constexpr _float AIM_START_DOWN = 0.62f;
	constexpr _float AIM_MIN_DISTANCE = 3.f;
	constexpr _float AIM_FALLBACK_DISTANCE = 10.f;

	_vector vTargetPos = vCamPos + vLook * AIM_FALLBACK_DISTANCE;

	if (nullptr != m_pCaptureTarget)
	{
		const _float3 vCaptureCenterF = m_pCaptureTarget->Get_CaptureCenter();
		const _vector vCaptureCenter = XMLoadFloat3(&vCaptureCenterF);

		_float fDistance = XMVectorGetX(XMVector3Dot(vCaptureCenter - vCamPos, vLook));
		if (fDistance < AIM_MIN_DISTANCE)
			fDistance = AIM_MIN_DISTANCE;

		vTargetPos = vCamPos + vLook * fDistance;
	}

	const _vector vStartPos = vCamPos + vLook * AIM_START_FORWARD - vUp * AIM_START_DOWN;

	_float3 vStartPosF{};
	_float3 vTargetPosF{};
	XMStoreFloat3(&vStartPosF, vStartPos);
	XMStoreFloat3(&vTargetPosF, vTargetPos);

	m_pMonsterBall->Set_AimPose(vStartPosF, vTargetPosF);
}

void CLevel_Capture::Update_IntroBallPose()
{
	if (nullptr == m_pCaptureCamera || nullptr == m_pMonsterBall)
		return;

	CTransform* pCamTransform = m_pCaptureCamera->Get_Transform();
	if (nullptr == pCamTransform)
		return;

	_vector vCamPos = pCamTransform->Get_State(STATE::POSITION);
	_vector vLook = pCamTransform->Get_State(STATE::LOOK);
	_vector vUp = pCamTransform->Get_State(STATE::UP);
	_vector vRight = pCamTransform->Get_State(STATE::RIGHT);

	if (XMVectorGetX(XMVector3LengthSq(vLook)) <= 0.000001f)
		return;

	vLook = XMVector3Normalize(vLook);
	vUp = XMVector3Normalize(vUp);
	vRight = XMVector3Normalize(vRight);

	//constexpr _float INTRO_START_FORWARD = 1.8f;
	//constexpr _float INTRO_START_DOWN = 0.35f;
	//constexpr _float INTRO_START_RIGHT = 0.f;
	constexpr _float INTRO_START_FORWARD = 1.15f;
	constexpr _float INTRO_START_DOWN = 1.00f;
	constexpr _float INTRO_START_RIGHT = 0.f;
	constexpr _float INTRO_MIN_DISTANCE = 3.f;
	constexpr _float INTRO_FALLBACK_DISTANCE = 10.f;

	_vector vTargetPos = vCamPos + vLook * INTRO_FALLBACK_DISTANCE;

	if (nullptr != m_pCaptureTarget)
	{
		const _float3 vCaptureCenterF = m_pCaptureTarget->Get_CaptureCenter();
		const _vector vCaptureCenter = XMLoadFloat3(&vCaptureCenterF);

		_float fDistance = XMVectorGetX(XMVector3Dot(vCaptureCenter - vCamPos, vLook));
		if (fDistance < INTRO_MIN_DISTANCE)
			fDistance = INTRO_MIN_DISTANCE;

		vTargetPos = vCamPos + vLook * fDistance;
	}

	const _vector vStartPos =
		vCamPos
		+ vLook * INTRO_START_FORWARD
		- vUp * INTRO_START_DOWN
		+ vRight * INTRO_START_RIGHT;

	_float3 vStartPosF{};
	_float3 vTargetPosF{};
	XMStoreFloat3(&vStartPosF, vStartPos);
	XMStoreFloat3(&vTargetPosF, vTargetPos);

	m_pMonsterBall->Set_AimPose(vStartPosF, vTargetPosF);
}

void CLevel_Capture::Reset_CaptureCameraPose()
{
	if (nullptr == m_pCaptureCamera)
		return;

	CTransform* pCamTransform = m_pCaptureCamera->Get_Transform();
	if (nullptr == pCamTransform)
		return;

	pCamTransform->Set_State(
		STATE::POSITION,
		XMVectorSetW(XMLoadFloat3(&CAPTURE_CAMERA_EYE), 1.f));

	pCamTransform->LookAt(
		XMVectorSetW(XMLoadFloat3(&CAPTURE_CAMERA_AT), 1.f));

	m_vStageBallAirCenter = {};
	m_vStageBallGroundCenter = {};
	m_vStageCameraTargetEye = {};
	m_vStageCameraTargetAt = {};
	m_iAppliedShakeIndex = { -1 };
}

void CLevel_Capture::Begin_StageDrop()
{
	if (nullptr == m_pMonsterBall)
		return;

	Set_AimingCameraControl(false);

	m_pMonsterBall->Begin_StageDrop(
		m_vStageBallAirCenter,
		m_vStageBallGroundCenter,
		m_vStageCameraTargetEye,
		STAGE_DROP_DURATION);
}

void CLevel_Capture::Begin_StageCamera()
{
	if (nullptr == m_pCaptureCamera || nullptr == m_pCaptureTarget)
		return;

	CTransform* pCamTransform = m_pCaptureCamera->Get_Transform();
	if (nullptr == pCamTransform)
		return;

	const _float3 vPivotF = m_pCaptureTarget->Get_EffectPivot();
	const _vector vPivot = XMLoadFloat3(&vPivotF);

	_vector vLookFlat = pCamTransform->Get_State(STATE::LOOK);
	vLookFlat = XMVectorSetY(vLookFlat, 0.f);

	if (XMVectorGetX(XMVector3LengthSq(vLookFlat)) <= 0.000001f)
		vLookFlat = XMVectorSet(0.f, 0.f, 1.f, 0.f);

	vLookFlat = XMVector3Normalize(vLookFlat);

	const _vector vGroundCenter = XMVectorSet(
		vPivotF.x,
		STAGE_SCENE_BALL_GROUND_CENTER_Y,
		vPivotF.z,
		0.f);

	const _vector vAirCenter = XMVectorSet(
		vPivotF.x,
		STAGE_SCENE_BALL_AIR_CENTER_Y,
		vPivotF.z,
		0.f);

	const _vector vTargetAt =
		vPivot + XMVectorSet(0.f, STAGE_SCENE_CAMERA_LOOK_UP_OFFSET, 0.f, 0.f);

	const _vector vTargetEye =
		vTargetAt
		- vLookFlat * STAGE_SCENE_CAMERA_BACK_DISTANCE
		+ XMVectorSet(0.f, STAGE_SCENE_CAMERA_HEIGHT_OFFSET, 0.f, 0.f);

	XMStoreFloat3(&m_vStageBallGroundCenter, vGroundCenter);
	XMStoreFloat3(&m_vStageBallAirCenter, vAirCenter);
	XMStoreFloat3(&m_vStageCameraTargetAt, vTargetAt);
	XMStoreFloat3(&m_vStageCameraTargetEye, vTargetEye);

	Set_AimingCameraControl(false);
	Apply_StageCameraPose();
}

void CLevel_Capture::Apply_StageCameraPose()
{
	if (nullptr == m_pCaptureCamera)
		return;

	CTransform* pCamTransform = m_pCaptureCamera->Get_Transform();
	if (nullptr == pCamTransform)
		return;

	pCamTransform->Set_State(
		STATE::POSITION,
		XMVectorSetW(XMLoadFloat3(&m_vStageCameraTargetEye), 1.f));

	pCamTransform->LookAt(
		XMVectorSetW(XMLoadFloat3(&m_vStageCameraTargetAt), 1.f));
}

void CLevel_Capture::Begin_CaptureIntroView()
{
	if (m_bCaptureIntroMessageActive || m_bCaptureIntroMessageFinished)
		return;

	if (nullptr != m_pCaptureMenu)
		m_pCaptureMenu->Close();

	if (nullptr == m_pCaptureMsg)
	{
		if (nullptr != m_pCaptureMenu)
			m_pCaptureMenu->Open(true);

		m_bCaptureIntroMessageFinished = true;
		return;
	}

	m_pCaptureMsg->Set_Message(Build_CaptureIntroMessage());
	m_pCaptureMsg->Open(true);

	m_bCaptureIntroMessageActive = true;
	m_fCaptureIntroMessageDoneElapsed = 0.f;
}

void CLevel_Capture::Tick_CaptureIntroView(_float fTimeDelta)
{
	if (false == m_bCaptureIntroMessageActive)
		return;

	if (nullptr != m_pCaptureManager
		&& CAPTURE_PHASE::INTRO != m_pCaptureManager->Get_Phase())
	{
		m_bCaptureIntroMessageActive = false;
		m_bCaptureIntroMessageFinished = true;
		return;
	}

	if (nullptr != m_pCaptureMsg && false == m_pCaptureMsg->Is_Done())
		return;

	m_fCaptureIntroMessageDoneElapsed += fTimeDelta;
	if (m_fCaptureIntroMessageDoneElapsed < CAPTURE_INTRO_MESSAGE_HOLD)
		return;

	if (nullptr != m_pCaptureMsg)
		m_pCaptureMsg->Close();

	if (nullptr != m_pCaptureMenu)
		m_pCaptureMenu->Open(true);

	m_bCaptureIntroMessageActive = false;
	m_bCaptureIntroMessageFinished = true;
}

_wstring CLevel_Capture::Build_CaptureIntroMessage() const
{
	const CPokemonData_Manager* pDataMgr = CPokemonData_Manager::GetInstance();
	const SPECIES_DATA* pSpecies = (nullptr != pDataMgr)
		? pDataMgr->Find_Species(m_tEnv.iSpeciesID)
		: nullptr;

	_wstring strName = (nullptr != pSpecies) ? pSpecies->szName : TEXT("포켓몬");
	return TEXT("앗! 야생 ") + strName + TEXT("이(가)\n 튀어나왔다!");
}

void CLevel_Capture::Tick_CaptureSuccessView()
{
	if (nullptr == m_pCaptureManager)
		return;

	if (CAPTURE_PHASE::SUCCESS_VIEW != m_pCaptureManager->Get_Phase())
		return;

	const _bool bConfirm =
		m_pGameInstance->Key_Down(DIK_RETURN) ||
		m_pGameInstance->Key_Down(DIK_SPACE);

	if (false == bConfirm)
		return;

	if (nullptr != m_pCaptureMsg &&
		m_pCaptureMsg->Is_Open() &&
		false == m_pCaptureMsg->Is_Done())
	{
		m_pCaptureMsg->Complete();
		return;
	}

	if (nullptr != m_pCaptureMsg)
		m_pCaptureMsg->Close();

	m_pCaptureManager->Confirm_SuccessView();
}

void CLevel_Capture::Begin_CaptureSuccessView()
{
	Set_AimingCameraControl(false);

	if (nullptr != m_pMonsterBall)
	{
		CEffect* pEffect = CEffect_Manager::GetInstance()->PlayAt(
			"capture_success",
			m_pMonsterBall->Get_CenterPosition());

		OutputDebugStringA(pEffect ? "[Capture] success effect ok\n" : "[Capture] success effect null\n");
	}

	if (nullptr != m_pCaptureMsg)
	{
		m_pCaptureMsg->Set_Message(Build_CaptureSuccessMessage());
		m_pCaptureMsg->Open(true);
	}
}

_wstring CLevel_Capture::Build_CaptureSuccessMessage() const
{
	const CPokemonData_Manager* pDataMgr = CPokemonData_Manager::GetInstance();
	const SPECIES_DATA* pSpecies = (nullptr != pDataMgr)
		? pDataMgr->Find_Species(m_tEnv.iSpeciesID)
		: nullptr;

	m_pGameInstance->Play_BGM(L"BGM/1-19. Successful Catch! (Wild Pokemon).mp3", 0.3f);

	_wstring strName = (nullptr != pSpecies) ? pSpecies->szName : TEXT("포켓몬");
	return strName + TEXT("을 잡았다!");
}

CLevel_Capture* CLevel_Capture::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const LEVEL_ENTRY_DESC* pEntryDesc)
{
	if (nullptr == pEntryDesc)
	{
		MSG_BOX("Capture entry desc is missing");
		return nullptr;
	}

	const void* pPayload = pEntryDesc->Get_Payload(LEVEL_ENTRY_PAYLOAD::CAPTURE_ENV, sizeof(CAPTURE_ENV));
	if (nullptr == pPayload)
	{
		MSG_BOX("Capture env payload is invalid");
		return nullptr;
	}

	const CAPTURE_ENV& tEnv = *static_cast<const CAPTURE_ENV*>(pPayload);

	CLevel_Capture* pInstance = new CLevel_Capture(pDevice, pContext, tEnv);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_Capture");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Capture::Free()
{
	UI_Set_Cursor_Sequence(nullptr);
	UI_Cleanup_Level(ETOUI(LEVEL::CAPTURE));
	m_pCaptureCamera = nullptr;
	m_pCursorSeq = nullptr;
	m_pCaptureMenu = nullptr;
	m_pCaptureTarget = nullptr;
	m_pCaptureMsg = nullptr;

	Safe_Release(m_pCaptureManager);

	__super::Free();
}
