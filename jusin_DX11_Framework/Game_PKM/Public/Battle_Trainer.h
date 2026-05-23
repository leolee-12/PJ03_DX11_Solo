#pragma once
#include "Actor.h"
#include "Game_PKM_Defines.h"
#include "Battle_Session.h"
#include "Battle_AnimDef.h"


NS_BEGIN(Game_PKM)

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
    virtual void Tick_Movement(_float fTimeDelta) override;

    void Play_Intro();
    void Play_Throw();
    void Play_Focus();
    void Play_Faint();

private:
    _uint m_iSide = { g_kBattleSide_Player };
    WNameID m_strBodyProtoTag = {};
    WNameID m_strModelProtoTag = {};
    WNameID m_strModelTag = {};

    ANIM_KIND m_eCurrentKind = { ANIM_KIND::IDLE };
    _float m_fAnimTimer = { 0.f };
    _float m_fAnimDuration = { 0.f };


private:
    HRESULT Ready_PartObjects(const BATTLE_TRAINER_DESC* pDesc);

    void Play_Anim_NonLoop(ANIM_KIND eKind, _float fDuration);
    void Play_Anim_Loop(ANIM_KIND eKind);
    void Return_To_Focus();

public:
    static CBattle_Trainer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg) override;

private:
    virtual void Free() override;
};

NS_END