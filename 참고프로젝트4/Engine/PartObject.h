#pragma once

#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CModel;
class CBone;
class CCollider;

class ENGINE_DLL CPartObject : public CGameObject
{
public:
	typedef struct tagPartDesc : tagGameObjectDesc
	{
		class shared_ptr<CTransform> pParentTransform = { nullptr };
		class CBoneGroup* pBoneGroup = { nullptr };
		wstring	  strBoneName;
	}PARTOBJECT_DESC;

protected:
	CPartObject(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CPartObject(const CPartObject& rhs);
	virtual ~CPartObject() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Begin_Play(_cref_time fTimeDelta) override;
	virtual void Priority_Tick(_cref_time fTimeDelta) override;
	virtual void Tick(_cref_time fTimeDelta) override;
	virtual void Late_Tick(_cref_time fTimeDelta) override;
	virtual void Before_Render(_cref_time fTimeDelta) override;
	virtual void End_Play(_cref_time fTimeDelta) override;
	virtual HRESULT Render() override;

	virtual HRESULT Render_Shadow() override;

public:
	virtual void OnCollision_Enter(class CCollider* pThisCol, class  CCollider* pOtherCol);
	virtual void OnCollision_Stay(class  CCollider* pThisCol, class  CCollider* pOtherCol);
	virtual void OnCollision_Exit(class  CCollider* pThisCol, class  CCollider* pOtherCol);

public:
	virtual void PhysX_OnCollision_Enter(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo);
	virtual void PhysX_OnCollision_Stay(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo);
	virtual void PhysX_OnCollision_Exit(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo);

public:
	_float4x4	 Get_WorldMatrix() { return m_WorldMatrix; }
	void		 Set_WorldMatrix(_float4x4 WorldMatrix) { m_WorldMatrix = WorldMatrix; }

	_float4x4	Get_WorldFloat4x4WithParent();
	_matrix		Get_WorldMatrixWithParent();

	_float4x4	Get_WorldFloat4x4FromBone(const wstring& strBoneName);
	_matrix		Get_WorldMatrixFromBone(const wstring& strBoneName);

	GETSET_EX2(PART_OBJTYPE, m_ePartObjType, PartObjType, GET, SET)

protected:
	weak_ptr<CTransform>		m_pParentTransform;					// 부모 트랜스폼
	CBoneGroup*					m_pBoneGroup = { nullptr };			// 이 오브젝트가 붙은 부모의 뼈
	_uint						m_iBoneIndex = 0;					// 딱 달라붙은 뼈의 인덱스
	_float4x4					m_WorldMatrix = {};					// 최종 행렬
	PART_OBJTYPE				m_ePartObjType = { PART_OBJTYPE_END };

	weak_ptr<class CBlendObject> m_pPartEffect;

public:
	GETSET_EX2(weak_ptr<class CBlendObject>, m_pPartEffect, PartEffect, GET, SET)

	void	TurnOff_PartEffect();
	void	TurnOn_PartEffect();

protected:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

	virtual void Free() override;
};

END