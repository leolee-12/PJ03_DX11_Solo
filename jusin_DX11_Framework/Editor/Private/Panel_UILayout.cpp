#include "Panel_UILayout.h"
#include "UIEditorSession.h"

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

	End_Panel();
	return S_OK;
}

void CPanel_UILayout::Draw_Toolbar()
{
	if (ImGui::Button("New"))
		m_pSession->New_Doc();

	ImGui::SameLine();
	if (ImGui::Button("Load"))
		m_pSession->Load(m_pSession->Get_DocPath());

	ImGui::SameLine();
	if (ImGui::Button("Save"))
		m_pSession->Save(m_pSession->Get_DocPath());

	ImGui::SameLine();
	{
		_string strPath = m_pSession->Get_DocPath();
		if (Edit_StringField<260>("##path", strPath))
			m_pSession->Set_DocPath(strPath);
	}

	ImGui::Text("[%s]%s  status: %s",
		m_pSession->Get_DocPath().c_str(),
		m_pSession->Is_Dirty() ? " *" : "",
		m_pSession->Get_Status().c_str());
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

	if (ImGui::DragFloat("Size X", &tBase.fSizeX, 1.f, 1.f, static_cast<_float>(g_iWinSizeX) * 4.f))
		MarkWidgetUpdated();

	if (ImGui::DragFloat("Size Y", &tBase.fSizeY, 1.f, 1.f, static_cast<_float>(g_iWinSizeY) * 4.f))
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

	ImGui::Separator();
	//ImGui::TextUnformatted("Animations");
	//
	//if (ImGui::Button("Add Anim"))
	//{
	//	_wstring strName;
	//	for (_int iAnim = 1;; ++iAnim)
	//	{
	//		wchar_t szBuffer[32] = {};
	//		swprintf_s(szBuffer, L"Anim_%03d", iAnim);
	//		strName = Make_UniqueAnimationName(*pWidget, szBuffer);
	//		if (strName == szBuffer)
	//			break;
	//	}
	//
	//	pWidget->vAnimations.push_back({ strName, {} });
	//	m_iSelectedAnimation = static_cast<_int>(pWidget->vAnimations.size()) - 1;
	//	m_iSelectedTrack = -1;
	//	Mark_Dirty("Animation added");
	//}
	//
	//ImGui::SameLine();
	//if (ImGui::Button("Delete Anim") && m_iSelectedAnimation >= 0)
	//{
	//	pWidget->vAnimations.erase(pWidget->vAnimations.begin() + m_iSelectedAnimation);
	//	Normalize_Selection();
	//	Sanitize_DocReferences();
	//	Mark_Dirty("Animation deleted");
	//}
	//
	//if (ImGui::BeginChild("AnimationList", ImVec2(0.f, 90.f), true))
	//{
	//	for (_int iAnim = 0; iAnim < static_cast<_int>(pWidget->vAnimations.size()); ++iAnim)
	//	{
	//		const _string strLabel = WtoS(pWidget->vAnimations[iAnim].strName) + "##anim_" + std::to_string(iAnim);
	//		if (ImGui::Selectable(strLabel.c_str(), m_iSelectedAnimation == iAnim))
	//		{
	//			m_iSelectedAnimation = iAnim;
	//			m_iSelectedTrack = -1;
	//			Normalize_Selection();
	//		}
	//	}
	//}
	//ImGui::EndChild();
	//
	//UISEQ_ANIMATION_NODE* pAnimation = Get_SelectedAnimation();
	//if (nullptr != pAnimation)
	//{
	//	const _wstring strOldName = pAnimation->strName;
	//	if (Edit_WStringField<256>("Animation Name", pAnimation->strName))
	//	{
	//		pAnimation->strName = Make_UniqueAnimationName(*pWidget, pAnimation->strName, m_iSelectedAnimation);
	//		if (strOldName != pAnimation->strName)
	//		{
	//			for (auto& tStep : m_Doc.vSteps)
	//			{
	//				if (tStep.eKind == UI_SEQ_STEP_KIND::PLAY_ANIM
	//					&& tStep.strTargetId == pWidget->strId
	//					&& tStep.strAnimName == strOldName)
	//				{
	//					tStep.strAnimName = pAnimation->strName;
	//				}
	//			}
	//		}
	//		Mark_Dirty("Animation renamed");
	//	}
	//
	//	if (ImGui::Button("Add Track"))
	//	{
	//		pAnimation->vTracks.push_back(Make_DefaultTrack(*pWidget));
	//		m_iSelectedTrack = static_cast<_int>(pAnimation->vTracks.size()) - 1;
	//		Mark_Dirty("Track added");
	//	}
	//
	//	ImGui::SameLine();
	//	if (ImGui::Button("Delete Track") && m_iSelectedTrack >= 0)
	//	{
	//		pAnimation->vTracks.erase(pAnimation->vTracks.begin() + m_iSelectedTrack);
	//		Normalize_Selection();
	//		Mark_Dirty("Track deleted");
	//	}
	//
	//	if (ImGui::BeginChild("TrackList", ImVec2(0.f, 90.f), true))
	//	{
	//		for (_int iTrack = 0; iTrack < static_cast<_int>(pAnimation->vTracks.size()); ++iTrack)
	//		{
	//			const _string strLabel = std::to_string(iTrack) + " " + To_String(pAnimation->vTracks[iTrack].eTarget) + "##track_" + std::to_string(iTrack);
	//			if (ImGui::Selectable(strLabel.c_str(), m_iSelectedTrack == iTrack))
	//				m_iSelectedTrack = iTrack;
	//		}
	//	}
	//	ImGui::EndChild();
	//
	//	CUITween::UITWEEN_DESC* pTrack = Get_SelectedTrack();
	//	if (nullptr != pTrack)
	//	{
	//		CUIObject* pSentinel = Resolve_Sentinel(pWidget->Get_Type());
	//		if (nullptr != pSentinel)
	//		{
	//			const char* pszPreview = To_String(pTrack->eTarget);
	//			if (ImGui::BeginCombo("Target", pszPreview))
	//			{
	//				for (_int iTarget = 0; iTarget < static_cast<_int>(UI_TWEEN_TARGET::END); ++iTarget)
	//				{
	//					const UI_TWEEN_TARGET eTarget = static_cast<UI_TWEEN_TARGET>(iTarget);
	//					if (!pSentinel->Can_Apply_Tween_Target(eTarget))
	//						continue;
	//
	//					const _bool bSelected = (pTrack->eTarget == eTarget);
	//					if (ImGui::Selectable(To_String(eTarget), bSelected))
	//					{
	//						pTrack->eTarget = eTarget;
	//						Mark_Dirty("Track updated");
	//					}
	//				}
	//				ImGui::EndCombo();
	//			}
	//		}
	//
	//		if (ImGui::DragFloat("Start", &pTrack->fStart, 0.01f)) Mark_Dirty("Track updated");
	//		if (ImGui::DragFloat("End", &pTrack->fEnd, 0.01f)) Mark_Dirty("Track updated");
	//		if (ImGui::DragFloat("Duration", &pTrack->fDuration, 0.01f, 0.f, 60.f)) Mark_Dirty("Track updated");
	//
	//		_int iEase = static_cast<_int>(pTrack->eEase);
	//		if (iEase < 0 || iEase >= IM_ARRAYSIZE(g_ppEaseNames))
	//			iEase = 0;
	//		if (ImGui::Combo("Ease", &iEase, g_ppEaseNames, IM_ARRAYSIZE(g_ppEaseNames)))
	//		{
	//			pTrack->eEase = static_cast<UI_EASE>(iEase);
	//			Mark_Dirty("Track updated");
	//		}
	//
	//		_int iLoop = static_cast<_int>(pTrack->eLoop);
	//		if (iLoop < 0 || iLoop >= IM_ARRAYSIZE(g_ppLoopNames))
	//			iLoop = 0;
	//		if (ImGui::Combo("Loop", &iLoop, g_ppLoopNames, IM_ARRAYSIZE(g_ppLoopNames)))
	//		{
	//			pTrack->eLoop = static_cast<UI_TWEEN_LOOP>(iLoop);
	//			Mark_Dirty("Track updated");
	//		}
	//	}
	//}
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
