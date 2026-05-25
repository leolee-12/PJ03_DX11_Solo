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

namespace
{
	// 보유 pv 울음 파일이 있는 종족만 자기 울음, 나머지는 pv0025(피카츄) 폴백.
	_wstring Build_CryKey(_uint iSpeciesID, _bool bHappy)
	{
		static const _uint s_AvailableCries[] =
		{ 1, 4, 7, 10, 25, 41, 43, 59, 74, 95, 121 };

		_uint iCryID = 25u;
		for (_uint id : s_AvailableCries)
		{
			if (id == iSpeciesID)
			{
				iCryID = iSpeciesID;
				break;
			}
		}

		_tchar szKey[64] = {};
		swprintf_s(szKey, L"SFX/pv%04u_%s_adpcm.wav", iCryID, bHappy ? L"Happy" : L"Common");
		return szKey;
	}

	constexpr _float FAINT_CAPTURE_HIT_DELAY = 0.65f;
	constexpr _float FAINT_DISAPPEAR_DURATION = 0.45f;

	// 빗나감(명중 실패)·타입 무효면 공격/피격 연출을 생략하기 위한 판정.
	// 매니저/시퀀서가 없으면 게이팅 불가로 보고 기존 동작(연출 출력)을 유지한다.
	_bool Move_Connects(const BATTLE_CONTEXT& ctx)
	{
		if (nullptr == ctx.pManager)
			return true;

		const CBattle_ActionSequencer* pSeq = ctx.pManager->Get_Sequencer();
		return (nullptr == pSeq) ? true : pSeq->Get_ActionData().Connects();
	}
}

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

#pragma region STrainerFaint
STrainerFaint::STrainerFaint()
{
}

HRESULT STrainerFaint::Initialize(_uint iSide)
{
	if (iSide >= g_kBattleSideCount)
		return E_FAIL;

	m_iSide = iSide;
	return S_OK;
}

void STrainerFaint::OnEnter(const BATTLE_CONTEXT& ctx)
{
	if (nullptr == ctx.pManager)
		return;

	CBattle_Trainer* pTrainer =
		dynamic_cast<CBattle_Trainer*>(ctx.pManager->Get_TrainerObj(m_iSide));

	if (nullptr != pTrainer)
		pTrainer->Play_Faint();
}

void STrainerFaint::Update(const BATTLE_CONTEXT& ctx, _float fTimeDelta)
{
	(void)ctx;
	(void)fTimeDelta;
}

_bool STrainerFaint::Is_Complete(const BATTLE_CONTEXT& ctx) const
{
	(void)ctx;
	return true;
}

