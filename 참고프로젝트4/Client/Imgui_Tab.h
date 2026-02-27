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

class CImgui_Tab abstract : public CBase
{
protected:
	CImgui_Tab(vector< shared_ptr<CGameObject>>* pGameObjectList, ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CImgui_Tab(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual	~CImgui_Tab() = default;

public:
	virtual HRESULT Initialize();
	virtual void Tick(_cref_time fTimeDelta);
	virtual HRESULT Render();

public:
	void	Use_ImGuizmo(shared_ptr<CGameObject> pGameObject, _bool bModel);
	void	Use_ImGuizmo(_float4x4& Transform, _bool bModel);
	void	Change_Value(string szName, _float fValue, _float* fOrigin);
	//지호 추가
protected:
	void HelpMarker(const char* desc);

protected:
	class CGameInstance* m_pGameInstance = { nullptr };
	void* m_pDesc = { nullptr };
	ComPtr<ID3D11Device> m_pDevice = { nullptr };
	ComPtr<ID3D11DeviceContext> m_pContext = { nullptr };
	CImgui_Manager* m_pImgui_Manger = { nullptr };
	vector< shared_ptr<CGameObject>>* m_pGameObjectList[LAYERTYPE_END];

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


#pragma region 컬러

protected:
	/*_bool		m_bAlpha = { false };
	_bool		m_bAlpha_bar = { false };
	_bool		m_bAlpha = { false };*/

#pragma endregion

public:
	virtual void	Free() override;
};

END
#endif // DEBUG
