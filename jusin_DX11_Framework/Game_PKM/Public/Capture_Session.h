#ifndef Game_PKM_CaptureSession_h__
#define Game_PKM_CaptureSession_h__

#include "Game_PKM_Defines.h"

NS_BEGIN(Game_PKM)

enum class CAPTURE_RESULT : _ubyte
{
    NONE,           // 미결정 (RESULT 페이즈 이전)
    SUCCESS,        // 포획 성공
    FAIL_BREAK,     // 볼 탈출 (확률 실패)
    FAIL_RUN,       // 포켓몬 도주 (특정 조건)
    END
};

struct CAPTURE_ENV
{
	_uint iSpeciesID;          // 포획 대상 야생 포켓몬 species ID
	_uint iLevel;              // 포획 대상 레벨
	_uint iInitialBallItemID;  // 시작 시 자동 선택될 볼 아이템 ID (0 = 미지정 -> 인벤토리 기본)
	_uint iZoneID;             // 포획 확률 보정용 존 ID (0 = 보정 없음)
};

enum class CAPTURE_PHASE : _ubyte
{
    INTRO,
    AIMING,
    THROWING,
    MISS_VIEW,
    STAGE,
    DROP,
    SHAKE,
    SUCCESS_VIEW,
    BREAK_VIEW,
    DONE,
    END
};

NS_END

#endif // Game_PKM_CaptureSession_h__