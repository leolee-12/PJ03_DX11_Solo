#include "Level_Battle.h"
#include "Battle_Manager.h"
#include "Player_Status.h"
#include "PokemonData_Manager.h"
#include "TrainerData_Manager.h"
#include "Battle_Pokemon.h"
#include "Battle_Trainer.h"
#include "BattleMsg.h"
#include "Battle_MsgListener.h"
#include "Battle_EventDispatcher.h"
#include "BattlePlate.h"
#include "Battle_CommandMenu.h"
#include "Battle_MoveMenu.h"
#include "Battle_InputDirector.h"
#include "Battle_PokemonListener.h"
#include "Battle_ExpGainListener.h"
#include "Battle_PlateListener.h"
#include "Game_API.h"
#include "Camera_Free.h"
#include "Camera_Director.h"

#include "GameInstance.h"
#include "UISequence.h"

NS_BEGIN(Game_PKM)
static constexpr _uint CURRENT_LEVEL = ETOUI(LEVEL::BATTLE);

namespace
{
	const _tchar* Get_BattlePhaseName(BATTLE_PHASE ePhase)
	{
		switch (ePhase)
		{
		case BATTLE_PHASE::INTRO: return TEXT("INTRO");
		case BATTLE_PHASE::INPUT_PLAYER: return TEXT("INPUT_PLAYER");
		case BATTLE_PHASE::INPUT_OPPONENT: return TEXT("INPUT_OPPONENT");
		case BATTLE_PHASE::RESOLVE_ORDER: return TEXT("RESOLVE_ORDER");
		case BATTLE_PHASE::RESOLVE_ACTION_1: return TEXT("RESOLVE_ACTION_1");
		case BATTLE_PHASE::RESOLVE_ACTION_2: return TEXT("RESOLVE_ACTION_2");
		case BATTLE_PHASE::RESOLVE_END_TURN: return TEXT("RESOLVE_END_TURN");
		case BATTLE_PHASE::CHECK_END: return TEXT("CHECK_END");
		case BATTLE_PHASE::FORCED_SWITCH: return TEXT("FORCED_SWITCH");
		case BATTLE_PHASE::OUTRO: return TEXT("OUTRO");
		case BATTLE_PHASE::DONE: return TEXT("DONE");
		default: return TEXT("UNKNOWN");
		}
	}

	const _tchar* Get_CameraModeName(CAMERA_MODE eMode)
	{
		switch (eMode)
		{
		case CAMERA_MODE::FIELD: return TEXT("FIELD");
		case CAMERA_MODE::BATTLE_DEFAULT: return TEXT("BATTLE_DEFAULT");
		case CAMERA_MODE::CINEMATIC: return TEXT("CINEMATIC");
		case CAMERA_MODE::DEBUG_FREE: return TEXT("DEBUG_FREE");
		default: return TEXT("UNKNOWN");
		}
	}

	const _tchar* Get_CameraShotTypeName(CAMERA_SHOT_TYPE eType)
	{
		switch (eType)
		{
		case CAMERA_SHOT_TYPE::CUT: return TEXT("CUT");
		case CAMERA_SHOT_TYPE::BLEND_TO: return TEXT("BLEND_TO");
		case CAMERA_SHOT_TYPE::FOLLOW_LOOKAT: return TEXT("FOLLOW_LOOKAT");
		case CAMERA_SHOT_TYPE::ORBIT: return TEXT("ORBIT");
		case CAMERA_SHOT_TYPE::DOLLY: return TEXT("DOLLY");
		case CAMERA_SHOT_TYPE::SHAKE_ONLY: return TEXT("SHAKE_ONLY");
		case CAMERA_SHOT_TYPE::RETURN_DEFAULT: return TEXT("RETURN_DEFAULT");
		default: return TEXT("NONE");
		}
	}

