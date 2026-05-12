#include "CommandQueue.h"
#include "IBattleCommand.h"
#include "Battle_Commands.h"
#include "Battler.h"
#include "PokemonData_Manager.h"

CCommandQueue::CCommandQueue()
{
}

HRESULT CCommandQueue::Initialize()
{
	m_vCommands.reserve(g_kMaxBattlers);
	return S_OK;
}

HRESULT CCommandQueue::Push(IBattleCommand* pCmd)
{
	if (nullptr == pCmd)
		return E_FAIL;

	Safe_AddRef(pCmd);
	m_vCommands.push_back(pCmd);

	return S_OK;
}

IBattleCommand* CCommandQueue::Pop()
{
	if (m_vCommands.empty())
		return nullptr;

	IBattleCommand* pCommand = m_vCommands.front();
	m_vCommands.erase(m_vCommands.begin());

	return pCommand;
}

void CCommandQueue::Sort(const BATTLE_CONTEXT& ctx)
{
	std::stable_sort(m_vCommands.begin(), m_vCommands.end(),
		[&](IBattleCommand* pLeft, IBattleCommand* pRight) -> bool
		{
			const _byte iLeftPriority = Resolve_Priority(pLeft, ctx);
			const _byte iRightPriority = Resolve_Priority(pRight, ctx);

			if (iLeftPriority != iRightPriority)
				return iLeftPriority > iRightPriority;

			const _ushort iLeftSpeed = Resolve_Speed(pLeft, ctx);
			const _ushort iRightSpeed = Resolve_Speed(pRight, ctx);

			if (iLeftSpeed != iRightSpeed)
			{
				if (nullptr != ctx.pField && ctx.pField->bTrickRoom)
					return iLeftSpeed < iRightSpeed;

				return iLeftSpeed > iRightSpeed;
			}

			return false;
		});
}

void CCommandQueue::Clear()
{
	for (auto& pCommand : m_vCommands)
		Safe_Release(pCommand);

	m_vCommands.clear();
}

_byte CCommandQueue::Resolve_Priority(IBattleCommand * pCmd, const BATTLE_CONTEXT & ctx)
{
	if (nullptr == pCmd)
		return 0;

	if (ACTION_TYPE::USE_MOVE != pCmd->Get_Type())
		return pCmd->Get_Priority();

	CMoveCommand* pMoveCommand = static_cast<CMoveCommand*>(pCmd);
	CBattler* pActor = ctx.Get_Self(pMoveCommand->Get_ActorSide());

	if (nullptr == pActor || nullptr == ctx.pDataMgr)
		return pCmd->Get_Priority();

	const _uint iMoveID = pActor->Get_MoveID(pMoveCommand->Get_MoveSlot());
	const MOVE_DATA* pMove = ctx.pDataMgr->Find_Move(iMoveID);

	return (nullptr != pMove) ? pMove->iPriority : pCmd->Get_Priority();
}

_ushort CCommandQueue::Resolve_Speed(IBattleCommand* pCmd, const BATTLE_CONTEXT& ctx)
{
	if (nullptr == pCmd)
		return 0;

	return pCmd->Get_ActorSpeed(ctx);
}

CCommandQueue* CCommandQueue::Create()
{
	CCommandQueue* pInstance = new CCommandQueue();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CCommandQueue");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCommandQueue::Free()
{
	Clear();

	__super::Free();
}