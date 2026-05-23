#pragma once
#include "Game_PKM_Defines.h"

NS_BEGIN(Game_PKM)

enum class EVENT_TRIGGER : _ubyte
{
	NONE,
	TALK,
	TOUCH,
	TRIGGER_ENTER,
	AUTO_ON_LOAD,
	MANUAL,
	END
};

enum class EVENT_ACTION_KIND : _ubyte
{
	NONE,

	LOCK_INPUT,
	RESTORE_INPUT,

	WAIT_SECONDS,
	WAIT_DIALOGUE,

	MESSAGE_KEY,
	MESSAGE_TEXT,

	CAMERA_PUSH,
	CAMERA_POP,
	CAMERA_SET_POSE,
	CAMERA_MOVE_TO,
	CAMERA_BLEND_TO_ACTOR,
	CAMERA_FOLLOW_ACTOR,

	ACTOR_FACE,
	ACTOR_FACE_YAW,
	ACTOR_FACE_CAMERA,
	ACTOR_MOVE_TO,
	ACTOR_SET_ANIM,
	ACTOR_SET_VISIBLE,

	SPAWN_NPC,
	DESPAWN_ACTOR,
	BIND_ACTOR_BY_SPAWN_ID,

	REQUEST_BATTLE,

	DEBUG_LOG,
	END
};

enum class EVENT_STEP_MODE : _ubyte
{
	SEQUENTIAL,
	PARALLEL,
	END
};

enum class EVENT_PLAY_STATE : _ubyte
{
	IDLE,
	PLAYING,
	WAITING,
	FINISHED,
	FAILED,
	CANCELED,
	END
};

struct EVENT_STEP_DESC
{
	EVENT_ACTION_KIND eKind = { EVENT_ACTION_KIND::NONE };
	unordered_map<_string, _string> Params;
};

struct EVENT_STEP_GROUP
{
	EVENT_STEP_MODE eMode = { EVENT_STEP_MODE::SEQUENTIAL };
	vector<EVENT_STEP_DESC> Steps;
};

struct CAMERA_EVENT_SNAPSHOT
{
	_bool bValid = { false };

	_bool bFollowing = { false };
	_bool bControlEnabled = { false };

	_float3 vEye = {};
	_float3 vAt = {};
};

NS_END