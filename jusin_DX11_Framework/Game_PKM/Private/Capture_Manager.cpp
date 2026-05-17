#include "Capture_Manager.h"

namespace
{
	constexpr _float INTRO_DURATION = 2.0f;
	constexpr _float THROWING_DURATION = 1.5f;
	constexpr _float RESULT_DURATION = 2.0f;
}

CCapture_Manager::CCapture_Manager()
{
}

HRESULT CCapture_Manager::Initialize(const CAPTURE_ENV& tEnv)
{
	m_tEnv = tEnv;
	m_ePhase = CAPTURE_PHASE::INTRO;
	m_eResult = CAPTURE_RESULT::NONE;
	m_fPhaseElapsed = 0.f;

	return S_OK;
}

void CCapture_Manager::Begin()
{
	/* Level_Capture::Initialize 마지막에 호출. 모든 Layer 가 준비된 상태에서 페이즈 시작. */
	m_ePhase = CAPTURE_PHASE::INTRO;
	m_fPhaseElapsed = 0.f;

	OutputDebugStringW(L"[Capture_Manager] Begin → INTRO\n");
}

void CCapture_Manager::Update(_float fTimeDelta)
{
	if (CAPTURE_PHASE::DONE == m_ePhase)
		return;

	m_fPhaseElapsed += fTimeDelta;

	switch (m_ePhase)
	{
	case CAPTURE_PHASE::INTRO:
		/* 메뉴 주도 — INTRO 에서는 시간 경과 자동 전이 안 함.
		   메뉴 "준비한다" Activate 가 Enter_Aiming() 을 호출해야 AIMING 으로 전이. */
		break;

	case CAPTURE_PHASE::AIMING:
		/* 진행 트리거는 Level_Capture::Update → Try_Throw() 경로.
		   여기서는 시간 진행 없음. */
		break;

	case CAPTURE_PHASE::THROWING:
		if (m_fPhaseElapsed >= THROWING_DURATION)
		{
			/* 본 골격 단위에서는 항상 SUCCESS. 후속 단위에서 확률 산식으로 교체. */
			m_eResult = CAPTURE_RESULT::SUCCESS;
			Goto_Phase(CAPTURE_PHASE::RESULT);
		}
		break;

	case CAPTURE_PHASE::RESULT:
		if (m_fPhaseElapsed >= RESULT_DURATION)
		{
			/* 성공이면 세션 종료, 실패면 AIMING 으로 복귀해 재던지기.
			   m_eResult 결정 시점은 THROWING 종료. 현재는 임시 SUCCESS (C-3 산식 도입 시 교체). */
			switch (m_eResult)
			{
			case CAPTURE_RESULT::FAIL_BREAK:
			case CAPTURE_RESULT::FAIL_RUN:
				m_eResult = CAPTURE_RESULT::NONE;
				Goto_Phase(CAPTURE_PHASE::AIMING);
				break;

			case CAPTURE_RESULT::SUCCESS:
			case CAPTURE_RESULT::NONE:
			default:
				Goto_Phase(CAPTURE_PHASE::DONE);
				break;
			}
		}
		break;

	default:
		break;
	}
}

void CCapture_Manager::Enter_Aiming()
{
	/* INTRO 에서만 유효. 메뉴 "준비한다" Activate 의 라우팅 대상.
	   다른 페이즈에서 호출되면 무시. */
	if (CAPTURE_PHASE::INTRO != m_ePhase)
		return;

	Goto_Phase(CAPTURE_PHASE::AIMING);
}

void CCapture_Manager::Try_Throw()
{
	/* AIMING 에서만 유효. 다른 페이즈에서 들어온 입력은 무시. */
	if (CAPTURE_PHASE::AIMING != m_ePhase)
		return;

	Goto_Phase(CAPTURE_PHASE::THROWING);
}

void CCapture_Manager::Request_Run()
{
	/* DONE 외 어디서든 호출 가능 (메뉴 "도망간다" + ESC 통합 라우팅).
	   이미 DONE 이면 중복 호출 방지로 무시.
	   결과를 FAIL_RUN 으로 설정한 뒤 DONE 으로 직접 전이 →
	   Level_Capture::Update 의 Is_Done() 분기가 Pop_Level 실행. */
	if (CAPTURE_PHASE::DONE == m_ePhase)
		return;

	m_eResult = CAPTURE_RESULT::FAIL_RUN;
	Goto_Phase(CAPTURE_PHASE::DONE);
}

void CCapture_Manager::Goto_Phase(CAPTURE_PHASE ePhase)
{
	m_ePhase = ePhase;
	m_fPhaseElapsed = 0.f;

	wchar_t szLog[128] = {};
	swprintf_s(szLog, L"[Capture_Manager] Phase → %u (result=%u)\n",
		static_cast<_uint>(ePhase), static_cast<_uint>(m_eResult));
	OutputDebugStringW(szLog);
}

CCapture_Manager* CCapture_Manager::Create(const CAPTURE_ENV& tEnv)
{
	CCapture_Manager* pInstance = new CCapture_Manager();

	if (FAILED(pInstance->Initialize(tEnv)))
	{
		MSG_BOX("Failed to Created : CCapture_Manager");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCapture_Manager::Free()
{
	__super::Free();
}