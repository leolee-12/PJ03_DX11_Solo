#include "VIBuffer_XZPlane.h"

CVIBuffer_XZPlane::CVIBuffer_XZPlane(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer{ pDevice, pContext }
{
}

CVIBuffer_XZPlane::CVIBuffer_XZPlane(const CVIBuffer_XZPlane& Prototype)
	: CVIBuffer{ Prototype }
{
}

HRESULT CVIBuffer_XZPlane::Initialize_Prototype()
{
	m_iNumVertexBuffers = 1;
	m_iNumVertices = 4;
	m_iVertexStride = sizeof(VTXMESH);

	m_iNumIndices = 6;
	m_iIndexStride = 2;
	m_eIndexFormat = DXGI_FORMAT_R16_UINT;
	m_ePrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	return S_OK;
}

HRESULT CVIBuffer_XZPlane::Initialize(void* pArg)
{
	XZ_PLANE_DESC tDesc{};
	if (nullptr != pArg)
		tDesc = *static_cast<XZ_PLANE_DESC*>(pArg);

	const _float fWidth = tDesc.fWidth <= 0.f ? 1.f : tDesc.fWidth;
	const _float fDepth = tDesc.fDepth <= 0.f ? 1.f : tDesc.fDepth;
	const _float fTileU = tDesc.fTileU <= 0.f ? 1.f : tDesc.fTileU;
	const _float fTileV = tDesc.fTileV <= 0.f ? 1.f : tDesc.fTileV;

	const _float fHalfW = fWidth * 0.5f;
	const _float fHalfD = fDepth * 0.5f;

	D3D11_BUFFER_DESC VertexBufferDesc{};
	VertexBufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	VertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	VertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	VTXMESH* pVertices = new VTXMESH[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXMESH) * m_iNumVertices);

	pVertices[0].vPosition = _float3(-fHalfW, 0.f, fHalfD);
	pVertices[1].vPosition = _float3(fHalfW, 0.f, fHalfD);
	pVertices[2].vPosition = _float3(fHalfW, 0.f, -fHalfD);
	pVertices[3].vPosition = _float3(-fHalfW, 0.f, -fHalfD);

	pVertices[0].vTexcoord = _float2(0.f, 0.f);
	pVertices[1].vTexcoord = _float2(fTileU, 0.f);
	pVertices[2].vTexcoord = _float2(fTileU, fTileV);
	pVertices[3].vTexcoord = _float2(0.f, fTileV);

	for (_uint i = 0; i < m_iNumVertices; ++i)
	{
		pVertices[i].vNormal = _float3(0.f, 1.f, 0.f);
		pVertices[i].vTangent = _float3(1.f, 0.f, 0.f);
		pVertices[i].vBinormal = _float3(0.f, 0.f, -1.f);
	}

	D3D11_SUBRESOURCE_DATA VertexInitialData{};
	VertexInitialData.pSysMem = pVertices;

	if (FAILED(m_pDevice->CreateBuffer(&VertexBufferDesc, &VertexInitialData, &m_pVB)))
	{
		Safe_Delete_Array(pVertices);
		return E_FAIL;
	}

	Safe_Delete_Array(pVertices);

	D3D11_BUFFER_DESC IndexBufferDesc{};
	IndexBufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
	IndexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	IndexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

	_ushort* pIndices = new _ushort[m_iNumIndices];
	pIndices[0] = 0; pIndices[1] = 1; pIndices[2] = 2;
	pIndices[3] = 0; pIndices[4] = 2; pIndices[5] = 3;

	D3D11_SUBRESOURCE_DATA IndexInitialData{};
	IndexInitialData.pSysMem = pIndices;

	if (FAILED(m_pDevice->CreateBuffer(&IndexBufferDesc, &IndexInitialData, &m_pIB)))
	{
		Safe_Delete_Array(pIndices);
		return E_FAIL;
	}

	Safe_Delete_Array(pIndices);
	return S_OK;
}

CVIBuffer_XZPlane* CVIBuffer_XZPlane::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CVIBuffer_XZPlane* pInstance = new CVIBuffer_XZPlane(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CVIBuffer_XZPlane");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CVIBuffer_XZPlane::Clone(void* pArg)
{
	CVIBuffer_XZPlane* pInstance = new CVIBuffer_XZPlane(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CVIBuffer_XZPlane");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_XZPlane::Free()
{
	__super::Free();
}