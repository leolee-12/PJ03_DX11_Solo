#include "Battle_Commands.h"
#include "Battler.h"
#include "PokemonData_Manager.h"
#include "Battle_Targeting.h"
#include "Battle_Math.h"
#include "Battle_Manager.h"
#include "Damage_Calculator.h"
#include "Battle_EventDispatcher.h"

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
    if (nullptr == pAttacker || nullptr == ctx.pDataMgr)
        return E_FAIL;

    const _uint iMoveID = pAttacker->Get_MoveID(m_tDesc.iMoveSlot);
    const MOVE_DATA* pMove = ctx.pDataMgr->Find_Move(iMoveID);
    if (nullptr == pMove)
        return E_FAIL;

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

    pAttacker->Consume_PP(m_tDesc.iMoveSlot);
    pAttacker->Set_LastMoveUsed(iMoveID);

    if (nullptr != ctx.pDispatcher)
    {
        EVENT_MOVE_USED tEvent{};
        tEvent.iSide = m_tDesc.iActorSide;
        tEvent.iMoveID = iMoveID;
        ctx.pDispatcher->Publish(tEvent);
    }

    BattleTargeting::TARGET_LIST tTargets{};
    BattleTargeting::Resolve(ctx, m_tDesc.iActorSide, m_tDesc.iActorSlot, *pMove, tTargets);

    for (_uint i = 0; i < tTargets.iCount; ++i)
    {
        CBattler* pDefender = tTargets.aTargets[i];
        if (nullptr == pDefender)
            continue;

        if (false == BattleMath::Roll_Accuracy(pMove->iAccuracy, pAttacker, pDefender))
        {
            if (nullptr != ctx.pDispatcher)
            {
                EVENT_MOVE_FAILED tEvent{};
                tEvent.iSide = m_tDesc.iActorSide;
                tEvent.iMoveID = iMoveID;
                tEvent.eReason = MOVE_FAIL_REASON::MISSED;
                ctx.pDispatcher->Publish(tEvent);
            }

            continue;
        }

        if (MOVE_CATEGORY::STATUS != pMove->eCategory && pMove->iPower > 0)
        {
            if (nullptr == ctx.pManager || nullptr == ctx.pManager->Get_Damage_Calculator())
                return E_FAIL;

            POKEMON_INSTANCE* pAttackerInst = pAttacker->Get_Instance();
            POKEMON_INSTANCE* pDefenderInst = pDefender->Get_Instance();

            if (nullptr == pAttackerInst || nullptr == pDefenderInst)
                continue;

            DAMAGE_PIPE_DATA tPipe{};
            tPipe.pAttacker = pAttacker;
            tPipe.pDefender = pDefender;
            tPipe.pMove = pMove;
            tPipe.pField = ctx.pField;
            tPipe.iBasePower = pMove->iPower;
            tPipe.iAttackStat = BattleMath::Pick_AttackStat(*pAttackerInst, pMove->eCategory);
            tPipe.iDefenseStat = BattleMath::Pick_DefenseStat(*pDefenderInst, pMove->eCategory);
            tPipe.iAttackerLevel = (0 == pAttackerInst->iLevel) ? 1 : pAttackerInst->iLevel;

            ctx.pManager->Get_Damage_Calculator()->Calculate(ctx, tPipe);

            if (tPipe.fEffectiveness <= 0.f)
            {
                if (nullptr != ctx.pDispatcher)
                {
                    EVENT_MOVE_FAILED tEvent{};
                    tEvent.iSide = m_tDesc.iActorSide;
                    tEvent.iMoveID = iMoveID;
                    tEvent.eReason = MOVE_FAIL_REASON::IMMUNE;
                    ctx.pDispatcher->Publish(tEvent);
                }

                continue;
            }

            const _bool bWasAlive = pDefender->Is_Alive();
            const _ushort iAppliedDamage = pDefender->Apply_Damage(tPipe.iFinalDamage);

            if (nullptr != ctx.pDispatcher)
            {
                EVENT_DAMAGE_DEALT tEvent{};
                tEvent.iTargetSide = pDefender->Get_Side();
                tEvent.iAmount = iAppliedDamage;
                tEvent.eSource = DAMAGE_SOURCE::MOVE;
                tEvent.iMoveID = iMoveID;
                tEvent.fEffectiveness = tPipe.fEffectiveness;
                tEvent.bCrit = tPipe.bCrit;
                ctx.pDispatcher->Publish(tEvent);
            }

            if (bWasAlive && false == pDefender->Is_Alive() && nullptr != ctx.pDispatcher)
            {
                EVENT_POKEMON_FAINTED tEvent{};
                tEvent.iSide = pDefender->Get_Side();
                ctx.pDispatcher->Publish(tEvent);
            }
        }

        BattleMath::Apply_MoveEffect(ctx, *pMove, pAttacker, pDefender);
    }

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