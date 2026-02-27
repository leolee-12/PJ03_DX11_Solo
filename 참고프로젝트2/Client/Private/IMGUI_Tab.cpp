#include "stdafx.h"
#include "IMGUI_Tab.h"

#include "GameInstance.h"
#include "RenderInstance.h"

CIMGUI_Tab::CIMGUI_Tab(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:m_pDevice{ pDevice }, m_pContext{ pContext }, m_pGameInstance{ CGameInstance::Get_Instance() }, m_pRenderInstance{ CRenderInstance::Get_Instance() }
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pRenderInstance);
}

void CIMGUI_Tab::Free()
{
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	Safe_Release(m_pGameInstance);
	Safe_Release(m_pRenderInstance);
}
