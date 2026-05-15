#include "Level_Battle.h"
#include "Battle_Manager.h"
#include "PlayerState.h"
#include "PokemonData_Manager.h"
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
#include "Game_API.h"
#include "Camera_Free.h"

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
		case BATTLE_PHASE::OUTRO: return TEXT("OUTRO");
		case BATTLE_PHASE::DONE: return TEXT("DONE");
		default: return TEXT("UNKNOWN");
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

	if (nullptr != m_pBattleMsgListener)
		m_pBattleMsgListener->Tick(fTimeDelta);

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

	CPlayerState* pPlayerState = static_cast<CPlayerState*>(pObjects->front());
	if (nullptr == pPlayerState)
		return E_FAIL;

	const _uint iLead = PartyOps::Find_First_Alive(pPlayerState->Get_Party());
	if (g_kMaxPartySize == iLead)
		return E_FAIL;

	if (FAILED(m_pBattleManager->Bind_PlayerParty(pPlayerState, iLead)))
		return E_FAIL;

	if (FAILED(Ready_Debug_WildOpponent()))
		return E_FAIL;

	if (FAILED(m_pBattleManager->Bind_OpponentSingle(&m_tDebugWildOpponent)))
		return E_FAIL;

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
		ETOUI(LEVEL::BATTLE), strLayerTag,
		&CameraDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Battle::Ready_Layer_BackGround(WNameID strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::GAMEPLAY), PROTO_BMAP_TOWN, ETOUI(LEVEL::BATTLE), strLayerTag)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Battle::Ready_Layer_Battler(WNameID strLayerTag)
{
	if (nullptr == m_pBattleManager)
		return E_FAIL;

	for (_uint iSide = 0; iSide < g_kBattleSideCount; ++iSide)
	{
		CBattle_Trainer::BATTLE_TRAINER_DESC tDesc{};
		tDesc.iSide = iSide;
		tDesc.strBodyProtoTag = PROTO_OBJ_BODY_HERO;
		tDesc.strModelProtoTag = PROTO_COM_MODEL_HERO;
		tDesc.strShaderProtoTag = PROTO_COM_SHADER_PLAYER_LGPE;
		tDesc.iDefaultAnim = 17;
		tDesc.bLoop = true;
		tDesc.fScale = 0.4f;
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

		m_pBattleManager->Register_TrainerObj(iSide, pList->back());
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

			pListener->Bind(pPoke, iSide);

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

		if (FAILED(UI_Register(pBattlePlate)))
		{
			Safe_Release(pBattlePlate);
			return E_FAIL;
		}

		m_pBattlePlate = pBattlePlate;  // weak — UI Hub owns
		Safe_Release(pBattlePlate);

		m_pBattlePlate->Open();
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

		if (FAILED(UI_Register(pCmdMenu)))
		{
			Safe_Release(pCmdMenu);
			return E_FAIL;
		}

		m_pCommandMenu = pCmdMenu;      // weak — UI Hub owns
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

		if (FAILED(UI_Register(pMoveMenu)))
		{
			Safe_Release(pMoveMenu);
			return E_FAIL;
		}

		m_pMoveMenu = pMoveMenu;        // weak — UI Hub owns
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

	if (FAILED(UI_Register(pBattleMsg)))
	{
		Safe_Release(pBattleMsg);
		return E_FAIL;
	}

	m_pBattleMsg = pBattleMsg;      // weak - Hub owns
	Safe_Release(pBattleMsg);       // local ref--

	m_pBattleManager->Set_BattleMsg(m_pBattleMsg);

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

	m_pBattleMsgListener = CBattle_MsgListener::Create();
	if (nullptr == m_pBattleMsgListener)
		return E_FAIL;

	m_pBattleMsgListener->Bind(m_pBattleManager, m_pBattleMsg);

	CBattle_EventDispatcher* pDispatcher = m_pBattleManager->Get_EventDispatcher();
	if (nullptr == pDispatcher)
		return E_FAIL;

	if (FAILED(pDispatcher->Subscribe(m_pBattleMsgListener)))
		return E_FAIL;

	m_pInputDirector = CBattle_InputDirector::Create();
	if (nullptr == m_pInputDirector)
		return E_FAIL;

	m_pInputDirector->Bind(m_pBattleManager, m_pCommandMenu, m_pMoveMenu);

	if (FAILED(pDispatcher->Subscribe(m_pInputDirector)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Battle::Ready_Debug_WildOpponent()
{
	auto* pDataMgr = CPokemonData_Manager::GetInstance();
	if (nullptr == pDataMgr)
		return E_FAIL;

	const SPECIES_DATA* pSpecies = pDataMgr->Find_Species(1);
	if (nullptr == pSpecies)
		return E_FAIL;

	m_tDebugWildOpponent = {};

	m_tDebugWildOpponent.iSpeciesID = pSpecies->iDexNo;
	wcscpy_s(m_tDebugWildOpponent.szNickname, pSpecies->szName);

	m_tDebugWildOpponent.iLevel = 5;
	m_tDebugWildOpponent.iExp = 0;

	for (size_t i = 0; i < static_cast<size_t>(STAT::END); ++i)
	{
		m_tDebugWildOpponent.iIV[i] = g_kMaxIV;
		m_tDebugWildOpponent.iEV[i] = 0;
	}

	m_tDebugWildOpponent.eNature = NATURE::HARDY;
	m_tDebugWildOpponent.iAbilityID = pSpecies->iAbility1;

	const _uint iInitialMoves[g_kMaxMovesPerPokemon] =
	{
		  pSpecies->iLearnset[0],
		  pSpecies->iLearnset[1],
		  pSpecies->iLearnset[2],
		  pSpecies->iLearnset[3],
	};

	Assign_Moves(m_tDebugWildOpponent, iInitialMoves, g_kMaxMovesPerPokemon, pDataMgr);

	m_tDebugWildOpponent.eStatus = STATUS_CONDITION::NONE;

	Recalc_All_Stats(m_tDebugWildOpponent, *pSpecies);
	m_tDebugWildOpponent.iCurrentHP = m_tDebugWildOpponent.iStat[static_cast<size_t>(STAT::HP)];

	m_tDebugWildOpponent.iCapturedAtZoneID = m_tEnv.iZoneID;

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

	if (nullptr != m_pBattleMsgListener && nullptr != m_pBattleManager)
	{
		CBattle_EventDispatcher* pDispatcher = m_pBattleManager->Get_EventDispatcher();
		if (nullptr != pDispatcher)
			pDispatcher->Unsubscribe(m_pBattleMsgListener);
	}

	Safe_Release(m_pBattleMsgListener);

	UI_Set_Cursor_Sequence(nullptr);
	UI_Close_All();
	m_pCursorSeq = nullptr;
	m_pBattleMsg = nullptr;
	m_pBattlePlate = nullptr;
	m_pCommandMenu = nullptr;
	m_pMoveMenu = nullptr;

	Safe_Release(m_pBattleManager);

	__super::Free();
}