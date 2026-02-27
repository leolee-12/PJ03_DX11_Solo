#ifdef _DEBUG
#pragma once
#include "Imgui_Window.h"

BEGIN(Client)

class CImgui_Window_ModelEdit : public CImgui_Window
{
private:
	CImgui_Window_ModelEdit(vector< shared_ptr<CGameObject>>* pGameObjectList, ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual	~CImgui_Window_ModelEdit() = default;

public:
	virtual HRESULT Initialize();
	virtual void Tick(_cref_time fTimeDelta);
	virtual HRESULT Render();

private:
	class CImgui_Tab_BakeBinary* m_pBakeBinaryTab = { nullptr };
public:
	_bool	m_bSequencer = true;

public:
	static CImgui_Window_ModelEdit* Create(vector< shared_ptr<CGameObject>>* pGameObjectList, ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual void	Free() override;
};

END

#endif // DEBUG
