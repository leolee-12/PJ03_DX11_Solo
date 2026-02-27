#ifdef _DEBUG

#pragma once
#include "Imgui_Window.h"

BEGIN(Client)

class CImgui_Window_ShaderEdit : public CImgui_Window
{
private:
	CImgui_Window_ShaderEdit(vector< shared_ptr<CGameObject>>* pGameObjectList, ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual	~CImgui_Window_ShaderEdit() = default;

public:
	virtual HRESULT Initialize();
	virtual void Tick(_cref_time fTimeDelta);
	virtual HRESULT Render();

public:
	_bool	m_bTexEditTabOn = true;
	_bool	m_bMotionEditTabOn = true;
	_bool	m_bWindowEditTabOn = true;

	class CImgui_Tab_ShaderEdit* m_pShaderEditTab = nullptr;

public:
	static CImgui_Window_ShaderEdit* Create(vector< shared_ptr<CGameObject>>* pGameObjectList, ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual void	Free() override;
};

END

#endif // DEBUG
