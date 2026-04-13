#pragma once
#include "VIBuffer.h"

NS_BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Cell final : public CVIBuffer
{
private:
	CVIBuffer_Cell(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _float3* pPoints);
	CVIBuffer_Cell(const CVIBuffer_Cell& Prototype);
	virtual ~CVIBuffer_Cell() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

private:
	_float3 m_vPoints[ETOUI(VTXPOINT::END)] = {};

public:
	static CVIBuffer_Cell* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _float3* pPoints);
	virtual CComponent* Clone(void* pArg) override;

protected:
	virtual void Free() override;

};

NS_END