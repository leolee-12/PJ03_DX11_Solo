#pragma once

#include "Collider.h"

/* 1. 여러 충돌체의 정보를 가지고 있는 자식클래스들의 부모가 되기위한 클래스이다. */

BEGIN(Engine)

class CBounding abstract : public CBase
{
public:
	struct BOUNDING_DESC
	{
		_float3			vCenter;
		CGameObject* pMineGameObject;
		CCollider_Manager::COLLIDERGROUP colliderGroup;
	};
protected:
	CBounding(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CBounding() = default;


public:
	virtual void Update(_fmatrix TransformMatrix) = 0;
	virtual void Update(_vector vTransform) = 0;

	
	virtual HRESULT Render(PrimitiveBatch<VertexPositionColor>* pBatch, _fvector vColor) = 0;

public:
	virtual _bool isCollision(CCollider::TYPE eTargetColliderType, CBounding* pTargetBounding) = 0;
	virtual _bool isRayCollision(const _float3& rayOrigin, const _float3& rayDir) = 0;
	virtual _bool isPointInAABB(const _float3& point);

protected:
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };


public:
	virtual void Free() override;

};

END