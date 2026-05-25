#include "Battle_Commands.h"
#include "Battler.h"
#include "PokemonData_Manager.h"
#include "Battle_Targeting.h"
#include "Battle_Math.h"
#include "Battle_Manager.h"
#include "Player_Status.h"
#include "Damage_Calculator.h"
#include "Battle_EventDispatcher.h"
#include "Battle_ActionSequencer.h"
#include "Battle_Action_Steps.h"
#include "Battle_Camera_Steps.h"

namespace
{
	CAMERA_SEQUENCE_ID Resolve_CameraSequence(const CMoveCommand::DESC& desc, const MOVE_DATA& move)
	{
		if (CAMERA_SEQUENCE_ID::NONE != desc.eCameraSequence)
			return desc.eCameraSequence;

		if (MOVE_CATEGORY::STATUS == move.eCategory || 0 == move.iPower)
			return CAMERA_SEQUENCE_ID::NONE;

		const _bool bPlayer = (g_kBattleSide_Player == desc.iActorSide);

		switch (move.eCategory)
		{
		case MOVE_CATEGORY::PHYSICAL:
			return bPlayer ? CAMERA_SEQUENCE_ID::TACKLE_PHYSICAL : CAMERA_SEQUENCE_ID::TACKLE_PHYSICAL_OPPONENT;

		case MOVE_CATEGORY::SPECIAL:
			return bPlayer ? CAMERA_SEQUENCE_ID::RANGED_ENERGY : CAMERA_SEQUENCE_ID::RANGED_ENERGY_OPPONENT;

		default:
			return CAMERA_SEQUENCE_ID::NONE;
		}
	}

	struct MOVE_VFX_PREFAB
	{
		_string strAttacker = {};            // 비면 attacker 이펙트 미사용
		_string strDefender = {};            // 비면 defender 이펙트 미사용
		_float  fAttackerDelay = { 0.0f };      // 직전 step → attacker 이펙트 전 대기
		_float  fBetweenDelay = { 0.15f };     // attacker → defender 사이 대기
		_float  fPreDamageDelay = { 0.10f };     // defender → SApplyDamage 사이 대기
		EFFECT_SLOT eAttackerSlot = { EFFECT_SLOT::CENTER };
		EFFECT_SLOT eDefenderSlot = { EFFECT_SLOT::CENTER };
		_float3 vAttackerOffset = {};
		_float3 vDefenderOffset = {};

		// 투사체 — true 면 attacker 이펙트가 공격자→방어자로 fProjectileTravel 초 동안 날아간다.
		_bool   bProjectile = { false };
		_float  fProjectileTravel = { 0.3f };

		// SFX — Sounds 루트 기준 상대경로(예: L"SFX/Moves/thunder_shock.wav"). 비면 미재생
		_wstring strSFX = { L"SFX/Tackle.mp3" };
		_float   fSFXVolume = { 1.f };
		_float   fSFXDelay = { 0.f };          // SPlaySFX 블로킹 지연(초)
	};

