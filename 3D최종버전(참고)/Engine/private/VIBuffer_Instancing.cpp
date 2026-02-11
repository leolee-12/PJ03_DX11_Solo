#include "VIBuffer_Instancing.h"

CVIBuffer_Instancing::CVIBuffer_Instancing(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer { pDevice, pContext }
{
}

CVIBuffer_Instancing::CVIBuffer_Instancing(const CVIBuffer_Instancing& Prototype)
	: CVIBuffer { Prototype }
	// , m_pVBInstance { Prototype.m_pVBInstance }
	, m_iInstanceVertexStride { Prototype.m_iInstanceVertexStride }
	, m_iNumInstance { Prototype.m_iNumInstance }
	, m_iIndexCountPerInstance { Prototype.m_iIndexCountPerInstance }
	, m_InstanceDesc{ Prototype.m_InstanceDesc }
{
	
	
}

HRESULT CVIBuffer_Instancing::Initialize_Prototype(const INSTANCE_DESC* pDesc)
{
	return S_OK;
}

HRESULT CVIBuffer_Instancing::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CVIBuffer_Instancing::Bind_Buffers()
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
	m_pContext->IASetIndexBuffer(m_pIB, m_eIndexFormat, 0);
	m_pContext->IASetPrimitiveTopology(m_ePrimitive);

	return S_OK;
}

HRESULT CVIBuffer_Instancing::Render()
{
	m_pContext->DrawIndexedInstanced(m_iIndexCountPerInstance, m_iNumInstance, 0, 0, 0);

	return S_OK;
}

void CVIBuffer_Instancing::Free()
{
	__super::Free();

	Safe_Release(m_pVBInstance);
}
