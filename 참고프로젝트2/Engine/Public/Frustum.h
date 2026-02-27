#pragma once

#include "Base.h"

BEGIN(Engine)

class CFrustum final : public CBase
{
private:
	CFrustum();
	virtual ~CFrustum() = default;

public:
	HRESULT Initialize();
	void Update();

	void Get_ParallelVectorsInPlane(_float3& tangent1, _float3& tangent2, _float fov);

private:
	class CGameInstance*	m_pGameInstance = { nullptr };
	_float3					m_vPoints[8];	
	_float3					m_vWorldPoints[8];
	_float4					m_vWorldPlanes[6];
	_float4					m_vLocalPlanes[6];

	

private:
	void Make_Planes(_float4* pPlanes, const _float3* pPoints);

public:
	static CFrustum* Create();
	virtual void Free() override;
};

END