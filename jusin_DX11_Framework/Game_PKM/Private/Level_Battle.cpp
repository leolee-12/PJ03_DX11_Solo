#include "Level_Battle.h"
#include "Level_Loading.h"
#include "Battle_Manager.h"
#include "PlayerState.h"
#include "PokemonData_Manager.h"
#include "Battle_Pokemon.h"
#include "Battle_Trainer.h"
#include "BattleMsg.h"
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

	return S_OK;
}

void CLevel_Battle::Update(_float fTimeDelta)
{
	if (nullptr == m_pBattleManager)
		return;

	m_pBattleManager->Update(fTimeDelta);

#ifdef _DEBUG
	if (nullptr != m_pBattleMsg)
	{
		if (m_pGameInstance->Key_Down(DIK_F5))
		{
			m_pBattleMsg->Set_Message(L"Pikachu used Thunderbolt!wwwwwqrwgfqegdwgefhqeijeuawus5s1246524yfsdzhsderfhestujs");
			m_pBattleMsg->Open();
		}

		if (m_pGameInstance->Key_Down(DIK_F6))
		{
			m_pBattleMsg->Close();
		}

		if (m_pGameInstance->Key_Down(DIK_F7))
		{
			m_pBattleMsg->Complete();
		}
	}
#endif

	UI_Update_All(fTimeDelta);

	if (m_pBattleManager->Is_Done())
	{
		if (SUCCEEDED(m_pGameInstance->Change_Level(ETOI(LEVEL::LOADING),
			CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL::GAMEPLAY))))
		{
			return;
		}

		MSG_BOX("Failed to Exit Battle");
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
	LightDesc.vDiffuse = _float4(0.9f, 0.9f, 0.9f, 1.f);
	LightDesc.vAmbient = _float4(0.35f, 0.35f, 0.35f, 1.f);
	LightDesc.vSpecular = _float4(0.6f, 0.6f, 0.6f, 1.f);
	LightDesc.vDirection = _float4(-0.3f, -1.f, 0.5f, 0.f);

	if (FAILED(m_pGameInstance->Add_Light(LightDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Battle::Ready_Layer_Camera(WNameID strLayerTag)
{
	CCamera_Free::CAMERA_FREE_DESC CameraDesc = {};

	CameraDesc.vEye = _float3(0.f, 7.f, -12.f);
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
	if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::BATTLE), PROTO_BMAP_TOWN, ETOUI(LEVEL::BATTLE), strLayerTag)))
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
		tDesc.iDefaultAnim = 0;
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
		tDesc.strShaderProtoTag = PROTO_COM_SHADER_VTXANIMMESH;
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

		m_pBattleManager->Register_BattlerObj(iSide, pList->back());
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
	CUISequence::UISEQUENCE_DESC tDesc{};
	tDesc.strPath = "../../DataFiles/UI/UI_BattlePlate.uiseq";
	tDesc.iProtoLevel = ETOUI(LEVEL::STATIC);

	CUISequence* pSeq = static_cast<CUISequence*>(m_pGameInstance->Clone_Prototype(
		PROTOTYPE::GAMEOBJECT, ETOUI(LEVEL::STATIC), PROTO_UI_SEQUENCE, &tDesc));
	if (nullptr == pSeq)
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_GameObject_Ex(CURRENT_LEVEL, strLayerTag, pSeq)))
	{
		Safe_Release(pSeq);
		return E_FAIL;
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

	pBattleMsg->Set_Message(L"A wild Pokemon appeared!");

	if (FAILED(UI_Register(pBattleMsg)))
	{
		Safe_Release(pBattleMsg);
		return E_FAIL;
	}

	m_pBattleMsg = pBattleMsg;      // weak - Hub owns
	Safe_Release(pBattleMsg);       // local ref--

	m_pBattleMsg->Open();

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

	m_tDebugWildOpponent.iMoves[0] = pSpecies->iLearnset[0];

	const MOVE_DATA* pMove = (0 != m_tDebugWildOpponent.iMoves[0]) ?
		pDataMgr->Find_Move(m_tDebugWildOpponent.iMoves[0]) :
		nullptr;

	m_tDebugWildOpponent.iCurrentPP[0] = (nullptr != pMove) ? pMove->iMaxPP : 0;
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
	UI_Close_All();
	m_pBattleMsg = nullptr;

	Safe_Release(m_pBattleManager);

	__super::Free();
}