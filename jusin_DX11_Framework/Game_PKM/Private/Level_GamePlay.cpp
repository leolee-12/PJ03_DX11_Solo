#include "Level_GamePlay.h"
#include "Camera_Free.h"
#include "Player_LGPE.h"
#include "UIButton_Glow.h"
#include "UIButton_Layered.h"
#include "UIButton_Group.h"
#include "Menu.h"
#include "BattleMsg.h"
#include "Game_API.h"
#include "Level_Battle.h"
#include "Level_Capture.h"
#include "Game_LevelEntry.h"
#include "Battle_Session.h"
#include "Body_Human.h"
#include "Body_Pokemon.h"
#include "Actor_NPC.h"
#include "Actor_WildPokemon.h"
#include "PokemonData_Manager.h"
#include "Spawn_Manager.h"
#include "Event_Manager.h"
#include "Region_Manager.h"
#include "Player_Status.h"
#include "Entry.h"
#include "MapObject.h"
#include "WaterPlane.h"
#include "FieldGrassBatch.h"
#include "Battle_Trainer.h"

#include "GameInstance.h"
#include "UISequence.h"
#include "UIText.h"

NS_BEGIN(Game_PKM)
static constexpr _uint CURRENT_LEVEL = ETOUI(LEVEL::GAMEPLAY);

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

namespace
{
	_bool Resolve_GamePlayDialogueText(const _wstring& strKey, _wstring& strOut)
	{
		struct DIALOGUE_TEXT
		{
			const wchar_t* pKey = nullptr;
			const wchar_t* pText = nullptr;
		};

		static constexpr DIALOGUE_TEXT DialogueTexts[] =
		{
				{ L"dialogue_npc_doctor", L"많은 곳을 탐험하여 모든 포켓몬을 도감에 기록해보거라.\f마을마다 있는 체육관에 도전해보는 것은 어떻겠느냐 ? " },
				{ L"dialogue_npc_juveniles", L"나는 포켓몬을 키우고 있어.\f풀숲에서 튀어나오는 포켓몬이나, 승부를 걸어오는 트레이너로부터 나를 지켜주거든." },
				{ L"dialogue_npc_fat", L"과학의 힘이란 대단해!" },
				{ L"dialogue_npc_nurse", L"안녕하세요. 포켓몬을 회복시켜 드리겠습니다." },

				{ L"dialogue_trainer_shortpants", L"포켓몬 트레이너끼리\n눈과 눈이 마주쳤다!\f내가 좋아하는 포켓몬과 승부하자!" },
				{ L"dialogue_trainer_rock", L"왔구나!\n나는 회색시티 포켓몬체육관의 관장인 웅이야!\f나의 굳은 의지는 내 포켓몬에게서도 드러나지!\n단단하고 참을성이 강해.\f좋아!\n자 덤벼라!" },
				{ L"dialogue_trainer_water", L"너!\f너는 포켓몬을 키울 때\n너만의 방침이 있니?\f나의 방침은 말이지...\n물타입 포켓몬으로 공격하고 ...또 공격하는거야!\f자! 세계의 미소녀\n이슬님이 상대해줄게!\f가라!\n내 귀염둥이!" },

				{ L"dialogue_pokemon_pm0001_00", L"이상해씨가 조용히 햇빛을 받고 있다." },
				{ L"dialogue_pokemon_pm0004_00", L"파이리가 꼬리의 불꽃을 살랑이고 있다." },
				{ L"dialogue_pokemon_pm0007_00", L"꼬부기가 이쪽을 빤히 바라본다." },
				{ L"dialogue_pokemon_pm0010_00", L"캐터피가 작은 몸을 꿈틀거린다." },
				{ L"dialogue_pokemon_pm0025_00", L"피카츄가 볼을 반짝이며 울었다." },
				{ L"dialogue_pokemon_pm0041_00", L"주뱃이 날개를 퍼덕이고 있다." },
				{ L"dialogue_pokemon_pm0043_00", L"뚜벅쵸가 잎사귀를 흔들고 있다." },
				{ L"dialogue_pokemon_pm0059_00", L"윈디가 늠름하게 자리를 지키고 있다." },
				{ L"dialogue_pokemon_pm0074_00", L"꼬마돌이 단단한 몸을 굴릴 준비를 한다." },
				{ L"dialogue_pokemon_pm0095_00", L"롱스톤이 거대한 몸을 낮게 웅크리고 있다." },
				{ L"dialogue_pokemon_pm0121_00", L"아쿠스타의 보석이 은은하게 빛난다." },
				{ L"dialogue_pokemon_pm0130_00", L"갸라도스가 위압적으로 포효한다." },
		};

		for (const DIALOGUE_TEXT& DialogueText : DialogueTexts)
		{
				if (strKey == DialogueText.pKey)
				{
						strOut = DialogueText.pText;
						return true;
				}
		}

		return false;
	}

