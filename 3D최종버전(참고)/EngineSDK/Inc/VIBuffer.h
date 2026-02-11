#pragma once

/* VIBuffer : Vertex + Index + Buffer(메모리공간) */
#include "Component.h"

NS_BEGIN(Engine)

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
	ID3D11Buffer*			m_pVB = { nullptr };
	ID3D11Buffer*			m_pIB = { nullptr };

	_uint					m_iNumVertexBuffers = { };
	
	_uint					m_iNumVertices = {};
	_uint					m_iVertexStride = {};

	_uint					m_iNumIndices = {};
	_uint					m_iIndexStride = {};
	DXGI_FORMAT				m_eIndexFormat = {};
	D3D_PRIMITIVE_TOPOLOGY	m_ePrimitive = {};

	_float3* m_pVertexPositions = { nullptr };

public:
	virtual CComponent* Clone(void* pArg) = 0;
	virtual void Free() override;
};

NS_END