#include "Bounding_AABB.h"
#include "Bounding_OBB.h"
#include "Bounding_Sphere.h"

CBounding_Sphere::CBounding_Sphere(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CBounding{ pDevice, pContext }
{
}

HRESULT CBounding_Sphere::Initialize(CBounding::BOUNDING_DESC* pInitialDesc)
{
    SPHERE_DESC*  pDesc = static_cast<CBounding_Sphere::SPHERE_DESC*>(pInitialDesc);

    m_pOriginalDesc = new BoundingSphere(pDesc->vCenter, pDesc->fRadius);
    m_pDesc = new BoundingSphere(*m_pOriginalDesc);

    return S_OK;
}

void CBounding_Sphere::Update(_fmatrix WorldMatrix)
{
  
    m_pOriginalDesc->Transform(*m_pDesc, WorldMatrix);
}

_bool CBounding_Sphere::Intersect(COLLIDER eType, CBounding* pBounding)
{
    _bool       isColl = { false };
    switch (eType)
    {
    case COLLIDER::AABB:
        isColl = m_pDesc->Intersects(*static_cast<CBounding_AABB*>(pBounding)->Get_Desc());
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

HRESULT CBounding_Sphere::Render(PrimitiveBatch<VertexPositionColor>* pBatch, _fvector vColor)
{
    DX::Draw(pBatch, *m_pDesc, vColor);

    return S_OK;
}

#endif

CBounding_Sphere* CBounding_Sphere::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CBounding::BOUNDING_DESC* pInitialDesc)
{
    CBounding_Sphere* pInstance = new CBounding_Sphere(pDevice, pContext);

    if (FAILED(pInstance->Initialize(pInitialDesc)))
    {
        MSG_BOX("Failed to Created : CBounding_Sphere");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CBounding_Sphere::Free()
{
    __super::Free();

    Safe_Delete(m_pDesc);
    Safe_Delete(m_pOriginalDesc);
}
