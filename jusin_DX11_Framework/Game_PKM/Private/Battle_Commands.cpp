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

    const _uint iMoveID = pAttacker->Get_MoveID(m_tDesc.iMoveSlot);
    const MOVE_DATA* pMove = ctx.pDataMgr->Find_Move(iMoveID);
    if (nullptr == pMove)
        return E_FAIL;

    // PP 0 — 시퀀스 빌드 없이 즉시 메시지 발행 후 종료
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

    // 타겟 결정 (싱글배틀 — 첫 번째 타겟만 사용)
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

    Push(SAnnounce::Create(m_tDesc.iActorSide, iMoveID));
    Push(SDelay::Create(0.3f));
    Push(SCloseMsg::Create());
    Push(SDelay::Create(0.2f));
    Push(SAccuracyCheck::Create());
    Push(SMissMessage::Create(m_tDesc.iActorSide, iMoveID));
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

    // 부가 효과(상태이상 등) 처리 — 본 트랙 보류, 추후 별도 step (SApplyEffect) 으로 통합 예정
    // BattleMath::Apply_MoveEffect(ctx, *pMove, pAttacker, pDefender);  // stub

    return S_OK;
}

CMoveCommand * CMoveCommand::Create(const DESC & tDesc)
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
    (void)ctx;
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