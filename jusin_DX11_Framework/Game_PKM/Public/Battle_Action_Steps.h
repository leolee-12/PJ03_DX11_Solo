#pragma once
#include "IBattleAction_Step.h"
#include "Effect_Defines.h"
#include "Camera_Defines.h"

NS_BEGIN(Game_PKM)
class CMonsterBall;
class CBattle_Trainer;
class CEffect;
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

class STrainerFaint final : public IBattleAction_Step
{
private:
    STrainerFaint();
    virtual ~STrainerFaint() = default;

public:
    HRESULT Initialize(_uint iSide);
    virtual void  OnEnter(const BATTLE_CONTEXT& ctx) override;
    virtual void  Update(const BATTLE_CONTEXT& ctx, _float fTimeDelta) override;
    virtual _bool Is_Complete(const BATTLE_CONTEXT& ctx) const override;

private:
    _uint m_iSide = { 0 };

public:
    static STrainerFaint* Create(_uint iSide);

private:
    virtual void Free() override;
};

class SSendOutBall final : public IBattleAction_Step
{
private:
    SSendOutBall();
    virtual ~SSendOutBall() = default;

public:
    HRESULT Initialize(_uint iSide, _float fFlightDuration, CAMERA_SEQUENCE_ID eCameraSequence);

    virtual void  OnEnter(const BATTLE_CONTEXT& ctx) override;
    virtual void  Update(const BATTLE_CONTEXT& ctx, _float fTimeDelta) override;
    virtual _bool Is_Complete(const BATTLE_CONTEXT& ctx) const override;

private:
    _uint m_iSide = { 0 };
    _float m_fFlightDuration = { 0.72f };
    CAMERA_SEQUENCE_ID m_eCameraSequence = { CAMERA_SEQUENCE_ID::NONE };
    _float m_fElapsed = { 0.f };
    _bool m_bFinished = { false };

    _float3 m_vTargetPos = {};
    CMonsterBall* m_pBall = { nullptr }; // weak - Battle layer owns it

public:
    static SSendOutBall* Create(_uint iSide, _float fFlightDuration = 0.72f,
        CAMERA_SEQUENCE_ID eCameraSequence = CAMERA_SEQUENCE_ID::NONE);

private:
    virtual void Free() override;
};

class SPokemonEnter final : public IBattleAction_Step
{
private:
    SPokemonEnter();
    virtual ~SPokemonEnter() = default;

public:
    HRESULT Initialize(_uint iSide, _float fHoldSeconds);
    virtual void  OnEnter(const BATTLE_CONTEXT& ctx) override;
    virtual void  Update(const BATTLE_CONTEXT& ctx, _float fTimeDelta) override;
    virtual _bool Is_Complete(const BATTLE_CONTEXT& ctx) const override;

private:
    _uint m_iSide = { 0 };
    _float m_fGrace = { 0.f };
    _float m_fHoldSeconds = { 2.f };
    _bool m_bCryPlayed = { false };
    CBattle_Trainer* m_pHiddenTrainers[g_kBattleSideCount] = {};
    _bool m_bPrevTrainerVisible[g_kBattleSideCount] = {};

public:
    static SPokemonEnter* Create(_uint iSide, _float fHoldSeconds = 2.f);

private:
    void Restore_Trainers();
    virtual void Free() override;
};

class SPokemonSwitchOut final : public IBattleAction_Step
{
private:
    SPokemonSwitchOut();
    virtual ~SPokemonSwitchOut() = default;

public:
    HRESULT Initialize(_uint iSide, _float fDuration);
    virtual void OnEnter(const BATTLE_CONTEXT& ctx) override;
    virtual void Update(const BATTLE_CONTEXT& ctx, _float fTimeDelta) override;
    virtual _bool Is_Complete(const BATTLE_CONTEXT& ctx) const override;

private:
    _uint m_iSide = { 0 };
    _float m_fDuration = { 0.3f };
    _float m_fElapsed = { 0.f };

public:
    static SPokemonSwitchOut* Create(_uint iSide, _float fDuration = 0.3f);

private:
    virtual void Free() override;
};

