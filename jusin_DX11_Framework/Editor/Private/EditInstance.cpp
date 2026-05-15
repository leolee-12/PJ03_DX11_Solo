#include "EditInstance.h"
#include "ImGui_Manager.h"
#include "Panel_Viewport.h"
#include "Select_Manager.h"
#include "Editor_Serializer.h"
#include "Model_Loader.h"
#include "UIEditorSession.h"
#include "UIPreviewHost.h"

#include "GameInstance.h"
#include "Camera.h"

IMPLEMENT_SINGLETON(CEditInstance)

CEditInstance::CEditInstance()
	: m_pGameInstance(CGameInstance::GetInstance())
{
}

#pragma region ENGINE

HRESULT CEditInstance::Initialize_Editor(const ENGINE_DESC& EngineDesc, ID3D11Device** ppDevice, ID3D11DeviceContext** ppContext)
{
	m_pSelect_Manager = CSelect_Manager::Create();
	if (nullptr == m_pSelect_Manager)
		return E_FAIL;

	m_pUIEditorSession = CUIEditorSession::Create(*ppDevice, *ppContext);
	if (nullptr == m_pUIEditorSession)
		return E_FAIL;

	m_pImGui_Manager = CImGui_Manager::Create(*ppDevice, *ppContext, EngineDesc.hWnd);
	if (nullptr == m_pImGui_Manager)
		return E_FAIL;

	m_pUIPreviewHost = CUIPreviewHost::Create(*ppDevice, *ppContext);
	if (nullptr == m_pUIPreviewHost)
		return E_FAIL;

	m_pObject_Registry = CObject_Registry::Create();
	if (nullptr == m_pObject_Registry)
		return E_FAIL;

	m_pModel_Loader = CModel_Loader::Create();
	if (nullptr == m_pModel_Loader)
		return E_FAIL;

	return S_OK;
}

void CEditInstance::Update_Editor(_float fTimeDelta)
{
	if (m_pGameInstance->Key_Down(DIK_F1))
		m_bCameraEnabled = !m_bCameraEnabled;

	{	// Esc로 SCENE 복귀 (text input 중에는 무시)
		const ImGuiIO& io = ImGui::GetIO();
		if (!io.WantCaptureKeyboard
			&& m_pGameInstance->Key_Down(DIK_ESCAPE))
		{
			m_pUIEditorSession->Set_VPMode(
				CUIEditorSession::VPMODE::SCENE);
		}
	}

	_int iCurrLevel = m_pGameInstance->Get_CurrentLevel();
	if (m_iPrevLevel != iCurrLevel)
	{
		Clear();
		Sync_LevelObjects(static_cast<_uint>(iCurrLevel));
		m_iPrevLevel = iCurrLevel;
	}

	if (m_pUIPreviewHost)
		m_pUIPreviewHost->Tick(fTimeDelta);

	m_pImGui_Manager->Update(fTimeDelta);

	ImGuiIO& io = ImGui::GetIO();
	const _bool bFreeCameraInput = m_bCameraEnabled && !io.WantTextInput;

	if (bFreeCameraInput)
	{
		m_pGameInstance->Set_InputState(INPUT_STATE::GAMEPLAY);
	}
	else
	{
		m_pGameInstance->Set_InputState(INPUT_STATE::NAVIGATE);
	}

	if (CCamera* pMainCamera = m_pGameInstance->Get_MainCamera())
		pMainCamera->Set_ControlEnabled(bFreeCameraInput);
}

HRESULT CEditInstance::Draw()
{
	if (FAILED(m_pImGui_Manager->Render()))
		return E_FAIL;

	return S_OK;
}

void CEditInstance::Release_Editor()
{
	Safe_Release(m_pModel_Loader);
	Safe_Release(m_pObject_Registry);
	Safe_Release(m_pUIPreviewHost);
	Safe_Release(m_pImGui_Manager);
	Safe_Release(m_pUIEditorSession);
	Safe_Release(m_pSelect_Manager);

	DestroyInstance();
}
#pragma endregion

#pragma region UIEDITOR_SESSION & UIPREVIEW_HOST

#pragma endregion

#pragma region IMGUI_MANAGER
void CEditInstance::Begin_PlaceMode(const CATALOG_ITEM& tItem)
{
	m_pImGui_Manager->Begin_PlaceMode(tItem);
}