	const _tchar* Get_CameraSequenceName(CAMERA_SEQUENCE_ID eID)
	{
		switch (eID)
		{
		case CAMERA_SEQUENCE_ID::TACKLE_PHYSICAL: return TEXT("TACKLE_PHYSICAL");
		case CAMERA_SEQUENCE_ID::RANGED_ENERGY: return TEXT("RANGED_ENERGY");
		case CAMERA_SEQUENCE_ID::AREA_WIDE: return TEXT("AREA_WIDE");
		case CAMERA_SEQUENCE_ID::HIT_ONLY: return TEXT("HIT_ONLY");
		case CAMERA_SEQUENCE_ID::BUFF_SELF: return TEXT("BUFF_SELF");
		default: return TEXT("NONE");
		}
	}
}
NS_END

CLevel_Battle::CLevel_Battle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const BATTLE_ENV& tEnv)
	: CLevel{ pDevice, pContext }
	, m_tEnv{ tEnv }
{
}

HRESULT CLevel_Battle::Initialize()
{
	m_pBattleManager = CBattle_Manager::Create(m_tEnv);
	if (nullptr == m_pBattleManager)
		return E_FAIL;

	if (FAILED(Bind_Battle_Sources()))
		return E_FAIL;

	if (FAILED(Ready_Lights()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera(LAYER_CAMERA)))
		return E_FAIL;

	if (FAILED(Ready_Layer_BackGround(LAYER_BACKGROUND)))
		return E_FAIL;

	if (FAILED(Ready_Layer_Battler(LAYER_MONSTER)))
		return E_FAIL;

	if (FAILED(Ready_Layer_Effect(LAYER_EFFECT)))
		return E_FAIL;

	if (FAILED(Ready_Layer_UI(LAYER_UI)))
		return E_FAIL;

	m_pBattleManager->Begin();

	return S_OK;
}

void CLevel_Battle::Update(_float fTimeDelta)
{
	if (m_pGameInstance->Key_Down(DIK_F3))
		m_pGameInstance->Toggle_Debug();

	if (nullptr == m_pBattleManager)
		return;

	if (nullptr != m_pBattleMsgListener)
		m_pBattleMsgListener->Tick(fTimeDelta);

	m_pBattleManager->Update(fTimeDelta);

	CCamera_Director::GetInstance()->Tick(fTimeDelta);

	if (nullptr != m_pBattleMsgListener)
		m_pBattleMsgListener->Tick(fTimeDelta);

	if (nullptr != m_pExpGainListener)
		m_pExpGainListener->Tick(fTimeDelta);

	UI_Update_All(fTimeDelta);

	if (nullptr != m_pInputDirector)
		m_pInputDirector->Tick(fTimeDelta);

	if (m_pBattleManager->Is_Done())
	{
		/* Pop_Level 성공 시 BATTLE 레벨(=this) 이 즉시 Free 되므로
		   호출 후 어떤 멤버에도 접근하지 않고 곧바로 return. */
		if (FAILED(m_pGameInstance->Pop_Level()))
		{
			MSG_BOX("Failed to Exit Battle");
			return;
		}
		return;
	}
}