class SApplySwitch final : public IBattleAction_Step
{
private:
    SApplySwitch();
    virtual ~SApplySwitch() = default;

public:
    HRESULT Initialize(_uint iSide, _uint iPartyIndex);
    virtual void OnEnter(const BATTLE_CONTEXT& ctx) override;
    virtual void Update(const BATTLE_CONTEXT& ctx, _float fTimeDelta) override;
    virtual _bool Is_Complete(const BATTLE_CONTEXT& ctx) const override;

private:
    _uint m_iSide = { 0 };
    _uint m_iPartyIndex = { 0 };
    _bool m_bApplied = { false };

public:
    static SApplySwitch* Create(_uint iSide, _uint iPartyIndex);

private:
    virtual void Free() override;
};

/* SSetPlateVisible
   - 즉시 완료. ctx.pManager->Set_PlateVisible(b) 호출.
   - 등장/교체 구간에서 플레이트를 숨기고, 끝나면 다시 표시. */
class SSetPlateVisible final : public IBattleAction_Step
{
private:
    SSetPlateVisible();
    virtual ~SSetPlateVisible() = default;

public:
    HRESULT Initialize(_bool bVisible);
    virtual void  OnEnter(const BATTLE_CONTEXT& ctx) override;
    virtual void  Update(const BATTLE_CONTEXT& ctx, _float fTimeDelta) override;
    virtual _bool Is_Complete(const BATTLE_CONTEXT& ctx) const override;

private:
    _bool m_bVisible = { false };

public:
    static SSetPlateVisible* Create(_bool bVisible);

private:
    virtual void Free() override;
};

/* SHideTrainers
     - 즉시 완료. 양측 트레이너를 모두 숨긴다(Set_BattleVisible(false)).
     - 송출 클로즈업 직전에 호출. */
class SHideTrainers final : public IBattleAction_Step
{
private:
    SHideTrainers();
    virtual ~SHideTrainers() = default;

public:
    virtual void  OnEnter(const BATTLE_CONTEXT& ctx) override;
    virtual void  Update(const BATTLE_CONTEXT& ctx, _float fTimeDelta) override;
    virtual _bool Is_Complete(const BATTLE_CONTEXT& ctx) const override;

public:
    static SHideTrainers* Create();

private:
    virtual void Free() override;
};

/* SRevealTrainers
   - 카메라가 전역 포즈(BATTLE_DEFAULT)로 복귀하고 시퀀스가 끝난 시점에
     양측 트레이너를 한꺼번에 노출(Set_BattleVisible(true)+Play_Focus).
   - 클로즈업 도중 재노출이 잡히지 않도록 대기. 안전 타임아웃 1.5초. */
class SRevealTrainers final : public IBattleAction_Step
{
private:
    SRevealTrainers();
    virtual ~SRevealTrainers() = default;

public:
    virtual void  OnEnter(const BATTLE_CONTEXT& ctx) override;
    virtual void  Update(const BATTLE_CONTEXT& ctx, _float fTimeDelta) override;
    virtual _bool Is_Complete(const BATTLE_CONTEXT& ctx) const override;

private:
    void Reveal(const BATTLE_CONTEXT& ctx);

    _float m_fElapsed = { 0.f };
    _bool  m_bRevealed = { false };

public:
    static SRevealTrainers* Create();

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
    _uint  m_iFaintedSide = { g_kBattleSideCount };
    _bool  m_bPublished = { false };
    _bool  m_bHitPlayed = { false };

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
    CEffect*            m_pEffect = { nullptr };   // AddRef 로 공동 소유, Free 에서 정리

public:
    static SPlayEffect* Create(const _string& strEffectID,
        EFFECT_VFX_TARGET eTarget,
        EFFECT_SLOT eSlot = EFFECT_SLOT::CENTER,
        const _float3& vOffset = {});

private:
    virtual void Free() override;
};

/* SPlayEffectProjectile
     - strEffectID 이펙트를 공격자 pivot 에서 스폰해 방어자 pivot 까지 fTravel 초 동안 직선 이동.
     - MATRIX attach 로 매 프레임 추종(CEffect::Late_Update). 도착 시 이펙트 Destroy 후 완료.
     - 비행 중 이펙트가 자체 소멸해도 안전하도록 AddRef 로 공동 소유한다. */
class SPlayEffectProjectile final : public IBattleAction_Step
{
private:
    SPlayEffectProjectile();
    virtual ~SPlayEffectProjectile() = default;

public:
    HRESULT Initialize(const _string& strEffectID,
        EFFECT_SLOT eStartSlot, const _float3& vStartOffset,
        EFFECT_SLOT eEndSlot, const _float3& vEndOffset,
        _float fTravel);

