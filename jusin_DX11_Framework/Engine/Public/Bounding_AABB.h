#pragma once
#include "Bounding.h"

NS_BEGIN(Engine)

class CBounding_AABB final : public CBounding
{
public:
	struct BOUNDING_AABB_DESC : public CBounding::BOUNDING_DESC
	{
		_float3 vSize;
	};

private:
	CBounding_AABB(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CBounding_AABB() = default;

public:
	virtual HRESULT Initialize(const CBounding::BOUNDING_DESC* pBoundingDesc) override;
	virtual void XM_CALLCONV Update(_fmatrix TransformMatrix) override;

#ifdef _DEBUG
	virtual HRESULT Render(PrimitiveBatch<VertexPositionColor>* pBatch) override;
#endif

private:
	BoundingBox* m_pOriginalDesc = { nullptr };
	BoundingBox* m_pDesc = { nullptr };

public:
	static CBounding_AABB* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const CBounding::BOUNDING_DESC* pDesc);
	virtual void Free() override;

};

NS_END