void CEditInstance::End_PlaceMode()
{
	m_pImGui_Manager->End_PlaceMode();
}

_bool CEditInstance::Is_PlaceMode() const
{
	return m_pImGui_Manager->Is_PlaceMode();
}

const CATALOG_ITEM& CEditInstance::Get_PlaceItem() const
{
	return m_pImGui_Manager->Get_PlaceItem();
}

HRESULT CEditInstance::Begin_ViewportRender()
{
	/*return m_pImGui_Manager->Get_ViewportPanel()->Begin_SceneRender();*/

	auto pViewport = m_pImGui_Manager->Get_ViewportPanel();
	if (FAILED(pViewport->Begin_SceneRender()))
		return E_FAIL;

	// RT가 active된 시점에서 위젯 재구성 (UIObject가 viewport size를 캡처하기 때문)
	if (m_pUIPreviewHost->Has_Rebuild_Pending())
		m_pUIPreviewHost->Process_Rebuild_If_Pending();   // 내부에서 Rebuild() 호출

	// Late_Update를 여기서 부르면 GameInstance::Draw가 RENDERID::UI 큐를 소비할 때 즉시 그려짐
	m_pUIPreviewHost->Render_Queue_Submit();
	return S_OK;
}

HRESULT CEditInstance::End_ViewportRender()
{
	return m_pImGui_Manager->Get_ViewportPanel()->End_SceneRender();
}

_bool CEditInstance::Is_NavEditMode() const
{
	return m_pImGui_Manager->Is_NavEditMode();
}

_bool CEditInstance::Is_NavPointMode() const
{
	return m_pImGui_Manager->Is_NavPointMode();
}

void  CEditInstance::Fire_NavClick(const _float3& vWorldPos)
{
	m_pImGui_Manager->Fire_NavClick(vWorldPos);
}

ImVec2 CEditInstance::Get_ViewportScreenPos()  const
{
	return m_pImGui_Manager->Get_ViewportScreenPos();
}

ImVec2 CEditInstance::Get_ViewportScreenSize() const
{
	return m_pImGui_Manager->Get_ViewportScreenSize();
}

_uint CEditInstance::Get_NavToolMode() const
{
	return m_pImGui_Manager->Get_NavToolMode();
}

void CEditInstance::Update_NavDragHit(const _float3& vWorldPos)
{
	m_pImGui_Manager->Update_NavDragHit(vWorldPos);
}

//_bool CEditInstance::Get_CurrentWorldHit(_float3* pOut) const
//{
//	return m_pImGui_Manager->Get_CurrentWorldHit(pOut);
//}
#pragma endregion

#pragma region SELECT_MANAGER
void CEditInstance::Select(CGameObject* pObj, bool bMultiSelect)
{
	m_pSelect_Manager->Select(pObj, bMultiSelect);
}

void CEditInstance::Deselect(CGameObject* pObj)
{
	m_pSelect_Manager->Deselect(pObj);
}

void CEditInstance::Clear()
{
	m_pSelect_Manager->Clear();
}

const vector<CGameObject*>& CEditInstance::Get_Selected() const
{
	return m_pSelect_Manager->Get_Selected();
}

CGameObject* CEditInstance::Get_Primary() const
{
	return m_pSelect_Manager->Get_Primary();
}

bool CEditInstance::Is_Selected(CGameObject* pObj) const
{
	return m_pSelect_Manager->Is_Selected(pObj);
}

void CEditInstance::Register_Callback(const _string& strKey, SelectionChangedCB cb)
{
	m_pSelect_Manager->Register_Callback(strKey, move(cb));
}

void CEditInstance::Unregister_Callback(const _string& strKey)
{
	m_pSelect_Manager->Unregister_Callback(strKey);
}
#pragma endregion

#pragma region OBJECT_REGISTRY
const vector<OBJ_RECORD>& CEditInstance::Get_Records() const
{
	return m_pObject_Registry->Get_Records();
}

const vector<CGameObject*>& CEditInstance::Get_EditorObjects() const
{
	return m_pObject_Registry->Get_EditorObjects();
}

const vector<EDITOR_OBJECT_ENTRY>& CEditInstance::Get_EditorObjectEntries() const
{
	return m_pObject_Registry->Get_EditorObjectEntries();
}

