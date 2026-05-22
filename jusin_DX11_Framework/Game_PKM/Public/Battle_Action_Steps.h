#pragma once
#include "IBattleAction_Step.h"
#include "Effect_Defines.h"

NS_BEGIN(Game_PKM)

/* SDelay
   - 순수 시간 대기. duration 초 경과 시 완료.
   - 메시지 사이 텀, anim 페이즈 사이 텀 등 페이싱 조절용. */
class SDelay final : public IBattleAction_Step
{
private:
	SDelay();
	virtual ~SDelay() = default;

public:
	HRESULT Initialize(_float fDuration);

	virtual void  OnEnter(const BATTLE_CONTEXT& ctx) override;
	virtual void  Update(const BATTLE_CONTEXT& ctx, _float fTimeDelta) override;
	virtual _bool Is_Complete(const BATTLE_CONTEXT& ctx) const override;

private:
	_float m_fDuration = { 0.f };
	_float m_fElapsed = { 0.f };

public:
	static SDelay* Create(_float fDuration);

private:
	virtual void Free() override;
};

/* SCloseMsg
   - BattleMsg 가 열려 있으면 Close() 호출 후 즉시 완료.
   - Close 후의 후속 대기는 별도 SDelay step 으로 처리. */
class SCloseMsg final : public IBattleAction_Step
{
private:
	SCloseMsg();
	virtual ~SCloseMsg() = default;

public:
	virtual void  OnEnter(const BATTLE_CONTEXT& ctx) override;
	virtual void  Update(const BATTLE_CONTEXT& ctx, _float fTimeDelta) override;
	virtual _bool Is_Complete(const BATTLE_CONTEXT& ctx) const override;

public:
	static SCloseMsg* Create();

private:
	virtual void Free() override;
};

class SBattleText final : public IBattleAction_Step
{
private:
    SBattleText();
    virtual ~SBattleText() = default;

public:
    HRESULT Initialize(const _wstring& strText, _float fHoldSeconds);
    virtual void  OnEnter(const BATTLE_CONTEXT& ctx) override;
    virtual void  Update(const BATTLE_CONTEXT& ctx, _float fTimeDelta) override;
    virtual _bool Is_Complete(const BATTLE_CONTEXT& ctx) const override;

private:
    _wstring m_strText = {};
    _float m_fHoldSeconds = { 0.4f };
    _float m_fHoldTimer = { 0.f };
    _bool m_bOpened = { false };

public:
    static SBattleText* Create(const _wstring& strText, _float fHoldSeconds = 0.4f);

private:
    virtual void Free() override;
};

class STrainerThrow final : public IBattleAction_Step
{
private:
    STrainerThrow();
    virtual ~STrainerThrow() = default;

public:
    HRESULT Initialize(_uint iSide, _float fDuration);
    virtual void  OnEnter(const BATTLE_CONTEXT& ctx) override;
    virtual void  Update(const BATTLE_CONTEXT& ctx, _float fTimeDelta) override;
    virtual _bool Is_Complete(const BATTLE_CONTEXT& ctx) const override;

private:
    _uint m_iSide = { 0 };
    _float m_fDuration = { 0.8f };
    _float m_fElapsed = { 0.f };

public:
    static STrainerThrow* Create(_uint iSide, _float fDuration = 0.8f);

private:
    virtual void Free() override;
};

class SPokemonEnter final : public IBattleAction_Step
{
private:
    SPokemonEnter();
    virtual ~SPokemonEnter() = default;

public:
    HRESULT Initialize(_uint iSide);
    virtual void  OnEnter(const BATTLE_CONTEXT& ctx) override;
    virtual void  Update(const BATTLE_CONTEXT& ctx, _float fTimeDelta) override;
    virtual _bool Is_Complete(const BATTLE_CONTEXT& ctx) const override;

private:
    _uint m_iSide = { 0 };
    _float m_fGrace = { 0.f };

public:
    static SPokemonEnter* Create(_uint iSide);

private:
    virtual void Free() override;
};

/* SDone
   - 시퀀스 마지막에 명시적으로 두는 sentinel. 즉시 완료.
   - 시퀀서는 빈 step 도달 시 자동 종료하므로 필수는 아니지만,
	 CMoveCommand 등에서 시퀀스 끝을 명시적으로 표현할 때 사용. */
