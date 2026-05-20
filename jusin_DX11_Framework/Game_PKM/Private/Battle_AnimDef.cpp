#include "Battle_AnimDef.h"
#include "Game_PKM_Tags.h"

namespace
{
	struct ANIM_INDEX_ENTRY
	{
		WNameID strModelTag;
		_uint   iIndex[ETOUI(ANIM_KIND::END)];
	};

	// { IDLE, WALK, TALK, ATTACK_PHYSICAL, ATTACK_SPECIAL, HURT, FAINT, ENTER }
	const constexpr ANIM_INDEX_ENTRY s_AnimTable[] =
	{
		{ PROTO_COM_MODEL_PM0001_00,		{ 0, 0, 0, 27, 27, 34, 38, 4 } },
		{ PROTO_COM_MODEL_PM0004_00,		{ 0, 0, 0, 30, 30, 37, 41, 4 } },
		{ PROTO_COM_MODEL_PM0007_00,		{ 0, 0, 0, 33, 33, 40, 44, 4 } },
		{ PROTO_COM_MODEL_PM0025_00,		{ 0, 0, 0, 31, 31, 36, 41, 14 } },

		{ PROTO_COM_MODEL_HERO,				{ 17, 76, 17, 17, 17, 17, 17, 17 } },
		{ PROTO_COM_MODEL_HEROINE,			{ 17, 76, 17, 17, 17, 17, 17, 17 } },
		{ PROTO_COM_MODEL_DOCTOR,			{ 0, 17, 5, 0, 0, 0, 0, 0 } },
		{ PROTO_COM_MODEL_PPL_JUVENILES,	{ 2, 11, 7, 0, 0, 0, 0, 0 } },
		{ PROTO_COM_MODEL_PPL_FAT,			{ 0, 1, 2, 0, 0, 0, 0, 0 } },
		{ PROTO_COM_MODEL_PPL_SHORTPANTS,	{ 3, 5, 3, 0, 0, 0, 12, 14 } },	// 7 Throw, 11 order, 15 Focus
		{ PROTO_COM_MODEL_PPL_NURSE,		{ 3, 8, 5, 0, 0, 0, 0, 0 } }, // 6 give
		{ PROTO_COM_MODEL_PPL_ROCK,			{ 11, 10, 8, 0, 0, 0, 14, 6 } },	// 1 order 17 throw
		{ PROTO_COM_MODEL_PPL_WATER,		{ 0, 1, 0, 0, 0, 0, 0, 0 } },
	};

	constexpr _uint s_DefaultIndex[ETOUI(ANIM_KIND::END)] =
	{
		0, // IDLE
		1, // WALK
		0, // TALK
		1, // ATTACK_PHYSICAL 
		1, // ATTACK_SPECIAL
		2, // HURT
		3, // FAINT
		4, // ENTER
	};
}

_uint BattleAnim::Find_AnimIndex(WNameID strModelTag, ANIM_KIND eKind)
{
	if (eKind >= ANIM_KIND::END)
		return 0;

	const size_t iKindIdx = static_cast<size_t>(eKind);

	for (const auto& tEntry : s_AnimTable)
	{
		if (tEntry.strModelTag == strModelTag)
			return tEntry.iIndex[iKindIdx];
	}

	return s_DefaultIndex[iKindIdx];
}

ANIM_KIND BattleAnim::Resolve_MoveAnimKind(MOVE_CATEGORY eCategory)
{
	switch (eCategory)
	{
	case MOVE_CATEGORY::PHYSICAL:
		return ANIM_KIND::ATTACK_PHYSICAL;

	case MOVE_CATEGORY::SPECIAL:
		return ANIM_KIND::ATTACK_SPECIAL;

	case MOVE_CATEGORY::STATUS:
		return ANIM_KIND::TALK;

	default:
		return ANIM_KIND::ATTACK_PHYSICAL;
	}
}