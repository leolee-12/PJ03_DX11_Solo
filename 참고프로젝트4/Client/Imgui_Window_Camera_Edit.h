#ifdef _DEBUG

#pragma once
#include "Imgui_Window.h"

BEGIN(Client)

class CImgui_Window_Camera_Edit : public CImgui_Window
{
private:
	CImgui_Window_Camera_Edit(vector< shared_ptr<CGameObject>>* pGameObjectList, ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual	~CImgui_Window_Camera_Edit() = default;

public:
	virtual HRESULT Initialize();
	virtual void Tick(_cref_time fTimeDelta);
	virtual HRESULT Render();

private:
	class CImgui_Tab_Camera_Setting* m_pCameraSetTab = { nullptr };
	class CImgui_Tab_Camera_Target_Edit* m_pCameraTargetSetTab = { nullptr };

public:
	_bool	m_bCameraSetTabOn = true;
	_bool	m_bCameraTargetSetTabOn = true;

public:
	static CImgui_Window_Camera_Edit* Create(vector< shared_ptr<CGameObject>>* pGameObjectList, ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual void	Free() override;
};

END
#endif // DEBUG
