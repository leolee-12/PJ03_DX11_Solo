#pragma once
#include "CBase.h"
#include "CGraphicDev.h"
#include "CTimerMgr.h"
#include "CFrameMgr.h"
#include "CManagement.h"

class CToolApp : public CBase
{
private:
	explicit	CToolApp();
	virtual		~CToolApp();

public:
	HRESULT		Ready_ToolApp();
	int			Update_ToolApp(const float& fTimeDelta);
	void		LateUpdate_ToolApp(const float& fTimeDelta);
	void		Render_ToolApp();

private:
	HRESULT		Ready_DefaultSetting(LPDIRECT3DDEVICE9* ppGraphicDev);
	HRESULT		Ready_Scene(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	LPDIRECT3DDEVICE9		m_pGraphicDev;
	Engine::CGraphicDev*	m_pDeviceClass;
	Engine::CManagement*	m_pManagementClass;

public:
	static CToolApp* Create();

private:
	void	Free() override;
};

