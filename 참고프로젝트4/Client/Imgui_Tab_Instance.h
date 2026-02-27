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

class CImgui_Tab_Instance : public CImgui_MapToolBase
{
private:
	CImgui_Tab_Instance(vector< shared_ptr<CGameObject>>* pGameObjectList, ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual	~CImgui_Tab_Instance() = default;

public:
	virtual HRESULT Initialize();
	virtual void Tick(_cref_time fTimeDelta);
	virtual HRESULT Render();

private:
	_int			m_iPickInstanceIdx = 1;
	shared_ptr<CGameObject> m_pPickInstancing = nullptr;
	shared_ptr<CGameObject> m_pPickInstance;
	wstring		m_szInstanceName = L"Inst_";

	vector <wstring> m_vecInstanceTag;
	vector <shared_ptr<CGameObject>> m_InstancingList;

public:

	HRESULT		Create_Instance(vector<float4x4> vecInst, vector<shared_ptr<CGameObject>> pObject);

	void		RenderListBox();

	void		RenderObjectCreate();

	void		RenderInstanceList();

	void		RenderInstanceCreate();

	void		RenderErrorMsg();

	HRESULT		SaveLoadData();

	void		AutoInstancing();
public:
	static CImgui_Tab_Instance* Create(vector< shared_ptr<CGameObject>>* pGameObjectList, ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual void	Free() override;

};

END

#endif // DEBUG
