#include "stdafx.h"
#ifdef _DEBUG

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include "imgui_internal.h"

#include "Imgui_Window_ModelAnim.h"

#include "Imgui_Manager.h"

#include "Imgui_Tab_ParticleEdit.h"
#include "Imgui_Tab_TrailEdit.h"
#include "Imgui_Tab_ModelAnim.h"

CImgui_Window_ModelAnim::CImgui_Window_ModelAnim(vector<shared_ptr<CGameObject>>* pGameObjectList, ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: CImgui_Window(pGameObjectList, pDevice, pContext)
{
}

HRESULT CImgui_Window_ModelAnim::Initialize(CImgui_Tab_ParticleEdit* pParticleTab, CImgui_Tab_TrailEdit* pTrailTab)
{
	//	m_pModelEditTab = CImgui_Tab_ModelAnim::Create(*m_pGameObjectList, m_pDevice, m_pContext,pParticleTab,pTrailTab);
	return S_OK;
}

void CImgui_Window_ModelAnim::Tick(_cref_time fTimeDelta)
{
}

HRESULT CImgui_Window_ModelAnim::Render()
{
	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove;
	ImVec4 vBackGroundColor = ImVec4(1.f, 1.f, 1.f, 1.f);
	ImVec2 vWinSize = { 250.f,g_iWinSizeY };
	string strName = "ModelAnim";

	ImGui::SetNextWindowSize(vWinSize, 0);

	ImGui::SetNextWindowBgAlpha(0.95f);

	ImGui::Begin(strName.c_str(), 0, window_flags);

	ImGui::PushItemWidth(ImGui::GetFontSize() * 10);

	ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
	if (ImGui::BeginTabBar("ModelEdit", tab_bar_flags))
	{
		if (m_bModelEditTabOn = ImGui::BeginTabItem("ModelAnim_Edit"))
		{
			//		m_pModelEditTab->Render();

			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}


	ImGui::End();

	return S_OK;
}

CImgui_Window_ModelAnim* CImgui_Window_ModelAnim::Create(vector<shared_ptr<CGameObject>>* pGameObjectList, ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, CImgui_Tab_ParticleEdit* pParticleTab, CImgui_Tab_TrailEdit* pTrailTab)
{
	CImgui_Window_ModelAnim* pInstance = new CImgui_Window_ModelAnim(pGameObjectList, pDevice, pContext);

	if (FAILED(pInstance->Initialize(pParticleTab, pTrailTab)))
	{
		MSG_BOX("Failed to Created : CImgui_Window_ModelAnim");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CImgui_Window_ModelAnim::Free()
{
	//Safe_Release(m_pModelEditTab);
	__super::Free();
}

#endif // 
