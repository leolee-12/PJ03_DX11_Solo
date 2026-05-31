#include "Panel_UIAnim.h"
#include "UIEditorSession.h"
#include "UIPreviewHost.h"

#include "EditInstance.h"

CPanel_UIAnim::CPanel_UIAnim()
	: CPanel_Base()
	, m_pSession(m_pEditInstance->Get_UISession())
{
	Safe_AddRef(m_pSession);
}

HRESULT CPanel_UIAnim::Initialize()
{
	m_strTitle = "UI_Anim";

	if (FAILED(__super::Initialize()))
		return E_FAIL;

	return S_OK;
}

void CPanel_UIAnim::Update(_float fTimeDelta)
{
}

HRESULT CPanel_UIAnim::Render()
{
	if (!Begin_Panel())
	{
		End_Panel();
		return S_OK;
	}

	ImGui::PushID(m_strTitle.c_str());

	Draw_Header();
	ImGui::Separator();

	Draw_PreviewBar();
	ImGui::Separator();

	const ImVec2 vAvail = ImGui::GetContentRegionAvail();
	const _float fAnimH = std::clamp(vAvail.y * 0.5f, 240.f, 480.f);

	if (ImGui::BeginChild("Animations", ImVec2(0.f, fAnimH), true))
		Draw_Animations();
	ImGui::EndChild();

	if (ImGui::BeginChild("Timeline", ImVec2(0.f, 0.f), true))
		Draw_Timeline();
	ImGui::EndChild();

	ImGui::PopID();
	End_Panel();
	return S_OK;
}

void CPanel_UIAnim::Draw_Header()
{
	const _int iSel = m_pSession->Get_SelectedWidget();
	const auto& vW = m_pSession->Get_Doc().vWidgets;
	if (iSel < 0 || iSel >= (_int)vW.size())
	{
		ImGui::TextDisabled("Select a widget in UI_Layout panel.");
		return;
	}
	const auto& w = vW[iSel];
	ImGui::Text("Widget: [%s] %s   id=%s",
		Engine::To_String(w.Get_Type()),
		w.strDisplayName.c_str(),
		w.strId.c_str());

	ImGui::Separator();
	Draw_VPModeRadio(m_pSession, "vpmode_anim");
}

void CPanel_UIAnim::Draw_PreviewBar()
{
	CUIPreviewHost* pHost = m_pEditInstance->Get_UIPreviewHost();
	if (!pHost) return;

	// -- Row 1: Preview mode + State --
	{
		UI_PREVIEW_MODE eMode = pHost->Get_Mode();
		static const std::pair<UI_PREVIEW_MODE, const char*> kPreviewModes[] = {
			{ UI_PREVIEW_MODE::LAYOUT,        "Layout" },
			{ UI_PREVIEW_MODE::SELECTED_ANIM, "Selected Anim" },
			{ UI_PREVIEW_MODE::SEQUENCE,      "Sequence" },
		};
		const char* pszCur = "Layout";
		for (auto& p : kPreviewModes) if (p.first == eMode) { pszCur = p.second; break; }

		Label_Left("Mode");
		ImGui::SetNextItemWidth(-100.f);  // State 텍스트 자리 남김
		if (ImGui::BeginCombo("##preview_mode", pszCur))
		{
			for (auto& p : kPreviewModes)
			{
				const _bool bSel = (p.first == eMode);
				if (ImGui::Selectable(p.second, bSel) && !bSel)
				{
					pHost->Stop();
					pHost->Set_Mode(p.first);
				}
			}
			ImGui::EndCombo();
		}

		ImGui::SameLine();
		const UI_PREVIEW_STATE eState = pHost->Get_State();
		const char* pszState =
			(eState == UI_PREVIEW_STATE::PLAYING) ? "PLAYING" :
			(eState == UI_PREVIEW_STATE::PAUSED) ? "PAUSED" : "IDLE";
		ImGui::TextDisabled("[%s]", pszState);
	}

	// -- Row 2: Transport buttons --
	{
		const UI_PREVIEW_STATE eState = pHost->Get_State();
		const _bool bPlaying = (eState == UI_PREVIEW_STATE::PLAYING);
		const _bool bPaused = (eState == UI_PREVIEW_STATE::PAUSED);
		const _bool bIdle = (eState == UI_PREVIEW_STATE::IDLE);

		if (ImGui::Button("Rebuild")) pHost->Mark_Rebuild_Pending();
		ImGui::SameLine();

		ImGui::BeginDisabled(!bIdle);
		if (ImGui::Button("Play")) pHost->Play();
		ImGui::EndDisabled();
		ImGui::SameLine();

		ImGui::BeginDisabled(!bPlaying);
		if (ImGui::Button("Pause")) pHost->Pause();
		ImGui::EndDisabled();
		ImGui::SameLine();

		ImGui::BeginDisabled(!bPaused);
		if (ImGui::Button("Resume")) pHost->Resume();
		ImGui::EndDisabled();
		ImGui::SameLine();

		ImGui::BeginDisabled(bIdle);
		if (ImGui::Button("Stop")) pHost->Stop();
		ImGui::EndDisabled();
		ImGui::SameLine();

		if (ImGui::Button("Restart")) pHost->Restart();
	}
}

