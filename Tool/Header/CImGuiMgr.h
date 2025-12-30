#pragma once
#include "CBase.h"
#include "Engine_Define.h"
#include "ImGuizmo.h"

class CImGuiMgr : public CBase
{
	DECLARE_SINGLETON(CImGuiMgr)

private:
	explicit CImGuiMgr();
	virtual ~CImGuiMgr();

public:
	HRESULT		Ready_ImGui(HWND hWnd, LPDIRECT3DDEVICE9 pGraphicDev);
	void		PriorityUpdate_ImGui();
	void		Update_ImGui();
	void		LateUpdate_ImGui();
	void		Render_ImGui();

	void		Render_Main();
	void		Render_MapTool();
	void		Render_ObjTool();
	void		Render_EffectTool();
	void		Render_Gizmo();
private:
	LPDIRECT3DDEVICE9	m_pGraphicDev;
	_bool				m_bDemo;
	_bool				m_bMapTool;
	_bool				m_bObjTool;
	_bool				m_bEffectTool;

private:
	void Free() override;

};