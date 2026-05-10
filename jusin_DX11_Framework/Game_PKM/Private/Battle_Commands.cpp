#include "Battle_Commands.h"
#include "Battler.h"
#include "PokemonData_Manager.h"
#include "Battle_Targeting.h"
#include "Battle_Math.h"
#include "Battle_Manager.h"

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
        return S_OK;

    pAttacker->Consume_PP(m_tDesc.iMoveSlot);
    pAttacker->Set_LastMoveUsed(iMoveID);

    BattleTargeting::TARGET_LIST tTargets{};
    BattleTargeting::Resolve(ctx, m_tDesc.iActorSide, m_tDesc.iActorSlot, *pMove, tTargets);

    for (_uint i = 0; i < tTargets.iCount; ++i)
    {
        CBattler* pDefender = tTargets.aTargets[i];
        if (nullptr == pDefender)
            continue;

        if (false == BattleMath::Roll_Accuracy(pMove->iAccuracy, pAttacker, pDefender))
            continue;

        if (MOVE_CATEGORY::STATUS != pMove->eCategory && pMove->iPower > 0)
        {
            pDefender->Apply_Damage(10);
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
        return S_OK;

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