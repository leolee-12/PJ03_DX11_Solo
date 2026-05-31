#include "GpuProfiler.h"

CGpuProfiler* CGpuProfiler::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CGpuProfiler* pInstance = new CGpuProfiler();

	if (FAILED(pInstance->Initialize(pDevice, pContext)))
	{
		MSG_BOX("Failed to Created : CGpuProfiler");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

HRESULT CGpuProfiler::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	if (nullptr == pDevice || nullptr == pContext)
		return E_FAIL;

	m_pDevice = pDevice;
	m_pContext = pContext;
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);

	// PIX/RenderDoc 마커용. 미지원 환경이면 null → 마커는 생략된다.
	m_pContext->QueryInterface(IID_PPV_ARGS(m_pAnnotation.GetAddressOf()));

	m_OpenStack.reserve(MAX_SECTIONS);

	// Query 사전 생성 : 슬롯당 Disjoint 1 + Timestamp(Begin/End) MAX_SECTIONS 쌍.
	for (_uint i = 0; i < RING_COUNT; ++i)
	{
		GPU_PROFILE_FRAME& Frame = m_Frames[i];

		if (FAILED(Create_Query(D3D11_QUERY_TIMESTAMP_DISJOINT, Frame.pDisjointQuery.GetAddressOf())))
			return E_FAIL;

		Frame.Sections.resize(MAX_SECTIONS);
		for (_uint s = 0; s < MAX_SECTIONS; ++s)
		{
			if (FAILED(Create_Query(D3D11_QUERY_TIMESTAMP, Frame.Sections[s].pBeginQuery.GetAddressOf())))
				return E_FAIL;
			if (FAILED(Create_Query(D3D11_QUERY_TIMESTAMP, Frame.Sections[s].pEndQuery.GetAddressOf())))
				return E_FAIL;
		}
	}

	return S_OK;
}

HRESULT CGpuProfiler::Create_Query(D3D11_QUERY eType, ID3D11Query** ppOut)
{
	D3D11_QUERY_DESC Desc{};
	Desc.Query = eType;
	Desc.MiscFlags = 0;

	return m_pDevice->CreateQuery(&Desc, ppOut);
}

void CGpuProfiler::BeginFrame(uint64_t iFrameIndex)
{
	m_iCurrentSlot = static_cast<_uint>(iFrameIndex % RING_COUNT);

	GPU_PROFILE_FRAME& Frame = m_Frames[m_iCurrentSlot];

	// 슬롯 재사용 : 직전 데이터(드물게 미회수)는 폐기하고 초기화.
	Frame.iFrameIndex = iFrameIndex;
	Frame.iUsedSections = 0;
	Frame.bSubmitted = false;
	Frame.bResolved = false;
	Frame.bValid = false;

	m_OpenStack.clear();
	m_bFrameOpen = true;

	m_pContext->Begin(Frame.pDisjointQuery.Get());
}

void CGpuProfiler::EndFrame()
{
	if (false == m_bFrameOpen)
		return;

	GPU_PROFILE_FRAME& Frame = m_Frames[m_iCurrentSlot];

	m_pContext->End(Frame.pDisjointQuery.Get());
	Frame.bSubmitted = true;
	m_bFrameOpen = false;
}

void CGpuProfiler::BeginSection(const _tchar* pName)
{
	if (false == m_bFrameOpen)
		return;

	if (nullptr != m_pAnnotation)
		m_pAnnotation->BeginEvent(pName);

	GPU_PROFILE_FRAME& Frame = m_Frames[m_iCurrentSlot];

	// 슬롯 구간 초과 시 무시하되, 스택 균형을 위해 센티넬을 넣는다.
	if (Frame.iUsedSections >= MAX_SECTIONS)
	{
		m_OpenStack.push_back(INVALID_INDEX);
		return;
	}

	_uint iIndex = Frame.iUsedSections++;
	GPU_PROFILE_SECTION& Section = Frame.Sections[iIndex];
	Section.pName = pName;
	Section.iDepth = static_cast<_uint>(m_OpenStack.size());
	Section.dElapsedMs = 0.0;

	m_pContext->End(Section.pBeginQuery.Get());
	m_OpenStack.push_back(iIndex);
}

void CGpuProfiler::EndSection()
{
	if (true == m_OpenStack.empty())
		return;

	_uint iIndex = m_OpenStack.back();
	m_OpenStack.pop_back();

	if (nullptr != m_pAnnotation)
		m_pAnnotation->EndEvent();

	if (INVALID_INDEX == iIndex)
		return;

	GPU_PROFILE_FRAME& Frame = m_Frames[m_iCurrentSlot];
	m_pContext->End(Frame.Sections[iIndex].pEndQuery.Get());
}

void CGpuProfiler::ResolveOldFrames()
{
	for (_uint i = 0; i < RING_COUNT; ++i)
	{
		if (i == m_iCurrentSlot)
			continue;

		GPU_PROFILE_FRAME& Frame = m_Frames[i];
		if (false == Frame.bSubmitted || true == Frame.bResolved)
			continue;

		Resolve_Frame(Frame);
	}
}

void CGpuProfiler::Resolve_Frame(GPU_PROFILE_FRAME& Frame)
{
	D3D11_QUERY_DATA_TIMESTAMP_DISJOINT DisjointData{};

	// 비차단 : 아직 준비 안 됐으면 S_FALSE → 다음 기회로 미룸.
	if (S_OK != m_pContext->GetData(Frame.pDisjointQuery.Get(), &DisjointData, sizeof(DisjointData), D3D11_ASYNC_GETDATA_DONOTFLUSH))
		return;

	Frame.bResolved = true;

	// Disjoint 프레임 폐기.
	if (TRUE == DisjointData.Disjoint || 0 == DisjointData.Frequency)
		return;

	for (_uint s = 0; s < Frame.iUsedSections; ++s)
	{
		GPU_PROFILE_SECTION& Section = Frame.Sections[s];

		UINT64 iBegin = 0, iEnd = 0;
		if (S_OK != m_pContext->GetData(Section.pBeginQuery.Get(), &iBegin, sizeof(iBegin), D3D11_ASYNC_GETDATA_DONOTFLUSH))
			continue;
		if (S_OK != m_pContext->GetData(Section.pEndQuery.Get(), &iEnd, sizeof(iEnd), D3D11_ASYNC_GETDATA_DONOTFLUSH))
			continue;

		Section.dElapsedMs = (iEnd > iBegin)
			? static_cast<_double>(iEnd - iBegin) / static_cast<_double>(DisjointData.Frequency) * 1000.0
			: 0.0;
	}

	Frame.bValid = true;
}

const GPU_PROFILE_FRAME* CGpuProfiler::Get_LatestResolved() const
{
	const GPU_PROFILE_FRAME* pBest = nullptr;

	for (_uint i = 0; i < RING_COUNT; ++i)
	{
		const GPU_PROFILE_FRAME& Frame = m_Frames[i];
		if (false == Frame.bValid)
			continue;

		if (nullptr == pBest || Frame.iFrameIndex > pBest->iFrameIndex)
			pBest = &Frame;
	}

	return pBest;
}

void CGpuProfiler::Free()
{
	for (_uint i = 0; i < RING_COUNT; ++i)
	{
		m_Frames[i].Sections.clear();		// 구간 Query ComPtr 해제
		m_Frames[i].pDisjointQuery.Reset();
	}
	m_OpenStack.clear();
	m_pAnnotation.Reset();

	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);

	__super::Free();
}
