#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class CCell final : public CBase
{

private:
	CCell(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CCell() = default;

public:
	HRESULT Initialize(const _float3* pPoints);

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	_float3 m_vPoints[ETOUI(VTXPOINT::END)] = {};

public:
	static CCell* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _float3* pPoints);
	virtual void Free() override;
};

NS_END