	MOVE_VFX_PREFAB Resolve_MoveVFX(_uint iMoveID)
	{
		switch (iMoveID)
		{
		case 84: // 전기쇼크 (thunder_shock) — 정확한 MoveID 데이터 확인 후 case 값 보정
		{
			MOVE_VFX_PREFAB p{};
			p.strAttacker = "at_thunder_shock";
			p.strDefender = "df_thunder_shock";
			p.fAttackerDelay = 0.0f;
			p.fBetweenDelay = 0.15f;
			p.fPreDamageDelay = 0.10f;
			p.strSFX = L"SFX/Thunder Shock.mp3";
			p.fSFXVolume = 0.8f;
			p.fSFXDelay = 0.0f;
			return p;
		}

		case 52: // 불꽃세례
		{
			MOVE_VFX_PREFAB p{};
			p.strAttacker = "AT_FIRE";
			p.strDefender = "DF_FIRE";
			p.bProjectile = true;   // at_fire 가 공격자→방어자로 날아간 뒤 df_fire 가 임팩트
			p.fAttackerDelay = 0.0f;
			p.fBetweenDelay = 0.15f;
			p.fPreDamageDelay = 0.10f;
			p.strSFX = L"SFX/Ember.mp3";       // TODO: SFX 경로 (Sounds 루트 기준)
			p.fSFXVolume = 1.0f;
			p.fSFXDelay = 0.0f;
			return p;
		}

		case 55: // 물대포
		{
			MOVE_VFX_PREFAB p{};
			p.strAttacker = "at_fire";
			p.strDefender = "df_fire";
			p.bProjectile = true;   // at_fire 가 공격자→방어자로 날아간 뒤 df_fire 가 임팩트
			p.fAttackerDelay = 0.0f;
			p.fBetweenDelay = 0.15f;
			p.fPreDamageDelay = 0.10f;
			p.strSFX = L"";       // TODO: SFX 경로 (Sounds 루트 기준)
			p.fSFXVolume = 1.0f;
			p.fSFXDelay = 0.0f;
			return p;
		}

		case 88: // 돌떨구기
		{
			MOVE_VFX_PREFAB p{};
			p.strAttacker = "";   // TODO: attacker 이펙트 ID
			p.strDefender = "ROCK";   // TODO: defender 이펙트 ID
			p.fAttackerDelay = 0.0f;
			p.fBetweenDelay = 0.10f;
			p.fPreDamageDelay = 0.25f;
			p.strSFX = L"SFX/Rock Throw.mp3";       // TODO: SFX 경로 (Sounds 루트 기준)
			p.fSFXVolume = 1.f;
			p.fSFXDelay = 0.25f;
			return p;
		}

		case 157: // 스톤샤워
		{
			MOVE_VFX_PREFAB p{};
			p.strAttacker = "";   // TODO: attacker 이펙트 ID
			p.strDefender = "";   // TODO: defender 이펙트 ID
			p.fAttackerDelay = 0.0f;
			p.fBetweenDelay = 0.15f;
			p.fPreDamageDelay = 0.10f;
			p.strSFX = L"";       // TODO: SFX 경로 (Sounds 루트 기준)
			p.fSFXVolume = 1.0f;
			p.fSFXDelay = 0.0f;
			return p;
		}

		case 201: // 참방참방서핑
		{
			MOVE_VFX_PREFAB p{};
			p.strAttacker = "at_surf";   // TODO: attacker 이펙트 ID
			p.strDefender = "df_surf";   // TODO: defender 이펙트 ID
			p.bProjectile = true;   // at_surf 가 공격자→방어자로 날아간 뒤 df_surf 가 임팩트
			p.fAttackerDelay = 0.0f;
			p.fBetweenDelay = 0.15f;
			p.fPreDamageDelay = 0.10f;
			p.strSFX = L"SFX/Surf.mp3";       // TODO: SFX 경로 (Sounds 루트 기준)
			p.fSFXVolume = 1.0f;
			p.fSFXDelay = 0.0f;
			p.fProjectileTravel = 0.7f;
			return p;
		}

		default:
		{
			// 폴백 — defender 측에 기존 hit 이펙트 1발
			MOVE_VFX_PREFAB p{};
			p.strDefender = "hit";
			p.fAttackerDelay = 0.0f;
			p.fBetweenDelay = 0.0f;
			p.fPreDamageDelay = 0.0f;
			p.strSFX = L"SFX/Tackle.mp3";
			p.fSFXVolume = 0.8f;
			p.fSFXDelay = 0.0f;
			return p;
		}
		}
	}
}

#pragma region MoveCommand
CMoveCommand::CMoveCommand()
{
}

HRESULT CMoveCommand::Initialize(const DESC& tDesc)
{
	if (tDesc.iActorSide >= g_kBattleSideCount)
		return E_FAIL;

	if (tDesc.iActorSlot >= g_kMaxSlotsPerSide)
		return E_FAIL;

	if (tDesc.iMoveSlot >= g_kMaxMovesPerPokemon)
		return E_FAIL;

	m_tDesc = tDesc;

	return S_OK;
}

_ushort CMoveCommand::Get_ActorSpeed(const BATTLE_CONTEXT& ctx) const
{
	CBattler* pActor = ctx.Get_Self(m_tDesc.iActorSide);
	if (nullptr == pActor)
		return 0;

	_ushort iSpeed = pActor->Get_Stat(STAT::SPD);
	iSpeed = static_cast<_ushort>(static_cast<_float>(iSpeed) *
		BattleMath::StageMul(pActor->Get_StatStage(STAGE_INDEX::SPD)));

	if (STATUS_CONDITION::PARALYSIS == pActor->Get_Status())
		iSpeed = static_cast<_ushort>(iSpeed / 2);

	return iSpeed;
}

