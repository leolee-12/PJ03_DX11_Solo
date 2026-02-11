#include "Bounding_AABB.h"
#include "Bounding_OBB.h"
#include "Bounding_Sphere.h"

CBounding_OBB::CBounding_OBB(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CBounding{ pDevice, pContext }
{
}

HRESULT CBounding_OBB::Initialize(CBounding::BOUNDING_DESC* pInitialDesc)
{
    OBB_DESC*  pDesc = static_cast<CBounding_OBB::OBB_DESC*>(pInitialDesc);

    _float4     vQuaternion = {};
    XMStoreFloat4(&vQuaternion,
        XMQuaternionRotationRollPitchYaw(pDesc->vRadians.x, pDesc->vRadians.y, pDesc->vRadians.z));

    m_pOriginalDesc = new BoundingOrientedBox(pDesc->vCenter, _float3(pDesc->vSize.x * 0.5f, pDesc->vSize.y * 0.5f, pDesc->vSize.z * 0.5f), vQuaternion);
    m_pDesc = new BoundingOrientedBox(*m_pOriginalDesc);

    return S_OK;
}

void CBounding_OBB::Update(_fmatrix WorldMatrix)
{
    m_pOriginalDesc->Transform(*m_pDesc, WorldMatrix);
}

_bool CBounding_OBB::Intersect(COLLIDER eType, CBounding* pBounding)
{
    _bool       isColl = { false };
    switch (eType)
    {
    case COLLIDER::AABB:
        isColl = m_pDesc->Intersects(*static_cast<CBounding_AABB*>(pBounding)->Get_Desc());
        break;

    case COLLIDER::OBB:
        isColl = Intersect_OBB(static_cast<CBounding_OBB*>(pBounding));
        break;

    case COLLIDER::SPHERE:
        isColl = m_pDesc->Intersects(*static_cast<CBounding_Sphere*>(pBounding)->Get_Desc());
        break;
    }

    return isColl;
}

#ifdef _DEBUG

HRESULT CBounding_OBB::Render(PrimitiveBatch<VertexPositionColor>* pBatch, _fvector vColor)
{
    DX::Draw(pBatch, *m_pDesc, vColor);

    return S_OK;
}

#endif

CBounding_OBB::tagOBB CBounding_OBB::Compute_OBB()
{
    tagOBB          OBBDesc{};

    _float3         vPoints[8] = {};

    m_pDesc->GetCorners(vPoints);

    OBBDesc.vCenter = m_pDesc->Center;

    XMStoreFloat3(&OBBDesc.vCenterDir[0],
        (XMLoadFloat3(&vPoints[5]) - XMLoadFloat3(&vPoints[4])) * 0.5f);
    XMStoreFloat3(&OBBDesc.vCenterDir[1],
        (XMLoadFloat3(&vPoints[7]) - XMLoadFloat3(&vPoints[4])) * 0.5f);
    XMStoreFloat3(&OBBDesc.vCenterDir[2],
        (XMLoadFloat3(&vPoints[0]) - XMLoadFloat3(&vPoints[4])) * 0.5f);

    for (size_t i = 0; i < 3; i++)
    {
        XMStoreFloat3(&OBBDesc.vAlignDir[i],
            XMVector3Normalize(XMLoadFloat3(&OBBDesc.vCenterDir[i])));
    }
    
    return OBBDesc;
}

_bool CBounding_OBB::Intersect_OBB(CBounding_OBB* pTargetBounding)
{
    tagOBB      OBBDesc[2] = {};

    OBBDesc[0] = Compute_OBB();
    OBBDesc[1] = pTargetBounding->Compute_OBB();

    _float      fLength[3] = {};

    for (size_t i = 0; i < 2; i++)
    {
        for (size_t j = 0; j < 3; j++)        
        {
            fLength[0] = fabs(XMVectorGetX(XMVector3Dot(XMLoadFloat3(&OBBDesc[1].vCenter) - XMLoadFloat3(&OBBDesc[0].vCenter),
                XMLoadFloat3(&OBBDesc[i].vAlignDir[j]))));

            fLength[1] =
                fabs(XMVectorGetX(XMVector3Dot(XMLoadFloat3(&OBBDesc[0].vCenterDir[0]),
                    XMLoadFloat3(&OBBDesc[i].vAlignDir[j])))) +
                fabs(XMVectorGetX(XMVector3Dot(XMLoadFloat3(&OBBDesc[0].vCenterDir[1]),
                    XMLoadFloat3(&OBBDesc[i].vAlignDir[j])))) +
                fabs(XMVectorGetX(XMVector3Dot(XMLoadFloat3(&OBBDesc[0].vCenterDir[2]),
                    XMLoadFloat3(&OBBDesc[i].vAlignDir[j]))));

            fLength[2] =
                fabs(XMVectorGetX(XMVector3Dot(XMLoadFloat3(&OBBDesc[1].vCenterDir[0]),
                    XMLoadFloat3(&OBBDesc[i].vAlignDir[j])))) +
                fabs(XMVectorGetX(XMVector3Dot(XMLoadFloat3(&OBBDesc[1].vCenterDir[1]),
                    XMLoadFloat3(&OBBDesc[i].vAlignDir[j])))) +
                fabs(XMVectorGetX(XMVector3Dot(XMLoadFloat3(&OBBDesc[1].vCenterDir[2]),
                    XMLoadFloat3(&OBBDesc[i].vAlignDir[j]))));

            if (fLength[0] > (fLength[1] + fLength[2]))
                return false;
        }
    }
    

    return true;
}

CBounding_OBB* CBounding_OBB::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CBounding::BOUNDING_DESC* pInitialDesc)
{
    CBounding_OBB* pInstance = new CBounding_OBB(pDevice, pContext);

    if (FAILED(pInstance->Initialize(pInitialDesc)))
    {
        MSG_BOX("Failed to Created : CBounding_OBB");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CBounding_OBB::Free()
{
    __super::Free();

    Safe_Delete(m_pDesc);
    Safe_Delete(m_pOriginalDesc);
}
