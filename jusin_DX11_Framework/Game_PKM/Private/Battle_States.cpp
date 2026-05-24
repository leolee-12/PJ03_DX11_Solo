#include "Battle_States.h"
#include "IBattleCommand.h"
#include "Battle_Manager.h"
#include "CommandQueue.h"
#include "Battle_Commands.h"
#include "Battler.h"
#include "IBattleAI.h"
#include "Battle_EventDispatcher.h"
#include "Battle_ActionSequencer.h"
#include "Battle_Action_Steps.h"
#include "Battle_Camera_Steps.h"
#include "Battle_Trainer.h"

#include "GameInstance.h"

#pragma region IntroState
CIntroState::CIntroState()
{
}

void CIntroState::OnEnter(const BATTLE_CONTEXT& ctx)
{
	if (nullptr == ctx.pDispatcher || nullptr == ctx.pManager)
		return;

	EVENT_BATTLE_STARTED tEvent{};
	tEvent.tEnv = ctx.pManager->Get_Env();
	ctx.pDispatcher->Publish(tEvent);

	const BATTLE_ENV& tEnv = ctx.pManager->Get_Env();

	const bool bTrainerRule =
		BATTLE_RULE::TRAINER_SINGLE == tEnv.eRule ||
		BATTLE_RULE::TRAINER_DOUBLE == tEnv.eRule;

	if (false == bTrainerRule)
		return;

	CBattle_ActionSequencer* pSeq = ctx.pManager->Get_Sequencer();
	const TRAINER_DATA* pTrainer = ctx.pManager->Get_OpponentTrainer();
	if (nullptr == pSeq || nullptr == pTrainer)
		return;

	CBattler* pPlayer = ctx.Get_Self(g_kBattleSide_Player);
	CBattler* pOpponent = ctx.Get_Self(g_kBattleSide_Opponent);
	if (nullptr == pPlayer || nullptr == pOpponent)
		return;

	const _wstring strTrainerName = pTrainer->szName;
	const _wstring strOpponentPokemon = pOpponent->Get_Instance()->szNickname;
	const _wstring strPlayerPokemon = pPlayer->Get_Instance()->szNickname;

	auto Push = [pSeq](IBattleAction_Step* pStep)
		{
			if (nullptr == pStep)
				return;

			pSeq->Push_Step(pStep);
			Safe_Release(pStep);
		};

	Push(SCamera_PlaySequence::Create(CAMERA_SEQUENCE_ID::INTRO_TRAINER_OPPONENT, false));   // 진입 시 상대 트레이너 클로즈업 (hold)
	Push(SSetPlateVisible::Create(false));   // 인트로 동안 플레이트 숨김
	Push(SDelay::Create(0.2f));
	Push(SBattleText::Create(strTrainerName + TEXT("이(가) 승부를 걸어왔다!")));
	Push(SCloseMsg::Create());
	Push(SDelay::Create(0.2f));

	Push(SBattleText::Create(strTrainerName + TEXT("은(는) ") + strOpponentPokemon + TEXT("을(를) 내보냈다!")));
	Push(SSendOutBall::Create(g_kBattleSide_Opponent));
	Push(SPokemonEnter::Create(g_kBattleSide_Opponent));
	Push(SCloseMsg::Create());
	Push(SDelay::Create(0.2f));

	Push(SBattleText::Create(TEXT("플레이어는 ") + strPlayerPokemon + TEXT("을(를) 내보냈다!")));
	Push(SSendOutBall::Create(g_kBattleSide_Player));
	Push(SPokemonEnter::Create(g_kBattleSide_Player));
	Push(SCloseMsg::Create());
	Push(SDelay::Create(0.2f));

	Push(SRevealTrainers::Create());         // 전역 복귀 시점에 양측 노출
	Push(SSetPlateVisible::Create(true));    // 플레이트 표시
	Push(SDone::Create());

	pSeq->Submit();
}

