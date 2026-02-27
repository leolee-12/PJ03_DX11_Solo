#include "stdafx.h"
#include "..\Public\MainApp.h"

#include "GameInstance.h"
#include "Level_Loading.h"

#include "Imgui_Manager.h"

#include "MaterialComponent.h"
#include "CommonModelComp.h"
#include "ModelBinary.h"
#include "ObjPool_Manager.h"
#include "PhysX_Manager.h"
#include "Effect_Manager.h"
#include "Client_Manager.h"
#include "Utility_File.h"
#include "StatusComp.h"

#include "UI_Rect.h"
#include "UI_LoadingLogo.h"
#include "UI_StartMenu.h"
#include "UI_StartMenuBack.h"


CMainApp::CMainApp()	
	: m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CMainApp::Initialize()
{	
	GRAPHIC_DESC		GraphicDesc = {};

	GraphicDesc.hWnd = g_hWnd;
	GraphicDesc.eWinMode = GRAPHIC_DESC::WINMODE_WIN;
	GraphicDesc.iBackBufferSizeX = g_iWinSizeX;
	GraphicDesc.iBackBufferSizeY = g_iWinSizeY;

	if (FAILED(m_pGameInstance->Initialize_Engine(LEVEL_END,COLLIDER_LAYER_END ,g_hInst, GraphicDesc, &m_pDevice, &m_pContext)))
		RETURN_EFAIL;

	if (FAILED(Ready_Prototype_Component_ForStaticLevel()))
		RETURN_EFAIL;

	if (FAILED(Open_Level(LEVEL_LOGO)))
		RETURN_EFAIL;
	
	if (FAILED(Add_StaticEvnet()))
		RETURN_EFAIL;

	if (FAILED(Ready_Font()))
		RETURN_EFAIL;

	CClient_Manager::GetInstance()->Initialize_Client(m_pDevice, m_pContext);
	CObjPool_Manager::GetInstance()->Initialize(m_pDevice, m_pContext);
	CPhysX_Manager::GetInstance()->Initialize(m_pDevice, m_pContext, (_uint)COLLIDER_LAYER_END);
	CEffect_Manager::GetInstance()->Initialize();

	GET_SINGLE(CPhysX_Manager)->Register_PhysXFilterGroup(CL_PLAYER_CONTROLLER, CL_MAP_STATIC);	
	GET_SINGLE(CPhysX_Manager)->Register_PhysXFilterGroup(CL_PLAYER_CONTROLLER, CL_MAP_DYNAMIC);
	GET_SINGLE(CPhysX_Manager)->Register_PhysXFilterGroup(CL_PLAYER_CONTROLLER, CL_MONSTER_CONTROLLER);
	GET_SINGLE(CPhysX_Manager)->Register_PhysXFilterGroup(CL_PLAYER_CONTROLLER, CL_NPC_CONTROLLER);
	GET_SINGLE(CPhysX_Manager)->Register_PhysXFilterGroup(CL_PLAYER_CONTROLLER, CL_MOB_CONTROLLER);
	GET_SINGLE(CPhysX_Manager)->Register_PhysXFilterGroup(CL_NPC_CONTROLLER, CL_MOB_CONTROLLER);
	GET_SINGLE(CPhysX_Manager)->Register_PhysXFilterGroup(CL_MONSTER_CONTROLLER, CL_MAP_STATIC);
	GET_SINGLE(CPhysX_Manager)->Register_PhysXFilterGroup(CL_MONSTER_CONTROLLER, CL_MAP_DYNAMIC);
	GET_SINGLE(CPhysX_Manager)->Register_PhysXFilterGroup(CL_PLAYER_CONTROLLER, CL_WALL);
	GET_SINGLE(CPhysX_Manager)->Register_PhysXFilterGroup(CL_MONSTER_CONTROLLER, CL_WALL);
	GET_SINGLE(CPhysX_Manager)->Register_PhysXFilterGroup(CL_NPC_CONTROLLER, CL_WALL);
	GET_SINGLE(CPhysX_Manager)->Register_PhysXFilterGroup(CL_NPC_CONTROLLER, CL_MAP_DYNAMIC);
	GET_SINGLE(CPhysX_Manager)->Register_PhysXFilterGroup(CL_MOB_CONTROLLER, CL_WALL);
	GET_SINGLE(CPhysX_Manager)->Register_PhysXFilterGroup(CL_MOB_CONTROLLER, CL_MAP_DYNAMIC);
	GET_SINGLE(CPhysX_Manager)->Register_PhysXFilterGroup(CL_MONSTER_ATTACK, CL_WALL);
	GET_SINGLE(CPhysX_Manager)->Register_PhysXFilterGroup(CL_CAMERA, CL_WALL);

	GET_SINGLE(CPhysX_Manager)->Register_PhysXFilterGroup(CL_MAP_DYNAMIC, CL_WALL);
	GET_SINGLE(CPhysX_Manager)->Register_PhysXFilterGroup(CL_MAP_DYNAMIC, CL_PLAYER_ATTACK);
	GET_SINGLE(CPhysX_Manager)->Register_PhysXFilterGroup(CL_MAP_DYNAMIC, CL_MAP_DYNAMIC);

	GET_SINGLE(CPhysX_Manager)->Register_PhysXFilterGroup(CL_MAP_DYNAMIC_SPLIT, CL_WALL);
	GET_SINGLE(CPhysX_Manager)->Register_PhysXFilterGroup(CL_MAP_DYNAMIC_SPLIT, CL_PLAYER_ATTACK);
	GET_SINGLE(CPhysX_Manager)->Register_PhysXFilterGroup(CL_MAP_DYNAMIC_SPLIT, CL_MAP_DYNAMIC);

	GET_SINGLE(CPhysX_Manager)->Register_PhysXFilterGroup(CL_PLAYER_BODY, CL_TRIGGER);
	GET_SINGLE(CPhysX_Manager)->Register_PhysXFilterGroup(CL_PLAYER_BODY, CL_MONSTER_ATTACK);
	GET_SINGLE(CPhysX_Manager)->Register_PhysXFilterGroup(CL_PLAYER_BODY, CL_LASER);
	GET_SINGLE(CPhysX_Manager)->Register_PhysXFilterGroup(CL_PLAYER_CONTROLLER, CL_LASER);

	GET_SINGLE(CPhysX_Manager)->Register_PhysXFilterGroup(CL_PLAYER_BODY, CL_NPC_BODY);
	GET_SINGLE(CPhysX_Manager)->Register_PhysXFilterGroup(CL_PLAYER_BODY, CL_MOB_BODY);

	GET_SINGLE(CPhysX_Manager)->Register_PhysXFilterGroup(CL_MONSTER_BODY, CL_PLAYER_ATTACK);

	GET_SINGLE(CPhysX_Manager)->Register_PhysXFilterGroup(CL_MOB_BODY, CL_PLAYER_ATTACK);

	return S_OK;
}

