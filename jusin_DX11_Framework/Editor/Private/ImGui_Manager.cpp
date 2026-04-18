#include "ImGui_Manager.h"
#include "EditInstance.h"

#include "Panel_OutLiner.h"
#include "Panel_Property.h"
#include "Panel_MapTool.h"
#include "Panel_PlaceBrowser.h"
#include "Panel_UITool.h"
#include "Panel_Model.h"
#include "Panel_Viewport.h"

CImGui_Manager::CImGui_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }
	, m_pContext{ pContext }
	, m_pEditInstance{ CEditInstance::GetInstance() }
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CImGui_Manager::Initialize(HWND hWnd)
{
	// ImGui 컨텍스트 생성
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO();

	// 한글 지원 폰트 로드
	io.Fonts->AddFontFromFileTTF(
		"../../Resources/Fonts/NanumBarunGothic.ttf",
		16.0f,
		nullptr,
		io.Fonts->GetGlyphRangesKorean()
	);

	// 		"../../Resources/Fonts/malgun.ttf",				// 맑은 고딕
	//		"../../Resources/Fonts/NotoSansKR-VF.ttf",		// Noto Sans KR
	//		"../../Resources/Fonts/NanumBarunGothic.ttf",	// 나눔 바른 고딕

	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;	// Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;		// IF using Docking Branch - Docking
	//io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;	// IF using Docking Branch - Multi-Viewport

	// 플랫폼/렌더러 백엔드 초기화
	ImGui_ImplWin32_Init(g_hWnd);
	ImGui_ImplDX11_Init(m_pDevice, m_pContext);

	// 스타일 설정
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();

	// Setup scaling
	ImGuiStyle& style = ImGui::GetStyle();
	//style.ScaleAllSizes(main_scale);
	//io.ConfigDpiScaleFonts = true;

	// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.5
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

	if (FAILED(Add_Panels()))
		return E_FAIL;

	return S_OK;
}

void CImGui_Manager::Update(_float fTimeDelta)
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	ImGui::DockSpaceOverViewport(0, nullptr, ImGuiDockNodeFlags_PassthruCentralNode);

	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("Window"))
		{
			for (auto& pPanel : m_Panels)
				if (pPanel) ImGui::MenuItem(pPanel->Get_Title().c_str(), nullptr, pPanel->Get_OpenPtr());

			ImGui::EndMenu();
		}

		ImGui::SameLine(ImGui::GetWindowWidth() - 150.f);
		_bool bCamera = m_pEditInstance->Is_CameraEnabled();
		if (ImGui::Checkbox("Camera (F1)", &bCamera))
			m_pEditInstance->Set_CameraEnabled(bCamera);

		ImGui::EndMainMenuBar();
	}

	for (auto& pPanel : m_Panels)
	{
		if (pPanel && pPanel->Is_Opened())
			pPanel->Update(fTimeDelta);
	}
}

HRESULT CImGui_Manager::Render()
{
	for (auto& pPanel : m_Panels)
	{
		if (pPanel && pPanel->Is_Opened() && FAILED(pPanel->Render()))
			return E_FAIL;
	}

	ID3D11RenderTargetView* pBackBufferRTV = { nullptr };
	ID3D11DepthStencilView* pDepthStencilView = { nullptr };
	m_pContext->OMGetRenderTargets(1, &pBackBufferRTV, &pDepthStencilView);

	ImGui::Render();
	m_pContext->OMSetRenderTargets(1, &pBackBufferRTV, nullptr);
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	ImGuiIO& io = ImGui::GetIO();

	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
	}

	m_pContext->OMSetRenderTargets(1, &pBackBufferRTV, pDepthStencilView);
	// OMGetRenderTargets가 AddRef하므로 Release필요
	Safe_Release(pBackBufferRTV);
	Safe_Release(pDepthStencilView);

	return S_OK;
}

#pragma region Panel_MapTool
_uint CImGui_Manager::Get_NavToolMode() const
{
	auto* pMapTool = static_cast<CPanel_MapTool*>(m_Panels[ETOUI(PANEL::MAP)]);
	return ETOUI(pMapTool->Get_NavToolMode());
}

void CImGui_Manager::Update_NavDragHit(const _float3& vWorldPos)
{
	auto* pMapTool = static_cast<CPanel_MapTool*>(m_Panels[ETOUI(PANEL::MAP)]);
	if (pMapTool) pMapTool->Set_DragHitPos(vWorldPos);
}

//_bool CImGui_Manager::Get_CurrentWorldHit(_float3* pOut) const
//{
//	auto* pMapTool = static_cast<CPanel_MapTool*>(m_Panels[ETOUI(PANEL::MAP)]);
//	return pMapTool->Get_CurrentWorldHit(pOut);
//}