void CIntroState::Update(const BATTLE_CONTEXT& ctx, _float fTimeDelta)
{
	(void)fTimeDelta;

	if (nullptr == ctx.pManager)
		return;

	if (ctx.pManager->Is_Pacing_Busy())
		return;

	CBattle_ActionSequencer* pSeq = ctx.pManager->Get_Sequencer();
	if (nullptr != pSeq && pSeq->Is_Active())
		return;

	ctx.pManager->Request_State(BATTLE_PHASE::INPUT_PLAYER);
}

void CIntroState::OnExit(const BATTLE_CONTEXT& ctx)
{
	(void)ctx;
}

CIntroState* CIntroState::Create()
{
	return new CIntroState();
}

void CIntroState::Free()
{
	__super::Free();
}
#pragma endregion

#pragma region InputPlayerState
CInputPlayerState::CInputPlayerState()
{
}

void CInputPlayerState::OnEnter(const BATTLE_CONTEXT& ctx)
{
	if (nullptr == ctx.pDispatcher)
		return;

	EVENT_TURN_STARTED tEvent{};
	tEvent.iTurn = (nullptr != ctx.pTurn) ? ctx.pTurn->iTurnNumber : 0;
	ctx.pDispatcher->Publish(tEvent);
}

void CInputPlayerState::Update(const BATTLE_CONTEXT& ctx, _float fTimeDelta)
{
	(void)fTimeDelta;

	if (nullptr == ctx.pManager)
		return;

	// DIK_ESCAPE 는 Director 의 메뉴 Cancel 콜백이 단독 담당.
	//  MAIN 에서 Cancel -> 도망(CRunCommand)
	//  MOVE 에서 Cancel -> MAIN 복귀
	// State 는 큐에 명령이 올라온 것만 감지해 다음 단계로 전이한다.
	CCommandQueue* pQueue = ctx.pManager->Get_Queue();
	if (nullptr == pQueue || pQueue->Empty())
		return;

	ctx.pManager->Request_State(BATTLE_PHASE::INPUT_OPPONENT);
}

void CInputPlayerState::OnExit(const BATTLE_CONTEXT& ctx)
{
	(void)ctx;
}

CInputPlayerState* CInputPlayerState::Create()
{
	return new CInputPlayerState();
}

void CInputPlayerState::Free()
{
	__super::Free();
}
#pragma endregion

#pragma region InputOpponentState
CInputOpponentState::CInputOpponentState()
{
}

void CInputOpponentState::OnEnter(const BATTLE_CONTEXT& ctx)
{
	(void)ctx;
}

void CInputOpponentState::Update(const BATTLE_CONTEXT& ctx, _float fTimeDelta)
{
	(void)fTimeDelta;

	if (nullptr == ctx.pManager || nullptr == ctx.pManager->Get_Queue())
		return;

	IBattleAI* pAI = ctx.pManager->Get_AI(g_kBattleSide_Opponent);
	if (nullptr == pAI)
		return;

	IBattleCommand* pCommand = pAI->Decide(ctx, g_kBattleSide_Opponent);
	if (nullptr == pCommand)
		return;

	if (FAILED(ctx.pManager->Get_Queue()->Push(pCommand)))
	{
		Safe_Release(pCommand);
		return;
	}

	Safe_Release(pCommand);

	if (nullptr != ctx.pDispatcher)
	{
		EVENT_COMMAND_SELECTED tEvent{};
		tEvent.iSide = g_kBattleSide_Opponent;
		ctx.pDispatcher->Publish(tEvent);
	}

	ctx.pManager->Request_State(BATTLE_PHASE::RESOLVE_ORDER);
}

void CInputOpponentState::OnExit(const BATTLE_CONTEXT& ctx)
{
	(void)ctx;
}

CInputOpponentState* CInputOpponentState::Create()
{
	return new CInputOpponentState();
}

void CInputOpponentState::Free()
{
	__super::Free();
}
#pragma endregion

