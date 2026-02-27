#include "..\Public\VIBuffer.h"

CVIBuffer::CVIBuffer(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CComponent{ pDevice, pContext }
{

}

CVIBuffer::CVIBuffer(const CVIBuffer & Prototype)
	: CComponent{ Prototype }
	, m_pVB { Prototype.m_pVB }
	, m_pIB { Prototype.m_pIB }
	, m_pStagingVB{ Prototype.m_pStagingVB }
	, m_iNumVertexBuffers { Prototype.m_iNumVertexBuffers }
	, m_iNumVertices { Prototype.m_iNumVertices }
	, m_iVertexStride { Prototype.m_iVertexStride }
	, m_iNumIndices { Prototype.m_iNumIndices}
	, m_iIndexStride { Prototype.m_iIndexStride }
	, m_eIndexFormat { Prototype.m_eIndexFormat }
	, m_ePrimitive_Topology { Prototype.m_ePrimitive_Topology }
{
	Safe_AddRef(m_pIB);
	Safe_AddRef(m_pVB);
	Safe_AddRef(m_pStagingVB);
}

HRESULT CVIBuffer::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CVIBuffer::Initialize(void * pArg)
{
	return S_OK;
}

HRESULT CVIBuffer::Bind_Buffers()
{
	ID3D11Buffer*		pVertexBufffers[] = {
		m_pVB, 
		/*m_pVBInstance*/
	};

	_uint				iVertexStrides[] = {
		m_iVertexStride, 
	};

	_uint				iOffsets[] = {
		0, 
	};

	m_pContext->IASetVertexBuffers(0, m_iNumVertexBuffers, pVertexBufffers, iVertexStrides, iOffsets);
	m_pContext->IASetIndexBuffer(m_pIB, m_eIndexFormat, 0);
	m_pContext->IASetPrimitiveTopology(m_ePrimitive_Topology);

	return S_OK;
}

HRESULT CVIBuffer::Render()
{
	m_pContext->DrawIndexed(m_iNumIndices, 0, 0);

	return S_OK;
}


HRESULT CVIBuffer::Create_Buffer(ID3D11Buffer ** ppOut)
{
	/* m_BufferDesc : 내가 할당하고자하는 버퍼(메모리)의 정보를 담고 있다.(몇바이트나 할당할지? 동적, 정적으로 할당할건지?) */
	/* m_InitialData : 위에 해당하는 버퍼 공간을 할당할 때, 이 공간에 어떤 값을 채워넣어줄건지? */
	return m_pDevice->CreateBuffer(&m_BufferDesc, &m_InitialData, ppOut);	
}

void CVIBuffer::Free()
{
	Safe_Release(m_pIB);
	Safe_Release(m_pVB);
	Safe_Release(m_pStagingVB);

	__super::Free();
}

