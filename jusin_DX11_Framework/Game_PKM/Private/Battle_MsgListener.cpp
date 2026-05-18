#include "Battle_MsgListener.h"
#include "Battle_Manager.h"
#include "BattleMsg.h"
#include "Battler.h"
#include "PokemonData_Manager.h"

CBattle_MsgListener::CBattle_MsgListener()
{
}

HRESULT CBattle_MsgListener::Initialize()
{
	return S_OK;
}

void CBattle_MsgListener::Bind(CBattle_Manager* pManager, CBattleMsg* pMsg)
{
	m_pManager = pManager;
	m_pMsg = pMsg;
}

_wstring CBattle_MsgListener::Get_BattlerName(_uint iSide) const
{
	if (nullptr == m_pManager)
		return TEXT("?");

	CBattler* pBattler = m_pManager->Get_Battler(iSide);
	if (nullptr == pBattler || nullptr == pBattler->Get_Instance())
		return TEXT("?");

	return pBattler->Get_Instance()->szNickname;
}

_wstring CBattle_MsgListener::Get_MoveName(_uint iMoveID) const
{
	auto* pDataMgr = CPokemonData_Manager::GetInstance();
	if (nullptr == pDataMgr)
		return TEXT("?");

	const MOVE_DATA* pMove = pDataMgr->Find_Move(iMoveID);
	return (nullptr != pMove) ? _wstring(pMove->szName) : TEXT("?");
}

void CBattle_MsgListener::On_BattleStarted(const EVENT_BATTLE_STARTED& tEvent)
{
	if (BATTLE_RULE::TRAINER_SINGLE == tEvent.tEnv.eRule ||
		BATTLE_RULE::TRAINER_DOUBLE == tEvent.tEnv.eRule)
		return;

	m_qMessages.push(_wstring(TEXT("야생 ")) + Get_BattlerName(g_kBattleSide_Opponent) + TEXT("이(가) 나타났다!"));
}

void CBattle_MsgListener::On_MoveUsed(const EVENT_MOVE_USED& tEvent)
{
	m_qMessages.push(Get_BattlerName(tEvent.iSide) + TEXT("의 ") + Get_MoveName(tEvent.iMoveID) +
		TEXT("!"));
}

void CBattle_MsgListener::On_MoveFailed(const EVENT_MOVE_FAILED& tEvent)
{
	switch (tEvent.eReason)
	{
	case MOVE_FAIL_REASON::NO_PP:
		m_qMessages.push(TEXT("그러나 기술을 쓸 수 없었다!"));
		break;

	case MOVE_FAIL_REASON::MISSED:
		m_qMessages.push(TEXT("그러나 빗나갔다!"));
		break;

	case MOVE_FAIL_REASON::IMMUNE:
		m_qMessages.push(TEXT("효과가 없는 것 같다..."));
		break;

	default:
		m_qMessages.push(TEXT("기술이 실패했다!"));
		break;
	}
}

void CBattle_MsgListener::On_DamageDealt(const EVENT_DAMAGE_DEALT& tEvent)
{
	if (tEvent.bCrit)
		m_qMessages.push(TEXT("급소에 맞았다!"));

	if (tEvent.fEffectiveness > 1.f)
		m_qMessages.push(TEXT("효과가 굉장했다!"));
	else if (tEvent.fEffectiveness > 0.f && tEvent.fEffectiveness < 1.f)
		m_qMessages.push(TEXT("효과가 별로인 듯하다..."));
	// effectiveness == 1.0 은 메시지 없음. 0.0 (IMMUNE) 은 On_MoveFailed 경로에서 처리됨.
}

void CBattle_MsgListener::On_PokemonFainted(const EVENT_POKEMON_FAINTED& tEvent)
{
	m_qMessages.push(Get_BattlerName(tEvent.iSide) + TEXT("은(는) 쓰러졌다!"));
}

