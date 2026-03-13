#include "pch.h"
#include "CMainApp.h"
#include "CLogo.h"
#include "CStage.h"
#include "CProtoMgr.h"
#include "CDInputMgr.h"
#include "CFontMgr.h"
#include "CLightMgr.h"

CMainApp::CMainApp()
	:	m_pGraphicDev(nullptr),
		m_pDeviceClass(nullptr),
		m_pManagementClass(CManagement::GetInstance())
{
}

CMainApp::~CMainApp()
{
}

HRESULT CMainApp::Ready_MainApp()
{
	if (FAILED(Ready_DefaultSetting(&m_pGraphicDev)))
		return E_FAIL;

	if (FAILED(Ready_Scene(m_pGraphicDev)))
	{
		MSG_BOX("GraphicDev Ready Failed");
		return E_FAIL;
	}

	return S_OK;
}

int CMainApp::Update_MainApp(const float& fTimeDelta)
{
	CDInputMgr::GetInstance()->Update_InputDev();

	m_pManagementClass->Update_Scene(fTimeDelta);

	_ulong dwDst = 0;

	//if (dwDst = CDInputMgr::GetInstance()->Get_DIMouseMove(DIM_Z))
	//{
	//	int a = 0;
	//}

	return 0;
}

void CMainApp::LateUpdate_MainApp(const float& fTimeDelta)
{
	m_pManagementClass->LateUpdate_Scene(fTimeDelta);
}

void CMainApp::Render_MainApp()
{
	m_pDeviceClass->Render_Begin(D3DXCOLOR(0.f, 0.f, 1.f, 1.f));

	m_pManagementClass->Render_Scene(m_pGraphicDev);

	m_pDeviceClass->Render_End();
}

HRESULT CMainApp::Ready_DefaultSetting(LPDIRECT3DDEVICE9* ppGraphicDev)
{
	if (FAILED(CGraphicDev::GetInstance()->Ready_GraphicDev(g_hWnd, MODE_WIN, WINCX, WINCY, &m_pDeviceClass)))
	{
		MSG_BOX("GraphicDev Ready Failed");
		return E_FAIL;
	}

	m_pDeviceClass->AddRef();

	(*ppGraphicDev) = m_pDeviceClass->Get_GraphicDev();

	(*ppGraphicDev)->AddRef();

	(*ppGraphicDev)->SetRenderState(D3DRS_LIGHTING, FALSE);

	// DInputMgr
	if (FAILED(CDInputMgr::GetInstance()->Ready_InputDev(g_hInst, g_hWnd)))
		return E_FAIL;

	(*ppGraphicDev)->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	(*ppGraphicDev)->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);

	// 폰트 추가
	if (FAILED(CFontMgr::GetInstance()->Ready_Font((*ppGraphicDev), L"Font_Default", L"견명조", 20, 20, FW_HEAVY)))
		return E_FAIL;

	if (FAILED(CFontMgr::GetInstance()->Ready_Font((*ppGraphicDev), L"Font_Jinji", L"궁서", 20, 15, FW_THIN)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Ready_Scene(LPDIRECT3DDEVICE9 pGraphicDev)
{
	Engine::CScene* pLogo = CLogo::Create(pGraphicDev);
	
	if (nullptr == pLogo)
		return E_FAIL;
	
	if (FAILED(m_pManagementClass->Set_Scene(pLogo)))
	{
		Safe_Release(pLogo);
		MSG_BOX("Logo Setting Failed");
		return E_FAIL;
	}

	return S_OK;
}

CMainApp* CMainApp::Create()
{
	CMainApp* pMainApp = new CMainApp;

	if (FAILED(pMainApp->Ready_MainApp()))
	{
		Safe_Release(pMainApp);
		return nullptr;
	}

	return pMainApp;
}

void CMainApp::Free()
{
	Safe_Release(m_pDeviceClass);
	Safe_Release(m_pGraphicDev);

	CLightMgr::DestroyInstance();
	CFontMgr::DestroyInstance();
	CDInputMgr::DestroyInstance();
	CRenderer::DestroyInstance();
	CProtoMgr::DestroyInstance();
	CFrameMgr::DestroyInstance();
	CTimerMgr::DestroyInstance();
	CManagement::DestroyInstance();
	m_pDeviceClass->DestroyInstance();
}
