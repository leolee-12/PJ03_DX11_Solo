#pragma once

#include "VIBuffer.h"

NS_BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Instancing abstract : public CVIBuffer
{
public:
	typedef struct tagInstanceDesc
	{
		_uint		iNumInstance;
		_float2		vScale;
		_float3		vCenter;
		_float3		vRange;
	}INSTANCE_DESC;
protected:
	CVIBuffer_Instancing(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Instancing(const CVIBuffer_Instancing& Prototype);
	virtual ~CVIBuffer_Instancing() = default;

public:
	virtual HRESULT Initialize_Prototype(const INSTANCE_DESC* pDesc);
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Bind_Buffers() override;
	virtual HRESULT Render() override;
protected:
	ID3D11Buffer* m_pVBInstance = { };
	_uint		  m_iInstanceVertexStride = {};
	_uint		  m_iNumInstance = {};
	_uint		  m_iIndexCountPerInstance = {};

	D3D11_BUFFER_DESC m_InstanceDesc = {};

public:
	virtual CComponent* Clone(void* pArg) = 0;
	virtual void Free() override;
};

NS_END