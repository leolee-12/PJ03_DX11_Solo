#ifdef _DEBUG
#pragma once
#include "Imgui_Window.h"

BEGIN(Client)

class CImgui_Window_ModelAnim : public CImgui_Window
{
private:
	CImgui_Window_ModelAnim(vector< shared_ptr<CGameObject>>* pGameObjectList, ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual	~CImgui_Window_ModelAnim() = default;

public:
	virtual HRESULT Initialize(class CImgui_Tab_ParticleEdit* pParticleTab, class CImgui_Tab_TrailEdit* pTrailTab);
	virtual void Tick(_cref_time fTimeDelta);
	virtual HRESULT Render();

private:
	class CImgui_Tab_ModelAnim* m_pModelEditTab = { nullptr };
public:
	_bool	m_bModelEditTabOn = true;

public:
	static CImgui_Window_ModelAnim* Create(vector< shared_ptr<CGameObject>>* pGameObjectList, ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, CImgui_Tab_ParticleEdit* pParticleTab, CImgui_Tab_TrailEdit* pTrailTab);
	virtual void	Free() override;
};

END

#endif // DEBUG
