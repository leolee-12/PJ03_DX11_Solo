#include "Panel_Effect.h"
#include "EditInstance.h"
#include "EffectEditorSession.h"

#include "GameInstance.h"

namespace
{
	constexpr _int CURVE_PREVIEW_SAMPLES = 64;

	void Plot_FloatCurve(const char* pLabel, const CCurveFloat& curve)
	{
		_float Values[CURVE_PREVIEW_SAMPLES] = {};

		for (_int i = 0; i < CURVE_PREVIEW_SAMPLES; ++i)
		{
			const _float fT = static_cast<_float>(i) / static_cast<_float>(CURVE_PREVIEW_SAMPLES - 1);
			Values[i] = curve.IsEmpty() ? 0.f : curve.Sample(fT);
		}

		ImGui::PlotLines(pLabel, Values, CURVE_PREVIEW_SAMPLES, 0, nullptr, 0.f, 1.f, ImVec2(0.f, 48.f));
	}

	void Plot_ColorCurve(const char* pLabel, const CCurveColor& curve)
	{
		_float Values[CURVE_PREVIEW_SAMPLES] = {};

		for (_int i = 0; i < CURVE_PREVIEW_SAMPLES; ++i)
		{
			const _float fT = static_cast<_float>(i) / static_cast<_float>(CURVE_PREVIEW_SAMPLES - 1);
			const _float4 vColor = curve.IsEmpty() ? _float4(0.f, 0.f, 0.f, 0.f) : curve.Sample(fT);
			Values[i] = (vColor.x + vColor.y + vColor.z) / 3.f;
		}

		ImGui::PlotLines(pLabel, Values, CURVE_PREVIEW_SAMPLES, 0, nullptr, 0.f, 1.f, ImVec2(0.f, 48.f));
	}

	void Set_SizePopCurve(CCurveFloat& curve)
	{
		curve.Clear();
		curve.Add_Key(0.f, 0.1f);
		curve.Add_Key(0.2f, 1.f);
		curve.Add_Key(1.f, 0.f);
	}

	void Set_FireColorCurve(CCurveColor& curve)
	{
		curve.Clear();
		curve.Add_Key(0.f, _float4(1.f, 1.f, 1.f, 1.f));
		curve.Add_Key(0.5f, _float4(1.f, 1.f, 0.4f, 1.f));
		curve.Add_Key(1.f, _float4(1.f, 0.3f, 0.f, 0.f));
	}

	void Set_FadeAlphaCurve(CCurveFloat& curve)
	{
		curve.Clear();
		curve.Add_Key(0.f, 0.f);
		curve.Add_Key(0.1f, 1.f);
		curve.Add_Key(1.f, 0.f);
	}

	_float ClampRange(_float fValue, _float fMin, _float fMax)
	{
		if (fValue < fMin)
			return fMin;
		if (fValue > fMax)
			return fMax;
		return fValue;
	}

	void Draw_FloatCurveKeys(
		const char* pTitle,
		CCurveFloat& curve,
		CEffectEditorSession* pSession,
		const char* pDirtyReason,
		_float fValueMin,
		_float fValueMax,
		_float fSpeed = 0.01f,
		const char* pFormat = "%.2f")
	{
		if (!ImGui::TreeNode(pTitle))
			return;

		auto Keys = curve.Get_Keys();
		_bool bChanged = false;
		_int iRemove = -1;

		for (_int i = 0; i < static_cast<_int>(Keys.size()); ++i)
		{
			ImGui::PushID(i);

			_float fT = Keys[i].t;
			_float fValue = Keys[i].v;

			ImGui::SetNextItemWidth(90.f);
			if (ImGui::DragFloat("T", &fT, 0.01f, 0.f, 1.f, "%.2f"))
			{
				Keys[i].t = ClampRange(fT, 0.f, 1.f);
				bChanged = true;
			}

			ImGui::SameLine();

			ImGui::SetNextItemWidth(90.f);
			if (ImGui::DragFloat("V", &fValue, fSpeed, fValueMin, fValueMax, pFormat))
			{
				Keys[i].v = ClampRange(fValue, fValueMin, fValueMax);
				bChanged = true;
			}

			ImGui::SameLine();

			if (ImGui::Button("Delete"))
				iRemove = i;

			ImGui::PopID();
		}

		if (0 <= iRemove && iRemove < static_cast<_int>(Keys.size()))
		{
			Keys.erase(Keys.begin() + iRemove);
			bChanged = true;
		}

		if (ImGui::Button("Add Key"))
		{
			Keys.push_back({ 0.5f, 1.f });
			bChanged = true;
		}

		if (bChanged)
		{
			curve.Clear();

			for (const auto& key : Keys)
				curve.Add_Key(
					ClampRange(key.t, 0.f, 1.f),
					ClampRange(key.v, fValueMin, fValueMax));

			if (nullptr != pSession)
				pSession->Mark_Dirty(pDirtyReason);
		}

		ImGui::TreePop();
	}