void CPanel_UIAnim::Draw_Animations()
{
	const _int iSel = m_pSession->Get_SelectedWidget();
	auto& tDoc = m_pSession->Get_DocMutable();
	if (iSel < 0 || iSel >= (_int)tDoc.vWidgets.size())
	{
		ImGui::TextUnformatted("No widget selected.");
		return;
	}
	auto& w = tDoc.vWidgets[iSel];

	Draw_AnimationList(w);

	const _int iAnim = m_pSession->Get_SelectedAnimation();
	if (iAnim < 0 || iAnim >= (_int)w.vAnimations.size())
	{
		ImGui::TextDisabled("Select an animation.");
		return;
	}
	auto& tAnim = w.vAnimations[iAnim];

	ImGui::Separator();
	Draw_TrackList(w, tAnim);

	const _int iTrack = m_pSession->Get_SelectedTrack();
	if (iTrack < 0 || iTrack >= (_int)tAnim.vTracks.size())
	{
		ImGui::TextDisabled("Select a track.");
		return;
	}

	ImGui::Separator();
	Draw_TrackInspector(w, tAnim.vTracks[iTrack]);
}

void CPanel_UIAnim::Draw_AnimationList(UISEQ_WIDGET_NODE& tWidget)
{
	if (ImGui::Button("+ Anim"))
	{
		// 첫 사용 가능한 Anim_NNN 이름 생성
		_wstring strName;
		for (_int i = 1;; ++i)
		{
			wchar_t szBuf[32] = {};
			swprintf_s(szBuf, L"Anim_%03d", i);
			strName = m_pSession->Make_UniqueAnimationName(tWidget, szBuf);
			// ↑ 세션의 Make_UniqueAnimationName이 private이므로 public 노출 필요
			if (strName == szBuf) break;
		}
		tWidget.vAnimations.push_back({ strName, {} });
		m_pSession->Set_SelectedAnimation(static_cast<_int>(tWidget.vAnimations.size()) - 1);
		m_pSession->Set_SelectedTrack(-1);
		m_pSession->Mark_Dirty("Animation added");
	}

	ImGui::SameLine();
	const _int iAnim = m_pSession->Get_SelectedAnimation();
	ImGui::BeginDisabled(iAnim < 0);
	if (ImGui::Button("- Anim"))
	{
		tWidget.vAnimations.erase(tWidget.vAnimations.begin() + iAnim);
		m_pSession->Sanitize_DocReferences();
		m_pSession->Mark_Dirty("Animation deleted");
	}
	ImGui::EndDisabled();

	if (ImGui::BeginChild("AnimList", ImVec2(0.f, 80.f), true))
	{
		for (_int i = 0; i < static_cast<_int>(tWidget.vAnimations.size()); ++i)
		{
			const _string strLabel =
				WtoS(tWidget.vAnimations[i].strName) + "##anim_" + std::to_string(i);
			if (ImGui::Selectable(strLabel.c_str(), iAnim == i))
			{
				m_pSession->Set_SelectedAnimation(i);
				m_pSession->Set_SelectedTrack(-1);
			}
		}
	}
	ImGui::EndChild();

	// Animation rename - step rewrite 로직은 세션으로 이전하는 편이 깔끔
	if (iAnim >= 0)
	{
		auto& tAnim = tWidget.vAnimations[iAnim];
		const _wstring strOld = tAnim.strName;
		if (Edit_WStringField<256>("Anim Name", tAnim.strName))
		{
			// 세션 헬퍼 호출(권장):
			m_pSession->Rename_Animation(tWidget, iAnim, strOld, tAnim.strName);
			// 내부에서 Make_UniqueAnimationName + step rewrite + Mark_Dirty 처리
		}
	}
}

