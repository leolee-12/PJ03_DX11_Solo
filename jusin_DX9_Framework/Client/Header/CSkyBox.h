#pragma once	//	(P)
#include "CGameObject.h"

namespace Engine
{
	class CCubeTex;
	class CTransform;
	class CTexture;
}

class CSkyBox : public CGameObject
{
private:
	explicit CSkyBox(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CSkyBox(const CGameObject& rhs);
	virtual ~CSkyBox();

public:
	HRESULT		Ready_GameObject() override;
	_int		Update_GameObject(const _float& fTimeDelta) override;
	void		LateUpdate_GameObject(const _float& fTimeDelta) override;
	void		Render_GameObject() override;

private:
	HRESULT			Add_Component();

private:
	Engine::CCubeTex*	m_pBufferCom;
	Engine::CTransform* m_pTransformCom;
	Engine::CTexture*	m_pTextureCom;

public:
	static CSkyBox*	Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	void			Free() override;
};

