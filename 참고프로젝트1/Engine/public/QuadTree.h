#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class CQuadTree final : public CBase
{
private:
	CQuadTree();
	virtual ~CQuadTree() = default;

public:
	HRESULT Initialize(_uint iLT, _uint iRT, _uint iRB, _uint iLB);
	void Culling(class CGameInstance* pGameInstance, const _float3* pVertexPositions, _uint* pIndices, _uint* pNumIndices);
	void SetUp_Neighbors();
private:
	CQuadTree*	m_pChildren[ENUM_CLASS(CORNER::END)] = {};
	_uint		m_iCenterIndex = {};
	_uint		m_iCornerIndices[ENUM_CLASS(CORNER::END)] = {};
	CQuadTree*  m_pNeighbors[ENUM_CLASS(CORNER::END)] = { nullptr, };


private:
	_bool isDraw(class CGameInstance* pGameInstance, const _float3* pVertexPositions);

public:
	static CQuadTree* Create(_uint iLT, _uint iRT, _uint iRB, _uint iLB);
	virtual void Free() override;
};

NS_END