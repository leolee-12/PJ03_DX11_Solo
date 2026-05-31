#include "Panel_OutLiner.h"
#include "GameInstance.h"
#include "EditInstance.h"

CPanel_OutLiner::CPanel_OutLiner()
	: CPanel_Base()
{
}

HRESULT CPanel_OutLiner::Initialize()
{
	m_strTitle = "OutLiner";

	if (FAILED(__super::Initialize()))
		return E_FAIL;

	return S_OK;
}

void CPanel_OutLiner::Update(_float fTimeDelta)
{
}

HRESULT CPanel_OutLiner::Render()
{
	if (!Begin_Panel())
	{
		End_Panel();
		return S_OK;
	}

	ImGui::InputText("##search", m_szSearchBuffer, 128);

	static int iCnt = 0;
	if (ImGui::Button("Add_Object")) iCnt++;

	ImGui::Separator();
	
	_uint iLevel = static_cast<_uint>(m_pGameInstance->Get_CurrentLevel());
	auto vecObjects = m_pGameInstance->Get_LevelObjects(iLevel);
	for (auto pObj : vecObjects)
		Draw_ObjectNode(pObj);

	if (m_bOpenRenamePopup)
	{
		ImGui::OpenPopup("##rename_popup");
		m_bOpenRenamePopup = false;
	}

	if (ImGui::BeginPopup("##rename_popup") && m_pRenameTarget)
	{
		ImGui::Text(KOR("새 이름:"));
		if (ImGui::InputText("##rename", m_szRenameBuffer, 128,
			ImGuiInputTextFlags_EnterReturnsTrue))
		{
			m_pRenameTarget->Set_Name(StoW(m_szRenameBuffer));
			m_pRenameTarget = nullptr;
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}

	End_Panel();

	return S_OK;
}

void CPanel_OutLiner::Draw_ObjectNode(CGameObject* pObj)
{
	const _bool bSelected = m_pEditInstance->Is_Selected(pObj);
	string strName = WtoS(pObj->Get_Name());

	// 검색 : 검색어 있으면 이름에 포함되지 않는 오브젝트 스킵
	if (!Passes_Filter(strName)) // ← 인라인 중복 로직 제거, 함수 사용
		return;

	ImGuiTreeNodeFlags iNodeFlags =
		ImGuiTreeNodeFlags_Leaf
		| ImGuiTreeNodeFlags_SpanFullWidth
		| ImGuiTreeNodeFlags_FramePadding;

	if (bSelected) iNodeFlags |= ImGuiTreeNodeFlags_Selected;

	_bool bOpen = ImGui::TreeNodeEx((void*)pObj, iNodeFlags, "%s", strName.c_str());

	// 좌클릭 : 선택/해제, Ctrl+좌클릭 : 멀티 선택
	if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
	{
		if (bSelected)
			m_pEditInstance->Deselect(pObj);
		else
			m_pEditInstance->Select(pObj, ImGui::GetIO().KeyCtrl);
	}

	// 우클릭 : 메뉴
	if (ImGui::BeginPopupContextItem())
	{
		Draw_ContextMenu(pObj);
		ImGui::EndPopup();
	}

	if (bOpen)
		ImGui::TreePop();
}

void CPanel_OutLiner::Draw_ContextMenu(CGameObject* pObj)
{
	// 이름 변경
	if (ImGui::MenuItem(KOR("이름 변경")))
	{
		m_pRenameTarget = pObj;
		// 현재 이름을 버퍼에 복사해서 팝업 InputText에 표시
		strncpy_s(m_szRenameBuffer, WtoS(pObj->Get_Name()).c_str(), 127);
		m_bOpenRenamePopup = true;
	}

	// 복제: 동일 타입 Clone 후 에디터 레지스트리에 등록
	if (ImGui::MenuItem(KOR("복제")))
	{
		m_pEditInstance->Clone_Object(pObj);
	}

	// 삭제
	if (ImGui::MenuItem(KOR("삭제")))
	{
		m_pEditInstance->Deselect(pObj);
		m_pEditInstance->Unregister_Object(pObj);
	}
}

_bool CPanel_OutLiner::Passes_Filter(const _string& strName)
{
	if (m_szSearchBuffer[0] == '\0') return true;

	_string lower_name = strName;
	_string lower_buf = m_szSearchBuffer;
	transform(lower_name.begin(), lower_name.end(), lower_name.begin(),
		[](unsigned char c) { return (char)::tolower(c); }); // ← unsigned char 캐스트
	transform(lower_buf.begin(), lower_buf.end(), lower_buf.begin(),
		[](unsigned char c) { return (char)::tolower(c); });

	return lower_name.find(lower_buf) != _string::npos;
}

CPanel_OutLiner* CPanel_OutLiner::Create()
{
	CPanel_OutLiner* pInstance = new CPanel_OutLiner();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Create : CPanel_OutLiner");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPanel_OutLiner::Free()
{
	__super::Free();
}
