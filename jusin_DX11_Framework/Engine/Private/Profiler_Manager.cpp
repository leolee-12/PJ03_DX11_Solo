#include "Profiler_Manager.h"
#include "CpuProfiler.h"
#include "GpuProfiler.h"
#include "GameInstance.h"
#include <filesystem>

namespace
{
	// CSV CPU 컬럼 순서. 미측정 항목은 0 으로 기록된다.
	const _tchar* s_CpuColumns[] = {
		L"CPU_Update_Total", L"CPU_Render_Total", L"CPU_BeginDraw", L"CPU_Draw_Total",
		L"CPU_Renderer_Draw", L"CPU_Level_Render", L"CPU_Present",
		L"CPU_Render_Priority", L"CPU_Render_Shadow", L"CPU_Render_NonBlend", L"CPU_Render_OutlineMask",
		L"CPU_Render_Lights", L"CPU_Render_Combined", L"CPU_Render_PostProcess", L"CPU_Render_NonLight",
		L"CPU_Render_Blend", L"CPU_Render_UI", L"CPU_Render_Debug",
		L"CPU_RenderProfile_BindAndDraw", L"CPU_Model_BindBoneMatrices", L"CPU_Mesh_BindBoneMatrices",
		L"CPU_Model_BindMaterial", L"CPU_Draw_Text", L"CPU_Render_ProfilerOverlay",
	};

	// CSV GPU 컬럼. (패스별 GPU 는 P2-3 에서 추가)
	const _tchar* s_GpuColumns[] = {
		L"GPU_Frame_Total",
		L"GPU_Render_Shadow", L"GPU_Render_NonBlend", L"GPU_Render_Lights",
		L"GPU_Render_Combined", L"GPU_Render_Blend", L"GPU_Render_UI",
	};

	// topology 별 삼각형 수. line/point/patch 는 0(삼각형 오집계 방지).
	_uint Triangles_From(_uint iCount, D3D11_PRIMITIVE_TOPOLOGY eTopo)
	{
		switch (eTopo)
		{
		case D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST:  return iCount / 3;
		case D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP: return (iCount >= 3) ? (iCount - 2) : 0;
		default:                                     return 0;
		}
	}
}

CProfiler_Manager* CProfiler_Manager::s_pInstance = { nullptr };

CProfiler_Manager* CProfiler_Manager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CProfiler_Manager* pInstance = new CProfiler_Manager();

	if (FAILED(pInstance->Initialize(pDevice, pContext)))
	{
		MSG_BOX("Failed to Created : CProfiler_Manager");
		Safe_Release(pInstance);
		return nullptr;
	}

	s_pInstance = pInstance;

	OutputDebugStringW(L"[Profiler] Manager created (F9=master On/Off, F11=CSV)\n");

	return pInstance;
}

CProfiler_Manager* CProfiler_Manager::Get()
{
	return s_pInstance;
}

HRESULT CProfiler_Manager::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	m_pDevice = pDevice;
	m_pContext = pContext;
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);

	QueryPerformanceFrequency(&m_CsvFrequency);

	m_pCpuProfiler = CCpuProfiler::Create();
	if (nullptr == m_pCpuProfiler)
		return E_FAIL;

	m_pGpuProfiler = CGpuProfiler::Create(m_pDevice, m_pContext);
	if (nullptr == m_pGpuProfiler)
		return E_FAIL;

	return S_OK;
}

void CProfiler_Manager::BeginFrame()
{
	m_bFrameActive = m_Config.bMasterEnable;	// 측정 여부 스냅샷(프레임 중간 토글에도 대칭 유지)
	if (false == m_bFrameActive)
		return;

	++m_iFrameIndex;
	m_eCurrentPass = EPROFILE_PASS::NONE;
	m_Counters = {};

	if (true == m_Config.bProfileCpu && nullptr != m_pCpuProfiler)
		m_pCpuProfiler->BeginFrame(m_iFrameIndex);

	if (true == m_Config.bProfileGpu && nullptr != m_pGpuProfiler)
		m_pGpuProfiler->BeginFrame(m_iFrameIndex);
}

