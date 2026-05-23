#include "Battle_AnimDef.h"
#include "Game_PKM_Tags.h"

namespace
{
	struct ANIM_INDEX_ENTRY
	{
		WNameID strModelTag;
		_uint   iIndex[ETOUI(ANIM_KIND::END)];
	};
	//	0		1	2		3	4		5	6		7		8		9	10		11		12		13		14		15		16		17		18
	// {IDLE, WALK, RUN, TALK, HURT, FAINT, INTRO, FOCUS, ORDER	THROW, SWITCH, ATK_P, ATK_S, IDLE_1, IDLE_2, IDLE_3, EVENT_1, EVENT_2, EVENT_3, END }

	const constexpr ANIM_INDEX_ENTRY s_AnimTable[] =
	{										//0		1	2	3	4	5	6	7	8	9	10	11	12	13	14	15	16	17	18
		{ PROTO_COM_MODEL_PM0001_00,		{ 0,	6,	7,	0,	33,	38,	9,	0,	0,	0,	0,	28,	29,	0,	0,	0,	0,	0,	0} },
		{ PROTO_COM_MODEL_PM0004_00,		{ 0,	6,	7,	0,	36,	41,	10,	0,	0,	0,	0,	30,	28,	0,	0,	0,	0,	0,	0} },
		{ PROTO_COM_MODEL_PM0007_00,		{ 0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0} },
		{ PROTO_COM_MODEL_PM0010_00,		{ 4,	8,	9,	0,	6,	7,	2,	0,	0,	0,	0,	3,	5,	3,	14,	15,	0,	0,	0} },
		{ PROTO_COM_MODEL_PM0025_00,		{ 0,	5,	6,	0,	36,	41,	14,	0,	0,	0,	0,	31,	32,	1,	2,	27,	20,	0,	0} },
		{ PROTO_COM_MODEL_PM0041_00,		{ 2,	8,	9,	0,	5,	6,	1,	0,	0,	0,	0,	3,	4,	10,	11,	12,	0,	0,	0} },
		{ PROTO_COM_MODEL_PM0043_00,		{ 0,	6,	7,	0,	37,	42,	10,	0,	0,	0,	0,	31,	33,	1,	2,	3,	0,	0,	0} },
		{ PROTO_COM_MODEL_PM0059_00,		{ 1,	6,	7,	0,	33,	38,	10,	0,	0,	0,	0,	28,	29,	0,	0,	0,	0,	0,	0} },
		{ PROTO_COM_MODEL_PM0074_00,		{ 0,	6,	7,	0,	33,	38,	10,	0,	0,	0,	0,	28,	29,	0,	0,	0,	0,	0,	0} },
		{ PROTO_COM_MODEL_PM0095_00,		{ 13,	14,	15,	0,	10,	11,	6,	0,	0,	0,	0,	8,	9,	0,	0,	0,	0,	0,	0} },
		{ PROTO_COM_MODEL_PM0121_00,		{ 13,	14,	15,	0,	11,	12,	20,	0,	0,	0,	0,	9,	10,	0,	0,	0,	0,	0,	0} },
		{ PROTO_COM_MODEL_HERO,				{ 17,	76,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0} },
		{ PROTO_COM_MODEL_HEROINE,			{ 0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0} },
		{ PROTO_COM_MODEL_DOCTOR,			{ 0,	17,	3,	5,	0,	0,	0,	0,	0,	0,	0,	0,	0,	11,	0,	0,	0,	0,	0} },
		{ PROTO_COM_MODEL_PPL_JUVENILES,	{ 2,	11,	13,	7,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0} },
		{ PROTO_COM_MODEL_PPL_FAT,			{ 0,	12,	2,	1,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0} },
		{ PROTO_COM_MODEL_PPL_NURSE,		{ 3,	8,	4,	27,	0,	0,	0,	0,	0,	0,	0,	0,	0,	6,	5,	0,	0,	0,	0} },
		{ PROTO_COM_MODEL_PPL_SHORTPANTS,	{ 3,	5,	0,	27,	13,	16,	15,	18,	11,	7,	12,	0,	0,	0,	0,	0,	0,	0,	0} },
		{ PROTO_COM_MODEL_PPL_ROCK,			{ 11,	10,	21,	8,	0,	14,	22,	7,	1,	17,	0,	0,	0,	0,	0,	0,	0,	0,	0} },
		{ PROTO_COM_MODEL_PPL_WATER,		{ 3,	19,	0,	6,	12,	2,	14,	8,	1,	7,	1,	0,	0,	0,	0,	0,	0,	0,	0} },	// 일부 모델 anim 회전은 s_AnimRotationCorrections에서 임시 보정
	};

