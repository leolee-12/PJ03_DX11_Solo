#pragma once
#include "Game_PKM_Defines.h"

NS_BEGIN(Game_PKM)

/* Director가 매 프레임 어떤 상태로 카메라를 갱신하는지 표현.
   FIELD / DEBUG_FREE 에서는 Director가 휴면, Camera_Free 자체 입력/follow가 transform 소유. */
	enum class CAMERA_MODE : _ubyte
{
	FIELD,
	BATTLE_DEFAULT,
	CINEMATIC,
	DEBUG_FREE,
	END
};

/* CAMERA_SHOT_DESC.eType. M3 Sequence 재생기 도입 시 본격 활용,
   M1 단계에서는 enum 정의만 두고 분기는 stub. */
enum class CAMERA_SHOT_TYPE : _ubyte
{
	CUT,
	BLEND_TO,
	FOLLOW_LOOKAT,
	ORBIT,
	DOLLY,
	SHAKE_ONLY,
	RETURN_DEFAULT,
	END
};

/* M4 Target 해석기에서 사용. ATTACKER/DEFENDER 는 ActionData 기반,
   BATTLE_CENTER 는 양 슬롯 평균, ABSOLUTE 는 offset 자체가 월드 좌표. */
enum class CAMERA_TARGET_TYPE : _ubyte
{
	NONE,
	ATTACKER,
	DEFENDER,
	BATTLE_CENTER,
	PLAYER_TRAINER,
	OPPONENT_TRAINER,
	PLAYER_POKEMON,
	OPPONENT_POKEMON,
	ABSOLUTE_CAMERA,
	END
};

/* 회전은 Euler가 아닌 LookAt 으로 표현(분석서 §10 문제 2 회피).
   fFovY = 0 은 "이 Pose 는 FOV 를 변경하지 않음" 약속(§4.3, M6까지 모든 Pose 가 0 유지). */
struct CAMERA_POSE
{
	_float3 vPosition = {};
	_float3 vLookAt = {};
	_float3 vUp = { 0.f, 1.f, 0.f };
	_float  fFovY = 0.f;
};

/* M3 Sequence 가 보유할 한 컷의 정의. M1 단계에서는 사용하지 않으나
   헤더 한 번에 등록하고, 사용 분기는 후속 마일스톤에서 추가. */
struct CAMERA_SHOT_DESC
{
	CAMERA_SHOT_TYPE   eType = CAMERA_SHOT_TYPE::BLEND_TO;
	_float             fDuration = 0.5f;
	_float             fBlendTime = 0.3f;
	CAMERA_TARGET_TYPE eFollowTarget = CAMERA_TARGET_TYPE::NONE;
	CAMERA_TARGET_TYPE eLookAtTarget = CAMERA_TARGET_TYPE::NONE;
	_float3            vPositionOffset = {};
	_float3            vLookAtOffset = {};
	_float             fStartFov = 0.f;
	_float             fEndFov = 0.f;
	_bool              bUseShake = false;
	_float             fShakePower = 0.f;
	_float             fShakeFrequency = 30.f;
	_float             fShakeDuration = 0.12f;
};

NS_END