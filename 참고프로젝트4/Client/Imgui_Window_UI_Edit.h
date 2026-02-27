#ifdef _DEBUG

#pragma once
#include "Imgui_Window.h"

BEGIN(Client)

class CImgui_Window_UI_Edit : public CImgui_Window
{
private:
	CImgui_Window_UI_Edit(vector< shared_ptr<CGameObject>>* pGameObjectList, ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual	~CImgui_Window_UI_Edit() = default;

public:
	virtual HRESULT Initialize();
	virtual void Tick(_cref_time fTimeDelta);
	virtual HRESULT Render();

	class CImgui_Tab_UI_TexEdit* Get_UITex_Tab() { return m_pUITexEditTab; }

private:
	class CImgui_Tab_UI_TexEdit* m_pUITexEditTab = { nullptr };
	class CImgui_Tab_UI_MotionEdit* m_pUIMotionEditTab = { nullptr };
	class CImgui_Tab_UI_WindowEdit* m_pUIWindowEditTab = { nullptr };

public:
	_bool	m_bTexEditTabOn = true;
	_bool	m_bMotionEditTabOn = true;
	_bool	m_bWindowEditTabOn = true;

	_bool	m_MotionReset = false;

public:
	static CImgui_Window_UI_Edit* Create(vector< shared_ptr<CGameObject>>* pGameObjectList, ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual void	Free() override;
};

END

#endif // DEBUG
