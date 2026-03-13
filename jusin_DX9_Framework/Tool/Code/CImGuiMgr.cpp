#include "pch.h"
#include "CImGuiMgr.h"

IMPLEMENT_SINGLETON(CImGuiMgr)

CImGuiMgr::CImGuiMgr()
	: m_bDemo(true),
	m_bMapTool(false),
	m_bObjTool(false),
	m_bEffectTool(false)
{
}

CImGuiMgr::~CImGuiMgr()
{
	Free();
}

HRESULT CImGuiMgr::Ready_ImGui(HWND hWnd, LPDIRECT3DDEVICE9 pGraphicDev)
{
	// 장치 전달
	m_pGraphicDev = pGraphicDev;
	m_pGraphicDev->AddRef();

	// ImGui 컨텍스트 생성
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	/*ImGuiIO& io = ImGui::GetIO();*/

	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;	//  Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;	// Enable Gamepad Controls
	//io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;		// IF using Docking Branch

	// 플랫폼/렌더러 백엔드 초기화
	ImGui_ImplWin32_Init(hWnd);
	ImGui_ImplDX9_Init(pGraphicDev);

	// 스타일 설정
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();

	return S_OK;
}

void CImGuiMgr::PriorityUpdate_ImGui()
{
}

void CImGuiMgr::Update_ImGui()
{
	ImGuiIO& io = ImGui::GetIO();

	// 새 프레임 시작
	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	// 테스트용 데모 창
	if (m_bDemo) ImGui::ShowDemoWindow();

	// 창 만들기 : Begin/End 함수 쌍을 이용하여 이름이 지정된 창을 생성
	ImGui::Begin("Hello, 159");					// "Hello, 159" 라는 창을 생성

	ImGui::Text("I M G U I");					// 텍스트 표시
	
	ImGui::Checkbox("Demo Window", &m_bDemo);	// 창의 열림/닫힘 상태를 저장하는 bool 값을 편집

	static float fNum = 0.f;
	ImGui::SliderFloat("float", &fNum, 0.0f, 1.0f);			// 0.f에서 1.f까지 조절 가능한 슬라이더 생성
	
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
	ImGui::ColorEdit3("clear color", (float*)&clear_color);		// 3개의 float 값을 편집하여 색상을 조절

	static int iCnt = 0;
	if (ImGui::Button("Button")) iCnt++;		// 버튼을 클릭하면 true 반환 (대부분의 위젯은 편집되거나 활성화될 때 true 반환)

	ImGui::SameLine();							// 같은 줄에 다음 위젯 배치
	ImGui::Text("counter = %d", iCnt);

	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);

	ImGui::End();
}

void CImGuiMgr::LateUpdate_ImGui()
{
}

void CImGuiMgr::Render_ImGui()
{


	ImGui::Render();
	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
}

void CImGuiMgr::Render_Main()
{
}

void CImGuiMgr::Render_MapTool()
{
}

void CImGuiMgr::Render_ObjTool()
{
}

void CImGuiMgr::Render_EffectTool()
{
}

void CImGuiMgr::Render_Gizmo()
{
	//ImGuizmo::BeginFrame();

	_matrix matView;
	D3DXMatrixIdentity(&matView);
}

void CImGuiMgr::Free()
{
	Safe_Release(m_pGraphicDev);
	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}