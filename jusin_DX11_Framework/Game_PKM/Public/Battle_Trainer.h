#pragma once
#include "Actor.h"
#include "Game_PKM_Defines.h"
#include "Battle_Session.h"
#include "Battle_AnimDef.h"


NS_BEGIN(Game_PKM)
class CBattle_Ball;

class CBattle_Trainer final : public CActor
{
public:
    struct BATTLE_TRAINER_DESC : public CGameObject::GAMEOBJECT_DESC
    {
        _uint iSide = { g_kBattleSide_Player };
        WNameID strBodyProtoTag = {};
        WNameID strModelProtoTag = {};
        WNameID strShaderProtoTag = {};
        _uint iDefaultAnim = { 17 };
        _bool bLoop = { true };
        _float fScale = { 1.f };

        _float3 vPos = { 0.f, 0.f, 0.f };
        _float fYaw = { 0.f };

        _char szMappingPath[256] = {};
    };

protected:
    CBattle_Trainer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CBattle_Trainer(const CBattle_Trainer& Prototype);
    virtual ~CBattle_Trainer() = default;

public:
    virtual _string Get_TypeName() const override { return "BattleTrainer"; }

    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Priority_Update(_float fTimeDelta) override;
    virtual void Late_Update(_float fTimeDelta) override;
    virtual void Tick_Movement(_float fTimeDelta) override;

    void Play_Intro();
    void Play_Throw();
    void Play_Focus();
    void Play_Faint();
    void Set_BattleVisible(_bool bVisible) { m_bBattleVisible = bVisible; }
    _bool Is_BattleVisible() const { return m_bBattleVisible; }

private:
    _uint m_iSide = { g_kBattleSide_Player };
    WNameID m_strBodyProtoTag = {};
    WNameID m_strModelProtoTag = {};
    WNameID m_strModelTag = {};

    ANIM_KIND m_eCurrentKind = { ANIM_KIND::IDLE };
    _float m_fAnimTimer = { 0.f };
    _float m_fAnimDuration = { 0.f };
    _bool m_bBattleVisible = { true };

    CBattle_Ball* m_pBattleBall = { nullptr };
    BALL_THROW_DESC m_tPendingBallThrow = {};
    _float m_fPendingBallThrowDelay = { 0.f };
    _bool m_bPendingBallThrow = { false };

#ifdef _DEBUG
    _uint m_iDbgBallAnim = { 0u };
#endif

private:
    HRESULT Ready_PartObjects(const BATTLE_TRAINER_DESC* pDesc);

    void Play_Anim_NonLoop(ANIM_KIND eKind, _float fDuration);
    void Play_Anim_Loop(ANIM_KIND eKind);
    void Return_To_Focus();
    void Tick_BallThrow(_float fTimeDelta);
    void Start_BallThrow(const BALL_THROW_DESC& Desc);
    _bool Sync_BallToRightHand(const _float3& vCorrection);

#ifdef _DEBUG
    void Debug_BallThrowTune();
#endif

public:
    static CBattle_Trainer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg) override;

private:
    virtual void Free() override;
};

NS_END