class SDone final : public IBattleAction_Step
{
private:
	SDone();
	virtual ~SDone() = default;

public:
	virtual void  OnEnter(const BATTLE_CONTEXT& ctx) override;
	virtual void  Update(const BATTLE_CONTEXT& ctx, _float fTimeDelta) override;
	virtual _bool Is_Complete(const BATTLE_CONTEXT& ctx) const override;

public:
	static SDone* Create();

private:
	virtual void Free() override;
};

/* SAnnounce
     - 무브 발성 메시지 (EVENT_MOVE_USED) 발행 + BattleMsg 표시 완료 대기.
     - Listener 가 한글 메시지로 변환하여 큐에 push, 표시 후 자동 Close.
     - step 은 박스가 닫힐 때까지 대기. */
class SAnnounce final : public IBattleAction_Step
{
private:
    SAnnounce();
    virtual ~SAnnounce() = default;

public:
    HRESULT Initialize(_uint iActorSide, _uint iMoveID);

    virtual void  OnEnter(const BATTLE_CONTEXT& ctx) override;
    virtual void  Update(const BATTLE_CONTEXT& ctx, _float fTimeDelta) override;
    virtual _bool Is_Complete(const BATTLE_CONTEXT& ctx) const override;

private:
    _uint  m_iActorSide = { 0 };
    _uint  m_iMoveID = { 0 };
    _float m_fGrace = { 0.f };

public:
    static SAnnounce* Create(_uint iActorSide, _uint iMoveID);

private:
    virtual void Free() override;
};

/* SMissMessage
   - 빗나감 메시지 (EVENT_MOVE_FAILED, MISSED) 발행 + 표시 완료 대기.
   - Listener 가 "그러나 빗나갔다!" 메시지로 변환. */
class SMissMessage final : public IBattleAction_Step
{
private:
    SMissMessage();
    virtual ~SMissMessage() = default;

public:
    HRESULT Initialize(_uint iActorSide, _uint iMoveID);

    virtual void  OnEnter(const BATTLE_CONTEXT& ctx) override;
    virtual void  Update(const BATTLE_CONTEXT& ctx, _float fTimeDelta) override;
    virtual _bool Is_Complete(const BATTLE_CONTEXT& ctx) const override;

private:
    _uint  m_iActorSide = { 0 };
    _uint  m_iMoveID = { 0 };
    _float m_fGrace = { 0.f };
    _bool  m_bPublished = { false };

public:
    static SMissMessage* Create(_uint iActorSide, _uint iMoveID);

private:
    virtual void Free() override;
};

/* SResultMessages
   - 직전 step (SApplyDamage 등) 이 발행한 메시지 들(크리/효과적/등)이
     모두 표시되고 박스가 닫힐 때까지 대기.
   - 자체 EVENT 발행은 없음 - 순수 대기 step. */
class SResultMessages final : public IBattleAction_Step
{
private:
    SResultMessages();
    virtual ~SResultMessages() = default;

public:
    virtual void  OnEnter(const BATTLE_CONTEXT& ctx) override;
    virtual void  Update(const BATTLE_CONTEXT& ctx, _float fTimeDelta) override;
    virtual _bool Is_Complete(const BATTLE_CONTEXT& ctx) const override;

private:
    _float m_fGrace = { 0.f };

public:
    static SResultMessages* Create();

private:
    virtual void Free() override;
};

/* SAccuracyCheck
     - Sequencer ActionData 의 actor/target/move 정보로 명중 판정 수행.
     - 결과를 ActionData.bAccuracyHit 에 기록. 즉시 완료. */
class SAccuracyCheck final : public IBattleAction_Step
{
private:
    SAccuracyCheck();
    virtual ~SAccuracyCheck() = default;

public:
    virtual void  OnEnter(const BATTLE_CONTEXT& ctx) override;
    virtual void  Update(const BATTLE_CONTEXT& ctx, _float fTimeDelta) override;
    virtual _bool Is_Complete(const BATTLE_CONTEXT& ctx) const override;

public:
    static SAccuracyCheck* Create();

private:
    virtual void Free() override;
};

/* SApplyDamage
   - DAMAGE_PIPE_DATA 빌드 -> CDamage_Calculator 통과 -> CBattler::Apply_Damage.
   - 결과를 ActionData.tPipe / iAppliedDamage / bFaintedThisHit 에 기록.
   - 면역(effectiveness<=0) 시 MOVE_FAILED(IMMUNE) 발행 + 데미지 미적용.
   - 적용 시 DAMAGE_DEALT 발행. 즉시 완료 (메시지 대기는 SResultMessages 가 담당). */