    virtual void  OnEnter(const BATTLE_CONTEXT& ctx) override;
    virtual void  Update(const BATTLE_CONTEXT& ctx, _float fTimeDelta) override;
    virtual _bool Is_Complete(const BATTLE_CONTEXT& ctx) const override;

private:
    void Update_Matrix(_float t);

    _string         m_strEffectID = {};
    EFFECT_SLOT     m_eStartSlot = { EFFECT_SLOT::CENTER };
    EFFECT_SLOT     m_eEndSlot = { EFFECT_SLOT::CENTER };
    _float3         m_vStartOffset = {};
    _float3         m_vEndOffset = {};
    _float          m_fTravel = { 0.3f };
    _float          m_fElapsed = { 0.f };

    _float3         m_vStartPos = {};
    _float3         m_vEndPos = {};
    _float4x4       m_mProjectile = {};        // CEffect 가 매 프레임 추종 (이 주소를 attach)
    CEffect*        m_pEffect = { nullptr };   // AddRef 로 공동 소유
    _bool           m_bFinished = { false };

public:
    static SPlayEffectProjectile* Create(const _string& strEffectID,
        EFFECT_SLOT eStartSlot = EFFECT_SLOT::CENTER, const _float3& vStartOffset = {},
        EFFECT_SLOT eEndSlot = EFFECT_SLOT::CENTER, const _float3& vEndOffset = {},
        _float fTravel = 0.3f);

private:
    virtual void Free() override;
};

/* SPlaySFX
      - 기술 사용 시 SFX 사운드 1회 재생 (CHANNELID::SFX).
      - fDelay > 0 이면 그 시간만큼 대기 후 재생하고 완료 (블로킹 지연).
        대기 동안 시퀀스가 멈추므로 VFX/데미지 대비 선후를 fDelay 로 맞춘다.
      - fDelay == 0 이면 OnEnter 에서 즉시 재생 후 완료. */
class SPlaySFX final : public IBattleAction_Step
{
private:
    SPlaySFX();
    virtual ~SPlaySFX() = default;

public:
    HRESULT Initialize(const _wstring& strSFXKey, _float fVolume, _float fDelay);

    virtual void  OnEnter(const BATTLE_CONTEXT& ctx) override;
    virtual void  Update(const BATTLE_CONTEXT& ctx, _float fTimeDelta) override;
    virtual _bool Is_Complete(const BATTLE_CONTEXT& ctx) const override;

private:
    void Play_SFX();

    _wstring m_strSFXKey = {};
    _float   m_fVolume = { 1.f };
    _float   m_fDelay = { 0.f };
    _float   m_fElapsed = { 0.f };
    _bool    m_bPlayed = { false };

public:
    static SPlaySFX* Create(const _wstring& strSFXKey, _float fVolume = 1.f, _float fDelay = 0.f);

private:
    virtual void Free() override;
};

/* SPlayCry
     - 지정 side 배틀러의 종족 울음(Common/Happy)을 1회 재생.
     - OnEnter 시점에 iSpeciesID 로 키를 해석하므로 교체/등장 등 동적 대상도 정확.
     - 종족ID -> SFX/pv%04u_<Kind>_adpcm.wav. 보유 목록 외 종족은 pv0025 폴백.
     - fDelay > 0 이면 그 시간만큼 대기 후 재생(블로킹 지연). */
enum class CRY_KIND { COMMON, HAPPY };

class SPlayCry final : public IBattleAction_Step
{
private:
    SPlayCry();
    virtual ~SPlayCry() = default;

public:
    HRESULT Initialize(_uint iSide, CRY_KIND eKind, _float fVolume, _float fDelay);

    virtual void  OnEnter(const BATTLE_CONTEXT& ctx) override;
    virtual void  Update(const BATTLE_CONTEXT& ctx, _float fTimeDelta) override;
    virtual _bool Is_Complete(const BATTLE_CONTEXT& ctx) const override;

private:
    void Play_Cry();

    _uint    m_iSide = { 0 };
    CRY_KIND m_eKind = { CRY_KIND::COMMON };
    _float   m_fVolume = { 1.f };
    _float   m_fDelay = { 0.f };
    _float   m_fElapsed = { 0.f };
    _bool    m_bPlayed = { false };
    _wstring m_strKey = {};

public:
    static SPlayCry* Create(_uint iSide, CRY_KIND eKind, _float fVolume = 1.f, _float fDelay = 0.f);

private:
    virtual void Free() override;
};

NS_END
