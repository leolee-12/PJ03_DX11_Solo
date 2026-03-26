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
	HRESULT Initialize(HWND hWnd, ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void Update(_float fTimeDelta);
	void Render();

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

	array<class CPanel_Base*, EDITOR_MODE_COUNT> m_Panels{};

private:
	HRESULT Add_Panels();

protected:
	virtual void Free() override;
};

NS_END