	void Draw_ColorCurveKeys(
		const char* pTitle,
		CCurveColor& curve,
		CEffectEditorSession* pSession,
		const char* pDirtyReason)
	{
		if (!ImGui::TreeNode(pTitle))
			return;

		auto Keys = curve.Get_Keys();
		_bool bChanged = false;
		_int iRemove = -1;

		for (_int i = 0; i < static_cast<_int>(Keys.size()); ++i)
		{
			ImGui::PushID(i);

			_float fT = Keys[i].t;
			_float4 vColor = Keys[i].v;

			ImGui::SetNextItemWidth(90.f);
			if (ImGui::DragFloat("T", &fT, 0.01f, 0.f, 1.f, "%.2f"))
			{
				Keys[i].t = ClampRange(fT, 0.f, 1.f);
				bChanged = true;
			}

			ImGui::SameLine();

			ImGui::SetNextItemWidth(180.f);
			if (ImGui::ColorEdit4("RGBA", &vColor.x))
			{
				Keys[i].v = _float4(
					ClampRange(vColor.x, 0.f, 1.f),
					ClampRange(vColor.y, 0.f, 1.f),
					ClampRange(vColor.z, 0.f, 1.f),
					ClampRange(vColor.w, 0.f, 1.f));
				bChanged = true;
			}

			ImGui::SameLine();

			if (ImGui::Button("Delete"))
				iRemove = i;

			ImGui::PopID();
		}

		if (0 <= iRemove && iRemove < static_cast<_int>(Keys.size()))
		{
			Keys.erase(Keys.begin() + iRemove);
			bChanged = true;
		}

		if (ImGui::Button("Add Color Key"))
		{
			Keys.push_back({ 0.5f, _float4(1.f, 1.f, 1.f, 1.f) });
			bChanged = true;
		}

		if (bChanged)
		{
			curve.Clear();

			for (const auto& key : Keys)
				curve.Add_Key(ClampRange(key.t, 0.f, 1.f), key.v);

			if (nullptr != pSession)
				pSession->Mark_Dirty(pDirtyReason);
		}

		ImGui::TreePop();
	}

	bool Draw_BlendCombo(const char* pLabel, BLEND_MODE& eBlend,
		CEffectEditorSession* pSession, const char* pDirtyReason)
	{
		const char* pBlendLabels[] = { "ALPHA", "ADDITIVE" };
		_int iBlend = static_cast<_int>(eBlend);
		if (ImGui::Combo(pLabel, &iBlend, pBlendLabels, IM_ARRAYSIZE(pBlendLabels)))
		{
			eBlend = static_cast<BLEND_MODE>(iBlend);
			if (nullptr != pSession) pSession->Mark_Dirty(pDirtyReason);
			return true;
		}
		return false;
	}

