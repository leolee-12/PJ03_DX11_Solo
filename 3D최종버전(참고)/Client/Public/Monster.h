#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CCollider;
class CShader;
class CModel;
NS_END

NS_BEGIN(Client)

class CMonster final : public CGameObject
{
private:
	CMonster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMonster(const CMonster& Prototype);
	virtual ~CMonster() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CCollider*			m_pColliderCom[3] = { nullptr };
	CShader*			m_pShaderCom = { nullptr };
	CModel*				m_pModelCom = { nullptr };	
	
private:
	HRESULT Ready_Components();
	_bool Intersect_ToPlayer();

public:	
	static CGameObject* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);/* 盔屈积己 */
	virtual CGameObject* Clone(void* pArg) override;/* 荤夯积己 */ 
	virtual void Free();
};

NS_END