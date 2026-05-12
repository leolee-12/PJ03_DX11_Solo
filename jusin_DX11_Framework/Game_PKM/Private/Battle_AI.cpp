#include "Battle_AI.h"
#include "Battle_Commands.h"
#include "Battler.h"

CRandomAI::CRandomAI()
{
}

IBattleCommand* CRandomAI::Decide(const BATTLE_CONTEXT& ctx, _uint iSide)
{
	CBattler* pSelf = ctx.Get_Self(iSide);
	if (nullptr == pSelf)
		return nullptr;

	_uint aValidMoves[g_kMaxMovesPerPokemon] = {};
	_uint iValidCount = 0;

	for (_uint i = 0; i < g_kMaxMovesPerPokemon; ++i)
	{
		if (0 != pSelf->Get_MoveID(i) && pSelf->Get_PP(i) > 0)
			aValidMoves[iValidCount++] = i;
	}

	CMoveCommand::DESC tDesc{};
	tDesc.iActorSide = iSide;
	tDesc.iActorSlot = 0;
	tDesc.iTargetSide = iSide ^ 1u;
	tDesc.iTargetSlot = 0;

	if (0 == iValidCount)
	{
		tDesc.iMoveSlot = 0;
		return CMoveCommand::Create(tDesc);
	}

	tDesc.iMoveSlot = aValidMoves[rand() % iValidCount];

	return CMoveCommand::Create(tDesc);
}

CRandomAI* CRandomAI::Create()
{
	return new CRandomAI();
}

void CRandomAI::Free()
{
	__super::Free();
}