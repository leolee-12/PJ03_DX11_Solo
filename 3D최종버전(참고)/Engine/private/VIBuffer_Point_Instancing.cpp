#include "VIBuffer_Point_Instancing.h"
#include "GameInstance.h"

CVIBuffer_Point_Instancing::CVIBuffer_Point_Instancing(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CVIBuffer_Instancing { pDevice, pContext }
{
}

CVIBuffer_Point_Instancing::CVIBuffer_Point_Instancing(const CVIBuffer_Point_Instancing& Prototype)
    : CVIBuffer_Instancing { Prototype }
    , m_pInstanceVertices { Prototype.m_pInstanceVertices }   
    , m_pSpeeds { Prototype.m_pSpeeds }
    , m_isLoop { Prototype.m_isLoop } 
    , m_vPivot{ Prototype.m_vPivot }
{
}

HRESULT CVIBuffer_Point_Instancing::Initialize_Prototype(const INSTANCE_DESC* pDesc)
{
    const POINT_INSTANCE_DESC* pInstanceDesc = static_cast<const POINT_INSTANCE_DESC*>(pDesc);

    m_iNumVertexBuffers = 2;
    m_iNumVertices = 1;
    m_iVertexStride = sizeof(VTXPOS);

    m_iNumIndices = 0;
    m_iIndexStride = 0;
    m_eIndexFormat = DXGI_FORMAT_UNKNOWN;
    m_ePrimitive = D3D_PRIMITIVE_TOPOLOGY_POINTLIST;

    m_iNumInstance = pInstanceDesc->iNumInstance;
    m_iInstanceVertexStride = sizeof(VTXINSTANCEPARTICLE);
    m_iIndexCountPerInstance = 0;

#pragma region VERTEX_BUFFER
    D3D11_BUFFER_DESC           VBDesc{};
    VBDesc.ByteWidth = m_iNumVertices * m_iVertexStride;
    VBDesc.Usage = D3D11_USAGE_DEFAULT;
    VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    VBDesc.CPUAccessFlags = 0;
    VBDesc.MiscFlags = 0;
    VBDesc.StructureByteStride = m_iVertexStride;

    VTXPOS* pVertices = new VTXPOS[m_iNumVertices];
    ZeroMemory(pVertices, sizeof(VTXPOS) * m_iNumVertices);

    pVertices->vPosition = _float3(0.0f, 0.0f, 0.f);    
    
    D3D11_SUBRESOURCE_DATA      VertexInitialData{};
    VertexInitialData.pSysMem = pVertices;    

    if (FAILED(m_pDevice->CreateBuffer(&VBDesc, &VertexInitialData, &m_pVB)))
        return E_FAIL;

    Safe_Delete_Array(pVertices);

#pragma endregion

#pragma region INDEX_BUFFER 

#pragma endregion

#pragma region INSTANCE_BUFFER
    
    m_InstanceDesc.ByteWidth = m_iNumInstance * m_iInstanceVertexStride;
    m_InstanceDesc.Usage = D3D11_USAGE_DYNAMIC;
    m_InstanceDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    m_InstanceDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    m_InstanceDesc.MiscFlags = 0;
    m_InstanceDesc.StructureByteStride = m_iInstanceVertexStride;

    m_pInstanceVertices = new VTXINSTANCEPARTICLE[m_iNumInstance];
    ZeroMemory(m_pInstanceVertices, sizeof(VTXINSTANCEPARTICLE) * m_iNumInstance);
    m_pSpeeds = new _float[m_iNumInstance];

    for (size_t i = 0; i < m_iNumInstance; i++)
    {
        _float      fScale = m_pGameInstance->Random(pInstanceDesc->vScale.x, pInstanceDesc->vScale.y);

        m_pInstanceVertices[i].vRight = _float4(fScale, 0.f, 0.f, 0.f);
        m_pInstanceVertices[i].vUp = _float4(0.f, fScale, 0.f, 0.f);
        m_pInstanceVertices[i].vLook = _float4(0.f, 0.f, fScale, 0.f);
        m_pInstanceVertices[i].vTranslation = _float4(
            m_pGameInstance->Random(pInstanceDesc->vCenter.x - pInstanceDesc->vRange.x * 0.5f, pInstanceDesc->vCenter.x + pInstanceDesc->vRange.x * 0.5f),
            m_pGameInstance->Random(pInstanceDesc->vCenter.y - pInstanceDesc->vRange.y * 0.5f, pInstanceDesc->vCenter.y + pInstanceDesc->vRange.y * 0.5f),
            m_pGameInstance->Random(pInstanceDesc->vCenter.z - pInstanceDesc->vRange.z * 0.5f, pInstanceDesc->vCenter.z + pInstanceDesc->vRange.z * 0.5f),
            1.f
        );
        m_pSpeeds[i] = m_pGameInstance->Random(pInstanceDesc->vSpeed.x, pInstanceDesc->vSpeed.y);
        m_pInstanceVertices[i].vLifeTime = _float2(
            m_pGameInstance->Random(pInstanceDesc->vLifeTime.x, pInstanceDesc->vLifeTime.y),
            0.0f
        );        
    }

    m_vPivot = pInstanceDesc->vPivot;
    m_isLoop = pInstanceDesc->isLoop;
 
#pragma endregion


    return S_OK;
}

HRESULT CVIBuffer_Point_Instancing::Initialize(void* pArg)
{
    D3D11_SUBRESOURCE_DATA      InstanceInitialData{};
    InstanceInitialData.pSysMem = m_pInstanceVertices;

    if (FAILED(m_pDevice->CreateBuffer(&m_InstanceDesc, &InstanceInitialData, &m_pVBInstance)))
        return E_FAIL;

    return S_OK;
}

HRESULT CVIBuffer_Point_Instancing::Bind_Buffers()
{
    ID3D11Buffer* pVertexBuffers[] = {
        m_pVB,
        m_pVBInstance,
    };

    _uint			iVertexStrides[] = {
        m_iVertexStride,
        m_iInstanceVertexStride,

    };

    _uint			iOffsets[] = {
        0,
        0
    };

    m_pContext->IASetVertexBuffers(0, m_iNumVertexBuffers, pVertexBuffers, iVertexStrides, iOffsets);    
    m_pContext->IASetPrimitiveTopology(m_ePrimitive);

    return S_OK;
}

HRESULT CVIBuffer_Point_Instancing::Render()
{
    m_pContext->DrawInstanced(1, m_iNumInstance, 0, 0);

    return S_OK;
}

void CVIBuffer_Point_Instancing::Drop(_float fTimeDelta)
{
    D3D11_MAPPED_SUBRESOURCE        SubResource{};
    m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

    VTXINSTANCEPARTICLE* pVertices = static_cast<VTXINSTANCEPARTICLE*>(SubResource.pData);

    for (size_t i = 0; i < m_iNumInstance; i++)
    {
        pVertices[i].vTranslation.y -= m_pSpeeds[i] * fTimeDelta;
        pVertices[i].vLifeTime.y += fTimeDelta;

        if (true == m_isLoop && pVertices[i].vLifeTime.y >= pVertices[i].vLifeTime.x)
        {
            pVertices[i].vLifeTime.y = 0.f;
            pVertices[i].vTranslation = m_pInstanceVertices[i].vTranslation;
        }
    }

    m_pContext->Unmap(m_pVBInstance, 0);
    
}

void CVIBuffer_Point_Instancing::Spread(_float fTimeDelta)
{
    D3D11_MAPPED_SUBRESOURCE        SubResource{};
    m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

    VTXINSTANCEPARTICLE* pVertices = static_cast<VTXINSTANCEPARTICLE*>(SubResource.pData);

    for (size_t i = 0; i < m_iNumInstance; i++)
    {
        _vector     vMoveDir = XMVectorSetW(XMLoadFloat4(&pVertices[i].vTranslation) - XMLoadFloat3(&m_vPivot), 0.f);

        XMStoreFloat4(&pVertices[i].vTranslation,
            XMLoadFloat4(&pVertices[i].vTranslation) + XMVector3Normalize(vMoveDir) * m_pSpeeds[i] * fTimeDelta);

        pVertices[i].vLifeTime.y += fTimeDelta;

        if (true == m_isLoop && pVertices[i].vLifeTime.y >= pVertices[i].vLifeTime.x)
        {
            pVertices[i].vLifeTime.y = 0.f;
            pVertices[i].vTranslation = m_pInstanceVertices[i].vTranslation;
        }
    }

    m_pContext->Unmap(m_pVBInstance, 0);
}

CVIBuffer_Point_Instancing* CVIBuffer_Point_Instancing::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const INSTANCE_DESC* pDesc)
{
    CVIBuffer_Point_Instancing* pInstance = new CVIBuffer_Point_Instancing(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype(pDesc)))
    {
        MSG_BOX("Failed to Created : CVIBuffer_Point_Instancing");
        Safe_Release(pInstance);
    }

    return pInstance;
}


CComponent* CVIBuffer_Point_Instancing::Clone(void* pArg)
{
    CVIBuffer_Point_Instancing* pInstance = new CVIBuffer_Point_Instancing(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CVIBuffer_Point_Instancing");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CVIBuffer_Point_Instancing::Free()
{
    __super::Free();

    if (false == m_isCloned)
    {
        Safe_Delete_Array(m_pInstanceVertices);
        Safe_Delete_Array(m_pSpeeds);
    }

}