void CBattle_MsgListener::On_RunFailed(const EVENT_RUN_FAILED& tEvent)
{
	(void)tEvent;
	m_qMessages.push(TEXT("도망갈 수 없었다!"));
}

void CBattle_MsgListener::On_RunSucceeded(const EVENT_RUN_SUCCEEDED& tEvent)
{
	(void)tEvent;
	m_qMessages.push(TEXT("도망쳤다!"));
}

void CBattle_MsgListener::On_BattleEnded(const EVENT_BATTLE_ENDED& tEvent)
{
	if (tEvent.iWinnerSide == g_kBattleSide_Player)
		m_qMessages.push(_wstring(TEXT("야생 ")) + Get_BattlerName(g_kBattleSide_Opponent) + TEXT("을(를) 쓰러뜨렸다!"));
	else
		m_qMessages.push(TEXT("눈앞이 캄캄해졌다..."));
}

void CBattle_MsgListener::Tick(_float fTimeDelta)
{
	if (nullptr == m_pManager || nullptr == m_pMsg)
		return;

	const _bool bTyping = m_pMsg->Is_Open() && false == m_pMsg->Is_Done();
	const _bool bFinishedDisplay = m_pMsg->Is_Open() && m_pMsg->Is_Done();

	const _bool bHasListenerWork =
		m_bLockHeld || m_bWaiting || false == m_qMessages.empty();

	if (false == bHasListenerWork && m_pMsg->Is_Open())
		return;

	// 1) 타이핑 완료 시점 감지 — 현재 메시지에 대해 대기가 아직이면 타이머 시작
	if (bFinishedDisplay && false == m_bWaiting && false == m_bMessageWaitConsumed)
	{
		m_bWaiting = true;
		m_fWaitTimer = m_fInterMessageDelay;
	}

	// 2) 대기 진행
	if (m_bWaiting)
	{
		m_fWaitTimer -= fTimeDelta;
		if (m_fWaitTimer <= 0.f)
		{
			m_bWaiting = false;
			m_fWaitTimer = 0.f;
			m_bMessageWaitConsumed = true;
		}
	}

	// 3) busy 판정 — 타이핑 중 / 대기 중 / 큐에 다음 메시지 있음 중 하나라도 해당하면 락 유지
	const _bool bBusy = bTyping || m_bWaiting || (false == m_qMessages.empty());

	if (bBusy && false == m_bLockHeld)
	{
		m_pManager->Add_Pacing_Lock();
		m_bLockHeld = true;
	}
	else if (false == bBusy && m_bLockHeld)
	{
		m_pManager->Release_Pacing_Lock();
		m_bLockHeld = false;
	}

	// 4) 다음 메시지 시작 — 타이핑 중도 아니고 대기 중도 아니며 큐에 메시지가 있을 때
	const _bool bCanStartNext = (false == bTyping) && (false == m_bWaiting) && (false == m_qMessages.empty());
	if (bCanStartNext)
	{
		_wstring strNext = m_qMessages.front();
		m_qMessages.pop();

		m_pMsg->Set_Message(strNext);
		m_pMsg->Open();

		m_bMessageWaitConsumed = false; // 새 메시지 시작 — 대기 플래그 리셋
	}

	// 5) 현재 메시지의 표시·대기 모두 끝났고 큐가 비면 박스 닫기
	if (m_bMessageWaitConsumed && m_qMessages.empty() && m_pMsg->Is_Open())
	{
		m_pMsg->Close();
	}
}

CBattle_MsgListener* CBattle_MsgListener::Create()
{
	CBattle_MsgListener* pInstance = new CBattle_MsgListener();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CBattle_MsgListener");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBattle_MsgListener::Free()
{
	if (m_bLockHeld && nullptr != m_pManager)
	{
		m_pManager->Release_Pacing_Lock();
		m_bLockHeld = false;
	}

	m_pManager = nullptr;
	m_pMsg = nullptr;

	__super::Free();
}