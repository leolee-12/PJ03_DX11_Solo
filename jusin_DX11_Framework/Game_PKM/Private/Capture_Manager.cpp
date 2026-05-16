#include "Capture_Manager.h"

namespace
{
	constexpr _float INTRO_DURATION = 2.0f;
	constexpr _float AIMING_DURATION = 1.5f;
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
		if (m_fPhaseElapsed >= INTRO_DURATION)
			Goto_Phase(CAPTURE_PHASE::AIMING);
		break;

	case CAPTURE_PHASE::AIMING:
		/* 본 골격 단위에서는 자동 진행. 후속 단위에서 키 입력으로 교체. */
		if (m_fPhaseElapsed >= AIMING_DURATION)
			Goto_Phase(CAPTURE_PHASE::THROWING);
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
			Goto_Phase(CAPTURE_PHASE::DONE);
		break;

	default:
		break;
	}
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