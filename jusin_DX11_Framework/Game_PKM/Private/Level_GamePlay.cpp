#include "Level_GamePlay.h"
#include "Camera_Free.h"
#include "Player_LGPE.h"
#include "Effect_Star.h"
#include "UIButton_Glow.h"
#include "UIButton_Layered.h"
#include "UIButton_Group.h"
#include "Menu.h"
#include "Game_API.h"
#include "Level_Battle.h"
#include "Level_Capture.h"
#include "Game_LevelEntry.h"
#include "Battle_Session.h"
#include "Body_Human.h"
#include "Body_Pokemon.h"
#include "Actor_NPC.h"
#include "Actor_WildPokemon.h"
#include "RenderRule_Manager.h"

#include "GameInstance.h"
#include "UISequence.h"

NS_BEGIN(Game_PKM)
static constexpr _uint CURRENT_LEVEL = ETOUI(LEVEL::GAMEPLAY);
NS_END

CLevel_GamePlay::CLevel_GamePlay(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel{ pDevice, pContext }
{
}

HRESULT CLevel_GamePlay::Initialize()
{
	if (FAILED(Ready_Lights()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera(LAYER_CAMERA)))
		return E_FAIL;

	if (FAILED(Ready_Layer_BackGround(LAYER_BACKGROUND)))
		return E_FAIL;

	if (FAILED(Ready_Layer_Player(LAYER_PLAYER)))
		return E_FAIL;

	if (FAILED(Ready_Layer_Monster(LAYER_MONSTER)))
		return E_FAIL;
	
	if (FAILED(Ready_Layer_NPC(LAYER_NPC)))
		return E_FAIL;

	if (FAILED(Ready_Layer_Wild(LAYER_INTERACTABLE)))
		return E_FAIL;

	if (FAILED(Ready_Layer_Effect(LAYER_EFFECT)))
		return E_FAIL;

	if (FAILED(Ready_Layer_UI(LAYER_UI)))
		return E_FAIL;

	CCamera* pCamera = static_cast<CCamera*>(*(m_pGameInstance->Get_ObjectList(ETOUI(LEVEL::GAMEPLAY), LAYER_CAMERA)->begin()));
	CPlayer_LGPE* pPlayer = static_cast<CPlayer_LGPE*>(*(m_pGameInstance->Get_ObjectList(ETOUI(LEVEL::GAMEPLAY), LAYER_PLAYER)->begin()));
	pCamera->Set_FollowTarget(pPlayer->Get_Transform());
	pCamera->Set_FollowOffset({ 0.f, 6.5f, -7.5f });
	m_pGameInstance->Set_MainCamera(pCamera);
	pCamera->Set_Following(true);
	pCamera->Set_ControlEnabled(true);

	m_pGameInstance->Play_BGM(L"BGM/1-04. Pallet Town Theme.mp3", 0.3f);

	return S_OK;
}

void CLevel_GamePlay::Update(_float fTimeDelta)
{
	/* 트랜지션 진행 중이면 입력 분기 전부 차단. 경과 누적 후 임계값 도달 시 Push_Level. */
	if (TRANSITION_STATE::BUSY == m_eTransition)
	{
		m_fTransitionElapsed += fTimeDelta;

		if (m_fTransitionElapsed >= TRANSITION_PUSH_AT_SEC)
		{
			/* Push 직전 시퀀스 가시화 해제. (Paused 동안 GAMEPLAY 는 렌더되지 않아 어차피
			   Pop 으로 OnResume 됐을 때 다음 트리거를 위해 깨끗한 상태로 둠.) */
			if (nullptr != m_pFadeBattleSeq)
				m_pFadeBattleSeq->Set_Visible(false);

			/* CAPTURE 진입: CLevel_Capture Push. */
			if (LEVEL::CAPTURE == m_PendingEntryDesc.eNextLevelID)
			{
				CLevel_Capture* pCapture = CLevel_Capture::Create(m_pDevice, m_pContext, &m_PendingEntryDesc);
				if (nullptr == pCapture)
				{
					m_pGameInstance->Set_InputState(INPUT_STATE::GAMEPLAY);
					m_eTransition = TRANSITION_STATE::IDLE;
					m_fTransitionElapsed = 0.f;
					return;
				}

				/* Push 직전 — Fade 가림 상태에서 충돌 트리거 WildPokemon 을 deferred 삭제.
				   Layer 가 다음 Update 사이클에서 Safe_Release + 리스트 제거 →
				   Pop 후 OnResume 시점에 collider 검사 대상에서 자동 빠짐 → 무한 루프 방지. */
				if (nullptr != m_pPendingDeleteWild)
				{
					m_pPendingDeleteWild->Set_Dead();
					m_pPendingDeleteWild = nullptr;   // weak 초기화 — dangling 차단
				}

				if (FAILED(m_pGameInstance->Push_Level(ETOI(LEVEL::CAPTURE), pCapture)))
				{
					Safe_Release(pCapture);
					m_pGameInstance->Set_InputState(INPUT_STATE::GAMEPLAY);
					m_eTransition = TRANSITION_STATE::IDLE;
					m_fTransitionElapsed = 0.f;
					return;
				}

				/* Push 직후 입력 상태 복귀. CAPTURE 측에서 별도 상태가 필요하면 Initialize 에서 덮어쓰면 됨. */
				m_pGameInstance->Set_InputState(INPUT_STATE::GAMEPLAY);

				m_eTransition = TRANSITION_STATE::IDLE;
				m_fTransitionElapsed = 0.f;
				return;
			}

			CLevel_Battle* pBattle = CLevel_Battle::Create(m_pDevice, m_pContext, &m_PendingEntryDesc);
			if (nullptr == pBattle)
			{
				m_pGameInstance->Set_InputState(INPUT_STATE::GAMEPLAY);
				m_eTransition = TRANSITION_STATE::IDLE;
				m_fTransitionElapsed = 0.f;
				return;
			}

			if (FAILED(m_pGameInstance->Push_Level(ETOI(LEVEL::BATTLE), pBattle)))
			{
				Safe_Release(pBattle);
				m_pGameInstance->Set_InputState(INPUT_STATE::GAMEPLAY);
				m_eTransition = TRANSITION_STATE::IDLE;
				m_fTransitionElapsed = 0.f;
				return;
			}

			/* Push 직후 입력 상태를 GAMEPLAY 로 복귀.
			   BATTLE 측에서 별도 상태(MENU 등)를 원하면 Initialize 에서 덮어쓰면 됨. */
			m_pGameInstance->Set_InputState(INPUT_STATE::GAMEPLAY);

			/* Push 성공. GAMEPLAY 는 paused 상태로 전환되어 본 Update 는 더 이상 호출되지 않음 */
			m_eTransition = TRANSITION_STATE::IDLE;
			m_fTransitionElapsed = 0.f;
			return;
		}

		UI_Update_All(fTimeDelta);
		return;
	}

	if (m_pGameInstance->Key_Down(DIK_F2))
		m_pGameInstance->Toggle_CameraFollow();

	if (m_pGameInstance->Key_Down(DIK_F3))
		m_pGameInstance->Toggle_Debug();

	/* F4 — 메뉴 열기/닫기 토글. Open() 이 시퀀스 Play 도 같이 트리거. */
	if (m_pGameInstance->Key_Down(DIK_F4) && nullptr != m_pMenu)
	{
		if (m_pMenu->Is_Open())
			m_pMenu->Close();
		else
			m_pMenu->Open();
	}

	if (m_pGameInstance->Key_Down(DIK_P))
	{
		/* UI 열린 상태면 트리거 무시. Fade 시퀀스 미보유면 진입 불가. */
		if (UI_Is_AnyOpen())
			return;
		if (nullptr == m_pFadeBattleSeq)
			return;

		BATTLE_ENV tEnv = {};
		tEnv.eEnvironment = ENVIRONMENT_TYPE::GRASS;
		tEnv.eRule = BATTLE_RULE::WILD_SINGLE;
		tEnv.iBGResourceID = 0;
		tEnv.iZoneID = 0;

		m_PendingEntryDesc.Clear();
		m_PendingEntryDesc.eNextLevelID = LEVEL::BATTLE;

		if (FAILED(m_PendingEntryDesc.Set_Payload(LEVEL_ENTRY_PAYLOAD::BATTLE_ENV, &tEnv,
			sizeof(BATTLE_ENV))))
			return;

		/* Fade 시작과 동시에 BATTLE BGM 시작. 실제 Push_Level 은 본 함수 상단의 경과 임계값 초과 시 호출됨. */
		m_pGameInstance->Play_BGM(L"BGM/1-24. Battle! (Gym Leader).mp3", 0.3f);

		m_pFadeBattleSeq->Set_Visible(true);
		m_pFadeBattleSeq->Play();

		/* 트랜지션 동안 게임 객체 수준 입력 전체 차단(WASD/마우스 등).
		   SYSTEM 키는 LOCKED 에서도 통과하나, 본 함수 상단 BUSY 가드가 추가 방어. */
		m_pGameInstance->Set_InputState(INPUT_STATE::LOCKED);

		m_eTransition = TRANSITION_STATE::BUSY;
		m_fTransitionElapsed = 0.f;

		return;
	}

	/* 등록된 모든 UI 컨트롤러에 Update 전파. 닫혀 있으면 베이스가 즉시 return. */
	UI_Update_All(fTimeDelta);
}

HRESULT CLevel_GamePlay::Render()
{
#ifdef _DEBUG
	SetWindowText(m_pGameInstance->Get_HWND(), TEXT("게임플레이레벨입니다."));
#endif

	return S_OK;
}

void CLevel_GamePlay::OnPause()
{
	/* BATTLE Push 직후 호출. 현재 단계에서는 BGM 변경을 DIK_P 진입 분기에서 처리하므로
	   여기서는 별도 작업 없음. 후속에서 입력 동결·카메라 일시정지 등을 추가할 자리. */
}

void CLevel_GamePlay::OnResume()
{
	/* BATTLE 이 InputState 를 변경했을 수 있으므로 GAMEPLAY 로 강제 복귀.
	   BATTLE 측에서 LOCKED 이나 MENU 로 두고 종료했더라도 안전. */
	m_pGameInstance->Set_InputState(INPUT_STATE::GAMEPLAY);

	/* GAMEPLAY BGM 복원. BGM 키/볼륨은 Initialize 와 동일 값. */
	m_pGameInstance->Play_BGM(L"BGM/1-04. Pallet Town Theme.mp3", 0.3f);

	/* Pause 동안 LAYER_INTERACTABLE 의 액터가 소멸·재배치됐을 수 있으므로
	   Player 의 직전-overlap 캐시를 비워 다음 접촉을 정상적인 Enter 로 처리. */
	if (const list<CGameObject*>* pPlayerList =
		m_pGameInstance->Get_ObjectList(CURRENT_LEVEL, LAYER_PLAYER))
	{
		for (CGameObject* pObject : *pPlayerList)
		{
			if (CPlayer_LGPE* pPlayer = dynamic_cast<CPlayer_LGPE*>(pObject))
			{
				pPlayer->Clear_TouchSet();
				break;
			}
		}
	}
}

void CLevel_GamePlay::Request_Capture(const CAPTURE_ENV& tEnv, CGameObject* pTarget)
{
	/* 이미 다른 트랜지션 중이거나, UI 가 열려 있거나, Fade 시퀀스 미보유면 무시. */
	if (TRANSITION_STATE::IDLE != m_eTransition)
		return;
	if (UI_Is_AnyOpen())
		return;
	if (nullptr == m_pFadeBattleSeq)
		return;

	m_PendingEntryDesc.Clear();
	m_PendingEntryDesc.eNextLevelID = LEVEL::CAPTURE;

	if (FAILED(m_PendingEntryDesc.Set_Payload(LEVEL_ENTRY_PAYLOAD::CAPTURE_ENV, &tEnv,
		sizeof(CAPTURE_ENV))))
		return;

	/* 트랜지션이 실제 시작될 때만 삭제 대상 보관 — 가드 통과 후로 두어
	   UI 열린 상태 등에서 무시된 호출은 WildPokemon 도 그대로 두는 정합성 확보. */
	m_pPendingDeleteWild = pTarget;   // weak

	/* Fade 시퀀스는 BATTLE 과 공유. CAPTURE 전용 Fade·BGM 분기는 후속에서 결정. */
	m_pFadeBattleSeq->Set_Visible(true);
	m_pFadeBattleSeq->Play();

	/* 트랜지션 동안 게임 객체 수준 입력 차단. */
	m_pGameInstance->Set_InputState(INPUT_STATE::LOCKED);

	m_eTransition = TRANSITION_STATE::BUSY;
	m_fTransitionElapsed = 0.f;
}

HRESULT CLevel_GamePlay::Ready_Lights()
{
	m_pGameInstance->Clear_Lights();

	LIGHT_DESC      LightDesc{};

	LightDesc.eType = LIGHT::DIRECTIONAL;
	LightDesc.vDiffuse = _float4(0.75f, 0.75f, 0.75f, 1.f);  // 중성 화이트 (RGB 균일), 살짝 감소
	LightDesc.vAmbient = _float4(0.70f, 0.70f, 0.70f, 1.f);  // 중성 화이트, 강화 → 평평·푸른 끼 제거
	LightDesc.vSpecular = _float4(0.3f, 0.3f, 0.3f, 1.f);  // 변화 없음
	LightDesc.vDirection = _float4(0.5f, -0.5f, 0.5f, 0.f);

	if (FAILED(m_pGameInstance->Add_Light(LightDesc)))
		return E_FAIL;

	//LightDesc.eType = LIGHT::POINT;
	//LightDesc.vDiffuse = _float4(1.f, 0.f, 0.f, 1.f);
	//LightDesc.vAmbient = _float4(0.05f, 0.f, 0.f, 1.f);
	//LightDesc.vSpecular = _float4(1.f, 0.1f, 0.1f, 1.f);
	//LightDesc.vPosition = _float4(10.f, 5.f, 10.f, 1.f);
	//LightDesc.fRange = 15.f;
	//
	//if (FAILED(m_pGameInstance->Add_Light(LightDesc)))
	//	return E_FAIL;
	//
	//LightDesc.eType = LIGHT::POINT;
	//LightDesc.vDiffuse = _float4(0.f, 1.f, 0.f, 1.f);
	//LightDesc.vAmbient = _float4(0.f, 0.05f, 0.f, 1.f);
	//LightDesc.vSpecular = _float4(0.1f, 1.f, 0.1f, 1.f);
	//LightDesc.vPosition = _float4(25.f, 5.f, 10.f, 1.f);
	//LightDesc.fRange = 15.f;
	//
	//if (FAILED(m_pGameInstance->Add_Light(LightDesc)))
	//	return E_FAIL;

	SHADOW_LIGHT_DESC ShadowDesc{};
	ShadowDesc.vEye = _float4(0.f, 8.f, 0.f, 1.f);
	ShadowDesc.vAt = _float4(5.f, 0.f, 5.f, 1.f);
	ShadowDesc.fFovy = XMConvertToRadians(60.f);
	ShadowDesc.fNear = 0.1f;
	ShadowDesc.fFar = 200.f;

	if (FAILED(m_pGameInstance->Set_ShadowLight(ShadowDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Camera(WNameID strLayerTag)
{
	CCamera_Free::CAMERA_FREE_DESC CameraDesc = {};

	CameraDesc.vEye = _float3(0.f, 8.f, -7.f);
	CameraDesc.vAt = _float3(0.f, 0.f, 0.f);
	CameraDesc.fFovy = XMConvertToRadians(35.f);
	CameraDesc.fNear = 0.1f;
	CameraDesc.fFar = 500.f;
	CameraDesc.fSpeedPerSec = 20.f;
	CameraDesc.fRotationPerSec = XMConvertToRadians(180.f);
	CameraDesc.fMouseSensor = 0.03f;

	if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::STATIC), PROTO_OBJ_CAMERA_FREE,
		ETOUI(LEVEL::GAMEPLAY), strLayerTag, &CameraDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_BackGround(WNameID strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::GAMEPLAY), PROTO_OBJ_SKY, ETOUI(LEVEL::GAMEPLAY), strLayerTag)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::GAMEPLAY), PROTO_MAP_TOWN01, ETOUI(LEVEL::GAMEPLAY), strLayerTag)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::GAMEPLAY), PROTO_MAP_TOWN02, ETOUI(LEVEL::GAMEPLAY), strLayerTag)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::GAMEPLAY), PROTO_MAP_ROAD01, ETOUI(LEVEL::GAMEPLAY), strLayerTag)))
		return E_FAIL;

	//if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::GAMEPLAY), PROTO_OBJ_SNOW,
	//	ETOUI(LEVEL::GAMEPLAY), strLayerTag)))
	//	return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Player(WNameID strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::GAMEPLAY), PROTO_OBJ_PLAYER_LGPE, ETOUI(LEVEL::GAMEPLAY), strLayerTag)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Monster(WNameID strLayerTag)
{
	//for (size_t i = 0; i < 20; i++)
	//{
	//	if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::GAMEPLAY), PROTO_OBJ_MONSTER, ETOUI(LEVEL::GAMEPLAY), strLayerTag)))
	//		return E_FAIL;
	//}

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_NPC(WNameID strLayerTag)
{
	auto* pRuleManager = CRenderRule_Manager::GetInstance();
	if (nullptr == pRuleManager)
		return E_FAIL;

	// 1) 인간 NPC
	{
		CBody_Human::BODY_HUMAN_DESC BodyDesc{};
		BodyDesc.strModelProtoTag = PROTO_COM_MODEL_PPL_ROCK;
		BodyDesc.iDefaultAnim = 0;
		BodyDesc.bLoop = true;
		BodyDesc.fScale = 1.f;
		BodyDesc.pRenderRule = pRuleManager->Find_OrLoadMappingRule(
			"../../Resources/Models/people/rock/rock_mapping.json");

		CActor_NPC::ACTOR_NPC_DESC NpcDesc{};
		NpcDesc.iBodyProtoLevel = ETOUI(LEVEL::STATIC);
		NpcDesc.iComponentLevel = ETOUI(LEVEL::GAMEPLAY);
		NpcDesc.strBodyProtoTag = PROTO_OBJ_BODY_HUMAN;
		NpcDesc.pBodyDesc = &BodyDesc;
		NpcDesc.strDialogueKey = L"dialogue_npc_human";
		NpcDesc.vSpawnPos = _float3(21.f, 0.f, -14.f);   // 검수 시 조정

		if (FAILED(m_pGameInstance->Add_GameObject(
			CURRENT_LEVEL, PROTO_OBJ_ACTOR_NPC,
			CURRENT_LEVEL, strLayerTag,
			&NpcDesc)))
			return E_FAIL;
	}

	// 1) 인간 NPC
	{
		CBody_Human::BODY_HUMAN_DESC BodyDesc{};
		BodyDesc.strModelProtoTag = PROTO_COM_MODEL_PPL_WATER;
		BodyDesc.iDefaultAnim = 0;
		BodyDesc.bLoop = true;
		BodyDesc.fScale = 1.f;
		BodyDesc.pRenderRule = pRuleManager->Find_OrLoadMappingRule(
			"../../Resources/Models/people/water/water_mapping.json");

		CActor_NPC::ACTOR_NPC_DESC NpcDesc{};
		NpcDesc.iBodyProtoLevel = ETOUI(LEVEL::STATIC);
		NpcDesc.iComponentLevel = ETOUI(LEVEL::GAMEPLAY);
		NpcDesc.strBodyProtoTag = PROTO_OBJ_BODY_HUMAN;
		NpcDesc.pBodyDesc = &BodyDesc;
		NpcDesc.strDialogueKey = L"dialogue_npc_human";
		NpcDesc.vSpawnPos = _float3(22.f, 0.f, -14.f);   // 검수 시 조정

		if (FAILED(m_pGameInstance->Add_GameObject(
			CURRENT_LEVEL, PROTO_OBJ_ACTOR_NPC,
			CURRENT_LEVEL, strLayerTag,
			&NpcDesc)))
			return E_FAIL;
	}

	// 1) 인간 NPC
	{
		CBody_Human::BODY_HUMAN_DESC BodyDesc{};
		BodyDesc.strModelProtoTag = PROTO_COM_MODEL_PPL_FAT;
		BodyDesc.iDefaultAnim = 0;
		BodyDesc.bLoop = true;
		BodyDesc.fScale = 1.f;
		BodyDesc.pRenderRule = pRuleManager->Find_OrLoadMappingRule(
			"../../Resources/Models/people/fat/fat_mapping.json");

		CActor_NPC::ACTOR_NPC_DESC NpcDesc{};
		NpcDesc.iBodyProtoLevel = ETOUI(LEVEL::STATIC);
		NpcDesc.iComponentLevel = ETOUI(LEVEL::GAMEPLAY);
		NpcDesc.strBodyProtoTag = PROTO_OBJ_BODY_HUMAN;
		NpcDesc.pBodyDesc = &BodyDesc;
		NpcDesc.strDialogueKey = L"dialogue_npc_human";
		NpcDesc.vSpawnPos = _float3(23.f, 0.f, -14.f);   // 검수 시 조정

		if (FAILED(m_pGameInstance->Add_GameObject(
			CURRENT_LEVEL, PROTO_OBJ_ACTOR_NPC,
			CURRENT_LEVEL, strLayerTag,
			&NpcDesc)))
			return E_FAIL;
	}

	// 1) 인간 NPC
	{
		CBody_Human::BODY_HUMAN_DESC BodyDesc{};
		BodyDesc.strModelProtoTag = PROTO_COM_MODEL_DOCTOR;
		BodyDesc.iDefaultAnim = 0;
		BodyDesc.bLoop = true;
		BodyDesc.fScale = 1.f;
		BodyDesc.pRenderRule = pRuleManager->Find_OrLoadMappingRule(
			"../../Resources/Models/people/doctor/doctor_mapping.json");

		CActor_NPC::ACTOR_NPC_DESC NpcDesc{};
		NpcDesc.iBodyProtoLevel = ETOUI(LEVEL::STATIC);
		NpcDesc.iComponentLevel = ETOUI(LEVEL::GAMEPLAY);
		NpcDesc.strBodyProtoTag = PROTO_OBJ_BODY_HUMAN;
		NpcDesc.pBodyDesc = &BodyDesc;
		NpcDesc.strDialogueKey = L"dialogue_npc_human";
		NpcDesc.vSpawnPos = _float3(24.f, 0.f, -14.f);   // 검수 시 조정

		if (FAILED(m_pGameInstance->Add_GameObject(
			CURRENT_LEVEL, PROTO_OBJ_ACTOR_NPC,
			CURRENT_LEVEL, strLayerTag,
			&NpcDesc)))
			return E_FAIL;
	}

	// 2) 포켓몬 NPC (피카츄 외형, 대화 가능)
	{
		CBody_Pokemon::BODY_POKEMON_DESC BodyDesc{};
		BodyDesc.strModelProtoTag = PROTO_COM_MODEL_PM0025_00;
		BodyDesc.strShaderProtoTag = PROTO_COM_SHADER_POKEMON;
		BodyDesc.iDefaultAnim = 0;
		BodyDesc.bLoop = true;
		BodyDesc.fScale = 1.f;
		BodyDesc.pRenderRule = CRenderRule_Manager::GetInstance()->Find_OrLoadMappingRule(
			"../../Resources/Models/pkm/pm0025_00/pm0025_00_mapping.json");

		CActor_NPC::ACTOR_NPC_DESC NpcDesc{};
		NpcDesc.iBodyProtoLevel = ETOUI(LEVEL::STATIC);
		NpcDesc.iComponentLevel = ETOUI(LEVEL::GAMEPLAY);
		NpcDesc.strBodyProtoTag = PROTO_OBJ_BODY_POKEMON;
		NpcDesc.pBodyDesc = &BodyDesc;
		NpcDesc.strDialogueKey = L"dialogue_npc_pokemon";
		NpcDesc.vSpawnPos = _float3(19.f, 0.f, -13.f);  // 검수 시 조정

		if (FAILED(m_pGameInstance->Add_GameObject(
			CURRENT_LEVEL, PROTO_OBJ_ACTOR_NPC,
			CURRENT_LEVEL, strLayerTag,
			&NpcDesc)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Wild(WNameID strLayerTag)
{
	// 1) Wild — Pikachu Lv5
	{
		CBody_Pokemon::BODY_POKEMON_DESC BodyDesc{};
		BodyDesc.strModelProtoTag = PROTO_COM_MODEL_PM0025_00;
		BodyDesc.strShaderProtoTag = PROTO_COM_SHADER_POKEMON;
		BodyDesc.iDefaultAnim = 0;
		BodyDesc.bLoop = true;
		BodyDesc.fScale = 1.f;
		BodyDesc.pRenderRule = nullptr;

		CActor_WildPokemon::ACTOR_WILD_DESC WildDesc{};
		WildDesc.iBodyProtoLevel = ETOUI(LEVEL::STATIC);
		WildDesc.iComponentLevel = ETOUI(LEVEL::GAMEPLAY);
		WildDesc.strBodyProtoTag = PROTO_OBJ_BODY_POKEMON;
		WildDesc.pBodyDesc = &BodyDesc;
		WildDesc.iSpeciesID = 25;
		WildDesc.iLevel = 5;
		WildDesc.vSpawnPos = _float3(19.f, 0.f, -8.f);   // 검수 시 조정

		if (FAILED(m_pGameInstance->Add_GameObject(
			CURRENT_LEVEL, PROTO_OBJ_ACTOR_WILD_POKEMON,
			CURRENT_LEVEL, strLayerTag,
			&WildDesc)))
			return E_FAIL;
	}

	// 2) Wild — Bulbasaur Lv5
	{
		CBody_Pokemon::BODY_POKEMON_DESC BodyDesc{};
		BodyDesc.strModelProtoTag = PROTO_COM_MODEL_PM0001_00;
		BodyDesc.strShaderProtoTag = PROTO_COM_SHADER_POKEMON;
		BodyDesc.iDefaultAnim = 0;
		BodyDesc.bLoop = true;
		BodyDesc.fScale = 1.f;
		BodyDesc.pRenderRule = nullptr;

		CActor_WildPokemon::ACTOR_WILD_DESC WildDesc{};
		WildDesc.iBodyProtoLevel = ETOUI(LEVEL::STATIC);
		WildDesc.iComponentLevel = ETOUI(LEVEL::GAMEPLAY);
		WildDesc.strBodyProtoTag = PROTO_OBJ_BODY_POKEMON;
		WildDesc.pBodyDesc = &BodyDesc;
		WildDesc.iSpeciesID = 1;
		WildDesc.iLevel = 5;
		WildDesc.vSpawnPos = _float3(21.f, 0.f, -8.f);   // 검수 시 조정

		if (FAILED(m_pGameInstance->Add_GameObject(
			CURRENT_LEVEL, PROTO_OBJ_ACTOR_WILD_POKEMON,
			CURRENT_LEVEL, strLayerTag,
			&WildDesc)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Effect(WNameID strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::GAMEPLAY), PROTO_OBJ_EXPLOSION,
		ETOUI(LEVEL::GAMEPLAY), strLayerTag)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_UI(WNameID strLayerTag)
{
	CUISequence* pSeq{ nullptr };
	CUISequence::UISEQUENCE_DESC tDesc{};
	tDesc.strPath = "../../DataFiles/UI/UI_Menu.uiseq";
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
	
	m_pRuntimeUI = pSeq;  // weak

	/* ===== Fade Battle 트랜지션 시퀀스 (F6) — 초기엔 숨김. 트리거 시 Play 시작 ===== */
	{
		CUISequence::UISEQUENCE_DESC tFadeDesc{};
		tFadeDesc.strPath = "../../DataFiles/UI/UI_FadeBattle.uiseq";
		tFadeDesc.iProtoLevel = ETOUI(LEVEL::STATIC);

		CUISequence* pFadeSeq = static_cast<CUISequence*>(m_pGameInstance->Clone_Prototype(
			PROTOTYPE::GAMEOBJECT, ETOUI(LEVEL::STATIC), PROTO_UI_SEQUENCE, &tFadeDesc));
			if (nullptr == pFadeSeq)
				return E_FAIL;

		if (FAILED(m_pGameInstance->Add_GameObject_Ex(CURRENT_LEVEL, strLayerTag, pFadeSeq)))
		{
			Safe_Release(pFadeSeq);
			return E_FAIL;
		}

		pFadeSeq->Set_Visible(false);   // 트리거 전까지 숨김
		m_pFadeBattleSeq = pFadeSeq;    // weak (Add_GameObject_Ex 가 owner)
	}

	/* ===== 커서 시퀀스 — Hub 가 단일 인스턴스로 공유 ===== */
	{
		CUISequence::UISEQUENCE_DESC tCursorDesc{};
		tCursorDesc.strPath = "../../DataFiles/UI/UI_Cursor.uiseq";
		tCursorDesc.iProtoLevel = ETOUI(LEVEL::STATIC);

		CUISequence* pCursorSeq = static_cast<CUISequence*>(m_pGameInstance->Clone_Prototype(
			PROTOTYPE::GAMEOBJECT, ETOUI(LEVEL::STATIC), PROTO_UI_SEQUENCE, &tCursorDesc));
		if (nullptr == pCursorSeq)
			return E_FAIL;

		if (FAILED(m_pGameInstance->Add_GameObject_Ex(CURRENT_LEVEL, strLayerTag, pCursorSeq)))
		{
			Safe_Release(pCursorSeq);
			return E_FAIL;
		}

		UI_Set_Cursor_Sequence(pCursorSeq);  // Hub 에 weak 주입
		m_pCursorSeq = pCursorSeq;           // 레벨도 weak (Add_GameObject_Ex 가 owner)
	}

	/* ===== 메뉴 컨트롤러 테스트 등록 ===== */
	CMenu* pMenu = CMenu::Create();
	if (nullptr == pMenu)
		return E_FAIL;

	if (FAILED(pMenu->Initialize(pSeq)))
	{
		Safe_Release(pMenu);
		return E_FAIL;
	}

	/* 활성화 콜백 — 어떤 항목이 선택됐는지 OutputDebugString 으로 확인 */
	pMenu->Set_OnActivate([](_int iIndex)
		{
			static constexpr const _char* s_Names[] =
			{
					"PARTNER", "DEX", "BAG", "ENTRY", "LINK", "REPORT"
			};
			if (iIndex >= 0 && iIndex < static_cast<_int>(CMenu::MENU_ENTRY::END))
			{
				OutputDebugStringA("[Menu] Activated: ");
				OutputDebugStringA(s_Names[iIndex]);
				OutputDebugStringA("\n");
			}
			else
			{
				OutputDebugStringA("[Menu] Activated: index out of range\n");
			}
		});

	/* 취소 콜백 — 베이스가 콜백 후 Close() 자동 호출 */
	pMenu->Set_OnCancel([]()
		{
			OutputDebugStringA("[Menu] Cancelled\n");
		});

	/* Hub 등록 — 내부 AddRef. 이후 local 레퍼런스 해제. */
	if (FAILED(UI_Register(pMenu)))
	{
		Safe_Release(pMenu);
		return E_FAIL;
	}

	m_pMenu = pMenu;        // weak — Hub 가 owner
	Safe_Release(pMenu);    // local ref-- (Hub 가 ref 보유 중이라 안전)

	return S_OK;
}

CLevel_GamePlay* CLevel_GamePlay::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_GamePlay* pInstance = new CLevel_GamePlay(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_GamePlay");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_GamePlay::Free()
{
	/* Hub 의 cursor weak 를 UI_Close_All 보다 먼저 끊어 안전성 확보.
	   (Hub::Update_Cursor 가 다음 프레임에 호출되더라도 m_pCursor == nullptr 분기로 빠짐.) */
	UI_Set_Cursor_Sequence(nullptr);
	UI_Close_All();

	m_pCursorSeq = nullptr;
	m_pFadeBattleSeq = nullptr;
	m_pMenu = nullptr;
	m_pRuntimeUI = nullptr;

	__super::Free();
}
