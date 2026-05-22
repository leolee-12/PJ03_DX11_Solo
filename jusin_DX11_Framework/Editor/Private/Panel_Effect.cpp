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
		_float fValueMax)
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
			if (ImGui::DragFloat("V", &fValue, 0.01f, fValueMin, fValueMax, "%.2f"))
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

	const EFFECT_DEFINITION& doc = m_pSession->Get_Doc();

	{
		_char szID[128] = {};
		strncpy_s(szID, IM_ARRAYSIZE(szID), doc.strID.c_str(), _TRUNCATE);

		if (ImGui::InputText("ID", szID, IM_ARRAYSIZE(szID)))
			m_pSession->Set_DocID(szID);
	}

	{
		_char szPath[260] = {};
		strncpy_s(szPath, IM_ARRAYSIZE(szPath), m_pSession->Get_DocPath().c_str(), _TRUNCATE);

		if (ImGui::InputText("Path", szPath, IM_ARRAYSIZE(szPath)))
			m_pSession->Set_DocPath(szPath);
	}

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



	if (ImGui::Button("Spawn Preview"))
		m_pSession->Spawn_Preview();

	ImGui::SameLine();

	if (ImGui::Button("Stop Preview"))
		m_pSession->Stop_Preview();

	ImGui::SameLine();

	if (ImGui::Button("Destroy Preview"))
		m_pSession->Destroy_Preview();



	ImGui::TextDisabled("Preview: %s", m_pSession->Is_PreviewAlive() ? "Alive" : "None");

	_float3 vPreviewPosition = m_pSession->Get_PreviewPosition();
	if (ImGui::DragFloat3("Preview Position", &vPreviewPosition.x, 0.05f, -100.f, 100.f, "%.2f"))
		m_pSession->Set_PreviewPosition(vPreviewPosition);



	if (ImGui::Button("Reset Preview Position"))
		m_pSession->Reset_PreviewPosition();



	if (ImGui::Button("Add Emitter"))
		m_pSession->Add_Emitter();

	ImGui::SameLine();

	const _bool bHasSelection = (m_pSession->Get_SelectedEmitter() >= 0);
	ImGui::BeginDisabled(!bHasSelection);
	if (ImGui::Button("Remove Emitter"))
		m_pSession->Erase_SelectedEmitter();
	ImGui::EndDisabled();



	if (ImGui::Button("Add Mesh"))
		m_pSession->Add_Mesh();

	ImGui::SameLine();

	const _bool bHasMeshSelection = (m_pSession->Get_SelectedMesh() >= 0);
	ImGui::BeginDisabled(!bHasMeshSelection);
	if (ImGui::Button("Remove Mesh"))
		m_pSession->Erase_SelectedMesh();
	ImGui::EndDisabled();

	ImGui::Separator();

	ImGui::Text("Emitters: %d", static_cast<_int>(doc.Emitters.size()));

	for (_int i = 0; i < static_cast<_int>(doc.Emitters.size()); ++i)
	{
		const EMITTER_DEFINITION& emitter = doc.Emitters[i];

		_string label = emitter.strName.empty()
			? ("Emitter " + std::to_string(i))
			: emitter.strName;

		const _bool bSelected = (m_pSession->Get_SelectedEmitter() == i);
		if (ImGui::Selectable(label.c_str(), bSelected))
			m_pSession->Set_SelectedEmitter(i);
	}

	ImGui::Separator();
	ImGui::TextUnformatted("Emitter Inspector");

	EMITTER_DEFINITION* pEmitter = m_pSession->Get_SelectedEmitterMutable();
	if (nullptr == pEmitter)
	{
		ImGui::TextDisabled("No emitter selected.");
	}
	else
	{

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

		if (ImGui::DragFloat2("Rotation Speed", &pEmitter->vRotationSpeedRange.x, 0.01f, -20.f, 20.f,
			"%.2f"))
		{
			if (pEmitter->vRotationSpeedRange.x > pEmitter->vRotationSpeedRange.y)
				std::swap(pEmitter->vRotationSpeedRange.x, pEmitter->vRotationSpeedRange.y);

			m_pSession->Mark_Dirty("Emitter rotation speed changed");
		}

		if (ImGui::DragFloat3("Emit Direction", &pEmitter->vEmitDirection.x, 0.01f, -1.f, 1.f, "%.2f"))
			m_pSession->Mark_Dirty("Emitter direction changed");

		if (ImGui::DragFloat("Cone Half Angle", &pEmitter->fEmitConeHalfAngle, 0.01f, 0.f, XM_PI, "%.3f"))
			m_pSession->Mark_Dirty("Emitter cone changed");

		{
			const char* pBillboardLabels[] = { "VIEW_ALIGNED", "AXIS_LOCKED", "FIXED_NORMAL", "VELOCITY_ALIGNED" };

			_int iBillboard = static_cast<_int>(pEmitter->eBillboard);

			if (ImGui::Combo("Billboard", &iBillboard, pBillboardLabels, IM_ARRAYSIZE(pBillboardLabels)))
			{
				pEmitter->eBillboard =
					static_cast<BILLBOARD_MODE>(iBillboard);
				m_pSession->Mark_Dirty("Emitter billboard changed");
			}
		}

		if (ImGui::DragFloat3("Fixed Axis", &pEmitter->vBillboardFixedAxis.x, 0.01f, -1.f, 1.f, "%.2f"))
			m_pSession->Mark_Dirty("Emitter fixed axis changed");

		{
			const char* pBlendLabels[] = { "ALPHA", "ADDITIVE" };
			_int iBlend = static_cast<_int>(pEmitter->eBlend);

			if (ImGui::Combo("Blend", &iBlend, pBlendLabels, IM_ARRAYSIZE(pBlendLabels)))
			{
				pEmitter->eBlend =
					static_cast<BLEND_MODE>(iBlend);
				m_pSession->Mark_Dirty("Emitter blend changed");
			}
		}

		if (ImGui::Checkbox("Ignore Depth", &pEmitter->bIgnoreDepth))
			m_pSession->Mark_Dirty("Emitter ignore depth flag changed");

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

		if (pEmitter->bMirrorUV
			&& (pEmitter->iAtlasCols > 1 || pEmitter->iAtlasRows > 1))
		{
			ImGui::TextColored(ImVec4(1.f, 0.6f, 0.2f, 1.f),
				"[!] Mirror UV is active — atlas settings are ignored.");
		}

		const auto& TextureOptions = Game_PKM::g_EffectTextureOptions;

		_int iTexture = 0;
		for (_int i = 0; i < static_cast<_int>(IM_ARRAYSIZE(TextureOptions)); ++i)
		{
			if (TextureOptions[i].strTag == pEmitter->strTextureProtoTag)
			{
				iTexture = i;
				break;
			}
		}

		if (ImGui::BeginCombo("Texture", TextureOptions[iTexture].pLabel))
		{
			for (_int i = 0; i < static_cast<_int>(IM_ARRAYSIZE(TextureOptions)); ++i)
			{
				const _bool bSelected = (iTexture == i);
				if (ImGui::Selectable(TextureOptions[i].pLabel, bSelected))
				{
					pEmitter->strTextureProtoTag = TextureOptions[i].strTag;
					m_pSession->Mark_Dirty("Emitter texture changed");
				}

				if (bSelected)
					ImGui::SetItemDefaultFocus();
			}

			ImGui::EndCombo();
		}

		if (ImGui::CollapsingHeader("Curves", ImGuiTreeNodeFlags_DefaultOpen))
		{
			Plot_FloatCurve("Size Curve", pEmitter->curveSize);

			if (ImGui::Button("Size Pop"))
			{
				Set_SizePopCurve(pEmitter->curveSize);
				m_pSession->Mark_Dirty("Size curve preset changed");
			}

			ImGui::SameLine();

			if (ImGui::Button("Clear Size"))
			{
				pEmitter->curveSize.Clear();
				m_pSession->Mark_Dirty("Size curve cleared");
			}
			Draw_FloatCurveKeys("Size Keys", pEmitter->curveSize, m_pSession, "Size curve key changed", 0.f, 2.f);
			Plot_ColorCurve("Color Curve", pEmitter->curveColor);

			if (ImGui::Button("Fire Color"))
			{
				Set_FireColorCurve(pEmitter->curveColor);
				m_pSession->Mark_Dirty("Color curve preset changed");
			}

			ImGui::SameLine();

			if (ImGui::Button("Clear Color"))
			{
				pEmitter->curveColor.Clear();
				m_pSession->Mark_Dirty("Color curve cleared");
			}
			Draw_ColorCurveKeys("Color Keys", pEmitter->curveColor, m_pSession, "Color curve key changed");
			Plot_FloatCurve("Alpha Curve", pEmitter->curveAlpha);

			if (ImGui::Button("Fade Alpha"))
			{
				Set_FadeAlphaCurve(pEmitter->curveAlpha);
				m_pSession->Mark_Dirty("Alpha curve preset changed");
			}

			ImGui::SameLine();

			if (ImGui::Button("Clear Alpha"))
			{
				pEmitter->curveAlpha.Clear();
				m_pSession->Mark_Dirty("Alpha curve cleared");
			}

			Draw_FloatCurveKeys("Alpha Keys", pEmitter->curveAlpha, m_pSession, "Alpha curve key changed", 0.f, 1.f);
		}
	}

	ImGui::Separator();
	ImGui::Text("Meshes: %d", static_cast<_int>(doc.Meshes.size()));

	for (_int i = 0; i < static_cast<_int>(doc.Meshes.size()); ++i)
	{
		const MESH_EFFECT_DEFINITION& mesh = doc.Meshes[i];

		_string label = mesh.strName.empty()
			? ("Mesh " + std::to_string(i))
			: mesh.strName;

		/* emitter Selectable과 ID 충돌 회피 */
		ImGui::PushID(i + 10000);

		const _bool bSelected = (m_pSession->Get_SelectedMesh() == i);
		if (ImGui::Selectable(label.c_str(), bSelected))
			m_pSession->Set_SelectedMesh(i);

		ImGui::PopID();
	}

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

		{
			const auto& TextureOptions = Game_PKM::g_EffectTextureOptions;

			_int iTexture = 0;
			for (_int i = 0; i < static_cast<_int>(IM_ARRAYSIZE(TextureOptions)); ++i)
			{
				if (TextureOptions[i].strTag == pMesh->strTextureProtoTag)
				{
					iTexture = i;
					break;
				}
			}

			if (ImGui::BeginCombo("Texture##Mesh", TextureOptions[iTexture].pLabel))
			{
				for (_int i = 0; i < static_cast<_int>(IM_ARRAYSIZE(TextureOptions)); ++i)
				{
					const _bool bSelected = (iTexture == i);
					if (ImGui::Selectable(TextureOptions[i].pLabel, bSelected))
					{
						pMesh->strTextureProtoTag = TextureOptions[i].strTag;
						m_pSession->Mark_Dirty("Mesh texture changed");
					}
					if (bSelected) ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}
		}

		{
			const char* pBlendLabels[] = { "ALPHA", "ADDITIVE" };
			_int iBlend = static_cast<_int>(pMesh->eBlend);

			if (ImGui::Combo("Blend##Mesh", &iBlend, pBlendLabels, IM_ARRAYSIZE(pBlendLabels)))
			{
				pMesh->eBlend = static_cast<BLEND_MODE>(iBlend);
				m_pSession->Mark_Dirty("Mesh blend changed");
			}
		}

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

		if (ImGui::CollapsingHeader("Curves##Mesh", ImGuiTreeNodeFlags_DefaultOpen))
		{
			Plot_FloatCurve("Scale Curve", pMesh->curveScale);

			if (ImGui::Button("Size Pop##MeshScale"))
			{
				Set_SizePopCurve(pMesh->curveScale);
				m_pSession->Mark_Dirty("Mesh scale curve preset changed");
			}
			ImGui::SameLine();
			if (ImGui::Button("Clear Scale"))
			{
				pMesh->curveScale.Clear();
				m_pSession->Mark_Dirty("Mesh scale curve cleared");
			}
			Draw_FloatCurveKeys("Scale Keys##Mesh", pMesh->curveScale, m_pSession,
				"Mesh scale curve key changed", 0.f, 5.f);

			Plot_ColorCurve("Color Curve##Mesh", pMesh->curveColor);

			if (ImGui::Button("Fire Color##Mesh"))
			{
				Set_FireColorCurve(pMesh->curveColor);
				m_pSession->Mark_Dirty("Mesh color curve preset changed");
			}
			ImGui::SameLine();
			if (ImGui::Button("Clear Color##Mesh"))
			{
				pMesh->curveColor.Clear();
				m_pSession->Mark_Dirty("Mesh color curve cleared");
			}
			Draw_ColorCurveKeys("Color Keys##Mesh", pMesh->curveColor, m_pSession,
				"Mesh color curve key changed");

			Plot_FloatCurve("Alpha Curve##Mesh", pMesh->curveAlpha);

			if (ImGui::Button("Fade Alpha##Mesh"))
			{
				Set_FadeAlphaCurve(pMesh->curveAlpha);
				m_pSession->Mark_Dirty("Mesh alpha curve preset changed");
			}
			ImGui::SameLine();
			if (ImGui::Button("Clear Alpha##Mesh"))
			{
				pMesh->curveAlpha.Clear();
				m_pSession->Mark_Dirty("Mesh alpha curve cleared");
			}
			Draw_FloatCurveKeys("Alpha Keys##Mesh", pMesh->curveAlpha, m_pSession,
				"Mesh alpha curve key changed", 0.f, 1.f);
		}
	}

	End_Panel();
	return S_OK;
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