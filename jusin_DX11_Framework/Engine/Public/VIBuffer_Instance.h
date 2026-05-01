#pragma once
#include "VIBuffer.h"

NS_BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Instance abstract : public CVIBuffer
{
public:
	struct INSTANCE_DESC
	{
		_uint iNumInstance = {};
		_float3 vCenter;
		_float3 vPosOffset;
		_float2 vSizeRange;
	};

protected:
	CVIBuffer_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Instance(const CVIBuffer_Instance& Prototype);
	virtual ~CVIBuffer_Instance() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);

	virtual HRESULT Bind_Resources() override;
	virtual HRESULT Render() override;

	HRESULT Update_Instances(const VTXPARTICLE_INSTANCE* pInstances, _uint iNumInstances);

	_uint Get_NumInstances() const { return m_iNumInstances; }
	_uint Get_MaxInstances() const { return m_iMaxInstances; }

protected:
	ID3D11Buffer* m_pVBInstance = { nullptr };
	_uint m_iNumInstances = {};
	_uint m_iMaxInstances = {};
	_uint m_iInstanceStride = {};
	_uint m_iIndexCountPerInstance = {};

	D3D11_BUFFER_DESC m_InstanceBufferDesc = {};
	VTXPARTICLE_INSTANCE* m_pInstanceVertices = { nullptr };

public:
	virtual CComponent* Clone(void* pArg) = 0;

protected:
	virtual void Free() override;
};

NS_END