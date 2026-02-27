#pragma once

#include "Component.h"
#include "Bounding_AABB.h"
#include "Bounding_OBB.h"
#include "Bounding_Sphere.h"

/* 충돌체의 정보를 가진다. */

NS_BEGIN(Engine)

class ENGINE_DLL CCollider final : public CComponent
{
private:
	CCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCollider(const CCollider& Prototype);
	virtual ~CCollider() = default;

public:
	virtual HRESULT Initialize_Prototype(COLLIDER eType);
	virtual HRESULT Initialize(void* pArg);
	void Update(_fmatrix WorldMatrix);
	_bool Intersect(CCollider* pCollider);
#ifdef _DEBUG
	HRESULT Render();

#endif

private:
	COLLIDER			m_eType = { COLLIDER::END };
	class CBounding*	m_pBounding = { nullptr };
	ID3D11InputLayout*	m_pInputLayout = { nullptr };
	_bool				m_isColl = { false };
	
#ifdef _DEBUG
private:
	BasicEffect*							m_pEffect = { nullptr };
	PrimitiveBatch<VertexPositionColor>*	m_pBatch = { nullptr };
#endif 


public:
	static CCollider* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, COLLIDER eType);
	virtual CComponent* Clone(void* pArg);
	virtual void Free() override;
};

NS_END