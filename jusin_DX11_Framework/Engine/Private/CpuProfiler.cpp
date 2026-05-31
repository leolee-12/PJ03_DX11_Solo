#include "CpuProfiler.h"

CCpuProfiler* CCpuProfiler::Create()
{
	CCpuProfiler* pInstance = new CCpuProfiler();

	QueryPerformanceFrequency(&pInstance->m_Frequency);
	pInstance->m_Stack.reserve(32);
	pInstance->m_Result.Entries.reserve(48);

	return pInstance;
}

void CCpuProfiler::BeginFrame(uint64_t iFrameIndex)
{
	m_Result.iFrameIndex = iFrameIndex;
	m_Result.dFrameTotalMs = 0.0;
	m_Result.Entries.clear();
	m_Stack.clear();

	QueryPerformanceCounter(&m_FrameStartCounter);
}

void CCpuProfiler::EndFrame()
{
	LARGE_INTEGER End{};
	QueryPerformanceCounter(&End);

	if (0 < m_Frequency.QuadPart)
		m_Result.dFrameTotalMs =
			static_cast<_double>(End.QuadPart - m_FrameStartCounter.QuadPart) * 1000.0 / static_cast<_double>(m_Frequency.QuadPart);

	// RAII 가 정상이면 비어 있다. 비정상(언밸런스) 잔여는 폐기.
	m_Stack.clear();
}

void CCpuProfiler::BeginSection(const _tchar* pName)
{
	STACK_NODE Node{};
	Node.pName = pName;
	Node.iDepth = static_cast<_uint>(m_Stack.size());
	QueryPerformanceCounter(&Node.StartCounter);

	m_Stack.push_back(Node);
}

void CCpuProfiler::EndSection()
{
	if (true == m_Stack.empty())
		return;

	LARGE_INTEGER End{};
	QueryPerformanceCounter(&End);

	const STACK_NODE Node = m_Stack.back();
	m_Stack.pop_back();

	const _double dMs = (0 < m_Frequency.QuadPart)
		? static_cast<_double>(End.QuadPart - Node.StartCounter.QuadPart) * 1000.0 / static_cast<_double>(m_Frequency.QuadPart)
		: 0.0;

	PROFILE_CPU_ENTRY* pEntry = Find_OrAdd_Entry(Node.pName);
	pEntry->dSumMs += dMs;
	++pEntry->iCallCount;
	pEntry->iDepth = Node.iDepth;
}

void CCpuProfiler::Reset()
{
	m_Stack.clear();
	m_Result.Entries.clear();
	m_Result.dFrameTotalMs = 0.0;
}

PROFILE_CPU_ENTRY* CCpuProfiler::Find_OrAdd_Entry(const _tchar* pName)
{
	// 같은 호출 지점의 리터럴은 포인터가 동일하므로 포인터 비교로 누적한다.
	for (auto& Entry : m_Result.Entries)
	{
		if (Entry.pName == pName)
			return &Entry;
	}

	PROFILE_CPU_ENTRY New{};
	New.pName = pName;
	m_Result.Entries.push_back(New);

	return &m_Result.Entries.back();
}

void CCpuProfiler::Free()
{
	m_Stack.clear();
	m_Result.Entries.clear();

	__super::Free();
}
