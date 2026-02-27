#include "Cell.h"
#include "VIBuffer_Cell.h"

CCell::CCell(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : m_pDevice { pDevice }
    , m_pContext { pContext }
{
    Safe_AddRef(m_pContext);
    Safe_AddRef(m_pDevice);
}

HRESULT CCell::Initialize(const _float3* pPoints, _uint iIndex)
{
    memcpy(m_vPoints, pPoints, sizeof(_float3) * ENUM_CLASS(POINT::END));

    m_iIndex = iIndex;

    m_vNormals[ENUM_CLASS(LINE::AB)] = _float3((m_vPoints[ENUM_CLASS(POINT::B)].z - m_vPoints[ENUM_CLASS(POINT::A)].z) * -1.f, 0.f, m_vPoints[ENUM_CLASS(POINT::B)].x - m_vPoints[ENUM_CLASS(POINT::A)].x);
    m_vNormals[ENUM_CLASS(LINE::BC)] = _float3((m_vPoints[ENUM_CLASS(POINT::C)].z - m_vPoints[ENUM_CLASS(POINT::B)].z) * -1.f, 0.f, m_vPoints[ENUM_CLASS(POINT::C)].x - m_vPoints[ENUM_CLASS(POINT::B)].x);
    m_vNormals[ENUM_CLASS(LINE::CA)] = _float3((m_vPoints[ENUM_CLASS(POINT::A)].z - m_vPoints[ENUM_CLASS(POINT::C)].z) * -1.f, 0.f, m_vPoints[ENUM_CLASS(POINT::A)].x - m_vPoints[ENUM_CLASS(POINT::C)].x);

#ifdef _DEBUG
    m_pDebugBuffer = CVIBuffer_Cell::Create(m_pDevice, m_pContext, m_vPoints);
    if (nullptr == m_pDebugBuffer)
        return E_FAIL;
#endif

    return S_OK;
}

_bool CCell::isIn(_fvector vResultPos, _int* pNeighborIndex)
{
    for (size_t i = 0; i < ENUM_CLASS(LINE::END); i++)
    {
        _vector vNormal = XMVector3Normalize(XMLoadFloat3(&m_vNormals[i]));
        _vector vDir = XMVector3Normalize(vResultPos - XMLoadFloat3(&m_vPoints[i]));

        if (0 < XMVectorGetX(XMVector3Dot(vNormal, vDir)))
        {
            *pNeighborIndex = m_iNeighborIndices[i];
            return false;
        }            
    }
    return true;
}

_bool CCell::Compare_Points(_fvector vSour, _fvector vDest)
{    
    if (true == XMVector3Equal(vSour, XMLoadFloat3(&m_vPoints[ENUM_CLASS(POINT::A)])))
    {
        if (true == XMVector3Equal(vDest, XMLoadFloat3(&m_vPoints[ENUM_CLASS(POINT::B)])))
            return true;
        if (true == XMVector3Equal(vDest, XMLoadFloat3(&m_vPoints[ENUM_CLASS(POINT::C)])))
            return true;
    }

    if (true == XMVector3Equal(vSour, XMLoadFloat3(&m_vPoints[ENUM_CLASS(POINT::B)])))
    {
        if (true == XMVector3Equal(vDest, XMLoadFloat3(&m_vPoints[ENUM_CLASS(POINT::C)])))
            return true;
        if (true == XMVector3Equal(vDest, XMLoadFloat3(&m_vPoints[ENUM_CLASS(POINT::A)])))
            return true;
    }

    if (true == XMVector3Equal(vSour, XMLoadFloat3(&m_vPoints[ENUM_CLASS(POINT::C)])))
    {
        if (true == XMVector3Equal(vDest, XMLoadFloat3(&m_vPoints[ENUM_CLASS(POINT::A)])))
            return true;
        if (true == XMVector3Equal(vDest, XMLoadFloat3(&m_vPoints[ENUM_CLASS(POINT::B)])))
            return true;
    }

    return false;
}

_float CCell::Compute_Height(_fvector vResultPos)
{
    _float4 vPlane = {};
    
    XMStoreFloat4(&vPlane, XMPlaneFromPoints(
        XMLoadFloat3(&m_vPoints[ENUM_CLASS(POINT::A)]),
        XMLoadFloat3(&m_vPoints[ENUM_CLASS(POINT::B)]),
        XMLoadFloat3(&m_vPoints[ENUM_CLASS(POINT::C)])
        ));

    /*ax + by + cz + d = 0;

    y = (-ax - cz - d) / b;*/

    float   fy = (-vPlane.x * XMVectorGetX(vResultPos) - vPlane.z * XMVectorGetZ(vResultPos) - vPlane.w) / vPlane.y;

    return fy;
}

#ifdef _DEBUG
HRESULT CCell::Render()
{
    m_pDebugBuffer->Bind_Buffers();
    m_pDebugBuffer->Render();

    return S_OK;
}
#endif

CCell* CCell::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _float3* pPoints, _uint iIndex)
{
    CCell* pInstance = new CCell(pDevice, pContext);

    if (FAILED(pInstance->Initialize(pPoints, iIndex)))
    {
        MSG_BOX("Failed to Created : CCell");
        Safe_Release(pInstance);
    }

    return pInstance;
}



void CCell::Free()
{
    __super::Free();

#ifdef _DEBUG
    Safe_Release(m_pDebugBuffer);

#endif

    Safe_Release(m_pContext);
    Safe_Release(m_pDevice);
}
