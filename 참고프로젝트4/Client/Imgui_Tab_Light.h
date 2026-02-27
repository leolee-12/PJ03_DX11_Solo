#ifdef _DEBUG
#pragma once
#include "Imgui_Tab.h"

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include "imgui_internal.h"
#include "ImGuizmo.h"

BEGIN(Engine)
class CLight;
END

BEGIN(Client)

class CImgui_Tab_Light : public CImgui_Tab
{
private:
	CImgui_Tab_Light(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual	~CImgui_Tab_Light() = default;

public:
	virtual HRESULT Initialize();
	virtual void	Tick(_cref_time fTimeDelta);
	virtual HRESULT Render();

	void		Set_LightData();
	void		Update_LightData();
	void		Save_Load_LightData();

	HRESULT		Create_Light(LIGHT_DESC& _LightDesc, shared_ptr<class CLight>* ppOut = nullptr);
	void		Use_ImGuizmo();

private:
#pragma region ImGuizmo Variable

	ImGuizmo::OPERATION mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
	bool useSnap = false;
	float snap[3] = { 1.f, 1.f, 1.f };

#pragma endregion

private:
	shared_ptr<class CLight>	m_pPickLight = nullptr;

	_int						m_iPickIndex = -1;
	_int						m_iCreateIndex = -1;
	_int						m_iCreateLayerType = { L_OBJECT };


	_int						m_iType = { 1 }; //Dir , Point, Spot, Symmetry, Shadow
	_bool						m_bCreateMode = false;
	_bool						m_bUpdateData = false;

	vector<shared_ptr<class CLight>>  m_LightList;
	shared_ptr<class CLight>	m_DirectionalLight;
	LIGHT_DESC					m_tLightDesc;
	string						m_strLightCatergoryName = "";
	_bool						m_bPickingCreateMode = false;

public:
	static CImgui_Tab_Light* Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual void	Free() override;
};

END

#endif // DEBUG
