#include "EditInstance.h"

#include "GameInstance.h"
#include "ImGui_Manager.h"
#include "Select_Manager.h"
#include "Editor_Serializer.h"
#include "Model_Loader.h"
#include "Panel_Viewport.h"

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

	m_pImGui_Manager = CImGui_Manager::Create(*ppDevice, *ppContext, EngineDesc.hWnd);
	if (nullptr == m_pImGui_Manager)
		return E_FAIL;

	m_pObject_Registry = CObject_Registry::Create();
	if (nullptr == m_pObject_Registry)
		return E_FAIL;

	m_pModel_Loader = CModel_Loader::Create();
	if (nullptr == m_pObject_Registry)
		return E_FAIL;

	return S_OK;
}

void CEditInstance::Update_Editor(_float fTimeDelta)
{
	_int iCurrLevel = m_pGameInstance->Get_CurrentLevel();

	if (m_iPrevLevel != iCurrLevel)
	{
		Clear();
		m_iPrevLevel = iCurrLevel;
	}

	m_pImGui_Manager->Update(fTimeDelta);

	// ImGui 포커스 여부에 따라 입력 상태 전환
	ImGuiIO& io = ImGui::GetIO();
	const _bool bViewportActive = m_pImGui_Manager->Is_ViewportActive();
	const _bool bOtherPanelActive = m_pImGui_Manager->Is_AnyNonViewportPanelActive();

	if (io.WantTextInput)
	{
		m_pGameInstance->Set_InputState(INPUT_STATE::NAVIGATE);	// 텍스트 입력 중 → WASD만 차단
	}
	else if (bOtherPanelActive)
	{
		m_pGameInstance->Set_InputState(INPUT_STATE::LOCKED);	// 패널 위 → 카메라 포함 차단
	}
	else if (bViewportActive)
	{
		// Viewport 패널 조작 시에는 ImGui가 마우스를 캡처하더라도 게임 로직 입력 막지 않음
		m_pGameInstance->Set_InputState(INPUT_STATE::GAMEPLAY);
	}
	else if (io.WantCaptureMouse || io.WantCaptureKeyboard)
	{
		m_pGameInstance->Set_InputState(INPUT_STATE::LOCKED);
	}
	else
	{
		m_pGameInstance->Set_InputState(INPUT_STATE::GAMEPLAY);
	}
}

HRESULT CEditInstance::Draw()
{
	if (FAILED(m_pImGui_Manager->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CEditInstance::Begin_ViewportRender()
{
	if (nullptr == m_pImGui_Manager)
		return E_FAIL;

	CPanel_Viewport* pPanel = m_pImGui_Manager->Get_ViewportPanel();
	if (nullptr == pPanel)
		return E_FAIL;

	return pPanel->Begin_SceneRender();
}

HRESULT CEditInstance::End_ViewportRender()
{
	if (nullptr == m_pImGui_Manager)
		return E_FAIL;

	CPanel_Viewport* pPanel = m_pImGui_Manager->Get_ViewportPanel();
	if (nullptr == pPanel)
		return E_FAIL;

	return pPanel->End_SceneRender();
}

void CEditInstance::Release_Editor()
{
	Safe_Release(m_pModel_Loader);
	Safe_Release(m_pObject_Registry);
	Safe_Release(m_pImGui_Manager);
	Safe_Release(m_pSelect_Manager);

	DestroyInstance();
}

#pragma endregion

#pragma region IMGUI_MANAGER

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

HRESULT CEditInstance::Save_UILayout(const _string& strPath, const vector<struct UI_ELEMENT>& Elements)
{
	return CEditor_Serializer::Save_UILayout(strPath, Elements);
}

HRESULT CEditInstance::Load_UILayout(const _string& strPath, vector<struct UI_ELEMENT>& Elements)
{
	return CEditor_Serializer::Load_UILayout(strPath, Elements);
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
HRESULT XM_CALLCONV CEditInstance::Export_Binary(const _char* pFbxPath, const _char* pOutputPath, MODEL eType, _fmatrix PreTransform)
{
	return m_pModel_Loader->Export_Binary(pFbxPath, pOutputPath, eType, PreTransform);
}

HRESULT XM_CALLCONV CEditInstance::Export_JSON(const _char* pFbxPath, const _char* pOutputPath, MODEL eType, _fmatrix PreTransform, _uint iVertexSampleCount)
{
	return m_pModel_Loader->Export_JSON(pFbxPath, pOutputPath, eType, PreTransform, iVertexSampleCount);
}

HRESULT XM_CALLCONV CEditInstance::Export_All(const _char* pFbxPath, const _char* pOutputDir, MODEL eType, _fmatrix PreTransform)
{
	return m_pModel_Loader->Export_All(pFbxPath, pOutputDir, eType, PreTransform);
}

HRESULT XM_CALLCONV CEditInstance::Load_FBX(const _char* pFbxPath, MODEL eType, _fmatrix PreTransform)
{
	return m_pModel_Loader->Load_FBX(pFbxPath, eType, PreTransform);
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