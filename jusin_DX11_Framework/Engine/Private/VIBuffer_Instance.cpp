#include "VIBuffer_Instance.h"

CVIBuffer_Instance::CVIBuffer_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer{ pDevice, pContext }
{

}

CVIBuffer_Instance::CVIBuffer_Instance(const CVIBuffer_Instance& Prototype)
	: CVIBuffer{ Prototype }
	, m_pVBInstance{ Prototype.m_pVBInstance }
	, m_iNumInstances{ Prototype.m_iNumInstances }
	, m_iMaxInstances{ Prototype.m_iMaxInstances }
	, m_iInstanceStride{ Prototype.m_iInstanceStride }
	, m_iIndexCountPerInstance{ Prototype.m_iIndexCountPerInstance }
	, m_InstanceBufferDesc{ Prototype.m_InstanceBufferDesc }
	, m_pInstanceVertices{ Prototype.m_pInstanceVertices }
{
}

HRESULT CVIBuffer_Instance::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CVIBuffer_Instance::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CVIBuffer_Instance::Bind_Resources()
{
	ID3D11Buffer* pVertexBuffers[] =
	{
		m_pVB,
		m_pVBInstance
	};

	_uint iVertexStrides[] =
	{
		m_iVertexStride,
		m_iInstanceStride,
	};

	_uint iOffsets[] =
	{
		0, 0
	};

	m_pContext->IASetVertexBuffers(0, m_iNumVertexBuffers, pVertexBuffers, iVertexStrides, iOffsets);
	m_pContext->IASetIndexBuffer(m_pIB, m_eIndexFormat, 0);
	m_pContext->IASetPrimitiveTopology(m_ePrimitiveType);

	return S_OK;
}

HRESULT CVIBuffer_Instance::Render()
{
	if (0 == m_iNumInstances)
		return S_OK;

	m_pContext->DrawIndexedInstanced(m_iIndexCountPerInstance, m_iNumInstances, 0, 0, 0);

	return S_OK;
}

HRESULT CVIBuffer_Instance::Update_Instances(const VTXPARTICLE_INSTANCE* pInstances, _uint iNumInstances)
{
	if (iNumInstances > m_iMaxInstances)
		return E_FAIL;

	if (0 == iNumInstances)
	{
		m_iNumInstances = 0;
		return S_OK;
	}

	if (nullptr == pInstances || nullptr == m_pVBInstance || 0 == m_iInstanceStride)
		return E_FAIL;

	D3D11_MAPPED_SUBRESOURCE SubResource{};
	if (FAILED(m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_DISCARD, 0, &SubResource)))
		return E_FAIL;

	memcpy(SubResource.pData, pInstances, m_iInstanceStride * iNumInstances);

	m_pContext->Unmap(m_pVBInstance, 0);

	m_iNumInstances = iNumInstances;

	return S_OK;
}

void CVIBuffer_Instance::Free()
{
	__super::Free();

	if (false == m_isCloned)
		Safe_Delete_Array(m_pInstanceVertices);

	Safe_Release(m_pVBInstance);
}