void CEditInstance::Register_Object(_uint iProtoLevel, WNameID strProtoTag, _uint iLayerLevel, WNameID strLayerTag, void* pArg)
{
	return m_pObject_Registry->Register_Object(iProtoLevel, strProtoTag, iLayerLevel, strLayerTag, pArg);
}

void CEditInstance::Unregister_Object(CGameObject* pObj)
{
	return m_pObject_Registry->Unregister_Object(pObj);
}

void CEditInstance::Clone_Object(CGameObject* pObj)
{
	return m_pObject_Registry->Clone_Object(pObj);
}

void CEditInstance::Sync_LevelObjects(_uint iLevel)
{
	m_pObject_Registry->Sync_LevelObjects(iLevel);
}
#pragma endregion

#pragma region EDITOR_SERIALIZER
HRESULT CEditInstance::Save_Map(const _string& strPath)
{
	return CEditor_Serializer::Save_Map(strPath, this);
}

HRESULT CEditInstance::Load_Map(const _string& strPath)
{
	return CEditor_Serializer::Load_Map(strPath, this);
}

HRESULT CEditInstance::Save_UISequence(const _string& strPath, const UISEQ_DOC& tDoc)
{
	return CEditor_Serializer::Save_UISequence(strPath, tDoc);
}

HRESULT CEditInstance::Load_UISequence(const _string& strPath, UISEQ_DOC& tDoc)
{
	return CEditor_Serializer::Load_UISequence(strPath, tDoc);
}

HRESULT CEditInstance::Save_EffectPreset(const _string& strPath, const vector<struct EFFECT_PRESET>& Presets)
{
	return CEditor_Serializer::Save_EffectPreset(strPath, Presets);
}

HRESULT CEditInstance::Load_EffectPreset(const _string& strPath, vector<struct EFFECT_PRESET>& Presets)
{
	return CEditor_Serializer::Load_EffectPreset(strPath, Presets);
}
#pragma endregion

#pragma region MODEL_LOADER
HRESULT XM_CALLCONV CEditInstance::Export_Binary(const _char* pFbxPath, const _char* pOutputPath, MODEL eType, _fmatrix PreTransform, const _char* pMappingJsonPath)
{
	return m_pModel_Loader->Export_Binary(pFbxPath, pOutputPath, eType, PreTransform, pMappingJsonPath);
}

HRESULT XM_CALLCONV CEditInstance::Export_JSON(const _char* pFbxPath, const _char* pOutputPath, MODEL eType, _fmatrix PreTransform, _uint iVertexSampleCount)
{
	return m_pModel_Loader->Export_JSON(pFbxPath, pOutputPath, eType, PreTransform, iVertexSampleCount);
}

HRESULT XM_CALLCONV CEditInstance::Export_All(const _char* pFbxPath, const _char* pOutputDir, MODEL eType, _fmatrix PreTransform, const _char* pMappingJsonPath)
{
	return m_pModel_Loader->Export_All(pFbxPath, pOutputDir, eType, PreTransform, pMappingJsonPath);
}

HRESULT XM_CALLCONV CEditInstance::Load_FBX(const _char* pFbxPath, MODEL eType, _fmatrix PreTransform)
{
	return m_pModel_Loader->Load_FBX(pFbxPath, eType, PreTransform);
}

HRESULT CEditInstance::Generate_MappingJSON(const _char* pTexDir, const _char* pOutputPath)
{
	return m_pModel_Loader->Generate_MappingJSON(pTexDir, pOutputPath);
}

_bool CEditInstance::Is_ModelLoaded() const
{
	return m_pModel_Loader->Is_ModelLoaded();
}

const _char* CEditInstance::Get_FbxPath() const
{
	return m_pModel_Loader->Get_FbxPath();
}

const WMODEL_HEADER& CEditInstance::Get_ModelMetaData() const
{
	return m_pModel_Loader->Get_ModelMetaData();
}

const vector<WMODEL_BONE>& CEditInstance::Get_ModelBones() const
{
	return m_pModel_Loader->Get_ModelBones();
}
#pragma endregion

#pragma region 6

#pragma endregion

#pragma region 7

#pragma endregion

#pragma region 8

#pragma endregion

void CEditInstance::Free()
{
	__super::Free();
}