void CMainApp::Tick(_cref_time fTimeDelta)
{
	//auto start = chrono::high_resolution_clock::now();
	
	m_pGameInstance->Tick_Engine(fTimeDelta);

	/*auto end = chrono::high_resolution_clock::now();
	chrono::duration<double, micro> elapsed = end - start;
	cout << "Function execution took " << elapsed.count() << " milliseconds.\n";*/

//#ifdef _DEBUG
	if (m_pGameInstance->Key_DownEx(DIK_F1, DIK_MD_LCTRL_ALT))
	{
		RecompileShader();
	}
//#endif // _DEBUG

}

HRESULT CMainApp::Render()
{
	m_pGameInstance->Clear_BackBuffer_View(_float4(0.f, 0.f, 1.f, 1.f));

	m_pGameInstance->Clear_DepthStencil_View();
	/* 그려야할 모델들을 그린다.*/
	m_pGameInstance->Render_Engine();

	//MakeSpriteFont "넥슨lv1고딕 Bold" / FontSize:30 / FastPack / CharacterRegion : 0x0020 - 0x00FF / CharacterRegion : 0x3131 - 0x3163 / CharacterRegion : 0xAC00 - 0xD800 / DefaultCharacter : 0xAC00 138ex.spritefont
	//m_pGameInstance->Render_Font(TEXT("Font_Default"), TEXT("아자아자 화이팅!"), _float2(500.f, 150.f)); //<<성공

	m_pGameInstance->Present();

	return S_OK;
}

HRESULT CMainApp::Open_Level(LEVEL eStartLevelID)
{
	if (nullptr == m_pGameInstance)
		RETURN_EFAIL;

	CLevel*		pLevel = CLevel_Loading::Create(m_pDevice, m_pContext, eStartLevelID);
	if (nullptr == pLevel)
		RETURN_EFAIL;

	return m_pGameInstance->Open_Level(LEVEL_LOADING, pLevel);	
}

