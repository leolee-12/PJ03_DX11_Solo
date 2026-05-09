#include "Battle_Session.h"

NS_BEGIN(Game_PKM)

void Reset_BattleSlot(BATTLE_SLOT& tSlot)
{
    tSlot = {};
    tSlot.iLastMoveUsed = 0;
}

void Reset_FieldState(FIELD_STATE& tField)
{
    tField = {};
}

void Reset_TurnContext(TURN_CONTEXT& tTurn)
{
    tTurn = {};
    tTurn.iTurnNumber = 1;
}

NS_END