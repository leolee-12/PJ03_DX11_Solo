#include "Shadow.h"

CShadow::CShadow()
{
}

HRESULT CShadow::Add_Shadow_Light(const SHADOW_DESC& ShadowDesc)
{
    _float3     vUpDir = { 0.f, 1.f, 0.f };

    XMStoreFloat4x4(&m_TransformationMatrices[ENUM_CLASS(D3DTS::VIEW)],
        XMMatrixLookAtLH(
            XMVectorSetW(XMLoadFloat3(&ShadowDesc.vEye), 1.f), 
            XMVectorSetW(XMLoadFloat3(&ShadowDesc.vAt), 1.f), 
            XMLoadFloat3(&vUpDir)));

    XMStoreFloat4x4(&m_TransformationMatrices[ENUM_CLASS(D3DTS::PROJECTION)],
        XMMatrixPerspectiveFovLH(ShadowDesc.fFovy, ShadowDesc.fAspect, ShadowDesc.fNear, ShadowDesc.fFar));




    return S_OK;
}

CShadow* CShadow::Create()
{
    return new CShadow();
}

void CShadow::Free()
{
    __super::Free();


}
