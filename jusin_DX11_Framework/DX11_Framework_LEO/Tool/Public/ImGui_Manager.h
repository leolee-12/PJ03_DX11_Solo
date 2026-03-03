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
	HRESULT		Initialize(HWND hWnd, _Inout_ ID3D11Device** ppDevice, _Inout_ ID3D11DeviceContext** ppContext, ID3D11RenderTargetView** ppBackBufferRTV);
	void		Update(_float fTimeDelta);
	void		Render();

private:
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };
	ID3D11RenderTargetView* m_pBackBufferRTV = { nullptr };

	vector<class CPanel_Base*>	m_vecPanels;

private:
	HRESULT		Add_Panels();

protected:
	virtual void	Free() override;
};

NS_END