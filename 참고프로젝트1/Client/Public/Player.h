#pragma once

#include "Client_Defines.h"
#include "ContainerObject.h"

NS_BEGIN(Engine)
class CNavigation;
class CCollider;
NS_END

NS_BEGIN(Client)

class CPlayer final : public CContainerObject
{
public:
	enum class PARTOBJ { BODY, WEAPON, EFFECT, END };
	enum STATE { 
		IDLE = 0x00000001,		/* 0001 */
		RUN = 0x00000002,		/* 0010 */
		ATTACK = 0x00000004,	/* 0100 */
		END };

private:
	CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer(const CPlayer& Prototype);
	virtual ~CPlayer() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	_uint				m_iState = {};
	CNavigation*		m_pNavigationCom = { nullptr };
	CCollider*			m_pColliderCom = { nullptr };
private:
	HRESULT Ready_Components();
	HRESULT Ready_PartObjects();

public:
	static CGameObject* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);/* 盔屈积己 */
	virtual CGameObject* Clone(void* pArg) override;/* 荤夯积己 */
	virtual void Free();

};

NS_END