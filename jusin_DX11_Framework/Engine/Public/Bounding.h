#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class CBounding abstract : public CBase
{
public:
	struct BOUNDING_DESC
	{
		_float3 vCenter;
	};


protected:
	CBounding(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CBounding() = default;

public:
	virtual HRESULT Initialize(const CBounding::BOUNDING_DESC* pBoundingDesc);
	virtual void XM_CALLCONV Update(_fmatrix TransformMatrix) = 0;
	virtual _bool Intersect(COLLIDER eTargetType, CBounding* pBounding) = 0;

#ifdef _DEBUG
	virtual HRESULT Render(PrimitiveBatch<VertexPositionColor>* pBatch) = 0;
#endif

protected:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	_bool m_isColl = { false };

protected:
	virtual void Free() override;
};

NS_END