	constexpr _uint s_DefaultIndex[ETOUI(ANIM_KIND::END)] = {};

	enum class ANIM_ROTATION_AXIS : _ubyte { X, Y, Z };

	struct ANIM_ROTATION_CORRECTION_ENTRY
	{
		WNameID strModelTag;
		_uint iAnimIndex;
		ANIM_ROTATION_AXIS eAxis;
		_float fDegree;
	};

	const constexpr ANIM_ROTATION_CORRECTION_ENTRY s_AnimRotationCorrections[] =
	{
		// INTRO -> THROW 전환 중 스냅은 카메라 연출로 가린다.
		{ PROTO_COM_MODEL_PPL_WATER, 7, ANIM_ROTATION_AXIS::X, -90.f },	// WATER 7번 Anim : THROW
		{ PROTO_COM_MODEL_PPL_WATER, 8, ANIM_ROTATION_AXIS::X, -90.f },	// WATER 8번 Anim : FOCUS
	};

	struct ROOT_MOTION_SCALE_ENTRY
	{
		WNameID strModelTag;
		_float  fScale;
	};

	const constexpr ROOT_MOTION_SCALE_ENTRY s_RootMotionScaleTable[] =
	{
			{ PROTO_COM_MODEL_PM0001_00, 0.3f   },
			{ PROTO_COM_MODEL_PM0004_00, 0.3f   },
			{ PROTO_COM_MODEL_PM0007_00, 0.3f   },
			{ PROTO_COM_MODEL_PM0010_00, 0.2f   },
			{ PROTO_COM_MODEL_PM0025_00, 0.3f   },
			{ PROTO_COM_MODEL_PM0041_00, 0.013f },
			{ PROTO_COM_MODEL_PM0043_00, 0.5f   },
			{ PROTO_COM_MODEL_PM0059_00, 0.3f   },
			{ PROTO_COM_MODEL_PM0074_00, 0.3f   },
			{ PROTO_COM_MODEL_PM0095_00, 0.3f   },
			{ PROTO_COM_MODEL_PM0121_00, 0.3f   },
	};

	constexpr _float kDefaultRootMotionScale = 0.3f;
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

_float BattleAnim::Find_RootMotionScale(WNameID strModelTag)
{
	for (const auto& tEntry : s_RootMotionScaleTable)
	{
		if (tEntry.strModelTag == strModelTag)
			return tEntry.fScale;
	}
	return kDefaultRootMotionScale;
}

_matrix BattleAnim::Find_AnimRotationCorrection(WNameID strModelTag, _uint iAnimIndex)
{
	for (const auto& tEntry : s_AnimRotationCorrections)
	{
		if (tEntry.strModelTag != strModelTag || tEntry.iAnimIndex != iAnimIndex)
			continue;

		const _float fRadian = XMConvertToRadians(tEntry.fDegree);

		switch (tEntry.eAxis)
		{
		case ANIM_ROTATION_AXIS::X:
			return XMMatrixRotationX(fRadian);

		case ANIM_ROTATION_AXIS::Y:
			return XMMatrixRotationY(fRadian);

		case ANIM_ROTATION_AXIS::Z:
			return XMMatrixRotationZ(fRadian);

		default:
			return XMMatrixIdentity();
		}
	}

	return XMMatrixIdentity();
}