HRESULT CMoveCommand::Execute(const BATTLE_CONTEXT& ctx)
{
	CBattler* pAttacker = ctx.Get_Self(m_tDesc.iActorSide);
	if (nullptr == pAttacker || nullptr == ctx.pDataMgr || nullptr == ctx.pManager)
		return E_FAIL;

	if (false == pAttacker->Is_Alive())
		return S_OK;

	const _uint iMoveID = pAttacker->Get_MoveID(m_tDesc.iMoveSlot);
	const MOVE_DATA* pMove = ctx.pDataMgr->Find_Move(iMoveID);
	if (nullptr == pMove)
		return E_FAIL;

	// PP 0 - 시퀀스 빌드 없이 즉시 메시지 발행 후 종료
	if (0 == pAttacker->Get_PP(m_tDesc.iMoveSlot))
	{
		if (nullptr != ctx.pDispatcher)
		{
			EVENT_MOVE_FAILED tEvent{};
			tEvent.iSide = m_tDesc.iActorSide;
			tEvent.iMoveID = iMoveID;
			tEvent.eReason = MOVE_FAIL_REASON::NO_PP;
			ctx.pDispatcher->Publish(tEvent);
		}

		return S_OK;
	}

	// PP 차감 + 마지막 무브 기록
	pAttacker->Consume_PP(m_tDesc.iMoveSlot);
	pAttacker->Set_LastMoveUsed(iMoveID);

	// 타겟 결정 (싱글배틀 - 첫 번째 타겟만 사용)
	BattleTargeting::TARGET_LIST tTargets{};
	BattleTargeting::Resolve(ctx, m_tDesc.iActorSide, m_tDesc.iActorSlot, *pMove, tTargets);

	if (0 == tTargets.iCount || nullptr == tTargets.aTargets[0])
		return S_OK;

	CBattler* pDefender = tTargets.aTargets[0];

	CBattle_ActionSequencer* pSeq = ctx.pManager->Get_Sequencer();
	if (nullptr == pSeq)
		return E_FAIL;

	// ActionData 셋업
	pSeq->Reset_ActionData();
	BATTLE_ACTION_DATA& tData = pSeq->Get_ActionData();
	tData.iActorSide = m_tDesc.iActorSide;
	tData.iActorSlot = m_tDesc.iActorSlot;
	tData.iTargetSide = pDefender->Get_Side();
	tData.iTargetSlot = pDefender->Get_SlotIndex();
	tData.iMoveID = iMoveID;
	tData.iMoveSlot = m_tDesc.iMoveSlot;

	// 시퀀스 빌드
	auto Push = [pSeq](IBattleAction_Step* pStep)
		{
			if (nullptr == pStep)
				return;

			pSeq->Push_Step(pStep);
			Safe_Release(pStep);
		};

	// 변경 후 (동일 블록)
	const CAMERA_SEQUENCE_ID eCameraSequence = Resolve_CameraSequence(m_tDesc, *pMove);
	const _bool bUseCameraSequence = (CAMERA_SEQUENCE_ID::NONE != eCameraSequence);

	const MOVE_VFX_PREFAB tVFX = Resolve_MoveVFX(iMoveID);

	Push(SAccuracyCheck::Create());   // 발성·연출 전에 명중/타입무효 확정 (연출 게이팅용)

	Push(SAnnounce::Create(m_tDesc.iActorSide, iMoveID));
	Push(SDelay::Create(0.3f));
	Push(SCloseMsg::Create());
	Push(SDelay::Create(0.2f));

	if (bUseCameraSequence)
		Push(SCamera_PlaySequence::Create(eCameraSequence, true, true));

	Push(SMissMessage::Create(m_tDesc.iActorSide, iMoveID));

	// 기술 SFX — 시전(attacker VFX) 시점에 1회 재생. fSFXDelay 로 선후 미세조정(블로킹 지연)
	if (false == tVFX.strSFX.empty())
		Push(SPlaySFX::Create(tVFX.strSFX, tVFX.fSFXVolume, tVFX.fSFXDelay));

	if (false == tVFX.strAttacker.empty())
	{
		if (tVFX.fAttackerDelay > 0.f)
			Push(SDelay::Create(tVFX.fAttackerDelay));

		if (tVFX.bProjectile)
			Push(SPlayEffectProjectile::Create(
				tVFX.strAttacker,
				tVFX.eAttackerSlot, tVFX.vAttackerOffset,
				tVFX.eDefenderSlot, tVFX.vDefenderOffset,
				tVFX.fProjectileTravel));
		else
			Push(SPlayEffect::Create(
				tVFX.strAttacker, EFFECT_VFX_TARGET::ATTACKER,
				tVFX.eAttackerSlot, tVFX.vAttackerOffset));
	}

	if (tVFX.fBetweenDelay > 0.f)
		Push(SDelay::Create(tVFX.fBetweenDelay));

	if (false == tVFX.strDefender.empty())
	{
		Push(SPlayEffect::Create(
			tVFX.strDefender, EFFECT_VFX_TARGET::DEFENDER,
			tVFX.eDefenderSlot, tVFX.vDefenderOffset));
	}

	if (tVFX.fPreDamageDelay > 0.f)
		Push(SDelay::Create(tVFX.fPreDamageDelay));

	Push(SApplyDamage::Create());

	Push(SResultMessages::Create());
	Push(SDelay::Create(0.2f));
	Push(SCloseMsg::Create());
	Push(SDelay::Create(0.3f));
	Push(SFaintCheck::Create());
	Push(SDelay::Create(0.2f));
	Push(SCloseMsg::Create());

	Push(SDone::Create());

	pSeq->Submit();

	return S_OK;
}