HRESULT CLevel_Battle::Render()
{
#ifdef _DEBUG
	if (nullptr != m_pBattleManager)
	{
		const TURN_CONTEXT& tTurn = m_pBattleManager->Get_Turn();

		_wstring strTitle = TEXT("Battle Level | Phase: ");
		strTitle += Get_BattlePhaseName(m_pBattleManager->Get_Phase());
		strTitle += TEXT(" | Turn: ");
		strTitle += to_wstring(tTurn.iTurnNumber);

		CCamera_Director* pDirector = CCamera_Director::GetInstance();
		strTitle += TEXT(" | Cam: ");
		strTitle += Get_CameraModeName(pDirector->Get_Mode());

		if (pDirector->Is_Sequence_Playing())
		{
			strTitle += TEXT(" ");
			strTitle += Get_CameraSequenceName(pDirector->Get_CurrentSequenceID());
			strTitle += TEXT(" ");
			strTitle += Get_CameraShotTypeName(pDirector->Get_CurrentShotType());
			strTitle += TEXT(" ");
			strTitle += to_wstring(static_cast<_uint>(pDirector->Get_CurrentShotElapsed() * 1000.f));
			strTitle += TEXT("/");
			strTitle += to_wstring(static_cast<_uint>(pDirector->Get_CurrentShotDuration() * 1000.f));
			strTitle += TEXT("ms");
		}

		if (pDirector->Is_Shake_Active())
			strTitle += TEXT(" Shake");

		SetWindowText(m_pGameInstance->Get_HWND(), strTitle.c_str());
	}
	else
	{
		SetWindowText(m_pGameInstance->Get_HWND(), TEXT("Battle Level"));
	}
#endif

	return S_OK;
}

HRESULT CLevel_Battle::Bind_Battle_Sources()
{
	const list<CGameObject*>* pObjects =
		m_pGameInstance->Get_ObjectList(ETOUI(LEVEL::STATIC), LAYER_PERSISTENT);

	if (nullptr == pObjects || pObjects->empty())
		return E_FAIL;

	CPlayer_Status* pPlayerState = static_cast<CPlayer_Status*>(pObjects->front());
	if (nullptr == pPlayerState)
		return E_FAIL;

	const _uint iLead = PartyOps::Find_First_Alive(pPlayerState->Get_Party());
	if (g_kMaxPartySize == iLead)
		return E_FAIL;

	if (FAILED(m_pBattleManager->Bind_PlayerParty(pPlayerState, iLead)))
		return E_FAIL;

	auto* pTrainerMgr = CTrainerData_Manager::GetInstance();
	if (nullptr == pTrainerMgr)
		return E_FAIL;

	const TRAINER_DATA* pTrainerData =
		pTrainerMgr->Find_Trainer(m_tEnv.iOpponentTrainerID);
	if (nullptr == pTrainerData)
		return E_FAIL;

	m_tOpponentTrainer = *pTrainerData;

	if (FAILED(m_pBattleManager->Bind_OpponentTrainer(&m_tOpponentTrainer)))
		return E_FAIL;

	const PARTY& tTrainerParty = m_tOpponentTrainer.tParty;

	for (_uint i = 0; i < tTrainerParty.iCount; ++i)
	{
		const POKEMON_INSTANCE* pOpponentPokemon = PartyOps::Get(tTrainerParty, i);
		if (nullptr == pOpponentPokemon)
			continue;

		if (0 == pOpponentPokemon->iSpeciesID || 0 == pOpponentPokemon->iCurrentHP)
			continue;

		pPlayerState->Mark_DexSeen(pOpponentPokemon->iSpeciesID);
	}

	return S_OK;
}

