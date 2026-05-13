#pragma once
#include "Game_PKM_Defines.h"

NS_BEGIN(Game_PKM)

/* ANIM_KIND
   - 배틀에서 사용하는 anim 의 시맨틱 식별자.
   - 실제 anim 인덱스는 모델 태그 단위로 lookup 테이블에서 조회. */
    enum class ANIM_KIND : _ubyte
{
    IDLE,
    ATTACK,
    HURT,
    FAINT,
    ENTER,
    END
};

namespace BattleAnim
{
    /* 모델 태그 + anim kind 로 실제 모델 anim 인덱스 조회.
       매핑되지 않은 모델은 기본값 반환 (0=IDLE / 1=ATTACK / 2=HURT / 3=FAINT / 4=ENTER 가정). */
    _uint Find_AnimIndex(WNameID strModelTag, ANIM_KIND eKind);
}

NS_END