#pragma once
#include "Base.h"
#include "Profiler_Defines.h"

/* ------------------------------------------------------------ */
// CProfiler_Manager : CPU/GPU 측정·카운터·CSV 를 통합하는 매니저.
// CTimer_Manager 와 동일한 CBase 매니저 패턴(Create/Free, private 생성자).
// 매크로가 Client/Game_PKM 등 DLL 경계를 넘어 Get() 을 호출하므로 ENGINE_DLL export.
/* ------------------------------------------------------------ */

NS_BEGIN(Engine)

class CCpuProfiler;
class CGpuProfiler;

struct PROFILE_CONFIG
{
	_bool bMasterEnable = { false };	// 기본 OFF. F9 로 토글.
	_bool bProfileCpu   = { true };
	_bool bProfileGpu   = { true };
	_bool bCounters     = { true };
	_bool bOverlay      = { false };	// 정식 CSV 측정 시 Off 권장(P4)
	_bool bPixMarker    = { false };
};

struct PROFILE_COUNTERS
{
	// [pass][counter] 버킷. ETOUI 로 인덱싱. 프레임마다 리셋.
	_uint Values[ETOUI(EPROFILE_PASS::END)][ETOUI(EPROFILE_COUNTER::END)] = {};
};

struct PROFILE_SNAPSHOT	// 직전 완료 프레임 요약(오버레이 표시용, 1프레임 지연)
{
	_double cpuFrame = { 0.0 }, gpuFrame = { 0.0 };
	_double cpuShadow = { 0.0 }, gpuShadow = { 0.0 };
	_double cpuNonBlend = { 0.0 }, gpuNonBlend = { 0.0 };
	_uint   drawCalls = { 0 }, triangles = { 0 };
};

struct PROFILE_STATSUMMARY	// 캡처 구간 통계 요약(P5)
{
	_double dAvg = { 0.0 }, dMedian = { 0.0 }, dP95 = { 0.0 }, dP99 = { 0.0 }, dMin = { 0.0 }, dMax = { 0.0 };
	_uint   iCount = { 0 };
};

class ENGINE_DLL CProfiler_Manager final : public CBase
{
private:
	CProfiler_Manager() = default;
	virtual ~CProfiler_Manager() = default;

public:
	static CProfiler_Manager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	static CProfiler_Manager* Get();			// 매크로 전역 접근자(Free 에서 null 화)

public:
	void BeginFrame();							// CPU/GPU BeginFrame, frameIndex++
	void EndFrame();							// CPU/GPU EndFrame, GPU Resolve, CSV append

	void BeginCpuSection(const _tchar* pName);	// (P1)
	void EndCpuSection();						// (P1)
	void BeginGpuSection(const _tchar* pName);	// (P2) PIX 마커 동시 발행
	void EndGpuSection();						// (P2)

	void AddCounter(EPROFILE_COUNTER eId, _uint iValue = 1);				// (P3)
	void Record_DrawIndexed(_uint iIndexCount, D3D11_PRIMITIVE_TOPOLOGY eTopo);						// (P3)
	void Record_DrawIndexedInstanced(_uint iIndexPerInst, _uint iInstances, D3D11_PRIMITIVE_TOPOLOGY eTopo);	// (P3)
	void Record_DrawInstanced(_uint iVtxPerInst, _uint iInstances, D3D11_PRIMITIVE_TOPOLOGY eTopo);	// (P3)
	void Set_CurrentPass(EPROFILE_PASS ePass);	// (P3) 카운터 패스 버킷팅

	void StartCsvCapture(const _tchar* pFilePath);	// (P1-4)
	void StopCsvCapture();							// (P1-4)

	void RenderOverlay();						// (P4) 화면 오버레이(F10)
	void UpdateStats();							// (P4/P5) 스냅샷 + 롤링 히스토리
	void SetOverlayEnabled(_bool bEnable);		// (P4)
	void Set_OverlayFont(WNameID strFontTag);	// (P4) 오버레이 폰트 교체(기본 Font_Malgun)