HRESULT CLevel_Battle::Ready_Lights()
{
	m_pGameInstance->Clear_Lights();

	LIGHT_DESC LightDesc{};

	LightDesc.eType = LIGHT::DIRECTIONAL;
	LightDesc.vDiffuse = _float4(0.75f, 0.75f, 0.75f, 1.f);   // GamePlay와 동일
	LightDesc.vAmbient = _float4(0.70f, 0.70f, 0.70f, 1.f);   // GamePlay와 동일
	LightDesc.vSpecular = _float4(0.3f, 0.3f, 0.3f, 1.f);     // 0.6 -> 0.3 (포켓몬 표면 하이라이트 완화)
	LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);

	if (FAILED(m_pGameInstance->Add_Light(LightDesc)))
		return E_FAIL;

	LightDesc.eType = LIGHT::POINT;
	LightDesc.vDiffuse = _float4(0.1f, 0.1f, 0.1f, 1.f);
	LightDesc.vAmbient = _float4(0.0f, 0.0f, 0.0f, 1.f);
	LightDesc.vSpecular = _float4(0.05f, 0.05f, 0.05f, 1.f);
	LightDesc.vPosition = _float4(-1.f, 1.f, -1.f, 1.f);
	LightDesc.fRange = 15.f;

	if (FAILED(m_pGameInstance->Add_Light(LightDesc)))
		return E_FAIL;

	LightDesc.eType = LIGHT::POINT;
	LightDesc.vDiffuse = _float4(0.1f, 0.1f, 0.1f, 1.f);
	LightDesc.vAmbient = _float4(0.0f, 0.0f, 0.0f, 1.f);
	LightDesc.vSpecular = _float4(0.05f, 0.05f, 0.05f, 1.f);
	LightDesc.vPosition = _float4(1.f, 1.f, -1.f, 1.f);
	LightDesc.fRange = 15.f;

	if (FAILED(m_pGameInstance->Add_Light(LightDesc)))
		return E_FAIL;

	LightDesc.eType = LIGHT::POINT;
	LightDesc.vDiffuse = _float4(0.1f, 0.1f, 0.1f, 1.f);
	LightDesc.vAmbient = _float4(0.0f, 0.0f, 0.0f, 1.f);
	LightDesc.vSpecular = _float4(0.05f, 0.05f, 0.05f, 1.f);
	LightDesc.vPosition = _float4(-1.f, 1.f, 1.f, 1.f);
	LightDesc.fRange = 15.f;

	if (FAILED(m_pGameInstance->Add_Light(LightDesc)))
		return E_FAIL;

	LightDesc.eType = LIGHT::POINT;
	LightDesc.vDiffuse = _float4(0.1f, 0.1f, 0.1f, 1.f);
	LightDesc.vAmbient = _float4(0.0f, 0.0f, 0.0f, 1.f);
	LightDesc.vSpecular = _float4(0.05f, 0.05f, 0.05f, 1.f);
	LightDesc.vPosition = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.fRange = 15.f;

	if (FAILED(m_pGameInstance->Add_Light(LightDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Battle::Ready_Layer_Camera(WNameID strLayerTag)
{
	CCamera_Free::CAMERA_FREE_DESC CameraDesc = {};

	CameraDesc.vEye = _float3(-0.11f, 1.74f, -5.11f);
	CameraDesc.vAt = _float3(0.f, 0.f, 0.f);
	CameraDesc.fFovy = XMConvertToRadians(35.f);
	CameraDesc.fNear = 0.1f;
	CameraDesc.fFar = 500.f;
	CameraDesc.fSpeedPerSec = 20.f;
	CameraDesc.fRotationPerSec = XMConvertToRadians(180.f);
	CameraDesc.fMouseSensor = 0.03f;

	/* Camera_Free 인스턴스 직접 확보 (Director Bind 용).
	   Effect_Manager::Spawn 과 동일 패턴 — Clone 의 ref 는 Layer 가 가져가고
	   caller 는 borrowed 포인터로만 사용(실패 시에만 Safe_Release). */
	CBase* pCloned = m_pGameInstance->Clone_Prototype(
		PROTOTYPE::GAMEOBJECT,
		ETOUI(LEVEL::STATIC),
		PROTO_OBJ_CAMERA_FREE,
		&CameraDesc);
	if (nullptr == pCloned)
		return E_FAIL;

	CCamera_Free* pCamera = static_cast<CCamera_Free*>(pCloned);

	if (FAILED(m_pGameInstance->Add_GameObject_Ex(
		ETOUI(LEVEL::BATTLE), strLayerTag, pCamera)))
	{
		Safe_Release(pCamera);
		return E_FAIL;
	}

	/* M1: Director Bind + BATTLE_DEFAULT 모드 진입 + 기본 카메라 위치 즉시 cut.
	   호출 순서 — Set_Mode 가 먼저여야 Set_Default_Battle_Pose 가 즉시 Apply 됨
	   (Camera_Director.cpp:Set_Default_Battle_Pose 의 mode 가드 참고). */
	CAMERA_POSE def = {};
	def.vPosition = _float3(-2.0f, 1.84f, -4.9f);
	def.vLookAt = _float3(-0.3f, 0.0f, 0.0f);
	def.vUp = _float3(0.0f, 1.0f, 0.0f);
	def.fFovY = 0.f;                            // M1: FOV 변경 없음 (§4.3)

	CCamera_Director* pDirector = CCamera_Director::GetInstance();
	pDirector->Bind(pCamera, m_pBattleManager);
	pDirector->Set_Mode(CAMERA_MODE::BATTLE_DEFAULT);
	pDirector->Set_Default_Battle_Pose(def);

	return S_OK;
}

HRESULT CLevel_Battle::Ready_Layer_BackGround(WNameID strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::GAMEPLAY), PROTO_BMAP_GRASS, ETOUI(LEVEL::BATTLE), strLayerTag)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Battle::Ready_Layer_Battler(WNameID strLayerTag)
{
	if (nullptr == m_pBattleManager)
		return E_FAIL;

	CPlayer_Status* pPlayerState = m_pBattleManager->Get_PlayerState();
	const TRAINER_DATA* pTrainerData = m_pBattleManager->Get_OpponentTrainer();

	for (_uint iSide = 0; iSide < g_kBattleSideCount; ++iSide)
	{
		CBattle_Trainer::BATTLE_TRAINER_DESC tDesc{};
		tDesc.iSide = iSide;

		if (g_kBattleSide_Player == iSide && nullptr != pPlayerState)
		{
			tDesc.strBodyProtoTag = pPlayerState->Get_TrainerBodyProtoTag();
			tDesc.strModelProtoTag = pPlayerState->Get_TrainerModelProtoTag();
			tDesc.strShaderProtoTag = pPlayerState->Get_TrainerShaderProtoTag();
		}
		else if (g_kBattleSide_Opponent == iSide && nullptr != pTrainerData)
		{
			tDesc.strBodyProtoTag = pTrainerData->strBodyProtoTag;
			tDesc.strModelProtoTag = pTrainerData->strModelProtoTag;
			tDesc.strShaderProtoTag = pTrainerData->strShaderProtoTag;
			strcpy_s(tDesc.szMappingPath, sizeof(tDesc.szMappingPath), pTrainerData->szMappingPath);
		}
		else
		{
			// 야생 룰 공존 기간 또는 데이터 누락 시 폴백
			tDesc.strBodyProtoTag = PROTO_OBJ_BODY_HERO;
			tDesc.strModelProtoTag = PROTO_COM_MODEL_HERO;
			tDesc.strShaderProtoTag = PROTO_COM_SHADER_PLAYER_LGPE;
		}

		tDesc.iDefaultAnim = BattleAnim::Find_AnimIndex(tDesc.strModelProtoTag, ANIM_KIND::INTRO);
		tDesc.bLoop = false;
		tDesc.fScale = 1.f;
		tDesc.vPos = m_pBattleManager->Get_TrainerPos(iSide);
		tDesc.fYaw = m_pBattleManager->Get_TrainerYaw(iSide);

		if (FAILED(m_pGameInstance->Add_GameObject(
			ETOUI(LEVEL::STATIC), PROTO_OBJ_BATTLE_TRAINER,
			ETOUI(LEVEL::BATTLE), strLayerTag,
			&tDesc)))
			return E_FAIL;

		const list<CGameObject*>* pList =
			m_pGameInstance->Get_ObjectList(ETOUI(LEVEL::BATTLE), strLayerTag);

		if (nullptr == pList || pList->empty())
			return E_FAIL;

		CGameObject* pTrainerObj = pList->back();
		m_pBattleManager->Register_TrainerObj(iSide, pTrainerObj);

		if (CBattle_Trainer* pTrainer = dynamic_cast<CBattle_Trainer*>(pTrainerObj))
			pTrainer->Play_Intro();
	}

	for (_uint iSide = 0; iSide < g_kBattleSideCount; ++iSide)
	{
		const BATTLE_SLOT& tSlot = m_pBattleManager->Get_Slot(iSide);

		if (nullptr == tSlot.pPokemon)
			return E_FAIL;

		CBattle_Pokemon::POKEMON_DESC tDesc{};
		tDesc.pInstance = tSlot.pPokemon;
		tDesc.iSide = iSide;
		tDesc.strBodyProtoTag = PROTO_OBJ_BODY_POKEMON;
		tDesc.strShaderProtoTag = PROTO_COM_SHADER_POKEMON;
		tDesc.iDefaultAnim = 0;
		tDesc.bLoop = true;
		tDesc.fScale = 1.f;
		tDesc.bStartVisible = false;
		tDesc.vPos = m_pBattleManager->Get_PokemonPos(iSide);
		tDesc.fYaw = m_pBattleManager->Get_PokemonYaw(iSide);

		if (FAILED(m_pGameInstance->Add_GameObject(
			ETOUI(LEVEL::STATIC), PROTO_OBJ_MONSTER,
			ETOUI(LEVEL::BATTLE), strLayerTag,
			&tDesc)))
			return E_FAIL;

		const list<CGameObject*>* pList =
			m_pGameInstance->Get_ObjectList(ETOUI(LEVEL::BATTLE), strLayerTag);

		if (nullptr == pList || pList->empty())
			return E_FAIL;

		CGameObject* pObj = pList->back();
		m_pBattleManager->Register_BattlerObj(iSide, pObj);

		// Battle_Pokemon 캐스팅 후 Manager 주입 + PokemonListener 결합
		CBattle_Pokemon* pPoke = dynamic_cast<CBattle_Pokemon*>(pObj);
		if (nullptr != pPoke)
		{
			pPoke->Set_Manager(m_pBattleManager);

			CBattle_PokemonListener* pListener = CBattle_PokemonListener::Create();
			if (nullptr == pListener)
				return E_FAIL;

			pListener->Bind(pPoke, iSide, m_pBattleManager);

			CBattle_EventDispatcher* pDispatcher = m_pBattleManager->Get_EventDispatcher();
			if (nullptr == pDispatcher)
			{
				Safe_Release(pListener);
				return E_FAIL;
			}

			if (FAILED(pDispatcher->Subscribe(pListener)))
			{
				Safe_Release(pListener);
				return E_FAIL;
			}

			m_pPokemonListeners[iSide] = pListener;  // strong ref 유지 (Free 에서 Safe_Re
		}
	}

	return S_OK;
}

HRESULT CLevel_Battle::Ready_Layer_Effect(WNameID strLayerTag)
{
	(void)strLayerTag;

	return S_OK;
}

HRESULT CLevel_Battle::Ready_Layer_UI(WNameID strLayerTag)
{
	CUISequence* pSeq{ nullptr };
	CUISequence::UISEQUENCE_DESC tDesc{};
	tDesc.strPath = "../../DataFiles/UI/UI_BattlePlate.uiseq";
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
		CBattlePlate* pBattlePlate = CBattlePlate::Create();
		if (nullptr == pBattlePlate)
			return E_FAIL;

		if (FAILED(pBattlePlate->Initialize(pSeq)))
		{
			Safe_Release(pBattlePlate);
			return E_FAIL;
		}

		pBattlePlate->Bind(m_pBattleManager);

		if (FAILED(UI_Register(pBattlePlate, ETOUI(LEVEL::BATTLE))))
		{
			Safe_Release(pBattlePlate);
			return E_FAIL;
		}

		m_pBattlePlate = pBattlePlate;  // weak - UI Hub owns
		Safe_Release(pBattlePlate);

		m_pBattlePlate->Open();
		m_pBattlePlate->Snap_HPDisplay();
	}

	tDesc.strPath = "../../DataFiles/UI/UI_BattleCommand.uiseq";
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
		CBattle_CommandMenu* pCmdMenu = CBattle_CommandMenu::Create();
		if (nullptr == pCmdMenu)
			return E_FAIL;

		if (FAILED(pCmdMenu->Initialize(pSeq)))
		{
			Safe_Release(pCmdMenu);
			return E_FAIL;
		}

		if (FAILED(UI_Register(pCmdMenu, ETOUI(LEVEL::BATTLE))))
		{
			Safe_Release(pCmdMenu);
			return E_FAIL;
		}

		m_pCommandMenu = pCmdMenu;      // weak - UI Hub owns
		Safe_Release(pCmdMenu);
	}

	tDesc.strPath = "../../DataFiles/UI/UI_BattleMove.uiseq";
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
		CBattle_MoveMenu* pMoveMenu = CBattle_MoveMenu::Create();
		if (nullptr == pMoveMenu)
			return E_FAIL;

		if (FAILED(pMoveMenu->Initialize(pSeq)))
		{
			Safe_Release(pMoveMenu);
			return E_FAIL;
		}

		pMoveMenu->Bind(m_pBattleManager);

		if (FAILED(UI_Register(pMoveMenu, ETOUI(LEVEL::BATTLE))))
		{
			Safe_Release(pMoveMenu);
			return E_FAIL;
		}

		m_pMoveMenu = pMoveMenu;        // weak - UI Hub owns
		Safe_Release(pMoveMenu);
	}

	tDesc.strPath = "../../DataFiles/UI/UI_BattleMsg.uiseq";
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

	CBattleMsg* pBattleMsg = CBattleMsg::Create();
	if (nullptr == pBattleMsg)
		return E_FAIL;

	if (FAILED(pBattleMsg->Initialize(pSeq)))
	{
		Safe_Release(pBattleMsg);
		return E_FAIL;
	}

	if (FAILED(UI_Register(pBattleMsg, ETOUI(LEVEL::BATTLE))))
	{
		Safe_Release(pBattleMsg);
		return E_FAIL;
	}

	m_pBattleMsg = pBattleMsg;      // weak - Hub owns
	Safe_Release(pBattleMsg);       // local ref--

	m_pBattleManager->Set_BattleMsg(m_pBattleMsg);

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

	m_pBattleMsgListener = CBattle_MsgListener::Create();
	if (nullptr == m_pBattleMsgListener)
		return E_FAIL;

	m_pBattleMsgListener->Bind(m_pBattleManager, m_pBattleMsg);

	CBattle_EventDispatcher* pDispatcher = m_pBattleManager->Get_EventDispatcher();
	if (nullptr == pDispatcher)
		return E_FAIL;

	if (FAILED(pDispatcher->Subscribe(m_pBattleMsgListener)))
		return E_FAIL;

	m_pExpGainListener = CBattle_ExpGainListener::Create();
	if (nullptr == m_pExpGainListener)
		return E_FAIL;

	m_pExpGainListener->Bind(m_pBattleManager);

	if (FAILED(pDispatcher->Subscribe(m_pExpGainListener)))
		return E_FAIL;

	m_pBattlePlateListener = CBattle_PlateListener::Create();
	if (nullptr == m_pBattlePlateListener)
		return E_FAIL;

	m_pBattlePlateListener->Bind(m_pBattlePlate);

	if (FAILED(pDispatcher->Subscribe(m_pBattlePlateListener)))
		return E_FAIL;

	m_pInputDirector = CBattle_InputDirector::Create();
	if (nullptr == m_pInputDirector)
		return E_FAIL;

	m_pInputDirector->Bind(m_pBattleManager, m_pCommandMenu, m_pMoveMenu);

	if (FAILED(pDispatcher->Subscribe(m_pInputDirector)))
		return E_FAIL;

	return S_OK;
}