void CPanel_UIAnim::Draw_TrackList(UISEQ_WIDGET_NODE& tWidget, UISEQ_ANIMATION_NODE& tAnim)
{
	if (ImGui::Button("+ Track"))
	{
		tAnim.vTracks.push_back(m_pSession->Make_DefaultTrack(tWidget));
		m_pSession->Set_SelectedTrack((_int)tAnim.vTracks.size() - 1);
		m_pSession->Mark_Dirty("Track added");
	}
	ImGui::SameLine();
	const _int iTrack = m_pSession->Get_SelectedTrack();
	ImGui::BeginDisabled(iTrack < 0);
	if (ImGui::Button("- Track"))
	{
		tAnim.vTracks.erase(tAnim.vTracks.begin() + iTrack);
		m_pSession->Mark_Dirty("Track deleted");
	}
	ImGui::EndDisabled();

	if (ImGui::BeginChild("TrackList", ImVec2(0.f, 80.f), true))
	{
		for (_int i = 0; i < (_int)tAnim.vTracks.size(); ++i)
		{
			const _string strLabel =
				std::to_string(i) + " " +
				Engine::To_String(tAnim.vTracks[i].eTarget) +
				"##track_" + std::to_string(i);
			if (ImGui::Selectable(strLabel.c_str(), iTrack == i))
				m_pSession->Set_SelectedTrack(i);
		}
	}
	ImGui::EndChild();
}

void CPanel_UIAnim::Draw_TrackInspector(UISEQ_WIDGET_NODE& tWidget, CUITween::UITWEEN_DESC& tTrack)
{
	auto MarkUpdated = [&] { m_pSession->Mark_Dirty("Track updated"); };

	// Target combo (sentinel-filtered)
	CUIObject* pSentinel = m_pSession->Resolve_Sentinel(tWidget.Get_Type());
	if (pSentinel)
	{
		const char* pszPreview = Engine::To_String(tTrack.eTarget);
		if (ImGui::BeginCombo("Target", pszPreview))
		{
			for (_int i = 0; i < (_int)UI_TWEEN_TARGET::END; ++i)
			{
				const auto eT = (UI_TWEEN_TARGET)i;
				if (!pSentinel->Can_Apply_Tween_Target(eT)) continue;

				const _bool bSel = (tTrack.eTarget == eT);
				if (ImGui::Selectable(Engine::To_String(eT), bSel) && !bSel)
				{
					tTrack.eTarget = eT;
					MarkUpdated();
				}
			}
			ImGui::EndCombo();
		}
	}

	if (ImGui::DragFloat("Start", &tTrack.fStart, 0.01f)) MarkUpdated();
	if (ImGui::DragFloat("End", &tTrack.fEnd, 0.01f)) MarkUpdated();
	if (ImGui::DragFloat("Duration", &tTrack.fDuration, 0.01f, 0.f, 60.f)) MarkUpdated();

	if (Combo_Enum("Ease", tTrack.eEase, Engine::detail::kEase))      MarkUpdated();
	if (Combo_Enum("Loop", tTrack.eLoop, Engine::detail::kTweenLoop)) MarkUpdated();
}

