#pragma once
#include "Game_PKM_Defines.h"
#include "Battle_Session.h"

#include "ContainerObject.h"

NS_BEGIN(Game_PKM)

class CBattle_Trainer final : public CContainerObject
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
    virtual void Update(_float fTimeDelta) override;
    virtual void Late_Update(_float fTimeDelta) override;
    virtual HRESULT Render() override;

private:
    _uint m_iSide = { g_kBattleSide_Player };
    WNameID m_strBodyProtoTag = {};
    WNameID m_strModelProtoTag = {};

private:
    HRESULT Ready_PartObjects(const BATTLE_TRAINER_DESC* pDesc);

public:
    static CBattle_Trainer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg) override;

private:
    virtual void Free() override;
};

NS_END