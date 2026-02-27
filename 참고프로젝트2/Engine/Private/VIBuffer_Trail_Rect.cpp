#include "..\Public\VIBuffer_Trail_Rect.h"

CVIBuffer_Trail_Rect::CVIBuffer_Trail_Rect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CVIBuffer{ pDevice, pContext }
{
}

CVIBuffer_Trail_Rect::CVIBuffer_Trail_Rect(const CVIBuffer_Trail_Rect& Prototype)
    : CVIBuffer{ Prototype }
    , m_iNumRect{ Prototype.m_iNumRect}
    , trailVertices{ Prototype.trailVertices }
{
}

HRESULT CVIBuffer_Trail_Rect::Initialize_Prototype()
{
    m_iNumVertexBuffers = 1;
    m_iNumVertices = 8;
    m_iVertexStride = sizeof(VTXPOSTEX);

    m_iNumIndices = 12;
    m_iIndexStride = sizeof(_ushort);
    m_eIndexFormat = DXGI_FORMAT_R16_UINT;
    m_ePrimitive_Topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

#pragma region VERTEX_BUFFER
    m_BufferDesc.ByteWidth = m_iNumVertices * m_iVertexStride;
    m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC; // 동적 할당
    m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; // CPU 접근 가능
    m_BufferDesc.MiscFlags = 0;
    m_BufferDesc.StructureByteStride = m_iVertexStride;
   

    VTXPOSTEX* pVertices = new VTXPOSTEX[m_iNumVertices];
    ZeroMemory(pVertices, sizeof(VTXPOSTEX) * m_iNumVertices);

    // 초기 정점 위치는 임의로 설정 (나중에 업데이트 예정)
    pVertices[0].vPosition = _float3(-0.5f, 0.5f, 0.f);
    pVertices[0].vTexcoord = _float2(0.0f, 0.f);

    pVertices[1].vPosition = _float3(0.5f, 0.5f, 0.f);
    pVertices[1].vTexcoord = _float2(1.0f, 0.f);

    pVertices[2].vPosition = _float3(0.5f, -0.5f, 0.f);
    pVertices[2].vTexcoord = _float2(1.0f, 1.0f);

    pVertices[3].vPosition = _float3(-0.5f, -0.5f, 0.f);
    pVertices[3].vTexcoord = _float2(0.0f, 1.0f);

    ZeroMemory(&m_InitialData, sizeof(m_InitialData));
    m_InitialData.pSysMem = pVertices;

    if (FAILED(__super::Create_Buffer(&m_pVB)))
        return E_FAIL;

    Safe_Delete_Array(pVertices);
#pragma endregion

#pragma region INDEX_BUFFER
    m_BufferDesc.ByteWidth = m_iNumIndices * m_iIndexStride;
    m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
    m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    m_BufferDesc.CPUAccessFlags = 0;
    m_BufferDesc.MiscFlags = 0;
    m_BufferDesc.StructureByteStride = 0;

    _ushort* pIndices = new _ushort[m_iNumIndices];
    ZeroMemory(pIndices, sizeof(_ushort) * m_iNumIndices);

    pIndices[0] = 0;
    pIndices[1] = 1;
    pIndices[2] = 2;

    pIndices[3] = 0;
    pIndices[4] = 2;
    pIndices[5] = 3;

    ZeroMemory(&m_InitialData, sizeof(m_InitialData));
    m_InitialData.pSysMem = pIndices;

    if (FAILED(__super::Create_Buffer(&m_pIB)))
        return E_FAIL;

    Safe_Delete_Array(pIndices);
#pragma endregion

    return S_OK;
}

