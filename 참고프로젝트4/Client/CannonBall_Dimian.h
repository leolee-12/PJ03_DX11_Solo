#pragma once

#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Engine)
class CShader;
class CCollider;
END

BEGIN(Client)

class CCannonBall_Dimian final : public CPartObject
{
public:
	typedef struct tagCannonBallDesc : tagGameObjectDesc
	{
		CTransform* pTargetTransformCom = { nullptr };
	}CANNONBALL_DESC;

private:
	CCannonBall_Dimian(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCannonBall_Dimian(const CCannonBall_Dimian& rhs);
	virtual ~CCannonBall_Dimian() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual void OnCollision_Enter(CCollider* pThisCol, CCollider* pOtherCol);
	virtual void OnCollision_Stay(CCollider* pThisCol, CCollider* pOtherCol);
	virtual void OnCollision_Exit(CCollider* pThisCol, CCollider* pOtherCol);

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

private:
	_bool	m_bAttackEnable = { false };
	CTransform* m_pTargetTransformCom = { nullptr };
	class CParticle* m_pParticle[3] = { nullptr ,nullptr,nullptr };
	_float4 vTargetPosition = {};
public:
	void	Set_AttackEnable(_bool value) { m_bAttackEnable = value; };

public:
	/* 원형객체를 생성한다. */
	static CCannonBall_Dimian* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

	/* 사본객체를 생성한다. */
	virtual CGameObject* Clone(void* pArg) override;

	virtual void Free() override;
};

END