#pragma once

#include "Client_Defines.h"
#include "PartObject.h"

NS_BEGIN(Engine)
class CCollider;
class CShader;
class CModel;
NS_END

NS_BEGIN(Client)

class CWeapon final : public CPartObject
{
public:
	typedef struct tagWeaponDesc final : public CPartObject::PART_OBJECT_DESC
	{
		const _float4x4* pSocketBoneMatrix = { nullptr };
		const _uint* pParentState = { nullptr };
	}WEAPON_DESC;

private:
	CWeapon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CWeapon(const CWeapon& Prototype);
	virtual ~CWeapon() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	const _uint* m_pParentState = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	CCollider* m_pColliderCom = { nullptr };
private:
	const _float4x4* m_pSocketBoneMatrix = { };


private:
	HRESULT Ready_Components();	
	HRESULT Bind_ShaderResources();

public:
	static CGameObject* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);/* 盔屈积己 */
	virtual CGameObject* Clone(void* pArg) override;/* 荤夯积己 */
	virtual void Free();

};

NS_END