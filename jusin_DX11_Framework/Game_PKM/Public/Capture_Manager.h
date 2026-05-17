#pragma once
#include "Base.h"
#include "Capture_Session.h"

/* -------------------------------------------------- */
// CCapture_Manager : 포획(세션) 진행을 책임지는 매니저
//  1) 한 판의 도메인 상태 보유 (CAPTURE_ENV / CAPTURE_PHASE / CAPTURE_RESULT)
//  2) 시간 기반 단순 페이즈 머신 진행 (단위 C-1 골격)
//  3) 본격 도메인 로직 — 확률 산식·입력·EventDispatcher — 은 후속 단위에서 도입
/* -------------------------------------------------- */

NS_BEGIN(Game_PKM)

class CCapture_Manager final : public CBase
{
private:
	CCapture_Manager();
	virtual ~CCapture_Manager() = default;

public:
	HRESULT Initialize(const CAPTURE_ENV& tEnv);

	void    Begin();
	void    Update(_float fTimeDelta);
	void    Enter_Aiming();   // INTRO 일 때 메뉴 "준비한다" 로 호출 → AIMING 전이. 다른 페이즈에 호출되면 무시.
	void    Try_Throw();      // AIMING 일 때 마우스 좌클릭으로 호출 → THROWING 전이.
	void    Request_Run();    // 메뉴 "도망간다" / ESC 로 호출 → m_eResult=FAIL_RUN, DONE 전이. 이 DONE 이면 무시.
	_bool   Is_Done() const { return CAPTURE_PHASE::DONE == m_ePhase; }
	

	const CAPTURE_ENV&	Get_Env()    const { return m_tEnv; }
	CAPTURE_PHASE       Get_Phase()  const { return m_ePhase; }
	CAPTURE_RESULT      Get_Result() const { return m_eResult; }

private:
	void    Goto_Phase(CAPTURE_PHASE ePhase);

private:
	CAPTURE_ENV    m_tEnv = {};
	CAPTURE_PHASE  m_ePhase = { CAPTURE_PHASE::INTRO };
	CAPTURE_RESULT m_eResult = { CAPTURE_RESULT::NONE };
	_float         m_fPhaseElapsed = { 0.f };

public:
	static CCapture_Manager* Create(const CAPTURE_ENV& tEnv);

protected:
	virtual void Free() override;
};

NS_END