	void Build_DialoguePages(const _wstring& strMessage, vector<_wstring>& Pages)
	{
		Pages.clear();

		_wstring strPage;
		for (wchar_t ch : strMessage)
		{
			if (L'\f' == ch)
			{
				if (false == strPage.empty())
					Pages.push_back(strPage);

				strPage.clear();
				continue;
			}

			strPage.push_back(ch);
		}

		if (false == strPage.empty())
			Pages.push_back(strPage);
	}
}

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

	if (FAILED(Ready_EventSystem()))
		return E_FAIL;

	if (FAILED(Ready_RegionSystem()))
		return E_FAIL;

	if (FAILED(Ready_MainCamera()))
		return E_FAIL;

	if (FAILED(Ready_Cloud()))
		return E_FAIL;

	return S_OK;
}

void CLevel_GamePlay::Update(_float fTimeDelta)
{
#ifdef _DEBUG
	Debug_TickFPS(fTimeDelta);
#endif

	if (Tick_Dialogue(fTimeDelta))
		return;

	if (Tick_Event(fTimeDelta))
		return;

	if (Tick_Transition(fTimeDelta))
		return;

	Tick_Gameplay(fTimeDelta);
}

HRESULT CLevel_GamePlay::Render()
{
#ifdef _DEBUG
	SetWindowText(m_pGameInstance->Get_HWND(), TEXT("게임플레이레벨입니다."));

	if (CSpawn_Manager* pSpawnMgr = CSpawn_Manager::GetInstance())
		pSpawnMgr->Render_Debug();

	Debug_RenderFPS();
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

	/* GAMEPLAY BGM 복원. 배틀/캡처 진입 전 지역 BGM 으로 되돌린다. */
	if (nullptr != m_pRegionMgr)
		m_pRegionMgr->Play_Current_BGM();

	/* Capture/Battle 종료 시 UI_Set_Cursor_Sequence(nullptr) 로 Hub 커서가 해제됨 → GamePlay 커서 재주입. */
	if (nullptr != m_pCursorSeq)
		UI_Set_Cursor_Sequence(m_pCursorSeq);

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

_bool CLevel_GamePlay::Request_Battle(const BATTLE_ENV& tEnv)
{
	/* 트랜지션 BUSY 중에는 재진입 차단. Capture_Manager::Request_Capture 의 가드 순서와 일관. */
	if (TRANSITION_STATE::IDLE != m_eTransition)
		return false;

	/* 대화 활성 중에는 배틀 진입 금지. M1 Start_Dialogue 와의 충돌 방지. */
	if (true == m_bDialogueActive)
		return false;

	/* UI 가 하나라도 열려 있으면 무시. 대화 UI 가 닫힌 다음 프레임 이후 호출되어야 함. */
	if (true == UI_Is_AnyOpen())
		return false;

	/* Fade 시퀀스 미보유면 진입 불가. Ready_Layer_UI 에서 등록되지 못한 경우 방어. */
	if (nullptr == m_pFadeBattleSeq)
		return false;

	m_PendingEntryDesc.Clear();
	m_PendingEntryDesc.eNextLevelID = LEVEL::BATTLE;

	if (FAILED(m_PendingEntryDesc.Set_Payload(LEVEL_ENTRY_PAYLOAD::BATTLE_ENV, &tEnv,
		sizeof(BATTLE_ENV))))
		return false;

	m_pGameInstance->Play_BGM(L"BGM/1-24. Battle! (Gym Leader).mp3", 0.3f);

	m_pFadeBattleSeq->Set_Visible(true);
	m_pFadeBattleSeq->Play();

	m_pGameInstance->Set_InputState(INPUT_STATE::LOCKED);

	m_eTransition = TRANSITION_STATE::BUSY;
	m_fTransitionElapsed = 0.f;

	return true;
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

	/* 트랜지션이 실제 시작될 때만 삭제 대상 보관 - 가드 통과 후로 두어
	   UI 열린 상태 등에서 무시된 호출은 WildPokemon 도 그대로 두는 정합성 확보. */
	m_pPendingDeleteWild = pTarget;   // weak

	m_pGameInstance->Play_BGM(L"BGM/1-18. Catch! (Wild Pokemon).mp3", 0.3f);

	/* Fade 시퀀스는 BATTLE 과 공유. CAPTURE 전용 Fade·BGM 분기는 후속에서 결정. */
	m_pFadeBattleSeq->Set_Visible(true);
	m_pFadeBattleSeq->Play();

	/* 트랜지션 동안 게임 객체 수준 입력 차단. */
	m_pGameInstance->Set_InputState(INPUT_STATE::LOCKED);

	m_eTransition = TRANSITION_STATE::BUSY;
	m_fTransitionElapsed = 0.f;
}

_bool CLevel_GamePlay::Start_Dialogue(const _wstring& strDialogueKey)
{
	_wstring strMessage;
	if (false == Resolve_GamePlayDialogueText(strDialogueKey, strMessage))
		return false;

	if (false == Start_Dialogue_Text(strMessage))
		return false;

	m_strActiveDialogueKey = strDialogueKey;
	return true;
}

_bool CLevel_GamePlay::Start_Dialogue_Text(const _wstring& strMessage)
{
	if (true == m_bDialogueActive)
		return false;

	if (nullptr == m_pDialogueMsg)
		return false;

	if (true == strMessage.empty())
		return false;

	Build_DialoguePages(strMessage, m_DialoguePages);
	if (true == m_DialoguePages.empty())
		return false;

	m_iDialoguePageIndex = 0;
	m_strActiveDialogueKey.clear();

	m_pDialogueMsg->Set_Message(m_DialoguePages[m_iDialoguePageIndex]);
	m_pDialogueMsg->Open(true);

	m_pGameInstance->Set_InputState(INPUT_STATE::MENU);
	m_bDialogueActive = true;

	return true;
}

_bool CLevel_GamePlay::Is_Dialogue_Playing() const
{
	return m_bDialogueActive;
}

_bool CLevel_GamePlay::Is_Dialogue_Done() const
{
	return nullptr == m_pDialogueMsg ? true : m_pDialogueMsg->Is_Done();
}

_bool CLevel_GamePlay::Is_Event_Playing() const
{
	return nullptr != m_pEventMgr && true == m_pEventMgr->Is_Playing();
}

void CLevel_GamePlay::Close_Dialogue()
{
	if (nullptr != m_pDialogueMsg)
		m_pDialogueMsg->Close();

	m_bDialogueActive = false;
	m_strActiveDialogueKey.clear();
	m_DialoguePages.clear();
	m_iDialoguePageIndex = 0;

	if (nullptr != m_pEventMgr && true == m_pEventMgr->Is_Playing())
		m_pGameInstance->Set_InputState(INPUT_STATE::LOCKED);
	else
		m_pGameInstance->Set_InputState(INPUT_STATE::GAMEPLAY);
}

HRESULT CLevel_GamePlay::Ready_Lights()
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

	CWaterPlane::WATER_PLANE_DESC WaterDesc{};
	WaterDesc.vSpawnPos = _float3(15.f, -0.5f, -25.f);
	WaterDesc.fWidth = 260.f;
	WaterDesc.fDepth = 260.f;
	WaterDesc.fScale = 0.1f;
	WaterDesc.fTileU = 12.f;
	WaterDesc.fTileV = 12.f;
	WaterDesc.fTimeScale = 0.18f;

	if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::GAMEPLAY), PROTO_OBJ_WATERPLANE,
		ETOUI(LEVEL::GAMEPLAY), strLayerTag, &WaterDesc)))
		return E_FAIL;

	vector<_float3> GrassPositions;
	GrassPositions.reserve(512);

	auto AppendGrassGrid = [&GrassPositions](_float fStartX, _float fY, _float fStartZ,
											_uint iCountX, _uint iCountZ, _float fStepX, _float fStepZ)
		{
			for (_uint z = 0; z < iCountZ; ++z)
			{
				for (_uint x = 0; x < iCountX; ++x)
				{
					GrassPositions.emplace_back(
						fStartX + static_cast<_float>(x) * fStepX,
						fY,
						fStartZ + static_cast<_float>(z) * fStepZ);
				}
			}
		};

	AppendGrassGrid(19.2f, 0.0f, -0.6f, 6, 8, 0.5f, 0.5f);
	AppendGrassGrid(22.5f, 0.0f, 7.3f, 20, 8, 0.5f, -0.5f);
	AppendGrassGrid(14.5f, 0.0f, 7.3f, 8, 8, 0.5f, -0.5f);
	AppendGrassGrid(22.2f, 1.0f, 17.5f, 8, 8, 0.5f, -0.5f);
	AppendGrassGrid(22.0f, 2.5f, 31.5f, 12, 8, 0.5f, -0.5f);

	CFieldGrassBatch::FIELDGRASS_BATCH_DESC GrassBatchDesc{};
	GrassBatchDesc.pPositions = GrassPositions.data();
	GrassBatchDesc.iNumPositions = static_cast<_uint>(GrassPositions.size());

	if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::GAMEPLAY), PROTO_OBJ_FIELD_GRASS_BATCH,
		ETOUI(LEVEL::GAMEPLAY), strLayerTag, &GrassBatchDesc)))
		return E_FAIL;

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
	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_NPC(WNameID strLayerTag)
{
	(void)strLayerTag;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Wild(WNameID strLayerTag)
{
	(void)strLayerTag;

	CSpawn_Manager* pSpawnMgr = CSpawn_Manager::GetInstance();
	if (nullptr == pSpawnMgr) return E_FAIL;

	if (FAILED(pSpawnMgr->Initialize(ETOUI(LEVEL::STATIC), PROTO_COM_NAVIGATION_MAP)))
		return E_FAIL;

	if (FAILED(pSpawnMgr->Load_From_File(TEXT("../../DataFiles/Spawn_GamePlay.spawn"))))
		return E_FAIL;

	if (FAILED(pSpawnMgr->Begin())) return E_FAIL;

	return S_OK;
}


HRESULT CLevel_GamePlay::Ready_Layer_Effect(WNameID strLayerTag)
{
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

	/* Fade */
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

	/* Region Banner */
	{
		CUISequence::UISEQUENCE_DESC tRegionDesc{};
		tRegionDesc.strPath = "../../DataFiles/UI/UI_Region.uiseq";
		tRegionDesc.iProtoLevel = ETOUI(LEVEL::STATIC);

		CUISequence* pRegionSeq = static_cast<CUISequence*>(m_pGameInstance->Clone_Prototype(
			PROTOTYPE::GAMEOBJECT, ETOUI(LEVEL::STATIC), PROTO_UI_SEQUENCE, &tRegionDesc));
		if (nullptr == pRegionSeq)
			return E_FAIL;

		CUIText* pRegionNameText = dynamic_cast<CUIText*>(pRegionSeq->Find_Widget("widget_002"));
		if (nullptr == pRegionNameText)
		{
			Safe_Release(pRegionSeq);
			return E_FAIL;
		}

		if (FAILED(m_pGameInstance->Add_GameObject_Ex(CURRENT_LEVEL, strLayerTag, pRegionSeq)))
		{
			Safe_Release(pRegionSeq);
			return E_FAIL;
		}

		pRegionSeq->Set_Visible(false);
		m_pRegionSeq = pRegionSeq;                 // weak
		m_pRegionNameText = pRegionNameText;       // weak - RegionSeq owns widget
	}

	/* MsgBox */
	{
		CUISequence::UISEQUENCE_DESC tDialogueDesc{};
		tDialogueDesc.strPath = "../../DataFiles/UI/UI_BattleMsg.uiseq";
		tDialogueDesc.iProtoLevel = ETOUI(LEVEL::STATIC);

		CUISequence* pDialogueSeq = static_cast<CUISequence*>(m_pGameInstance->Clone_Prototype(
			PROTOTYPE::GAMEOBJECT, ETOUI(LEVEL::STATIC), PROTO_UI_SEQUENCE, &tDialogueDesc));
		if (nullptr == pDialogueSeq)
			return E_FAIL;

		if (FAILED(m_pGameInstance->Add_GameObject_Ex(CURRENT_LEVEL, strLayerTag, pDialogueSeq)))
		{
			Safe_Release(pDialogueSeq);
			return E_FAIL;
		}

		CBattleMsg* pDialogueMsg = CBattleMsg::Create();
		if (nullptr == pDialogueMsg)
			return E_FAIL;

		if (FAILED(pDialogueMsg->Initialize(pDialogueSeq)))
		{
			Safe_Release(pDialogueMsg);
			return E_FAIL;
		}

		pDialogueMsg->Close();

		if (FAILED(UI_Register(pDialogueMsg, ETOUI(LEVEL::GAMEPLAY))))
		{
			Safe_Release(pDialogueMsg);
			return E_FAIL;
		}

		m_pDialogueSeq = pDialogueSeq;
		m_pDialogueMsg = pDialogueMsg;      // weak - Hub owns
		Safe_Release(pDialogueMsg);         // local ref--
	}

	/* ===== 커서 시퀀스 - Hub 가 단일 인스턴스로 공유 ===== */
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

	/* 활성화 콜백 - 어떤 항목이 선택됐는지 OutputDebugString 으로 확인 */
	pMenu->Set_OnActivate([this](_int iIndex)
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

			if (iIndex == static_cast<_int>(CMenu::MENU_ENTRY::ENTRY))
			{
				if (m_pMenu)
					m_pMenu->Close();
				if (m_pEntry)
					m_pEntry->Open();
			}
		});

	/* 취소 콜백 - 베이스가 콜백 후 Close() 자동 호출 */
	pMenu->Set_OnCancel([]()
		{
			OutputDebugStringA("[Menu] Cancelled\n");
		});

	/* Hub 등록 - 내부 AddRef. 이후 local 레퍼런스 해제. */
	if (FAILED(UI_Register(pMenu, ETOUI(LEVEL::GAMEPLAY))))
	{
		Safe_Release(pMenu);
		return E_FAIL;
	}

	m_pMenu = pMenu;        // weak - Hub 가 owner
	Safe_Release(pMenu);    // local ref-- (Hub 가 ref 보유 중이라 안전)

	/* ===== Entry 컨트롤러 등록 ===== */
	{
		CUISequence::UISEQUENCE_DESC tEntryDesc{};
		tEntryDesc.strPath = "../../DataFiles/UI/UI_Entry.uiseq";
		tEntryDesc.iProtoLevel = ETOUI(LEVEL::STATIC);

		CUISequence* pEntrySeq = static_cast<CUISequence*>(m_pGameInstance->Clone_Prototype(
			PROTOTYPE::GAMEOBJECT, ETOUI(LEVEL::STATIC), PROTO_UI_SEQUENCE, &tEntryDesc));
		if (nullptr == pEntrySeq)
			return E_FAIL;

		if (FAILED(m_pGameInstance->Add_GameObject_Ex(CURRENT_LEVEL, strLayerTag, pEntrySeq)))
		{
			Safe_Release(pEntrySeq);
			return E_FAIL;
		}

		pEntrySeq->Set_Visible(false);   // Open 호출 전까지 숨김
		m_pEntrySeq = pEntrySeq;         // weak (Add_GameObject_Ex 가 owner)

		CEntry* pEntry = CEntry::Create();
		if (nullptr == pEntry)
			return E_FAIL;

		if (FAILED(pEntry->Initialize(pEntrySeq)))
		{
			Safe_Release(pEntry);
			return E_FAIL;
		}

		pEntry->Bind(Find_PlayerState(m_pGameInstance));

		pEntry->Set_OnCancel([this]()
			{
				if (m_pMenu)
					m_pMenu->Open();
			});

		if (FAILED(UI_Register(pEntry, ETOUI(LEVEL::GAMEPLAY))))
		{
			Safe_Release(pEntry);
			return E_FAIL;
		}

		m_pEntry = pEntry;          // weak - Hub 가 owner
		Safe_Release(pEntry);       // local ref--
	}

	return S_OK;
  }

  HRESULT CLevel_GamePlay::Ready_MainCamera()
  {
	  const list<CGameObject*>* pCameraList =
		  m_pGameInstance->Get_ObjectList(ETOUI(LEVEL::GAMEPLAY), LAYER_CAMERA);
	  const list<CGameObject*>* pPlayerList =
		  m_pGameInstance->Get_ObjectList(ETOUI(LEVEL::GAMEPLAY), LAYER_PLAYER);

	  if (nullptr == pCameraList || true == pCameraList->empty())
		  return E_FAIL;

	  if (nullptr == pPlayerList || true == pPlayerList->empty())
		  return E_FAIL;

	  CCamera* pCamera = static_cast<CCamera*>(pCameraList->front());
	  CPlayer_LGPE* pPlayer = static_cast<CPlayer_LGPE*>(pPlayerList->front());

	  pCamera->Set_FollowTarget(pPlayer->Get_Transform());
	  pCamera->Set_FollowOffset({ 0.f, 6.5f, -7.5f });
	  m_pGameInstance->Set_MainCamera(pCamera);
	  pCamera->Set_Following(true);
	  pCamera->Set_ControlEnabled(true);

	  return S_OK;
  }

  HRESULT CLevel_GamePlay::Ready_Cloud()
  {
	  m_pCloudTexture = static_cast<CTexture*>(
		  m_pGameInstance->Clone_Prototype(
			  PROTOTYPE::COMPONENT,
			  ETOUI(LEVEL::GAMEPLAY),
			  PROTO_COM_TEX_CLOUD));

	  if (nullptr == m_pCloudTexture)
		  return E_FAIL;

	  m_CloudParam.bEnable = true;
	  m_CloudParam.fStrength = 0.7f;
	  m_CloudParam.fTiling = 0.025f;
	  m_CloudParam.fSpeed = 0.015f;
	  m_CloudParam.vScrollDir = XMFLOAT2(-1.f, 1.f);
	  m_CloudParam.fCoverageLow = 0.00f;
	  m_CloudParam.fCoverageHigh = 0.40f;
	  m_CloudParam.fDarkness = 0.72f;
	  m_CloudParam.fTime = 0.f;

	  m_pGameInstance->Set_DecalParam(m_CloudParam);
	  m_pGameInstance->Set_DecalTexture(m_pCloudTexture, 0);

	  return S_OK;
  }

