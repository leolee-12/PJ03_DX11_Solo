#pragma once
#include "Editor_Defines.h"
#include "Object_Registry.h"

/* ------------------------------------------------------------ */
// CEditInstance : 에디터 기능 간 오케스트레이션 및 총괄 관리
/* ------------------------------------------------------------ */

NS_BEGIN(Editor)

class CEditInstance final : public CBase
{
	DECLARE_SINGLETON(CEditInstance)

private:
	CEditInstance();
	virtual ~CEditInstance() = default;

public:
#pragma region EDITOR
	HRESULT Initialize_Editor(const ENGINE_DESC& EngineDesc, ID3D11Device** ppDevice, ID3D11DeviceContext** ppContext);
	void Update_Editor(_float fTimeDelta);
	HRESULT Draw();
	HRESULT Begin_ViewportRender();
	HRESULT End_ViewportRender();
	void Release_Editor();

	_bool Is_CameraEnabled() const { return m_bCameraEnabled; }
	void Toggle_Camera() { m_bCameraEnabled = !m_bCameraEnabled; }
	void Set_CameraEnabled(_bool b) { m_bCameraEnabled = b; }
#pragma endregion

#pragma region UIEDITOR_SESSION & UIPREVIEW_HOST
	class CUIEditorSession* Get_UISession() const { return m_pUIEditorSession; }
	class CUIPreviewHost* Get_UIPreviewHost() const { return m_pUIPreviewHost; }
#pragma endregion

#pragma region IMGUI_MANAGER
	void Begin_PlaceMode(const CATALOG_ITEM& tItem);
	void End_PlaceMode();
	_bool Is_PlaceMode() const;
	const CATALOG_ITEM& Get_PlaceItem() const;

	_bool Is_NavEditMode() const;
	_bool Is_NavPointMode() const;
	void Fire_NavClick(const _float3& vWorldPos);
	ImVec2 Get_ViewportScreenPos() const;
	ImVec2 Get_ViewportScreenSize() const;

	_uint Get_NavToolMode() const;
	void Update_NavDragHit(const _float3& vWorldPos);
	//_bool Get_CurrentWorldHit(_float3* pOut) const;
#pragma endregion

#pragma region SELECT_MANAGER
	void Select(CGameObject* pObj, bool bMultiSelect = false);
	void Deselect(CGameObject* pObj);
	void Clear();

	const vector<CGameObject*>& Get_Selected() const;
	CGameObject* Get_Primary() const;
	bool Is_Selected(CGameObject* pObj) const;

	void Register_Callback(const _string& strKey, SelectionChangedCB cb);
	void Unregister_Callback(const _string& strKey);
#pragma endregion

#pragma region OBJECT_REGISTRY
	const vector<OBJ_RECORD>& Get_Records() const;
	const vector<CGameObject*>& Get_EditorObjects() const;
	const vector<EDITOR_OBJECT_ENTRY>& Get_EditorObjectEntries() const;
	void Register_Object(_uint iProtoLevel, WNameID strProtoTag, _uint iLayerLevel, WNameID strLayerTag, void* pArg);
	void Unregister_Object(CGameObject* pObj);
	void Clone_Object(CGameObject* pObj);
	void Sync_LevelObjects(_uint iLevel);
#pragma endregion

#pragma region EDITOR_SERIALIZER
	HRESULT Save_Map(const _string& strPath);
	HRESULT Load_Map(const _string& strPath);

	HRESULT Save_UISequence(const _string& strPath, const UISEQ_DOC& tDoc);
	HRESULT Load_UISequence(const _string& strPath, UISEQ_DOC& tDoc);

	HRESULT Save_EffectPreset(const _string& strPath, const vector<struct EFFECT_PRESET>& Presets);
	HRESULT Load_EffectPreset(const _string& strPath, vector<struct EFFECT_PRESET>& Presets);
#pragma endregion

#pragma region MODEL_LOADER
	HRESULT XM_CALLCONV Export_Binary(const _char* pFbxPath, const _char* pOutputPath,
		MODEL eType, _fmatrix PreTransform, const _char* pMappingJsonPath = nullptr);
	HRESULT XM_CALLCONV Export_JSON(const _char* pFbxPath, const _char* pOutputPath,
		MODEL eType, _fmatrix PreTransform, _uint iVertexSampleCount = 3);
	HRESULT XM_CALLCONV Export_All(const _char* pFbxPath, const _char* pOutputDir,
		MODEL eType, _fmatrix PreTransform, const _char* pMappingJsonPath = nullptr);
	HRESULT XM_CALLCONV Load_FBX(const _char* pFbxPath, MODEL eType, _fmatrix PreTransform);
	HRESULT Generate_MappingJSON(const _char* pTexDir, const _char* pOutputPath);

	_bool Is_ModelLoaded() const;
	const _char* Get_FbxPath() const;
	const WMODEL_HEADER& Get_ModelMetaData() const;
	const vector<WMODEL_BONE>& Get_ModelBones() const;
#pragma endregion

#pragma region 6

#pragma endregion

#pragma region 7

#pragma endregion

#pragma region 8

#pragma endregion

private:
	_int m_iPrevLevel = { -1 };
	_bool m_bCameraEnabled = { false };

	class CGameInstance* m_pGameInstance = { nullptr };
	class CUIEditorSession* m_pUIEditorSession = { nullptr };
	class CImGui_Manager* m_pImGui_Manager = { nullptr };
	class CUIPreviewHost* m_pUIPreviewHost = { nullptr };
	class CSelect_Manager* m_pSelect_Manager = { nullptr };
	class CObject_Registry* m_pObject_Registry = { nullptr };
	class CModel_Loader* m_pModel_Loader = { nullptr };

private:
	virtual void Free() override;
};

NS_END