HRESULT CMainApp::Ready_Prototype_Component_ForStaticLevel()
{
	if (FAILED(CStatusComp::Initialize_SkillDataTable(
		m_pGameInstance->MainDataPath() + TEXT("Table_Data/SkillTable.json"))))
		RETURN_EFAIL;
	if (FAILED(CStatusComp::Initialize_CharacterDataTable(
		m_pGameInstance->MainDataPath() + TEXT("Table_Data/CharacterTable.json"))))
		RETURN_EFAIL;
	if (FAILED(CStatusComp::Initialize_EnemyDataTable(
		m_pGameInstance->MainDataPath() + TEXT("Table_Data/CharacterTable.json"))))
		RETURN_EFAIL;
	if (FAILED(CStatusComp::Initialize_ActionInfoTable(
		m_pGameInstance->MainDataPath() + TEXT("Table_Data/ActionInfoTable.json"))))
		RETURN_EFAIL;

	if (FAILED(CUtility_File::All_FilePath_Save(L"../Bin/Data/")))
		RETURN_EFAIL;

	if (FAILED(CUtility_File::All_FilePath_Save(L"../Bin/Resources/Model/")))
		RETURN_EFAIL;

	if (FAILED(CUtility_File::All_FilePath_Save(L"../Bin/Resources/Textures/")))
		RETURN_EFAIL;

	GET_SINGLE(CPath_Mgr)->Test_MAP_SIZE();

	// 머터리얼 리베이크가 필요하면 이 코드를 활성화
	//if (FAILED(m_pGameInstance->Rebake_MaterialsAll(L"")))
	//	RETURN_EFAIL;

	/* For.Prototype_Component_VIBuffer_Rect*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		CVIBuffer_Rect::Create(m_pDevice, m_pContext))))
		RETURN_EFAIL;

	/* For.Prototype_Component_Shader_VtxPosTex*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("Shader_VtxPosTex"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements))))
		RETURN_EFAIL;

	/* For.Prototype_Component_Shader_UI*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"),
		CShader::Create(m_pDevice, m_pContext, TEXT("Shader_UI.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements))))
		RETURN_EFAIL;

	if (FAILED(Ready_LoadingLogo()))
		RETURN_EFAIL;

	return S_OK;
}

HRESULT CMainApp::Add_StaticEvnet()
{
	return S_OK;
}

HRESULT CMainApp::Ready_Font()
{
	if (FAILED(m_pGameInstance->Add_Font(TEXT("Font_Default"), TEXT("../Bin/Resources/Fonts/139ex.spritefont"))))
		RETURN_EFAIL;

	return S_OK;
}

HRESULT CMainApp::Ready_LoadingLogo()
{
	/* For.Prototype_GameObject_UIRect */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UIRect"),
		CUI_Rect::Create(m_pDevice, m_pContext))))
		RETURN_EFAIL;

	/* For.Prototype_GameObject_UI_LoadingLogo */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_LoadingLogo"),
		CUI_LoadingLogo::Create(m_pDevice, m_pContext))))
		RETURN_EFAIL;

	/* For.Prototype_GameObject_UI_StartMenu */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_StartMenu"),
		CUI_StartMenu::Create(m_pDevice, m_pContext))))
		RETURN_EFAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_StartMenuBack"),
		CUI_StartMenuBack::Create(m_pDevice, m_pContext))))
		RETURN_EFAIL;

	if (FAILED(m_pGameInstance->Include_All_Textures(TEXT("UI_Logo/"), true)))
		RETURN_EFAIL;
	
	if (FAILED(m_pGameInstance->Include_All_Textures(TEXT("UI_Resource/"), true)))
		RETURN_EFAIL;

	return S_OK;
}

void CMainApp::RecompileShader()
{
	//Batch 실행
	{
		wstring strPath = L"../../Bat/Copy.bat";
		wchar_t strFilePath[MAX_PATH];
		GetFullPathName(strPath.c_str(), MAX_PATH, strFilePath, NULL);
		// CreateProcess 함수 사용
		{
			WCHAR ApplicationName[MAX_PATH];
			if (!GetEnvironmentVariable(L"comspec", ApplicationName, RTL_NUMBER_OF(ApplicationName)))
			{
				return;
			}

			SIZE_T cch = wcslen(strFilePath);

			PWSTR CommandLine = (PWSTR)alloca(cch * sizeof(WCHAR) + sizeof(L"/c \"\""));

			swprintf(CommandLine, L"/c \"%s\"", strFilePath);

			PROCESS_INFORMATION procHandles;
			STARTUPINFO startWinInfo = { sizeof(startWinInfo) };

			if (CreateProcess(ApplicationName, CommandLine, NULL, NULL, 0,
				CREATE_DEFAULT_ERROR_MODE, NULL,
				NULL, &startWinInfo, &procHandles))
			{
				_int a = 0;
				CloseHandle(procHandles.hThread);

				switch (WaitForSingleObject(procHandles.hProcess, 3000))
				{
				case WAIT_OBJECT_0:
					break;
				default:
					TerminateProcess(procHandles.hProcess, 0);
				}
				CloseHandle(procHandles.hProcess);

			}
		}
	}
	m_pGameInstance->Recompile_EffectsAll();
}

CMainApp * CMainApp::Create()
{
	CMainApp*		pInstance = new CMainApp();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CMainApp");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMainApp::Free()
{
	CEffect_Manager::GetInstance()->DestroyInstance();
	
	// 디버그 집계를 제대로 하기위해 넣은 코드
	m_pDevice.Reset();
	m_pContext.Reset();

	CObjPool_Manager::DestroyInstance();
	CGameInstance::Release_Engine();
	CPhysX_Manager::DestroyInstance();
	CClient_Manager::DestroyInstance();

	/*  내 멤버를 정리하면. */
	Safe_Release(m_pGameInstance);
}
