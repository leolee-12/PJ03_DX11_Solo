#pragma once
#include "Base.h"
#include "Tool_Defines.h"

NS_BEGIN(Tool)

class CImGui_Manager : public CBase
{
	DECLARE_SINGLETON(CImGui_Manager)

private:
	CImGui_Manager();
	virtual ~CImGui_Manager() = default;

public:
	HRESULT		Ready_ImGui(HWND hWnd, _Inout_ ID3D11Device** ppDevice, _Inout_ ID3D11DeviceContext** ppContext, ID3D11RenderTargetView** ppBackBufferRTV);
	void		Priority_Update(_float fTimeDelta);
	void		Update(_float fTimeDelta);
	void		Late_Update(_float fTimeDelta);
	void		Render();

private:
	void		Update_Example(_float fTimeDelta);
	void		Update_Main(_float fTimeDelta);
	void		Update_MapTool(_float fTimeDelta);
	void		Update_ObjectTool(_float fTimeDelta);
	void		Update_UITool(_float fTimeDelta);
	void		Update_EffectTool(_float fTimeDelta);
	void		Update_Gizmo(_float fTimeDelta);

private:
	ID3D11Device*			m_pDevice = { nullptr };			// COM按眉 积己 包访
	ID3D11DeviceContext*	m_pContext = { nullptr };			// COM按眉 扁瓷 包访
	ID3D11RenderTargetView* m_pBackBufferRTV = { nullptr };
	EDITOR_MODE				m_eCurMode = { };
	_bool					m_bAnother_Window = { false };
	ImVec4					m_vClear_color = { };


private:
	void Free() override;

};

NS_END