#pragma once	//	(T)
#include "CGameObject.h"

namespace Engine
{
	class CRcTex;
	class CTransform;
	class CTexture;
}

class CEffect : public CGameObject
{
private:
	explicit CEffect(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CEffect(const CGameObject& rhs);
	virtual ~CEffect();

public:
	HRESULT		Ready_GameObject() override;
	_int		Update_GameObject(const _float& fTimeDelta) override;
	void		LateUpdate_GameObject(const _float& fTimeDelta) override;
	void		Render_GameObject() override;

private:
	HRESULT			Add_Component();
private:
	Engine::CRcTex* m_pBufferCom;
	Engine::CTransform* m_pTransformCom;
	Engine::CTexture* m_pTextureCom;

	_float	m_fFrame;	// 스프라이트 넘기기용 변수

public:
	static CEffect* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	void Free() override;
};

