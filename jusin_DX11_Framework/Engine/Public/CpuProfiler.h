#pragma once
#include "Base.h"
#include "Profiler_Defines.h"

/* ------------------------------------------------------------ */
// CCpuProfiler : QPC 기반 CPU 구간 측정.
// 프레임 내 동일 이름 구간을 "이름 단위로 누적"(sumMs/callCount)하여
// 고빈도 함수 계측 시 vector 폭증을 막는다. inclusive time(자식 포함).
// 1차는 메인 스레드 전용.
/* ------------------------------------------------------------ */

NS_BEGIN(Engine)

struct PROFILE_CPU_ENTRY
{
	const _tchar*	pName = { nullptr };
	_double			dSumMs = { 0.0 };
	_uint			iCallCount = { 0 };
	_uint			iDepth = { 0 };
};

struct PROFILE_CPU_FRAME
{
	uint64_t					iFrameIndex = { 0 };
	_double						dFrameTotalMs = { 0.0 };
	vector<PROFILE_CPU_ENTRY>	Entries;
};

class ENGINE_DLL CCpuProfiler final : public CBase
{
private:
	CCpuProfiler() = default;
	virtual ~CCpuProfiler() = default;

public:
	static CCpuProfiler* Create();

public:
	void BeginFrame(uint64_t iFrameIndex);
	void EndFrame();
	void BeginSection(const _tchar* pName);		// QPC 시작, depth++
	void EndSection();							// QPC 종료, 이름별 sumMs 누적
	void Reset();

	const PROFILE_CPU_FRAME& Get_FrameResult() const { return m_Result; }

private:
	struct STACK_NODE
	{
		const _tchar*	pName = { nullptr };
		LARGE_INTEGER	StartCounter{};
		_uint			iDepth = { 0 };
	};

	LARGE_INTEGER				m_Frequency{};
	LARGE_INTEGER				m_FrameStartCounter{};

	vector<STACK_NODE>			m_Stack;		// 진행 중(열린) 구간 스택
	PROFILE_CPU_FRAME			m_Result;		// 현재 프레임 누적 결과

private:
	PROFILE_CPU_ENTRY* Find_OrAdd_Entry(const _tchar* pName);

protected:
	virtual void Free() override;
};

NS_END
