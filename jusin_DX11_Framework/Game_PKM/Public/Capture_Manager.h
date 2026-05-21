#pragma once
#include "Base.h"
#include "Capture_Session.h"

/* -------------------------------------------------- */
// CCapture_Manager : 포획(세션) 진행을 책임지는 매니저
//  1) 한 판의 도메인 상태 보유 (CAPTURE_ENV / CAPTURE_PHASE / CAPTURE_RESULT)
//  2) 시간 기반 단순 페이즈 머신 진행 (단위 C-1 골격)
//  3) 본격 도메인 로직 - 확률 산식·입력·EventDispatcher - 은 후속 단위에서 도입
/* -------------------------------------------------- */

NS_BEGIN(Game_PKM)
class CActor_CaptureTarget;
class CMonsterBall;

class CCapture_Manager final : public CBase
{
private:
	CCapture_Manager();
	virtual ~CCapture_Manager() = default;

public:
	_bool				Is_Done() const { return CAPTURE_PHASE::DONE == m_ePhase; }
	const CAPTURE_ENV&	Get_Env() const { return m_tEnv; }
	CAPTURE_PHASE		Get_Phase() const { return m_ePhase; }
	CAPTURE_RESULT		Get_Result() const { return m_eResult; }
	_float				Get_PhaseElapsed() const { return m_fPhaseElapsed; }
	_int				Get_ShakeIndex() const { return m_iShakeIdx; }

	HRESULT Initialize(const CAPTURE_ENV& tEnv);

	void    Begin();
	void    Update(_float fTimeDelta);
	void    Enter_Aiming();   // INTRO 일 때 메뉴 "준비한다" 로 호출 -> AIMING 전이. 다른 페이즈에 호출되면 무시.
	void    Try_Throw();      // AIMING 일 때 마우스 좌클릭으로 호출 -> THROWING 전이.
	void    Request_Run();    // 메뉴 "도망간다" / ESC 로 호출 -> m_eResult=FAIL_RUN, DONE 전이. 이 DONE 이면 무시.
	void    Set_Combatants(CActor_CaptureTarget* pTarget, CMonsterBall* pBall);
	void	Confirm_SuccessView();

private:
	CAPTURE_ENV    m_tEnv = {};
	CAPTURE_PHASE  m_ePhase = { CAPTURE_PHASE::INTRO };
	CAPTURE_RESULT m_eResult = { CAPTURE_RESULT::NONE };
	_float         m_fPhaseElapsed = { 0.f };

	CActor_CaptureTarget* m_pTarget = { nullptr };   // weak
	CMonsterBall* m_pBall = { nullptr };     // weak
	_bool m_bHitThisThrow = { false };

	/* SHAKE 회당 통과 확률 = P^(1/m_iShakeMax). 회 끝마다 굴려 도중 실패 시 BREAK_VIEW. */
	_int    m_iShakeIdx = { 0 };
	_int    m_iShakeMax = { 3 };
	_float  m_fPerShakeProb = { 0.f };

private:
	void    Goto_Phase(CAPTURE_PHASE ePhase);
	void    Tick_Throwing();
	void    Tick_MissView();
	void    Tick_Stage();
	void    Tick_Drop();
	void    Tick_Shake();
	void    Tick_SuccessView();
	void    Tick_BreakView();

	_float  Calc_Capture_Probability() const;
	void    Resolve_Throw();

public:
	static CCapture_Manager* Create(const CAPTURE_ENV& tEnv);

protected:
	virtual void Free() override;
};

NS_END