_bool CImGui_Manager::Is_NavEditMode() const
{
	auto* pMapTool = static_cast<CPanel_MapTool*>(m_Panels[ETOUI(PANEL::MAP)]);
	return pMapTool ? pMapTool->Is_NavEditMode() : false;
}

_bool CImGui_Manager::Is_NavPointMode() const
{
	auto* pMapTool = static_cast<CPanel_MapTool*>(m_Panels[ETOUI(PANEL::MAP)]);
	return pMapTool ? pMapTool->Is_NavPointMode() : false;
}

void CImGui_Manager::Fire_NavClick(const _float3& vWorldPos)
{
	auto* pMapTool = static_cast<CPanel_MapTool*>(m_Panels[ETOUI(PANEL::MAP)]);
	if (pMapTool) pMapTool->Handle_NavClick(vWorldPos);
}
#pragma endregion

#pragma region Panel_PlaceBrowser
void CImGui_Manager::Begin_PlaceMode(const CATALOG_ITEM& tItem)
{
	m_bPlaceMode = true;
	m_tPlaceItem = tItem;
}

void CImGui_Manager::End_PlaceMode()
{
	m_bPlaceMode = false;
	m_tPlaceItem = {};
}
#pragma endregion

#pragma region Panel_Viewport
ImVec2 CImGui_Manager::Get_ViewportScreenPos() const
{
	CPanel_Viewport* pViewport = Get_ViewportPanel();
	if (nullptr == pViewport)
		return ImVec2(0.f, 0.f);
	return pViewport->Get_ViewportPos();
}

ImVec2 CImGui_Manager::Get_ViewportScreenSize() const
{
	CPanel_Viewport* pViewport = Get_ViewportPanel();
	if (nullptr == pViewport)
		return ImVec2(1.f, 1.f);
	return pViewport->Get_ViewportSize();
}

CPanel_Viewport* CImGui_Manager::Get_ViewportPanel() const
{
	return static_cast<CPanel_Viewport*>(m_Panels[ETOUI(PANEL::VIEWPORT)]);
}

_bool CImGui_Manager::Is_ViewportActive() const
{
	CPanel_Viewport* pViewport = Get_ViewportPanel();
	if (nullptr == pViewport)
		return false;

	return pViewport->Is_Hovered();
}

_bool CImGui_Manager::Is_AnyNonViewportPanelActive() const
{
	for (_uint i = 0; i < g_kNumPanels; ++i)
	{
		if (i == ETOUI(PANEL::VIEWPORT))
			continue;

		CPanel_Base* pPanel = m_Panels[i];
		if (nullptr == pPanel || !pPanel->Is_Opened())
			continue;

		if (pPanel->Is_Hovered() || pPanel->Is_Focused())
			return true;
	}

	return false;
}
#pragma endregion

HRESULT CImGui_Manager::Add_Panels()
{
	CPanel_Base* pInstance = nullptr;

	pInstance = CPanel_OutLiner::Create();
	if (nullptr == pInstance) return E_FAIL;
	m_Panels[ETOUI(PANEL::OUTLINER)] = pInstance;

	pInstance = CPanel_MapTool::Create();
	if (nullptr == pInstance) return E_FAIL;
	m_Panels[ETOUI(PANEL::MAP)] = pInstance;

	pInstance = CPanel_Property::Create();
	if (nullptr == pInstance) return E_FAIL;
	m_Panels[ETOUI(PANEL::PROPERTY)] = pInstance;

	pInstance = CPanel_PlaceBrowser::Create();
	if (nullptr == pInstance) return E_FAIL;
	m_Panels[ETOUI(PANEL::PLACEBROWSER)] = pInstance;

	pInstance = CPanel_UITool::Create();
	if (nullptr == pInstance) return E_FAIL;
	m_Panels[ETOUI(PANEL::UI)] = pInstance;

	pInstance = CPanel_Model::Create();
	if (nullptr == pInstance) return E_FAIL;
	m_Panels[ETOUI(PANEL::MODEL)] = pInstance;

	pInstance = CPanel_Viewport::Create(m_pDevice, m_pContext);
	if (nullptr == pInstance) return E_FAIL;
	m_Panels[ETOUI(PANEL::VIEWPORT)] = pInstance;

	return S_OK;
}

CImGui_Manager* CImGui_Manager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, HWND hWnd)
{
	CImGui_Manager* pInstance = new CImGui_Manager(pDevice, pContext);

	if (FAILED(pInstance->Initialize(hWnd)))
	{
		MSG_BOX("Failed to Created : CImGui_Manager");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CImGui_Manager::Free()
{
	__super::Free();

	if (ImGui::GetCurrentContext() != nullptr)
	{
		ImGuiIO& io = ImGui::GetIO();

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
			ImGui::DestroyPlatformWindows();
	}

	for(auto& pPanel : m_Panels)
		Safe_Release(pPanel);

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);
}