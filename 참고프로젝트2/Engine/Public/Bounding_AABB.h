#pragma once

#include "Bounding.h"

/* 구의 충돌을 위한 데이터를 가진다. */
/* 충돌을 위한 함수를 가진다. */

BEGIN(Engine)

class CBounding_AABB final : public CBounding
{
public:
	struct BOUNDING_AABB_DESC : public BOUNDING_DESC
	{
		_float3		vExtents;
	};

private:
	CBounding_AABB(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CBounding_AABB() = default;

public:
	BoundingBox Get_Desc() const {
		return *m_pDesc;
	}

	void Set_Desc(BoundingBox _box);

public:
	virtual HRESULT Initialize(CBounding::BOUNDING_DESC* pDesc);
	virtual void Update(_fmatrix TransformMatrix) override;
	virtual void Update(_vector vTransform);

	virtual HRESULT Render(PrimitiveBatch<VertexPositionColor>* pBatch, _fvector vColor) override;
	virtual _bool isCollision(CCollider::TYPE eTargetColliderType, CBounding* pTargetBounding);
	_bool isRayCollision(const _float3& rayOrigin, const _float3& rayDir) override;
	virtual _bool isPointInAABB(const _float3& point) override;
	_float Get_Overlap_X(const CBounding_AABB* pOther) const;
	_float3 Get_Overlap_Center_Position(const CBounding_AABB* pOther) const;

	
private:
	BoundingBox* m_pOriginalDesc = { nullptr };
	BoundingBox* m_pDesc = { nullptr };

private:
	_bool Collision_AABB(CBounding_AABB* pTargetDesc);

	_float3 Compute_Min();
	_float3 Compute_Max();

public:
	static CBounding_AABB* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CBounding::BOUNDING_DESC* pDesc);
	virtual void Free() override;

	// CBounding을(를) 통해 상속됨
};

END