#pragma region ResolveOrderState
CResolveOrderState::CResolveOrderState()
{
}

void CResolveOrderState::OnEnter(const BATTLE_CONTEXT& ctx)
{
	if (nullptr != ctx.pManager && nullptr != ctx.pManager->Get_Queue())
		ctx.pManager->Get_Queue()->Sort(ctx);
}

void CResolveOrderState::Update(const BATTLE_CONTEXT& ctx, _float fTimeDelta)
{
	(void)fTimeDelta;

	if (nullptr == ctx.pManager)
		return;

	ctx.pManager->Request_State(BATTLE_PHASE::RESOLVE_ACTION_1);
}

void CResolveOrderState::OnExit(const BATTLE_CONTEXT& ctx)
{
	(void)ctx;
}

CResolveOrderState* CResolveOrderState::Create()
{
	return new CResolveOrderState();
}

void CResolveOrderState::Free()
{
	__super::Free();
}
#pragma endregion

#pragma region ResolveActionState
CResolveActionState::CResolveActionState()
{
}

void CResolveActionState::OnEnter(const BATTLE_CONTEXT& ctx)
{
	if (nullptr == ctx.pManager) return;
	CCommandQueue* pQueue = ctx.pManager->Get_Queue();
	if (nullptr == pQueue) return;
	m_pCommand = pQueue->Pop();
}

void CResolveActionState::Update(const BATTLE_CONTEXT& ctx, _float fTimeDelta)
{
	(void)fTimeDelta;

	if (nullptr == ctx.pManager)
		return;

	if (nullptr != m_pCommand)
	{
		m_pCommand->Execute(ctx);
		Safe_Release(m_pCommand);
	}

	if (ctx.pManager->Has_Pending_Transition())
		return;

	if (ctx.pManager->Is_Pacing_Busy())
		return;

	// Sequencer 활성 중이면 다음 step 진행 대기
	CBattle_ActionSequencer* pSeq = ctx.pManager->Get_Sequencer();
	if (nullptr != pSeq && pSeq->Is_Active())
		return;

	if (nullptr != ctx.pManager->Get_Queue() && false == ctx.pManager->Get_Queue()->Empty())
	{
		ctx.pManager->Request_State(BATTLE_PHASE::RESOLVE_ACTION_1);
		return;
	}

	ctx.pManager->Request_State(BATTLE_PHASE::RESOLVE_END_TURN);
}

void CResolveActionState::OnExit(const BATTLE_CONTEXT& ctx)
{
	(void)ctx;
	Safe_Release(m_pCommand);
}

CResolveActionState* CResolveActionState::Create()
{
	return new CResolveActionState();
}

void CResolveActionState::Free()
{
	Safe_Release(m_pCommand);
	__super::Free();
}
#pragma endregion

#pragma region ResolveEndTurnState
CResolveEndTurnState::CResolveEndTurnState()
{
}

void CResolveEndTurnState::OnEnter(const BATTLE_CONTEXT& ctx)
{
	for (_uint i = 0; i < g_kBattleSideCount; ++i)
	{
		CBattler* pBattler = ctx.pBattlers[i];
		if (nullptr != pBattler)
			pBattler->Tick_Volatile_Turns();
	}

	if (nullptr != ctx.pTurn)
		++ctx.pTurn->iTurnNumber;
}

void CResolveEndTurnState::Update(const BATTLE_CONTEXT& ctx, _float fTimeDelta)
{
	(void)fTimeDelta;

	if (nullptr == ctx.pManager)
		return;

	if (ctx.pManager->Is_Pacing_Busy())
		return;

	ctx.pManager->Request_State(BATTLE_PHASE::CHECK_END);
}

void CResolveEndTurnState::OnExit(const BATTLE_CONTEXT& ctx)
{
	if (nullptr == ctx.pDispatcher)
		return;

	EVENT_TURN_ENDED tEvent{};
	tEvent.iTurn = (nullptr != ctx.pTurn) ? ctx.pTurn->iTurnNumber : 0;
	ctx.pDispatcher->Publish(tEvent);
}

