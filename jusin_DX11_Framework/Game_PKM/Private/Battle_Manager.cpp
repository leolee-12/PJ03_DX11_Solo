#include "Battle_Manager.h"
#include "PlayerState.h"
#include "PokemonData_Manager.h"
#include "Battle_Layout.h"
#include "Battler.h"
#include "CommandQueue.h"
#include "Battle_States.h"

#include "GameInstance.h"

CBattle_Manager::CBattle_Manager()
{
}

HRESULT CBattle_Manager::Initialize(const BATTLE_ENV& tEnv)
{
	m_tEnv = tEnv;

	Reset_FieldState(m_tField);
	Reset_TurnContext(m_tTurn);

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

	if (FAILED(Initialize_CoreComponents()))
		return E_FAIL;

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

	Safe_Release(m_pBattlers[g_kBattleSide_Player]);

	CBattler::BATTLER_DESC tDesc{};
	tDesc.iSide = g_kBattleSide_Player;
	tDesc.iSlotIndex = 0;
	tDesc.pInstance = pLead;

	m_pBattlers[g_kBattleSide_Player] = CBattler::Create(tDesc);
	if (nullptr == m_pBattlers[g_kBattleSide_Player])
		return E_FAIL;

	return S_OK;
}

HRESULT CBattle_Manager::Bind_OpponentSingle(POKEMON_INSTANCE* pOpponent)
{
	if (nullptr == pOpponent)
		return E_FAIL;

	m_pOpponentSingle = pOpponent;

	Safe_Release(m_pBattlers[g_kBattleSide_Opponent]);

	CBattler::BATTLER_DESC tDesc{};
	tDesc.iSide = g_kBattleSide_Opponent;
	tDesc.iSlotIndex = 0;
	tDesc.pInstance = pOpponent;

	m_pBattlers[g_kBattleSide_Opponent] = CBattler::Create(tDesc);
	if (nullptr == m_pBattlers[g_kBattleSide_Opponent])
		return E_FAIL;

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

	POKEMON_INSTANCE* pLead = PartyOps::Get(tParty, iLead);
	if (nullptr == pLead)
		return E_FAIL;

	Safe_Release(m_pBattlers[g_kBattleSide_Opponent]);

	CBattler::BATTLER_DESC tDesc{};
	tDesc.iSide = g_kBattleSide_Opponent;
	tDesc.iSlotIndex = 0;
	tDesc.pInstance = pLead;

	m_pBattlers[g_kBattleSide_Opponent] = CBattler::Create(tDesc);
	if (nullptr == m_pBattlers[g_kBattleSide_Opponent])
		return E_FAIL;

	return S_OK;
}

void CBattle_Manager::Begin()
{
	Request_State(BATTLE_PHASE::INTRO);

	BATTLE_CONTEXT ctx = Build_Context();
	Apply_Pending_Transition(ctx);
}

void CBattle_Manager::Update(_float fTimeDelta)
{
	BATTLE_CONTEXT ctx = Build_Context();

	if (nullptr == m_pCurrentState)
	{
		Request_State(BATTLE_PHASE::INTRO);
		Apply_Pending_Transition(ctx);
	}

	if (nullptr != m_pCurrentState)
		m_pCurrentState->Update(ctx, fTimeDelta);

	Apply_Pending_Transition(ctx);
}

void CBattle_Manager::Request_Exit()
{
	Request_State(BATTLE_PHASE::DONE);
}

const BATTLE_SLOT& CBattle_Manager::Get_Slot(_uint iSide) const
{
	static BATTLE_SLOT s_tEmptySlot{};

	CBattler* pBattler = Get_Battler(iSide);
	if (nullptr == pBattler)
		return s_tEmptySlot;

	return pBattler->Get_Slot();
}

CBattler* CBattle_Manager::Get_Battler(_uint iSide) const
{
	if (iSide >= g_kBattleSideCount)
		return nullptr;

	return m_pBattlers[iSide];
}

void CBattle_Manager::Register_BattlerObj(_uint iSide, CGameObject* pObj)
{
	if (iSide >= g_kBattleSideCount)
		return;

	m_pBattlerObj[iSide] = pObj;
}

void CBattle_Manager::Register_TrainerObj(_uint iSide, CGameObject* pObj)
{
	if (iSide >= g_kBattleSideCount)
		return;

	m_pTrainerObj[iSide] = pObj;
}

