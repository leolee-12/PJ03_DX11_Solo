#include "Editor_Defines.h"
#include "UIEditorSession.h"

NS_BEGIN(Editor)

void Draw_VPModeRadio(CUIEditorSession* pSession, const char* pszId)
{
    using V = CUIEditorSession::VPMODE;
    const V eCurrent = pSession->Get_VPMode();

    ImGui::PushID(pszId);
    ImGui::TextUnformatted("Viewport Tool:");
    ImGui::SameLine();

    struct ModeEntry { const char* pszLabel; V eMode; };
    static const ModeEntry kEntries[] =
    {
        { "Scene",     V::SCENE     },
        { "UI Layout", V::UI_LAYOUT },
        { "UI Anim",   V::UI_ANIM   },
    };

    for (const ModeEntry& tEntry : kEntries)
    {
        const _bool bSel = (eCurrent == tEntry.eMode);
        if (bSel)
        {
            const ImU32 c = IM_COL32(255, 200, 80, 255);
            ImGui::PushStyleColor(ImGuiCol_Button, c);
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, c);
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, c);
        }
        if (ImGui::Button(tEntry.pszLabel))
            pSession->Set_VPMode(tEntry.eMode);
        if (bSel)
            ImGui::PopStyleColor(3);
        ImGui::SameLine();
    }

    ImGui::NewLine();
    ImGui::PopID();
}

NS_END