#pragma once
#include "Base.h"
#include "Battle_Events.h"

NS_BEGIN(Game_PKM)

class IBattleEventListener abstract : public CBase
{
protected:
    IBattleEventListener() = default;
    virtual ~IBattleEventListener() = default;

public:
    virtual void On_BattleEvent(const BATTLE_EVENT_BASE& tEvent) PURE;
};

NS_END