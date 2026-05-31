#pragma once
#include "Base.h"
#include "Damage_Pipe.h"

NS_BEGIN(Game_PKM)

class IBattleAction_Step;
struct BATTLE_CONTEXT;

/* BATTLE_ACTION_DATA
	 - 한 액션 시퀀스 동안 step 들 사이에서 공유되는 데이터.
	 - SAccuracyCheck 가 hit 결과 기록, SApplyDamage 가 데미지 결과 기록 등.
	 - Sequencer 가 멤버로 보유. step 은 ctx.pManager->Get_Sequencer()->Get_ActionData() 로 접근. */
struct BATTLE_ACTION_DATA
{
	_uint  iActorSide = { 0 };
	_uint  iActorSlot = { 0 };
	_uint  iTargetSide = { 0 };
	_uint  iTargetSlot = { 0 };
	_uint  iMoveID = { 0 };
	_uint  iMoveSlot = { 0 };

	_bool  bAccuracyHit = { false };   // SAccuracyCheck 가 기록
	_bool  bImmune = { false };        // SAccuracyCheck 가 기록 — 타입 상성상 무효(효과 0)
	DAMAGE_PIPE_DATA tPipe = {};          // SApplyDamage 가 기록 (crit/effectiveness/final 등)
	_ushort iAppliedDamage = { 0 };
	_bool  bFaintedThisHit = { false };

	// 명중 + 비면역일 때만 공격/피격 연출(애니·이펙트·사운드)을 출력한다.
	_bool  Connects() const { return bAccuracyHit && (false == bImmune); }
};

/* CBattle_ActionSequencer
   - Manager 가 owns. 한 시점에 하나의 액션 시퀀스만 실행.
   - 시퀀스는 Push_Step 으로 빌드 후 Submit 으로 시작.
   - Tick 이 매 프레임 현재 step 의 Update 를 호출, Is_Complete 시 다음 step 으로 진행.
   - 활성 동안 ResolveAction State 가 락처럼 사용 - Is_Active() == true 면 다음 단계 보류. */
class CBattle_ActionSequencer final : public CBase
{
private:
	CBattle_ActionSequencer();
	virtual ~CBattle_ActionSequencer() = default;

public:
	HRESULT Initialize();

	void Push_Step(IBattleAction_Step* pStep);  // 시퀀스 빌드 (ref +1)
	void Submit();                              // 빌드 완료 후 시작
	void Clear();                               // 강제 종료 + step 모두 해제

	void Tick(const BATTLE_CONTEXT& ctx, _float fTimeDelta);

	_bool Is_Active() const { return m_bActive; }
	_bool Is_Empty()  const { return m_vSteps.empty(); }

	BATTLE_ACTION_DATA& Get_ActionData() { return m_tActionData; }
	const BATTLE_ACTION_DATA& Get_ActionData() const { return m_tActionData; }
	void Reset_ActionData() { m_tActionData = {}; }

private:
	std::vector<IBattleAction_Step*> m_vSteps;
	BATTLE_ACTION_DATA m_tActionData = {};
	_uint m_iCursor = { 0 };
	_bool m_bActive = { false };
	_bool m_bStepEntered = { false };

public:
	static CBattle_ActionSequencer* Create();

private:
	virtual void Free() override;
};

NS_END