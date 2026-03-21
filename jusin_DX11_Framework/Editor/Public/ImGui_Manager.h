#pragma once
#include "Base.h"
#include "Editor_Defines.h"

NS_BEGIN(Editor)

class CImGui_Manager : public CBase
{
	DECLARE_SINGLETON(CImGui_Manager)

private:
	CImGui_Manager();
	virtual ~CImGui_Manager() = default;

public:
	HRESULT Initialize(HWND hWnd, _Inout_ ID3D11Device** ppDevice, _Inout_ ID3D11DeviceContext** ppContext);
	void Update(_float fTimeDelta);
	void Render();

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	ID3D11RenderTargetView* m_pBackBufferRTV = { nullptr };
	ID3D11DepthStencilView* m_pDepthStencilView = { nullptr };

	array<class CPanel_Base*, EDITOR_MODE_COUNT> m_Panels{};

private:
	HRESULT Add_Panels();

protected:
	virtual void Free() override;
};

NS_END