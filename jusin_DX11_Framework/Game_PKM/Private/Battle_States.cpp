#include "Battle_States.h"
#include "IBattleCommand.h"
#include "Battle_Manager.h"
#include "CommandQueue.h"
#include "Battle_Commands.h"
#include "Battler.h"

#include "GameInstance.h"

#pragma region IntroState
CIntroState::CIntroState()
{
}

void CIntroState::OnEnter(const BATTLE_CONTEXT& ctx)
{
    (void)ctx;
}

void CIntroState::Update(const BATTLE_CONTEXT& ctx, _float fTimeDelta)
{
    (void)fTimeDelta;

    if (nullptr != ctx.pManager)
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
    (void)ctx;
}

void CInputPlayerState::Update(const BATTLE_CONTEXT& ctx, _float fTimeDelta)
{
    (void)fTimeDelta;

    if (nullptr == ctx.pManager || nullptr == ctx.pManager->Get_Queue())
        return;

    CGameInstance* pGameInstance = CGameInstance::GetInstance();
    if (nullptr == pGameInstance)
        return;

    if (pGameInstance->Key_Down(DIK_ESCAPE))
    {
        ctx.pManager->Request_State(BATTLE_PHASE::OUTRO);
        return;
    }

    if (pGameInstance->Key_Down(DIK_RETURN))
    {
        CMoveCommand::DESC tDesc{};
        tDesc.iActorSide = g_kBattleSide_Player;
        tDesc.iActorSlot = 0;
        tDesc.iMoveSlot = 0;
        tDesc.iTargetSide = g_kBattleSide_Opponent;
        tDesc.iTargetSlot = 0;

        IBattleCommand* pCommand = CMoveCommand::Create(tDesc);
        if (nullptr == pCommand)
            return;

        if (FAILED(ctx.pManager->Get_Queue()->Push(pCommand)))
        {
            Safe_Release(pCommand);
            return;
        }

        Safe_Release(pCommand);

        ctx.pManager->Request_State(BATTLE_PHASE::INPUT_OPPONENT);
    }
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

    CMoveCommand::DESC tDesc{};
    tDesc.iActorSide = g_kBattleSide_Opponent;
    tDesc.iActorSlot = 0;
    tDesc.iMoveSlot = 0;
    tDesc.iTargetSide = g_kBattleSide_Player;
    tDesc.iTargetSlot = 0;

    IBattleCommand* pCommand = CMoveCommand::Create(tDesc);
    if (nullptr == pCommand)
        return;

    if (FAILED(ctx.pManager->Get_Queue()->Push(pCommand)))
    {
        Safe_Release(pCommand);
        return;
    }

    Safe_Release(pCommand);

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
}

void CResolveEndTurnState::Update(const BATTLE_CONTEXT& ctx, _float fTimeDelta)
{
    (void)fTimeDelta;

    if (nullptr != ctx.pTurn)
        ++ctx.pTurn->iTurnNumber;

    if (nullptr != ctx.pManager)
        ctx.pManager->Request_State(BATTLE_PHASE::CHECK_END);
}

void CResolveEndTurnState::OnExit(const BATTLE_CONTEXT& ctx)
{
    (void)ctx;
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

    if (false == bPlayerAlive || false == bOpponentAlive)
    {
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
CForcedSwitchState::CForcedSwitchState()
{
}

void CForcedSwitchState::OnEnter(const BATTLE_CONTEXT& ctx)
{
    (void)ctx;
}

void CForcedSwitchState::Update(const BATTLE_CONTEXT& ctx, _float fTimeDelta)
{
    (void)ctx;
    (void)fTimeDelta;
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
    (void)ctx;
}

void COutroState::Update(const BATTLE_CONTEXT& ctx, _float fTimeDelta)
{
    (void)fTimeDelta;

    if (nullptr != ctx.pManager)
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