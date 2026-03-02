#include "ImGui_Manager.h"

IMPLEMENT_SINGLETON(CImGui_Manager)

CImGui_Manager::CImGui_Manager()
{
}

HRESULT CImGui_Manager::Ready_ImGui(HWND hWnd, _Inout_ ID3D11Device** ppDevice, _Inout_ ID3D11DeviceContext** ppContext, ID3D11RenderTargetView** ppBackBufferRTV)
{
	// Make process DPI aware and obtain main monitor scale
	ImGui_ImplWin32_EnableDpiAwareness();
	_float main_scale = ImGui_ImplWin32_GetDpiScaleForMonitor(::MonitorFromPoint(POINT{ 0, 0 }, MONITOR_DEFAULTTOPRIMARY));

	// 장치 전달
	m_pDevice = *ppDevice;
	m_pContext = *ppContext;
	m_pBackBufferRTV = *ppBackBufferRTV;
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pBackBufferRTV);

	// ImGui 컨텍스트 생성
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;	// Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;	// Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;		// IF using Docking Branch - Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;		// IF using Docking Branch - Multi-Viewport
	//io.ConfigViewportsNoAutoMerge = true;
	//io.ConfigViewportsNoTaskBarIcon = true;
	//io.ConfigDockingAlwaysTabBar = true;
	//io.ConfigDockingTransparentPayload = true;

	// 플랫폼/렌더러 백엔드 초기화
	ImGui_ImplWin32_Init(g_hWnd);
	ImGui_ImplDX11_Init(m_pDevice, m_pContext);

	// 스타일 설정
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();

	// Setup scaling
	ImGuiStyle& style = ImGui::GetStyle();
	style.ScaleAllSizes(main_scale);        // Bake a fixed style scale. (until we have a solution for dynamic style scaling, changing this requires resetting Style + calling this again)
	style.FontScaleDpi = main_scale;        // Set initial font scale. (in docking branch: using io.ConfigDpiScaleFonts=true automatically overrides this for every window depending on the current monitor)
	io.ConfigDpiScaleFonts = true;          // [Experimental] Automatically overwrite style.FontScaleDpi in Begin() when Monitor DPI changes. This will scale fonts but _NOT_ scale sizes/padding for now.
	io.ConfigDpiScaleViewports = true;      // [Experimental] Scale Dear ImGui and Platform Windows when Monitor DPI changes.

	// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

	m_vClear_color = { 0.45f, 0.55f, 0.60f, 1.00f };

	return S_OK;
}

void CImGui_Manager::Priority_Update(_float fTimeDelta)
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	ImGui::DockSpaceOverViewport();
}

void CImGui_Manager::Update(_float fTimeDelta)
{
	switch (m_eCurMode)
	{
		case EDITOR_MODE::DEMO:
			Update_Example(fTimeDelta);
			break;

		case EDITOR_MODE::MAP:
			Update_MapTool(fTimeDelta);
			break;

		case EDITOR_MODE::OBJECT:
			Update_ObjectTool(fTimeDelta);
			break;

		case EDITOR_MODE::UI:
			Update_UITool(fTimeDelta);
			break;

		case EDITOR_MODE::EFFECT:
			Update_EffectTool(fTimeDelta);
			break;

		default:
			break;
	}

}

void CImGui_Manager::Late_Update(_float fTimeDelta)
{
	ImGui::End();
}

void CImGui_Manager::Render()
{
	ImGuiIO& io = ImGui::GetIO();

	ImGui::Render();
	const _float clear_color_with_alpha[4] = { m_vClear_color.x * m_vClear_color.w, m_vClear_color.y * m_vClear_color.w, m_vClear_color.z * m_vClear_color.w, m_vClear_color.w };
	m_pContext->OMSetRenderTargets(1, &m_pBackBufferRTV, nullptr);
	m_pContext->ClearRenderTargetView(m_pBackBufferRTV, clear_color_with_alpha);

	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
	}
}

void CImGui_Manager::Update_Example(_float fTimeDelta)
{
	ImGuiIO& io = ImGui::GetIO();

	ImGui::ShowDemoWindow();

	// 창 만들기 : Begin/End 함수 쌍을 이용하여 이름이 지정된 창을 생성
	ImGui::Begin("Hello, 159");					// "Hello, 159" 라는 창을 생성

	ImGui::Text("I M G U I");					// 텍스트 표시
	ImGui::Checkbox("Another Window", &m_bAnother_Window);

	static float fNum = 0.f;
	ImGui::SliderFloat("float", &fNum, 0.0f, 1.0f);			// 0.f에서 1.f까지 조절 가능한 슬라이더 생성


	ImGui::ColorEdit3("clear color", (float*)&m_vClear_color);		// 3개의 float 값을 편집하여 색상을 조절

	static int iCnt = 0;
	if (ImGui::Button("Button")) iCnt++;		// 버튼을 클릭하면 true 반환 (대부분의 위젯은 편집되거나 활성화될 때 true 반환)

	ImGui::SameLine();							// 같은 줄에 다음 위젯 배치
	ImGui::Text("counter = %d", iCnt);

	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);

	if (m_bAnother_Window)
	{
		ImGui::Begin("Another Window", &m_bAnother_Window);
		
		ImGui::Text("Hello from another window!");
		
		if (ImGui::Button("Close Me"))
			m_bAnother_Window = false;
		
		ImGui::End();
	}
}

void CImGui_Manager::Update_Main(_float fTimeDelta)
{
}

void CImGui_Manager::Update_MapTool(_float fTimeDelta)
{
}

void CImGui_Manager::Update_ObjectTool(_float fTimeDelta)
{
}

void CImGui_Manager::Update_UITool(_float fTimeDelta)
{
}

void CImGui_Manager::Update_EffectTool(_float fTimeDelta)
{
}

void CImGui_Manager::Update_Gizmo(_float fTimeDelta)
{
}

void CImGui_Manager::Free()
{
	Safe_Release(m_pBackBufferRTV);
	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}