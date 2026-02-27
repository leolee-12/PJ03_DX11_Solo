#ifdef _DEBUG
#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "GameInstance.h"
#include "Imgui_Manager.h"

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include "imgui_internal.h"
#include "ImGuizmo.h"

BEGIN(Client)

class CImgui_Window abstract : public CBase
{
protected:
	CImgui_Window(vector< shared_ptr<CGameObject>>* pGameObjectList, ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual	~CImgui_Window() = default;

public:
	virtual HRESULT Initialize();
	virtual void Tick(_cref_time fTimeDelta);
	virtual HRESULT Render();

protected:
	class CGameInstance* m_pGameInstance = { nullptr };
	ComPtr<ID3D11Device> m_pDevice = { nullptr };
	ComPtr<ID3D11DeviceContext> m_pContext = { nullptr };
	void* m_pDesc = { nullptr };
	vector< shared_ptr<CGameObject>>* m_pGameObjectList[LAYERTYPE_END];

protected:
	CImgui_Manager* m_pImgui_Manger = { nullptr };

	// ------ 기즈모 전용 ----------

public:
	void	Use_ImGuizmo(shared_ptr<CGameObject> pGameObject, _bool bModel);
	void	Change_Value(string szName, _float fValue, _float* fOrigin);

protected:
	_bool		m_bGridMode = false;
	_float		m_fAddArrow = { 0.f };
	_float3		m_vTesOffset = { 2.f,-1.f,1.f };
	_bool		m_bGizmo = { false };

#pragma region ImGuizmo Variable

protected:
	ImGuizmo::OPERATION mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
	bool useSnap = false;
	float snap[3] = { 1.f, 1.f, 1.f };

#pragma endregion

	// ---------------

public:
	virtual void	Free() override;
};

END
#endif // DEBUG