CResolveEndTurnState* CResolveEndTurnState::Create()
{
	return new CResolveEndTurnState();
}

void CResolveEndTurnState::Free()
{
	__super::Free();
}
#pragma endregion

#pragma region CheckEndState
CCheckEndState::CCheckEndState()
{
}

void CCheckEndState::OnEnter(const BATTLE_CONTEXT& ctx)
{
	(void)ctx;
}

void CCheckEndState::Update(const BATTLE_CONTEXT& ctx, _float fTimeDelta)
{
	(void)fTimeDelta;

	if (nullptr == ctx.pManager)
		return;

	CBattler* pPlayer = ctx.Get_Self(g_kBattleSide_Player);
	CBattler* pOpponent = ctx.Get_Self(g_kBattleSide_Opponent);

	const _bool bPlayerAlive = (nullptr != pPlayer && pPlayer->Is_Alive());
	const _bool bOpponentAlive = (nullptr != pOpponent && pOpponent->Is_Alive());

	if (false == bPlayerAlive && false == bOpponentAlive)
	{
		ctx.pManager->Request_State(BATTLE_PHASE::OUTRO);
		return;
	}

	if (false == bOpponentAlive)
	{
		const _uint iNext = ctx.pManager->Find_FirstAlivePartyIndex(g_kBattleSide_Opponent);
		if (g_kMaxPartySize != iNext)
			ctx.pManager->Request_ForcedSwitch(g_kBattleSide_Opponent);
		else
			ctx.pManager->Request_State(BATTLE_PHASE::OUTRO);

		return;
	}

	if (false == bPlayerAlive)
	{
		const _uint iNext = ctx.pManager->Find_FirstAlivePartyIndex(g_kBattleSide_Player);
		if (g_kMaxPartySize != iNext)
			ctx.pManager->Request_ForcedSwitch(g_kBattleSide_Player);
		else
			ctx.pManager->Request_State(BATTLE_PHASE::OUTRO);

		return;
	}

	ctx.pManager->Request_State(BATTLE_PHASE::INPUT_PLAYER);
}

void CCheckEndState::OnExit(const BATTLE_CONTEXT& ctx)
{
	(void)ctx;
}

CCheckEndState* CCheckEndState::Create()
{
	return new CCheckEndState();
}

void CCheckEndState::Free()
{
	__super::Free();
}
#pragma endregion

#pragma region ForcedSwitchState
// KO 후 강제 교체 등
// - CBattle_Manager::Create_State 의 switch 에 해당 case 없음 (생성 경로 없음).
// - CCheckEndState 도 이 상태로의 분기를 가지지 않음.
// - Get_Phase 가 BATTLE_PHASE::CHECK_END 를 반환하는 것은 임시값
CForcedSwitchState::CForcedSwitchState()
{
}

