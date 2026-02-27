#include "VIBuffer_Rect_Instancing.h"
#include "GameInstance.h"

CVIBuffer_Rect_Instancing::CVIBuffer_Rect_Instancing(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CVIBuffer_Instancing { pDevice, pContext }
{
}

CVIBuffer_Rect_Instancing::CVIBuffer_Rect_Instancing(const CVIBuffer_Rect_Instancing& Prototype)
    : CVIBuffer_Instancing { Prototype }
    , m_pInstanceVertices { Prototype.m_pInstanceVertices }   
    , m_pSpeeds { Prototype.m_pSpeeds }
    , m_isLoop { Prototype.m_isLoop } 
    , m_vPivot{ Prototype.m_vPivot }
{
}

HRESULT CVIBuffer_Rect_Instancing::Initialize_Prototype(const INSTANCE_DESC* pDesc)
{
    const RECT_INSTANCE_DESC* pInstanceDesc = static_cast<const RECT_INSTANCE_DESC*>(pDesc);

    m_iNumVertexBuffers = 2;
    m_iNumVertices = 4;
    m_iVertexStride = sizeof(VTXPOSTEX);

    m_iNumIndices = 6;
    m_iIndexStride = 2;
    m_eIndexFormat = m_iIndexStride == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
    m_ePrimitive = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

    m_iNumInstance = pInstanceDesc->iNumInstance;
    m_iInstanceVertexStride = sizeof(VTXINSTANCEPARTICLE);
    m_iIndexCountPerInstance = 6;

#pragma region VERTEX_BUFFER
    D3D11_BUFFER_DESC           VBDesc{};
    VBDesc.ByteWidth = m_iNumVertices * m_iVertexStride;
    VBDesc.Usage = D3D11_USAGE_DEFAULT;
    VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    VBDesc.CPUAccessFlags = 0;
    VBDesc.MiscFlags = 0;
    VBDesc.StructureByteStride = m_iVertexStride;

    VTXPOSTEX* pVertices = new VTXPOSTEX[m_iNumVertices];
    ZeroMemory(pVertices, sizeof(VTXPOSTEX) * m_iNumVertices);

    pVertices[0].vPosition = _float3(-0.5f, 0.5f, 0.f);
    pVertices[0].vTexcoord = _float2(0.f, 0.f);

    pVertices[1].vPosition = _float3(0.5f, 0.5f, 0.f);
    pVertices[1].vTexcoord = _float2(1.f, 0.f);

    pVertices[2].vPosition = _float3(0.5f, -0.5f, 0.f);
    pVertices[2].vTexcoord = _float2(1.f, 1.f);

    pVertices[3].vPosition = _float3(-0.5f, -0.5f, 0.f);
    pVertices[3].vTexcoord = _float2(0.f, 1.f);

    D3D11_SUBRESOURCE_DATA      VertexInitialData{};
    VertexInitialData.pSysMem = pVertices;    

    if (FAILED(m_pDevice->CreateBuffer(&VBDesc, &VertexInitialData, &m_pVB)))
        return E_FAIL;

    Safe_Delete_Array(pVertices);

#pragma endregion

#pragma region INDEX_BUFFER 
    D3D11_BUFFER_DESC           IBDesc{};
    IBDesc.ByteWidth = m_iNumIndices * m_iIndexStride;
    IBDesc.Usage = D3D11_USAGE_DEFAULT;
    IBDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    IBDesc.CPUAccessFlags = 0;
    IBDesc.MiscFlags = 0;
    IBDesc.StructureByteStride = m_iIndexStride;

    _ushort* pIndices = new _ushort[m_iNumIndices];
    ZeroMemory(pIndices, sizeof(_ushort) * m_iNumIndices);

    pIndices[0] = 0;
    pIndices[1] = 1;
    pIndices[2] = 2;

    pIndices[3] = 0;
    pIndices[4] = 2;
    pIndices[5] = 3;

    D3D11_SUBRESOURCE_DATA      IndexInitialData{};
    IndexInitialData.pSysMem = pIndices;

    if (FAILED(m_pDevice->CreateBuffer(&IBDesc, &IndexInitialData, &m_pIB)))
        return E_FAIL;

    Safe_Delete_Array(pIndices);

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

HRESULT CVIBuffer_Rect_Instancing::Initialize(void* pArg)
{
    D3D11_SUBRESOURCE_DATA      InstanceInitialData{};
    InstanceInitialData.pSysMem = m_pInstanceVertices;

    if (FAILED(m_pDevice->CreateBuffer(&m_InstanceDesc, &InstanceInitialData, &m_pVBInstance)))
        return E_FAIL;

    return S_OK;
}

void CVIBuffer_Rect_Instancing::Drop(_float fTimeDelta)
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

void CVIBuffer_Rect_Instancing::Spread(_float fTimeDelta)
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

CVIBuffer_Rect_Instancing* CVIBuffer_Rect_Instancing::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const INSTANCE_DESC* pDesc)
{
    CVIBuffer_Rect_Instancing* pInstance = new CVIBuffer_Rect_Instancing(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype(pDesc)))
    {
        MSG_BOX("Failed to Created : CVIBuffer_Rect_Instancing");
        Safe_Release(pInstance);
    }

    return pInstance;
}


CComponent* CVIBuffer_Rect_Instancing::Clone(void* pArg)
{
    CVIBuffer_Rect_Instancing* pInstance = new CVIBuffer_Rect_Instancing(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CVIBuffer_Rect_Instancing");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CVIBuffer_Rect_Instancing::Free()
{
    __super::Free();

    if (false == m_isCloned)
    {
        Safe_Delete_Array(m_pInstanceVertices);
        Safe_Delete_Array(m_pSpeeds);
    }

}