void CProfiler_Manager::EndFrame()
{
	if (false == m_bFrameActive)
		return;

	if (true == m_Config.bProfileCpu && nullptr != m_pCpuProfiler)
		m_pCpuProfiler->EndFrame();

	if (true == m_Config.bProfileGpu && nullptr != m_pGpuProfiler)
	{
		m_pGpuProfiler->EndFrame();
		m_pGpuProfiler->ResolveOldFrames();
	}

	UpdateStats();

	if (true == m_bCsvCapturing && true == m_CsvFile.is_open())
	{
		Write_CsvRow();

		m_CapCpuFrame.push_back(m_Snapshot.cpuFrame);
		m_CapGpuFrame.push_back(m_Snapshot.gpuFrame);

		if (true == m_bCaptureTimed)
		{
			LARGE_INTEGER Now{};
			QueryPerformanceCounter(&Now);
			const _double dElapsed = (0 < m_CsvFrequency.QuadPart)
				? static_cast<_double>(Now.QuadPart - m_CsvStartCounter.QuadPart) / static_cast<_double>(m_CsvFrequency.QuadPart)
				: 0.0;
			if (dElapsed >= m_dCaptureDurationSec)
				StopCsvCapture();
		}
	}

	m_bFrameActive = false;
}

void CProfiler_Manager::BeginCpuSection(const _tchar* pName)
{
	if (false == m_bFrameActive || false == m_Config.bProfileCpu)
		return;

	if (nullptr != m_pCpuProfiler)
		m_pCpuProfiler->BeginSection(pName);
}

void CProfiler_Manager::EndCpuSection()
{
	if (nullptr != m_pCpuProfiler)
		m_pCpuProfiler->EndSection();
}

void CProfiler_Manager::BeginGpuSection(const _tchar* pName)
{
	if (false == m_bFrameActive || false == m_Config.bProfileGpu)
		return;

	if (nullptr != m_pGpuProfiler)
		m_pGpuProfiler->BeginSection(pName);
}

void CProfiler_Manager::EndGpuSection()
{
	if (nullptr != m_pGpuProfiler)
		m_pGpuProfiler->EndSection();
}

void CProfiler_Manager::AddCounter(EPROFILE_COUNTER eId, _uint iValue)
{
	if (false == m_bFrameActive || false == m_Config.bCounters)
		return;

	const _uint iPass = ETOUI(m_eCurrentPass);
	const _uint iId = ETOUI(eId);
	if (iPass < ETOUI(EPROFILE_PASS::END) && iId < ETOUI(EPROFILE_COUNTER::END))
		m_Counters.Values[iPass][iId] += iValue;
}

void CProfiler_Manager::Record_DrawIndexed(_uint iIndexCount, D3D11_PRIMITIVE_TOPOLOGY eTopo)
{
	if (false == m_bFrameActive || false == m_Config.bCounters)
		return;

	AddCounter(EPROFILE_COUNTER::DRAWCALLS, 1);
	AddCounter(EPROFILE_COUNTER::TRIANGLES, Triangles_From(iIndexCount, eTopo));
}

void CProfiler_Manager::Record_DrawIndexedInstanced(_uint iIndexPerInst, _uint iInstances, D3D11_PRIMITIVE_TOPOLOGY eTopo)
{
	if (false == m_bFrameActive || false == m_Config.bCounters)
		return;

	AddCounter(EPROFILE_COUNTER::DRAWCALLS, 1);
	AddCounter(EPROFILE_COUNTER::TRIANGLES, Triangles_From(iIndexPerInst, eTopo) * iInstances);
}

void CProfiler_Manager::Record_DrawInstanced(_uint iVtxPerInst, _uint iInstances, D3D11_PRIMITIVE_TOPOLOGY eTopo)
{
	if (false == m_bFrameActive || false == m_Config.bCounters)
		return;

	AddCounter(EPROFILE_COUNTER::DRAWCALLS, 1);
	AddCounter(EPROFILE_COUNTER::TRIANGLES, Triangles_From(iVtxPerInst, eTopo) * iInstances);
}

void CProfiler_Manager::Set_CurrentPass(EPROFILE_PASS ePass)
{
	m_eCurrentPass = ePass;
}