class SApplyDamage final : public IBattleAction_Step
{
private:
    SApplyDamage();
    virtual ~SApplyDamage() = default;

public:
    virtual void  OnEnter(const BATTLE_CONTEXT& ctx) override;
    virtual void  Update(const BATTLE_CONTEXT& ctx, _float fTimeDelta) override;
    virtual _bool Is_Complete(const BATTLE_CONTEXT& ctx) const override;

public:
    static SApplyDamage* Create();

private:
    virtual void Free() override;
};

/* SFaintCheck
   - ActionData.bFaintedThisHit 검사. true 면 POKEMON_FAINTED 발행 + 메시지 표시 대기.
   - false 면 즉시 완료. */
class SFaintCheck final : public IBattleAction_Step
{
private:
    SFaintCheck();
    virtual ~SFaintCheck() = default;

public:
    virtual void  OnEnter(const BATTLE_CONTEXT& ctx) override;
    virtual void  Update(const BATTLE_CONTEXT& ctx, _float fTimeDelta) override;
    virtual _bool Is_Complete(const BATTLE_CONTEXT& ctx) const override;

private:
    _float m_fGrace = { 0.f };
    _bool  m_bPublished = { false };

public:
    static SFaintCheck* Create();

private:
    virtual void Free() override;
};

/* SPrizeMoney
     - 트레이너 OUTRO 의 상금 step.
     - OnEnter 시 Player_Status.m_iMoney 를 1회 증가 (m_bApplied 가드).
     - 메시지 박스에 "플레이어는 상금으로 N원을 손에 넣었다!" 텍스트 표시.
     - SBattleText 와 동일한 hold/Type 페이스 규약 사용. */
class SPrizeMoney final : public IBattleAction_Step
{
private:
    SPrizeMoney();
    virtual ~SPrizeMoney() = default;

public:
    HRESULT Initialize(_uint iAmount, _float fHoldSeconds);

    virtual void  OnEnter(const BATTLE_CONTEXT& ctx) override;
    virtual void  Update(const BATTLE_CONTEXT& ctx, _float fTimeDelta) override;
    virtual _bool Is_Complete(const BATTLE_CONTEXT& ctx) const override;

private:
    _uint    m_iAmount = { 0 };
    _wstring m_strText = {};
    _float   m_fHoldSeconds = { 0.4f };
    _float   m_fHoldTimer = { 0.f };
    _bool    m_bOpened = { false };
    _bool    m_bApplied = { false };

public:
    static SPrizeMoney* Create(_uint iAmount, _float fHoldSeconds = 0.4f);

private:
    virtual void Free() override;
};

/* SPlayEffect
     - ActionData 의 actor/target side 로 CBattle_Pokemon 을 찾아
       Get_EffectPivot(slot, offset) 위치에서 strID 이펙트 1회 출력.
     - duration 0 (즉시 완료). 페이싱은 앞뒤 SDelay 가 담당. */
class SPlayEffect final : public IBattleAction_Step
{
private:
    SPlayEffect();
    virtual ~SPlayEffect() = default;

public:
    HRESULT Initialize(const _string& strEffectID,
        EFFECT_VFX_TARGET eTarget,
        EFFECT_SLOT eSlot,
        const _float3& vOffset);

    virtual void  OnEnter(const BATTLE_CONTEXT& ctx) override;
    virtual void  Update(const BATTLE_CONTEXT& ctx, _float fTimeDelta) override;
    virtual _bool Is_Complete(const BATTLE_CONTEXT& ctx) const override;

private:
    _string             m_strEffectID = {};
    EFFECT_VFX_TARGET   m_eTarget = { EFFECT_VFX_TARGET::ATTACKER };
    EFFECT_SLOT         m_eSlot = { EFFECT_SLOT::CENTER };
    _float3             m_vOffset = {};

public:
    static SPlayEffect* Create(const _string& strEffectID,
        EFFECT_VFX_TARGET eTarget,
        EFFECT_SLOT eSlot = EFFECT_SLOT::CENTER,
        const _float3& vOffset = {});

private:
    virtual void Free() override;
};

NS_END