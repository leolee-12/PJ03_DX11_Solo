#include "stdafx.h"
#include "IMGUI_Level_Tab.h"
#include "GameInstance.h"
#include "RenderInstance.h"
#include "Level_Loading.h"


CIMGUI_Level_Tab::CIMGUI_Level_Tab(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CIMGUI_Tab{ pDevice,pContext }
{
}

HRESULT CIMGUI_Level_Tab::Initialize()
{
	return S_OK;
}

void CIMGUI_Level_Tab::Render(_float fTimeDelta)
{
	IMGUI_ShowLevels();
}

HRESULT CIMGUI_Level_Tab::IMGUI_ShowLevels()
{
	// 가정: 레벨 이름과 ID를 가진 배열 또는 리스트
	vector<pair<string, LEVELID>> levels = {
			{"Logo", LEVEL_LOGO},
			{"Lobby", LEVEL_LOBBY},
			{"Character", LEVEL_CHARACTER},
			{"GamePlay", LEVEL_GAMEPLAY},
	};

	for (auto& level : levels) {
		if (ImGui::Button(level.first.c_str())) {
			return m_pGameInstance->Change_Level(CLevel_Loading::Create(m_pDevice, m_pContext, level.second));
		}
	}

	return S_OK;
}

CIMGUI_Level_Tab* CIMGUI_Level_Tab::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CIMGUI_Level_Tab* pInstance = new CIMGUI_Level_Tab(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CIMGUI_Level_Tab"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CIMGUI_Level_Tab::Free()
{
	__super::Free();
}
