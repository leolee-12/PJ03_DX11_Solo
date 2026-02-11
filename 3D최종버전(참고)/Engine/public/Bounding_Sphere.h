#pragma once

#include "Bounding.h"

NS_BEGIN(Engine)

class CBounding_Sphere final : public CBounding
{
public:
	typedef struct tagSphereDesc final : public CBounding::BOUNDING_DESC
	{
		_float		fRadius;
	}SPHERE_DESC;
private:
	CBounding_Sphere(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CBounding_Sphere() = default;
public:
	const BoundingSphere* Get_Desc() const {
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
	BoundingSphere*		m_pOriginalDesc = {};
	BoundingSphere*		m_pDesc = {};

public:
	static CBounding_Sphere* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CBounding::BOUNDING_DESC* pInitialDesc);
	virtual void Free() override;
};

NS_END