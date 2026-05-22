#pragma once
#include "Base.h"


NS_BEGIN(Engine)
class CGameInstance;

class CQuadTree : public CBase
{
public:
	enum CORNER { LT, RT, RB, LB, END };

private:
	CQuadTree();
	virtual ~CQuadTree() = default;

public:
	HRESULT Initialize(_uint iLT, _uint iRT, _uint iRB, _uint iLB);
	void Culling(CGameInstance* pGI, const _float3* pVtxPos, _uint* pIndices, _uint* pNumIndices);

private:
	_uint m_iCorners[CORNER::END] = {};
	_uint m_iCenter = {};
	CQuadTree* m_Children[CORNER::END] = {};

public:
	static CQuadTree* Create(_uint iLT, _uint iRT, _uint iRB, _uint iLB);

private:
	virtual void Free() override;
};

NS_END