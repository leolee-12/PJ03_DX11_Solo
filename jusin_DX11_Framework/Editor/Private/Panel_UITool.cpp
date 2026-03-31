#include "Panel_UITool.h"
#include "GameInstance.h"
#include "EditInstance.h"

CPanel_UITool::CPanel_UITool()
	: CPanel_Base()
{
}

HRESULT CPanel_UITool::Initialize()
{
	m_strTitle = "UI";

	if (FAILED(__super::Initialize()))
		return E_FAIL;

	return S_OK;
}

void CPanel_UITool::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
}

HRESULT CPanel_UITool::Render()
{
    if (!Begin_Panel()) { End_Panel(); return S_OK; }

	// --- 상단: 요소 추가/선택 --------------------------
    Draw_UIList();

    ImGui::Separator();

	// --- 중앙: 2D 캔버스 프리뷰 -----------------------
    Draw_Canvas();

    ImGui::Separator();

	// --- 하단: 선택 요소 프로퍼티 편집 ----------------------
    if (m_iSelectedIdx >= 0)
        Draw_ElementProps(m_Elements[m_iSelectedIdx]);

	// --- 저장/불러오기 버튼 ----------------------
    if (ImGui::Button(KOR("저장")))
        m_pEditInstance->Save_UILayout("../../Data/UI/HUD_Layout.json", m_Elements);
    ImGui::SameLine();
    if (ImGui::Button(KOR("불러오기")))
		m_pEditInstance->Load_UILayout("../../Data/UI/HUD_Layout.json", m_Elements);

    End_Panel();
    return S_OK;
}

