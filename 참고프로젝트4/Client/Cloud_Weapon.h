#pragma once

#include "WeaponPart.h"
#include "Client_Defines.h"

#include "Level_Test_Defines.h"

BEGIN(Client)

class CCloud_Weapon : public CWeaponPart
{
	INFO_CLASS(CCloud_Weapon, CWeaponPart)

	enum SOKETBONETYPE
	{
		HAND,BACK
	};

protected:
	CCloud_Weapon(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CCloud_Weapon(const CCloud_Weapon& rhs);
	virtual ~CCloud_Weapon() = default;

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

private:
	HRESULT Ready_Components(void* pArg);
	HRESULT Bind_ShaderResources();

private:
	void	Ascension_Mode(_cref_time fTimeDelta);

public:
	virtual void OnCollision_Enter(class CCollider* pThisCol, class  CCollider* pOtherCol);
	virtual void OnCollision_Stay(class  CCollider* pThisCol, class  CCollider* pOtherCol);
	virtual void OnCollision_Exit(class  CCollider* pThisCol, class  CCollider* pOtherCol);

public:
	virtual void PhysX_OnCollision_Enter(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo);
	virtual void PhysX_OnCollision_Stay(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo);
	virtual void PhysX_OnCollision_Exit(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo);

private:
	FTimeChecker	m_TimeChecker;
	FTimeChecker	m_TImeChecker_Limit_Always_Spark;
	FTimeChecker	m_TImeChecker_Limit_Always_Splash;

private:
	_bool			m_bAscensionMode = { false };
	_bool			m_bAlwaysParticleRepeat = { true };

private:
	shared_ptr<CParticle>	m_pAlwaysEffect = { nullptr };

	_uint m_iSocketBoneIndices[2];

private:
	_bool			m_bCheck = { true };

public:
	void Set_SocketBoneType(SOKETBONETYPE etype) { m_iBoneIndex = m_iSocketBoneIndices[etype];}
	GETSET_EX2(_bool, m_bAscensionMode, AscensionMode, GET, SET)

public:
	static shared_ptr<CCloud_Weapon> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	virtual void Free() override;
};

END