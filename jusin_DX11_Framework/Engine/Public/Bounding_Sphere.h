#pragma once
#include "Bounding.h"

NS_BEGIN(Engine)

class CBounding_Sphere final : public CBounding
{
public:
	struct BOUNDING_SPHERE_DESC : public CBounding::BOUNDING_DESC
	{
		_float fRadius;
	};

private:
	CBounding_Sphere(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CBounding_Sphere() = default;

public:
	virtual HRESULT Initialize(const CBounding::BOUNDING_DESC* pBoundingDesc) override;
	virtual void XM_CALLCONV Update(_fmatrix TransformMatrix) override;

#ifdef _DEBUG
	virtual HRESULT Render(PrimitiveBatch<VertexPositionColor>* pBatch) override;
#endif

private:
	BoundingSphere* m_pOriginalDesc = { nullptr };
	BoundingSphere* m_pDesc = { nullptr };

public:
	static CBounding_Sphere* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const CBounding::BOUNDING_DESC* pDesc);
	virtual void Free() override;

};

NS_END