void CPanel_UITool::Draw_UIList()
{
    // 새 요소 추가
    if (ImGui::Button(KOR("+ UI 요소 추가")))
    {
        UI_ELEMENT newElement{};
        newElement.id = "element_" + to_string(m_Elements.size());
        newElement.displayName = KOR("새 요소");
        newElement.tDesc.fCenterX = static_cast<_float>(g_iWinSizeX) * 0.5f;
        newElement.tDesc.fCenterY = static_cast<_float>(g_iWinSizeY) * 0.5f;
        newElement.tDesc.fSizeX = 200.f;
        newElement.tDesc.fSizeY = 50.f;
        m_Elements.push_back(newElement);
        m_iSelectedIdx = (int)m_Elements.size() - 1;
    }

    // 요소 목록 (Selectable)
    for (int i = 0; i < (int)m_Elements.size(); ++i)
    {
        bool bSel = (m_iSelectedIdx == i);
        if (ImGui::Selectable(m_Elements[i].displayName.c_str(), bSel))
            m_iSelectedIdx = i;

        // 우클릭 → 삭제
        if (ImGui::BeginPopupContextItem())
        {
            if (ImGui::MenuItem(KOR("삭제")))
            {
                if (m_iSelectedIdx == i)
                    m_iSelectedIdx = -1;
                m_Elements.erase(m_Elements.begin() + i);

                if (m_iSelectedIdx >= i) --m_iSelectedIdx;
                if (m_iSelectedIdx < 0 && !m_Elements.empty()) m_iSelectedIdx = -1;

                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
    }
}

void CPanel_UITool::Draw_Canvas()
{
    ImDrawList* pDraw = ImGui::GetWindowDrawList();
    ImVec2 canvasOrigin = ImGui::GetCursorScreenPos();

    // 캔버스 영역 계산 (화면 비율 유지)
    float aspect = (float)g_iWinSizeX / (float)g_iWinSizeY;
    ImVec2 available = ImGui::GetContentRegionAvail();
    available.y = min(available.y, 200.f);   // 최대 높이 제한
    float canvasW = min(available.x, available.y * aspect);
    float canvasH = canvasW / aspect;
    float scale = canvasW / (float)g_iWinSizeX;

    // 배경 (게임 화면 영역 표시)
    pDraw->AddRectFilled(canvasOrigin,
        ImVec2(canvasOrigin.x + canvasW, canvasOrigin.y + canvasH),
        IM_COL32(30, 30, 30, 200));
    pDraw->AddRect(canvasOrigin,
        ImVec2(canvasOrigin.x + canvasW, canvasOrigin.y + canvasH),
        IM_COL32(100, 100, 100, 255));

    // 각 UI 요소를 캔버스에 사각형으로 표시
    for (_uint i = 0; i < m_Elements.size(); ++i)
    {
		UI_ELEMENT el = m_Elements[i];

        float cx = canvasOrigin.x + el.tDesc.fCenterX * scale;
        float cy = canvasOrigin.y + el.tDesc.fCenterY * scale;
        float hw = el.tDesc.fSizeX * scale * 0.5f;
        float hh = el.tDesc.fSizeY * scale * 0.5f;

        ImU32 col = (m_iSelectedIdx == i)
            ? IM_COL32(255, 200, 0, 200)   // 선택: 노랑
            : IM_COL32(0, 150, 255, 150);  // 기본: 파랑

        pDraw->AddRectFilled(
            ImVec2(cx - hw, cy - hh), ImVec2(cx + hw, cy + hh), col);
        pDraw->AddRect(
            ImVec2(cx - hw, cy - hh), ImVec2(cx + hw, cy + hh),
            IM_COL32(255, 255, 255, 255));
        pDraw->AddText(ImVec2(cx - hw + 2, cy - hh + 2),
            IM_COL32(255, 255, 255, 255),
            el.displayName.c_str());

        // 캔버스 클릭으로 요소 선택
        ImGui::SetCursorScreenPos(ImVec2(cx - hw, cy - hh));
        ImGui::InvisibleButton(
            ("##canvas_" + el.id).c_str(),
            ImVec2(el.tDesc.fSizeX * scale, el.tDesc.fSizeY * scale));
        if (ImGui::IsItemClicked())
            m_iSelectedIdx = i;
    }

    // 캔버스 영역 소비 (커서 이동)
    ImGui::SetCursorScreenPos(
        ImVec2(canvasOrigin.x, canvasOrigin.y + canvasH + 4.f));
}

void CPanel_UITool::Draw_ElementProps(UI_ELEMENT& tElement)
{
    if (!ImGui::CollapsingHeader(KOR("UI 요소 속성"),
        ImGuiTreeNodeFlags_DefaultOpen)) return;

    // 이름
    char nameBuf[128]; strncpy_s(nameBuf, tElement.displayName.c_str(), 127);
    if (ImGui::InputText(KOR("이름"), nameBuf, 128)) tElement.displayName = nameBuf;

    // 위치 (픽셀 절대 좌표)
    ImGui::DragFloat(KOR("중심 X"), &tElement.tDesc.fCenterX, 1.f, 0.f, (float)g_iWinSizeX);
    ImGui::DragFloat(KOR("중심 Y"), &tElement.tDesc.fCenterY, 1.f, 0.f, (float)g_iWinSizeY);

    // 크기
    ImGui::DragFloat(KOR("너비"), &tElement.tDesc.fSizeX, 1.f, 1.f, (float)g_iWinSizeX);
    ImGui::DragFloat(KOR("높이"), &tElement.tDesc.fSizeY, 1.f, 1.f, (float)g_iWinSizeY);

    // 앵커 콤보
    static const char* anchors[] =
    { "TL","TC","TR","ML","MC","MR","BL","BC","BR" };
    int anchorIdx = 0;
    for (int i = 0; i < 9; i++)
        if (tElement.anchor == anchors[i]) { anchorIdx = i; break; }

    if (ImGui::Combo(KOR("앵커"), &anchorIdx, anchors, 9))
        tElement.anchor = anchors[anchorIdx];

    // 색상
    ImGui::ColorEdit4(KOR("색상"), tElement.color);

    // 스프라이트 이름
    char spriteBuf[128]; strncpy_s(spriteBuf, tElement.spriteName.c_str(), 127);
    if (ImGui::InputText(KOR("스프라이트"), spriteBuf, 128))
        tElement.spriteName = spriteBuf;
}

CPanel_UITool* CPanel_UITool::Create()
{
	CPanel_UITool* pInstance = new CPanel_UITool();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Create : CPanel_UITool");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPanel_UITool::Free()
{
	__super::Free();
}
