#include "Capture_Manager.h"
#include "Actor_CaptureTarget.h"
#include "MonsterBall.h"

#include "GameInstance.h"

namespace
{
	constexpr _float CAPTURE_BASE_PROBABILITY = 0.6f;

	constexpr _float MISS_VIEW_DURATION = 0.5f;
	constexpr _float STAGE_DURATION = 0.6f;
	constexpr _float DROP_DURATION = 0.45f;
	constexpr _float SHAKE_DURATION = 0.8f;
	constexpr _float SHAKE_GAP_DURATION = 0.25f;
	constexpr _float BREAK_VIEW_DURATION = 0.8f;
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
	m_iShakeIdx = 0;
	m_fPerShakeProb = 0.f;

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
		break;

	case CAPTURE_PHASE::AIMING:
		break;

	case CAPTURE_PHASE::THROWING:
		Tick_Throwing();
		break;

	case CAPTURE_PHASE::MISS_VIEW:
		Tick_MissView();
		break;

	case CAPTURE_PHASE::STAGE:
		Tick_Stage();
		break;

	case CAPTURE_PHASE::DROP:
		Tick_Drop();
		break;

	case CAPTURE_PHASE::SHAKE:
		Tick_Shake();
		break;

	case CAPTURE_PHASE::SUCCESS_VIEW:
		Tick_SuccessView();
		break;

	case CAPTURE_PHASE::BREAK_VIEW:
		Tick_BreakView();
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

void CCapture_Manager::Confirm_SuccessView()
{
	if (CAPTURE_PHASE::SUCCESS_VIEW != m_ePhase)
		return;

	if (CAPTURE_RESULT::SUCCESS != m_eResult)
		return;

	Goto_Phase(CAPTURE_PHASE::DONE);
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
		OutputDebugStringW(L"[Capture_Manager] Throwing failed: ball is null\n");
		m_bHitThisThrow = false;
		Goto_Phase(CAPTURE_PHASE::MISS_VIEW);
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

			/* 적중 즉시 몬스터 축소 퇴장 + 이펙트 시작 */
			m_pTarget->Begin_Absorb();

			OutputDebugStringW(L"[Capture_Manager] Hit detected\n");
		}
	}

	if (CMonsterBall::BALL_STATE::DONE == m_pBall->Get_State())
		Resolve_Throw();
}

void CCapture_Manager::Tick_MissView()
{
	if (m_fPhaseElapsed < MISS_VIEW_DURATION)
		return;

	m_eResult = CAPTURE_RESULT::NONE;
	Goto_Phase(CAPTURE_PHASE::INTRO);
}

void CCapture_Manager::Tick_Stage()
{
	if (m_fPhaseElapsed < STAGE_DURATION)
		return;

	Goto_Phase(CAPTURE_PHASE::DROP);
}

void CCapture_Manager::Tick_Drop()
{
	if (m_fPhaseElapsed < DROP_DURATION)
		return;

	Goto_Phase(CAPTURE_PHASE::SHAKE);
}

void CCapture_Manager::Tick_Shake()
{
	const _float fOneCycle = SHAKE_DURATION + SHAKE_GAP_DURATION;
	if (m_fPhaseElapsed < fOneCycle)
		return;

	const _float fRoll = CGameInstance::GetInstance()->Random(0.f, 1.f);
	const _bool bPass = (fRoll < m_fPerShakeProb);

	wchar_t szLog[160] = {};
	swprintf_s(szLog, L"[Capture_Manager] Shake %d/%d roll=%.3f prob=%.3f pass=%u\n",
		m_iShakeIdx + 1, m_iShakeMax, fRoll, m_fPerShakeProb, static_cast<_uint>(bPass));
	OutputDebugStringW(szLog);

	if (!bPass)
	{
		m_eResult = CAPTURE_RESULT::FAIL_BREAK;
		Goto_Phase(CAPTURE_PHASE::BREAK_VIEW);
		return;
	}

	++m_iShakeIdx;
	if (m_iShakeIdx >= m_iShakeMax)
	{
		m_eResult = CAPTURE_RESULT::SUCCESS;
		Goto_Phase(CAPTURE_PHASE::SUCCESS_VIEW);
		return;
	}

	/* 동일 SHAKE 페이즈에 머무르며 다음 회만 시작 - 타이머 리셋. */
	m_fPhaseElapsed = 0.f;
}

void CCapture_Manager::Tick_SuccessView()
{
	// SUCCESS_VIEW는 Level_Capture의 메시지 입력 확정으로만 DONE 전이한다.
}

void CCapture_Manager::Tick_BreakView()
{
	if (m_fPhaseElapsed < BREAK_VIEW_DURATION)
		return;

	if (nullptr != m_pBall)
	{
		m_pBall->Reset();
		m_pBall->Hide();
	}

	/* 적중 후 실패였을 때만 몬스터 복원. 미스(MISS_VIEW)는 이 경로를 안 거침. */
	if (m_bHitThisThrow && nullptr != m_pTarget)
		m_pTarget->Begin_Appear();

	m_bHitThisThrow = false;
	m_eResult = CAPTURE_RESULT::NONE;
	Goto_Phase(CAPTURE_PHASE::INTRO);
}

_float CCapture_Manager::Calc_Capture_Probability() const
{
	return CAPTURE_BASE_PROBABILITY;
}

void CCapture_Manager::Resolve_Throw()
{
	OutputDebugStringA(m_bHitThisThrow ? "[Resolve] enter hit=1\n" : "[Resolve] enter hit=0\n");

	if (!m_bHitThisThrow)
	{
		Goto_Phase(CAPTURE_PHASE::MISS_VIEW);
		return;
	}

	/* 회당 통과 확률 = P^(1/N). 실제 굴림은 SHAKE 회 종료 시점에 수행. */
	const _float fProb = Calc_Capture_Probability();
	const _float fInvN = (m_iShakeMax > 0) ? (1.f / static_cast<_float>(m_iShakeMax)) : 1.f;
	m_fPerShakeProb = powf(max(fProb, 0.f), fInvN);
	m_iShakeIdx = 0;

	wchar_t szLog[160] = {};
	swprintf_s(szLog, L"[Capture_Manager] Resolve hit=1 P=%.3f P_one=%.3f shakes=%d\n",
		fProb, m_fPerShakeProb, m_iShakeMax);
	OutputDebugStringW(szLog);

	Goto_Phase(CAPTURE_PHASE::STAGE);
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