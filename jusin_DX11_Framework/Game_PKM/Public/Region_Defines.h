#ifndef Region_Defines_h__
#define Region_Defines_h__

#include <type_traits>
#include "Game_PKM_Defines.h"

NS_BEGIN(Game_PKM)

inline constexpr _uint INVALID_REGION_ID = 0;   // 0 = 미지정

struct REGION_RECT_DESC
{
	_uint   iRegionID = { INVALID_REGION_ID };

	_tchar  szRegionName[64] = {};               // 배너 표시용 이름
	_tchar  szBGM[128] = {};                     // Play_BGM 키 (예: L"BGM/1-04. Pallet Town Theme.mp3")
		_float  fBGMVolume = { 0.3f };               // 기존 Play_BGM 호출과 동일 기본값

	_float3 vCenter = {};                        // 월드 좌표 (XZ 사용)
	_float2 vSize = { 1.f, 1.f };                // 가로(X)/세로(Z) 전체 크기
	_float  fRotationY = { 0.f };                // 라디안

	_uint   iPriority = { 0 };                   // 사각형 겹침 시 높은 값 우선
};

static_assert(std::is_trivially_copyable_v<REGION_RECT_DESC>,
	"REGION_RECT_DESC must remain trivially copyable for future file serialization.");

// 월드 좌표가 지역 사각형(XZ, fRotationY 회전) 안에 있는지 검사.
// SpawnMath::Is_PointInsideRectXZ 와 동일한 수식이나, SPAWN_RECT_DESC 의존을 피하려 별도 정의.
inline _bool Is_PointInsideRegionXZ(const _float3& vWorldPos, const REGION_RECT_DESC& tDesc)
{
	const _vector vDelta = XMLoadFloat3(&vWorldPos) - XMLoadFloat3(&tDesc.vCenter);
	const _vector vLocal = XMVector3TransformCoord(vDelta, XMMatrixRotationY(-tDesc.fRotationY));

	_float3 vLocalF{};
	XMStoreFloat3(&vLocalF, vLocal);

	return fabsf(vLocalF.x) <= tDesc.vSize.x * 0.5f
		&& fabsf(vLocalF.z) <= tDesc.vSize.y * 0.5f;
}

NS_END

#endif // Region_Defines_h__