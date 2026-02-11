#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class CCell final : public CBase
{
private:
	CCell(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CCell() = default;

public:
	_vector Get_Point(POINT ePoint) const {
		return XMLoadFloat3(&m_vPoints[ENUM_CLASS(ePoint)]);
	}

	void Set_Neighbor(LINE eLine, CCell* pNeighbor) {
		m_iNeighborIndices[ENUM_CLASS(eLine)] = pNeighbor->m_iIndex;
	}

public:
	HRESULT Initialize(const _float3* pPoints, _uint iIndex);
	_bool isIn(_fvector vResultPos, _int* pNeighborIndex);
	_bool Compare_Points(_fvector vSour, _fvector vDest);
	_float Compute_Height(_fvector vResultPos);

#ifdef _DEBUG
public:
	HRESULT Render();
#endif

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

#ifdef _DEBUG
private:
	class CVIBuffer_Cell* m_pDebugBuffer = { };
#endif

private:
	_uint			m_iIndex = {};
	_float3			m_vPoints[ENUM_CLASS(POINT::END)] = {};
	_float3			m_vNormals[ENUM_CLASS(LINE::END)] = {};

	_int			m_iNeighborIndices[3] = { -1, -1, -1 };

public:
	static CCell* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _float3* pPoints, _uint iIndex);
	virtual void Free() override;
};

NS_END