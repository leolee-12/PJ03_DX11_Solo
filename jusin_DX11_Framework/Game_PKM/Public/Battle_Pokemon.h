#pragma once
#include "Game_PKM_Defines.h"
#include "Battle_Session.h"

#include "ContainerObject.h"

NS_BEGIN(Engine)
class CRenderRule;
NS_END

NS_BEGIN(Game_PKM)

class CBattle_Pokemon final : public CContainerObject
{
public:
    struct POKEMON_DESC : public CGameObject::GAMEOBJECT_DESC
    {
        POKEMON_INSTANCE* pInstance = { nullptr };
        _uint iSide = { g_kBattleSide_Player };
        WNameID strBodyProtoTag = {};
        WNameID strShaderProtoTag = {};
        const CRenderRule* pRenderRule = { nullptr };
        _uint iDefaultAnim = { 0 };
        _bool bLoop = { true };
        _float fScale = { 1.f };

        _float3 vPos = { 0.f, 0.f, 0.f };
        _float fYaw = { 0.f };
    };

protected:
    CBattle_Pokemon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CBattle_Pokemon(const CBattle_Pokemon& Prototype);
    virtual ~CBattle_Pokemon() = default;

public:
    virtual _string Get_TypeName() const override { return "Pokemon"; }
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Priority_Update(_float fTimeDelta) override;
    virtual void Update(_float fTimeDelta) override;
    virtual void Late_Update(_float fTimeDelta) override;
    virtual HRESULT Render() override;

private:
    POKEMON_INSTANCE* m_pInstance = { nullptr };
    _uint m_iSide = { g_kBattleSide_Player };
    WNameID m_strBodyProtoTag = {};
    WNameID m_strSpeciesModelTag = {};
    const CRenderRule* m_pRenderRule = { nullptr };

private:
    HRESULT Ready_PartObjects(const POKEMON_DESC* pDesc);

public:
    static CBattle_Pokemon* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg) override;

private:
    virtual void Free() override;
};

NS_END