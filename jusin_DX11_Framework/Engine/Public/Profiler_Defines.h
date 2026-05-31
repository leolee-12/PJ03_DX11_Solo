#pragma once

#include <cstdint>

// 외부에서 PROFILE_ENABLE 을 미리 정의하면 그 값을 우선한다(Build_Mode.h 패턴).
// 기본값 1 : Release 포함 동작. 0 으로 정의하면 모든 매크로가 nop.
#ifndef PROFILE_ENABLE
#define PROFILE_ENABLE 1
#endif

namespace Engine
{
	// 카운터 적립 시 패스별 버킷팅에 사용하는 패스 식별자.
	enum class EPROFILE_PASS : unsigned int
	{
		NONE = 0,
		PRIORITY,
		SHADOW,
		NONBLEND,
		OUTLINEMASK,
		LIGHTS,
		COMBINED,
		POSTPROCESS,
		NONLIGHT,
		BLEND,
		UI,
		DEBUGRENDER,
		LEVEL,
		END
	};

	// 누적 카운터 종류. 패스 차원은 EPROFILE_PASS 로 별도 관리.
	enum class EPROFILE_COUNTER : unsigned int
	{
		DRAWCALLS = 0,
		TRIANGLES,
		OBJECTS,
		TEXT_DRAWCALLS,
		SHADER_BEGIN,
		MATERIAL_BIND,
		TEXTURE_BIND,
		CONSTANTBUFFER_UPDATE,
		END
	};
}

#if PROFILE_ENABLE

#define PROFILE_CONCAT_(a, b) a##b
#define PROFILE_CONCAT(a, b)  PROFILE_CONCAT_(a, b)

#define PROFILE_FRAME_BEGIN()	do { if (auto* _pp = ::Engine::CProfiler_Manager::Get()) _pp->BeginFrame(); } while (0)
#define PROFILE_FRAME_END()		do { if (auto* _pp = ::Engine::CProfiler_Manager::Get()) _pp->EndFrame();   } while (0)

// RAII 스코프(권장). 패스처럼 중간 return 이 있는 구간은 반드시 이 매크로를 쓴다.
#define PROFILE_CPU_SCOPE(NAME)	::Engine::CScopedCpuProfile PROFILE_CONCAT(_cpuScope_, __LINE__)(NAME)
#define PROFILE_GPU_SCOPE(NAME)	::Engine::CScopedGpuProfile PROFILE_CONCAT(_gpuScope_, __LINE__)(NAME)

// 수동 GPU 구간 : 중간 return 이 없는 직선 구간 전용(패스에는 쓰지 말 것).
#define PROFILE_GPU_BEGIN(NAME)	do { if (auto* _pp = ::Engine::CProfiler_Manager::Get()) _pp->BeginGpuSection(NAME); } while (0)
#define PROFILE_GPU_END()		do { if (auto* _pp = ::Engine::CProfiler_Manager::Get()) _pp->EndGpuSection();      } while (0)

#define PROFILE_SET_PASS(PASS)	do { if (auto* _pp = ::Engine::CProfiler_Manager::Get()) _pp->Set_CurrentPass(PASS); } while (0)

#define PROFILE_COUNTER_ADD(ID, V)						do { if (auto* _pp = ::Engine::CProfiler_Manager::Get()) _pp->AddCounter(ID, V); } while (0)

#define PROFILE_DRAW_INDEXED(IC, TOPO)					do { if (auto* _pp = ::Engine::CProfiler_Manager::Get()) _pp->Record_DrawIndexed(IC, TOPO); } while (0)
#define PROFILE_DRAW_INDEXED_INSTANCED(IC, NI, TOPO)	do { if (auto* _pp = ::Engine::CProfiler_Manager::Get()) _pp->Record_DrawIndexedInstanced(IC, NI, TOPO); } while (0)
#define PROFILE_DRAW_INSTANCED(VC, NI, TOPO)			do { if (auto* _pp = ::Engine::CProfiler_Manager::Get()) _pp->Record_DrawInstanced(VC, NI, TOPO); } while (0)

#else

#define PROFILE_FRAME_BEGIN()
#define PROFILE_FRAME_END()
#define PROFILE_CPU_SCOPE(NAME)
#define PROFILE_GPU_SCOPE(NAME)
#define PROFILE_GPU_BEGIN(NAME)
#define PROFILE_GPU_END()
#define PROFILE_SET_PASS(PASS)
#define PROFILE_COUNTER_ADD(ID, V)
#define PROFILE_DRAW_INDEXED(IC, TOPO)
#define PROFILE_DRAW_INDEXED_INSTANCED(IC, NI, TOPO)
#define PROFILE_DRAW_INSTANCED(VC, NI, TOPO)

#endif