void CPanel_UIAnim::Draw_Timeline()
{
	auto& tDoc = m_pSession->Get_DocMutable();

	auto AddStep = [&](UI_SEQ_STEP_KIND eKind, _bool bJoin)
		{
			tDoc.vSteps.push_back(m_pSession->Make_DefaultStep(eKind, bJoin));
			m_pSession->Set_SelectedStep((_int)tDoc.vSteps.size() - 1);
			m_pSession->Mark_Dirty("Step added");
		};

	ImGui::TextUnformatted("Sequence Timeline");
	if (ImGui::Button("+ PLAY_ANIM"))    AddStep(UI_SEQ_STEP_KIND::PLAY_ANIM, false);
	ImGui::SameLine();
	if (ImGui::Button("+ SET_VISIBLE"))  AddStep(UI_SEQ_STEP_KIND::SET_VISIBLE, false);

	if (ImGui::Button("+ WAIT"))         AddStep(UI_SEQ_STEP_KIND::WAIT, false);
	ImGui::SameLine();
	if (ImGui::Button("+ CALLBACK"))     AddStep(UI_SEQ_STEP_KIND::USE_CALLBACK, false);

	if (ImGui::Button("+ EFFECT_PLAY"))  AddStep(UI_SEQ_STEP_KIND::EFFECT_PLAY, false);
	ImGui::SameLine();
	if (ImGui::Button("+ EFFECT_STOP"))  AddStep(UI_SEQ_STEP_KIND::EFFECT_STOP, false);

	if (ImGui::Button("+ BGM_PLAY"))     AddStep(UI_SEQ_STEP_KIND::BGM_PLAY, false);
	ImGui::SameLine();
	if (ImGui::Button("+ BGM_STOP"))     AddStep(UI_SEQ_STEP_KIND::BGM_STOP, false);

	if (ImGui::Button("+ SFX_PLAY"))     AddStep(UI_SEQ_STEP_KIND::SFX_PLAY, false);
	ImGui::SameLine();
	if (ImGui::Button("+ SIGNAL"))		AddStep(UI_SEQ_STEP_KIND::SIGNAL_FIRE, false);

	const _int iStep = m_pSession->Get_SelectedStep();
	const _bool bHasSel = (iStep >= 0 && iStep < static_cast<_int>(tDoc.vSteps.size()));

	ImGui::BeginDisabled(!bHasSel);
	if (ImGui::Button("- Step"))
	{
		tDoc.vSteps.erase(tDoc.vSteps.begin() + iStep);
		m_pSession->Sanitize_DocReferences();
		m_pSession->Set_SelectedStep(iStep);
		m_pSession->Mark_Dirty("Step deleted");
	}
	ImGui::SameLine();
	if (ImGui::Button("Up") && iStep > 0)
	{
		std::swap(tDoc.vSteps[iStep], tDoc.vSteps[iStep - 1]);
		m_pSession->Set_SelectedStep(iStep - 1);
		m_pSession->Mark_Dirty("Step reordered");
	}
	ImGui::SameLine();
	if (ImGui::Button("Down") && bHasSel
		&& iStep + 1 < (_int)tDoc.vSteps.size())
	{
		std::swap(tDoc.vSteps[iStep], tDoc.vSteps[iStep + 1]);
		m_pSession->Set_SelectedStep(iStep + 1);
		m_pSession->Mark_Dirty("Step reordered");
	}
	ImGui::EndDisabled();

	if (ImGui::BeginChild("StepList", ImVec2(0.f, 120.f), true))
	{
		for (_int i = 0; i < (_int)tDoc.vSteps.size(); ++i)
		{
			const auto& s = tDoc.vSteps[i];
			char szLabel[256];
			const _bool bSlotStep =
				s.eKind == UI_SEQ_STEP_KIND::EFFECT_PLAY ||
				s.eKind == UI_SEQ_STEP_KIND::EFFECT_STOP ||
				s.eKind == UI_SEQ_STEP_KIND::BGM_PLAY ||
				s.eKind == UI_SEQ_STEP_KIND::BGM_STOP ||
				s.eKind == UI_SEQ_STEP_KIND::SFX_PLAY;

			if (bSlotStep)
			{
				sprintf_s(szLabel, "%s%d %s slot=%s target=%s##step_%d",
					s.bJoinPrev ? "└- " : "",
					i,
					Engine::To_String(s.eKind),
					s.strSlotId.c_str(),
					s.strTargetId.c_str(),
					i);
			}
			else
			{
				sprintf_s(szLabel, "%s%d %s %s %s##step_%d",
					s.bJoinPrev ? "└- " : "",
					i,
					Engine::To_String(s.eKind),
					s.strTargetId.c_str(),
					WtoS(s.strAnimName).c_str(),
					i);
			}

			if (ImGui::Selectable(szLabel, iStep == i))
				m_pSession->Set_SelectedStep(i);
		}
	}
	ImGui::EndChild();

	const _int iStepNow = m_pSession->Get_SelectedStep();
	const _bool bValidNow = (iStepNow >= 0 && iStepNow < (_int)tDoc.vSteps.size());

	if (bValidNow)
	{
		ImGui::Separator();
		Draw_StepInspector(tDoc.vSteps[iStepNow]);
	}
}

