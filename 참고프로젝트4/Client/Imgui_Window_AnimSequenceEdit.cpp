#include "stdafx.h"
#ifdef _DEBUG
#include "Imgui_Window_AnimSequenceEdit.h"

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include "imgui_internal.h"

#include "Imgui_Manager.h"

#include "Imgui_Tab_AnimSequencer.h"

CImgui_Window_AnimSequenceEdit::CImgui_Window_AnimSequenceEdit(vector<shared_ptr<CGameObject>>* pGameObjectList, ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: CImgui_Window(pGameObjectList, pDevice, pContext)
{
}

HRESULT CImgui_Window_AnimSequenceEdit::Initialize()
{
	m_pTab_AnimSequencer = CImgui_Tab_AnimSequencer::Create(*m_pGameObjectList, m_pDevice, m_pContext);

	return S_OK;
}

void CImgui_Window_AnimSequenceEdit::Tick(_cref_time fTimeDelta)
{
}

HRESULT CImgui_Window_AnimSequenceEdit::Render()
{
	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove;
	ImVec4 vBackGroundColor = ImVec4(1.f, 1.f, 1.f, 1.f);
	ImVec2 vWinPos = { 0.f, 500.f };
	ImVec2 vWinSize = { g_iWinSizeX, 300.f };
	string strName = "Sequence";

	ImGui::SetNextWindowPos(vWinPos);
	ImGui::SetNextWindowSize(vWinSize, 0);

	ImGui::SetNextWindowBgAlpha(0.95f);

	ImGui::Begin(strName.c_str(), 0, window_flags);

	ImGui::PushItemWidth(ImGui::GetFontSize() * 10);



	ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
	if (ImGui::BeginTabBar("Sequencer", tab_bar_flags))
	{
		if (m_bSequencer = ImGui::BeginTabItem("Sequencer"))
		{
			m_pTab_AnimSequencer->Render();

			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}

	ImGui::End();

	return S_OK;
}

CImgui_Window_AnimSequenceEdit* CImgui_Window_AnimSequenceEdit::Create(vector<shared_ptr<CGameObject>>* pGameObjectList, ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	CImgui_Window_AnimSequenceEdit* pInstance = new CImgui_Window_AnimSequenceEdit(pGameObjectList, pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CImgui_Window_AnimSequenceEdit");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CImgui_Window_AnimSequenceEdit::Free()
{
	__super::Free();

	Safe_Release(m_pTab_AnimSequencer);
}

#endif // DEBUG
