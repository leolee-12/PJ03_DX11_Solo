#pragma once
#include "Base.h"
#include "Profiler_Defines.h"
#include <wrl/client.h>
#include <d3d11_1.h>

/* ------------------------------------------------------------ */
// CGpuProfiler : D3D11 Timestamp/Disjoint Query 기반 GPU 구간 측정.
// 프레임을 Disjoint 로 감싸고, 구간마다 Timestamp 두 개(Begin/End)를 End() 로 찍는다.
// 링버퍼(RING_COUNT 슬롯)로 2~수 프레임 지연 후 비차단 GetData 회수.
// ComPtr 사용은 의도적 예외(사용자 승인) — 다수 Query 보유·재사용의 누수 위험 축소.
/* ------------------------------------------------------------ */

NS_BEGIN(Engine)

struct GPU_PROFILE_SECTION
{
	const _tchar*						pName = { nullptr };
	_uint								iDepth = { 0 };
	_double								dElapsedMs = { 0.0 };
	Microsoft::WRL::ComPtr<ID3D11Query>	pBeginQuery;
	Microsoft::WRL::ComPtr<ID3D11Query>	pEndQuery;
};

struct GPU_PROFILE_FRAME
{
	uint64_t							iFrameIndex = { 0 };
	Microsoft::WRL::ComPtr<ID3D11Query>	pDisjointQuery;
	vector<GPU_PROFILE_SECTION>			Sections;
	_uint								iUsedSections = { 0 };
	_bool								bSubmitted = { false };	// End(disjoint) 제출됨
	_bool								bResolved  = { false };	// GetData 회수 시도 완료
	_bool								bValid     = { false };	// 회수 성공 + Disjoint 아님
};

class ENGINE_DLL CGpuProfiler final : public CBase
{
private:
	CGpuProfiler() = default;
	virtual ~CGpuProfiler() = default;

public:
	static CGpuProfiler* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

public:
	void BeginFrame(uint64_t iFrameIndex);
	void EndFrame();
	void BeginSection(const _tchar* pName);		// Timestamp 는 End 로 찍는다(F2)
	void EndSection();
	void ResolveOldFrames();					// 비차단 GetData, 미준비 시 skip(F1)
	const GPU_PROFILE_FRAME* Get_LatestResolved() const;

private:
	static const _uint		RING_COUNT   = 6;
	static const _uint		MAX_SECTIONS = 32;

	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };
	Microsoft::WRL::ComPtr<ID3DUserDefinedAnnotation>	m_pAnnotation;	// PIX/RenderDoc 마커(선택)

	GPU_PROFILE_FRAME		m_Frames[RING_COUNT];
	_uint					m_iCurrentSlot = { 0 };
	_bool					m_bFrameOpen = { false };	// 현재 슬롯 Disjoint Begin 발행됨
	vector<_uint>			m_OpenStack;				// 중첩 구간 인덱스 스택

private:
	HRESULT Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	HRESULT Create_Query(D3D11_QUERY eType, ID3D11Query** ppOut);
	void    Resolve_Frame(GPU_PROFILE_FRAME& Frame);

protected:
	virtual void Free() override;
};

NS_END
