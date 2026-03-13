#pragma once
#include "CGameObject.h"

class CBaseTool : public CGameObject
{
protected:
	explicit	CBaseTool();
	explicit	CBaseTool(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit	CBaseTool(const CBaseTool& rhs);	// 프로토타입 복사용
	virtual ~CBaseTool() = default;

public:
	virtual		HRESULT		Ready_ProtoType() { return S_OK; }
	virtual		HRESULT		Ready_GameObject(void* pArg) { return S_OK; }
	virtual		void		PriorityUpdate_Tool(const _float& fTimeDelta) {}
	virtual		_int		Update_GameObject(const _float& fTimeDelta) { return 0; }
	virtual		void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual		HRESULT		Render_Tool();

protected:
	void		Free()		override;
};