void CProfiler_Manager::StartCsvCapture(const _tchar* pFilePath)
{
	if (true == m_bCsvCapturing)
		return;

	m_CsvFilePath = (nullptr != pFilePath) ? pFilePath : Make_CsvFilePath();

	std::error_code ec;
	std::filesystem::create_directories(std::filesystem::path(m_CsvFilePath).parent_path(), ec);

	m_CsvFile.open(m_CsvFilePath.c_str(), ios::out | ios::trunc);
	if (false == m_CsvFile.is_open())
	{
		OutputDebugStringW(L"[Profiler] CSV open failed\n");
		return;
	}

	QueryPerformanceCounter(&m_CsvStartCounter);
	m_iCsvRowCount = 0;
	m_bCsvCapturing = true;
	m_CapCpuFrame.clear();
	m_CapGpuFrame.clear();

	Write_CsvHeader();

	OutputDebugStringW((L"[Profiler] CSV START: " + std::filesystem::absolute(m_CsvFilePath).wstring() + L"\n").c_str());
}

void CProfiler_Manager::StopCsvCapture()
{
	if (false == m_bCsvCapturing)
		return;

	Write_Summary();

	m_bCsvCapturing = false;
	m_bCaptureTimed = false;

	if (true == m_CsvFile.is_open())
		m_CsvFile.close();

	m_CapCpuFrame.clear();
	m_CapGpuFrame.clear();

	OutputDebugStringW(L"[Profiler] CSV capture STOP\n");
}

wstring CProfiler_Manager::Make_CsvFilePath() const
{
	time_t now = time(nullptr);
	tm lt{};
	localtime_s(&lt, &now);

	_tchar szName[160] = {};
	swprintf_s(szName, L"../../DataFiles/Profile/Profile_%04d%02d%02d_%02d%02d%02d.csv",
		lt.tm_year + 1900, lt.tm_mon + 1, lt.tm_mday, lt.tm_hour, lt.tm_min, lt.tm_sec);

	return wstring(szName);
}

_double CProfiler_Manager::Get_CpuMs(const _tchar* pName) const
{
	if (nullptr == m_pCpuProfiler)
		return 0.0;

	const PROFILE_CPU_FRAME& Frame = m_pCpuProfiler->Get_FrameResult();
	for (const auto& Entry : Frame.Entries)
	{
		if (0 == wcscmp(Entry.pName, pName))
			return Entry.dSumMs;
	}

	return 0.0;
}

_double CProfiler_Manager::Get_GpuMs(const _tchar* pName) const
{
	if (nullptr == m_pGpuProfiler)
		return 0.0;

	const GPU_PROFILE_FRAME* pFrame = m_pGpuProfiler->Get_LatestResolved();
	if (nullptr == pFrame)
		return 0.0;

	for (_uint i = 0; i < pFrame->iUsedSections; ++i)
	{
		if (nullptr != pFrame->Sections[i].pName && 0 == wcscmp(pFrame->Sections[i].pName, pName))
			return pFrame->Sections[i].dElapsedMs;
	}

	return 0.0;
}

_uint CProfiler_Manager::Get_Counter(EPROFILE_PASS ePass, EPROFILE_COUNTER eId) const
{
	const _uint iPass = ETOUI(ePass);
	const _uint iId = ETOUI(eId);
	if (iPass < ETOUI(EPROFILE_PASS::END) && iId < ETOUI(EPROFILE_COUNTER::END))
		return m_Counters.Values[iPass][iId];
	return 0;
}

_uint CProfiler_Manager::Get_CounterTotal(EPROFILE_COUNTER eId) const
{
	const _uint iId = ETOUI(eId);
	if (iId >= ETOUI(EPROFILE_COUNTER::END))
		return 0;

	_uint iSum = 0;
	for (_uint iPass = 0; iPass < ETOUI(EPROFILE_PASS::END); ++iPass)
		iSum += m_Counters.Values[iPass][iId];
	return iSum;
}

void CProfiler_Manager::Write_CsvHeader()
{
	m_CsvFile << L"FrameIndex,TimeSec,LevelIndex,CPU_Frame_Total";
	for (const _tchar* pCol : s_CpuColumns)
		m_CsvFile << L',' << pCol;
	for (const _tchar* pCol : s_GpuColumns)
		m_CsvFile << L',' << pCol;
	m_CsvFile << L",DrawCalls_Total,DrawCalls_Shadow,DrawCalls_NonBlend,Triangles_Total,Triangles_Shadow,Triangles_NonBlend,Objects_Shadow,Objects_NonBlend,ShaderBeginCount,MaterialBindCount,TextureBindCount,TextDrawCalls";
	m_CsvFile << L'\n';
}