void CForcedSwitchState::OnEnter(const BATTLE_CONTEXT& ctx)
{
	m_bSubmitted = false;

	if (nullptr == ctx.pManager)
		return;

	const _uint iSide = ctx.pManager->Get_ForcedSwitchSide();
	const _uint iNext = ctx.pManager->Find_FirstAlivePartyIndex(iSide);

	if (g_kMaxPartySize == iNext)
	{
		ctx.pManager->Request_State(BATTLE_PHASE::OUTRO);
		return;
	}

	if (FAILED(ctx.pManager->Replace_BattlerSlot(iSide, iNext)))
	{
		ctx.pManager->Request_State(BATTLE_PHASE::OUTRO);
		return;
	}

	CBattle_ActionSequencer* pSeq = ctx.pManager->Get_Sequencer();
	if (nullptr == pSeq)
	{
		m_bSubmitted = true;
		return;
	}

	CBattler* pBattler = ctx.pManager->Get_Battler(iSide);
	const POKEMON_INSTANCE* pInstance = (nullptr != pBattler) ? pBattler->Get_Instance() : nullptr;
	const _wstring strPokemonName = (nullptr != pInstance)
		? _wstring(pInstance->szNickname) : _wstring(TEXT("?"));

	_wstring strSendOutMsg;
	if (g_kBattleSide_Opponent == iSide)
	{
		const TRAINER_DATA* pTrainer = ctx.pManager->Get_OpponentTrainer();
		const _wstring strTrainerName = (nullptr != pTrainer)
			? _wstring(pTrainer->szName) : _wstring(TEXT("상대"));
		strSendOutMsg = strTrainerName + TEXT("은(는) ") + strPokemonName + TEXT("을(를) 내보냈다!");
	}
	else
	{
		strSendOutMsg = _wstring(TEXT("플레이어는 ")) + strPokemonName + TEXT("을(를) 내보냈다!");
	}

	auto Push = [pSeq](IBattleAction_Step* pStep)
		{
			if (nullptr == pStep)
				return;

			pSeq->Push_Step(pStep);
			Safe_Release(pStep);
		};

	Push(SBattleText::Create(strSendOutMsg));
	Push(SSetPlateVisible::Create(false));

	const BATTLE_ENV& tEnv = ctx.pManager->Get_Env();
	const bool bTrainerRule =
		BATTLE_RULE::TRAINER_SINGLE == tEnv.eRule ||
		BATTLE_RULE::TRAINER_DOUBLE == tEnv.eRule;

	if (bTrainerRule)
		Push(SSendOutBall::Create(iSide));

	Push(SPokemonEnter::Create(iSide));

	if (bTrainerRule)
		Push(SRevealTrainers::Create());

	Push(SSetPlateVisible::Create(true));
	Push(SCloseMsg::Create());
	Push(SDelay::Create(0.2f));
	Push(SDone::Create());
	pSeq->Submit();

	m_bSubmitted = true;
}

void CForcedSwitchState::Update(const BATTLE_CONTEXT& ctx, _float fTimeDelta)
{
	(void)fTimeDelta;

	if (nullptr == ctx.pManager)
		return;

	if (ctx.pManager->Has_Pending_Transition())
		return;

	CBattle_ActionSequencer* pSeq = ctx.pManager->Get_Sequencer();
	if (nullptr != pSeq && pSeq->Is_Active())
		return;

	if (m_bSubmitted)
		ctx.pManager->Request_State(BATTLE_PHASE::INPUT_PLAYER);
	else
		ctx.pManager->Request_State(BATTLE_PHASE::OUTRO);
}

void CForcedSwitchState::OnExit(const BATTLE_CONTEXT& ctx)
{
	(void)ctx;
}

CForcedSwitchState* CForcedSwitchState::Create()
{
	return new CForcedSwitchState();
}

void CForcedSwitchState::Free()
{
	__super::Free();
}
#pragma endregion

#pragma region OutroState
COutroState::COutroState()
{
}

