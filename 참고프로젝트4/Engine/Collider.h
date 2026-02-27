//Collider.h
#pragma once

#include "Component.h"

#include "Bounding_AABB.h"
#include "Bounding_OBB.h"
#include "Bounding_Sphere.h"

/* 충돌체. */
/* 충돌체를 정의하는 기능 + 충돌체을 그리는 기능 + 충돌을 비교하는 기능 . */

BEGIN(Engine)

class ENGINE_DLL CCollider final : public CComponent
{
public:
	enum TYPE { TYPE_SPHERE, TYPE_AABB, TYPE_OBB, TYPE_RAY, TYPE_END };
private:
	CCollider(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CCollider(const CCollider& rhs);
	virtual ~CCollider() = default;

public:
	virtual HRESULT Initialize_Prototype(TYPE eType);
	virtual HRESULT Initialize(void* pArg);

public:
	TYPE Get_ColliderType() const {
		return m_eType;
	}

	vector<class CBounding*> Get_Bounding() {
		return m_vecBounding;
	}

#ifdef _DEBUG
public:
	HRESULT Render();
#endif
	void	Update(_fmatrix TransformMatrix);
	_bool   Collision(CCollider* pTargetCollider);

	HRESULT Add_Bounding(void* pArg);
	COLLISIONINFO	Update_CollisionInfo(CCollider* pTargetCollider, _vector& vDirection, _float& fOverapDist);
private:
	TYPE							m_eType = { TYPE_END };
	vector<class CBounding*>		m_vecBounding;
	_bool							m_isCollision = { false };
	COLLISIONINFO					m_CollisionInfo = {};
	_float3							m_vCollisionCenter = {};

	static _uint			m_iNewColliderID;
	_uint					m_iColliderID;
	_uint					m_iColliderLayer = {};

	_bool					m_bIsEnable = { true };
public:
	_uint					Get_ColliderID() { return m_iColliderID; }
	void					Set_ColliderID(_uint iID) {m_iColliderID = iID;}
	void					Set_CollisionCenter(_float3 vCenter) { m_vCollisionCenter = vCenter; }
	_float3					Get_CollisionCenter() {	return m_vCollisionCenter;}

	GETSET_EX2(_bool, m_bIsEnable,IsEnable,GET,SET)
	GETSET_EX2(_uint, m_iColliderLayer, ColliderLayer, GET, SET)
	GETSET_EX2(COLLISIONINFO, m_CollisionInfo, CollisionInfo, GET, SET)

public:
	virtual void OnCollision_Enter(CCollider* pThisCol, CCollider* pOtherCol);
	virtual void OnCollision_Stay(CCollider* pThisCol, CCollider* pOtherCol);
	virtual void OnCollision_Exit(CCollider* pThisCol, CCollider* pOtherCol);

#ifdef _DEBUG
private:
	PrimitiveBatch<VertexPositionColor>* m_pBatch = { nullptr };
	BasicEffect* m_pEffect = { nullptr };
	ID3D11InputLayout* m_pInputLayout = { nullptr };
#endif


public:
	static shared_ptr<CCollider> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, TYPE eType);
	virtual shared_ptr<CComponent> Clone(void* pArg) override;
	virtual void Free() override;
};

END
