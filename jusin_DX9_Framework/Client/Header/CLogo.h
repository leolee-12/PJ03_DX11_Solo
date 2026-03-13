#pragma once
#include "CScene.h"
#include "CLoading.h"

class CLogo : public CScene
{
private:
	explicit CLogo(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CLogo();

public:
	virtual			HRESULT		Ready_Scene();
	virtual			_int		Update_Scene(const _float& fTimeDelta);
	virtual			void		LateUpdate_Scene(const _float& fTimeDelta);
	virtual			void		Render_Scene();

private:
	HRESULT			Ready_Environment_Layer(const _tchar* pLayerTag);
	HRESULT			Ready_GameLogic_Layer(const _tchar* pLayerTag) { return S_OK; }
	HRESULT			Ready_UI_Layer(const _tchar* pLayerTag) { return S_OK; }

private:
	HRESULT			Ready_Prototype();

private:
	CLoading*		m_pLoading;

public:
	static CLogo* Create(LPDIRECT3DDEVICE9 pGraphicDev);
private:
	virtual void Free();

};

