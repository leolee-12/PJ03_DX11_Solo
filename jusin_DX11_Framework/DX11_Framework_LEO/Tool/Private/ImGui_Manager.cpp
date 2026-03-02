#include "ImGui_Manager.h"

IMPLEMENT_SINGLETON(CImGui_Manager)

CImGui_Manager::CImGui_Manager()
{
}

HRESULT CImGui_Manager::Ready_ImGui(HWND hWnd, _Inout_ ID3D11Device** ppDevice, _Inout_ ID3D11DeviceContext** ppContext)
{
	// 장치 전달
	m_pDevice = *ppDevice;
	m_pContext = *ppContext;
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);

	// ImGui 컨텍스트 생성
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // IF using Docking Branch

	// 플랫폼/렌더러 백엔드 초기화
	ImGui_ImplWin32_Init(g_hWnd);
	ImGui_ImplDX11_Init(m_pDevice, m_pContext);

	// 스타일 설정
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();

	return S_OK;
}

void CImGui_Manager::Priority_Update()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void CImGui_Manager::Update()
{
	switch (m_eCurMode)
	{
		case EDITOR_MODE::DEMO:
			Update_Example();
			break;

		case EDITOR_MODE::MAP:
			Update_MapTool();
			break;

		case EDITOR_MODE::OBJECT:
			Update_ObjectTool();
			break;

		case EDITOR_MODE::UI:
			Update_UITool();
			break;

		case EDITOR_MODE::EFFECT:
			Update_EffectTool();
			break;

		default:
			break;
	}

}

void CImGui_Manager::Late_Update()
{
	ImGui::End();
}

void CImGui_Manager::Render()
{
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void CImGui_Manager::Update_Example()
{
	ImGuiIO& io = ImGui::GetIO();

	ImGui::ShowDemoWindow();

	// 창 만들기 : Begin/End 함수 쌍을 이용하여 이름이 지정된 창을 생성
	ImGui::Begin("Hello, 159");					// "Hello, 159" 라는 창을 생성

	ImGui::Text("I M G U I");					// 텍스트 표시

	static float fNum = 0.f;
	ImGui::SliderFloat("float", &fNum, 0.0f, 1.0f);			// 0.f에서 1.f까지 조절 가능한 슬라이더 생성

	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
	ImGui::ColorEdit3("clear color", (float*)&clear_color);		// 3개의 float 값을 편집하여 색상을 조절

	static int iCnt = 0;
	if (ImGui::Button("Button")) iCnt++;		// 버튼을 클릭하면 true 반환 (대부분의 위젯은 편집되거나 활성화될 때 true 반환)

	ImGui::SameLine();							// 같은 줄에 다음 위젯 배치
	ImGui::Text("counter = %d", iCnt);

	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
}

void CImGui_Manager::Update_Main()
{
}

void CImGui_Manager::Update_MapTool()
{
}

void CImGui_Manager::Update_ObjectTool()
{
}

void CImGui_Manager::Update_UITool()
{
}

void CImGui_Manager::Update_EffectTool()
{
}

void CImGui_Manager::Update_Gizmo()
{
}

void CImGui_Manager::Free()
{
	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}