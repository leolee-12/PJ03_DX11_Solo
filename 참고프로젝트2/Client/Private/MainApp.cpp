#include "stdafx.h"

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

#include "UI_Manager.h"

#include "..\Public\MainApp.h"

#include "GameInstance.h"
#include "RenderInstance.h"
#include "Level_Loading.h"
#include "Imgui_Manager.h"

#include "Effect_Manager.h"
#include "thread"

CMainApp::CMainApp()
	: m_pGameInstance{ CGameInstance::Get_Instance() }
	, m_pRenderInstance{ CRenderInstance::Get_Instance() }
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pRenderInstance);
}

HRESULT CMainApp::Initialize()
{
	//게임인스턴스 엔진 초기화
	if (FAILED(m_pGameInstance->Initialize_Engine(g_hInst, g_hWnd, true, LEVEL_END, g_iWinSizeX, g_iWinSizeY, &m_pDevice, &m_pContext)))
		return E_FAIL;

	//렌더인스턴스 엔진 초기화
	if (FAILED(m_pRenderInstance->Initialize_Engine(g_hWnd, true, LEVEL_END, g_iWinSizeX, g_iWinSizeY, &m_pDevice, &m_pContext)))
		return E_FAIL;

	//메인앱_Static
	if (FAILED(Ready_Prototype_Component_ForStatic()))
		return E_FAIL;

	////IMGUI 생성, 싱글턴
	Create_IMGUI_Manager();

	//스레드풀 초기화
	//하드웨어의 스레드 수를 넘겨준다. (소프트웨어 스레드 수 아님)
	if (FAILED(m_pGameInstance->Initialize_ThreadPool(thread::hardware_concurrency())))
		return E_FAIL;

	if (FAILED(Open_Level(LEVEL_LOGO)))
		return E_FAIL;

	if (FAILED(Ready_Fonts()))
		return E_FAIL;

	return S_OK;
}

void CMainApp::Update(_float fTimeDelta)
{
	m_pGameInstance->Update_Engine(fTimeDelta);
}

// 1/50 -> 0.02초 고정 Update
void CMainApp::Fixed_Update(_float fTimeDelta)
{

}

HRESULT CMainApp::Render(_float fTimeDelta)
{
	//m_pGameInstance->Clear_BackBuffer_View(_float4(0.f, 0.f, 1.f, 1.f));
	m_pGameInstance->Clear_BackBuffer_View(_float4(0.f, 0.f, 0.f, 1.f));
	m_pGameInstance->Clear_DepthStencil_View();

	//레벨매니저 렌더는 게임인스턴스
	m_pGameInstance->Render_Engine(fTimeDelta);

	//나머지 렌더는 렌더인스턴스
	m_pRenderInstance->Render_Engine(fTimeDelta);

	//IMGUI 렌더는 로딩때는 하면 안됨
 
	//_uint currentLevel_Index = m_pGameInstance->Get_CurrentLevel_Index();

	//_bool isOk_Render = currentLevel_Index != (_uint)LEVEL_LOADING && (_uint)currentLevel_Index != LEVEL_LOGO;
	//if (isOk_Render && m_pImgui_Manager != nullptr)
	//	m_pImgui_Manager->Render(fTimeDelta);

	m_pGameInstance->Present();

	return S_OK;
}

HRESULT CMainApp::Open_Level(LEVELID eStartLevelID)
{
	if (FAILED(m_pGameInstance->Change_Level(CLevel_Loading::Create(m_pDevice, m_pContext, eStartLevelID))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Create_IMGUI_Manager()
{
	m_pImgui_Manager = CImgui_Manager::Get_Instance();
	m_pImgui_Manager->Initialize(m_pDevice, m_pContext);

	return S_OK;
}

HRESULT CMainApp::Ready_Prototype_Component_ForStatic()
{
#pragma region 쉐이더
	/* For.Prototype_Component_Shader_VtxPosTex */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxAnimMesh.hlsl"), VTXANIMMESH::Elements, VTXANIMMESH::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_UI_VtxRect */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI_VtxRect"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_UI_VtxRect.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxPosTex */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPosTex.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_Single_Eff_VtxMesh"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Single_Eff_VtxMesh.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxMesh.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements))))
		return E_FAIL;

#pragma endregion

#pragma region VIBuffer

	/* For.Prototype_Component_VIBuffer_Rect */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		CVIBuffer_Rect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Rect */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Line_Rect"),
		CVIBuffer_Line_Rect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Trail */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Trail"),
		CVIBuffer_Trail_Rect::Create(m_pDevice, m_pContext, 100, 0.25f))))
		return E_FAIL;

#pragma endregion

	return S_OK;
}

HRESULT CMainApp::Ready_Fonts()
{
	if (FAILED(m_pGameInstance->Add_Font(m_pDevice, m_pContext, TEXT("Font_Nexon"), TEXT("../Bin/Resources/Fonts/144ex.spriteFont"))))
		return E_FAIL;

	return S_OK;
}

CMainApp* CMainApp::Create()
{
	CMainApp* pInstance = new CMainApp();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CMainApp"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMainApp::Free()
{
	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);

	m_pGameInstance->Release_Engine();
	Safe_Release(m_pGameInstance);

	m_pImgui_Manager->Free();

	m_pRenderInstance->Release_Engine();
	Safe_Release(m_pRenderInstance);

}

