#pragma once

#include "Bounding.h"

NS_BEGIN(Engine)

class CBounding_OBB final : public CBounding
{
public:
	typedef struct tagOBBDesc final : public CBounding::BOUNDING_DESC
	{
		_float3		vSize;
		_float3		vRadians;
	}OBB_DESC;

	struct tagOBB
	{
		_float3		vCenter;
		_float3		vCenterDir[3];
		_float3		vAlignDir[3];
	};

private:
	CBounding_OBB(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CBounding_OBB() = default;
public:
	const BoundingOrientedBox* Get_Desc() const {
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
	BoundingOrientedBox*		m_pOriginalDesc = {};
	BoundingOrientedBox*		m_pDesc = {};
private:
	tagOBB Compute_OBB();
	_bool Intersect_OBB(CBounding_OBB* pTargetBounding);



public:
	static CBounding_OBB* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CBounding::BOUNDING_DESC* pInitialDesc);
	virtual void Free() override;
};

NS_END