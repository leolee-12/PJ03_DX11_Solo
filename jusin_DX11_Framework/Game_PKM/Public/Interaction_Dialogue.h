#pragma once
#include "Interaction.h"

NS_BEGIN(Game_PKM)

class CInteraction_Dialogue final : public CInteraction
{
public:
    struct INTERACTION_DIALOGUE_DESC
    {
        _wstring strDialogueKey;
    };

private:
    CInteraction_Dialogue(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CInteraction_Dialogue(const CInteraction_Dialogue& Prototype);
    virtual ~CInteraction_Dialogue() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;

public:
    virtual _bool Supports(INTERACTION_EVENT eEvent) const override;
    virtual _bool CanInteract(const INTERACTION_CONTEXT& ctx) const override;
    virtual void Execute(const INTERACTION_CONTEXT& ctx) override;

private:
    _wstring m_strDialogueKey;

public:
    static CInteraction_Dialogue* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CComponent* Clone(void* pArg) override;

private:
    virtual void Free() override;
};

NS_END