CLevel_Battle* CLevel_Battle::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const LEVEL_ENTRY_DESC* pEntryDesc)
{
	if (nullptr == pEntryDesc)
	{
		MSG_BOX("Battle entry desc is missing");
		return nullptr;
	}

	const void* pPayload = pEntryDesc->Get_Payload(LEVEL_ENTRY_PAYLOAD::BATTLE_ENV, sizeof(BATTLE_ENV));
	if (nullptr == pPayload)
	{
		MSG_BOX("Battle env payload is invalid");
		return nullptr;
	}

	const BATTLE_ENV& tEnv = *static_cast<const BATTLE_ENV*>(pPayload);

	CLevel_Battle* pInstance = new CLevel_Battle(pDevice, pContext, tEnv);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_Battle");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Battle::Free()
{
	/* §함정 10 + 종료 누수 차단:
	   MainApp::Free 에서 Cleanup_StaticTables 가 Release_Engine 보다 먼저 호출되어
	   Director 인스턴스가 이미 파괴된 상태에서 본 함수가 호출되는 경로가 있다.
	   GetInstance 를 거치면 새 인스턴스가 만들어져 누수가 되므로, Try_Unbind 로 우회. */
	CCamera_Director::Try_Unbind();

	// PokemonListener 들 정리 (Battle_Pokemon weak ref 가 살아있을 때)
	for (_uint i = 0; i < g_kBattleSideCount; ++i)
	{
		if (nullptr != m_pPokemonListeners[i] && nullptr != m_pBattleManager)
		{
			CBattle_EventDispatcher* pDispatcher = m_pBattleManager->Get_EventDispatcher();
			if (nullptr != pDispatcher)
				pDispatcher->Unsubscribe(m_pPokemonListeners[i]);
		}

		Safe_Release(m_pPokemonListeners[i]);
	}

	// Director 정리
	if (nullptr != m_pInputDirector && nullptr != m_pBattleManager)
	{
		CBattle_EventDispatcher* pDispatcher = m_pBattleManager->Get_EventDispatcher();
		if (nullptr != pDispatcher)
			pDispatcher->Unsubscribe(m_pInputDirector);
	}

	Safe_Release(m_pInputDirector);

	if (nullptr != m_pExpGainListener && nullptr != m_pBattleManager)
	{
		CBattle_EventDispatcher* pDispatcher = m_pBattleManager->Get_EventDispatcher();
		if (nullptr != pDispatcher)
			pDispatcher->Unsubscribe(m_pExpGainListener);
	}

	Safe_Release(m_pExpGainListener);

	if (nullptr != m_pBattleMsgListener && nullptr != m_pBattleManager)
	{
		CBattle_EventDispatcher* pDispatcher = m_pBattleManager->Get_EventDispatcher();
		if (nullptr != pDispatcher)
			pDispatcher->Unsubscribe(m_pBattleMsgListener);
	}

	Safe_Release(m_pBattleMsgListener);

	if (nullptr != m_pBattlePlateListener && nullptr != m_pBattleManager)
	{
		CBattle_EventDispatcher* pDispatcher = m_pBattleManager->Get_EventDispatcher();
		if (nullptr != pDispatcher)
			pDispatcher->Unsubscribe(m_pBattlePlateListener);
	}

	Safe_Release(m_pBattlePlateListener);

	UI_Set_Cursor_Sequence(nullptr);
	UI_Cleanup_Level(ETOUI(LEVEL::BATTLE));
	m_pCursorSeq = nullptr;
	m_pBattleMsg = nullptr;
	m_pBattlePlate = nullptr;
	m_pCommandMenu = nullptr;
	m_pMoveMenu = nullptr;

	Safe_Release(m_pBattleManager);

	__super::Free();
}