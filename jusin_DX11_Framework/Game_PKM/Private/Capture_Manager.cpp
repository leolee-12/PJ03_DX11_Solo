#include "Capture_Manager.h"
#include "Actor_CaptureTarget.h"
#include "MonsterBall.h"

#include "GameInstance.h"

namespace
{
	constexpr _float RESULT_DURATION = 2.0f;
	constexpr _float CAPTURE_BASE_PROBABILITY = 0.6f;
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

	OutputDebugStringW(L"[Capture_Manager] Begin -> INTRO\n");
}

void CCapture_Manager::Update(_float fTimeDelta)
{
	if (CAPTURE_PHASE::DONE == m_ePhase)
		return;

	m_fPhaseElapsed += fTimeDelta;

	switch (m_ePhase)
	{
	case CAPTURE_PHASE::INTRO:
		/* 메뉴 주도 - INTRO 에서는 시간 경과 자동 전이 안 함.
		   메뉴 "준비한다" Activate 가 Enter_Aiming() 을 호출해야 AIMING 으로 전이. */
		break;

	case CAPTURE_PHASE::AIMING:
		/* 진행 트리거는 Level_Capture::Update -> Try_Throw() 경로.
		   여기서는 시간 진행 없음. */
		break;

	case CAPTURE_PHASE::THROWING:
		Tick_Throwing();
		break;

	case CAPTURE_PHASE::RESULT:
		if (m_fPhaseElapsed >= RESULT_DURATION)
		{
			/* 성공이면 세션 종료, 실패면 AIMING 으로 복귀해 재던지기.
			   m_eResult 는 THROWING 중 ball이 DONE 에 도달했을 때 Resolve_Throw() 에서 확정된다. */
			switch (m_eResult)
			{
			case CAPTURE_RESULT::FAIL_BREAK:
				if (nullptr != m_pBall)
				{
					m_pBall->Reset();
					m_pBall->Hide();
				}

				m_eResult = CAPTURE_RESULT::NONE;
				Goto_Phase(CAPTURE_PHASE::AIMING);
				break;

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

	m_bHitThisThrow = false;
	m_eResult = CAPTURE_RESULT::NONE;

	Goto_Phase(CAPTURE_PHASE::THROWING);
}

void CCapture_Manager::Request_Run()
{
	/* DONE 외 어디서든 호출 가능 (메뉴 "도망간다" + ESC 통합 라우팅).
	   이미 DONE 이면 중복 호출 방지로 무시.
	   결과를 FAIL_RUN 으로 설정한 뒤 DONE 으로 직접 전이 ->
	   Level_Capture::Update 의 Is_Done() 분기가 Pop_Level 실행. */
	if (CAPTURE_PHASE::DONE == m_ePhase)
		return;

	m_eResult = CAPTURE_RESULT::FAIL_RUN;
	Goto_Phase(CAPTURE_PHASE::DONE);
}

void CCapture_Manager::Set_Combatants(CActor_CaptureTarget* pTarget, CMonsterBall* pBall)
{
	m_pTarget = pTarget;
	m_pBall = pBall;
}

void CCapture_Manager::Goto_Phase(CAPTURE_PHASE ePhase)
{
	m_ePhase = ePhase;
	m_fPhaseElapsed = 0.f;

	wchar_t szLog[128] = {};
	swprintf_s(szLog, L"[Capture_Manager] Phase -> %u (result=%u)\n",
		static_cast<_uint>(ePhase), static_cast<_uint>(m_eResult));
	OutputDebugStringW(szLog);
}

void CCapture_Manager::Tick_Throwing()
{
	if (nullptr == m_pBall)
	{
		/* 충돌은 throw 당 1회만 기록한다.
			- 결과 확정은 충돌 순간이 아니라 ball 의 IMPACT/OPEN 흐름이 끝나 DONE 이 된 뒤 수행한다. */

		OutputDebugStringW(L"[Capture_Manager] Throwing failed: ball is null\n");
		m_eResult = CAPTURE_RESULT::FAIL_BREAK;
		Goto_Phase(CAPTURE_PHASE::RESULT);
		return;
	}

	if (!m_bHitThisThrow
		&& nullptr != m_pTarget
		&& CMonsterBall::BALL_STATE::FLYING == m_pBall->Get_State())
	{
		CCollider* pBallCol = m_pBall->Get_Collider();
		CCollider* pTargetCol = m_pTarget->Get_Collider();

		if (nullptr != pBallCol && nullptr != pTargetCol
			&& pBallCol->Intersect(pTargetCol))
		{
			m_bHitThisThrow = true;
			m_pBall->Trigger_Impact(m_pTarget->Get_CaptureCenter());
			OutputDebugStringW(L"[Capture_Manager] Hit detected\n");
		}
	}

	if (CMonsterBall::BALL_STATE::DONE == m_pBall->Get_State())
		Resolve_Throw();
}

_float CCapture_Manager::Calc_Capture_Probability() const
{
	return CAPTURE_BASE_PROBABILITY;
}

void CCapture_Manager::Resolve_Throw()
{
	if (!m_bHitThisThrow)
	{
		m_eResult = CAPTURE_RESULT::FAIL_BREAK;
		Goto_Phase(CAPTURE_PHASE::RESULT);
		return;
	}

	const _float fRoll = CGameInstance::GetInstance()->Random(0.f, 1.f);
	const _float fProb = Calc_Capture_Probability();

	m_eResult = (fRoll < fProb) ? CAPTURE_RESULT::SUCCESS : CAPTURE_RESULT::FAIL_BREAK;

	wchar_t szLog[128] = {};
	swprintf_s(szLog, L"[Capture_Manager] Resolve hit=%u roll=%.3f prob=%.3f result=%u\n",
		static_cast<_uint>(m_bHitThisThrow), fRoll, fProb,
		static_cast<_uint>(m_eResult));
	OutputDebugStringW(szLog);

	Goto_Phase(CAPTURE_PHASE::RESULT);
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
	m_pTarget = nullptr;
	m_pBall = nullptr;

	__super::Free();
}