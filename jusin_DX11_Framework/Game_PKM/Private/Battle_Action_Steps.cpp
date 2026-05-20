#include "Battle_Action_Steps.h"
#include "BattleMsg.h"
#include "Battle_EventDispatcher.h"
#include "Battle_Manager.h"
#include "Battle_ActionSequencer.h"
#include "Battler.h"
#include "PokemonData_Manager.h"
#include "Battle_Math.h"
#include "Damage_Calculator.h"
#include "Battle_Pokemon.h"
#include "Battle_Trainer.h"
#include "Player_Status.h"

#pragma region SDelay
SDelay::SDelay()
{
}

HRESULT SDelay::Initialize(_float fDuration)
{
	m_fDuration = (fDuration > 0.f) ? fDuration : 0.f;
	return S_OK;
}

void SDelay::OnEnter(const BATTLE_CONTEXT& ctx)
{
	(void)ctx;
	m_fElapsed = 0.f;
}

void SDelay::Update(const BATTLE_CONTEXT& ctx, _float fTimeDelta)
{
	(void)ctx;
	m_fElapsed += fTimeDelta;
}

_bool SDelay::Is_Complete(const BATTLE_CONTEXT& ctx) const
{
	(void)ctx;
	return m_fElapsed >= m_fDuration;
}

