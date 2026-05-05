#include "MainApp.h"

#include "Game_API.h"
#include "Game_BattleData.h"
#include "SharedTexture_Manager.h"

#include "GameInstance.h"

CMainApp::CMainApp()
	: m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CMainApp::Initialize()
{
	ENGINE_DESC	EngineDesc{};
	EngineDesc.hInstance = g_hInstance;
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

	if (FAILED(Ready_Fonts()))
		return E_FAIL;

	if (FAILED(Ready_Prototypes_For_Static(m_pDevice, m_pContext)))
		return E_FAIL;

	if (FAILED(Ready_SharedTextures(m_pDevice, m_pContext)))
		return E_FAIL;

	if (FAILED(Ready_StaticTables()))
		return E_FAIL;

	if (FAILED(Ready_PersistentObjects(m_pDevice, m_pContext)))
		return E_FAIL;

	if (FAILED(Start_Level(m_pDevice, m_pContext, LEVEL::LOGO)))
		return E_FAIL;

	return S_OK;
}

void CMainApp::Update(_float fTimeDelta)
{
	m_pGameInstance->Update_Engine(fTimeDelta);
}

HRESULT CMainApp::Render()
{
	if (FAILED(m_pGameInstance->Begin_Draw()))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Draw()))
		return E_FAIL;

	if (FAILED(m_pGameInstance->End_Draw()))
		return E_FAIL;

	return S_OK;
}

CMainApp* CMainApp::Create()
{
	CMainApp* pInstance = new CMainApp;

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Create : CMainApp");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMainApp::Free()
{
	__super::Free();

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);


	m_pGameInstance->Set_SharedTextureBinder(nullptr);
	CSharedTexture_Manager::DestroyInstance();

	Cleanup_StaticTables();
	UI_Cleanup();

	m_pGameInstance->Release_Engine();
	Safe_Release(m_pGameInstance);
}
