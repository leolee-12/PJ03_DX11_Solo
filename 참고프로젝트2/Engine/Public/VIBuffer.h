#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer abstract : public CComponent
{
protected:
	CVIBuffer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer(const CVIBuffer& Prototype);
	virtual ~CVIBuffer() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Bind_Buffers();
	virtual HRESULT Render();

protected:
	ID3D11Buffer*				m_pVB = { nullptr };
	ID3D11Buffer*				m_pIB = { nullptr };
	ID3D11Buffer*				m_pStagingVB = { nullptr };
protected:
	D3D11_BUFFER_DESC			m_BufferDesc = {};
	D3D11_SUBRESOURCE_DATA		m_InitialData = {};

	_uint						m_iNumVertexBuffers = { 0 };
	_uint						m_iNumVertices = { 0 };
	_uint						m_iVertexStride = { 0 };

	_uint						m_iNumIndices = { 0 };
	_uint						m_iIndexStride = { 0 };
	DXGI_FORMAT					m_eIndexFormat = {};
	D3D11_PRIMITIVE_TOPOLOGY	m_ePrimitive_Topology = {};


protected:
	HRESULT Create_Buffer(ID3D11Buffer** ppOut);


public:
	virtual CComponent* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END