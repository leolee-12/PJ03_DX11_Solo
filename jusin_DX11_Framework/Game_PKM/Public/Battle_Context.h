#pragma once
#include "Game_PKM_Defines.h"
#include "Battle_Session.h"

NS_BEGIN(Game_PKM)

class CBattle_Manager;
class CBattler;
class CBattle_EventDispatcher;
class CPokemonData_Manager;
class CBattleMsg;

struct BATTLE_CONTEXT
{
	CBattle_Manager* pManager = { nullptr }; // weak
	CBattler* pBattlers[g_kBattleSideCount] = { nullptr, nullptr }; // weak
	FIELD_STATE* pField = { nullptr }; // weak
	TURN_CONTEXT* pTurn = { nullptr }; // weak
	CBattle_EventDispatcher* pDispatcher = { nullptr }; // weak
	CPokemonData_Manager* pDataMgr = { nullptr }; // weak
	CBattleMsg* pMsg = { nullptr }; // weak - step 이 BattleMsg::Close 등을 직접 호출하기 위한 통

	CBattler* Get_Self(_uint iSide) const
	{
		return (iSide < g_kBattleSideCount) ? pBattlers[iSide] : nullptr;
	}

	CBattler* Get_Foe(_uint iSide) const
	{
		return (iSide < g_kBattleSideCount) ? pBattlers[iSide ^ 1u] : nullptr;
	}
};

NS_END