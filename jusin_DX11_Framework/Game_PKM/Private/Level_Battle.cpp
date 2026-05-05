#include "Level_Battle.h"
#include "Level_Loading.h"
#include "Battle_Manager.h"
#include "PlayerState.h"
#include "PokemonData_Manager.h"

#include "GameInstance.h"

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
	return S_OK;
}

HRESULT CLevel_Battle::Ready_Layer_Camera(WNameID strLayerTag)
{
	(void)strLayerTag;

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
	(void)strLayerTag;

	return S_OK;
}

HRESULT CLevel_Battle::Ready_Layer_Effect(WNameID strLayerTag)
{
	(void)strLayerTag;

	return S_OK;
}

HRESULT CLevel_Battle::Ready_Layer_UI(WNameID strLayerTag)
{
	(void)strLayerTag;

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
	__super::Free();

	Safe_Release(m_pBattleManager);
}