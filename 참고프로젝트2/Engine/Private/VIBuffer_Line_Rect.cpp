#include "..\Public\VIBuffer_Line_Rect.h"

CVIBuffer_Line_Rect::CVIBuffer_Line_Rect(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CVIBuffer { pDevice, pContext }
{
}

CVIBuffer_Line_Rect::CVIBuffer_Line_Rect(const CVIBuffer_Line_Rect & Prototype)
	: CVIBuffer{ Prototype }
{
}

HRESULT CVIBuffer_Line_Rect::Initialize_Prototype()
{
    m_iNumVertexBuffers = 1;
    m_iNumVertices = 4;
    m_iVertexStride = sizeof(VTXPOSTEX);

    m_iNumIndices = 6;
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

HRESULT CVIBuffer_Line_Rect::Initialize_Prototype_Half()
{
	m_iNumVertexBuffers = 1;
	m_iNumVertices = 4;
	m_iVertexStride = sizeof(VTXPOSTEX);

	m_iNumIndices = 6;
	m_iIndexStride = 2;
	m_eIndexFormat = DXGI_FORMAT_R16_UINT;
	m_ePrimitive_Topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

#pragma region VERTEX_BUFFER
	m_BufferDesc.ByteWidth = m_iNumVertices * m_iVertexStride;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT; /* 정적할당한다. 추후 우리가 직접 정점버퍼의 공간을 변경할 일이 없다. */
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iVertexStride;

	/* 장치가 할당해주는 공간에 채워넣어야할 값들을 내가 미리 준비한다. */
	VTXPOSTEX* pVertices = new VTXPOSTEX[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXPOSTEX) * m_iNumVertices);

	pVertices[0].vPosition = _float3(-0.25f, 0.25f, 0.f);
	pVertices[0].vTexcoord = _float2(0.0f, 0.f);

	pVertices[1].vPosition = _float3(0.25f, 0.25f, 0.f);
	pVertices[1].vTexcoord = _float2(1.0f, 0.f);

	pVertices[2].vPosition = _float3(0.25f, -0.25f, 0.f);
	pVertices[2].vTexcoord = _float2(1.0f, 1.0f);

	pVertices[3].vPosition = _float3(-0.25f, -0.25f, 0.f);
	pVertices[3].vTexcoord = _float2(0.0f, 1.0f);

	ZeroMemory(&m_InitialData, sizeof m_InitialData);
	m_InitialData.pSysMem = pVertices;

	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

#pragma endregion

#pragma region INDEX_BUFFER
	m_BufferDesc.ByteWidth = m_iNumIndices * m_iIndexStride;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT; /* 정적할당한다. 추후 우리가 직접 정점버퍼의 공간을 변경할 일이 없다. */
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = /*m_iIndexStride*/0;

	_ushort* pIndices = new _ushort[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_ushort) * m_iNumIndices);

	pIndices[0] = 0;
	pIndices[1] = 1;
	pIndices[2] = 2;

	pIndices[3] = 0;
	pIndices[4] = 2;
	pIndices[5] = 3;

	ZeroMemory(&m_InitialData, sizeof m_InitialData);
	m_InitialData.pSysMem = pIndices;

	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;

	Safe_Delete_Array(pIndices);

#pragma endregion

	return S_OK;
}


HRESULT CVIBuffer_Line_Rect::Initialize(void * pArg)
{
	return S_OK;
}

void CVIBuffer_Line_Rect::CalculateQuad(const _float3& start, const _float3& end, _float thickness, const _float3& cameraPos)
{
    // XMFLOAT3를 _vector로 로드
    _vector startVec = XMLoadFloat3(&start);
    _vector endVec = XMLoadFloat3(&end);
    _vector cameraVec = XMLoadFloat3(&cameraPos);

    // 선의 방향 벡터 계산
    _vector direction = XMVectorSubtract(endVec, startVec);

    // 방향 벡터의 길이 계산
    _vector lengthVec = XMVector3Length(direction);
    float length;
    XMStoreFloat(&length, lengthVec);

    if (length == 0.0f)
        return; // 시작점과 끝점이 동일한 경우 처리하지 않음

    // 방향 벡터 정규화
    _vector directionNorm = XMVectorDivide(direction, lengthVec);

    // 선의 중점 계산
    _vector midpoint = XMVectorAdd(startVec, direction);
    midpoint = XMVectorScale(midpoint, 0.5f);

    // 중점에서 카메라로 향하는 벡터 계산
    _vector toCamera = XMVectorSubtract(cameraVec, midpoint);

    // 카메라 방향 벡터 정규화
    _vector toCameraNorm = XMVector3Normalize(toCamera);

    // 노멀 벡터 계산 (선 방향과 카메라 방향의 교차 곱)
    _vector normal = XMVector3Cross(directionNorm, toCameraNorm);

    // 노멀 벡터 정규화
    normal = XMVector3Normalize(normal);

    // 노멀 벡터에 두께의 반을 곱함
    _vector halfThickness = XMVectorReplicate(thickness / 2.0f);
    normal = XMVectorMultiply(normal, halfThickness);

    // 노멀 벡터를 XMFLOAT3로 저장
    XMFLOAT3 normalFloat3;
    XMStoreFloat3(&normalFloat3, normal);

    // 사각형의 네 정점 계산
    VTXPOSTEX vertices[4];
    vertices[0].vPosition = _float3(
        start.x + normalFloat3.x,
        start.y + normalFloat3.y,
        start.z + normalFloat3.z
    );
    vertices[1].vPosition = _float3(
        end.x + normalFloat3.x,
        end.y + normalFloat3.y,
        end.z + normalFloat3.z
    );
    vertices[2].vPosition = _float3(
        end.x - normalFloat3.x,
        end.y - normalFloat3.y,
        end.z - normalFloat3.z
    );
    vertices[3].vPosition = _float3(
        start.x - normalFloat3.x,
        start.y - normalFloat3.y,
        start.z - normalFloat3.z
    );

    // 텍스처 좌표 설정 (필요에 따라 조정 가능)
    vertices[0].vTexcoord = _float2(0.0f, 0.0f);
    vertices[1].vTexcoord = _float2(1.0f, 0.0f);
    vertices[2].vTexcoord = _float2(1.0f, 1.0f);
    vertices[3].vTexcoord = _float2(0.0f, 1.0f);

    // 정점 버퍼 매핑 및 업데이트
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

    HRESULT hr = m_pContext->Map(m_pVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    if (FAILED(hr))
    {
        // 매핑 실패 시 처리 (예: 로그 출력)
        return;
    }

    // 정점 데이터 복사
    VTXPOSTEX* pVertexData = reinterpret_cast<VTXPOSTEX*>(mappedResource.pData);
    memcpy(pVertexData, vertices, sizeof(vertices));

    // 매핑 해제
    m_pContext->Unmap(m_pVB, 0);
}

CVIBuffer_Line_Rect * CVIBuffer_Line_Rect::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CVIBuffer_Line_Rect*		pInstance = new CVIBuffer_Line_Rect(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CVIBuffer_Line_Rect"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CVIBuffer_Line_Rect* CVIBuffer_Line_Rect::Create_Half(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CVIBuffer_Line_Rect* pInstance = new CVIBuffer_Line_Rect(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype_Half()))
	{
		MSG_BOX(TEXT("Failed to Created : CVIBuffer_Line_Rect"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CVIBuffer_Line_Rect::Clone(void * pArg)
{
	CVIBuffer_Line_Rect*		pInstance = new CVIBuffer_Line_Rect(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CVIBuffer_Line_Rect"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CVIBuffer_Line_Rect::Free()
{
	__super::Free();


}