void COutroState::OnEnter(const BATTLE_CONTEXT& ctx)
{
	if (nullptr == ctx.pDispatcher || nullptr == ctx.pManager)
		return;

	CBattler* pPlayer = ctx.Get_Self(g_kBattleSide_Player);
	CBattler* pOpponent = ctx.Get_Self(g_kBattleSide_Opponent);

	const _bool bPlayerAlive = (nullptr != pPlayer && pPlayer->Is_Alive());
	const _bool bOpponentAlive = (nullptr != pOpponent && pOpponent->Is_Alive());

	EVENT_BATTLE_ENDED tEvent{};

	if (bPlayerAlive && false == bOpponentAlive)
		tEvent.iWinnerSide = g_kBattleSide_Player;
	else if (false == bPlayerAlive && bOpponentAlive)
		tEvent.iWinnerSide = g_kBattleSide_Opponent;
	else
		tEvent.iWinnerSide = g_kBattleSide_Player;  // 도망/Esc 등 무승부적 종료 - 임시로 Player 표기

	ctx.pDispatcher->Publish(tEvent);

	const BATTLE_ENV& tEnv = ctx.pManager->Get_Env();
	const bool bTrainerRule =
		BATTLE_RULE::TRAINER_SINGLE == tEnv.eRule ||
		BATTLE_RULE::TRAINER_DOUBLE == tEnv.eRule;

	if (false == bTrainerRule)
		return;

	CBattle_ActionSequencer* pSeq = ctx.pManager->Get_Sequencer();
	if (nullptr == pSeq)
		return;

	const TRAINER_DATA* pTrainer = ctx.pManager->Get_OpponentTrainer();
	const _wstring strTrainerName = (nullptr != pTrainer)
		? _wstring(pTrainer->szName) : _wstring(TEXT("상대"));

	auto Push = [pSeq](IBattleAction_Step* pStep)
		{
			if (nullptr == pStep)
				return;

			pSeq->Push_Step(pStep);
			Safe_Release(pStep);
		};

	const _uint iLoserSide =
		(g_kBattleSide_Player == tEvent.iWinnerSide)
		? g_kBattleSide_Opponent
		: g_kBattleSide_Player;

	if (CBattle_Trainer* pLoser =
		dynamic_cast<CBattle_Trainer*>(ctx.pManager->Get_TrainerObj(iLoserSide)))
	{
		pLoser->Play_Faint();
	}

	if (g_kBattleSide_Player == tEvent.iWinnerSide)
	{
		Push(SBattleText::Create(strTrainerName + TEXT("과(와)의 승부에서 이겼다!")));
		Push(SCloseMsg::Create());
		Push(SDelay::Create(0.2f));

		if (nullptr != pTrainer && 0 != pTrainer->szDefeatDialog[0])
		{
			Push(SBattleText::Create(_wstring(pTrainer->szDefeatDialog)));
			Push(SCloseMsg::Create());
			Push(SDelay::Create(0.2f));
		}

		const _uint iReward = (nullptr != pTrainer) ? pTrainer->iRewardMoney : 0;
		Push(SPrizeMoney::Create(iReward));
		Push(SCloseMsg::Create());
		Push(SDelay::Create(0.2f));
	}
	else
	{
		Push(SBattleText::Create(_wstring(TEXT("눈앞이 캄캄해졌다..."))));
		Push(SCloseMsg::Create());
		Push(SDelay::Create(0.2f));
	}

	Push(SDone::Create());
	pSeq->Submit();
}

void COutroState::Update(const BATTLE_CONTEXT& ctx, _float fTimeDelta)
{
	(void)fTimeDelta;

	if (nullptr == ctx.pManager)
		return;

	if (ctx.pManager->Is_Pacing_Busy())
		return;

	CBattle_ActionSequencer* pSeq = ctx.pManager->Get_Sequencer();
	if (nullptr != pSeq && pSeq->Is_Active())
		return;

	ctx.pManager->Request_Exit();
}

void COutroState::OnExit(const BATTLE_CONTEXT& ctx)
{
	(void)ctx;
}

COutroState* COutroState::Create()
{
	return new COutroState();
}

void COutroState::Free()
{
	__super::Free();
}
#pragma endregion

#pragma region DoneState
CDoneState::CDoneState()
{
}

void CDoneState::OnEnter(const BATTLE_CONTEXT& ctx)
{
	(void)ctx;
}

void CDoneState::Update(const BATTLE_CONTEXT& ctx, _float fTimeDelta)
{
	(void)ctx;
	(void)fTimeDelta;
}

void CDoneState::OnExit(const BATTLE_CONTEXT& ctx)
{
	(void)ctx;
}

CDoneState* CDoneState::Create()
{
	return new CDoneState();
}

void CDoneState::Free()
{
	__super::Free();
}
#pragma endregion