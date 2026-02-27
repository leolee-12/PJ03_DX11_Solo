#pragma once

#include "Bounding.h"

NS_BEGIN(Engine)

class CBounding_AABB final : public CBounding
{
public:
	typedef struct tagAABBDesc final : public CBounding::BOUNDING_DESC
	{
		_float3		vSize;
	}AABB_DESC;
private:
	CBounding_AABB(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CBounding_AABB() = default;

public:
	const BoundingBox* Get_Desc() const {
		return m_pDesc;
	}

public:
	HRESULT Initialize(CBounding::BOUNDING_DESC* pInitialDesc);
	virtual void Update(_fmatrix WorldMatrix) override;
	virtual _bool Intersect(COLLIDER eType, CBounding* pBounding) override;
#ifdef _DEBUG
public:
	virtual HRESULT Render(PrimitiveBatch<VertexPositionColor>* pBatch, _fvector vColor) override;
#endif

private:
	BoundingBox*		m_pOriginalDesc = {};
	BoundingBox*		m_pDesc = {};

private:
	_float3 Compute_Min();
	_float3 Compute_Max();
	_bool Intersect_AABB(CBounding_AABB* pTargetBounding);




public:
	static CBounding_AABB* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CBounding::BOUNDING_DESC* pInitialDesc);
	virtual void Free() override;
};

NS_END