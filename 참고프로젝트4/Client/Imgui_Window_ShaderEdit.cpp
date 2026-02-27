#include "stdafx.h"
#ifdef _DEBUG

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include "imgui_internal.h"

#include "Imgui_Window_ShaderEdit.h"

#include "Imgui_Tab_ShaderEdit.h"
#include "Imgui_Manager.h"


CImgui_Window_ShaderEdit::CImgui_Window_ShaderEdit(vector<shared_ptr<CGameObject>>* pGameObjectList, ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: CImgui_Window(pGameObjectList, pDevice, pContext)
{
}

HRESULT CImgui_Window_ShaderEdit::Initialize()
{
	m_pShaderEditTab = CImgui_Tab_ShaderEdit::Create(*m_pGameObjectList, m_pDevice, m_pContext);
	return S_OK;
}

void CImgui_Window_ShaderEdit::Tick(_cref_time fTimeDelta)
{
}

HRESULT CImgui_Window_ShaderEdit::Render()
{
	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_AlwaysAutoResize;// | ImGuiWindowFlags_NoMove;
	ImVec4 vBackGroundColor = ImVec4(1.f, 1.f, 1.f, 1.f);
	ImVec2 vWinSize = { 315.f, g_iWinSizeY };
	string strName = "Shader_Edit";

	ImGui::SetNextWindowSize(vWinSize, 0);

	ImGui::SetNextWindowBgAlpha(0.95f);

	ImGui::Begin(strName.c_str(), 0, window_flags);

	ImGui::PushItemWidth(ImGui::GetFontSize() * 10);

	ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
	if (ImGui::BeginTabBar("Shader", tab_bar_flags))
	{
		if (m_bTexEditTabOn = ImGui::BeginTabItem("Shader_Tab"))
		{
			m_pShaderEditTab->Render();

			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}

	ImGui::End();

	return S_OK;
}

CImgui_Window_ShaderEdit* CImgui_Window_ShaderEdit::Create(vector<shared_ptr<CGameObject>>* pGameObjectList, ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	CImgui_Window_ShaderEdit* pInstance = new CImgui_Window_ShaderEdit(pGameObjectList, pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CImgui_Window_ShaderEdit");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CImgui_Window_ShaderEdit::Free()
{
	Safe_Release(m_pShaderEditTab);

	__super::Free();
}

#endif // 
