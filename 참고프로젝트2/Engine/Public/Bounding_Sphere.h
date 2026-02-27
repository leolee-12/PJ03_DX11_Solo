#pragma once

#include "Bounding.h"

/* 구의 충돌을 위한 데이터를 가진다. */
/* 충돌을 위한 함수를 가진다. */

BEGIN(Engine)

class CBounding_Sphere final : public CBounding
{
public:
	struct BOUNDING_SPHERE_DESC : public BOUNDING_DESC
	{
		_float		fRadius;
	};
private:
	CBounding_Sphere(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CBounding_Sphere() = default;

public:
	BoundingSphere Get_Desc() const {
		return *m_pDesc;
	}

public:
	virtual HRESULT Initialize(CBounding::BOUNDING_DESC* pDesc);
	virtual void Update(_fmatrix TransformMatrix) override;
	virtual void Update(_vector vTransform) override;

	virtual HRESULT Render(PrimitiveBatch<VertexPositionColor>* pBatch, _fvector vColor) override;
	virtual _bool isCollision(CCollider::TYPE eTargetColliderType, CBounding* pTargetBounding) override;
	_bool isRayCollision(const _float3& rayOrigin, const _float3& rayDir) override;


private:
	BoundingSphere*				m_pOriginalDesc = { nullptr };
	BoundingSphere*				m_pDesc = { nullptr };	

public:
	static CBounding_Sphere* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CBounding::BOUNDING_DESC* pDesc);
	virtual void Free() override;
};

END