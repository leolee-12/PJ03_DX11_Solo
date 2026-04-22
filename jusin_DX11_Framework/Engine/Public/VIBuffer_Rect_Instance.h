#pragma once
#include "VIBuffer_Instance.h"

NS_BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Rect_Instance final : public CVIBuffer_Instance
{
public:
	struct RECT_INSTANCE_DESC final : public CVIBuffer_Instance::INSTANCE_DESC
	{
		_float2 vSpeedRange;
		_float2 vLifeRange;
		_bool isLoop;
	};

private:
	CVIBuffer_Rect_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const RECT_INSTANCE_DESC& tDesc);
	CVIBuffer_Rect_Instance(const CVIBuffer_Rect_Instance& Prototype);
	virtual ~CVIBuffer_Rect_Instance() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);

	void Drop(_float fTimeDelta);
	void Spread(_float fTimeDelta);

private:
	RECT_INSTANCE_DESC m_tInitDesc = {};
	_float* m_pSpeeds = { nullptr };
	_bool m_isLoop = { false };

public:
	static CVIBuffer_Rect_Instance* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pInitialDesc);
	virtual CComponent* Clone(void* pArg) override;

private:
	virtual void Free() override;

};

NS_END