void CPanel_UIAnim::Draw_StepInspector(UISEQ_STEP_NODE& tStep)
{
	auto MarkUpdated = [&] { m_pSession->Mark_Dirty("Step updated"); };

	if (Combo_Enum("Kind", tStep.eKind, Engine::detail::kStepKind))
	{
		// kind 변경 시 fallback 적용
		m_pSession->Apply_StepTargetFallback(tStep);
		MarkUpdated();
	}

	if (ImGui::Checkbox("Join Prev", &tStep.bJoinPrev))
	{
		m_pSession->Sanitize_DocReferences(); // 첫 step의 join 자동 false
		MarkUpdated();
	}

	switch (tStep.eKind)
	{
	case UI_SEQ_STEP_KIND::PLAY_ANIM:
	{
		// Target widget combo (PLAY_ANIM 가능한 widget만)
		const auto& vW = m_pSession->Get_Doc().vWidgets;
		const char* pszPrev = tStep.strTargetId.empty() ? "<none>" : tStep.strTargetId.c_str();
		if (ImGui::BeginCombo("Target Widget", pszPrev))
		{
			for (const auto& w : vW)
			{
				const _bool bSel = (tStep.strTargetId == w.strId);
				char szLbl[128];
				sprintf_s(szLbl, "[%s] %s##t_%s",
					Engine::To_String(w.Get_Type()),
					w.strId.c_str(),
					w.strId.c_str());
				if (ImGui::Selectable(szLbl, bSel) && !bSel)
				{
					tStep.strTargetId = w.strId;
					m_pSession->Apply_StepTargetFallback(tStep);
					MarkUpdated();
				}
			}
			ImGui::EndCombo();
		}

		// Animation combo (해당 widget의 animations만)
		if (const UISEQ_WIDGET_NODE* pW = m_pSession->Find_WidgetById(tStep.strTargetId)) // 세션에 노출 필요
		{
			const _string strPrev = WtoS(tStep.strAnimName);
			if (ImGui::BeginCombo("Animation",
				strPrev.empty() ? "<none>" : strPrev.c_str()))
			{
				for (const auto& a : pW->vAnimations)
				{
					const _bool bSel = (tStep.strAnimName == a.strName);
					const _string strLbl = WtoS(a.strName) + "##a_" + WtoS(a.strName);
					if (ImGui::Selectable(strLbl.c_str(), bSel) && !bSel)
					{
						tStep.strAnimName = a.strName;
						MarkUpdated();
					}
				}
				ImGui::EndCombo();
			}
		}
		break;
	}

	case UI_SEQ_STEP_KIND::SET_VISIBLE:
	{
		// Target widget combo (전 widget 후보)
		const auto& vW = m_pSession->Get_Doc().vWidgets;
		const char* pszPrev = tStep.strTargetId.empty() ? "<none>" : tStep.strTargetId.c_str();
		if (ImGui::BeginCombo("Target Widget", pszPrev))
		{
			for (const auto& w : vW)
			{
				const _bool bSel = (tStep.strTargetId == w.strId);
				if (ImGui::Selectable(w.strId.c_str(), bSel) && !bSel)
				{
					tStep.strTargetId = w.strId;
					MarkUpdated();
				}
			}
			ImGui::EndCombo();
		}
		if (ImGui::Checkbox("Visible", &tStep.bVisible)) MarkUpdated();
		break;
	}

	case UI_SEQ_STEP_KIND::WAIT:
		if (ImGui::DragFloat("Wait (sec)", &tStep.fWaitSec, 0.01f, 0.f, 60.f))
			MarkUpdated();
		break;

	case UI_SEQ_STEP_KIND::USE_CALLBACK:
		if (Edit_StringField<256>("Callback Id", tStep.strCallbackId))
			MarkUpdated();
		ImGui::TextDisabled("(callback execution is not implemented in this phase)");
		break;

	case UI_SEQ_STEP_KIND::EFFECT_PLAY:
	case UI_SEQ_STEP_KIND::SFX_PLAY:
	case UI_SEQ_STEP_KIND::SIGNAL_FIRE:
	{
		if (Edit_StringField<256>("Slot Id", tStep.strSlotId))
			MarkUpdated();

		const auto& vW = m_pSession->Get_Doc().vWidgets;
		const char* pszPrev = tStep.strTargetId.empty() ? "<none>" : tStep.strTargetId.c_str();

		if (ImGui::BeginCombo("Target Widget", pszPrev))
		{
			const _bool bNone = tStep.strTargetId.empty();
			if (ImGui::Selectable("<none>", bNone) && !bNone)
			{ 
				tStep.strTargetId.clear();
				MarkUpdated();
			}

			for (const auto& w : vW)
			{
				const _bool bSel = (tStep.strTargetId == w.strId);
				if (ImGui::Selectable(w.strId.c_str(), bSel) && !bSel)
				{
					tStep.strTargetId = w.strId;
					MarkUpdated();
				}
			}
			ImGui::EndCombo();
		}

		if (ImGui::Checkbox("Required", &tStep.bRequired))
			MarkUpdated();

		break;
	}

	case UI_SEQ_STEP_KIND::EFFECT_STOP:
	case UI_SEQ_STEP_KIND::BGM_PLAY:
	case UI_SEQ_STEP_KIND::BGM_STOP:
	{
		if (Edit_StringField<256>("Slot Id", tStep.strSlotId))
			MarkUpdated();

		if (ImGui::Checkbox("Required", &tStep.bRequired))
			MarkUpdated();

		break;
	}
	}
}

CPanel_UIAnim* CPanel_UIAnim::Create()
{
	CPanel_UIAnim* pInstance = new CPanel_UIAnim();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Create : CPanel_UIAnim");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPanel_UIAnim::Free()
{
	__super::Free();

	Safe_Release(m_pSession);
}