HRESULT CLevel_GamePlay::Ready_EventSystem()
{
	m_pEventMgr = CEvent_Manager::Create(this);
	if (nullptr == m_pEventMgr)
		return E_FAIL;

	if (FAILED(m_pEventMgr->Load_From_File(TEXT("../../DataFiles/Event/Event_GamePlay.event"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_RegionSystem()
{
	m_pRegionMgr = CRegion_Manager::Create();
	if (nullptr == m_pRegionMgr)
		return E_FAIL;

	// 좌표/크기/BGM 키는 실제 맵 배치에 맞춰 채운다. (아래는 형식 예시)
	{
		REGION_RECT_DESC tRegion{};
		tRegion.iRegionID = 1;
		wcscpy_s(tRegion.szRegionName, L"태초 마을");
		wcscpy_s(tRegion.szBGM, L"BGM/1-04. Pallet Town Theme.mp3");
		tRegion.fBGMVolume = 0.3f;
		tRegion.vCenter = { 0.f, 0.f, -25.f };
		tRegion.vSize = { 50.f, 50.f };
		tRegion.fRotationY = 0.f;
		tRegion.iPriority = 0;
		if (FAILED(m_pRegionMgr->Register_Region(tRegion)))
			return E_FAIL;
	}

	{
		REGION_RECT_DESC tRegion{};
		tRegion.iRegionID = 2;
		wcscpy_s(tRegion.szRegionName, L"1번 도로");
		wcscpy_s(tRegion.szBGM, L"BGM/1-09. Road to Viridian City - From Pallet Town.mp3");
		tRegion.fBGMVolume = 0.3f;
		tRegion.vCenter = { 0.f, 0.f, 20.f };
		tRegion.vSize = { 50.f, 40.f };
		tRegion.fRotationY = 0.f;
		tRegion.iPriority = 0;
		if (FAILED(m_pRegionMgr->Register_Region(tRegion)))
			return E_FAIL;
	}

	{
		REGION_RECT_DESC tRegion{};
		tRegion.iRegionID = 3;
		wcscpy_s(tRegion.szRegionName, L"회색 시티");
		wcscpy_s(tRegion.szBGM, L"BGM/1-22. Pewter City Theme.mp3");
		tRegion.fBGMVolume = 0.3f;
		tRegion.vCenter = { 0.f, 0.f, 65.f };
		tRegion.vSize = { 50.f, 50.f };
		tRegion.fRotationY = 0.f;
		tRegion.iPriority = 0;
		if (FAILED(m_pRegionMgr->Register_Region(tRegion)))
			return E_FAIL;
	}

	// 추가 지역은 같은 형식으로 Register_Region 반복 (iRegionID 는 1 이상 고유값).

	m_pRegionMgr->Set_OnRegionChanged(
		[this](const REGION_RECT_DESC* pPrevRegion, const REGION_RECT_DESC& tNewRegion)
		{
			(void)pPrevRegion;

			if (nullptr == m_pRegionSeq || nullptr == m_pRegionNameText)
				return;

			m_pRegionNameText->Set_Text(tNewRegion.szRegionName);
			m_pRegionSeq->Set_Visible(true);
			m_pRegionSeq->Play();
		});

	m_pRegionMgr->Resolve_Initial();   // 초기 지역 확정 + BGM (배너 없음)
	return S_OK;
}

_bool CLevel_GamePlay::Tick_Dialogue(_float fTimeDelta)
{
	if (false == m_bDialogueActive)
		return false;

	UI_Update_All(fTimeDelta);

	if (m_pGameInstance->Key_Down(DIK_RETURN) ||
		m_pGameInstance->Key_Down(DIK_SPACE))
	{
		if (nullptr == m_pDialogueMsg)
		{
			Close_Dialogue();
			return true;
		}

		if (false == m_pDialogueMsg->Is_Done())
		{
			m_pDialogueMsg->Complete();
		}
		else
		{
			if (m_iDialoguePageIndex + 1 < m_DialoguePages.size())
			{
				++m_iDialoguePageIndex;
				m_pDialogueMsg->Set_Message(m_DialoguePages[m_iDialoguePageIndex]);
			}
			else
			{
				Close_Dialogue();
			}
		}
	}

	return true;
}

_bool CLevel_GamePlay::Tick_Event(_float fTimeDelta)
{
	if (nullptr == m_pEventMgr || false == m_pEventMgr->Is_Playing())
		return false;

	m_pEventMgr->Update(fTimeDelta);
	UI_Update_All(fTimeDelta);

	return true;
}

_bool CLevel_GamePlay::Tick_Transition(_float fTimeDelta)
{
	if (TRANSITION_STATE::BUSY != m_eTransition)
		return false;

	m_fTransitionElapsed += fTimeDelta;

	if (m_fTransitionElapsed < TRANSITION_PUSH_AT_SEC)
	{
		UI_Update_All(fTimeDelta);
		return true;
	}

	if (nullptr != m_pFadeBattleSeq)
		m_pFadeBattleSeq->Set_Visible(false);

	if (LEVEL::CAPTURE == m_PendingEntryDesc.eNextLevelID)
	{
		CLevel_Capture* pCapture = CLevel_Capture::Create(m_pDevice, m_pContext, &m_PendingEntryDesc);
		if (nullptr == pCapture)
		{
			m_pGameInstance->Set_InputState(INPUT_STATE::GAMEPLAY);
			Reset_Transition();
			return true;
		}

		if (nullptr != m_pPendingDeleteWild)
		{
			m_pPendingDeleteWild->Set_Dead();
			m_pPendingDeleteWild = nullptr;
		}

		if (FAILED(m_pGameInstance->Push_Level(ETOI(LEVEL::CAPTURE), pCapture)))
		{
			Safe_Release(pCapture);
			m_pGameInstance->Set_InputState(INPUT_STATE::GAMEPLAY);
			Reset_Transition();
			return true;
		}

		m_pGameInstance->Set_InputState(INPUT_STATE::GAMEPLAY);
		Reset_Transition();
		return true;
	}

	CLevel_Battle* pBattle = CLevel_Battle::Create(m_pDevice, m_pContext, &m_PendingEntryDesc);
	if (nullptr == pBattle)
	{
		m_pGameInstance->Set_InputState(INPUT_STATE::GAMEPLAY);
		Reset_Transition();
		return true;
	}

	if (FAILED(m_pGameInstance->Push_Level(ETOI(LEVEL::BATTLE), pBattle)))
	{
		Safe_Release(pBattle);
		m_pGameInstance->Set_InputState(INPUT_STATE::GAMEPLAY);
		Reset_Transition();
		return true;
	}

	m_pGameInstance->Set_InputState(INPUT_STATE::GAMEPLAY);
	Reset_Transition();
	return true;
}

void CLevel_GamePlay::Reset_Transition()
{
	m_eTransition = TRANSITION_STATE::IDLE;
	m_fTransitionElapsed = 0.f;
}

void CLevel_GamePlay::Tick_Gameplay(_float fTimeDelta)
{
	CSpawn_Manager::GetInstance()->Update(fTimeDelta);

	if (nullptr != m_pRegionMgr)
		m_pRegionMgr->Update();

	if (m_pGameInstance->Key_Down(DIK_TAB) && nullptr != m_pMenu)
	{
		if (m_pMenu->Is_Open())
			m_pMenu->Close();
		else if (!UI_Is_AnyOpen())
			m_pMenu->Open();
	}

#ifdef _DEBUG
	Debug_Common();
	//Debug_Outline();
	Debug_Event();
	//Debug_Culling();
	//Debug_Decal();
#endif

	m_CloudParam.fTime += fTimeDelta;
	m_pGameInstance->Set_DecalParam(m_CloudParam);
	UI_Update_All(fTimeDelta);
}



#ifdef _DEBUG
void CLevel_GamePlay::Debug_Common()
{
	if (m_pGameInstance->Key_Down(DIK_F2))
	{
		m_pGameInstance->Toggle_CameraFollow();
	}

	if (m_pGameInstance->Key_Down(DIK_F3))
	{
		m_pGameInstance->Toggle_Debug();
	}
}

void CLevel_GamePlay::Debug_Outline()
{
	if (m_pGameInstance->Key_Down(DIK_F6))
	{
		m_DebugOutlineParam.bEnable = true;
		m_DebugOutlineParam.iMode = (m_DebugOutlineParam.iMode + 1) % 6;
		m_pGameInstance->Set_OutlineParam(m_DebugOutlineParam);
	}

	if (m_pGameInstance->Key_Down(DIK_F7))
	{
		m_DebugOutlineParam.fStrength += 0.1f;
		if (m_DebugOutlineParam.fStrength > 1.0f)
			m_DebugOutlineParam.fStrength = 0.f;
		m_pGameInstance->Set_OutlineParam(m_DebugOutlineParam);
	}

	if (m_pGameInstance->Key_Down(DIK_F8))
	{
		m_DebugOutlineParam.fThresholdLow += 0.01f;
		if (m_DebugOutlineParam.fThresholdLow > 0.10f)
			m_DebugOutlineParam.fThresholdLow = 0.f;
		m_DebugOutlineParam.fThresholdHigh = m_DebugOutlineParam.fThresholdLow + 0.06f;
		m_pGameInstance->Set_OutlineParam(m_DebugOutlineParam);
	}
}

void CLevel_GamePlay::Debug_Event()
{
	if (m_pGameInstance->Key_Down(DIK_F6) && nullptr != m_pEventMgr)
	{
		EVENT_CONTEXT tContext{};
		tContext.pGameInstance = m_pGameInstance;
		tContext.pLevelGamePlay = this;
	
		m_pEventMgr->Start_Sequence(L"TestScene_NPC_CameraMove", tContext);
	}

	if (m_pGameInstance->Key_Down(DIK_F7))
	{
		BATTLE_ENV tEnv = {};
		tEnv.eEnvironment = ENVIRONMENT_TYPE::GRASS;
		tEnv.eRule = BATTLE_RULE::TRAINER_SINGLE;
		tEnv.iOpponentTrainerID = 1;
		tEnv.iBGResourceID = 0;
		tEnv.iZoneID = 0;
	
		Request_Battle(tEnv);
		return;
	}

	if (m_pGameInstance->Key_Down(DIK_F8))
	{
	}
}

void CLevel_GamePlay::Debug_Culling()
{
	if (m_pGameInstance->Key_Down(DIK_F6))
	{
		CMapObject::Debug_ToggleCulling();
	}

	if (m_pGameInstance->Key_Down(DIK_F7))
	{
		CMapObject::Debug_ToggleCullLog();
	}
}

void CLevel_GamePlay::Debug_Decal()
{
	if (m_pGameInstance->Key_Down(DIK_F6))
	{
		m_CloudParam.bEnable = !m_CloudParam.bEnable;
		m_pGameInstance->Set_DecalParam(m_CloudParam);

#ifdef _DEBUG
		OutputDebugStringA(m_CloudParam.bEnable ? "[CloudDecal] ON\n" : "[CloudDecal] OFF\n");
#endif
	}

	if (m_pGameInstance->Key_Down(DIK_F7))
	{

	}
}

void CLevel_GamePlay::Debug_TickFPS(_float fTimeDelta)
{
	m_fDebugFpsAccum += fTimeDelta;
	++m_iDebugFpsFrames;

	m_fDebugRendererMSAccum += m_pGameInstance->Get_DebugRendererMS();

	if (m_fDebugFpsAccum >= 0.5f)
	{
		const _uint iFrames = (0 == m_iDebugFpsFrames ? 1 : m_iDebugFpsFrames);

		m_fDebugFps = static_cast<_float>(iFrames) / m_fDebugFpsAccum;
		m_fDebugRendererMS = m_fDebugRendererMSAccum / iFrames;

		m_fDebugFpsAccum = 0.f;
		m_iDebugFpsFrames = 0;
		m_fDebugRendererMSAccum = 0.f;
	}
}

void CLevel_GamePlay::Debug_RenderFPS()
{
	wchar_t szText[80] = {};
	swprintf_s(
		szText,
		L"FPS %.1f | Render %.3f ms | Cull %ls | Log %ls",
		m_fDebugFps,
		m_fDebugRendererMS,
		CMapObject::Debug_IsCullingEnabled() ? L"ON" : L"OFF",
		CMapObject::Debug_IsCullLogEnabled() ? L"ON" : L"OFF");

	m_pGameInstance->Draw_Text(
		FONT_MALGUN,
		szText,
		_float2(128.f, 16.f),
		XMVectorSet(1.f, 0.f, 0.f, 1.f),
		0.f,
		_float2(0.f, 0.f),
		_float2(0.75f, 0.75f));
}
#endif

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
#ifdef _DEBUG
	OutputDebugStringA("[Event] Level_GamePlay::Free entered\n");
#endif

	if (nullptr != m_pEventMgr)
		m_pEventMgr->Cancel_ActiveSequence();

	if (nullptr != m_pGameInstance)
	{
#ifdef _DEBUG
		OutputDebugStringA(("[Event] Pre-Free InputState=" +
			std::to_string(static_cast<_int>(m_pGameInstance->Get_InputState())) + "\n").c_str());
#endif
		m_pGameInstance->Set_InputState(INPUT_STATE::GAMEPLAY);
	}

	Safe_Release(m_pEventMgr);
	Safe_Release(m_pRegionMgr);
	m_pGameInstance->Set_DecalTexture(nullptr, 0);
	m_pGameInstance->Set_DecalParam(DECAL_PARAM{});
	Safe_Release(m_pCloudTexture);
	/* Hub 의 cursor weak 를 UI_Close_All 보다 먼저 끊어 안전성 확보.
	   (Hub::Update_Cursor 가 다음 프레임에 호출되더라도 m_pCursor == nullptr 분기로 빠짐.) */
	UI_Set_Cursor_Sequence(nullptr);
	UI_Close_All();

	m_pDialogueMsg = nullptr;
	m_pDialogueSeq = nullptr;
	m_bDialogueActive = false;
	m_strActiveDialogueKey.clear();

	m_pCursorSeq = nullptr;
	m_pFadeBattleSeq = nullptr;
	m_pMenu = nullptr;
	m_pEntry = nullptr;
	m_pEntrySeq = nullptr;
	m_pRegionSeq = nullptr;
	m_pRegionNameText = nullptr;

	__super::Free();
}
