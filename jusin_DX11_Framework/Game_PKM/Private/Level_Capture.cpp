#include "Level_Capture.h"
#include "Capture_Manager.h"
#include "Camera_Free.h"
#include "Actor_CaptureTarget.h"
#include "Body_Pokemon.h"
#include "PokemonData_Manager.h"
#include "Capture_Menu.h"
#include "Game_API.h"
#include "MonsterBall.h"
#include "RenderRule_Manager.h"
#include "Player_Status.h"

#include "GameInstance.h"

NS_BEGIN(Game_PKM)
static constexpr _uint CURRENT_LEVEL = ETOUI(LEVEL::CAPTURE);

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

	if (FAILED(Ready_Layer_Camera(LAYER_CAMERA)))
		return E_FAIL;

	if (FAILED(Ready_Layer_Battler(LAYER_MONSTER)))
		return E_FAIL;

	if (FAILED(Ready_Layer_Ball(LAYER_INTERACTABLE)))
		return E_FAIL;

	if (FAILED(Ready_Layer_UI(LAYER_UI)))
		return E_FAIL;

	if (CPlayer_Status* pPlayerState = Find_PlayerState(m_pGameInstance))
		pPlayerState->Mark_DexSeen(m_tEnv.iSpeciesID);

	m_pCaptureManager->Begin();

	return S_OK;
}

