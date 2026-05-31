#include "VIBuffer_Trail.h"

CVIBuffer_Trail::CVIBuffer_Trail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer{ pDevice, pContext } {
}

CVIBuffer_Trail::CVIBuffer_Trail(const CVIBuffer_Trail& Prototype)
	: CVIBuffer{ Prototype }, m_VBDesc{ Prototype.m_VBDesc } {
}

HRESULT CVIBuffer_Trail::Initialize_Prototype()
{
	m_iNumVertexBuffers = 1;
	m_iVertexStride = sizeof(VTXTRAIL);
	m_iNumVertices = 0;
	m_ePrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
	m_iNumIndices = 0;
	m_pIB = nullptr;

	m_VBDesc.ByteWidth = iMaxVertices * m_iVertexStride;
	m_VBDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_VBDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_VBDesc.MiscFlags = 0;
	m_VBDesc.StructureByteStride = 0;
	return S_OK;
}

HRESULT CVIBuffer_Trail::Initialize(void* /*pArg*/)
{
	/* 인스턴스마다 자기 dynamic VB (매 프레임 WRITE_DISCARD map하므로 공유 불가) */
	if (FAILED(m_pDevice->CreateBuffer(&m_VBDesc, nullptr, &m_pVB)))
		return E_FAIL;
	return S_OK;
}

HRESULT CVIBuffer_Trail::Update_Vertices(const VTXTRAIL* pVertices, _uint iNumVertices)
{
	if (iNumVertices > iMaxVertices) iNumVertices = iMaxVertices;
	m_iNumVertices = iNumVertices;
	if (0 == iNumVertices || nullptr == pVertices || nullptr == m_pVB)
		return S_OK;

	D3D11_MAPPED_SUBRESOURCE SubResource{};
	if (FAILED(m_pContext->Map(m_pVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &SubResource)))
		return E_FAIL;
	memcpy(SubResource.pData, pVertices, sizeof(VTXTRAIL) * iNumVertices);
	m_pContext->Unmap(m_pVB, 0);
	return S_OK;
}

HRESULT CVIBuffer_Trail::Render()
{
	if (m_iNumVertices < 3) return S_OK;   // strip 최소 삼각형 1개
	m_pContext->Draw(m_iNumVertices, 0);
	return S_OK;
}

CVIBuffer_Trail* CVIBuffer_Trail::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CVIBuffer_Trail* pInstance = new CVIBuffer_Trail(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CVIBuffer_Trail");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CVIBuffer_Trail::Clone(void* pArg)
{
	CVIBuffer_Trail* pInstance = new CVIBuffer_Trail(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CVIBuffer_Trail");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CVIBuffer_Trail::Free()
{
	__super::Free();
}