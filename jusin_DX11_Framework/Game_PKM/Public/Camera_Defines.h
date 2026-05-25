#pragma once
#include "Game_PKM_Defines.h"

NS_BEGIN(Game_PKM)

enum class CAMERA_MODE : _ubyte
{
	FIELD,
	BATTLE_DEFAULT,
	CINEMATIC,
	DEBUG_FREE,
	END
};

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

enum class CAMERA_SEQUENCE_ID : _ubyte
{
	NONE,
	TACKLE_PHYSICAL,
	RANGED_ENERGY,
	AREA_WIDE,
	HIT_ONLY,
	BUFF_SELF,
	SENDOUT_PLAYER,
	SENDOUT_OPPONENT,
	TACKLE_PHYSICAL_OPPONENT,
	RANGED_ENERGY_OPPONENT,
	INTRO_TRAINER_OPPONENT,
	INTRO_SETTLE,
	SENDOUT_OPPONENT_INTRO_HOLD,
	OUTRO_TRAINER_OPPONENT_FAINT,
	END
};

struct CAMERA_POSE
{
	_float3 vPosition = {};
	_float3 vLookAt = {};
	_float3 vUp = { 0.f, 1.f, 0.f };
	_float  fFovY = 0.f;
};

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
