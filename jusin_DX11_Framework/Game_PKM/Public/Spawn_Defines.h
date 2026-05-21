#ifndef Spawn_Defines_h__
#define Spawn_Defines_h__

#include <type_traits>
#include "Game_PKM_Defines.h"

NS_BEGIN(Game_PKM)

/* ============================================================
   상수
   ============================================================ */

inline constexpr _uint INVALID_NAV_CELL = static_cast<_uint>(-1);
inline constexpr _uint NAV_AREA_WALKABLE = 0xFFFFFFFFu;   // 1차 no-op - 모든 마스크 통과
inline constexpr _uint g_kMaxSpawnAttemptsPerTry = 16;
inline constexpr _uint g_kMaxWanderAttempts = 8;

/* ============================================================
   타입
   ============================================================ */

enum class SPAWN_KIND : _ubyte
{
	WILD_POKEMON,
	TRAINER,
	NPC,
	EVENT_OBJECT,
	END
};

enum class SPAWN_NPC_PROFILE : _ubyte
{
	NONE,
	DOCTOR,
	JUVENILES,
	FAT,
	SHORTPANTS,
	NURSE,
	ROCK,
	WATER,
	PM0001_00,
	PM0004_00,
	PM0007_00,
	PM0010_00,
	PM0025_00,
	PM0041_00,
	PM0043_00,
	PM0059_00,
	PM0074_00,
	PM0095_00,
	PM0121_00,
	END
};

struct SPAWN_RECT_DESC
{
	_uint       iSpawnID = { 0 };                          // 0 = unassigned

	SPAWN_KIND  eSpawnKind = { SPAWN_KIND::WILD_POKEMON };
	SPAWN_NPC_PROFILE eNpcProfile = { SPAWN_NPC_PROFILE::NONE };
	_tchar		szDialogueKey[64] = {};
	_tchar		szEventSequenceID[64] = {};
	_uint		iTrainerID = { 0 };

	_float3     vCenter = {};                             // 월드 좌표. Y 는 검증 시 NavMesh 투영 결과로 덮임
	_float2     vSize = { 1.f, 1.f };
	_float      fRotationY = { 0.f };                        // 라디안

	_uint       iAllowedAreaMask = { NAV_AREA_WALKABLE };          // 1차 no-op

	_uint       iEncounterTableID = { 0 };                          // S2 본 단위에서는 미사용 - 본격 인카운터 테이블 도입 시 활성화
	_uint       iMaxAliveCount = { 1 };

	// TODO(S2 임시 - 본격 인카운터 테이블 도입 시 제거)
	_uint       iSpeciesID_Temp = { 0 };
	_uint       iLevel_Temp = { 1 };

	_float      fProjectRadius = { 2.f };
	_float      fLeashRadius = { 10.f };

	_float      fMinDistanceFromPlayer = { 5.f };
	_float      fMaxDistanceFromPlayer = { 40.f };

	_bool       bRequireReachable = { true };
	_bool       bSpawnOnLoad = { false };
	_bool       bRespawn = { true };

	_float      fRespawnDelay = { 10.f };
};

struct EVENT_NPC_SPAWN_DESC
{
	SPAWN_NPC_PROFILE eNpcProfile = { SPAWN_NPC_PROFILE::NONE };

	_float3 vPosition = {};
	_float fRotationY = { 0.f };

	_tchar szDialogueKey[64] = {};
	_tchar szEventSequenceID[64] = {};
};

static_assert(std::is_trivially_copyable_v<EVENT_NPC_SPAWN_DESC>,
	"EVENT_NPC_SPAWN_DESC must remain trivially copyable.");

struct SPAWN_RECT_RUNTIME
{
	SPAWN_RECT_DESC tDesc = {};

	_bool       bValid = { false };

	_float3     vProjectedCenter = {};
	_uint       iCenterCellIndex = { INVALID_NAV_CELL };

	_uint       iAliveCount = { 0 };
	_uint       iPrevAliveCount = { 0 };       // 사망 감지(Recount 가 갱신)
	_float      fRespawnTimer = { 0.f };
	_bool       bHasEverSpawned = { false };   // bRespawn=false 게이트 용
};

static_assert(std::is_trivially_copyable_v<SPAWN_RECT_DESC>,
	"SPAWN_RECT_DESC must remain trivially copyable for future memcpy serialization.");
static_assert(std::is_trivially_copyable_v<SPAWN_RECT_RUNTIME>,
	"SPAWN_RECT_RUNTIME must remain trivially copyable.");

/* ============================================================
   사각형 유틸 (Game_PKM 측 actor / 매니저가 공유 사용)
   - Engine 측 CNavigation 은 본 헤더에 의존하지 않는다.
   ============================================================ */

namespace SpawnMath
{
	inline _float RandomFloat(_float fMin, _float fMax)
	{
		if (fMin >= fMax) return fMin;
		const _float fNorm = static_cast<_float>(rand()) / static_cast<_float>(RAND_MAX);
		return fMin + (fMax - fMin) * fNorm;
	}

	inline _float3 Make_RandomPointInRect(const SPAWN_RECT_DESC& tDesc)
	{
		const _float fHalfX = tDesc.vSize.x * 0.5f;
		const _float fHalfZ = tDesc.vSize.y * 0.5f;

		const _float fLocalX = RandomFloat(-fHalfX, fHalfX);
		const _float fLocalZ = RandomFloat(-fHalfZ, fHalfZ);

		const _vector vLocal = XMVectorSet(fLocalX, 0.f, fLocalZ, 0.f);
		const _vector vRotated = XMVector3TransformCoord(vLocal, XMMatrixRotationY(tDesc.fRotationY));
		const _vector vWorld = vRotated + XMLoadFloat3(&tDesc.vCenter);

		_float3 vResult{};
		XMStoreFloat3(&vResult, vWorld);
		return vResult;
	}

	inline _bool Is_PointInsideRectXZ(const _float3& vWorldPos, const SPAWN_RECT_DESC& tDesc)
	{
		const _vector vDelta = XMLoadFloat3(&vWorldPos) - XMLoadFloat3(&tDesc.vCenter);
		const _vector vLocal = XMVector3TransformCoord(vDelta, XMMatrixRotationY(-tDesc.fRotationY));

		_float3 vLocalF{};
		XMStoreFloat3(&vLocalF, vLocal);

		return fabsf(vLocalF.x) <= tDesc.vSize.x * 0.5f
			&& fabsf(vLocalF.z) <= tDesc.vSize.y * 0.5f;
	}
}

NS_END

#endif // Spawn_Defines_h__