void CLevel_Capture::Update(_float fTimeDelta)
{
	if (nullptr == m_pCaptureManager)
		return;

	/* AIMING 에서만 좌클릭 = 볼 던지기. 재던지기 시 볼이 DONE 이면 Reset 선행. */
	if (m_pGameInstance->Mouse_Down(DIMB::LBUTTON)
		&& CAPTURE_PHASE::AIMING == m_pCaptureManager->Get_Phase())
	{
		if (nullptr != m_pMonsterBall
			&& CMonsterBall::BALL_STATE::DONE == m_pMonsterBall->Get_State())
		{
			m_pMonsterBall->Reset();
		}

		m_pCaptureManager->Try_Throw();

		if (nullptr != m_pMonsterBall)
			m_pMonsterBall->Launch();
	}

	m_pCaptureManager->Update(fTimeDelta);

	UI_Update_All(fTimeDelta);

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

HRESULT CLevel_Capture::Ready_Layer_Camera(WNameID strLayerTag)
{
	CCamera_Free::CAMERA_FREE_DESC CameraDesc = {};

	CameraDesc.vEye = _float3(-1.3f, 3.2f, -7.3f);
	CameraDesc.vAt = _float3(0.f, 0.f, 0.f);
	CameraDesc.fFovy = XMConvertToRadians(35.f);
	CameraDesc.fNear = 0.1f;
	CameraDesc.fFar = 500.f;
	CameraDesc.fSpeedPerSec = 20.f;
	CameraDesc.fRotationPerSec = XMConvertToRadians(180.f);
	CameraDesc.fMouseSensor = 0.03f;

	if (FAILED(m_pGameInstance->Add_GameObject(
		ETOUI(LEVEL::STATIC), PROTO_OBJ_CAMERA_FREE,
		CURRENT_LEVEL, strLayerTag,
		&CameraDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Capture::Ready_Layer_Battler(WNameID strLayerTag)
{
	/* CAPTURE_ENV.iSpeciesID 기반 모델 매핑 — PokemonData_Manager 조회. */
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
	BodyDesc.iDefaultAnim = 0;
	BodyDesc.bLoop = true;
	BodyDesc.fScale = 1.f;
	BodyDesc.pRenderRule = pRuleManager->Find_PokemonRenderRule(pSpecies);
	if (nullptr == BodyDesc.pRenderRule)
		return E_FAIL;

	CActor_CaptureTarget::ACTOR_CAPTURE_DESC TargetDesc{};
	TargetDesc.iBodyProtoLevel = ETOUI(LEVEL::STATIC);
	TargetDesc.iComponentLevel = ETOUI(LEVEL::GAMEPLAY);
	TargetDesc.strBodyProtoTag = PROTO_OBJ_BODY_POKEMON;
	TargetDesc.pBodyDesc = &BodyDesc;
	TargetDesc.iSpeciesID = m_tEnv.iSpeciesID;
	TargetDesc.iLevel = m_tEnv.iLevel;
	TargetDesc.iInitialBallItemID = m_tEnv.iInitialBallItemID;
	TargetDesc.bCaughtBefore = false;
	TargetDesc.vSpawnPos = _float3(0.f, 0.f, 0.f);    // 카메라 vAt 지점

	/* Clone → LookAt → Add_GameObject_Ex 패턴.
	   Add_GameObject 와 달리 인스턴스 포인터를 잡을 수 있어 등록 전에 회전 적용 가능. */
	CActor_CaptureTarget* pTarget = static_cast<CActor_CaptureTarget*>(
		m_pGameInstance->Clone_Prototype(
			PROTOTYPE::GAMEOBJECT, ETOUI(LEVEL::GAMEPLAY),
			PROTO_OBJ_ACTOR_CAPTURE_TARGET, &TargetDesc));
	if (nullptr == pTarget)
		return E_FAIL;

	/* 카메라 위치를 정면으로 보도록 회전.
	   좌표는 Ready_Layer_Camera 의 vEye 와 동일해야 함 — 수동 동기화. */
	const _vector vCamPos = XMVectorSet(-1.3f, 3.2f, -7.3f, 1.f);
	pTarget->Get_Transform()->LookAt(vCamPos);

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
	/* 카메라 vEye(-1.3, 3.2, -7.3) 기준 Y만 하강 — 화면 하단부 근사.
	   도착점은 vAt=(0,0,0) = CaptureTarget 위치. */
	BallDesc.vSpawnPos = _float3(-0.7f, 0.5f, -4.f);
	BallDesc.vTargetPos = _float3(0.f, 0.f, 0.f);
	BallDesc.fFlightDuration = 1.0f;
	BallDesc.fArcHeight = 2.0f;
	BallDesc.fImpactDuration = 0.5f;

	/* Clone → Add_GameObject_Ex 패턴 — 인스턴스 포인터 캐시 후 등록. */
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

	m_pMonsterBall = pBall;   // weak — 레이어가 owner
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
		CCapture_Menu* pMenu = CCapture_Menu::Create();
		if (nullptr == pMenu)
			return E_FAIL;

		if (FAILED(pMenu->Initialize(pSeq)))
		{
			Safe_Release(pMenu);
			return E_FAIL;
		}

		pMenu->Bind(m_pCaptureTarget);

		/* Activate: MENU 인덱스 → 행동 라우팅.
			READY=메뉴 닫고 AIMING 진입 / BAG·HELP=미구현 로그 / RUN=Request_Run. */
		pMenu->Set_OnActivate([this](_int iIdx)
			{
				const CCapture_Menu::MENU eMenu = static_cast<CCapture_Menu::MENU>(iIdx);

					switch (eMenu)
					{
					case CCapture_Menu::MENU::READY:
						/* 메뉴 닫고 AIMING 진입. 이후 마우스로 시점 조작 + 좌클릭 던지기.
						   볼 숨김 — 좌클릭 발사(Launch) 시 자동 가시화. */
						if (nullptr != m_pCaptureMenu)
							m_pCaptureMenu->Close();
						if (nullptr != m_pCaptureManager)
							m_pCaptureManager->Enter_Aiming();
						if (nullptr != m_pMonsterBall)
							m_pMonsterBall->Hide();
						break;

					case CCapture_Menu::MENU::BAG:
						OutputDebugStringW(L"[CCapture_Menu] BAG 선택 — 미구현\n");
						break;

					case CCapture_Menu::MENU::HELP:
						OutputDebugStringW(L"[CCapture_Menu] HELP 선택 — 미구현\n");
						break;

					case CCapture_Menu::MENU::RUN:
						/* 도망간다 — Request_Run() 로 FAIL_RUN + DONE 전이.
						   Is_Done() 분기가 Pop_Level 실행. */
						if (nullptr != m_pCaptureManager)
							m_pCaptureManager->Request_Run();
						break;

					default:
						break;
					}
					});

		/* Cancel(ESC): "도망간다" 와 동일 동작 — Request_Run 으로 통합 라우팅.
		   Request_Run 이 FAIL_RUN + DONE 으로 전이 → Is_Done() 분기가 Pop_Level 실행.
		   m_bExitRequested 경로는 단위 δ 에서 제거 예정. */
		pMenu->Set_OnCancel([this]()
			{
				if (nullptr != m_pCaptureManager)
					m_pCaptureManager->Request_Run();
			});

		if (FAILED(UI_Register(pMenu)))
		{
			Safe_Release(pMenu);
			return E_FAIL;
		}

		m_pCaptureMenu = pMenu;  // weak — UI Hub owns
		Safe_Release(pMenu);
	}

	/* Director 가 범위 외 → 등록 직후 직접 Open (Battle 의 BattlePlate 양식과 동일).
	   Open 안 하면 m_bOpen=false 라 입력 무시. */
	if (nullptr != m_pCaptureMenu)
		m_pCaptureMenu->Open();

	/* ===== 커서 시퀀스 — Hub 가 단일 인스턴스로 공유 =====
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
	UI_Close_All();
	m_pCursorSeq = nullptr;
	m_pCaptureMenu = nullptr;
	m_pCaptureTarget = nullptr;

	Safe_Release(m_pCaptureManager);

	__super::Free();
}