void CProfiler_Manager::Write_CsvRow()
{
	if (nullptr == m_pCpuProfiler)
		return;

	const PROFILE_CPU_FRAME& Frame = m_pCpuProfiler->Get_FrameResult();

	LARGE_INTEGER Now{};
	QueryPerformanceCounter(&Now);
	const _double dTimeSec = (0 < m_CsvFrequency.QuadPart)
		? static_cast<_double>(Now.QuadPart - m_CsvStartCounter.QuadPart) / static_cast<_double>(m_CsvFrequency.QuadPart)
		: 0.0;

	_int iLevelIndex = -1;
	if (CGameInstance* pGameInstance = CGameInstance::GetInstance())
		iLevelIndex = pGameInstance->Get_CurrentLevel();

	m_CsvFile << Frame.iFrameIndex << L',' << dTimeSec << L',' << iLevelIndex << L',' << Frame.dFrameTotalMs;
	for (const _tchar* pCol : s_CpuColumns)
		m_CsvFile << L',' << Get_CpuMs(pCol);
	for (const _tchar* pCol : s_GpuColumns)
		m_CsvFile << L',' << Get_GpuMs(pCol);
	m_CsvFile << L',' << Get_CounterTotal(EPROFILE_COUNTER::DRAWCALLS)
		<< L',' << Get_Counter(EPROFILE_PASS::SHADOW, EPROFILE_COUNTER::DRAWCALLS)
		<< L',' << Get_Counter(EPROFILE_PASS::NONBLEND, EPROFILE_COUNTER::DRAWCALLS)
		<< L',' << Get_CounterTotal(EPROFILE_COUNTER::TRIANGLES)
		<< L',' << Get_Counter(EPROFILE_PASS::SHADOW, EPROFILE_COUNTER::TRIANGLES)
		<< L',' << Get_Counter(EPROFILE_PASS::NONBLEND, EPROFILE_COUNTER::TRIANGLES)
		<< L',' << Get_Counter(EPROFILE_PASS::SHADOW, EPROFILE_COUNTER::OBJECTS)
		<< L',' << Get_Counter(EPROFILE_PASS::NONBLEND, EPROFILE_COUNTER::OBJECTS)
		<< L',' << Get_CounterTotal(EPROFILE_COUNTER::SHADER_BEGIN)
		<< L',' << Get_CounterTotal(EPROFILE_COUNTER::MATERIAL_BIND)
		<< L',' << Get_CounterTotal(EPROFILE_COUNTER::TEXTURE_BIND)
		<< L',' << Get_CounterTotal(EPROFILE_COUNTER::TEXT_DRAWCALLS);
	m_CsvFile << L'\n';

	++m_iCsvRowCount;
}

void CProfiler_Manager::UpdateStats()
{
	// 직전 완료 프레임 스냅샷(오버레이는 1프레임 지연 표시). GPU 는 최신 회수 프레임.
	m_Snapshot.cpuFrame    = (nullptr != m_pCpuProfiler) ? m_pCpuProfiler->Get_FrameResult().dFrameTotalMs : 0.0;
	m_Snapshot.cpuShadow   = Get_CpuMs(L"CPU_Render_Shadow");
	m_Snapshot.cpuNonBlend = Get_CpuMs(L"CPU_Render_NonBlend");
	m_Snapshot.gpuFrame    = Get_GpuMs(L"GPU_Frame_Total");
	m_Snapshot.gpuShadow   = Get_GpuMs(L"GPU_Render_Shadow");
	m_Snapshot.gpuNonBlend = Get_GpuMs(L"GPU_Render_NonBlend");
	m_Snapshot.drawCalls   = Get_CounterTotal(EPROFILE_COUNTER::DRAWCALLS);
	m_Snapshot.triangles   = Get_CounterTotal(EPROFILE_COUNTER::TRIANGLES);

	Push_History(m_CpuFrameHistory, m_Snapshot.cpuFrame);
	Push_History(m_GpuFrameHistory, m_Snapshot.gpuFrame);
}

void CProfiler_Manager::Push_History(vector<_double>& History, _double dValue)
{
	History.push_back(dValue);
	if (History.size() > 240)			// 최근 240프레임(약 4초@60fps) 윈도우
		History.erase(History.begin());
}