HRESULT CVIBuffer_Trail_Rect::Initialize_Prototype_Trail(_uint iNumRect, _float fRectWidth)
{
    m_iNumRect = iNumRect;
    trailVertices.resize(m_iNumRect * 4);

    m_iNumVertexBuffers = 1;
    m_iNumVertices = 4 * iNumRect; // 렉트 50개이므로, 각 렉트에 4개의 정점 필요
    m_iVertexStride = sizeof(VTXPOSTEX);

    m_iNumIndices = 6 * iNumRect;  // 렉트 50개이므로, 각 렉트에 6개의 인덱스 필요
    m_iIndexStride = sizeof(_ushort);
    m_eIndexFormat = DXGI_FORMAT_R16_UINT;
    m_ePrimitive_Topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

#pragma region VERTEX_BUFFER
    m_BufferDesc.ByteWidth = m_iNumVertices * m_iVertexStride;
    m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC; // 동적 할당
    m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; // CPU 접근 가능
    m_BufferDesc.MiscFlags = 0;
    m_BufferDesc.StructureByteStride = m_iVertexStride;


    VTXPOSTEX* pVertices = new VTXPOSTEX[m_iNumVertices];
    ZeroMemory(pVertices, sizeof(VTXPOSTEX) * m_iNumVertices);

    float rectWidth = fRectWidth; // 각 렉트의 너비 (원하는 크기로 조정)

    // 초기 정점 위치는 임의로 설정 (나중에 업데이트 예정)
    for (int i = 0; i < m_iNumRect; ++i) {
        
                _float fRatio = ((_float)(i + 1) / (_float)m_iNumRect);
                _float fScaleOffset = 1.f - fRatio;
                _float offset = fRatio * 5.f; // 각 렉트를 오른쪽으로 오프셋
        
                trailVertices[i * 4 + 0].vPosition = _float3((-0.5f * fScaleOffset) + offset, (0.5f * fScaleOffset), 0.f);
                trailVertices[i * 4 + 0].vTexcoord = _float2(0.0f, 0.f);

                trailVertices[i * 4 + 1].vPosition = _float3((0.5f * fScaleOffset) + offset, 0.5f * fScaleOffset, 0.f);
                trailVertices[i * 4 + 1].vTexcoord = _float2(1.0f, 0.f);
        
                trailVertices[i * 4 + 2].vPosition = _float3((0.5f * fScaleOffset) + offset, -0.5f * fScaleOffset, 0.f);
                trailVertices[i * 4 + 2].vTexcoord = _float2(1.0f, 1.0f);

                trailVertices[i * 4 + 3].vPosition = _float3((-0.5f * fScaleOffset) + offset, -0.5f * fScaleOffset, 0.f);
                trailVertices[i * 4 + 3].vTexcoord = _float2(0.0f, 1.0f);
            }

    ZeroMemory(&m_InitialData, sizeof(m_InitialData));
    m_InitialData.pSysMem = pVertices;

    if (FAILED(__super::Create_Buffer(&m_pVB)))
        return E_FAIL;

    Safe_Delete_Array(pVertices);
#pragma endregion

#pragma region INDEX_BUFFER
    m_BufferDesc.ByteWidth = m_iNumIndices * m_iIndexStride;
    m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
    m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    m_BufferDesc.CPUAccessFlags = 0;
    m_BufferDesc.MiscFlags = 0;
    m_BufferDesc.StructureByteStride = 0;
    
    _ushort* pIndices = new _ushort[m_iNumIndices];
    ZeroMemory(pIndices, sizeof(_ushort) * m_iNumIndices);

    // 각 렉트에 대해 인덱스 설정
    for (int i = 0; i < iNumRect; ++i) {
        pIndices[i * 6 + 0] = i * 4 + 0;
        pIndices[i * 6 + 1] = i * 4 + 1;
        pIndices[i * 6 + 2] = i * 4 + 2;

        pIndices[i * 6 + 3] = i * 4 + 0;
        pIndices[i * 6 + 4] = i * 4 + 2;
        pIndices[i * 6 + 5] = i * 4 + 3;
    }
    
    ZeroMemory(&m_InitialData, sizeof(m_InitialData));
    m_InitialData.pSysMem = pIndices;

    if (FAILED(__super::Create_Buffer(&m_pIB)))
        return E_FAIL;

    Safe_Delete_Array(pIndices);
#pragma endregion

    return S_OK;
}


HRESULT CVIBuffer_Trail_Rect::Initialize(void* pArg)
{
    return S_OK;
}

void CVIBuffer_Trail_Rect::Line(_float3 vOffPos)
{
    trailVertices[0].vPosition = _float3(-0.5f + vOffPos.x, 0.5f + vOffPos.y, 0.f);
    trailVertices[0].vTexcoord = _float2(0.0f, 0.f);

    trailVertices[1].vPosition = _float3(0.5f + vOffPos.x, 0.5f + vOffPos.y, 0.f);
    trailVertices[1].vTexcoord = _float2(1.0f, 0.f);

    trailVertices[2].vPosition = _float3(0.5f + vOffPos.x, -0.5f + vOffPos.y, 0.f);
    trailVertices[2].vTexcoord = _float2(1.0f, 1.0f);

    trailVertices[3].vPosition = _float3(-0.5f + vOffPos.x, -0.5f + vOffPos.y , 0.f);
    trailVertices[3].vTexcoord = _float2(0.0f, 1.0f);

   //for (int i = trailVertices.size() - 1; i > 3; --i) {
   //    trailVertices[i] = trailVertices[i - 4];
   //}

    for (int i = 0 ; i < trailVertices.size() - 4; ++i) {
        trailVertices[i + 4] = trailVertices[i];
    }

    D3D11_MAPPED_SUBRESOURCE mappedResource;
    HRESULT hr = m_pContext->Map(m_pVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

    if (SUCCEEDED(hr)) {
        VTXPOSTEX* pVertexData = reinterpret_cast<VTXPOSTEX*>(mappedResource.pData);
        memcpy(pVertexData, trailVertices.data(), trailVertices.size() * sizeof(VTXPOSTEX));
        m_pContext->Unmap(m_pVB, 0);
    }
    
}

CVIBuffer_Trail_Rect* CVIBuffer_Trail_Rect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CVIBuffer_Trail_Rect* pInstance = new CVIBuffer_Trail_Rect(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CVIBuffer_Trail_Rect"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CVIBuffer_Trail_Rect* CVIBuffer_Trail_Rect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext , _uint iNummRect , _float fRectWidth)
{
    CVIBuffer_Trail_Rect* pInstance = new CVIBuffer_Trail_Rect(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype_Trail(iNummRect , fRectWidth)))
    {
        MSG_BOX(TEXT("Failed to Created : CVIBuffer_Trail_Rect"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CComponent* CVIBuffer_Trail_Rect::Clone(void* pArg)
{
    CVIBuffer_Trail_Rect* pInstance = new CVIBuffer_Trail_Rect(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CVIBuffer_Trail_Rect"));
        Safe_Release(pInstance);
    }

    return pInstance;
}



void CVIBuffer_Trail_Rect::Free()
{
    __super::Free();


}
