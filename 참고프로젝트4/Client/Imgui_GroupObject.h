#ifdef _DEBUG
#pragma once
#include "Imgui_MapToolBase.h"

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include "imgui_internal.h"
#include "ImGuizmo.h"

BEGIN(Engine)
class CCommonModelComp;
END

BEGIN(Client)

class CImgui_GroupObject : public CImgui_MapToolBase
{
private:
	CImgui_GroupObject(vector< shared_ptr<CGameObject>>* pGameObjectList, ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual	~CImgui_GroupObject() = default;

public:
	virtual HRESULT Initialize();
	virtual void Tick(_cref_time fTimeDelta);
	virtual HRESULT Render();

private:
	shared_ptr<CGameObject> m_pPickTabObject = nullptr;
	shared_ptr<CGameObject> m_pPickObject = nullptr;

	vector<shared_ptr<CGameObject>> m_GroupObjects;
	vector<pair<LAYERTYPE, shared_ptr<CGameObject>>> m_SaveObjects;

public:
	void	Set_SaveObjects(vector<pair<LAYERTYPE, shared_ptr<CGameObject>>> pSaveList) { m_SaveObjects = pSaveList; }

	void	RenderList();
	void	RenderAddOut();
	void	RenderSave();

public:
	HRESULT		GroupSave(const wstring& filePath);
	HRESULT		AllGroupSave(const wstring& filePath);

public:
	static CImgui_GroupObject* Create(vector< shared_ptr<CGameObject>>* pGameObjectList, ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual void	Free() override;


};

END

#endif // DEBUG
