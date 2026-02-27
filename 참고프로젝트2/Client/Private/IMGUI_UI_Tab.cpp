#include "stdafx.h"
#include "IMGUI_UI_Tab.h"
#include "GameInstance.h"
#include "RenderInstance.h"


CIMGUI_UI_Tab::CIMGUI_UI_Tab(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CIMGUI_Tab{ pDevice,pContext }
{
}

HRESULT CIMGUI_UI_Tab::Initialize()
{
	return S_OK;
}

void CIMGUI_UI_Tab::Render(_float fTimeDelta)
{
}

CIMGUI_UI_Tab* CIMGUI_UI_Tab::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CIMGUI_UI_Tab* pInstance = new CIMGUI_UI_Tab(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CIMGUI_UI_Tab"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CIMGUI_UI_Tab::Free()
{
	__super::Free();
}