	void Draw_TextureCombo(const char* pLabel, WNameID& strTag,
		CEffectEditorSession* pSession, const char* pDirtyReason)
	{
		const auto& TextureOptions = Game_PKM::g_EffectTextureOptions;

		_int iTexture = 0;
		for (_int i = 0; i < static_cast<_int>(IM_ARRAYSIZE(TextureOptions)); ++i)
			if (TextureOptions[i].strTag == strTag) { iTexture = i; break; }

		if (ImGui::BeginCombo(pLabel, TextureOptions[iTexture].pLabel))
		{
			for (_int i = 0; i < static_cast<_int>(IM_ARRAYSIZE(TextureOptions)); ++i)
			{
				const _bool bSelected = (iTexture == i);
				if (ImGui::Selectable(TextureOptions[i].pLabel, bSelected))
				{
					strTag = TextureOptions[i].strTag;
					if (nullptr != pSession) pSession->Mark_Dirty(pDirtyReason);
				}
				if (bSelected) ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}
	}

	void Draw_FloatCurveRow(CCurveFloat& curve, CEffectEditorSession* pSession,
		const char* pPlotLabel, const char* pPresetLabel, void(*pPresetFn)(CCurveFloat&),
		const char* pClearLabel, const char* pKeysLabel, _float fKeyMin, _float fKeyMax,
		const char* pPresetDirty, const char* pClearDirty, const char* pKeysDirty,
		_float fSpeed = 0.01f, const char* pFormat = "%.2f")
	{
		Plot_FloatCurve(pPlotLabel, curve);
		if (ImGui::Button(pPresetLabel)) {
			pPresetFn(curve); if (pSession)
				pSession->Mark_Dirty(pPresetDirty);
		}
		ImGui::SameLine();
		if (ImGui::Button(pClearLabel)) {
			curve.Clear(); if (pSession)
				pSession->Mark_Dirty(pClearDirty);
		}
		Draw_FloatCurveKeys(pKeysLabel, curve, pSession, pKeysDirty, fKeyMin, fKeyMax, fSpeed, pFormat);
	}

	void Draw_ColorCurveRow(CCurveColor& curve, CEffectEditorSession* pSession,
		const char* pPlotLabel, const char* pPresetLabel, const char* pClearLabel,
		const char* pKeysLabel, const char* pPresetDirty, const char* pClearDirty, const char* pKeysDirty)
	{
		Plot_ColorCurve(pPlotLabel, curve);
		if (ImGui::Button(pPresetLabel)) {
			Set_FireColorCurve(curve); if (pSession)
				pSession->Mark_Dirty(pPresetDirty);
		}
		ImGui::SameLine();
		if (ImGui::Button(pClearLabel)) {
			curve.Clear(); if (pSession)
				pSession->Mark_Dirty(pClearDirty);
		}
		Draw_ColorCurveKeys(pKeysLabel, curve, pSession, pKeysDirty);
	}
}

CPanel_Effect::CPanel_Effect()
	: CPanel_Base()
{
}

HRESULT CPanel_Effect::Initialize()
{
	m_strTitle = "Effect";

	if (FAILED(__super::Initialize()))
		return E_FAIL;

	m_pSession = CEffectEditorSession::Create();
	if (nullptr == m_pSession)
		return E_FAIL;

	return S_OK;
}

void CPanel_Effect::Update(_float fTimeDelta)
{
	if (nullptr != m_pSession)
		m_pSession->Update(fTimeDelta);
}

HRESULT CPanel_Effect::Render()
{
	if (!Begin_Panel()) { End_Panel(); return S_OK; }

	if (nullptr == m_pSession)
	{
		ImGui::TextDisabled("Effect session is not available.");
		End_Panel();
		return S_OK;
	}

	Render_DocHeader();
	Render_Toolbar();
	Render_PreviewControls();

	ImGui::Separator();

	if (ImGui::BeginTabBar("##EffectTabs"))
	{
		if (ImGui::BeginTabItem("Emitters"))
		{
			if (ImGui::Button("Add Emitter"))
				m_pSession->Add_Emitter();
			ImGui::SameLine();
			ImGui::BeginDisabled(m_pSession->Get_SelectedEmitter() < 0);
			if (ImGui::Button("Remove Emitter"))
				m_pSession->Erase_SelectedEmitter();
			ImGui::EndDisabled();

			Render_EmitterList();
			Render_EmitterInspector();
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Meshes"))
		{
			if (ImGui::Button("Add Mesh"))
				m_pSession->Add_Mesh();
			ImGui::SameLine();
			ImGui::BeginDisabled(m_pSession->Get_SelectedMesh() < 0);
			if (ImGui::Button("Remove Mesh"))
				m_pSession->Erase_SelectedMesh();
			ImGui::EndDisabled();

			Render_MeshList();
			Render_MeshInspector();
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Trails"))
		{
			if (ImGui::Button("Add Trail"))
				m_pSession->Add_Trail();
			ImGui::SameLine();
			ImGui::BeginDisabled(m_pSession->Get_SelectedTrail() < 0);
			if (ImGui::Button("Remove Trail"))
				m_pSession->Erase_SelectedTrail();
			ImGui::EndDisabled();

			Render_TrailList();
			Render_TrailInspector();
			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}

	End_Panel();
	return S_OK;
}

void CPanel_Effect::Render_DocHeader()
{
	const EFFECT_DEFINITION& doc = m_pSession->Get_Doc();

	{
		_char szID[128] = {};
		strncpy_s(szID, IM_ARRAYSIZE(szID), doc.strID.c_str(), _TRUNCATE);
		if (ImGui::InputText("ID", szID, IM_ARRAYSIZE(szID)))
			m_pSession->Set_DocID(szID);
	}

	/* 존재하는 .effect.json 드롭다운 (Panel_UILayout 패턴 미러) */
	{
		namespace fs = std::filesystem;
		const char* pEffectsDir = "../../Resources/Effects/";
		const _string strCurPath = m_pSession->Get_DocPath();

		if (ImGui::BeginCombo("File", strCurPath.c_str()))
		{
			std::error_code ec;
			if (fs::exists(pEffectsDir, ec))
			{
				for (const auto& entry : fs::directory_iterator(pEffectsDir, ec))
				{
					if (!entry.is_regular_file())
						continue;

					/* ".effect.json"(12자)로 끝나는 파일만 — Save가 만드는 .bak· */
					const _string strName = entry.path().filename().string();
					if (strName.size() < 12 ||
						strName.compare(strName.size() - 12, 12, ".effect.json"))
						continue;

					const _string strPath = entry.path().string();
					const _bool bSelected = (strPath == strCurPath);
					if (ImGui::Selectable(strPath.c_str(), bSelected) && !bSelected)
						m_pSession->Set_DocPath(strPath);
					if (bSelected)
						ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}
	}

	{
		_char szPath[260] = {};
		strncpy_s(szPath, IM_ARRAYSIZE(szPath), m_pSession->Get_DocPath().c_str(), _TRUNCATE);
		if (ImGui::InputText("Path", szPath, IM_ARRAYSIZE(szPath)))
			m_pSession->Set_DocPath(szPath);
	}
}

void CPanel_Effect::Render_Toolbar()
{
	ImGui::Text("State: %s", m_pSession->Is_Dirty() ? "Dirty" : "Clean");

	const _string& strStatus = m_pSession->Get_Status();
	if (!strStatus.empty())
		ImGui::TextDisabled("%s", strStatus.c_str());

	ImGui::Separator();

	if (ImGui::Button("New"))
		m_pSession->New_Doc();
	ImGui::SameLine();
	if (ImGui::Button("Save"))
		m_pSession->Save(m_pSession->Get_DocPath());
	ImGui::SameLine();
	if (ImGui::Button("Load"))
		m_pSession->Load(m_pSession->Get_DocPath());
}

void CPanel_Effect::Render_PreviewControls()
{
	if (ImGui::Button("Spawn Preview"))   m_pSession->Spawn_Preview();
	ImGui::SameLine();
	if (ImGui::Button("Stop Preview"))    m_pSession->Stop_Preview();
	ImGui::SameLine();
	if (ImGui::Button("Destroy Preview")) m_pSession->Destroy_Preview();

	ImGui::TextDisabled("Preview: %s", m_pSession->Is_PreviewAlive() ? "Alive" : "None");

	_float3 vPreviewPosition = m_pSession->Get_PreviewPosition();
	if (ImGui::DragFloat3("Preview Position", &vPreviewPosition.x, 0.05f, -100.f, 100.f, "%.2f"))
		m_pSession->Set_PreviewPosition(vPreviewPosition);

	if (ImGui::Button("Reset Preview Position"))
		m_pSession->Reset_PreviewPosition();
}

void CPanel_Effect::Render_EmitterList()
{
	const EFFECT_DEFINITION& doc = m_pSession->Get_Doc();

	ImGui::Text("Emitters: %d", static_cast<_int>(doc.Emitters.size()));

	for (_int i = 0; i < static_cast<_int>(doc.Emitters.size()); ++i)
	{
		const EMITTER_DEFINITION& emitter = doc.Emitters[i];
		_string label = emitter.strName.empty() ? ("Emitter " + std::to_string(i)) : emitter.strName;

			const _bool bSelected = (m_pSession->Get_SelectedEmitter() == i);
		if (ImGui::Selectable(label.c_str(), bSelected))
			m_pSession->Set_SelectedEmitter(i);
	}
}

void CPanel_Effect::Render_EmitterInspector()
{
	ImGui::Separator();
	ImGui::TextUnformatted("Emitter Inspector");

	EMITTER_DEFINITION* pEmitter = m_pSession->Get_SelectedEmitterMutable();
	if (nullptr == pEmitter)
	{
		ImGui::TextDisabled("No emitter selected.");
		return;
	}

	{
		_char szName[128] = {};
		strcpy_s(szName, pEmitter->strName.c_str());
		if (ImGui::InputText("Name", szName, IM_ARRAYSIZE(szName)))
		{
			pEmitter->strName = szName;
			m_pSession->Mark_Dirty("Emitter name changed");
		}
	}

	{
		_int iCapacity = static_cast<_int>(pEmitter->iCapacity);
		if (ImGui::InputInt("Capacity", &iCapacity))
		{
			pEmitter->iCapacity = static_cast<_uint>(max(1, iCapacity));
			m_pSession->Mark_Dirty("Emitter capacity changed");
		}
	}

	if (ImGui::InputFloat("Spawn Rate", &pEmitter->fSpawnRate, 1.f, 10.f, "%.2f"))
	{
		pEmitter->fSpawnRate = max(0.f, pEmitter->fSpawnRate);
		m_pSession->Mark_Dirty("Emitter spawn rate changed");
	}

	{
		_int iBurstCount = static_cast<_int>(pEmitter->iBurstCount);
		if (ImGui::InputInt("Burst Count", &iBurstCount))
		{
			pEmitter->iBurstCount = static_cast<_uint>(max(0, iBurstCount));
			m_pSession->Mark_Dirty("Emitter burst count changed");
		}
	}

	if (ImGui::DragFloat2("Life Time", &pEmitter->vLifeTimeRange.x, 0.01f, 0.01f, 60.f, "%.2f"))
	{
		if (pEmitter->vLifeTimeRange.x > pEmitter->vLifeTimeRange.y)
			std::swap(pEmitter->vLifeTimeRange.x, pEmitter->vLifeTimeRange.y);
		m_pSession->Mark_Dirty("Emitter lifetime changed");
	}

	if (ImGui::DragFloat2("Speed", &pEmitter->vSpeedRange.x, 0.01f, 0.f, 100.f, "%.2f"))
	{
		if (pEmitter->vSpeedRange.x > pEmitter->vSpeedRange.y)
			std::swap(pEmitter->vSpeedRange.x, pEmitter->vSpeedRange.y);
		m_pSession->Mark_Dirty("Emitter speed changed");
	}

	if (ImGui::DragFloat2("Size", &pEmitter->vSizeRange.x, 0.01f, 0.f, 100.f, "%.2f"))
	{
		if (pEmitter->vSizeRange.x > pEmitter->vSizeRange.y)
			std::swap(pEmitter->vSizeRange.x, pEmitter->vSizeRange.y);
		m_pSession->Mark_Dirty("Emitter size changed");
	}

	if (ImGui::DragFloat2("Rotation", &pEmitter->vRotationRange.x, 0.01f, -XM_2PI, XM_2PI, "%.2f"))
	{
		if (pEmitter->vRotationRange.x > pEmitter->vRotationRange.y)
			std::swap(pEmitter->vRotationRange.x, pEmitter->vRotationRange.y);
			m_pSession->Mark_Dirty("Emitter rotation changed");
	}

	if (ImGui::DragFloat2("Rotation Speed", &pEmitter->vRotationSpeedRange.x, 0.01f, -20.f, 20.f, "%.2f"))
	{
		if (pEmitter->vRotationSpeedRange.x > pEmitter->vRotationSpeedRange.y)
			std::swap(pEmitter->vRotationSpeedRange.x, pEmitter->vRotationSpeedRange.y);
		m_pSession->Mark_Dirty("Emitter rotation speed changed");
	}

	if (ImGui::DragFloat3("Emit Direction", &pEmitter->vEmitDirection.x, 0.01f, -1.f, 1.f, "%.2f"))
		m_pSession->Mark_Dirty("Emitter direction changed");

	if (ImGui::DragFloat("Cone Half Angle", &pEmitter->fEmitConeHalfAngle, 0.01f, 0.f, XM_PI, "%.3f"))
		m_pSession->Mark_Dirty("Emitter cone changed");

	if (ImGui::DragFloat3("Start Offset", &pEmitter->vStartOffset.x, 0.05f, -50.f, 50.f, "%.2f"))
		m_pSession->Mark_Dirty("Emitter start offset changed");

	if (ImGui::DragFloat3("Gravity", &pEmitter->vGravity.x, 0.1f, -100.f, 100.f, "%.2f"))
		m_pSession->Mark_Dirty("Emitter gravity changed");

	{
			const char* pBillboardLabels[] = { "VIEW_ALIGNED", "AXIS_LOCKED", "FIXED_NORMAL", "VELOCITY_ALIGNED" };
			_int iBillboard = static_cast<_int>(pEmitter->eBillboard);
			if (ImGui::Combo("Billboard", &iBillboard, pBillboardLabels, IM_ARRAYSIZE(pBillboardLabels)))
			{
					pEmitter->eBillboard = static_cast<BILLBOARD_MODE>(iBillboard);
					m_pSession->Mark_Dirty("Emitter billboard changed");
			}
	}

	if (ImGui::DragFloat3("Fixed Axis", &pEmitter->vBillboardFixedAxis.x, 0.01f, -1.f, 1.f, "%.2f"))
		m_pSession->Mark_Dirty("Emitter fixed axis changed");

	Draw_BlendCombo("Blend", pEmitter->eBlend, m_pSession, "Emitter blend changed");

	if (ImGui::Checkbox("Ignore Depth", &pEmitter->bIgnoreDepth))
		m_pSession->Mark_Dirty("Emitter ignore depth flag changed");

	if (ImGui::Checkbox("World Space", &pEmitter->bWorldSpace))
		m_pSession->Mark_Dirty("Emitter world space changed");

	if (ImGui::Checkbox("Auto Destroy On Empty", &pEmitter->bAutoDestroyOnEmpty))
		m_pSession->Mark_Dirty("Emitter auto destroy flag changed");

	if (ImGui::DragFloat("Start Delay", &pEmitter->fStartDelay, 0.01f, 0.f, 30.f, "%.3f"))
	{
		pEmitter->fStartDelay = max(0.f, pEmitter->fStartDelay);
		m_pSession->Mark_Dirty("Emitter start delay changed");
	}

	{
		_int iCols = static_cast<_int>(pEmitter->iAtlasCols);
		if (ImGui::InputInt("Atlas Cols", &iCols))
		{
			pEmitter->iAtlasCols = static_cast<_uint>(max(1, iCols));
			m_pSession->Mark_Dirty("Emitter atlas cols changed");
		}
	}

	{
		_int iRows = static_cast<_int>(pEmitter->iAtlasRows);
		if (ImGui::InputInt("Atlas Rows", &iRows))
		{
			pEmitter->iAtlasRows = static_cast<_uint>(max(1, iRows));
			m_pSession->Mark_Dirty("Emitter atlas rows changed");
		}
	}

	if (ImGui::DragFloat("Atlas FPS", &pEmitter->fAtlasFps, 0.5f, 0.f, 240.f, "%.2f"))
	{
		pEmitter->fAtlasFps = max(0.f, pEmitter->fAtlasFps);
		m_pSession->Mark_Dirty("Emitter atlas fps changed");
	}

	if (ImGui::Checkbox("Atlas Loop", &pEmitter->bAtlasLoop))
		m_pSession->Mark_Dirty("Emitter atlas loop changed");

	if (ImGui::Checkbox("Mirror UV", &pEmitter->bMirrorUV))
		m_pSession->Mark_Dirty("Emitter mirror uv changed");

	if (pEmitter->bMirrorUV && (pEmitter->iAtlasCols > 1 || pEmitter->iAtlasRows > 1))
		ImGui::TextColored(ImVec4(1.f, 0.6f, 0.2f, 1.f),
			"[!] Mirror UV is active — atlas settings are ignored.");

	Draw_TextureCombo("Texture", pEmitter->strTextureProtoTag, m_pSession, "Emitter texture changed");

	if (ImGui::CollapsingHeader("Curves", ImGuiTreeNodeFlags_DefaultOpen))
	{
		Draw_FloatCurveRow(pEmitter->curveSize, m_pSession, "Size Curve", "Size Pop", Set_SizePopCurve,
			"Clear Size", "Size Keys", 0.f, 100.f, "Size curve preset changed", "Size curve cleared", "Size curve key changed");
		Draw_ColorCurveRow(pEmitter->curveColor, m_pSession, "Color Curve", "Fire Color",
			"Clear Color", "Color Keys", "Color curve preset changed", "Color curve cleared", "Color curve key changed");
		Draw_FloatCurveRow(pEmitter->curveAlpha, m_pSession, "Alpha Curve", "Fade Alpha", Set_FadeAlphaCurve,
			"Clear Alpha", "Alpha Keys", 0.f, 1.f, "Alpha curve preset changed", "Alpha curve cleared", "Alpha curve key changed");
	}
}

void CPanel_Effect::Render_MeshList()
{
	const EFFECT_DEFINITION& doc = m_pSession->Get_Doc();

	ImGui::Separator();
	ImGui::Text("Meshes: %d", static_cast<_int>(doc.Meshes.size()));

	for (_int i = 0; i < static_cast<_int>(doc.Meshes.size()); ++i)
	{
		const MESH_EFFECT_DEFINITION& mesh = doc.Meshes[i];
		_string label = mesh.strName.empty() ? ("Mesh " + std::to_string(i)) : mesh.strName;

		ImGui::PushID(i + 10000);   // emitter Selectable과 ID 충돌 회피
		const _bool bSelected = (m_pSession->Get_SelectedMesh() == i);
		if (ImGui::Selectable(label.c_str(), bSelected))
			m_pSession->Set_SelectedMesh(i);
		ImGui::PopID();
	}
}

void CPanel_Effect::Render_MeshInspector()
{
	ImGui::Separator();
	ImGui::TextUnformatted("Mesh Inspector");

	MESH_EFFECT_DEFINITION* pMesh = m_pSession->Get_SelectedMeshMutable();
	if (nullptr == pMesh)
	{
		ImGui::TextDisabled("No mesh selected.");
	}
	else
	{
		{
			_char szName[128] = {};
			strcpy_s(szName, pMesh->strName.c_str());

			if (ImGui::InputText("Name##Mesh", szName, IM_ARRAYSIZE(szName)))
			{
				pMesh->strName = szName;
				m_pSession->Mark_Dirty("Mesh name changed");
			}
		}

		{
			const auto& MeshOptions = Game_PKM::g_EffectMeshOptions;

			_int iModel = 0;
			for (_int i = 0; i < static_cast<_int>(IM_ARRAYSIZE(MeshOptions)); ++i)
			{
				if (MeshOptions[i].strTag == pMesh->strModelProtoTag)
				{
					iModel = i;
					break;
				}
			}

			if (ImGui::BeginCombo("Model", MeshOptions[iModel].pLabel))
			{
				for (_int i = 0; i < static_cast<_int>(IM_ARRAYSIZE(MeshOptions)); ++i)
				{
					const _bool bSelected = (iModel == i);
					if (ImGui::Selectable(MeshOptions[i].pLabel, bSelected))
					{
						pMesh->strModelProtoTag = MeshOptions[i].strTag;
						m_pSession->Mark_Dirty("Mesh model changed");
					}
					if (bSelected) ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}
		}

		Draw_TextureCombo("Texture##Mesh", pMesh->strTextureProtoTag, m_pSession, "Mesh texture changed");

		Draw_BlendCombo("Blend##Mesh", pMesh->eBlend, m_pSession, "Mesh blend changed");

		if (ImGui::Checkbox("Ignore Depth##Mesh", &pMesh->bIgnoreDepth))
			m_pSession->Mark_Dirty("Mesh ignore depth flag changed");

		{
			const char* pScaleAxisLabels[] = { "Z_ONLY", "UNIFORM" };
			_int iScaleAxis = static_cast<_int>(pMesh->eScaleAxis);

			if (ImGui::Combo("Scale Axis", &iScaleAxis, pScaleAxisLabels,
				IM_ARRAYSIZE(pScaleAxisLabels)))
			{
				pMesh->eScaleAxis = static_cast<MESH_EFFECT_DEFINITION::SCALE_AXIS>(iScaleAxis);
				m_pSession->Mark_Dirty("Mesh scale axis changed");
			}
		}

		if (ImGui::DragFloat("Life Time##Mesh", &pMesh->fLifeTime, 0.01f, 0.01f, 60.f, "%.2f"))
		{
			pMesh->fLifeTime = max(0.01f, pMesh->fLifeTime);
			m_pSession->Mark_Dirty("Mesh life time changed");
		}

		{
			_int iCount = static_cast<_int>(pMesh->iCount);
			if (ImGui::InputInt("Count##Mesh", &iCount))
			{
				pMesh->iCount = static_cast<_uint>(max(1, iCount));
				m_pSession->Mark_Dirty("Mesh count changed");
			}
		}
		if (ImGui::DragFloat3("Start Offset##Mesh", &pMesh->vStartOffset.x, 0.05f, -50.f, 50.f, "%.2f"))
			m_pSession->Mark_Dirty("Mesh start offset changed");
		if (ImGui::DragFloat3("Emit Dir##Mesh", &pMesh->vEmitDirection.x, 0.01f, -1.f, 1.f, "%.2f"))
			m_pSession->Mark_Dirty("Mesh emit dir changed");
		if (ImGui::DragFloat("Cone Half Angle##Mesh", &pMesh->fEmitConeHalfAngle, 0.01f, 0.f, XM_PI, "%.3f"))
			m_pSession->Mark_Dirty("Mesh cone changed");
		if (ImGui::DragFloat2("Speed Range##Mesh", &pMesh->vSpeedRange.x, 0.05f, 0.f, 100.f, "%.2f"))
			m_pSession->Mark_Dirty("Mesh speed range changed");
		if (ImGui::DragFloat3("Gravity##Mesh", &pMesh->vGravity.x, 0.1f, -100.f, 100.f, "%.2f"))
			m_pSession->Mark_Dirty("Mesh gravity changed");
		if (ImGui::DragFloat("Spin Max##Mesh", &pMesh->fSpinSpeedMax, 0.05f, 0.f, 50.f, "%.2f"))
			m_pSession->Mark_Dirty("Mesh spin changed");
		if (ImGui::DragFloat("Start Delay##Mesh", &pMesh->fStartDelay, 0.01f, 0.f, 10.f, "%.3f"))
			m_pSession->Mark_Dirty("Mesh start delay changed");

		if (ImGui::CollapsingHeader("Curves##Mesh", ImGuiTreeNodeFlags_DefaultOpen))
		{
			Draw_FloatCurveRow(pMesh->curveScale, m_pSession, "Scale Curve", "Size Pop##MeshScale", Set_SizePopCurve, "Clear Scale",
				"Scale Keys##Mesh", 0.f, 100.f, "Mesh scale curve preset changed", "Mesh scale curve cleared", "Mesh scale curve key changed", 0.001f, "%.3f");
			Draw_ColorCurveRow(pMesh->curveColor, m_pSession, "Color Curve##Mesh", "Fire Color##Mesh", "Clear Color##Mesh",
				"Color Keys##Mesh", "Mesh color curve preset changed", "Mesh color curve cleared", "Mesh color curve key changed");
			Draw_FloatCurveRow(pMesh->curveAlpha, m_pSession, "Alpha Curve##Mesh", "Fade Alpha##Mesh", Set_FadeAlphaCurve, "Clear Alpha##Mesh",
				"Alpha Keys##Mesh", 0.f, 1.f, "Mesh alpha curve preset changed", "Mesh alpha curve cleared", "Mesh alpha curve key changed");
		}
	}
}

void CPanel_Effect::Render_TrailList()
{
	const EFFECT_DEFINITION& doc = m_pSession->Get_Doc();

	ImGui::Separator();
	ImGui::Text("Trails: %d", static_cast<_int>(doc.Trails.size()));

	for (_int i = 0; i < static_cast<_int>(doc.Trails.size()); ++i)
	{
		const TRAIL_DEFINITION& trail = doc.Trails[i];
		_string label = trail.strName.empty()
			? ("Trail " + std::to_string(i))
			: trail.strName;

		ImGui::PushID(i + 20000);   // emitter(0)/mesh(+10000) Selectable과 ID 충돌 회피
		const _bool bSelected = (m_pSession->Get_SelectedTrail() == i);
		if (ImGui::Selectable(label.c_str(), bSelected))
			m_pSession->Set_SelectedTrail(i);
		ImGui::PopID();
	}
}

void CPanel_Effect::Render_TrailInspector()
{
	ImGui::Separator();
	ImGui::TextUnformatted("Trail Inspector");

	TRAIL_DEFINITION* pTrail = m_pSession->Get_SelectedTrailMutable();
	if (nullptr == pTrail)
	{
		ImGui::TextDisabled("No trail selected.");
		return;
	}

	{
		_char szName[128] = {};
		strcpy_s(szName, pTrail->strName.c_str());
		if (ImGui::InputText("Name##Trail", szName, IM_ARRAYSIZE(szName)))
		{
			pTrail->strName = szName;
			m_pSession->Mark_Dirty("Trail name changed");
		}
	}

	{
		_int iMaxSegments = static_cast<_int>(pTrail->iMaxSegments);
		if (ImGui::InputInt("Max Segments", &iMaxSegments))
		{
			pTrail->iMaxSegments = static_cast<_uint>(max(2, iMaxSegments));
			m_pSession->Mark_Dirty("Trail max segments changed");
		}
	}

	if (ImGui::DragFloat("Segment Spacing", &pTrail->fSegmentSpacing, 0.005f, 0.001f, 5.f, "%.3f"))
	{
		pTrail->fSegmentSpacing = max(0.001f, pTrail->fSegmentSpacing);
		m_pSession->Mark_Dirty("Trail segment spacing changed");
	}

	if (ImGui::DragFloat("Life Per Segment", &pTrail->fLifeTimePerSegment, 0.01f, 0.01f, 10.f, "%.3f"))
	{
		pTrail->fLifeTimePerSegment = max(0.01f, pTrail->fLifeTimePerSegment);
		m_pSession->Mark_Dirty("Trail life per segment changed");
	}

	if (ImGui::DragFloat("Width Start", &pTrail->fWidthStart, 0.001f, 0.f, 50.f, "%.3f"))
	{
		pTrail->fWidthStart = max(0.f, pTrail->fWidthStart);
		m_pSession->Mark_Dirty("Trail width start changed");
	}

	if (ImGui::DragFloat("Width End", &pTrail->fWidthEnd, 0.001f, 0.f, 50.f, "%.3f"))
	{
		pTrail->fWidthEnd = max(0.f, pTrail->fWidthEnd);
		m_pSession->Mark_Dirty("Trail width end changed");
	}

	if (ImGui::DragFloat3("Up Axis", &pTrail->vUpAxis.x, 0.01f, -1.f, 1.f, "%.2f"))
		m_pSession->Mark_Dirty("Trail up axis changed");

	Draw_TextureCombo("Texture##Trail", pTrail->strTextureProtoTag, m_pSession, "Trail texture changed");
		Draw_BlendCombo("Blend##Trail", pTrail->eBlend, m_pSession, "Trail blend changed");

	if (ImGui::Checkbox("Ignore Depth##Trail", &pTrail->bIgnoreDepth))
		m_pSession->Mark_Dirty("Trail ignore depth flag changed");

	if (ImGui::CollapsingHeader("Curves##Trail", ImGuiTreeNodeFlags_DefaultOpen))
	{
		Draw_ColorCurveRow(pTrail->curveColor, m_pSession, "Color Curve##Trail", "Fire Color##Trail", "Clear Color##Trail", "Color Keys##Trail",
			"Trail color curve preset changed", "Trail color curve cleared", "Trail color curve changed");
	}
}

CPanel_Effect* CPanel_Effect::Create()
{
	CPanel_Effect* pInstance = new CPanel_Effect();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Create : CPanel_Effect");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPanel_Effect::Free()
{
	Safe_Release(m_pSession);

	__super::Free();
}