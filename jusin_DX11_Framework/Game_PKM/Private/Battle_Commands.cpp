#include "Battle_Commands.h"
#include "Battler.h"
#include "PokemonData_Manager.h"
#include "Battle_Targeting.h"
#include "Battle_Math.h"
#include "Battle_Manager.h"
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

		switch (move.iMoveID)
		{
		case 157: // 스톤샤워
		case 201: // 참방참방서핑
			return CAMERA_SEQUENCE_ID::AREA_WIDE;

		default:
			break;
		}

		switch (move.eCategory)
		{
		case MOVE_CATEGORY::PHYSICAL:
			return CAMERA_SEQUENCE_ID::TACKLE_PHYSICAL;

		case MOVE_CATEGORY::SPECIAL:
			return CAMERA_SEQUENCE_ID::RANGED_ENERGY;

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

	Push(SAnnounce::Create(m_tDesc.iActorSide, iMoveID));
	Push(SDelay::Create(0.3f));
	Push(SCloseMsg::Create());
	Push(SDelay::Create(0.2f));

	if (bUseCameraSequence)
		Push(SCamera_PlaySequence::Create(eCameraSequence));

	Push(SAccuracyCheck::Create());
	Push(SMissMessage::Create(m_tDesc.iActorSide, iMoveID));

	if (false == tVFX.strAttacker.empty())
	{
		if (tVFX.fAttackerDelay > 0.f)
			Push(SDelay::Create(tVFX.fAttackerDelay));

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

	if (bUseCameraSequence)
		Push(SCamera_Shake::Create(0.08f, 35.f, 0.12f));

	Push(SResultMessages::Create());
	Push(SDelay::Create(0.2f));
	Push(SCloseMsg::Create());
	Push(SDelay::Create(0.3f));
	Push(SFaintCheck::Create());
	Push(SDelay::Create(0.2f));
	Push(SCloseMsg::Create());

	if (bUseCameraSequence)
		Push(SCamera_Return::Create(0.4f));

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

	// 즉시 모델 교체 — Apply_Switch 가 visible=false 로 가린 채 새 모델 준비
	if (FAILED(ctx.pManager->Replace_BattlerSlot(
		m_tDesc.iActorSide, m_tDesc.iTargetPartyIndex)))
		return E_FAIL;

	CBattle_ActionSequencer* pSeq = ctx.pManager->Get_Sequencer();
	if (nullptr == pSeq)
		return E_FAIL;

	// SPlayEffect 가 ActionData.iActorSide 로 회수/등장 이펙트 위치 결정
	pSeq->Reset_ActionData();
	BATTLE_ACTION_DATA& tData = pSeq->Get_ActionData();
	tData.iActorSide = m_tDesc.iActorSide;
	tData.iActorSlot = m_tDesc.iActorSlot;

	// 새 포켓몬 이름 추출 (CForcedSwitchState 와 동일 패턴 — Battle_States.cpp:484~500)
	CBattler* pBattler = ctx.pManager->Get_Battler(m_tDesc.iActorSide);
	const POKEMON_INSTANCE* pInstance = (nullptr != pBattler) ? pBattler->Get_Instance() : nullptr;
	const _wstring strPokemonName = (nullptr != pInstance)
		? _wstring(pInstance->szNickname) : _wstring(TEXT("?"));

	_wstring strSendOutMsg;
	if (g_kBattleSide_Opponent == m_tDesc.iActorSide)
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

	const BATTLE_ENV& tEnv = ctx.pManager->Get_Env();
	const _bool bTrainerRule =
		BATTLE_RULE::TRAINER_SINGLE == tEnv.eRule ||
		BATTLE_RULE::TRAINER_DOUBLE == tEnv.eRule;

	auto Push = [pSeq](IBattleAction_Step* pStep)
		{
			if (nullptr == pStep)
				return;

			pSeq->Push_Step(pStep);
			Safe_Release(pStep);
		};

	// 회수 단계 — 자기 진영 위치에서 ball_absorb 출력 (visible=false 인 새 모델은 안 보임)
	Push(SPlayEffect::Create(
		"ball_absorb", EFFECT_VFX_TARGET::ATTACKER,
		EFFECT_SLOT::CENTER, _float3{}));
	Push(SDelay::Create(0.3f));

	// 내보냄 단계 — INTRO / FORCED_SWITCH 와 동일 패턴
	Push(SBattleText::Create(strSendOutMsg));

	if (bTrainerRule)
		Push(STrainerThrow::Create(m_tDesc.iActorSide));

	Push(SPokemonEnter::Create(m_tDesc.iActorSide));
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