CGameObject* CBattle_Manager::Get_BattlerObj(_uint iSide) const
{
	if (iSide >= g_kBattleSideCount)
		return nullptr;

	return m_pBattlerObj[iSide];
}

CGameObject* CBattle_Manager::Get_TrainerObj(_uint iSide) const
{
	if (iSide >= g_kBattleSideCount)
		return nullptr;

	return m_pTrainerObj[iSide];
}

_float3 CBattle_Manager::Get_TrainerPos(_uint iSide, _uint iSlotIndex) const
{
	return BattleLayout::Get_TrainerPos(m_tEnv.eRule, iSide, iSlotIndex);
}

_float CBattle_Manager::Get_TrainerYaw(_uint iSide, _uint iSlotIndex) const
{
	return BattleLayout::Get_TrainerYaw(m_tEnv.eRule, iSide, iSlotIndex);
}

_float3 CBattle_Manager::Get_PokemonPos(_uint iSide, _uint iSlotIndex) const
{
	return BattleLayout::Get_PokemonPos(m_tEnv.eRule, iSide, iSlotIndex);
}

_float CBattle_Manager::Get_PokemonYaw(_uint iSide, _uint iSlotIndex) const
{
	return BattleLayout::Get_PokemonYaw(m_tEnv.eRule, iSide, iSlotIndex);
}

BATTLE_CONTEXT CBattle_Manager::Build_Context()
{
	BATTLE_CONTEXT ctx{};
	ctx.pManager = this;
	ctx.pBattlers[g_kBattleSide_Player] = m_pBattlers[g_kBattleSide_Player];
	ctx.pBattlers[g_kBattleSide_Opponent] = m_pBattlers[g_kBattleSide_Opponent];
	ctx.pField = &m_tField;
	ctx.pTurn = &m_tTurn;
	ctx.pDispatcher = nullptr;
	ctx.pDataMgr = CPokemonData_Manager::GetInstance();

	return ctx;
}

HRESULT CBattle_Manager::Initialize_CoreComponents()
{
	m_pQueue = CCommandQueue::Create();
	if (nullptr == m_pQueue)
		return E_FAIL;

	return S_OK;
}

IBattleState* CBattle_Manager::Create_State(BATTLE_PHASE ePhase)
{
	switch (ePhase)
	{
	case BATTLE_PHASE::INTRO:
		return CIntroState::Create();

	case BATTLE_PHASE::INPUT_PLAYER:
		return CInputPlayerState::Create();

	case BATTLE_PHASE::INPUT_OPPONENT:
		return CInputOpponentState::Create();

	case BATTLE_PHASE::RESOLVE_ORDER:
		return CResolveOrderState::Create();

	case BATTLE_PHASE::RESOLVE_ACTION_1:
	case BATTLE_PHASE::RESOLVE_ACTION_2:
		return CResolveActionState::Create();

	case BATTLE_PHASE::RESOLVE_END_TURN:
		return CResolveEndTurnState::Create();

	case BATTLE_PHASE::CHECK_END:
		return CCheckEndState::Create();

	case BATTLE_PHASE::OUTRO:
		return COutroState::Create();

	case BATTLE_PHASE::DONE:
		return CDoneState::Create();

	default:
		return nullptr;
	}
}

void CBattle_Manager::Release_State(IBattleState * &pState)
{
	Safe_Release(pState);
}

void CBattle_Manager::Apply_Pending_Transition(const BATTLE_CONTEXT& ctx)
{
	if (nullptr == m_pNextState)
		return;

	if (nullptr != m_pCurrentState)
		m_pCurrentState->OnExit(ctx);

	Release_State(m_pCurrentState);

	m_pCurrentState = m_pNextState;
	m_pNextState = nullptr;

	m_ePhase = m_pCurrentState->Get_Phase();
	m_pCurrentState->OnEnter(ctx);
}

void CBattle_Manager::Request_State(BATTLE_PHASE ePhase)
{
	Release_State(m_pNextState);
	m_pNextState = Create_State(ePhase);
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
	for (auto& pObj : m_pBattlerObj)
		pObj = nullptr;

	for (auto& pObj : m_pTrainerObj)
		pObj = nullptr;

	for (auto& pBattler : m_pBattlers)
		Safe_Release(pBattler);

	Safe_Release(m_pQueue);
	Release_State(m_pNextState);
	Release_State(m_pCurrentState);

	__super::Free();
}