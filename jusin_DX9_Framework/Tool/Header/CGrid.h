#pragma once
#include "CGameObject.h"

namespace Engine
{
	class CGridCol;
	class CTransform;
}

class CGrid : public CGameObject
{
private:
	explicit CGrid(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CGrid(const CGameObject& rhs);
	virtual ~CGrid();

public:
	HRESULT		Ready_GameObject() override;
	_int		Update_GameObject(const _float& fTimeDelta) override;
	void		LateUpdate_GameObject(const _float& fTimeDelta) override;
	void		Render_GameObject() override;

private:
	HRESULT		Add_Component();

private:
	//Engine::CGridCol* m_pBufferCom;
	Engine::CTransform* m_pTransformCom;

public:
	static CGrid*	Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	void			Free() override;
};

