#pragma once
#include "Interaction.h"

NS_BEGIN(Game_PKM)

class CInteraction_BallHit final : public CInteraction
{
public:
    struct INTERACTION_BALLHIT_DESC
    {
        _uint iBallItemID = { 0 };   // 예: 몬스터볼 / 슈퍼볼 / 마스터볼 식별
    };

private:
    CInteraction_BallHit(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CInteraction_BallHit(const CInteraction_BallHit& Prototype);
    virtual ~CInteraction_BallHit() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;

public:
    virtual _bool Supports(INTERACTION_EVENT eEvent) const override;
    virtual _bool CanInteract(const INTERACTION_CONTEXT& ctx) const override;
    virtual void  Execute(const INTERACTION_CONTEXT& ctx) override;

private:
    _uint m_iBallItemID = { 0 };

public:
    static CInteraction_BallHit* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CComponent* Clone(void* pArg) override;

private:
    virtual void Free() override;
};

NS_END