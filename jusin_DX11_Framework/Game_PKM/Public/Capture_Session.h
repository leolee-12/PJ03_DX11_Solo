#ifndef Game_PKM_CaptureSession_h__
#define Game_PKM_CaptureSession_h__

#include "Game_PKM_Defines.h"

NS_BEGIN(Game_PKM)

struct CAPTURE_ENV
{
	_uint iSpeciesID;          // 포획 대상 야생 포켓몬 species ID
	_uint iLevel;              // 포획 대상 레벨
	_uint iInitialBallItemID;  // 시작 시 자동 선택될 볼 아이템 ID (0 = 미지정 → 인벤토리 기본)
	_uint iZoneID;             // 포획 확률 보정용 존 ID (0 = 보정 없음)
};

NS_END

#endif // Game_PKM_CaptureSession_h__