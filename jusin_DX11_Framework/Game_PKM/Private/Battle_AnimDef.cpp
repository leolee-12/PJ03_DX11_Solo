#include "Battle_AnimDef.h"
#include "Game_PKM_Tags.h"

namespace
{
	struct ANIM_INDEX_ENTRY
	{
		WNameID strModelTag;
		_uint   iIndex[ETOUI(ANIM_KIND::END)];
	};

	/* 모델 태그별 anim 인덱스 매핑.
	   * 임시값 - 실제 모델 데이터의 anim 인덱스에 맞춰 사용자가 조정해야 한다.
	   * 사용자 검증 절차:
		   1) 모델을 화면에서 확인하며 Set_AnimationIndex(N) 호출로 N=0,1,2,...
		   2) 어느 인덱스가 IDLE / ATTACK / HURT / FAINT 인지 식별
		   3) 본 테이블의 값을 그 인덱스로 교체 */
	constexpr ANIM_INDEX_ENTRY s_AnimTable[] =
	{
		{ PROTO_COM_MODEL_PM0001_00, { 0, 27, 34, 38, 4 } },  // 이상해씨
		{ PROTO_COM_MODEL_PM0004_00, { 0, 30, 37, 41, 4 } },  // 파이리
		{ PROTO_COM_MODEL_PM0007_00, { 0, 33, 40, 44, 4 } },  // 꼬부기
		{ PROTO_COM_MODEL_PM0025_00, { 0, 31, 36, 41, 4 } },  // 피카츄
	};

	constexpr _uint s_DefaultIndex[ETOUI(ANIM_KIND::END)] =
	{
			0,  // IDLE
			1,  // ATTACK
			2,  // HURT
			3,  // FAINT
			4,  // ENTER
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