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
#include "Effect_Manager.h"
#include "MonsterBall.h"
#include "Camera_Director.h"
#include "GameInstance.h"
#include "Game_PKM_Tags.h"

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

#pragma region SSendOutBall
SSendOutBall::SSendOutBall()
{
}

HRESULT SSendOutBall::Initialize(_uint iSide, _float fFlightDuration)
{
	if (iSide >= g_kBattleSideCount)
		return E_FAIL;

	m_iSide = iSide;
	m_fFlightDuration = (fFlightDuration > 0.f) ? fFlightDuration : 0.72f;
	m_fElapsed = 0.f;
	m_bFinished = false;
	m_vTargetPos = {};
	m_pBall = nullptr;

	return S_OK;
}

void SSendOutBall::OnEnter(const BATTLE_CONTEXT& ctx)
{
	m_fElapsed = 0.f;
	m_bFinished = false;
	m_pBall = nullptr;
	m_vTargetPos = {};

	if (nullptr == ctx.pManager)
	{
		m_bFinished = true;
		return;
	}

	// 송출 클로즈업 동안 양측 트레이너 숨김. 카메라 컷과 동일 틱에 처리해야 글리치가 없다.
	// 노출은 SRevealTrainers(줌아웃 settle)가 전담한다.
	for (_uint i = 0; i < g_kBattleSideCount; ++i)
	{
		CBattle_Trainer* pTrainer =
			dynamic_cast<CBattle_Trainer*>(ctx.pManager->Get_TrainerObj(i));
		if (nullptr != pTrainer)
			pTrainer->Set_BattleVisible(false);
	}

	const CAMERA_SEQUENCE_ID eSequence =
		(g_kBattleSide_Player == m_iSide)
		? CAMERA_SEQUENCE_ID::SENDOUT_PLAYER
		: CAMERA_SEQUENCE_ID::SENDOUT_OPPONENT;

	CCamera_Director::GetInstance()->Play_Sequence(eSequence);

	CBattle_Pokemon* pPokemon =
		dynamic_cast<CBattle_Pokemon*>(ctx.pManager->Get_BattlerObj(m_iSide));

	if (nullptr == pPokemon)
	{
		m_bFinished = true;
		return;
	}

	m_vTargetPos = pPokemon->Get_EffectPivot();

	const _float fSideSign =
		(g_kBattleSide_Player == m_iSide) ? -1.f : 1.f;

	const _float3 vStartPos = _float3(
		m_vTargetPos.x + 0.85f * fSideSign,
		m_vTargetPos.y + 0.65f,
		m_vTargetPos.z + 1.35f * fSideSign);

	CMonsterBall::MONSTER_BALL_DESC BallDesc{};
	BallDesc.vSpawnPos = m_vTargetPos;
	BallDesc.vTargetPos = m_vTargetPos;
	BallDesc.fFlightDuration = m_fFlightDuration;
	BallDesc.fArcHeight = 0.45f;
	BallDesc.fImpactDuration = 0.5f;
	BallDesc.fRotationPerSec = XMConvertToRadians(720.f);

	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	CMonsterBall* pBall = static_cast<CMonsterBall*>(
		pGameInstance->Clone_Prototype(
			PROTOTYPE::GAMEOBJECT,
			ETOUI(LEVEL::GAMEPLAY),
			PROTO_OBJ_MONSTER_BALL,
			&BallDesc));

	if (nullptr == pBall)
	{
		m_bFinished = true;
		return;
	}

	if (FAILED(pGameInstance->Add_GameObject_Ex(
		ETOUI(LEVEL::BATTLE),
		LAYER_INTERACTABLE,
		pBall)))
	{
		Safe_Release(pBall);
		m_bFinished = true;
		return;
	}

	m_pBall = pBall;
	m_pBall->Reset();
	m_pBall->Play_BattleOpen(m_vTargetPos, vStartPos);
}

