#include "Panel_Base.h"
#include "GameInstance.h"
#include "EditInstance.h"

CPanel_Base::CPanel_Base()
	: m_pGameInstance(CGameInstance::GetInstance())
	, m_pEditInstance(CEditInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pEditInstance);
}

HRESULT CPanel_Base::Initialize(void* pArg)
{
	m_pEditInstance->Register_Callback(m_strTitle,
		[this](const vector<CGameObject*>& sel)
		{
			m_pSelected = sel.empty() ? nullptr : sel[0];
		});

	return S_OK;
}

void CPanel_Base::Update(_float fTimeDelta)
{
}

HRESULT CPanel_Base::Render()
{
	// 공식 예시 패널
	ImGui::ShowDemoWindow();

	if (!Begin_Panel())
	{
		End_Panel();	// 접혀있어도 Begin은 호출된 것 : End를 반드시 호출
		return S_OK;
	}

	ImGuiIO& io = ImGui::GetIO();

	ImGui::Text("I M G U I");					// 텍스트 표시

	//ImGui::Checkbox("CheckBox", &m_bChecked);

	static float fNum = 0.f;
	ImGui::SliderFloat("float", &fNum, 0.0f, 1.0f);			// 0.f에서 1.f까지 조절 가능한 슬라이더 생성


	ImGui::ColorEdit3("clear color", (float*)&m_vClear_color);		// 3개의 float 값을 편집하여 색상을 조절

	static int iCnt = 0;
	if (ImGui::Button("Button")) iCnt++;		// 버튼을 클릭하면 true 반환 (대부분의 위젯은 편집되거나 활성화될 때 true 반환)

	ImGui::SameLine();							// 같은 줄에 다음 위젯 배치

	ImGui::Text("counter = %d", iCnt);

	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);

	End_Panel();

	return S_OK;
}

_bool CPanel_Base::Begin_Panel()
{
	// ImGui_Manager에서 Is_Opened() 체크 후 호출 : m_bOpen은 항상 true
    return ImGui::Begin(m_strTitle.c_str(), &m_bOpen, m_iWindowFlags);
	// 패널이 접혀있으면 false 반환
}

void CPanel_Base::End_Panel()
{
	ImGui::End();
}

void CPanel_Base::Free()
{
	__super::Free();

	m_pEditInstance->Unregister_Callback(m_strTitle);
	Safe_Release(m_pEditInstance);
	Safe_Release(m_pGameInstance);
}