	void StartCapture(_float fDurationSec, const _tchar* pScenarioName);	// (P5) F12 시간제한 캡처

public:
	void SetEnabled(_bool bEnable);
	_bool IsEnabled() const { return m_Config.bMasterEnable; }
	void Handle_Input();						// F9~F12, Update_Engine 입력 갱신 직후 호출(§6.3)

private:
	static CProfiler_Manager* s_pInstance;

	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };

	PROFILE_CONFIG			m_Config{};
	uint64_t				m_iFrameIndex = { 0 };
	EPROFILE_PASS			m_eCurrentPass = { EPROFILE_PASS::NONE };
	_bool					m_bFrameActive = { false };		// BeginFrame 시점 측정 여부 스냅샷(토글 대칭)
	PROFILE_COUNTERS		m_Counters{};					// (P3) 프레임별 카운터 버킷

	CCpuProfiler*			m_pCpuProfiler = { nullptr };	// (P1)
	CGpuProfiler*			m_pGpuProfiler = { nullptr };	// (P2)

private:
	// CSV (P1-4)
	_bool			m_bCsvCapturing = { false };
	wofstream		m_CsvFile;
	wstring			m_CsvFilePath;
	LARGE_INTEGER	m_CsvFrequency{};
	LARGE_INTEGER	m_CsvStartCounter{};
	_uint			m_iCsvRowCount = { 0 };

	// 오버레이/통계 (P4/P5)
	WNameID				m_OverlayFont = { WName(L"Font_Malgun") };
	PROFILE_SNAPSHOT	m_Snapshot{};
	vector<_double>		m_CpuFrameHistory;
	vector<_double>		m_GpuFrameHistory;

	// 시간제한 캡처/요약 (P5)
	_bool			m_bCaptureTimed = { false };
	_double			m_dCaptureDurationSec = { 0.0 };
	wstring			m_CaptureScenario;
	vector<_double>	m_CapCpuFrame;
	vector<_double>	m_CapGpuFrame;

	HRESULT Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	_double Get_CpuMs(const _tchar* pName) const;
	_double Get_GpuMs(const _tchar* pName) const;
	_uint   Get_Counter(EPROFILE_PASS ePass, EPROFILE_COUNTER eId) const;
	_uint   Get_CounterTotal(EPROFILE_COUNTER eId) const;
	void    Write_CsvHeader();
	void    Write_CsvRow();
	wstring Make_CsvFilePath() const;
	void    Push_History(vector<_double>& History, _double dValue);
	void    Compute_Stat(const vector<_double>& History, _double& dAvg, _double& dP95) const;
	PROFILE_STATSUMMARY Compute_Summary(const vector<_double>& History) const;
	void    Write_Summary();

protected:
	virtual void Free() override;
};

/* ------------------------------------------------------------ */
// RAII 스코프 헬퍼 : 헤더 inline(비 export). Get() 이 null 이면 전부 통과.
// 매니저 정의 뒤에 두어 CProfiler_Manager 가 완전한 타입인 상태로 호출.
/* ------------------------------------------------------------ */

class CScopedCpuProfile final
{
public:
	explicit CScopedCpuProfile(const _tchar* pName)
	{
		if (auto* pProfiler = CProfiler_Manager::Get())
			pProfiler->BeginCpuSection(pName);
	}
	~CScopedCpuProfile()
	{
		if (auto* pProfiler = CProfiler_Manager::Get())
			pProfiler->EndCpuSection();
	}

	CScopedCpuProfile(const CScopedCpuProfile&) = delete;
	CScopedCpuProfile& operator=(const CScopedCpuProfile&) = delete;
};

class CScopedGpuProfile final
{
public:
	explicit CScopedGpuProfile(const _tchar* pName)
	{
		if (auto* pProfiler = CProfiler_Manager::Get())
			pProfiler->BeginGpuSection(pName);
	}
	~CScopedGpuProfile()
	{
		if (auto* pProfiler = CProfiler_Manager::Get())
			pProfiler->EndGpuSection();
	}

	CScopedGpuProfile(const CScopedGpuProfile&) = delete;
	CScopedGpuProfile& operator=(const CScopedGpuProfile&) = delete;
};

NS_END