void CProfiler_Manager::Compute_Stat(const vector<_double>& History, _double& dAvg, _double& dP95) const
{
	dAvg = 0.0;
	dP95 = 0.0;
	if (true == History.empty())
		return;

	_double dSum = 0.0;
	for (_double dValue : History)
		dSum += dValue;
	dAvg = dSum / static_cast<_double>(History.size());

	vector<_double> Sorted = History;
	sort(Sorted.begin(), Sorted.end());
	size_t iIndex = static_cast<size_t>(Sorted.size() * 0.95);
	if (iIndex >= Sorted.size())
		iIndex = Sorted.size() - 1;
	dP95 = Sorted[iIndex];
}

void CProfiler_Manager::SetOverlayEnabled(_bool bEnable)
{
	m_Config.bOverlay = bEnable;
	OutputDebugStringW(bEnable ? L"[Profiler] Overlay ON\n" : L"[Profiler] Overlay OFF\n");
}

void CProfiler_Manager::Set_OverlayFont(WNameID strFontTag)
{
	m_OverlayFont = strFontTag;
}

void CProfiler_Manager::RenderOverlay()
{
	if (false == m_Config.bMasterEnable || false == m_Config.bOverlay)
		return;

	PROFILE_CPU_SCOPE(L"CPU_Render_ProfilerOverlay");	// 자체 비용 분리(P4-2)

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	if (nullptr == pGameInstance)
		return;

	_double dCpuAvg = 0.0, dCpuP95 = 0.0, dGpuAvg = 0.0, dGpuP95 = 0.0;
	Compute_Stat(m_CpuFrameHistory, dCpuAvg, dCpuP95);
	Compute_Stat(m_GpuFrameHistory, dGpuAvg, dGpuP95);

	const _vector vColor = XMVectorSet(1.f, 0.95f, 0.2f, 1.f);
	const _float2 vScale = _float2(0.42f, 0.42f);
	_float2 vPos = _float2(12.f, 12.f);
	_tchar szLine[256] = {};

	auto Draw = [&](const _tchar* pText)
	{
		pGameInstance->Draw_Text(m_OverlayFont, pText, vPos, vColor, 0.f, _float2(0.f, 0.f), vScale);
		vPos.y += 22.f;
	};

	swprintf_s(szLine, L"[Profiler] CPU %.2f ms (avg %.2f / p95 %.2f)", m_Snapshot.cpuFrame, dCpuAvg, dCpuP95);
	Draw(szLine);
	swprintf_s(szLine, L"GPU %.2f ms (avg %.2f / p95 %.2f)", m_Snapshot.gpuFrame, dGpuAvg, dGpuP95);
	Draw(szLine);
	swprintf_s(szLine, L"Shadow   CPU %.2f / GPU %.2f", m_Snapshot.cpuShadow, m_Snapshot.gpuShadow);
	Draw(szLine);
	swprintf_s(szLine, L"NonBlend CPU %.2f / GPU %.2f", m_Snapshot.cpuNonBlend, m_Snapshot.gpuNonBlend);
	Draw(szLine);
	swprintf_s(szLine, L"DrawCalls %u   Tris %u", m_Snapshot.drawCalls, m_Snapshot.triangles);
	Draw(szLine);
}

void CProfiler_Manager::StartCapture(_float fDurationSec, const _tchar* pScenarioName)
{
	if (true == m_bCsvCapturing)
		return;

	m_CaptureScenario = (nullptr != pScenarioName) ? pScenarioName : L"capture";
	m_dCaptureDurationSec = fDurationSec;
	m_bCaptureTimed = (0.f < fDurationSec);

	_int iLevel = -1;
	if (CGameInstance* pGameInstance = CGameInstance::GetInstance())
		iLevel = pGameInstance->Get_CurrentLevel();

	time_t now = time(nullptr);
	tm lt{};
	localtime_s(&lt, &now);

	_tchar szPath[256] = {};
	swprintf_s(szPath, L"../../DataFiles/Profile/Profile_%s_Lv%d_%04d%02d%02d_%02d%02d%02d.csv",
		m_CaptureScenario.c_str(), iLevel,
		lt.tm_year + 1900, lt.tm_mon + 1, lt.tm_mday, lt.tm_hour, lt.tm_min, lt.tm_sec);

	StartCsvCapture(szPath);
}

