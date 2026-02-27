#ifdef _DEBUG
#pragma once
#include "Imgui_Tab.h"
//#include "ImGuizmo.h"

BEGIN(Engine)
class CCommonModelComp;
END

BEGIN(Client)

class CImgui_MapToolBase : public CImgui_Tab
{
protected:
	CImgui_MapToolBase(vector< shared_ptr<CGameObject>>* pGameObjectList, ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual	~CImgui_MapToolBase() = default;

protected:
	shared_ptr<CGameObject> m_pPickObject = nullptr;

	shared_ptr<CGameObject> m_pTestObject = nullptr;

	shared_ptr<class CGameObject> m_pCamera = nullptr;

	_int		 m_iPickIndex = 0;
	_int		 m_iCreateIndex = 0;

	_bool		m_bFillMode = true;
	//_bool		m_bGridMode = false;
	_bool		m_bCreateMode = false;
	_bool		m_bChoiceMode = false;

	_bool		m_bTestModelCreate = true;

	_int		m_iPickIndex_ProtoTag = 0;
	_int		m_iCreateLayerType = { L_OBJECT };
	_int		m_iModelFolder = { 0 };
	_int		m_iCurrentMode = { 0 };

	wstring		m_szFolder;
	wstring		m_szModelName;

	/*_float		m_fAddArrow = { 0.f };
	_float3		m_vTesOffset = { 2.f,-1.f,1.f };*/

	vector<float4x4> m_vecMatrix;

	vector<wstring> m_FolderNames;
	vector<pair<LAYERTYPE, shared_ptr<CGameObject>>> m_ObjectList;
	vector<wstring> m_ModelList;
	unordered_map<wstring, vector<wstring>> m_mapFolderList;

public:
	void Set_ObjectList(vector<pair<LAYERTYPE, shared_ptr<CGameObject>>> pGameObjectList) { m_ObjectList = pGameObjectList; }
	vector<pair<LAYERTYPE, shared_ptr<CGameObject>>> Get_ObjectList() { return m_ObjectList; }
	void Set_TestModel(shared_ptr<CGameObject> pObject) { m_pTestObject = pObject; }

public:
	//void		Use_ImGuizmo(shared_ptr<CGameObject> pGameObject,_bool bModel);

	void		OffsetTestModel();

	void		LoadFolder();

public:
	static CImgui_MapToolBase* Create(vector< shared_ptr<CGameObject>>* pGameObjectList, ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual void	Free() override;

	//#pragma region ImGuizmo Variable
	//
	//protected:
	//	ImGuizmo::OPERATION mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
	//	bool useSnap = false;
	//	float snap[3] = { 1.f, 1.f, 1.f };
	//	
	//#pragma endregion

};

END

#endif // DEBUG