STrainerFaint* STrainerFaint::Create(_uint iSide)
{
	STrainerFaint* pInstance = new STrainerFaint();

	if (FAILED(pInstance->Initialize(iSide)))
	{
		MSG_BOX("Failed to Created : STrainerFaint");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void STrainerFaint::Free()
{
	__super::Free();
}
#pragma endregion

#pragma region SSendOutBall
SSendOutBall::SSendOutBall()
{
}

HRESULT SSendOutBall::Initialize(_uint iSide, _float fFlightDuration, CAMERA_SEQUENCE_ID eCameraSequence)
{
	if (iSide >= g_kBattleSideCount)
		return E_FAIL;

	m_iSide = iSide;
	m_fFlightDuration = (fFlightDuration > 0.f) ? fFlightDuration : 0.72f;
	m_eCameraSequence = eCameraSequence;
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
		(CAMERA_SEQUENCE_ID::NONE != m_eCameraSequence)
		? m_eCameraSequence
		: ((g_kBattleSide_Player == m_iSide)
			? CAMERA_SEQUENCE_ID::SENDOUT_PLAYER
			: CAMERA_SEQUENCE_ID::SENDOUT_OPPONENT);

	CCamera_Director::GetInstance()->Play_Sequence(eSequence);

	CBattle_Pokemon* pPokemon =
		dynamic_cast<CBattle_Pokemon*>(ctx.pManager->Get_BattlerObj(m_iSide));

	if (nullptr == pPokemon)
	{
		m_bFinished = true;
		return;
	}

	m_vTargetPos = pPokemon->Get_EffectPivot();
	m_vTargetPos.y -= 0.25f;

	const _float fSideSign =
		(g_kBattleSide_Player == m_iSide) ? -1.f : 1.f;

	const _float3 vStartPos = _float3(
		m_vTargetPos.x + 0.85f * fSideSign,
		m_vTargetPos.y + 0.65f,
		m_vTargetPos.z + 1.35f * fSideSign);

	_float3 vBallFaceTarget = vStartPos;
	if (g_kBattleSide_Player == m_iSide || g_kBattleSide_Opponent == m_iSide)
	{
		vBallFaceTarget.x = m_vTargetPos.x - (vStartPos.x - m_vTargetPos.x);
		vBallFaceTarget.z = m_vTargetPos.z - (vStartPos.z - m_vTargetPos.z);
	}

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
	m_pBall->Play_BattleOpen(m_vTargetPos, vBallFaceTarget);
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

SSendOutBall* SSendOutBall::Create(_uint iSide, _float fFlightDuration, CAMERA_SEQUENCE_ID eCameraSequence)
{
	SSendOutBall* pInstance = new SSendOutBall();

	if (FAILED(pInstance->Initialize(iSide, fFlightDuration, eCameraSequence)))
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

HRESULT SPokemonEnter::Initialize(_uint iSide, _float fHoldSeconds)
{
	if (iSide >= g_kBattleSideCount)
		return E_FAIL;

	m_iSide = iSide;
	m_fGrace = 0.f;
	m_fHoldSeconds = (fHoldSeconds > 0.f) ? fHoldSeconds : 0.f;

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
	m_bCryPlayed = false;

	if (nullptr == ctx.pManager)
		return;

	CBattle_Pokemon* pPokemon =
		dynamic_cast<CBattle_Pokemon*>(ctx.pManager->Get_BattlerObj(m_iSide));

	if (nullptr != pPokemon)
		pPokemon->Begin_SendOutAppear();
}

void SPokemonEnter::Update(const BATTLE_CONTEXT& ctx, _float fTimeDelta)
{
	m_fGrace += fTimeDelta;

	// ball_absorb VFX(OnEnter) 직후 울음 - 홀드 길이와 무관하게 등장 직후 1회 재생
	if (false == m_bCryPlayed && m_fGrace >= 0.3f)
	{
		_uint iSpecies = 25u;
		CBattler* pBattler = ctx.Get_Self(m_iSide);
		if (nullptr != pBattler && nullptr != pBattler->Get_Instance())
			iSpecies = pBattler->Get_Instance()->iSpeciesID;

		CGameInstance* pGameInstance = CGameInstance::GetInstance();
		if (nullptr != pGameInstance)
			pGameInstance->Play(Build_CryKey(iSpecies, true).c_str(), CHANNELID::SFX, 1.5f);

		m_bCryPlayed = true;
	}
}

_bool SPokemonEnter::Is_Complete(const BATTLE_CONTEXT& ctx) const
{
	(void)ctx;

	return m_fGrace >= m_fHoldSeconds;
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

SPokemonEnter* SPokemonEnter::Create(_uint iSide, _float fHoldSeconds)
{
	SPokemonEnter* pInstance = new SPokemonEnter();

	if (FAILED(pInstance->Initialize(iSide, fHoldSeconds)))
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

	// 타입 상성 무효 사전 판정 (데미지 기술만). 데미지 계산 없이 타입 곱만으로 면역 여부 확정.
	tData.bImmune = false;
	if (MOVE_CATEGORY::STATUS != pMove->eCategory && 0 != pMove->iPower &&
		nullptr != pDefender && nullptr != pDefender->Get_Instance())
	{
		const SPECIES_DATA* pSpecies = ctx.pDataMgr->Find_Species(pDefender->Get_Instance()->iSpeciesID);
		if (nullptr != pSpecies)
		{
			const _float fType1 = ctx.pDataMgr->Get_TypeMultiplier(pMove->eType, pSpecies->eType1);
			const _float fType2 = (TYPE::NONE == pSpecies->eType2)
				? 1.f
				: ctx.pDataMgr->Get_TypeMultiplier(pMove->eType, pSpecies->eType2);
			tData.bImmune = ((fType1 * fType2) <= 0.f);
		}
	}
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
	m_iFaintedSide = g_kBattleSideCount;
	m_bPublished = false;
	m_bHitPlayed = false;

	if (nullptr == ctx.pManager)
		return;

	CBattle_ActionSequencer* pSeq = ctx.pManager->Get_Sequencer();
	if (nullptr == pSeq)
		return;

	BATTLE_ACTION_DATA& tData = pSeq->Get_ActionData();

	if (false == tData.bFaintedThisHit)
		return;

	m_iFaintedSide = tData.iTargetSide;

	if (nullptr != ctx.pDispatcher)
	{
		EVENT_POKEMON_FAINTED tEvent{};
		tEvent.iSide = tData.iTargetSide;
		ctx.pDispatcher->Publish(tEvent);
		m_bPublished = true;
	}

	// 쓰러진 포켓몬의 Common 울음 1회 (capture_hit 는 Update 에서 약간 뒤에 재생)
	CBattler* pFainted = ctx.Get_Self(tData.iTargetSide);
	if (nullptr != pFainted && nullptr != pFainted->Get_Instance())
	{
		CGameInstance* pGameInstance = CGameInstance::GetInstance();
		if (nullptr != pGameInstance)
			pGameInstance->Play(Build_CryKey(pFainted->Get_Instance()->iSpeciesID, false).c_str(), CHANNELID::SFX, 0.8f);
	}
}

void SFaintCheck::Update(const BATTLE_CONTEXT& ctx, _float fTimeDelta)
{
	m_fGrace += fTimeDelta;

	if (m_bPublished && false == m_bHitPlayed && m_fGrace >= FAINT_CAPTURE_HIT_DELAY)
	{
		CGameInstance* pGameInstance = CGameInstance::GetInstance();
		if (nullptr != pGameInstance)
			pGameInstance->Play(L"SFX/capture_hit.wav", CHANNELID::SFX, 0.8f);

		if (nullptr != ctx.pManager && m_iFaintedSide < g_kBattleSideCount)
		{
			CBattle_Pokemon* pPokemon =
				dynamic_cast<CBattle_Pokemon*>(ctx.pManager->Get_BattlerObj(m_iFaintedSide));
			if (nullptr != pPokemon)
			{
				if (CEffect_Manager* pEffectMgr = CEffect_Manager::GetInstance())
					pEffectMgr->PlayAt("ball_absorb", pPokemon->Get_EffectPivot());

				pPokemon->Begin_FaintDisappear(FAINT_DISAPPEAR_DURATION);
			}
		}

		m_bHitPlayed = true;
	}
}

_bool SFaintCheck::Is_Complete(const BATTLE_CONTEXT& ctx) const
{
	if (false == m_bPublished)
		return true;

	if (false == m_bHitPlayed)
		return false;

	if (m_fGrace < FAINT_CAPTURE_HIT_DELAY + FAINT_DISAPPEAR_DURATION)
		return false;

	if (nullptr != ctx.pManager && m_iFaintedSide < g_kBattleSideCount)
	{
		CBattle_Pokemon* pPokemon =
			dynamic_cast<CBattle_Pokemon*>(ctx.pManager->Get_BattlerObj(m_iFaintedSide));
		if (nullptr != pPokemon && false == pPokemon->Is_FaintDisappear_Finished())
			return false;
	}

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

	// 빗나감/타입 무효면 이펙트 생략
	if (false == Move_Connects(ctx))
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

	m_pEffect = pEffectMgr->PlayAt(m_strEffectID, pPokemon->Get_EffectPivot(m_eSlot, m_vOffset));
	if (nullptr != m_pEffect)
		m_pEffect->AddRef();
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
	if (nullptr != m_pEffect)
	{
		m_pEffect->Stop();          // 추가 spawn 중단 → 잔여 입자 자연 소멸 후 자체 Set_Dead
		Safe_Release(m_pEffect);    // 공동 소유 해제 (Layer ref 로 객체는 자연 종료까지 유지)
	}
	__super::Free();
}
#pragma endregion

#pragma region SPlayEffectProjectile
SPlayEffectProjectile::SPlayEffectProjectile()
{
}

HRESULT SPlayEffectProjectile::Initialize(const _string& strEffectID,
	EFFECT_SLOT eStartSlot, const _float3& vStartOffset,
	EFFECT_SLOT eEndSlot, const _float3& vEndOffset,
	_float fTravel)
{
	if (strEffectID.empty())
		return E_FAIL;

	m_strEffectID = strEffectID;
	m_eStartSlot = eStartSlot;
	m_vStartOffset = vStartOffset;
	m_eEndSlot = eEndSlot;
	m_vEndOffset = vEndOffset;
	m_fTravel = (fTravel > 0.f) ? fTravel : 0.3f;
	return S_OK;
}

void SPlayEffectProjectile::Update_Matrix(_float t)
{
	_vector vStart = XMLoadFloat3(&m_vStartPos);
	_vector vEnd = XMLoadFloat3(&m_vEndPos);
	_vector vPos = XMVectorLerp(vStart, vEnd, t);

	XMStoreFloat4x4(&m_mProjectile, XMMatrixTranslationFromVector(vPos));
}

void SPlayEffectProjectile::OnEnter(const BATTLE_CONTEXT& ctx)
{
	m_fElapsed = 0.f;
	m_bFinished = false;
	m_pEffect = nullptr;

	if (nullptr == ctx.pManager)
	{
		m_bFinished = true;
		return;
	}

	// 빗나감/타입 무효면 투사체 생략 (즉시 완료 처리)
	if (false == Move_Connects(ctx))
	{
		m_bFinished = true;
		return;
	}

	const CBattle_ActionSequencer* pSeq = ctx.pManager->Get_Sequencer();
	if (nullptr == pSeq)
	{
		m_bFinished = true;
		return;
	}

	const BATTLE_ACTION_DATA& tData = pSeq->Get_ActionData();

	CBattle_Pokemon* pAttacker =
		dynamic_cast<CBattle_Pokemon*>(ctx.pManager->Get_BattlerObj(tData.iActorSide));
	CBattle_Pokemon* pDefender =
		dynamic_cast<CBattle_Pokemon*>(ctx.pManager->Get_BattlerObj(tData.iTargetSide));

	if (nullptr == pAttacker || nullptr == pDefender)
	{
		m_bFinished = true;
		return;
	}

	m_vStartPos = pAttacker->Get_EffectPivot(m_eStartSlot, m_vStartOffset);
	m_vEndPos = pDefender->Get_EffectPivot(m_eEndSlot, m_vEndOffset);
	Update_Matrix(0.f);

	CEffect_Manager* pEffectMgr = CEffect_Manager::GetInstance();
	if (nullptr == pEffectMgr)
	{
		m_bFinished = true;
		return;
	}

	CEffect::EFFECT_DESC::ATTACH_INFO tAttach{};
	tAttach.eKind = CEffect::EFFECT_DESC::ATTACH_INFO::KIND::MATRIX;
	tAttach.pSourceMatrix = &m_mProjectile;

	m_pEffect = pEffectMgr->PlayAttached(m_strEffectID, tAttach);

	// 비행 중 이펙트가 자체 소멸(auto-destroy)해도 포인터가 유효하도록 공동 소유
	if (nullptr != m_pEffect)
		m_pEffect->AddRef();
	else
		m_bFinished = true;   // 스폰 실패 — 비행 생략하고 다음 step 으로
}

void SPlayEffectProjectile::Update(const BATTLE_CONTEXT& ctx, _float fTimeDelta)
{
	(void)ctx;

	if (m_bFinished)
		return;

	m_fElapsed += fTimeDelta;

	_float t = m_fElapsed / m_fTravel;
	if (t > 1.f)
		t = 1.f;

	Update_Matrix(t);

	if (t >= 1.f)
	{
		// 도착 — 추종 매트릭스(m_mProjectile)를 더는 읽지 않도록 이펙트를 즉시 소멸시킨다.
		// step 메모리는 액션 종료 시 일괄 해제되므로, 그 전에 이펙트를 정리해 dangling 을 막는다.
		if (nullptr != m_pEffect)
		{
			m_pEffect->Destroy();
			Safe_Release(m_pEffect);
			m_pEffect = nullptr;   // 공동 소유 해제 완료 — Free 의 2차 release 차단
		}
		m_bFinished = true;
	}
}

_bool SPlayEffectProjectile::Is_Complete(const BATTLE_CONTEXT& ctx) const
{
	(void)ctx;
	return m_bFinished;
}

SPlayEffectProjectile* SPlayEffectProjectile::Create(const _string& strEffectID,
	EFFECT_SLOT eStartSlot, const _float3& vStartOffset,
	EFFECT_SLOT eEndSlot, const _float3& vEndOffset,
	_float fTravel)
{
	SPlayEffectProjectile* pInstance = new SPlayEffectProjectile();
	if (FAILED(pInstance->Initialize(strEffectID, eStartSlot, vStartOffset,
		eEndSlot, vEndOffset, fTravel)))
	{
		MSG_BOX("Failed to Created : SPlayEffectProjectile");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void SPlayEffectProjectile::Free()
{
	// 도착 전에 시퀀스가 정리되는 예외 경로 — 남은 이펙트를 안전하게 소멸시킨다.
	if (nullptr != m_pEffect)
	{
		m_pEffect->Destroy();
		Safe_Release(m_pEffect);
	}
	__super::Free();
}
#pragma endregion

#pragma region SPlaySFX
SPlaySFX::SPlaySFX()
{
}

HRESULT SPlaySFX::Initialize(const _wstring& strSFXKey, _float fVolume, _float fDelay)
{
	if (strSFXKey.empty())
		return E_FAIL;

	m_strSFXKey = strSFXKey;
	m_fVolume = fVolume;
	m_fDelay = (fDelay > 0.f) ? fDelay : 0.f;
	return S_OK;
}

void SPlaySFX::OnEnter(const BATTLE_CONTEXT& ctx)
{
	m_fElapsed = 0.f;
	m_bPlayed = false;

	// 빗나감/타입 무효면 사운드 생략 (즉시 완료 처리)
	if (false == Move_Connects(ctx))
	{
		m_bPlayed = true;
		return;
	}

	if (0.f == m_fDelay)
		Play_SFX();
}

void SPlaySFX::Update(const BATTLE_CONTEXT& ctx, _float fTimeDelta)
{
	(void)ctx;
	if (m_bPlayed)
		return;

	m_fElapsed += fTimeDelta;
	if (m_fElapsed >= m_fDelay)
		Play_SFX();
}

_bool SPlaySFX::Is_Complete(const BATTLE_CONTEXT& ctx) const
{
	(void)ctx;
	return m_bPlayed;
}

void SPlaySFX::Play_SFX()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	if (nullptr != pGameInstance)
		pGameInstance->Play(m_strSFXKey.c_str(), CHANNELID::SFX, m_fVolume);

	m_bPlayed = true;
}

SPlaySFX* SPlaySFX::Create(const _wstring& strSFXKey, _float fVolume, _float fDelay)
{
	SPlaySFX* pInstance = new SPlaySFX();
	if (FAILED(pInstance->Initialize(strSFXKey, fVolume, fDelay)))
	{
		MSG_BOX("Failed to Created : SPlaySFX");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void SPlaySFX::Free()
{
	__super::Free();
}
#pragma endregion

#pragma region SPlayCry
SPlayCry::SPlayCry()
{
}

HRESULT SPlayCry::Initialize(_uint iSide, CRY_KIND eKind, _float fVolume, _float fDelay)
{
	if (iSide >= g_kBattleSideCount)
		return E_FAIL;

	m_iSide = iSide;
	m_eKind = eKind;
	m_fVolume = fVolume;
	m_fDelay = (fDelay > 0.f) ? fDelay : 0.f;
	return S_OK;
}

void SPlayCry::OnEnter(const BATTLE_CONTEXT& ctx)
{
	m_fElapsed = 0.f;
	m_bPlayed = false;

	_uint iSpeciesID = 25u;

	CBattler* pBattler = ctx.Get_Self(m_iSide);
	if (nullptr != pBattler && nullptr != pBattler->Get_Instance())
		iSpeciesID = pBattler->Get_Instance()->iSpeciesID;

	m_strKey = Build_CryKey(iSpeciesID, CRY_KIND::HAPPY == m_eKind);

	if (0.f == m_fDelay)
		Play_Cry();
}

void SPlayCry::Update(const BATTLE_CONTEXT& ctx, _float fTimeDelta)
{
	(void)ctx;
	if (m_bPlayed)
		return;

	m_fElapsed += fTimeDelta;
	if (m_fElapsed >= m_fDelay)
		Play_Cry();
}

_bool SPlayCry::Is_Complete(const BATTLE_CONTEXT& ctx) const
{
	(void)ctx;
	return m_bPlayed;
}

void SPlayCry::Play_Cry()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	if (nullptr != pGameInstance && false == m_strKey.empty())
		pGameInstance->Play(m_strKey.c_str(), CHANNELID::SFX, m_fVolume);

	m_bPlayed = true;
}

SPlayCry* SPlayCry::Create(_uint iSide, CRY_KIND eKind, _float fVolume, _float fDelay)
{
	SPlayCry* pInstance = new SPlayCry();
	if (FAILED(pInstance->Initialize(iSide, eKind, fVolume, fDelay)))
	{
		MSG_BOX("Failed to Created : SPlayCry");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void SPlayCry::Free()
{
	__super::Free();
}
#pragma endregion