CMoveCommand* CMoveCommand::Create(const DESC& tDesc)
{
	CMoveCommand* pInstance = new CMoveCommand();

	if (FAILED(pInstance->Initialize(tDesc)))
	{
		MSG_BOX("Failed to Created : CMoveCommand");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMoveCommand::Free()
{
	__super::Free();
}
#pragma endregion

#pragma region SwitchCommand
CSwitchCommand::CSwitchCommand()
{
}

HRESULT CSwitchCommand::Initialize(const DESC& tDesc)
{
	if (tDesc.iActorSide >= g_kBattleSideCount)
		return E_FAIL;

	if (tDesc.iActorSlot >= g_kMaxSlotsPerSide)
		return E_FAIL;

	if (tDesc.iTargetPartyIndex >= g_kMaxPartySize)
		return E_FAIL;

	m_tDesc = tDesc;

	return S_OK;
}

_ushort CSwitchCommand::Get_ActorSpeed(const BATTLE_CONTEXT& ctx) const
{
	(void)ctx;
	return 0;
}

HRESULT CSwitchCommand::Execute(const BATTLE_CONTEXT& ctx)
{
	if (nullptr == ctx.pManager)
		return E_FAIL;

	CBattle_ActionSequencer* pSeq = ctx.pManager->Get_Sequencer();
	if (nullptr == pSeq)
		return E_FAIL;

	CBattler* pOldBattler = ctx.pManager->Get_Battler(m_tDesc.iActorSide);
	const POKEMON_INSTANCE* pOldInstance =
		(nullptr != pOldBattler) ? pOldBattler->Get_Instance() : nullptr;
	const _wstring strOldName =
		(nullptr != pOldInstance) ? _wstring(pOldInstance->szNickname) : _wstring(TEXT("?"));

	const POKEMON_INSTANCE* pNewInstance = nullptr;
	if (g_kBattleSide_Player == m_tDesc.iActorSide)
	{
		CPlayer_Status* pPlayerState = ctx.pManager->Get_PlayerState();
		if (nullptr != pPlayerState)
			pNewInstance = PartyOps::Get(pPlayerState->Get_Party(), m_tDesc.iTargetPartyIndex);
	}
	else if (g_kBattleSide_Opponent == m_tDesc.iActorSide)
	{
		const TRAINER_DATA* pTrainer = ctx.pManager->Get_OpponentTrainer();
		if (nullptr != pTrainer)
			pNewInstance = PartyOps::Get(pTrainer->tParty, m_tDesc.iTargetPartyIndex);
	}

	if (nullptr == pNewInstance)
		return E_FAIL;

	const _wstring strNewName = _wstring(pNewInstance->szNickname);
	const _wstring strReturnMsg = _wstring(TEXT("돌아와!\n")) + strOldName;

	_wstring strSendOutMsg;
	if (g_kBattleSide_Opponent == m_tDesc.iActorSide)
	{
		const TRAINER_DATA* pTrainer = ctx.pManager->Get_OpponentTrainer();
		const _wstring strTrainerName =
			(nullptr != pTrainer) ? _wstring(pTrainer->szName) : _wstring(TEXT("상대"));
		strSendOutMsg = strTrainerName + TEXT("은(는) ") + strNewName + TEXT("을(를) 내보냈다!");
	}
	else
	{
		strSendOutMsg = _wstring(TEXT("플레이어는 ")) + strNewName + TEXT("을(를) 내보냈다!");
	}

	const BATTLE_ENV& tEnv = ctx.pManager->Get_Env();
	const _bool bTrainerRule =
		BATTLE_RULE::TRAINER_SINGLE == tEnv.eRule ||
		BATTLE_RULE::TRAINER_DOUBLE == tEnv.eRule;

	pSeq->Reset_ActionData();
	BATTLE_ACTION_DATA& tData = pSeq->Get_ActionData();
	tData.iActorSide = m_tDesc.iActorSide;
	tData.iActorSlot = m_tDesc.iActorSlot;

	auto Push = [pSeq](IBattleAction_Step* pStep)
		{
			if (nullptr == pStep)
				return;

			pSeq->Push_Step(pStep);
			Safe_Release(pStep);
		};

	Push(SBattleText::Create(strReturnMsg));
	Push(SCloseMsg::Create());
	Push(SDelay::Create(0.15f));
	Push(SSetPlateVisible::Create(false));
	Push(SPokemonSwitchOut::Create(m_tDesc.iActorSide));
	Push(SApplySwitch::Create(m_tDesc.iActorSide, m_tDesc.iTargetPartyIndex));
	Push(SDelay::Create(0.1f));

	Push(SBattleText::Create(strSendOutMsg));
	if (bTrainerRule)
		Push(SSendOutBall::Create(m_tDesc.iActorSide));

	Push(SPlaySFX::Create(L"SFX/capture_fail.wav", 0.8f));
	Push(SPokemonEnter::Create(m_tDesc.iActorSide));

	if (bTrainerRule)
		Push(SRevealTrainers::Create());

	Push(SSetPlateVisible::Create(true));
	Push(SCloseMsg::Create());
	Push(SDelay::Create(0.2f));
	Push(SDone::Create());

	pSeq->Submit();
	return S_OK;
}

CSwitchCommand* CSwitchCommand::Create(const DESC& tDesc)
{
	CSwitchCommand* pInstance = new CSwitchCommand();

	if (FAILED(pInstance->Initialize(tDesc)))
	{
		MSG_BOX("Failed to Created : CSwitchCommand");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSwitchCommand::Free()
{
	__super::Free();
}
#pragma endregion

#pragma region RunCommand
CRunCommand::CRunCommand()
{
}

HRESULT CRunCommand::Initialize(const DESC& tDesc)
{
	if (tDesc.iActorSide >= g_kBattleSideCount)
		return E_FAIL;

	m_tDesc = tDesc;

	return S_OK;
}

_ushort CRunCommand::Get_ActorSpeed(const BATTLE_CONTEXT& ctx) const
{
	(void)ctx;
	return 0;
}

HRESULT CRunCommand::Execute(const BATTLE_CONTEXT& ctx)
{
	if (false == BattleMath::Roll_RunSuccess(ctx, m_tDesc.iActorSide))
	{
		if (nullptr != ctx.pDispatcher)
		{
			EVENT_RUN_FAILED tEvent{};
			tEvent.iSide = m_tDesc.iActorSide;
			tEvent.eReason = RUN_FAIL_REASON::OTHER;
			ctx.pDispatcher->Publish(tEvent);
		}

		return S_OK;
	}

	if (nullptr != ctx.pDispatcher)
	{
		EVENT_RUN_SUCCEEDED tEvent{};
		tEvent.iSide = m_tDesc.iActorSide;
		ctx.pDispatcher->Publish(tEvent);
	}

	if (nullptr != ctx.pManager)
		ctx.pManager->Request_Exit();

	return S_OK;
}

CRunCommand* CRunCommand::Create(const DESC& tDesc)
{
	CRunCommand* pInstance = new CRunCommand();

	if (FAILED(pInstance->Initialize(tDesc)))
	{
		MSG_BOX("Failed to Created : CRunCommand");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRunCommand::Free()
{
	__super::Free();
}
#pragma endregion
