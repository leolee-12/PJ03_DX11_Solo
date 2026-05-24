#pragma once
#include "Base.h"
#include "Game_PKM_Defines.h"
#include "Region_Defines.h"

NS_BEGIN(Engine)
class CGameInstance;
NS_END

NS_BEGIN(Game_PKM)

class CRegion_Manager final : public CBase
{
private:
	CRegion_Manager();
	virtual ~CRegion_Manager() = default;

public:
	HRESULT Initialize();
	HRESULT Register_Region(const REGION_RECT_DESC& tDesc);
	void    Clear();

	// 매 프레임 플레이어 위치로 현재 지역 판정. 변경 시 BGM 교체 + 변경 훅 발동.
	void    Update();

	// 레벨 로드 직후 1회. 초기 지역 확정 + BGM 재생(배너 훅은 발동하지 않음).
	void    Resolve_Initial();

	// 배틀/캡처 복귀 등에서 현재 지역 BGM 재생(판정/훅 없음).
	void    Play_Current_BGM() const;

	_uint   Get_Current_RegionID() const { return m_iCurrentRegionID; }
	const REGION_RECT_DESC* Find_Region(_uint iRegionID) const;
	const REGION_RECT_DESC* Get_Current_Region() const { return Find_Region(m_iCurrentRegionID); }

	// U4 에서 사용자 배너를 연결하는 지점. 인자는 이전 지역, 새로 진입한 지역.
	void    Set_OnRegionChanged(function<void(const REGION_RECT_DESC*, const REGION_RECT_DESC&)> fnCallback)
	{
		m_fnOnRegionChanged = std::move(fnCallback);
	}

public:
	static CRegion_Manager* Create();

private:
	_bool   Get_PlayerPosition(_float3& vOut) const;
	_uint   Resolve_Region(const _float3& vPlayerPos) const;   // 겹침 시 우선순위 높은 지역
	void    Enter_Region(_uint iNewRegionID, _bool bNotify);   // BGM 교체 + (옵션)훅

private:
	CGameInstance* m_pGameInstance = { nullptr };   // weak

	vector<REGION_RECT_DESC> m_Regions;
	_uint  m_iCurrentRegionID = { INVALID_REGION_ID };

	function<void(const REGION_RECT_DESC*, const REGION_RECT_DESC&)> m_fnOnRegionChanged{};

private:
	virtual void Free() override;
};

NS_END