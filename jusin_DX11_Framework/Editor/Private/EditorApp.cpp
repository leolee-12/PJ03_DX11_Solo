#include "EditorApp.h"
#include "EditInstance.h"

#include "Game_API.h"
#include "Battle_Data.h"
#include "SharedTexture_Manager.h"

#include "GameInstance.h"

CEditorApp::CEditorApp()
	: m_pGameInstance{ CGameInstance::GetInstance() }
	, m_pEditInstance{ CEditInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pEditInstance);
}

HRESULT CEditorApp::Initialize()
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

	if (FAILED(m_pEditInstance->Initialize_Editor(EngineDesc, &m_pDevice, &m_pContext)))
	{
		MSG_BOX("Failed to Initialize : Editor");
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

void CEditorApp::Update(_float fTimeDelta)
{
	Apply_Resize();

	m_pEditInstance->Update_Editor(fTimeDelta);
	m_pGameInstance->Update_Engine(fTimeDelta);
}

HRESULT CEditorApp::Render()
{
	if (FAILED(m_pGameInstance->Begin_Draw()))
		return E_FAIL;

	if (FAILED(m_pEditInstance->Begin_ViewportRender()))
		return E_FAIL;

	HRESULT hr = m_pGameInstance->Draw();

	m_pEditInstance->End_ViewportRender();
	
	if(FAILED(hr))
		return E_FAIL;

	if (FAILED(m_pEditInstance->Draw()))
		return E_FAIL;

	m_pGameInstance->Draw_Text(FONT_MALGUN, TEXT("한글 이다"), _float2(0.f, 0.f), XMVectorSet(1.f, 0.f, 0.f, 1.f));

	if (FAILED(m_pGameInstance->End_Draw()))
		return E_FAIL;

	return S_OK;
}

void CEditorApp::Request_Resize(_uint iNewWidth, _uint iNewHeight)
{
	if(0 == iNewWidth || 0 == iNewHeight)
		return;

	m_bResizePending = true;
	m_iPendingWidth = iNewWidth;
	m_iPendingHeight = iNewHeight;
}

HRESULT CEditorApp::Apply_Resize()
{
	if (!m_bResizePending)
		return S_OK;

	m_bResizePending = false;

	if (FAILED(m_pGameInstance->Resize_Surface(m_iPendingWidth, m_iPendingHeight)))
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

	m_pEditInstance->Release_Editor();
	Safe_Release(m_pEditInstance);
	
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

	m_pGameInstance->Set_SharedTextureBinder(nullptr);
	CSharedTexture_Manager::DestroyInstance();

	Cleanup_StaticTables();
	UI_Cleanup();

	m_pGameInstance->Release_Engine();
	Safe_Release(m_pGameInstance);
}
