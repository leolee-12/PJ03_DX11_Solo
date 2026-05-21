#pragma once
#include "Game_PKM_Defines.h"

NS_BEGIN(Game_PKM)

enum class ANIM_KIND : _ubyte
{
    IDLE = 0,
    WALK = 1,
    RUN = 2,
    TALK = 3,
    HURT = 4,
    FAINT = 5,
    INTRO = 6,
    FOCUS = 7,
    ORDER = 8,
    THROW = 9,
    SWITCH = 10,
    ATTACK_PHYSICAL = 11,
    ATTACK_SPECIAL = 12,
    IDLE_1 = 13,
    IDLE_2 = 14,
    IDLE_3 = 15,
    EVENT_1 = 16,
    EVENT_2 = 17,
    EVENT_3 = 18,

    ATTACK = ATTACK_PHYSICAL, // 기존 호출 호환용
    END = 19
};

namespace BattleAnim
{
    _uint Find_AnimIndex(WNameID strModelTag, ANIM_KIND eKind);
    ANIM_KIND Resolve_MoveAnimKind(MOVE_CATEGORY eCategory);
    _float Find_RootMotionScale(WNameID strModelTag);
}

NS_END