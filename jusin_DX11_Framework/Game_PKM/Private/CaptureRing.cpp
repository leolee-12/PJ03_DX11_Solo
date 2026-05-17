#include "CaptureRing.h"

#include "GameInstance.h"

CCaptureRing::CCaptureRing(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CPartObject{m_pDevice, m_pContext}
{

}

CCaptureRing::CCaptureRing(const CCaptureRing& Prototype)
    : CPartObject{ Prototype }
{

}

HRESULT CCaptureRing::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CCaptureRing::Initialize(void* pArg)
{
    return S_OK;
}

void CCaptureRing::Priority_Update(_float fTimeDelta)
{
}

void CCaptureRing::Update(_float fTimeDelta)
{
}

void CCaptureRing::Late_Update(_float fTimeDelta)
{
}

HRESULT CCaptureRing::Render()
{
    return S_OK;
}

HRESULT CCaptureRing::Ready_Components()
{
    return S_OK;
}

HRESULT CCaptureRing::Bind_ShaderResources()
{
    return S_OK;
}

CCaptureRing* CCaptureRing::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CCaptureRing* pInstance = new CCaptureRing(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CCaptureRing");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CCaptureRing::Clone(void* pArg)
{
    CCaptureRing* pInstance = new CCaptureRing(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CCaptureRing");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CCaptureRing::Free()
{
    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pColliderCom);

    __super::Free();
}