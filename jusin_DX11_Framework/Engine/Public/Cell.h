#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class CCell final : public CBase
{
private:
	CCell(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CCell() = default;

public:
	_fvector Get_Point(VTXPOINT ePoint) { return XMLoadFloat3(&m_vPoints[ETOUI(ePoint)]); }
	void Set_Neighbor(LINE eLine, CCell* pNeighborCell) { m_iNeighbors[ETOUI(eLine)] = pNeighborCell->m_iIndex; }
	void Set_Neighbors(const _int* pNeighborIndices) { memcpy(m_iNeighbors, pNeighborIndices, sizeof(_int) * 3); }
	_int* Get_NeighborIndices() { return m_iNeighbors; }
	_vector Get_Center()
	{
		_vector vCenter = {};
		for (size_t i = 0; i < 3; i++)
			vCenter += XMLoadFloat3(&m_vPoints[i]);
		return XMVectorSetW(vCenter / 3.f, 1.f);
	}

	HRESULT Initialize(const _float3* pPoints, _uint iIndex);
	_bool XM_CALLCONV Is_In(_fvector vResultPos, _int* pNeighborIndex);
	_bool XM_CALLCONV Compare_Points(_fvector vSourPoint, _fvector vDestPoint);
	_float XM_CALLCONV Compute_Height(_fvector vTargetPos);

#ifdef _DEBUG
public:
	HRESULT Render();
#endif

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	_float3 m_vPoints[ETOUI(VTXPOINT::END)] = {};
	_float3 m_vNormals[ETOUI(LINE::END)] = {};
	_uint m_iIndex = {};
	_int m_iNeighbors[ETOUI(LINE::END)] = { -1, -1, -1 };
	_float4 m_vPlane = {};

#ifdef _DEBUG
private:
	class CVIBuffer_Cell* m_pVIBuffer = { nullptr };
#endif

public:
	static CCell* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _float3* pPoints, _uint iIndex);
	virtual void Free() override;
};

NS_END