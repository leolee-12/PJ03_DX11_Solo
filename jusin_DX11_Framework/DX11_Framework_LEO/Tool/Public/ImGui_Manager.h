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
	HRESULT		Ready_ImGui(HWND hWnd, _Inout_ ID3D11Device** ppDevice, _Inout_ ID3D11DeviceContext** ppContext);
	void		Priority_Update();
	void		Update();
	void		Late_Update();
	void		Render();

	void		Update_Example();
	void		Update_Main();
	void		Update_MapTool();
	void		Update_ObjectTool();
	void		Update_UITool();
	void		Update_EffectTool();
	void		Update_Gizmo();

private:
	ID3D11Device*			m_pDevice = { nullptr };			// COM按眉 积己 包访
	ID3D11DeviceContext*	m_pContext = { nullptr };			// COM按眉 扁瓷 包访
	EDITOR_MODE				m_eCurMode = { };

private:
	void Free() override;

};

NS_END