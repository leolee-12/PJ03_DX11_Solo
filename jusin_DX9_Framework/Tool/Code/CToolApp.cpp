#include "pch.h"
#include "CToolApp.h"
#include "CDInputMgr.h"
#include "CProtoMgr.h"
#include "CFontMgr.h"
#include "CLightMgr.h"
#include "CImGuiMgr.h"

CToolApp::CToolApp()
	:	m_pGraphicDev(nullptr),
		m_pDeviceClass(nullptr),
		m_pManagementClass(CManagement::GetInstance())
{
}

CToolApp::~CToolApp()
{
}

HRESULT CToolApp::Ready_ToolApp()
{
	// GraphicDev 초기화
	if (FAILED(CGraphicDev::GetInstance()->Ready_GraphicDev(g_hWnd, MODE_WIN, WINCX, WINCY, &m_pDeviceClass)))
	{
		MSG_BOX("GraphicDev Ready Failed");
		return E_FAIL;
	}

	m_pDeviceClass->AddRef();
	m_pGraphicDev = m_pDeviceClass->Get_GraphicDev();
	m_pGraphicDev->AddRef();
	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);

	// ImGui 초기화
	if (FAILED(CImGuiMgr::GetInstance()->Ready_ImGui(g_hWnd, m_pGraphicDev)))
	{
		MSG_BOX("ImGui Ready Failed");
		return E_FAIL;
	}




	// DInputMgr
	if (FAILED(CDInputMgr::GetInstance()->Ready_InputDev(g_hInst, g_hWnd)))
		return E_FAIL;

	m_pGraphicDev->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pGraphicDev->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);

	return S_OK;

	if (FAILED(Ready_Scene(m_pGraphicDev)))
	{
		MSG_BOX("GraphicDev Ready Failed");
		return E_FAIL;
	}

	return S_OK;
}

int CToolApp::Update_ToolApp(const float& fTimeDelta)
{
	CDInputMgr::GetInstance()->Update_InputDev();

	m_pManagementClass->Update_Scene(fTimeDelta);

	CImGuiMgr::GetInstance()->Update_ImGui();

	return 0;
}

void CToolApp::LateUpdate_ToolApp(const float& fTimeDelta)
{
	//m_pManagementClass->LateUpdate_Scene(fTimeDelta);
}

void CToolApp::Render_ToolApp()
{
	m_pDeviceClass->Render_Begin(D3DXCOLOR(0.f, 1.f, 1.f, 1.f));

	//m_pManagementClass->Render_Scene(m_pGraphicDev);
	CImGuiMgr::GetInstance()->Render_ImGui();

	m_pDeviceClass->Render_End();
}

HRESULT CToolApp::Ready_DefaultSetting(LPDIRECT3DDEVICE9* ppGraphicDev)
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

	return S_OK;
}

HRESULT CToolApp::Ready_Scene(LPDIRECT3DDEVICE9 pGraphicDev)
{
	//Engine::CScene* pLogo = CLogo::Create(pGraphicDev);

	//if (nullptr == pLogo)
	//	return E_FAIL;

	//if (FAILED(m_pManagementClass->Set_Scene(pLogo)))
	//{
	//	Safe_Release(pLogo);
	//	MSG_BOX("Logo Setting Failed");
	//	return E_FAIL;
	//}

	return S_OK;
}

CToolApp* CToolApp::Create()
{
	CToolApp* pToolApp = new CToolApp;

	if (FAILED(pToolApp->Ready_ToolApp()))
	{
		Safe_Release(pToolApp);
		return nullptr;
	}

	return pToolApp;
}

void CToolApp::Free()
{
	Safe_Release(m_pDeviceClass);
	Safe_Release(m_pGraphicDev);

	CImGuiMgr::DestroyInstance();
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
