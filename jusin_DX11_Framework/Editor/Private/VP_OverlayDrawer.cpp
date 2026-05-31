#include "VP_OverlayDrawer.h"
#include "VP_CoordMapper.h"
#include "UIEditorSession.h"
#include "UIPreviewHost.h"
#include "EditInstance.h"

#include "UIObject.h"

CVP_OverlayDrawer::CVP_OverlayDrawer()
	: m_pEditInstance(CEditInstance::GetInstance())
{
	Safe_AddRef(m_pEditInstance);
}

HRESULT CVP_OverlayDrawer::Initialize(CVP_CoordMapper* pMapper)
{
	if (nullptr == pMapper)
		return E_FAIL;

	m_pMapper = pMapper;
	Safe_AddRef(m_pMapper);

	return S_OK;
}

void CVP_OverlayDrawer::Draw(_bool bHovered)
{
	auto* pHost = m_pEditInstance->Get_UIPreviewHost();
	auto* pSess = m_pEditInstance->Get_UISession();
	if (nullptr == pHost || nullptr == pSess) return;

	const auto& vWidgets = pHost->Get_Widgets();
	const auto& vZIdx = pHost->Get_ZOrderIdx();
	if (vWidgets.empty()) return;

	const auto& tDoc = pSess->Get_Doc();
	const auto& vDocW = tDoc.vWidgets;

	const ImVec2& vDisplayPos = m_pMapper->Get_DisplayPos();
	const ImVec2& vDisplaySize = m_pMapper->Get_DisplaySize();

	ImDrawList* dl = ImGui::GetWindowDrawList();
	dl->PushClipRect(
		vDisplayPos,
		ImVec2(vDisplayPos.x + vDisplaySize.x, vDisplayPos.y + vDisplaySize.y),
		true);

	const _int iSelected = pSess->Get_SelectedWidget();

	// 1. 모든 widget bounding
	for (_int idx : vZIdx)
	{
		if (idx < 0 || idx >= (_int)vWidgets.size())
			continue;

		CUIObject* p = vWidgets[idx];
		if (!p)
			continue;

		const _float4 rc = p->Get_DesignRect();
		ImVec2 vMin, vMax;
		m_pMapper->DocRectToScreen(rc, &vMin, &vMax);

		ImU32 col = IM_COL32(180, 180, 180, 180);
		if (idx < (_int)vDocW.size())
		{
			switch (vDocW[idx].Get_Type())
			{
			case UI_TYPE::CONTAINER:   col = IM_COL32(160, 200, 255, 200); break;
			case UI_TYPE::IMAGE:       col = IM_COL32(200, 200, 200, 200); break;
			case UI_TYPE::TEXT:        col = IM_COL32(255, 220, 120, 200); break;
			case UI_TYPE::BUTTON:      col = IM_COL32(180, 255, 180, 200); break;
			case UI_TYPE::PROGRESSBAR: col = IM_COL32(255, 180, 180, 200); break;
			}
		}

		dl->AddRect(vMin, vMax, col, 0.f, 0, 1.f);
	}

	// 2. selected outline
	if (iSelected >= 0 && iSelected < (_int)vWidgets.size())
	{
		if (CUIObject* pSelected = vWidgets[iSelected])
		{
			const _float4 rc = pSelected->Get_DesignRect();
			ImVec2 vMin, vMax;
			m_pMapper->DocRectToScreen(rc, &vMin, &vMax);

			dl->AddRect(vMin, vMax, IM_COL32(255, 255, 0, 255), 0.f, 0, 2.f);
		}
	}

	// 3. hover label
	if (bHovered)
	{
		const ImVec2 vMouse = ImGui::GetMousePos();
		const ImVec2 vDoc = m_pMapper->ScreenToDoc(vMouse);
		const _string strHit = pHost->Hit_Test_TopMost(vDoc);
		if (!strHit.empty())
		{
			const _int iIdx = pSess->Find_WidgetIndexById(strHit);
			if (iIdx >= 0 && iIdx < (_int)vDocW.size())
			{
				char szBuf[128] = {};
				sprintf_s(szBuf, "[%s] %s",
					Engine::To_String(vDocW[iIdx].Get_Type()),
					vDocW[iIdx].strDisplayName.c_str());

				dl->AddText(
					ImVec2(vMouse.x + 12.f, vMouse.y + 12.f),
					IM_COL32(255, 255, 255, 240),
					szBuf);
			}
		}
	}

	dl->PopClipRect();
}


CVP_OverlayDrawer* CVP_OverlayDrawer::Create(CVP_CoordMapper* pMapper)
{
    CVP_OverlayDrawer* pInstance = new CVP_OverlayDrawer();

    if (FAILED(pInstance->Initialize(pMapper)))
    {
        MSG_BOX("Failed to Create : CVP_OverlayDrawer");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CVP_OverlayDrawer::Free()
{
    __super::Free();

    Safe_Release(m_pMapper);
    Safe_Release(m_pEditInstance);
}