void SSendOutBall::Update(const BATTLE_CONTEXT& ctx, _float fTimeDelta)
{
	(void)ctx;

	if (m_bFinished)
		return;

	m_fElapsed += fTimeDelta;

	if (nullptr == m_pBall)
	{
		m_bFinished = true;
		return;
	}

	if (m_pBall->Is_OpenFinished())
	{
		m_pBall->Hide();
		m_bFinished = true;
		return;
	}

	if (m_fElapsed >= m_fFlightDuration + 2.0f)
	{
		m_pBall->Hide();
		m_bFinished = true;
	}
}

_bool SSendOutBall::Is_Complete(const BATTLE_CONTEXT& ctx) const
{
	(void)ctx;
	return m_bFinished;
}

SSendOutBall* SSendOutBall::Create(_uint iSide, _float fFlightDuration)
{
	SSendOutBall* pInstance = new SSendOutBall();

	if (FAILED(pInstance->Initialize(iSide, fFlightDuration)))
	{
		MSG_BOX("Failed to Created : SSendOutBall");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void SSendOutBall::Free()
{
	m_pBall = nullptr;
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
	for (_uint i = 0; i < g_kBattleSideCount; ++i)
	{
		m_pHiddenTrainers[i] = nullptr;
		m_bPrevTrainerVisible[i] = true;
	}

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

void SPokemonEnter::Restore_Trainers()
{
	for (_uint i = 0; i < g_kBattleSideCount; ++i)
	{
		if (nullptr != m_pHiddenTrainers[i])
		{
			m_pHiddenTrainers[i]->Play_Focus();
			m_pHiddenTrainers[i]->Set_BattleVisible(m_bPrevTrainerVisible[i]);
		}

		m_pHiddenTrainers[i] = nullptr;
		m_bPrevTrainerVisible[i] = true;
	}
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
	Restore_Trainers();
	__super::Free();
}
#pragma endregion

#pragma region SPokemonSwitchOut
SPokemonSwitchOut::SPokemonSwitchOut()
{
}

HRESULT SPokemonSwitchOut::Initialize(_uint iSide, _float fDuration)
{
	if (iSide >= g_kBattleSideCount)
		return E_FAIL;

	m_iSide = iSide;
	m_fDuration = (fDuration > 0.f) ? fDuration : 0.f;
	m_fElapsed = 0.f;

	return S_OK;
}

void SPokemonSwitchOut::OnEnter(const BATTLE_CONTEXT& ctx)
{
	m_fElapsed = 0.f;

	if (nullptr == ctx.pManager)
		return;

	CBattle_Pokemon* pPokemon =
		dynamic_cast<CBattle_Pokemon*>(ctx.pManager->Get_BattlerObj(m_iSide));
	if (nullptr == pPokemon)
		return;

	if (CEffect_Manager* pEffectMgr = CEffect_Manager::GetInstance())
		pEffectMgr->PlayAt("ball_absorb", pPokemon->Get_EffectPivot());

	pPokemon->Set_BattleVisible(false);
}

void SPokemonSwitchOut::Update(const BATTLE_CONTEXT& ctx, _float fTimeDelta)
{
	(void)ctx;
	m_fElapsed += fTimeDelta;
}

_bool SPokemonSwitchOut::Is_Complete(const BATTLE_CONTEXT& ctx) const
{
	(void)ctx;
	return m_fElapsed >= m_fDuration;
}

SPokemonSwitchOut* SPokemonSwitchOut::Create(_uint iSide, _float fDuration)
{
	SPokemonSwitchOut* pInstance = new SPokemonSwitchOut();

	if (FAILED(pInstance->Initialize(iSide, fDuration)))
	{
		MSG_BOX("Failed to Created : SPokemonSwitchOut");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void SPokemonSwitchOut::Free()
{
	__super::Free();
}
#pragma endregion

#pragma region SApplySwitch
SApplySwitch::SApplySwitch()
{
}

HRESULT SApplySwitch::Initialize(_uint iSide, _uint iPartyIndex)
{
	if (iSide >= g_kBattleSideCount || iPartyIndex >= g_kMaxPartySize)
		return E_FAIL;

	m_iSide = iSide;
	m_iPartyIndex = iPartyIndex;
	m_bApplied = false;

	return S_OK;
}

void SApplySwitch::OnEnter(const BATTLE_CONTEXT& ctx)
{
	m_bApplied = false;

	if (nullptr == ctx.pManager)
		return;

	m_bApplied = SUCCEEDED(ctx.pManager->Replace_BattlerSlot(m_iSide, m_iPartyIndex));
}

void SApplySwitch::Update(const BATTLE_CONTEXT& ctx, _float fTimeDelta)
{
	(void)ctx;
	(void)fTimeDelta;
}

_bool SApplySwitch::Is_Complete(const BATTLE_CONTEXT& ctx) const
{
	(void)ctx;
	return true;
}

SApplySwitch* SApplySwitch::Create(_uint iSide, _uint iPartyIndex)
{
	SApplySwitch* pInstance = new SApplySwitch();

	if (FAILED(pInstance->Initialize(iSide, iPartyIndex)))
	{
		MSG_BOX("Failed to Created : SApplySwitch");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void SApplySwitch::Free()
{
	__super::Free();
}
#pragma endregion

#pragma region SSetPlateVisible
SSetPlateVisible::SSetPlateVisible()
{
}

HRESULT SSetPlateVisible::Initialize(_bool bVisible)
{
	m_bVisible = bVisible;
	return S_OK;
}

void SSetPlateVisible::OnEnter(const BATTLE_CONTEXT& ctx)
{
	if (nullptr != ctx.pManager)
		ctx.pManager->Set_PlateVisible(m_bVisible);
}

void SSetPlateVisible::Update(const BATTLE_CONTEXT& ctx, _float fTimeDelta)
{
	(void)ctx;
	(void)fTimeDelta;
}

_bool SSetPlateVisible::Is_Complete(const BATTLE_CONTEXT& ctx) const
{
	(void)ctx;
	return true;
}

SSetPlateVisible* SSetPlateVisible::Create(_bool bVisible)
{
	SSetPlateVisible* pInstance = new SSetPlateVisible();

	if (FAILED(pInstance->Initialize(bVisible)))
	{
		MSG_BOX("Failed to Created : SSetPlateVisible");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void SSetPlateVisible::Free()
{
	__super::Free();
}
#pragma endregion

#pragma region SHideTrainers
SHideTrainers::SHideTrainers()
{
}

void SHideTrainers::OnEnter(const BATTLE_CONTEXT& ctx)
{
	if (nullptr == ctx.pManager)
		return;

	for (_uint i = 0; i < g_kBattleSideCount; ++i)
	{
		CBattle_Trainer* pTrainer =
			dynamic_cast<CBattle_Trainer*>(ctx.pManager->Get_TrainerObj(i));
		if (nullptr != pTrainer)
			pTrainer->Set_BattleVisible(false);
	}
}

void SHideTrainers::Update(const BATTLE_CONTEXT& ctx, _float fTimeDelta)
{
	(void)ctx;
	(void)fTimeDelta;
}

_bool SHideTrainers::Is_Complete(const BATTLE_CONTEXT& ctx) const
{
	(void)ctx;
	return true;
}

SHideTrainers* SHideTrainers::Create()
{
	return new SHideTrainers();
}

void SHideTrainers::Free()
{
	__super::Free();
}
#pragma endregion

#pragma region SRevealTrainers
SRevealTrainers::SRevealTrainers()
{
}

void SRevealTrainers::OnEnter(const BATTLE_CONTEXT& ctx)
{
	(void)ctx;
	m_fElapsed = 0.f;
	m_bRevealed = false;

	// 포켓몬만 보이는 타이트 샷 → 전역으로 줌아웃. 트레이너 노출은 타이트 구간(화면 밖)에서 일어난다.
	CCamera_Director::GetInstance()->Play_Sequence(CAMERA_SEQUENCE_ID::INTRO_SETTLE);
}

void SRevealTrainers::Update(const BATTLE_CONTEXT& ctx, _float fTimeDelta)
{
	m_fElapsed += fTimeDelta;

	// 타이트 컷이 적용된 뒤(화면 밖)에 노출
	if (false == m_bRevealed && m_fElapsed >= 0.12f)
	{
		Reveal(ctx);
		m_bRevealed = true;
	}
}

_bool SRevealTrainers::Is_Complete(const BATTLE_CONTEXT& ctx) const
{
	(void)ctx;

	if (false == m_bRevealed)
		return false;

	// 줌아웃이 끝날 때까지 대기 (안전 타임아웃 2초)
	CCamera_Director* pDirector = CCamera_Director::GetInstance();
	if (nullptr != pDirector && pDirector->Is_Sequence_Playing() && m_fElapsed < 2.0f)
		return false;

	return true;
}

void SRevealTrainers::Reveal(const BATTLE_CONTEXT& ctx)
{
	if (nullptr == ctx.pManager)
		return;

	for (_uint i = 0; i < g_kBattleSideCount; ++i)
	{
		CBattle_Trainer* pTrainer =
			dynamic_cast<CBattle_Trainer*>(ctx.pManager->Get_TrainerObj(i));
		if (nullptr != pTrainer)
		{
			pTrainer->Set_BattleVisible(true);
			pTrainer->Play_Focus();
		}
	}
}

SRevealTrainers* SRevealTrainers::Create()
{
	return new SRevealTrainers();
}

void SRevealTrainers::Free()
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

#pragma region SPlayEffect
SPlayEffect::SPlayEffect()
{
}

HRESULT SPlayEffect::Initialize(const _string& strEffectID,
	EFFECT_VFX_TARGET eTarget,
	EFFECT_SLOT eSlot,
	const _float3& vOffset)
{
	if (strEffectID.empty())
		return E_FAIL;

	m_strEffectID = strEffectID;
	m_eTarget = eTarget;
	m_eSlot = eSlot;
	m_vOffset = vOffset;
	return S_OK;
}

void SPlayEffect::OnEnter(const BATTLE_CONTEXT& ctx)
{
	if (nullptr == ctx.pManager)
		return;

	const CBattle_ActionSequencer* pSeq = ctx.pManager->Get_Sequencer();
	if (nullptr == pSeq)
		return;

	const BATTLE_ACTION_DATA& tData = pSeq->Get_ActionData();
	const _uint iSide = (EFFECT_VFX_TARGET::ATTACKER == m_eTarget)
		? tData.iActorSide
		: tData.iTargetSide;

	CGameObject* pObj = ctx.pManager->Get_BattlerObj(iSide);
	CBattle_Pokemon* pPokemon = dynamic_cast<CBattle_Pokemon*>(pObj);
	if (nullptr == pPokemon)
		return;

	CEffect_Manager* pEffectMgr = CEffect_Manager::GetInstance();
	if (nullptr == pEffectMgr)
		return;

	pEffectMgr->PlayAt(m_strEffectID, pPokemon->Get_EffectPivot(m_eSlot, m_vOffset));
}

void SPlayEffect::Update(const BATTLE_CONTEXT& ctx, _float fTimeDelta)
{
	(void)ctx;
	(void)fTimeDelta;
}

_bool SPlayEffect::Is_Complete(const BATTLE_CONTEXT& ctx) const
{
	(void)ctx;
	return true;
}

SPlayEffect* SPlayEffect::Create(const _string& strEffectID,
	EFFECT_VFX_TARGET eTarget,
	EFFECT_SLOT eSlot,
	const _float3& vOffset)
{
	SPlayEffect* pInstance = new SPlayEffect();
	if (FAILED(pInstance->Initialize(strEffectID, eTarget, eSlot, vOffset)))
	{
		MSG_BOX("Failed to Created : SPlayEffect");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void SPlayEffect::Free()
{
	__super::Free();
}
#pragma endregion
