#include "Battle_Manager.h"
#include "PlayerState.h"
#include "PokemonData_Manager.h"

#include "GameInstance.h"

CBattle_Manager::CBattle_Manager()
{
}

HRESULT CBattle_Manager::Initialize(const BATTLE_ENV& tEnv)
{
	m_tEnv = tEnv;
	m_tField = {};
	m_tTurn = {};
	m_tTurn.iTurnNumber = 1;

	for (auto& tSlot : m_tSlot)
	{
		tSlot = {};
		tSlot.iLastMoveUsed = 0;
	}

	auto* pDataMgr = CPokemonData_Manager::GetInstance();
	if (nullptr != pDataMgr)
	{
		const BATTLE_RULE_DESC* pRule = pDataMgr->Find_BattleRule(m_tEnv.eRule);
		if (nullptr != pRule)
		{
			m_tEnv.bCanRun = pRule->bCanRun;
			m_tEnv.bCanCapture = pRule->bCanCapture;
			m_tEnv.bExpGain = pRule->bExpGain;
		}
	}

	m_ePhase = BATTLE_PHASE::INTRO;

	return S_OK;
}

HRESULT CBattle_Manager::Bind_PlayerParty(CPlayerState* pPlayerState, _uint iLeadSlot)
{
	if (nullptr == pPlayerState)
		return E_FAIL;

	m_pPlayerState = pPlayerState;

	PARTY& tParty = pPlayerState->Get_Party();
	POKEMON_INSTANCE* pLead = PartyOps::Get(tParty, iLeadSlot);
	if (nullptr == pLead)
		return E_FAIL;

	m_tSlot[g_kBattleSide_Player].pPokemon = pLead;

	return S_OK;
}

HRESULT CBattle_Manager::Bind_OpponentSingle(POKEMON_INSTANCE* pOpponent)
{
	if (nullptr == pOpponent)
		return E_FAIL;

	m_pOpponentSingle = pOpponent;
	m_tSlot[g_kBattleSide_Opponent].pPokemon = pOpponent;

	return S_OK;
}

HRESULT CBattle_Manager::Bind_OpponentTrainer(TRAINER_DATA* pTrainerData)
{
	if (nullptr == pTrainerData)
		return E_FAIL;

	m_pOpponentTrainer = pTrainerData;

	PARTY& tParty = pTrainerData->tParty;
	const _uint iLead = PartyOps::Find_First_Alive(tParty);
	if (g_kMaxPartySize == iLead)
		return E_FAIL;

	m_tSlot[g_kBattleSide_Opponent].pPokemon = PartyOps::Get(tParty, iLead);
	if (nullptr == m_tSlot[g_kBattleSide_Opponent].pPokemon)
		return E_FAIL;

	return S_OK;
}

void CBattle_Manager::Update(_float fTimeDelta)
{
	switch (m_ePhase)
	{
	case BATTLE_PHASE::INTRO:
		Phase_Intro(fTimeDelta);
		break;
	case BATTLE_PHASE::INPUT_PLAYER:
		Phase_Input_Player(fTimeDelta);
		break;
	case BATTLE_PHASE::INPUT_OPPONENT:
		Phase_Input_Opponent(fTimeDelta);
		break;
	case BATTLE_PHASE::RESOLVE_ORDER:
		Phase_Resolve_Order(fTimeDelta);
		break;
	case BATTLE_PHASE::RESOLVE_ACTION_1:
		Phase_Resolve_Action(fTimeDelta, 0);
		break;
	case BATTLE_PHASE::RESOLVE_ACTION_2:
		Phase_Resolve_Action(fTimeDelta, 1);
		break;
	case BATTLE_PHASE::RESOLVE_END_TURN:
		Phase_Resolve_End(fTimeDelta);
		break;
	case BATTLE_PHASE::CHECK_END:
		Phase_Check_End(fTimeDelta);
		break;
	case BATTLE_PHASE::OUTRO:
		Phase_Outro(fTimeDelta);
		break;
	case BATTLE_PHASE::DONE:
	default:
		break;
	}
}

void CBattle_Manager::Request_Exit()
{
	m_ePhase = BATTLE_PHASE::DONE;
}

const BATTLE_SLOT& CBattle_Manager::Get_Slot(_uint iSide) const
{
	return m_tSlot[iSide < g_kBattleSideCount ? iSide : g_kBattleSide_Player];
}

void CBattle_Manager::Phase_Intro(_float fTimeDelta)
{
	(void)fTimeDelta;

	m_ePhase = BATTLE_PHASE::INPUT_PLAYER;
}

void CBattle_Manager::Phase_Input_Player(_float fTimeDelta)
{
	(void)fTimeDelta;

	auto* pGameInstance = CGameInstance::GetInstance();

	if (pGameInstance->Key_Down(DIK_ESCAPE))
	{
		Request_Exit();
		return;
	}

	if (pGameInstance->Key_Down(DIK_RETURN))
	{
		m_tTurn.tAction[g_kBattleSide_Player].eType = ACTION_TYPE::USE_MOVE;
		m_tTurn.tAction[g_kBattleSide_Player].iParam = 0;
		m_tTurn.tAction[g_kBattleSide_Player].iPriority = 0;

		m_ePhase = BATTLE_PHASE::INPUT_OPPONENT;
	}
}

void CBattle_Manager::Phase_Input_Opponent(_float fTimeDelta)
{
	(void)fTimeDelta;

	m_tTurn.tAction[g_kBattleSide_Opponent].eType = ACTION_TYPE::USE_MOVE;
	m_tTurn.tAction[g_kBattleSide_Opponent].iParam = 0;
	m_tTurn.tAction[g_kBattleSide_Opponent].iPriority = 0;

	m_ePhase = BATTLE_PHASE::RESOLVE_ORDER;
}

void CBattle_Manager::Phase_Resolve_Order(_float fTimeDelta)
{
	(void)fTimeDelta;

	m_tTurn.iFirstSide = g_kBattleSide_Player;
	m_ePhase = BATTLE_PHASE::RESOLVE_ACTION_1;
}

void CBattle_Manager::Phase_Resolve_Action(_float fTimeDelta, _uint iOrderIndex)
{
	(void)fTimeDelta;

	m_ePhase = (0 == iOrderIndex) ? BATTLE_PHASE::RESOLVE_ACTION_2 : BATTLE_PHASE::RESOLVE_END_TURN;
}

void CBattle_Manager::Phase_Resolve_End(_float fTimeDelta)
{
	(void)fTimeDelta;

	++m_tTurn.iTurnNumber;
	m_ePhase = BATTLE_PHASE::CHECK_END;
}

void CBattle_Manager::Phase_Check_End(_float fTimeDelta)
{
	(void)fTimeDelta;

	m_ePhase = BATTLE_PHASE::INPUT_PLAYER;
}

void CBattle_Manager::Phase_Outro(_float fTimeDelta)
{
	(void)fTimeDelta;

	Request_Exit();
}

CBattle_Manager* CBattle_Manager::Create(const BATTLE_ENV& tEnv)
{
	CBattle_Manager* pInstance = new CBattle_Manager();

	if (FAILED(pInstance->Initialize(tEnv)))
	{
		MSG_BOX("Failed to Created : CBattle_Manager");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBattle_Manager::Free()
{
	__super::Free();
}