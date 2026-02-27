#include "stdafx.h"
#ifdef _DEBUG
#include "Imgui_Window_AnimMainEdit.h"

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include "imgui_internal.h"

#include "Imgui_Manager.h"
#include "Imgui_Tab_AnimObject.h"
#include "Imgui_Tab_PartsObject.h"
#include "Imgui_Tab_AnimOffset.h"
#include "Imgui_Tab_AnimAdd.h"
#include "Imgui_Tab_AnimMask.h"

CImgui_Window_AnimEdit::CImgui_Window_AnimEdit(vector<shared_ptr<CGameObject>>* pGameObjectList, ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: CImgui_Window(pGameObjectList, pDevice, pContext)
{
}

HRESULT CImgui_Window_AnimEdit::Initialize()
{
	m_pTab_AnimObject = CImgui_Tab_AnimObject::Create(*m_pGameObjectList, m_pDevice, m_pContext);
	m_pTab_PartsObject = CImgui_Tab_PartsObject::Create(*m_pGameObjectList, m_pDevice, m_pContext);
	m_pTab_AnimOffset = CImgui_Tab_AnimOffset::Create(*m_pGameObjectList, m_pDevice, m_pContext);
	m_pTab_AnimMask = CImgui_Tab_AnimMask::Create(*m_pGameObjectList, m_pDevice, m_pContext);
	m_pTab_AnimAdd = CImgui_Tab_AnimAdd::Create(*m_pGameObjectList, m_pDevice, m_pContext);

	return S_OK;
}

void CImgui_Window_AnimEdit::Tick(_cref_time fTimeDelta)
{
}

HRESULT CImgui_Window_AnimEdit::Render()
{
	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove;
	ImVec4 vBackGroundColor = ImVec4(1.f, 1.f, 1.f, 1.f);
	ImVec2 vWinPos = { 0.f, 0.f };
	ImVec2 vWinSize = { 350.f, 480.f };
	string strName = "Animation";

	//ImGui::SetNextWindowPos(vWinPos);
	ImGui::SetNextWindowSize(vWinSize, 0);

	ImGui::SetNextWindowBgAlpha(0.95f);

	ImGui::Begin(strName.c_str(), 0, window_flags);

	ImGui::PushItemWidth(ImGui::GetFontSize() * 10);

	ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
	if (ImGui::BeginTabBar("Animation", tab_bar_flags))
	{
		if (m_bSequencer = ImGui::BeginTabItem("Animation"))
		{
			m_pTab_AnimObject->Render();

			ImGui::EndTabItem();
		}

		if (m_bSequencer = ImGui::BeginTabItem("Parts"))
		{
			m_pTab_PartsObject->Render();

			ImGui::EndTabItem();
		}

		if (m_bOffsetTab = ImGui::BeginTabItem("Notify Offset"))
		{
			m_pTab_AnimOffset->Render();

			ImGui::EndTabItem();
		}

		if (m_bOffsetTab = ImGui::BeginTabItem("AnimMask"))
		{
			m_pTab_AnimMask->Render();

			ImGui::EndTabItem();
		}

		if (m_bOffsetTab = ImGui::BeginTabItem("AnimAdd"))
		{
			m_pTab_AnimAdd->Render();

			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}


	ImGui::End();

	return S_OK;
}

CImgui_Window_AnimEdit* CImgui_Window_AnimEdit::Create(vector<shared_ptr<CGameObject>>* pGameObjectList, ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	CImgui_Window_AnimEdit* pInstance = new CImgui_Window_AnimEdit(pGameObjectList, pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CImgui_Window_AnimEdit");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CImgui_Window_AnimEdit::Free()
{
	__super::Free();

	Safe_Release(m_pTab_AnimObject);
	Safe_Release(m_pTab_PartsObject);
	Safe_Release(m_pTab_AnimOffset);
	Safe_Release(m_pTab_AnimAdd);
	Safe_Release(m_pTab_AnimMask);

}

#endif // DEBUG
