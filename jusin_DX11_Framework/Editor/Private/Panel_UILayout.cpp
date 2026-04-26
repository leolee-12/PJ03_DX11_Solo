#include "Panel_UILayout.h"
#include "UIEditorSession.h"

#include "GameInstance.h"
#include "EditInstance.h"

CPanel_UILayout::CPanel_UILayout()
	: CPanel_Base()
	, m_pSession(m_pEditInstance->Get_UISession())
{
	Safe_AddRef(m_pSession);
}

HRESULT CPanel_UILayout::Initialize()
{
	m_strTitle = "UI_Layout";

	if (FAILED(__super::Initialize()))
		return E_FAIL;

	return S_OK;
}

void CPanel_UILayout::Update(_float fTimeDelta)
{
}

HRESULT CPanel_UILayout::Render()
{
	if (!Begin_Panel())
	{
		End_Panel();
		return S_OK;
	}

	ImGui::PushID(m_strTitle.c_str());

	Draw_Toolbar();
	ImGui::Separator();

	const ImVec2 vAvail = ImGui::GetContentRegionAvail();
	const float fHierarchyH = std::clamp(vAvail.y * 0.35f, 160.f, 320.f);

	if (ImGui::BeginChild("Hierarchy", ImVec2(0.f, fHierarchyH), true))
		Draw_Hierarchy();
	ImGui::EndChild();

	if (ImGui::BeginChild("Inspector", ImVec2(0.f, 0.f), true))
		Draw_Inspector();
	ImGui::EndChild();

	ImGui::PopID();
	End_Panel();
	return S_OK;
}

