#pragma once
#include "CBase.h"
#include "Engine_Define.h"

BEGIN(Engine)

class CGameObject;

class ENGINE_DLL CComponent : public CBase
{
protected:
	explicit CComponent();
	explicit CComponent(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CComponent(const CComponent& rhs);
	virtual ~CComponent();

public:
	// === Setter/Getter ===
	void Set_Owner(CGameObject* pOwner) { m_pOwner = pOwner; }
	CGameObject* Get_Owner() const { return m_pOwner; }

	virtual _int Update_Component(const _float& fTimeDelta) { return 0; }
	virtual void LateUpdate_Component() { }

protected:
	LPDIRECT3DDEVICE9			m_pGraphicDev;
	_bool						m_bClone;
	CGameObject*				m_pOwner;

public:
	virtual CComponent* Clone()	PURE;

protected:
	virtual void				Free();
};

END