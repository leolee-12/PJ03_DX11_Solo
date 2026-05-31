#include "Interaction_BallHit.h"

CInteraction_BallHit::CInteraction_BallHit(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CInteraction{ pDevice, pContext }
{
}

CInteraction_BallHit::CInteraction_BallHit(const CInteraction_BallHit& Prototype)
    : CInteraction{ Prototype }
{
}

HRESULT CInteraction_BallHit::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CInteraction_BallHit::Initialize(void* pArg)
{
    if (nullptr == pArg)
        return E_FAIL;

    const INTERACTION_BALLHIT_DESC* pDesc = static_cast<const INTERACTION_BALLHIT_DESC*>(pArg);
    m_iBallItemID = pDesc->iBallItemID;

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    return S_OK;
}

_bool CInteraction_BallHit::Supports(INTERACTION_EVENT eEvent) const
{
    return INTERACTION_EVENT::TRIGGER_ENTER == eEvent;
}

_bool CInteraction_BallHit::CanInteract(const INTERACTION_CONTEXT& ctx) const
{
    if (!Supports(ctx.eEvent))
        return false;

    // TODO: 매니저가 SHAKE/RESULT 페이즈 진행 중이면 false (이번 단위에서는 일단 통과)
    return true;
}

void CInteraction_BallHit::Execute(const INTERACTION_CONTEXT& ctx)
{
    // TODO: Capture_Manager 와 결합 후 매니저에 hit 통보 (Request_Throw 의 후속 발화)
    OutputDebugStringW(L"[Interaction_BallHit] hit (stub)\n");
}

CInteraction_BallHit* CInteraction_BallHit::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CInteraction_BallHit* pInstance = new CInteraction_BallHit(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CInteraction_BallHit");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CComponent* CInteraction_BallHit::Clone(void* pArg)
{
    CInteraction_BallHit* pInstance = new CInteraction_BallHit(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CInteraction_BallHit");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CInteraction_BallHit::Free()
{
    __super::Free();
}