#include "EditorApp.h"
#include "GameInstance.h"
#include "ImGui_Manager.h"
#include "Level_EditLoading.h"

CEditorApp::CEditorApp()
	: m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CEditorApp::Initialize()
{
	ENGINE_DESC	EngineDesc{};
	EngineDesc.hWnd = g_hWnd;
	EngineDesc.eWinMode = WINMODE::WIN;
	EngineDesc.iViewportWidth = g_iWinSizeX;
	EngineDesc.iViewportHeight = g_iWinSizeY;
	EngineDesc.iNumLevels = ETOUI(LEVEL::END);

	if (FAILED(m_pGameInstance->Initialize_Engine(EngineDesc, &m_pDevice, &m_pContext)))
	{
		MSG_BOX("Failed to Initialize : Engine");
		return E_FAIL;
	}

	if (FAILED(CImGui_Manager::GetInstance()->Initialize(g_hWnd, &m_pDevice, &m_pContext, m_pGameInstance->Get_BackBufferRTV())))
	{
		MSG_BOX("ImGui Ready Failed");
		return E_FAIL;
	}

	if (FAILED(Start_Level(LEVEL::EDITLOGO)))
		return E_FAIL;

	return S_OK;
}

void CEditorApp::Update(_float fTimeDelta)
{
	m_pGameInstance->Update_Engine(fTimeDelta);

	CImGui_Manager::GetInstance()->Update(fTimeDelta);
}

HRESULT CEditorApp::Render()
{
	if (FAILED(m_pGameInstance->Begin_Draw()))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Draw()))
		return E_FAIL;

	CImGui_Manager::GetInstance()->Render();

	if (FAILED(m_pGameInstance->End_Draw()))
		return E_FAIL;

	return S_OK;
}

HRESULT CEditorApp::Start_Level(LEVEL eStartLevelID)
{
	CLevel* pPreLevel = CLevel_EditLoading::Create(m_pDevice, m_pContext, eStartLevelID);
	
	if (nullptr == pPreLevel)
		return E_FAIL;
	
	if (FAILED(m_pGameInstance->Change_Level(ETOI(LEVEL::LOADING), pPreLevel)))
		return E_FAIL;

	return S_OK;
}

CEditorApp* CEditorApp::Create()
{
	CEditorApp* pInstance = new CEditorApp;

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Create : CMainApp");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEditorApp::Free()
{
	__super::Free();

	CImGui_Manager::DestroyInstance();

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

	m_pGameInstance->Release_Engine();
	Safe_Release(m_pGameInstance);
}
