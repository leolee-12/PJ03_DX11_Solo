#include "stdafx.h"
#ifdef _DEBUG

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include "imgui_internal.h"

#include "Imgui_Window_ModelEdit.h"

#include "Imgui_Manager.h"

#include "Imgui_Tab_BakeBinary.h"

CImgui_Window_ModelEdit::CImgui_Window_ModelEdit(vector<shared_ptr<CGameObject>>* pGameObjectList, ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: CImgui_Window(pGameObjectList, pDevice, pContext)
{
}

HRESULT CImgui_Window_ModelEdit::Initialize()
{
	//	m_pBakeBinaryTab = CImgui_Tab_BakeBinary::Create(*m_pGameObjectList,m_pDevice,m_pContext);
	return S_OK;
}

void CImgui_Window_ModelEdit::Tick(_cref_time fTimeDelta)
{
}

HRESULT CImgui_Window_ModelEdit::Render()
{
	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove;
	ImVec4 vBackGroundColor = ImVec4(1.f, 1.f, 1.f, 1.f);
	ImVec2 vWinSize = { 300.f,g_iWinSizeY };
	string strName = "Model";

	ImGui::SetNextWindowSize(vWinSize, 0);

	ImGui::SetNextWindowBgAlpha(0.95f);

	ImGui::Begin(strName.c_str(), 0, window_flags);

	ImGui::PushItemWidth(ImGui::GetFontSize() * 10);

	ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
	if (ImGui::BeginTabBar("Model_Bake", tab_bar_flags))
	{
		if (m_bSequencer = ImGui::BeginTabItem("Model_Bake"))
		{
			//		m_pBakeBinaryTab->Render();

			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}


	ImGui::End();

	return S_OK;
}

CImgui_Window_ModelEdit* CImgui_Window_ModelEdit::Create(vector<shared_ptr<CGameObject>>* pGameObjectList, ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	CImgui_Window_ModelEdit* pInstance = new CImgui_Window_ModelEdit(pGameObjectList, pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CImgui_Window_ModelEdit");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CImgui_Window_ModelEdit::Free()
{
	//Safe_Release(m_pBakeBinaryTab);
	__super::Free();
}

#endif // 