void CPanel_UILayout::Draw_Toolbar()
{
	namespace fs = std::filesystem;
	const _string strDir = "../../DataFiles/UI/";

	// ── Row 1: 파일 콤보 (좌측 라벨 + 남은 폭 가득) ──
	{
		Label_Left("File");
		ImGui::SetNextItemWidth(-FLT_MIN);
		_string strCurPath = m_pSession->Get_DocPath();
		if (ImGui::BeginCombo("##file_combo", strCurPath.c_str()))
		{
			std::error_code ec;
			if (fs::exists(strDir, ec))
			{
				for (const auto& entry : fs::directory_iterator(strDir, ec))
				{
					if (!entry.is_regular_file()) continue;
					if (entry.path().extension() != ".uiseq") continue;
					const _string strPath = entry.path().string();
					const _bool bSel = (strPath == strCurPath);
					if (ImGui::Selectable(strPath.c_str(), bSel) && !bSel)
						m_pSession->Set_DocPath(strPath);
				}
			}
			ImGui::EndCombo();
		}
	}

	// ── Row 2: 경로 편집 ──
	{
		Label_Left("Path");
		ImGui::SetNextItemWidth(-FLT_MIN);
		_string strDocPath = m_pSession->Get_DocPath();
		if (Edit_StringField<512>("##path_edit", strDocPath))
			m_pSession->Set_DocPath(strDocPath);
	}

	// ── Row 3: 액션 버튼 ──
	if (ImGui::Button("New"))
	{
		if (m_pSession->Is_Dirty())
		{
			m_ePendingAction = PENDING_ACTION::NEW_DOC;
			ImGui::OpenPopup("Discard Changes?");
		}
		else m_pSession->New_Doc();
	}
	ImGui::SameLine();
	if (ImGui::Button("Load"))
	{
		const _string strPath = m_pSession->Get_DocPath();
		if (m_pSession->Is_Dirty())
		{
			m_ePendingAction = PENDING_ACTION::LOAD;
			m_strPendingPath = strPath;
			ImGui::OpenPopup("Discard Changes?");
		}
		else m_pSession->Load(strPath);
	}
	ImGui::SameLine();
	if (ImGui::Button("Save"))
		m_pSession->Save(m_pSession->Get_DocPath());

	// ── Modal (액션 버튼 직후로 이동, 가독성) ──
	if (ImGui::BeginPopupModal("Discard Changes?", nullptr,
		ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::TextUnformatted("Unsaved changes will be lost.");
		ImGui::TextUnformatted("Continue?");
		ImGui::Separator();
		if (ImGui::Button("Save and Continue", ImVec2(160, 0)))
		{
			if (SUCCEEDED(m_pSession->Save(m_pSession->Get_DocPath())))
			{
				switch (m_ePendingAction)
				{
				case PENDING_ACTION::NEW_DOC: m_pSession->New_Doc(); break;
				case PENDING_ACTION::LOAD:    m_pSession->Load(m_strPendingPath); break;
				default: break;
				}
			}
			m_ePendingAction = PENDING_ACTION::NONE;
			m_strPendingPath.clear();
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button("Discard", ImVec2(120, 0)))
		{
			switch (m_ePendingAction)
			{
			case PENDING_ACTION::NEW_DOC: m_pSession->New_Doc(); break;
			case PENDING_ACTION::LOAD:    m_pSession->Load(m_strPendingPath); break;
			default: break;
			}
			m_ePendingAction = PENDING_ACTION::NONE;
			m_strPendingPath.clear();
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel", ImVec2(80, 0)))
		{
			m_ePendingAction = PENDING_ACTION::NONE;
			m_strPendingPath.clear();
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}

	// ── Row 4: 상태 (별도 줄) ──
	ImGui::TextColored(
		m_pSession->Is_Dirty() ? ImVec4(1.f, 0.7f, 0.2f, 1.f) : ImVec4(0.7f, 0.7f, 0.7f, 1.f),
		"%s%s",
		m_pSession->Is_Dirty() ? "* " : "",
		m_pSession->Get_Status().c_str());

	ImGui::Separator();
	Draw_VPModeRadio(m_pSession, "vpmode_layout");
}

void CPanel_UILayout::Draw_Hierarchy()
{
	ImGui::TextUnformatted("Hierarchy");

	auto AddWidget = [&](UI_TYPE eType)
		{
			UISEQ_DOC& tDoc = m_pSession->Get_DocMutable();
			tDoc.vWidgets.push_back(m_pSession->Make_DefaultWidget(eType));
			m_pSession->Set_SelectedWidget(static_cast<_int>(tDoc.vWidgets.size()) - 1);
			m_pSession->Mark_Dirty("Widget added");
		};

	if (ImGui::Button("Image"))       AddWidget(UI_TYPE::IMAGE);       ImGui::SameLine();
	if (ImGui::Button("Text"))        AddWidget(UI_TYPE::TEXT);        ImGui::SameLine();
	if (ImGui::Button("Button"))      AddWidget(UI_TYPE::BUTTON);      ImGui::SameLine();
	if (ImGui::Button("ProgressBar")) AddWidget(UI_TYPE::PROGRESSBAR); ImGui::SameLine();
	if (ImGui::Button("Container"))   AddWidget(UI_TYPE::CONTAINER);

	const _int iSel = m_pSession->Get_SelectedWidget();
	UISEQ_DOC& tDoc = m_pSession->Get_DocMutable();

	const _bool bHasSel = (iSel >= 0 && iSel < static_cast<_int>(tDoc.vWidgets.size()));

	ImGui::BeginDisabled(!bHasSel);
	if (ImGui::Button("Delete"))    m_pSession->Erase_Widget(iSel);
	ImGui::SameLine();
	if (ImGui::Button("Duplicate")) m_pSession->Duplicate_Widget(iSel);
	ImGui::SameLine();
	if (ImGui::Button("Up") && iSel > 0)
	{
		std::swap(tDoc.vWidgets[iSel], tDoc.vWidgets[iSel - 1]);
		m_pSession->Set_SelectedWidget(iSel - 1);
		m_pSession->Mark_Dirty("Reorder");
	}
	ImGui::SameLine();
	if (ImGui::Button("Down") && bHasSel && iSel + 1 < static_cast<_int>(tDoc.vWidgets.size()))
	{
		std::swap(tDoc.vWidgets[iSel], tDoc.vWidgets[iSel + 1]);
		m_pSession->Set_SelectedWidget(iSel + 1);
		m_pSession->Mark_Dirty("Reorder");
	}
	ImGui::EndDisabled();

	ImGui::Separator();

	if (ImGui::BeginChild("WidgetList", ImVec2(0.f, 0.f), false))
	{
		for (_int i = 0; i < static_cast<_int>(tDoc.vWidgets.size()); ++i)
		{
			const auto& w = tDoc.vWidgets[i];
			const _string strLabel =
				"[" + _string(Engine::To_String(w.Get_Type())) + "] "
				+ w.strDisplayName + "##w" + std::to_string(i);

			if (ImGui::Selectable(strLabel.c_str(), iSel == i))
				m_pSession->Set_SelectedWidget(i);
		}
	}
	ImGui::EndChild();
}

void CPanel_UILayout::Draw_Inspector()
{
	ImGui::TextUnformatted("Inspector");

	const _int iSel = m_pSession->Get_SelectedWidget();
	UISEQ_DOC& tDoc = m_pSession->Get_DocMutable();
	if (iSel < 0 || iSel >= static_cast<_int>(tDoc.vWidgets.size()))
	{
		ImGui::TextUnformatted("Select a widget.");
		return;
	}

	UISEQ_WIDGET_NODE& w = tDoc.vWidgets[iSel];
	auto& tBase = Get_BaseDesc(w);
	auto MarkWidgetUpdated = [&]()
		{
			m_pSession->Mark_Dirty("Widget updated");
		};

	Draw_ReadOnlyString<256>("Id", w.strId);

	if (Edit_StringField<256>("Display Name", w.strDisplayName))
		MarkWidgetUpdated();

	if (ImGui::Checkbox("Visible", &tBase.bVisible))
		MarkWidgetUpdated();

	if (ImGui::DragInt("Z Order", &tBase.iZOrder, 1.f))
		MarkWidgetUpdated();

	_float2 vViewportSize = m_pGameInstance->Get_CurrentRefSize();

	if (ImGui::DragFloat("Size X", &tBase.fSizeX, 1.f, 1.f, vViewportSize.x * 4.f))
		MarkWidgetUpdated();

	if (ImGui::DragFloat("Size Y", &tBase.fSizeY, 1.f, 1.f, vViewportSize.y * 4.f))
		MarkWidgetUpdated();

	if (ImGui::Checkbox("Use Anchored Pos", &tBase.tAnchorDesc.bUseAnchoredPos))
		MarkWidgetUpdated();

	if (tBase.tAnchorDesc.bUseAnchoredPos)
	{
		if (Combo_Enum("Anchor", tBase.tAnchorDesc.eAnchor, Engine::detail::kAnchor))
			MarkWidgetUpdated();

		if (ImGui::DragFloat("Offset X", &tBase.tAnchorDesc.fOffsetX, 1.f))
			MarkWidgetUpdated();

		if (ImGui::DragFloat("Offset Y", &tBase.tAnchorDesc.fOffsetY, 1.f))
			MarkWidgetUpdated();
	}
	else
	{
		if (ImGui::DragFloat("Center X", &tBase.fCenterX, 1.f))
			MarkWidgetUpdated();

		if (ImGui::DragFloat("Center Y", &tBase.fCenterY, 1.f))
			MarkWidgetUpdated();
	}

	float fMargins[4] =
	{
		tBase.tLayoutSlot.vMargin.x,
		tBase.tLayoutSlot.vMargin.y,
		tBase.tLayoutSlot.vMargin.z,
		tBase.tLayoutSlot.vMargin.w,
	};

	if (ImGui::DragFloat4("Layout Margin", fMargins, 1.f))
	{
		tBase.tLayoutSlot.vMargin = { fMargins[0], fMargins[1], fMargins[2], fMargins[3] };
		MarkWidgetUpdated();
	}

	if (ImGui::DragFloat("Desired X", &tBase.tLayoutSlot.fDesiredSizeX, 1.f))
		MarkWidgetUpdated();

	if (ImGui::DragFloat("Desired Y", &tBase.tLayoutSlot.fDesiredSizeY, 1.f))
		MarkWidgetUpdated();

	ImGui::Separator();

	std::visit([&](auto& tDesc)
		{
			using T = std::decay_t<decltype(tDesc)>;

			if constexpr (std::is_same_v<T, CUIImage::UIIMAGE_DESC>)
			{
				if (Edit_TagField<256>("Texture Tag", tDesc.strTextureTag)) MarkWidgetUpdated();
				if (Edit_UIntField("Texture Level", tDesc.iTextureLevel)) MarkWidgetUpdated();
				if (Edit_UIntField("Texture Index", tDesc.iTextureIndex)) MarkWidgetUpdated();
				if (Edit_TagField<256>("Shader Tag", tDesc.strShaderTag)) MarkWidgetUpdated();
				if (Edit_UIntField("Shader Level", tDesc.iShaderLevel)) MarkWidgetUpdated();
				if (Edit_TagField<256>("VIBuffer Tag", tDesc.strVIBufferTag)) MarkWidgetUpdated();
				if (Edit_UIntField("VIBuffer Level", tDesc.iVIBufferLevel)) MarkWidgetUpdated();
				if (ImGui::ColorEdit4("Color", reinterpret_cast<float*>(&tDesc.vColor))) MarkWidgetUpdated();
			}
			else if constexpr (std::is_same_v<T, CUIButton::UIBUTTON_DESC>)
			{
				if (Edit_TagField<256>("Texture Tag", tDesc.strTextureTag)) MarkWidgetUpdated();
				if (Edit_UIntField("Texture Level", tDesc.iTextureLevel)) MarkWidgetUpdated();
				if (Edit_TagField<256>("Shader Tag", tDesc.strShaderTag)) MarkWidgetUpdated();
				if (Edit_UIntField("Shader Level", tDesc.iShaderLevel)) MarkWidgetUpdated();
				if (Edit_TagField<256>("VIBuffer Tag", tDesc.strVIBufferTag)) MarkWidgetUpdated();
				if (Edit_UIntField("VIBuffer Level", tDesc.iVIBufferLevel)) MarkWidgetUpdated();
				if (Edit_UIntField("Normal Index", tDesc.iNormalTextureIndex)) MarkWidgetUpdated();
				if (Edit_UIntField("Hover Index", tDesc.iHoverTextureIndex)) MarkWidgetUpdated();
				if (Edit_UIntField("Pressed Index", tDesc.iPressedTextureIndex)) MarkWidgetUpdated();
				if (Edit_UIntField("Disabled Index", tDesc.iDisabledTextureIndex)) MarkWidgetUpdated();
				if (ImGui::Checkbox("Interactable", &tDesc.bInteractable)) MarkWidgetUpdated();
				if (ImGui::ColorEdit4("Color", reinterpret_cast<float*>(&tDesc.vColor))) MarkWidgetUpdated();
			}
			else if constexpr (std::is_same_v<T, CUIText::UITEXT_DESC>)
			{
				if (Edit_WStringMultiline<1024>("Text", tDesc.strText, ImVec2(0.f, 70.f))) MarkWidgetUpdated();
				if (Edit_TagField<256>("Font Tag", tDesc.strFontTag)) MarkWidgetUpdated();
				if (Combo_Enum("Align", tDesc.eAlign, Engine::detail::kTextAlign)) MarkWidgetUpdated();
				if (ImGui::ColorEdit4("Color", reinterpret_cast<float*>(&tDesc.vColor))) MarkWidgetUpdated();
			}
			else if constexpr (std::is_same_v<T, CUIProgressBar::UIPROGRESSBAR_DESC>)
			{
				if (Edit_TagField<256>("Back Texture Tag", tDesc.strBackTextureTag)) MarkWidgetUpdated();
				if (Edit_UIntField("Back Texture Level", tDesc.iBackTextureLevel)) MarkWidgetUpdated();
				if (Edit_UIntField("Back Texture Index", tDesc.iBackTextureIndex)) MarkWidgetUpdated();
				if (Edit_TagField<256>("Fill Texture Tag", tDesc.strFillTextureTag)) MarkWidgetUpdated();
				if (Edit_UIntField("Fill Texture Level", tDesc.iFillTextureLevel)) MarkWidgetUpdated();
				if (Edit_UIntField("Fill Texture Index", tDesc.iFillTextureIndex)) MarkWidgetUpdated();
				if (Edit_TagField<256>("Shader Tag", tDesc.strShaderTag)) MarkWidgetUpdated();
				if (Edit_UIntField("Shader Level", tDesc.iShaderLevel)) MarkWidgetUpdated();
				if (Edit_TagField<256>("VIBuffer Tag", tDesc.strVIBufferTag)) MarkWidgetUpdated();
				if (Edit_UIntField("VIBuffer Level", tDesc.iVIBufferLevel)) MarkWidgetUpdated();
				if (ImGui::ColorEdit4("Back Color", reinterpret_cast<float*>(&tDesc.vBackColor))) MarkWidgetUpdated();
				if (ImGui::ColorEdit4("Fill Color", reinterpret_cast<float*>(&tDesc.vFillColor))) MarkWidgetUpdated();
				if (ImGui::SliderFloat("Fill Amount", &tDesc.fFillAmount, 0.f, 1.f)) MarkWidgetUpdated();
				if (Combo_Enum("Direction", tDesc.eDirection, Engine::detail::kProgressDir)) MarkWidgetUpdated();
			}
			else if constexpr (std::is_same_v<T, CUIContainer::UICONTAINER_DESC>)
			{
				if (Combo_Enum("Layout Kind", tDesc.tLayoutDesc.eLayout, Engine::detail::kUILayout)) MarkWidgetUpdated();
				if (ImGui::DragFloat("Padding", &tDesc.tLayoutDesc.fPadding, 1.f)) MarkWidgetUpdated();
				if (ImGui::DragFloat("Spacing", &tDesc.tLayoutDesc.fSpacing, 1.f)) MarkWidgetUpdated();
			}
		}, w.tDesc);
}

CPanel_UILayout* CPanel_UILayout::Create()
{
	CPanel_UILayout* pInstance = new CPanel_UILayout();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Create : CPanel_UILayout");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPanel_UILayout::Free()
{
	__super::Free();

	Safe_Release(m_pSession);
}
