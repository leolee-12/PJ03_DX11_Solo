#pragma once
#include "Game_PKM_Defines.h"
#include "PartObject.h"

NS_BEGIN(Engine)
class CCollider;
class CShader;
class CModel;
NS_END

NS_BEGIN(Game_PKM)

class CWeapon final : public CPartObject
{
public:
	struct WEAPON_DESC : public CPartObject::PARTOBJECT_DESC
	{
		const _float4x4* pSocketBoneMatrix = { nullptr };
		const _uint* pParentState = { nullptr };
	};

protected:
	CWeapon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CWeapon(const CWeapon& Prototype);
	virtual ~CWeapon() = default;

public:
	class Engine::CModel* Get_Model() const { return m_pModelCom; }

	virtual _string Get_TypeName() const { return "Weapon"; }
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CCollider* m_pColliderCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };

	const _float4x4* m_pSocketBoneMatrix = { nullptr };
	const _uint* m_pParentState = { nullptr };

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();


public:
	static CWeapon* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free();
};

NS_END