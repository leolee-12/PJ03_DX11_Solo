#pragma once
#include "GameObject.h"
#include "Editor_Defines.h"

NS_BEGIN(Editor)

class CImGui_Manager : public CBase
{
private:
	CImGui_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CImGui_Manager() = default;

public:
	HRESULT Initialize(HWND hWnd);
	void Update(_float fTimeDelta);
	HRESULT Render();

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

	array<class CPanel_Base*, PANEL_COUNT> m_Panels{};

private:
	HRESULT Add_Panels();

public:
	static CImGui_Manager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, HWND hWnd);

private:
	virtual void Free() override;
};

NS_END