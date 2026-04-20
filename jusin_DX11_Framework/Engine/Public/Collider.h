#pragma once
#include "Component.h"
#include "Bounding_AABB.h"
#include "Bounding_OBB.h"
#include "Bounding_Sphere.h"

NS_BEGIN(Engine)

class ENGINE_DLL CCollider final : public CComponent
{
private:
	CCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, COLLIDER eType);
	CCollider(const CCollider& Prototype);
	virtual ~CCollider() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);

	void XM_CALLCONV Update(_fmatrix TransformMatrix);
	_bool Intersect(CCollider* pTarget);

#ifdef _DEBUG
	HRESULT Render();
#endif

private:
	COLLIDER m_eType = { COLLIDER::END };
	class CBounding* m_pBounding = { nullptr };
	_bool m_isColl = { false };

#ifdef _DEBUG
	PrimitiveBatch<VertexPositionColor>* m_pBatch = { nullptr };
	BasicEffect* m_pEffect = { nullptr };
	ID3D11InputLayout* m_pInputLayout = { nullptr };
#endif

public:
	static CCollider* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, COLLIDER eType);
	virtual CComponent* Clone(void* pArg) override;

private:
	virtual void Free() override;
};

NS_END