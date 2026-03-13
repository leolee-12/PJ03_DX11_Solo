#pragma once
#include "CScene.h"
#include "CCamera.h"
#include "CGrid.h"

class CEditScene : public CScene
{
private:
	explicit	CEditScene(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual		~CEditScene();

public:
	HRESULT		Ready_Scene() override;
	_int		Update_Scene(const _float& fTimeDelta) override;
	void		LateUpdate_Scene(const _float& fTimeDelta) override;
	void		Render_Scene() override;

	CCamera*	Get_Camera() { return m_pCamera; }
	CGrid*		Get_Grid() { return m_pGrid; }

private:
	HRESULT		Ready_Environment_Layer(const _tchar* pLayerTag);
	HRESULT		Ready_GameLogic_Layer(const _tchar* pLayerTag);
	HRESULT		Ready_Camera_Layer(const _tchar* pLayerTag);
	HRESULT		Ready_UI_Layer(const _tchar* pLayerTag);

	HRESULT		Ready_Light();

	CCamera*	m_pCamera;
	CGrid*		m_pGrid;

public:
	static CEditScene* Create(LPDIRECT3DDEVICE9 pGraphicDev);
private:
	virtual void Free();

};

