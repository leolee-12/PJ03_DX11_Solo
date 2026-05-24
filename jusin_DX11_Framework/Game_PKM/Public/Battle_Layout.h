#ifndef Battle_Layout_h__
#define Battle_Layout_h__

#include "Battle_Session.h"

NS_BEGIN(Game_PKM)

/*
 * Battle_Layout
 * - 한 판 배틀의 표현 계층 좌표(트레이너/포켓몬 배치 위치, Yaw)를 룰/사이드/슬롯 인덱스로 조회하는 함수 모음.
 * - 도메인 상태(Battle_Session.h)와 표현 좌표를 분리하기 위해 별도 헤더로 둔다.
 * - 현재는 싱글 룰 4종(WILD_SINGLE, TRAINER_SINGLE, CUTSCENE, TUTORIAL) 좌표만 정의.
 *   더블/트리플/레이드 추가 시 본 헤더 내부 분기만 확장.
 */

namespace BattleLayout
{
	inline constexpr _float3 vSingleTrainerPos[g_kBattleSideCount] =
	{
		/* PLAYER   */ { -2.85f, 0.f, -4.05f },
		/* OPPONENT */ {  2.85f, 0.f,  4.05f },
	};

	inline constexpr _float3 vSinglePokemonPos[g_kBattleSideCount] =
	{
		/* PLAYER   */ { -1.6f, 0.f, -2.0f },
		/* OPPONENT */ {  1.6f, 0.f,  2.0f },
	};

	inline constexpr _float fSingleTrainerYaw[g_kBattleSideCount] =
	{
		/* PLAYER   */ XM_PI / 6.f,
		/* OPPONENT */ 7.f * XM_PI / 6.f,
	};

	inline constexpr _float fSinglePokemonYaw[g_kBattleSideCount] =
	{
		/* PLAYER   */ XM_PI / 6.f,
		/* OPPONENT */ 7.f * XM_PI / 6.f,
	};

	inline _float3 Get_TrainerPos(BATTLE_RULE eRule, _uint iSide, _uint iSlotIndex = 0)
	{
		(void)iSlotIndex;

		const _uint iSafeSide = (iSide < g_kBattleSideCount) ? iSide : g_kBattleSide_Player;

		switch (eRule)
		{
		case BATTLE_RULE::WILD_SINGLE:
		case BATTLE_RULE::TRAINER_SINGLE:
		case BATTLE_RULE::CUTSCENE:
		case BATTLE_RULE::TUTORIAL:
			return vSingleTrainerPos[iSafeSide];

		case BATTLE_RULE::TRAINER_DOUBLE:
			/* TODO: 더블 배틀 슬롯 위치 추가 시 iSlotIndex 분기 */
			return vSingleTrainerPos[iSafeSide];

		default:
			return vSingleTrainerPos[g_kBattleSide_Player];
		}
	}

	inline _float3 Get_PokemonPos(BATTLE_RULE eRule, _uint iSide, _uint iSlotIndex = 0)
	{
		(void)iSlotIndex;

		const _uint iSafeSide = (iSide < g_kBattleSideCount) ? iSide : g_kBattleSide_Player;

		switch (eRule)
		{
		case BATTLE_RULE::WILD_SINGLE:
		case BATTLE_RULE::TRAINER_SINGLE:
		case BATTLE_RULE::CUTSCENE:
		case BATTLE_RULE::TUTORIAL:
			return vSinglePokemonPos[iSafeSide];

		case BATTLE_RULE::TRAINER_DOUBLE:
			/* TODO: 더블 배틀 슬롯 위치 추가 시 iSlotIndex 분기 */
			return vSinglePokemonPos[iSafeSide];

		default:
			return vSinglePokemonPos[g_kBattleSide_Player];
		}
	}

	inline _float Get_TrainerYaw(BATTLE_RULE eRule, _uint iSide, _uint iSlotIndex = 0)
	{
		(void)eRule;
		(void)iSlotIndex;

		const _uint iSafeSide = (iSide < g_kBattleSideCount) ? iSide : g_kBattleSide_Player;
		return fSingleTrainerYaw[iSafeSide];
	}

	inline _float Get_PokemonYaw(BATTLE_RULE eRule, _uint iSide, _uint iSlotIndex = 0)
	{
		(void)eRule;
		(void)iSlotIndex;

		const _uint iSafeSide = (iSide < g_kBattleSideCount) ? iSide : g_kBattleSide_Player;
		return fSinglePokemonYaw[iSafeSide];
	}
}

NS_END

#endif // Battle_Layout_h__