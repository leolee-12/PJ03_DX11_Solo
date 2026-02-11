#include "Bounding_AABB.h"
#include "Bounding_OBB.h"
#include "Bounding_Sphere.h"

CBounding_AABB::CBounding_AABB(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CBounding{ pDevice, pContext }
{
}

HRESULT CBounding_AABB::Initialize(CBounding::BOUNDING_DESC* pInitialDesc)
{
    AABB_DESC*  pDesc = static_cast<CBounding_AABB::AABB_DESC*>(pInitialDesc);

    m_pOriginalDesc = new BoundingBox(pDesc->vCenter, _float3(pDesc->vSize.x * 0.5f, pDesc->vSize.y * 0.5f, pDesc->vSize.z * 0.5f));
    m_pDesc = new BoundingBox(*m_pOriginalDesc);

    return S_OK;
}

void CBounding_AABB::Update(_fmatrix WorldMatrix)
{
    _matrix     TransformMatrix = WorldMatrix;

    TransformMatrix.r[0] = XMVectorSet(1.f, 0.f, 0.f, 0.f) * XMVector3Length(WorldMatrix.r[0]);
    TransformMatrix.r[1] = XMVectorSet(0.f, 1.f, 0.f, 0.f) * XMVector3Length(WorldMatrix.r[1]);
    TransformMatrix.r[2] = XMVectorSet(0.f, 0.f, 1.f, 0.f) * XMVector3Length(WorldMatrix.r[2]);

    m_pOriginalDesc->Transform(*m_pDesc, TransformMatrix);
}

_bool CBounding_AABB::Intersect(COLLIDER eType, CBounding* pBounding)
{
    _bool       isColl = { false };
    switch (eType)
    {
    case COLLIDER::AABB:
        // isColl = m_pDesc->Intersects(*static_cast<CBounding_AABB*>(pBounding)->Get_Desc());
        isColl = Intersect_AABB(static_cast<CBounding_AABB*>(pBounding));
        break;

    case COLLIDER::OBB:
        isColl = m_pDesc->Intersects(*static_cast<CBounding_OBB*>(pBounding)->Get_Desc());
        break;

    case COLLIDER::SPHERE:
        isColl = m_pDesc->Intersects(*static_cast<CBounding_Sphere*>(pBounding)->Get_Desc());
        break;
    }

    return isColl;
}

#ifdef _DEBUG

HRESULT CBounding_AABB::Render(PrimitiveBatch<VertexPositionColor>* pBatch, _fvector vColor)
{
    DX::Draw(pBatch, *m_pDesc, vColor);

    return S_OK;
}

_float3 CBounding_AABB::Compute_Min()
{
    return _float3(
        m_pDesc->Center.x - m_pDesc->Extents.x, 
        m_pDesc->Center.y - m_pDesc->Extents.y,
        m_pDesc->Center.z - m_pDesc->Extents.z);
}

_float3 CBounding_AABB::Compute_Max()
{
    return _float3(
        m_pDesc->Center.x + m_pDesc->Extents.x,
        m_pDesc->Center.y + m_pDesc->Extents.y,
        m_pDesc->Center.z + m_pDesc->Extents.z);
}

#endif

_bool CBounding_AABB::Intersect_AABB(CBounding_AABB* pTargetBounding)
{
    _float3     vSourMin = Compute_Min();
    _float3     vSourMax = Compute_Max();
    _float3     vDestMin = pTargetBounding->Compute_Min();
    _float3     vDestMax = pTargetBounding->Compute_Max();

    if (max(vSourMin.x, vDestMin.x) > min(vSourMax.x, vDestMax.x))
        return false;

    if (max(vSourMin.y, vDestMin.y) > min(vSourMax.y, vDestMax.y))
        return false;

    if (max(vSourMin.z, vDestMin.z) > min(vSourMax.z, vDestMax.z))
        return false;

    return true;
}

CBounding_AABB* CBounding_AABB::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CBounding::BOUNDING_DESC* pInitialDesc)
{
    CBounding_AABB* pInstance = new CBounding_AABB(pDevice, pContext);

    if (FAILED(pInstance->Initialize(pInitialDesc)))
    {
        MSG_BOX("Failed to Created : CBounding_AABB");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CBounding_AABB::Free()
{
    __super::Free();

    Safe_Delete(m_pDesc);
    Safe_Delete(m_pOriginalDesc);
}
