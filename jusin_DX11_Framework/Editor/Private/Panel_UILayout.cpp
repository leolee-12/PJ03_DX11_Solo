#include "Panel_UILayout.h"
#include "UIEditorSession.h"
#include "EditInstance.h"

#include "SharedTexture_Manager.h"

#include "GameInstance.h"

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
					if (!entry.is_regular_file())
						continue;
					if (entry.path().extension() != ".uiseq")
						continue;

					const _string strPath = entry.path().string();
					const _bool bSel = (strPath == strCurPath);
					if (ImGui::Selectable(strPath.c_str(), bSel) && !bSel)
						m_pSession->Set_DocPath(strPath);
				}
			}
			ImGui::EndCombo();
		}
	}

	{
		Label_Left("Path");
		ImGui::SetNextItemWidth(-FLT_MIN);
		_string strDocPath = m_pSession->Get_DocPath();
		if (Edit_StringField<512>("##path_edit", strDocPath))
			m_pSession->Set_DocPath(strDocPath);
	}

	if (ImGui::Button("New"))
	{
		if (m_pSession->Is_Dirty())
		{
			m_ePendingAction = PENDING_ACTION::NEW_DOC;
			ImGui::OpenPopup("Discard Changes?");
		}
		else
		{
			m_pSession->New_Doc();
		}
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
		else
		{
			m_pSession->Load(strPath);
		}
	}
	ImGui::SameLine();
	if (ImGui::Button("Save"))
		m_pSession->Save(m_pSession->Get_DocPath());

	if (ImGui::BeginPopupModal("Discard Changes?", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
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

	ImGui::Separator();
	ImGui::TextUnformatted("Document");

	UISEQ_DOC& tDoc = m_pSession->Get_DocMutable();

	if (Edit_StringField<256>("Name", tDoc.strName))
		m_pSession->Mark_Dirty("Document updated");

	_float fDesignWidth = tDoc.fDesignWidth;
	_float fDesignHeight = tDoc.fDesignHeight;
	UI_SCALE_POLICY eScalePolicy = tDoc.eScalePolicy;
	_bool bCanvasChanged = false;

	if (ImGui::DragFloat("Design Width", &fDesignWidth, 1.f, 1.f, 16384.f, "%.0f"))
		bCanvasChanged = true;

	if (ImGui::DragFloat("Design Height", &fDesignHeight, 1.f, 1.f, 16384.f, "%.0f"))
		bCanvasChanged = true;

	if (Combo_Enum("Scale Policy", eScalePolicy, Engine::detail::kScalePolicy))
		bCanvasChanged = true;

	if (bCanvasChanged)
		m_pSession->Set_DocCanvas(fDesignWidth, fDesignHeight, eScalePolicy);

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

	if (Edit_TagField<256>("Prototype Tag", w.strPrototypeTag))
		MarkWidgetUpdated();

	if (w.Get_Type() == UI_TYPE::BUTTON)
	{
		ImGui::TextDisabled("Use actual tag string, e.g. Prototype_UIButton_Layered");
		ImGui::TextDisabled("Layered shader: Prototype_Component_Shader_UI_Button_Layered");
		ImGui::TextDisabled("Glow shader: Prototype_Component_Shader_UI_Button_Glow");
	}

	if (ImGui::Checkbox("Visible", &tBase.bVisible))
		MarkWidgetUpdated();

	if (ImGui::DragInt("Z Order", &tBase.iZOrder, 1.f))
		MarkWidgetUpdated();

	ImGui::Text("Canvas : %.0f x %.0f / %s",
		tDoc.fDesignWidth,
		tDoc.fDesignHeight,
		Engine::To_String(tDoc.eScalePolicy));

	const _float fMaxSizeX = (tDoc.fDesignWidth > 1.f) ? (tDoc.fDesignWidth * 4.f) : 1.f;
	const _float fMaxSizeY = (tDoc.fDesignHeight > 1.f) ? (tDoc.fDesignHeight * 4.f) : 1.f;

	if (ImGui::DragFloat("Size X", &tBase.fSizeX, 1.f, 1.f, fMaxSizeX))
		MarkWidgetUpdated();

	if (ImGui::DragFloat("Size Y", &tBase.fSizeY, 1.f, 1.f, fMaxSizeY))
		MarkWidgetUpdated();

	float fPivot[2] = { tBase.fPivotX, tBase.fPivotY };
	if (ImGui::SliderFloat2("Pivot", fPivot, 0.f, 1.f, "%.2f"))
	{
		tBase.fPivotX = fPivot[0];
		tBase.fPivotY = fPivot[1];
		MarkWidgetUpdated();
	}

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
				if (Edit_UIntField("Shader Pass", tDesc.iShaderPass)) MarkWidgetUpdated();

				if (Edit_TagField<256>("VIBuffer Tag", tDesc.strVIBufferTag)) MarkWidgetUpdated();
				if (Edit_UIntField("VIBuffer Level", tDesc.iVIBufferLevel)) MarkWidgetUpdated();

				if (ImGui::ColorEdit4("Color", reinterpret_cast<float*>(&tDesc.vColor))) MarkWidgetUpdated();

				// -- Sprite Animation --
				ImGui::Separator();
				if (ImGui::CollapsingHeader("Sprite Animation", ImGuiTreeNodeFlags_DefaultOpen))
				{
					if (ImGui::Checkbox("Enabled", &tDesc.bSpriteAnimEnabled))
					{
						// 켰을 때 frameDuration 0이면 12FPS 기본값
						if (tDesc.bSpriteAnimEnabled && tDesc.fSpriteFrameDuration <= 0.f)
							tDesc.fSpriteFrameDuration = 1.f / 12.f;
						MarkWidgetUpdated();
					}

					if (ImGui::DragFloat("Frame Duration (s)", &tDesc.fSpriteFrameDuration,
						0.001f, 0.f, 10.f, "%.4f"))
					{
						if (tDesc.fSpriteFrameDuration < 0.f)
							tDesc.fSpriteFrameDuration = 0.f;
						MarkWidgetUpdated();
					}

					if (tDesc.fSpriteFrameDuration > 0.f)
						ImGui::Text("= %.2f FPS", 1.f / tDesc.fSpriteFrameDuration);
					else
						ImGui::TextDisabled("(disabled)");
				}

				// -- Shared Textures --
				ImGui::Separator();
				if (ImGui::CollapsingHeader("Shared Textures", ImGuiTreeNodeFlags_DefaultOpen))
				{
					if (ImGui::Button("Add Binding"))
					{
						tDesc.SharedTextureBindings.emplace_back();
						MarkWidgetUpdated();
					}

					for (size_t i = 0; i < tDesc.SharedTextureBindings.size(); )
					{
						ImGui::PushID(static_cast<int>(i));
						auto& b = tDesc.SharedTextureBindings[i];

						ImGui::Text("[%zu]", i);

						// Group combo
						const _uint iCount = ETOUI(SHARED_TEXTURE_TYPE::END);

						const char* pszPreview = b.strSharedTexName.empty() ? "(none)" : b.strSharedTexName.c_str();
						if (ImGui::BeginCombo("Group", pszPreview))
						{
							for (_uint k = 0; k < iCount; ++k)
							{
								const char* psz = To_String(static_cast<SHARED_TEXTURE_TYPE>(k));
								const bool bSel = (b.strSharedTexName == psz);
								if (ImGui::Selectable(psz, bSel) && !bSel)
								{
									b.strSharedTexName = psz;
									MarkWidgetUpdated();
								}
							}
							ImGui::EndCombo();
						}

						if (Edit_StringField<128>("Shader Var", b.strShaderVarName))
							MarkWidgetUpdated();

						int iIdx = static_cast<int>(b.iTextureIndex);
						if (ImGui::InputInt("Index", &iIdx))
						{
							b.iTextureIndex = (iIdx < 0) ? 0u : static_cast<unsigned int>(iIdx);
							MarkWidgetUpdated();
						}

						if (ImGui::Button("Remove"))
						{
							tDesc.SharedTextureBindings.erase(tDesc.SharedTextureBindings.begin() + i);
							MarkWidgetUpdated();
							ImGui::PopID();
							continue;          // i 유지 - 다음 원소가 같은 위치로 당겨짐
						}

						ImGui::Separator();
						ImGui::PopID();
						++i;
					}
				}
			}
			else if constexpr (std::is_same_v<T, CUIButton::UIBUTTON_DESC>)
			{
				if (Edit_TagField<256>("Texture Tag", tDesc.strTextureTag)) MarkWidgetUpdated();
				if (Edit_UIntField("Texture Level", tDesc.iTextureLevel)) MarkWidgetUpdated();

				if (Edit_TagField<256>("Shader Tag", tDesc.strShaderTag)) MarkWidgetUpdated();
				if (Edit_UIntField("Shader Level", tDesc.iShaderLevel)) MarkWidgetUpdated();

				if (Edit_TagField<256>("VIBuffer Tag", tDesc.strVIBufferTag)) MarkWidgetUpdated();
				if (Edit_UIntField("VIBuffer Level", tDesc.iVIBufferLevel)) MarkWidgetUpdated();

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
