#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class CFrustum final : public CBase
{
private:
	CFrustum();
	virtual ~CFrustum() = default;

public:
	HRESULT Initialize();
	void Update();

public:
	void XM_CALLCONV Transform_ToLocalSpace(_fmatrix WorldMatrix);
	_bool XM_CALLCONV isIn_WorldSpace(_fvector vWorldPos, _float fRange = 0.f);
	_bool XM_CALLCONV isIn_LocalSpace(_fvector vLocalPos, _float fRange = 0.f);

private:
	class CGameInstance* m_pGameInstance = { nullptr };

private:
	_float4	m_vOriginalPoints[8] = {};
	_float4	m_vWorldPoints[8] = {};
	_float4	m_vWorldPlanes[6] = {};
	_float4	m_vLocalPlanes[6] = {};

private:
	void Make_Planes(const _float4* pPoints, _float4* pPlanes);

public:
	static CFrustum* Create();

private:
	virtual void Free() override;
};

NS_END