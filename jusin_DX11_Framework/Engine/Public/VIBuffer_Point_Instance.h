#pragma once
#include "VIBuffer_Instance.h"

NS_BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Point_Instance final : public CVIBuffer_Instance
{
public:
	struct POINT_INSTANCE_DESC final : public CVIBuffer_Instance::INSTANCE_DESC
	{
		_float2 vSpeedRange;
		_float2 vLifeRange;
		_bool isLoop;
		_float3 vPivot;
	};

private:
	CVIBuffer_Point_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const POINT_INSTANCE_DESC& tDesc);
	CVIBuffer_Point_Instance(const CVIBuffer_Point_Instance& Prototype);
	virtual ~CVIBuffer_Point_Instance() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual HRESULT Bind_Resources() override;
	virtual HRESULT Render() override;

	void Drop(_float fTimeDelta);
	void Spread(_float fTimeDelta);

private:
	POINT_INSTANCE_DESC m_tInitDesc = {};
	_float* m_pSpeeds = { nullptr };
	_bool m_isLoop = { false };

	_uint m_iVertexCountPerInstance = {};
	_float3	m_vPivot = { };

public:
	static CVIBuffer_Point_Instance* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pInitialDesc);
	virtual CComponent* Clone(void* pArg) override;

private:
	virtual void Free() override;

};

NS_END