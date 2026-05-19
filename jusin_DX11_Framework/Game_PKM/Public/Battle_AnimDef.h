#pragma once
#include "Game_PKM_Defines.h"

NS_BEGIN(Game_PKM)

enum class ANIM_KIND : _ubyte
{
    IDLE = 0,
    WALK = 1,
    TALK = 2,

    ATTACK_PHYSICAL = 3,
    ATTACK_SPECIAL = 4,

    HURT = 5,
    FAINT = 6,
    ENTER = 7,

    ATTACK = ATTACK_PHYSICAL, // 기존 호출 호환용
    END = 8
};

namespace BattleAnim
{
    _uint Find_AnimIndex(WNameID strModelTag, ANIM_KIND eKind);
    ANIM_KIND Resolve_MoveAnimKind(MOVE_CATEGORY eCategory);
}

NS_END