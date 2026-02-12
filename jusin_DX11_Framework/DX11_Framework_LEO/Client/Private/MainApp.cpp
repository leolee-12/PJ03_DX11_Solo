#include "MainApp.h"

CMainApp::CMainApp()
{
}

CMainApp::~CMainApp()
{
}

HRESULT CMainApp::Initialize()
{
	return E_NOTIMPL;
}

CMainApp* CMainApp::Create()
{
	CMainApp* pInstance = new CMainApp;

	if (FAILED(pInstance->Initialize()))
	{

	}

	return pInstance;
}

void CMainApp::Free()
{
	__super::Free();
}
