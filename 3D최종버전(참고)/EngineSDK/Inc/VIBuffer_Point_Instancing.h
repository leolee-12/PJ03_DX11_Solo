#pragma once

#include "VIBuffer_Instancing.h"

NS_BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Point_Instancing final : public CVIBuffer_Instancing
{
public:
	typedef struct tagPointInstance final : public CVIBuffer_Instancing::INSTANCE_DESC
	{
		_float3			vPivot;
		_float2			vSpeed;
		_float2			vLifeTime;
		_bool			isLoop;
	}POINT_INSTANCE_DESC;

private:
	CVIBuffer_Point_Instancing(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Point_Instancing(const CVIBuffer_Point_Instancing& Prototype);
	virtual ~CVIBuffer_Point_Instancing() = default;

public:
	virtual HRESULT Initialize_Prototype(const INSTANCE_DESC* pDesc) override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Bind_Buffers() override;
	virtual HRESULT Render() override;

public:
	void Drop(_float fTimeDelta);
	void Spread(_float fTimeDelta);

private:
	VTXINSTANCEPARTICLE*	m_pInstanceVertices = { nullptr };
	_float*					m_pSpeeds = { nullptr };
	_bool					m_isLoop = {};
	_float3					m_vPivot = {};

public:
	static CVIBuffer_Point_Instancing* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const INSTANCE_DESC* pDesc);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END