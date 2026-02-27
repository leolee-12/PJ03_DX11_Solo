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
	void Transform_ToLocalSpace(_fmatrix WorldMatrixInverse);
	_bool isIn_WorldSpace(_fvector vWorldPos, _float fRadius);
	_bool isIn_LocalSpace(_fvector vLocalPos, _float fRadius);
private:
	_float3			m_vOriginalPoints[8] = {};
	_float3			m_vWorldPoints[8] = {};
	_float4			m_vWorldPlanes[6] = {};
	_float4			m_vLocalPlanes[6] = {};

	class CGameInstance* m_pGameInstance = { nullptr };

private:
	void Make_Planes(const _float3* pPoints, _float4* pPlanes);

public:
	static CFrustum* Create();
	virtual void Free() override;
};

NS_END