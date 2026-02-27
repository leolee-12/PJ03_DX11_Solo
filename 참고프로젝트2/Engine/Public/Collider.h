#pragma once

#include "Component.h"
#include "Collider_Manager.h"

BEGIN(Engine)

class ENGINE_DLL CCollider final : public CComponent
{
public:
	typedef struct
	{
		_float3 center;  // Áß½É ÁÂÇ¥
		_float width, height, depth;
		class CGameObject* MineGameObject;
	}COLLIDER_DESC;

public:
	enum TYPE { TYPE_AABB, TYPE_OBB, TYPE_SPHERE, TYPE_END };

private:
	CCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCollider(const CCollider& Prototype);
	virtual ~CCollider() = default;

public:
	virtual HRESULT Initialize_Prototype(TYPE eColliderType);
	virtual HRESULT Initialize(void* pArg);
	virtual void Update(_fmatrix TransformMatrix);
	virtual void Update(_vector vPosition);

	virtual HRESULT Render(_float fTimeDelta);

public:
	_bool isCollision(CCollider* pTargetCollider);

	class CGameObject* GetMineGameObject() { return m_pMineGameObject; };
	BoundingBox AABB_GetDesc();
	void AABB_SetDesc(BoundingBox _box);
	_float Get_Overlap_X(CCollider* other) const;
	_float3 Get_Overlap_Center_Position(const CCollider* pOther) const;

public:
	void OnCollisionEnter(CCollider*, _float fTimeDelta);
	void OnCollisionStay(CCollider*, _float fTimeDelta);
	void OnCollisionExit(CCollider*);

public:
	_bool					m_isColl = { false };
	CCollider_Manager::COLLIDERGROUP m_ColliderGroup;

private:
	TYPE					m_eColliderType = { TYPE_END };
	class CBounding*		m_pBounding = { nullptr };
	class CGameObject*		m_pMineGameObject = { nullptr };

#ifdef _DEBUG
private:
	PrimitiveBatch<VertexPositionColor>*	m_pBatch = { nullptr };
	BasicEffect*							m_pEffect = { nullptr };
	ID3D11InputLayout*						m_pInputLayout = { nullptr };
#endif
	
public:
	static CCollider* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TYPE eColliderType);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END