PROFILE_STATSUMMARY CProfiler_Manager::Compute_Summary(const vector<_double>& History) const
{
	PROFILE_STATSUMMARY Summary{};
	if (true == History.empty())
		return Summary;

	vector<_double> Sorted = History;
	sort(Sorted.begin(), Sorted.end());

	const size_t iCount = Sorted.size();
	Summary.iCount = static_cast<_uint>(iCount);
	Summary.dMin = Sorted.front();
	Summary.dMax = Sorted.back();
	Summary.dMedian = Sorted[iCount / 2];

	_double dSum = 0.0;
	for (_double dValue : Sorted)
		dSum += dValue;
	Summary.dAvg = dSum / static_cast<_double>(iCount);

	auto Percentile = [&](_double p) -> _double
	{
		size_t i = static_cast<size_t>(static_cast<_double>(iCount) * p);
		if (i >= iCount)
			i = iCount - 1;
		return Sorted[i];
	};
	Summary.dP95 = Percentile(0.95);
	Summary.dP99 = Percentile(0.99);

	return Summary;
}

void CProfiler_Manager::Write_Summary()
{
	if (true == m_CapCpuFrame.empty())
		return;

	const PROFILE_STATSUMMARY Cpu = Compute_Summary(m_CapCpuFrame);
	const PROFILE_STATSUMMARY Gpu = Compute_Summary(m_CapGpuFrame);

	_tchar szBuf[640] = {};
	swprintf_s(szBuf,
		L"[Profiler] Summary scenario=%s frames=%u\n"
		L"  CPU_Frame ms : avg %.3f  median %.3f  p95 %.3f  p99 %.3f  min %.3f  max %.3f\n"
		L"  GPU_Frame ms : avg %.3f  median %.3f  p95 %.3f  p99 %.3f  min %.3f  max %.3f\n",
		m_CaptureScenario.c_str(), Cpu.iCount,
		Cpu.dAvg, Cpu.dMedian, Cpu.dP95, Cpu.dP99, Cpu.dMin, Cpu.dMax,
		Gpu.dAvg, Gpu.dMedian, Gpu.dP95, Gpu.dP99, Gpu.dMin, Gpu.dMax);

	OutputDebugStringW(szBuf);

	wstring SummaryPath = m_CsvFilePath;
	size_t iPos = SummaryPath.rfind(L".csv");
	if (wstring::npos != iPos)
		SummaryPath.replace(iPos, 4, L"_summary.txt");
	else
		SummaryPath += L"_summary.txt";

	wofstream File(SummaryPath.c_str(), ios::out | ios::trunc);
	if (true == File.is_open())
	{
		File << szBuf;
		File.close();
	}
}

void CProfiler_Manager::SetEnabled(_bool bEnable)
{
	m_Config.bMasterEnable = bEnable;

	OutputDebugStringW(bEnable ? L"[Profiler] Master ON\n" : L"[Profiler] Master OFF\n");
}

void CProfiler_Manager::Handle_Input()
{
	static _bool s_bFirstCall = true;
	if (true == s_bFirstCall)
	{
		OutputDebugStringW(L"[Profiler] Handle_Input reached\n");
		s_bFirstCall = false;
	}

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	if (nullptr == pGameInstance)
		return;

	// 1 : 마스터 On/Off. 숫자키 1~4 는 KeyGroup::SYSTEM 이라 모든 입력 상태에서 통과.
	if (pGameInstance->Key_Down(DIK_1))
		SetEnabled(!m_Config.bMasterEnable);

	// 3 : CSV 캡처 Start/Stop (마스터가 켜져 있을 때만 시작).
	if (pGameInstance->Key_Down(DIK_3))
	{
		if (true == m_bCsvCapturing)
			StopCsvCapture();
		else if (true == m_Config.bMasterEnable)
			StartCsvCapture(nullptr);
	}

	// 2 : 오버레이 On/Off.
	if (pGameInstance->Key_Down(DIK_2))
		SetOverlayEnabled(!m_Config.bOverlay);

	// 4 : 60초 원샷 캡처(마스터 ON, 캡처 중 아닐 때만).
	if (pGameInstance->Key_Down(DIK_4))
	{
		if (false == m_bCsvCapturing && true == m_Config.bMasterEnable)
			StartCapture(60.f, L"oneshot60");
	}
}

void CProfiler_Manager::Free()
{
	if (true == m_CsvFile.is_open())
		m_CsvFile.close();

	Safe_Release(m_pCpuProfiler);
	Safe_Release(m_pGpuProfiler);

	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);

	if (this == s_pInstance)
		s_pInstance = nullptr;

	__super::Free();
}