SDelay* SDelay::Create(_float fDuration)
{
	SDelay* pInstance = new SDelay();

	if (FAILED(pInstance->Initialize(fDuration)))
	{
		MSG_BOX("Failed to Created : SDelay");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void SDelay::Free()
{
	__super::Free();
}
#pragma endregion

#pragma region SCloseMsg
SCloseMsg::SCloseMsg()
{
}

void SCloseMsg::OnEnter(const BATTLE_CONTEXT& ctx)
{
	if (nullptr != ctx.pMsg && ctx.pMsg->Is_Open())
		ctx.pMsg->Close();
}

void SCloseMsg::Update(const BATTLE_CONTEXT& ctx, _float fTimeDelta)
{
	(void)ctx;
	(void)fTimeDelta;
}

_bool SCloseMsg::Is_Complete(const BATTLE_CONTEXT& ctx) const
{
	(void)ctx;
	return true;
}

SCloseMsg* SCloseMsg::Create()
{
	return new SCloseMsg();
}

void SCloseMsg::Free()
{
	__super::Free();
}
#pragma endregion

#pragma region SBattleText
SBattleText::SBattleText()
{
}

HRESULT SBattleText::Initialize(const _wstring& strText, _float fHoldSeconds)
{
	m_strText = strText;
	m_fHoldSeconds = (fHoldSeconds > 0.f) ? fHoldSeconds : 0.f;
	m_fHoldTimer = 0.f;
	m_bOpened = false;

	return S_OK;
}

void SBattleText::OnEnter(const BATTLE_CONTEXT& ctx)
{
	m_fHoldTimer = 0.f;
	m_bOpened = false;

	if (nullptr == ctx.pMsg)
		return;

	ctx.pMsg->Set_Message(m_strText);
	ctx.pMsg->Open();
	m_bOpened = true;
}

void SBattleText::Update(const BATTLE_CONTEXT& ctx, _float fTimeDelta)
{
	if (false == m_bOpened || nullptr == ctx.pMsg)
		return;

	m_fHoldTimer += fTimeDelta;

	const _float fTypeSeconds =
		static_cast<_float>(m_strText.length()) / 45.f;

	if (m_fHoldTimer >= fTypeSeconds)
		ctx.pMsg->Complete();
}

_bool SBattleText::Is_Complete(const BATTLE_CONTEXT& ctx) const
{
	(void)ctx;

	if (false == m_bOpened)
		return true;

	const _float fTypeSeconds =
		static_cast<_float>(m_strText.length()) / 45.f;

	return m_fHoldTimer >= (fTypeSeconds + m_fHoldSeconds);
}

SBattleText* SBattleText::Create(const _wstring& strText, _float fHoldSeconds)
{
	SBattleText* pInstance = new SBattleText();

	if (FAILED(pInstance->Initialize(strText, fHoldSeconds)))
	{
		MSG_BOX("Failed to Created : SBattleText");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void SBattleText::Free()
{
	__super::Free();
}
#pragma endregion

#pragma region STrainerThrow
STrainerThrow::STrainerThrow()
{
}

HRESULT STrainerThrow::Initialize(_uint iSide, _float fDuration)
{
	if (iSide >= g_kBattleSideCount)
		return E_FAIL;

	m_iSide = iSide;
	m_fDuration = (fDuration > 0.f) ? fDuration : 0.f;
	m_fElapsed = 0.f;

	return S_OK;
}

void STrainerThrow::OnEnter(const BATTLE_CONTEXT& ctx)
{
	m_fElapsed = 0.f;

	if (nullptr == ctx.pManager)
		return;

	CBattle_Trainer* pTrainer =
		dynamic_cast<CBattle_Trainer*>(ctx.pManager->Get_TrainerObj(m_iSide));

	if (nullptr != pTrainer)
		pTrainer->Play_Throw();
}

void STrainerThrow::Update(const BATTLE_CONTEXT& ctx, _float fTimeDelta)
{
	(void)ctx;
	m_fElapsed += fTimeDelta;
}

_bool STrainerThrow::Is_Complete(const BATTLE_CONTEXT& ctx) const
{
	(void)ctx;
	return m_fElapsed >= m_fDuration;
}

STrainerThrow* STrainerThrow::Create(_uint iSide, _float fDuration)
{
	STrainerThrow* pInstance = new STrainerThrow();

	if (FAILED(pInstance->Initialize(iSide, fDuration)))
	{
		MSG_BOX("Failed to Created : STrainerThrow");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void STrainerThrow::Free()
{
	__super::Free();
}
#pragma endregion

#pragma region SPokemonEnter
SPokemonEnter::SPokemonEnter()
{
}

HRESULT SPokemonEnter::Initialize(_uint iSide)
{
	if (iSide >= g_kBattleSideCount)
		return E_FAIL;

	m_iSide = iSide;
	m_fGrace = 0.f;

	return S_OK;
}

void SPokemonEnter::OnEnter(const BATTLE_CONTEXT& ctx)
{
	m_fGrace = 0.f;

	if (nullptr == ctx.pManager)
		return;

	CBattle_Pokemon* pPokemon =
		dynamic_cast<CBattle_Pokemon*>(ctx.pManager->Get_BattlerObj(m_iSide));

	if (nullptr != pPokemon)
		pPokemon->Begin_SendOutAppear();
}

void SPokemonEnter::Update(const BATTLE_CONTEXT& ctx, _float fTimeDelta)
{
	(void)ctx;
	m_fGrace += fTimeDelta;
}

_bool SPokemonEnter::Is_Complete(const BATTLE_CONTEXT& ctx) const
{
	(void)ctx;

	return m_fGrace >= 2.f;
}

SPokemonEnter* SPokemonEnter::Create(_uint iSide)
{
	SPokemonEnter* pInstance = new SPokemonEnter();

	if (FAILED(pInstance->Initialize(iSide)))
	{
		MSG_BOX("Failed to Created : SPokemonEnter");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void SPokemonEnter::Free()
{
	__super::Free();
}
#pragma endregion

#pragma region SDone
SDone::SDone()
{
}

void SDone::OnEnter(const BATTLE_CONTEXT& ctx)
{
	(void)ctx;
}

void SDone::Update(const BATTLE_CONTEXT& ctx, _float fTimeDelta)
{
	(void)ctx;
	(void)fTimeDelta;
}

_bool SDone::Is_Complete(const BATTLE_CONTEXT& ctx) const
{
	(void)ctx;
	return true;
}

SDone* SDone::Create()
{
	return new SDone();
}

void SDone::Free()
{
	__super::Free();
}
#pragma endregion

#pragma region SAnnounce
SAnnounce::SAnnounce()
{
}

HRESULT SAnnounce::Initialize(_uint iActorSide, _uint iMoveID)
{
	m_iActorSide = iActorSide;
	m_iMoveID = iMoveID;
	return S_OK;
}

void SAnnounce::OnEnter(const BATTLE_CONTEXT& ctx)
{
	m_fGrace = 0.f;

	if (nullptr == ctx.pDispatcher)
		return;

	EVENT_MOVE_USED tEvent{};
	tEvent.iSide = m_iActorSide;
	tEvent.iMoveID = m_iMoveID;
	ctx.pDispatcher->Publish(tEvent);
}

void SAnnounce::Update(const BATTLE_CONTEXT& ctx, _float fTimeDelta)
{
	(void)ctx;
	m_fGrace += fTimeDelta;
}

_bool SAnnounce::Is_Complete(const BATTLE_CONTEXT& ctx) const
{
	if (m_fGrace < 0.05f)
		return false;

	if (nullptr != ctx.pMsg && ctx.pMsg->Is_Open())
		return false;

	return true;
}

SAnnounce* SAnnounce::Create(_uint iActorSide, _uint iMoveID)
{
	SAnnounce* pInstance = new SAnnounce();

	if (FAILED(pInstance->Initialize(iActorSide, iMoveID)))
	{
		MSG_BOX("Failed to Created : SAnnounce");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void SAnnounce::Free()
{
	__super::Free();
}
#pragma endregion

#pragma region SMissMessage
SMissMessage::SMissMessage()
{
}

HRESULT SMissMessage::Initialize(_uint iActorSide, _uint iMoveID)
{
	m_iActorSide = iActorSide;
	m_iMoveID = iMoveID;
	return S_OK;
}

void SMissMessage::OnEnter(const BATTLE_CONTEXT& ctx)
{
	m_fGrace = 0.f;
	m_bPublished = false;

	// 명중했으면 메시지 발행 안 함 (즉시 완료)
	if (nullptr != ctx.pManager)
	{
		CBattle_ActionSequencer* pSeq = ctx.pManager->Get_Sequencer();
		if (nullptr != pSeq && true == pSeq->Get_ActionData().bAccuracyHit)
			return;
	}

	if (nullptr == ctx.pDispatcher)
		return;

	EVENT_MOVE_FAILED tEvent{};
	tEvent.iSide = m_iActorSide;
	tEvent.iMoveID = m_iMoveID;
	tEvent.eReason = MOVE_FAIL_REASON::MISSED;
	ctx.pDispatcher->Publish(tEvent);
	m_bPublished = true;
}

void SMissMessage::Update(const BATTLE_CONTEXT& ctx, _float fTimeDelta)
{
	(void)ctx;
	m_fGrace += fTimeDelta;
}

_bool SMissMessage::Is_Complete(const BATTLE_CONTEXT& ctx) const
{
	if (false == m_bPublished)
		return true;  // 명중 -> 메시지 발행 안 함 -> 즉시 완료

	if (m_fGrace < 0.05f)
		return false;

	if (nullptr != ctx.pMsg && ctx.pMsg->Is_Open())
		return false;

	return true;
}

SMissMessage* SMissMessage::Create(_uint iActorSide, _uint iMoveID)
{
	SMissMessage* pInstance = new SMissMessage();

	if (FAILED(pInstance->Initialize(iActorSide, iMoveID)))
	{
		MSG_BOX("Failed to Created : SMissMessage");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void SMissMessage::Free()
{
	__super::Free();
}
#pragma endregion

#pragma region SResultMessages
SResultMessages::SResultMessages()
{
}

void SResultMessages::OnEnter(const BATTLE_CONTEXT& ctx)
{
	(void)ctx;
	m_fGrace = 0.f;
}

void SResultMessages::Update(const BATTLE_CONTEXT& ctx, _float fTimeDelta)
{
	(void)ctx;
	m_fGrace += fTimeDelta;
}

_bool SResultMessages::Is_Complete(const BATTLE_CONTEXT& ctx) const
{
	if (m_fGrace < 0.05f)
		return false;

	if (nullptr != ctx.pMsg && ctx.pMsg->Is_Open())
		return false;

	return true;
}

SResultMessages* SResultMessages::Create()
{
	return new SResultMessages();
}

void SResultMessages::Free()
{
	__super::Free();
}
#pragma endregion

#pragma region SAccuracyCheck
SAccuracyCheck::SAccuracyCheck()
{
}

void SAccuracyCheck::OnEnter(const BATTLE_CONTEXT& ctx)
{
	if (nullptr == ctx.pManager)
		return;

	CBattle_ActionSequencer* pSeq = ctx.pManager->Get_Sequencer();
	if (nullptr == pSeq)
		return;

	BATTLE_ACTION_DATA& tData = pSeq->Get_ActionData();

	if (nullptr == ctx.pDataMgr)
	{
		tData.bAccuracyHit = false;
		return;
	}

	const MOVE_DATA* pMove = ctx.pDataMgr->Find_Move(tData.iMoveID);
	if (nullptr == pMove)
	{
		tData.bAccuracyHit = false;
		return;
	}

	CBattler* pAttacker = ctx.Get_Self(tData.iActorSide);
	CBattler* pDefender = ctx.Get_Self(tData.iTargetSide);

	tData.bAccuracyHit = BattleMath::Roll_Accuracy(pMove->iAccuracy, pAttacker, pDefender);
}

void SAccuracyCheck::Update(const BATTLE_CONTEXT& ctx, _float fTimeDelta)
{
	(void)ctx;
	(void)fTimeDelta;
}

_bool SAccuracyCheck::Is_Complete(const BATTLE_CONTEXT& ctx) const
{
	(void)ctx;
	return true;
}

SAccuracyCheck* SAccuracyCheck::Create()
{
	return new SAccuracyCheck();
}

void SAccuracyCheck::Free()
{
	__super::Free();
}
#pragma endregion

#pragma region SApplyDamage
SApplyDamage::SApplyDamage()
{
}

void SApplyDamage::OnEnter(const BATTLE_CONTEXT& ctx)
{
	if (nullptr == ctx.pManager)
		return;

	CBattle_ActionSequencer* pSeq = ctx.pManager->Get_Sequencer();
	if (nullptr == pSeq)
		return;

	BATTLE_ACTION_DATA& tData = pSeq->Get_ActionData();

	if (false == tData.bAccuracyHit)
		return;

	if (nullptr == ctx.pDataMgr)
		return;

	const MOVE_DATA* pMove = ctx.pDataMgr->Find_Move(tData.iMoveID);
	if (nullptr == pMove)
		return;

	if (MOVE_CATEGORY::STATUS == pMove->eCategory || 0 == pMove->iPower)
		return;

	CBattler* pAttacker = ctx.Get_Self(tData.iActorSide);
	CBattler* pDefender = ctx.Get_Self(tData.iTargetSide);

	if (nullptr == pAttacker || nullptr == pDefender)
		return;

	if (false == pDefender->Is_Alive())
		return;

	POKEMON_INSTANCE* pAtkInst = pAttacker->Get_Instance();
	POKEMON_INSTANCE* pDefInst = pDefender->Get_Instance();
	if (nullptr == pAtkInst || nullptr == pDefInst)
		return;

	CDamage_Calculator* pCalc = ctx.pManager->Get_Damage_Calculator();
	if (nullptr == pCalc)
		return;

	DAMAGE_PIPE_DATA& tPipe = tData.tPipe;
	tPipe = {};
	tPipe.pAttacker = pAttacker;
	tPipe.pDefender = pDefender;
	tPipe.pMove = pMove;
	tPipe.pField = ctx.pField;
	tPipe.iBasePower = pMove->iPower;
	tPipe.iAttackStat = BattleMath::Pick_AttackStat(*pAtkInst, pMove->eCategory);
	tPipe.iDefenseStat = BattleMath::Pick_DefenseStat(*pDefInst, pMove->eCategory);
	tPipe.iAttackerLevel = (0 == pAtkInst->iLevel) ? 1 : pAtkInst->iLevel;

	pCalc->Calculate(ctx, tPipe);

	// 면역 분기 - 데미지 미적용 + IMMUNE 메시지 발행
	if (tPipe.fEffectiveness <= 0.f)
	{
		if (nullptr != ctx.pDispatcher)
		{
			EVENT_MOVE_FAILED tEvent{};
			tEvent.iSide = tData.iActorSide;
			tEvent.iMoveID = tData.iMoveID;
			tEvent.eReason = MOVE_FAIL_REASON::IMMUNE;
			ctx.pDispatcher->Publish(tEvent);
		}
		return;
	}

	const _bool bWasAlive = pDefender->Is_Alive();
	tData.iAppliedDamage = pDefender->Apply_Damage(tPipe.iFinalDamage);
	tData.bFaintedThisHit = bWasAlive && (false == pDefender->Is_Alive());

	if (nullptr != ctx.pDispatcher)
	{
		EVENT_DAMAGE_DEALT tEvent{};
		tEvent.iTargetSide = pDefender->Get_Side();
		tEvent.iAmount = tData.iAppliedDamage;
		tEvent.eSource = DAMAGE_SOURCE::MOVE;
		tEvent.iMoveID = tData.iMoveID;
		tEvent.fEffectiveness = tPipe.fEffectiveness;
		tEvent.bCrit = tPipe.bCrit;
		ctx.pDispatcher->Publish(tEvent);
	}
}

void SApplyDamage::Update(const BATTLE_CONTEXT& ctx, _float fTimeDelta)
{
	(void)ctx;
	(void)fTimeDelta;
}

_bool SApplyDamage::Is_Complete(const BATTLE_CONTEXT& ctx) const
{
	(void)ctx;
	return true;
}

SApplyDamage* SApplyDamage::Create()
{
	return new SApplyDamage();
}

void SApplyDamage::Free()
{
	__super::Free();
}
#pragma endregion

#pragma region SFaintCheck
SFaintCheck::SFaintCheck()
{
}

void SFaintCheck::OnEnter(const BATTLE_CONTEXT& ctx)
{
	m_fGrace = 0.f;
	m_bPublished = false;

	if (nullptr == ctx.pManager)
		return;

	CBattle_ActionSequencer* pSeq = ctx.pManager->Get_Sequencer();
	if (nullptr == pSeq)
		return;

	BATTLE_ACTION_DATA& tData = pSeq->Get_ActionData();

	if (false == tData.bFaintedThisHit)
		return;

	if (nullptr != ctx.pDispatcher)
	{
		EVENT_POKEMON_FAINTED tEvent{};
		tEvent.iSide = tData.iTargetSide;
		ctx.pDispatcher->Publish(tEvent);
		m_bPublished = true;
	}
}

void SFaintCheck::Update(const BATTLE_CONTEXT& ctx, _float fTimeDelta)
{
	(void)ctx;
	m_fGrace += fTimeDelta;
}

_bool SFaintCheck::Is_Complete(const BATTLE_CONTEXT& ctx) const
{
	if (false == m_bPublished)
		return true;

	if (m_fGrace < 0.05f)
		return false;

	if (nullptr != ctx.pMsg && ctx.pMsg->Is_Open())
		return false;

	return true;
}

SFaintCheck* SFaintCheck::Create()
{
	return new SFaintCheck();
}

void SFaintCheck::Free()
{
	__super::Free();
}
#pragma endregion

#pragma region SPrizeMoney
SPrizeMoney::SPrizeMoney()
{
}

HRESULT SPrizeMoney::Initialize(_uint iAmount, _float fHoldSeconds)
{
	m_iAmount = iAmount;
	m_fHoldSeconds = (fHoldSeconds > 0.f) ? fHoldSeconds : 0.f;
	m_fHoldTimer = 0.f;
	m_bOpened = false;
	m_bApplied = false;

	m_strText = _wstring(TEXT("플레이어는 상금으로 ")) + to_wstring(m_iAmount) + TEXT("원을 손에 넣었다!");

		return S_OK;
}

void SPrizeMoney::OnEnter(const BATTLE_CONTEXT& ctx)
{
	m_fHoldTimer = 0.f;
	m_bOpened = false;

	if (false == m_bApplied && nullptr != ctx.pManager)
	{
		CPlayer_Status* pState = ctx.pManager->Get_PlayerState();
		if (nullptr != pState)
		{
			pState->Set_Money(pState->Get_Money() + m_iAmount);
			m_bApplied = true;
		}
	}

	if (nullptr == ctx.pMsg)
		return;

	ctx.pMsg->Set_Message(m_strText);
	ctx.pMsg->Open();
	m_bOpened = true;
}

void SPrizeMoney::Update(const BATTLE_CONTEXT& ctx, _float fTimeDelta)
{
	if (false == m_bOpened || nullptr == ctx.pMsg)
		return;

	m_fHoldTimer += fTimeDelta;

	const _float fTypeSeconds =
		static_cast<_float>(m_strText.length()) / 45.f;

	if (m_fHoldTimer >= fTypeSeconds)
		ctx.pMsg->Complete();
}

_bool SPrizeMoney::Is_Complete(const BATTLE_CONTEXT& ctx) const
{
	(void)ctx;

	if (false == m_bOpened)
		return true;

	const _float fTypeSeconds =
		static_cast<_float>(m_strText.length()) / 45.f;

	return m_fHoldTimer >= (fTypeSeconds + m_fHoldSeconds);
}

SPrizeMoney* SPrizeMoney::Create(_uint iAmount, _float fHoldSeconds)
{
	SPrizeMoney* pInstance = new SPrizeMoney();

	if (FAILED(pInstance->Initialize(iAmount, fHoldSeconds)))
	{
		MSG_BOX("Failed to Created : SPrizeMoney");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void SPrizeMoney::Free()
{
	__super::Free();
}
#pragma endregion