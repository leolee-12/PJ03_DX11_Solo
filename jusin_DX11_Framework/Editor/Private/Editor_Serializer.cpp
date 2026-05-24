#include "Editor_Serializer.h"
#include "EditInstance.h"

namespace Helper
{
	constexpr _float kLegacyDesignWidth = 1280.f;
	constexpr _float kLegacyDesignHeight = 720.f;

	// -- enum 변환: Engine_UI.h의 To_String / *_From_String 래핑 --
	inline _string EnumToStr(UI_TYPE			e) { return Engine::To_String(e); }
	inline _string EnumToStr(UI_LAYOUT			e) { return Engine::To_String(e); }
	inline _string EnumToStr(UI_PROGRESS_DIR	e) { return Engine::To_String(e); }
	inline _string EnumToStr(UI_TWEEN_TARGET	e) { return Engine::To_String(e); }
	inline _string EnumToStr(UI_EASE			e) { return Engine::To_String(e); }
	inline _string EnumToStr(UI_TWEEN_LOOP		e) { return Engine::To_String(e); }
	inline _string EnumToStr(UI_SEQ_STEP_KIND	e) { return Engine::To_String(e); }
	inline _string EnumToStr(UI_ANCHOR			e) { return Engine::To_String(e); }
	inline _string EnumToStr(UI_TEXT_ALIGN		e) { return Engine::To_String(e); }

	// -- WNameID <-> UTF-8 --
#ifdef _DEBUG
	inline _string TagToS(WNameID id)
	{
		if (INVALID_TAG == id) return {};
		const wchar_t* w = Engine::WNameRegistry::Lookup(id);
		return WtoS(_wstring(w));
	}
#else
// Release: registry 미동작 -> 해시 정수를 문자열화해 폴백
	inline _string TagToS(WNameID id)
	{
		return (INVALID_TAG == id) ? _string{} : std::to_string(static_cast<_uint32>(id));
	}
#endif

	// Load: 문자열이면 re-hash, 숫자면 그대로 파싱
	inline WNameID SToTag(const _string& s)
	{
		if (s.empty()) return INVALID_TAG;

		// release 빌드 출력물("123456789")도 로드 가능하게 숫자 먼저 시도
		if (!s.empty() && (s[0] == '-' || (s[0] >= '0' && s[0] <= '9')))
		{
			try { return static_cast<WNameID>(std::stoul(s)); }
			catch (...) { /* fall through */ }
		}
		return WNAME(StoW(s));   // DEBUG: registry 등록 + 해시, RELEASE: 해시
	}

	// --------------------- 기초 DESC ---------------------

	json To_Json(const UIANCHOR_DESC& d)
	{
		return {
			{ "kind",            EnumToStr(d.eAnchor) },
			{ "offsetX",         d.fOffsetX },
			{ "offsetY",         d.fOffsetY },
			{ "useAnchoredPos",  d.bUseAnchoredPos },
		};
	}
	void From_Json(const json& j, UIANCHOR_DESC& d)
	{
		d.eAnchor = UI_ANCHOR_From_String(j.value("kind", "MC").c_str());
		d.fOffsetX = j.value("offsetX", 0.f);
		d.fOffsetY = j.value("offsetY", 0.f);
		d.bUseAnchoredPos = j.value("useAnchoredPos", false);
	}

	json To_Json(const UILAYOUT_SLOT_DESC& d)
	{
		return {
			{ "margin",    { d.vMargin.x, d.vMargin.y, d.vMargin.z, d.vMargin.w } },
			{ "desiredX",  d.fDesiredSizeX },
			{ "desiredY",  d.fDesiredSizeY },
		};
	}
	void From_Json(const json& j, UILAYOUT_SLOT_DESC& d)
	{
		auto m = j.value("margin", json::array({ 0.f,0.f,0.f,0.f }));
		d.vMargin = { m[0], m[1], m[2], m[3] };
		d.fDesiredSizeX = j.value("desiredX", 0.f);
		d.fDesiredSizeY = j.value("desiredY", 0.f);
	}

	// UIOBJECT_DESC 공통 필드 직렬화 - 각 leaf DESC가 이걸 재사용
	json Base_To_Json(const CUIObject::UIOBJECT_DESC& d)
	{
		return {
				{ "transform", {
						{ "centerX",  d.fCenterX },
						{ "centerY",  d.fCenterY },
						{ "sizeX",    d.fSizeX },
						{ "sizeY",    d.fSizeY },
						{ "zOrder",   d.iZOrder },
						{ "visible",  d.bVisible },
						{ "pivotX",   d.fPivotX },
						{ "pivotY",   d.fPivotY },
				}},
				{ "anchor",     To_Json(d.tAnchorDesc) },
				{ "layoutSlot", To_Json(d.tLayoutSlot) },
		};
	}
	void Base_From_Json(const json& j, CUIObject::UIOBJECT_DESC& d)
	{
		if (j.contains("transform"))
		{
			const auto& t = j["transform"];
			d.fCenterX = t.value("centerX", 0.f);
			d.fCenterY = t.value("centerY", 0.f);
			d.fSizeX = t.value("sizeX", 100.f);
			d.fSizeY = t.value("sizeY", 100.f);
			d.iZOrder = t.value("zOrder", 0);
			d.bVisible = t.value("visible", true);
			d.fPivotX = t.value("pivotX", 0.5f);
			d.fPivotY = t.value("pivotY", 0.5f);
		}
		if (j.contains("anchor"))     From_Json(j["anchor"], d.tAnchorDesc);
		if (j.contains("layoutSlot")) From_Json(j["layoutSlot"], d.tLayoutSlot);
	}

	// --------------------- leaf DESC 5종 ---------------------

	// 공통 shader/viBuffer 기본 태그·레벨(temp §6.2)
	constexpr const _char* kDefaultShader = "Prototype_Component_Shader_UI";
	constexpr const _char* kDefaultVIBuffer = "Prototype_Component_VIBuffer_Rect";

	json To_Json(const CUIImage::UIIMAGE_DESC& d)
	{
		json j = Base_To_Json(d);
		j["shader"]		= {	{ "tag", TagToS(d.strShaderTag) },
							{ "level", d.iShaderLevel },
							{ "pass",  d.iShaderPass } };
		j["viBuffer"]	= {	{ "tag", TagToS(d.strVIBufferTag) },
							{ "level", d.iVIBufferLevel } };
		j["texture"]	= {	{ "tag", TagToS(d.strTextureTag) },
							{ "level", d.iTextureLevel },
							{ "index", d.iTextureIndex } };
		j["color"]		= { d.vColor.x, d.vColor.y, d.vColor.z, d.vColor.w };
		j["spriteAnim"] = { { "enabled",       d.bSpriteAnimEnabled },
							{ "frameDuration", d.fSpriteFrameDuration },
							{ "loop",          d.bSpriteAnimLoop } };

		json jArr = json::array();
		for (const auto& b : d.SharedTextureBindings)
		{
			jArr.push_back({
				{ "group",     b.strSharedTexName },
				{ "shaderVar", b.strShaderVarName },
				{ "index",     b.iTextureIndex },
				});
		}
		j["sharedTextures"] = std::move(jArr);

		return j;
	}
	void From_Json(const json& j, CUIImage::UIIMAGE_DESC& d)
	{
		Base_From_Json(j, d);
		const auto sh = j.value("shader", json::object());
		const auto vb = j.value("viBuffer", json::object());
		const auto tx = j.value("texture", json::object());
		const auto jSprite = j.value("spriteAnim", json::object());

		d.strShaderTag = SToTag(sh.value("tag", _string(kDefaultShader)));
		d.iShaderLevel = sh.value("level", 0u);
		d.iShaderPass = sh.value("pass", 0u);

		d.strVIBufferTag = SToTag(vb.value("tag", _string(kDefaultVIBuffer)));
		d.iVIBufferLevel = vb.value("level", 0u);

		d.strTextureTag = SToTag(tx.value("tag", _string{}));
		d.iTextureLevel = tx.value("level", 0u);
		d.iTextureIndex = tx.value("index", 0u);

		auto c = j.value("color", json::array({ 1.f,1.f,1.f,1.f }));
		d.vColor = { c[0], c[1], c[2], c[3] };

		d.bSpriteAnimEnabled = jSprite.value("enabled", false);
		d.fSpriteFrameDuration = jSprite.value("frameDuration", 0.f);
		d.bSpriteAnimLoop = jSprite.value("loop", true);

		d.SharedTextureBindings.clear();
		for (const auto& jb : j.value("sharedTextures", json::array()))
		{
			UI_SHARED_TEXTURE_BINDING_DESC b{};
			b.strSharedTexName = jb.value("group", _string{});
			b.strShaderVarName = jb.value("shaderVar", _string{});
			b.iTextureIndex = jb.value("index", static_cast<unsigned int>(-1));

			if (b.strSharedTexName.empty()
				|| b.strShaderVarName.empty()
				|| b.iTextureIndex == static_cast<unsigned int>(-1))
				continue;       // 결손 entry는 묵음 skip - 사용자 편집 중간 상태 보호

			d.SharedTextureBindings.push_back(std::move(b));
		}
	}

	// UIBUTTON_DESC - shader/viBuffer/texture/interactable/color.
	json To_Json(const CUIButton::UIBUTTON_DESC& d)
	{
		json j = Base_To_Json(d);

		j["shader"] = {
			{ "tag", TagToS(d.strShaderTag) },
			{ "level", d.iShaderLevel }
		};

		j["viBuffer"] = {
			{ "tag", TagToS(d.strVIBufferTag) },
			{ "level", d.iVIBufferLevel }
		};

		j["texture"] = {
			{ "tag", TagToS(d.strTextureTag) },
			{ "level", d.iTextureLevel }
		};

		j["interactable"] = d.bInteractable;
		j["color"] = { d.vColor.x, d.vColor.y, d.vColor.z, d.vColor.w };

		return j;
	}
	void From_Json(const json& j, CUIButton::UIBUTTON_DESC& d)
	{
		Base_From_Json(j, d);

		const auto sh = j.value("shader", json::object());
		const auto vb = j.value("viBuffer", json::object());
		const auto tx = j.value("texture", json::object());

		d.strShaderTag = SToTag(sh.value("tag", _string(kDefaultShader)));
		d.iShaderLevel = sh.value("level", 0u);

		d.strVIBufferTag = SToTag(vb.value("tag", _string(kDefaultVIBuffer)));
		d.iVIBufferLevel = vb.value("level", 0u);

		d.strTextureTag = SToTag(tx.value("tag", _string{}));
		d.iTextureLevel = tx.value("level", 0u);

		d.bInteractable = j.value("interactable", true);

		auto c = j.value("color", json::array({ 1.f,1.f,1.f,1.f }));
		d.vColor = { c[0], c[1], c[2], c[3] };

		// Legacy buttonIndices are intentionally ignored on load.
	}

	// UIPROGRESSBAR_DESC
	json To_Json(const CUIProgressBar::UIPROGRESSBAR_DESC& d)
	{
		json j = Base_To_Json(d);
		j["shader"] = { { "tag", TagToS(d.strShaderTag) }, { "level", d.iShaderLevel } };
		j["viBuffer"] = { { "tag", TagToS(d.strVIBufferTag) }, { "level", d.iVIBufferLevel } };
		j["back"] = { { "tag", TagToS(d.strBackTextureTag) }, { "level", d.iBackTextureLevel }, { "index", d.iBackTextureIndex } };
		j["fill"] = { { "tag", TagToS(d.strFillTextureTag) }, { "level", d.iFillTextureLevel }, { "index", d.iFillTextureIndex } };
		j["backColor"] = { d.vBackColor.x, d.vBackColor.y, d.vBackColor.z, d.vBackColor.w };
		j["fillColor"] = { d.vFillColor.x, d.vFillColor.y, d.vFillColor.z, d.vFillColor.w };
		j["fillAmount"] = d.fFillAmount;
		j["direction"] = EnumToStr(d.eDirection);
		return j;
	}
	void From_Json(const json& j, CUIProgressBar::UIPROGRESSBAR_DESC& d)
	{
		Base_From_Json(j, d);
		const auto sh = j.value("shader", json::object());
		const auto vb = j.value("viBuffer", json::object());
		const auto bk = j.value("back", json::object());
		const auto fl = j.value("fill", json::object());

		d.strShaderTag = SToTag(sh.value("tag", _string(kDefaultShader)));
		d.iShaderLevel = sh.value("level", 0u);
		d.strVIBufferTag = SToTag(vb.value("tag", _string(kDefaultVIBuffer)));
		d.iVIBufferLevel = vb.value("level", 0u);

		d.strBackTextureTag = SToTag(bk.value("tag", _string{}));
		d.iBackTextureLevel = bk.value("level", 0u);
		d.iBackTextureIndex = bk.value("index", 0u);

		d.strFillTextureTag = SToTag(fl.value("tag", _string{}));
		d.iFillTextureLevel = fl.value("level", 0u);
		d.iFillTextureIndex = fl.value("index", 0u);

		auto bc = j.value("backColor", json::array({ 1.f,1.f,1.f,1.f }));
		d.vBackColor = { bc[0], bc[1], bc[2], bc[3] };

		auto fc = j.value("fillColor", json::array({ 1.f,1.f,1.f,1.f }));
		d.vFillColor = { fc[0], fc[1], fc[2], fc[3] };

		d.fFillAmount = std::clamp(j.value("fillAmount", 1.f), 0.f, 1.f);

		d.eDirection = UI_PROGRESS_DIR_From_String(
			j.value("direction", _string("LEFT_TO_RIGHT")).c_str());
	}

	// UITEXT_DESC
	json To_Json(const CUIText::UITEXT_DESC& d)
	{
		json j = Base_To_Json(d);
		j["text"] = WtoS(d.strText);
		j["font"] = TagToS(d.strFontTag);
		j["align"] = EnumToStr(d.eAlign);
		j["color"] = { d.vColor.x, d.vColor.y, d.vColor.z, d.vColor.w };
		return j;
	}
	void From_Json(const json& j, CUIText::UITEXT_DESC& d)
	{
		Base_From_Json(j, d);
		d.strText = StoW(j.value("text", _string{}));
		d.strFontTag = SToTag(j.value("font", _string{}));
		d.eAlign = UI_TEXT_ALIGN_From_String(j.value("align", "LEFT").c_str());
		auto c = j.value("color", json::array({ 1.f,1.f,1.f,1.f }));
		d.vColor = { c[0], c[1], c[2], c[3] };
	}

	// UICONTAINER_DESC - layout 필드 추가 (O28-6 Phase A 범위에 포함)
	json To_Json(const CUIContainer::UICONTAINER_DESC& d)
	{
		json j = Base_To_Json(d);
		j["layout"] = {
			{ "kind",     EnumToStr(d.tLayoutDesc.eLayout) },
			{ "padding",  d.tLayoutDesc.fPadding },
			{ "spacing",  d.tLayoutDesc.fSpacing },
		};
		return j;
	}
	void From_Json(const json& j, CUIContainer::UICONTAINER_DESC& d)
	{
		Base_From_Json(j, d);
		auto lo = j.value("layout", json::object());
		d.tLayoutDesc.eLayout = UI_LAYOUT_From_String(
			lo.value("kind", _string("VERTICAL")).c_str());
		d.tLayoutDesc.fPadding = lo.value("padding", 0.f);
		d.tLayoutDesc.fSpacing = lo.value("spacing", 0.f);
	}

	// --------------------- UITween ---------------------

	json To_Json(const CUITween::UITWEEN_DESC& t)
	{
		return {
			{ "target",   EnumToStr(t.eTarget) },
			{ "start",    t.fStart },
			{ "end",      t.fEnd },
			{ "duration", t.fDuration },
			{ "ease",     EnumToStr(t.eEase) },
			{ "loop",     EnumToStr(t.eLoop) },
		};
	}
	void From_Json(const json& j, CUITween::UITWEEN_DESC& t)
	{
		t.eTarget = UI_TWEEN_TARGET_From_String(j.value("target", "END").c_str());
		t.fStart = j.value("start", 0.f);
		t.fEnd = j.value("end", 0.f);
		t.fDuration = j.value("duration", 0.f);
		t.eEase = UI_EASE_From_String(j.value("ease", "LINEAR").c_str());
		t.eLoop = UI_TWEEN_LOOP_From_String(j.value("loop", "NONE").c_str());
	}

	// --------------------- Editor 노드 ---------------------

	json To_Json(const UISEQ_ANIMATION_NODE& a)
	{
		json j;
		j["name"] = WtoS(a.strName);
		for (const auto& t : a.vTracks) j["tracks"].push_back(To_Json(t));
		return j;
	}
	void From_Json(const json& j, UISEQ_ANIMATION_NODE& a)
	{
		a.strName = StoW(j.value("name", _string{}));
		a.vTracks.clear();
		for (const auto& jt : j.value("tracks", json::array()))
		{
			CUITween::UITWEEN_DESC t{};
			From_Json(jt, t);
			a.vTracks.push_back(t);
		}
	}

	json To_Json(const UISEQ_STEP_NODE& s)
	{
		json j;
		j["kind"] = EnumToStr(s.eKind);
		j["joinPrev"] = s.bJoinPrev;
		switch (s.eKind)
		{
		case UI_SEQ_STEP_KIND::PLAY_ANIM:
			j["targetId"] = s.strTargetId;
			j["animName"] = WtoS(s.strAnimName);
			break;
		case UI_SEQ_STEP_KIND::SET_VISIBLE:
			j["targetId"] = s.strTargetId;
			j["visible"] = s.bVisible;
			break;
		case UI_SEQ_STEP_KIND::WAIT:
			j["waitSec"] = s.fWaitSec;
			break;
		case UI_SEQ_STEP_KIND::USE_CALLBACK:
			j["callbackId"] = s.strCallbackId;
			break;
		case UI_SEQ_STEP_KIND::EFFECT_PLAY:
			j["slotId"] = s.strSlotId;
			if (!s.strTargetId.empty())
				j["targetId"] = s.strTargetId;
			if (s.bRequired)
				j["required"] = s.bRequired;
			break;

		case UI_SEQ_STEP_KIND::EFFECT_STOP:
			j["slotId"] = s.strSlotId;
			if (s.bRequired)
				j["required"] = s.bRequired;
			break;

		case UI_SEQ_STEP_KIND::BGM_PLAY:
		case UI_SEQ_STEP_KIND::BGM_STOP:
			j["slotId"] = s.strSlotId;
			if (s.bRequired)
				j["required"] = s.bRequired;
			break;

		case UI_SEQ_STEP_KIND::SFX_PLAY:
			j["slotId"] = s.strSlotId;
			if (!s.strTargetId.empty())
				j["targetId"] = s.strTargetId;
			if (s.bRequired)
				j["required"] = s.bRequired;
			break;
		case UI_SEQ_STEP_KIND::SIGNAL_FIRE:
			j["slotId"] = s.strSlotId;
			if (!s.strTargetId.empty())
				j["targetId"] = s.strTargetId;
			if (s.bRequired)
				j["required"] = s.bRequired;
			break;
		default:
			break;
		}
		return j;
	}
	void From_Json(const json& j, UISEQ_STEP_NODE& s)
	{
		s.eKind = UI_SEQ_STEP_KIND_From_String(j.value("kind", "WAIT").c_str());
		s.bJoinPrev = j.value("joinPrev", false);
		s.strTargetId = j.value("targetId", _string{});
		s.strSlotId = j.value("slotId", _string{});
		s.strAnimName = StoW(j.value("animName", _string{}));
		s.fWaitSec = j.value("waitSec", 0.f);
		s.bVisible = j.value("visible", true);
		s.strCallbackId = j.value("callbackId", _string{});
		s.bRequired = j.value("required", false);
	}

	json To_Json(const UISEQ_WIDGET_NODE& w)
	{
		json j;
		j["id"] = w.strId;
		j["displayName"] = w.strDisplayName;
		j["type"] = EnumToStr(w.Get_Type());

		if (INVALID_TAG != w.strPrototypeTag)
			j["prototypeTag"] = TagToS(w.strPrototypeTag);

		std::visit([&](const auto& desc) { j["desc"] = To_Json(desc); }, w.tDesc);

		for (const auto& anim : w.vAnimations) j["animations"].push_back(To_Json(anim));
		return j;
	}
	void From_Json(const json& j, UISEQ_WIDGET_NODE& w)
	{
		w.strId = j.value("id", _string{});
		w.strDisplayName = j.value("displayName", _string{});
		w.strPrototypeTag = SToTag(j.value("prototypeTag", _string{}));

		const UI_TYPE eType = UI_TYPE_From_String(j.value("type", "IMAGE").c_str());
		const auto& jd = j.value("desc", json::object());

		switch (eType)
		{
		case UI_TYPE::IMAGE: { CUIImage::UIIMAGE_DESC d{};              From_Json(jd, d); w.tDesc = d; break; }
		case UI_TYPE::BUTTON: { CUIButton::UIBUTTON_DESC d{};            From_Json(jd, d); w.tDesc = d; break; }
		case UI_TYPE::PROGRESSBAR: { CUIProgressBar::UIPROGRESSBAR_DESC d{};  From_Json(jd, d); w.tDesc = d; break; }
		case UI_TYPE::TEXT: { CUIText::UITEXT_DESC d{};                From_Json(jd, d); w.tDesc = d; break; }
		case UI_TYPE::CONTAINER: { CUIContainer::UICONTAINER_DESC d{};      From_Json(jd, d); w.tDesc = d; break; }
		default: { CUIImage::UIIMAGE_DESC d{};              w.tDesc = d; break; }
		}

		w.vAnimations.clear();
		for (const auto& ja : j.value("animations", json::array()))
		{
			UISEQ_ANIMATION_NODE a{};
			From_Json(ja, a);
			w.vAnimations.push_back(a);
		}
	}

	const UISEQ_WIDGET_NODE* Find_Widget_ById(const UISEQ_DOC& tDoc, const _string& strId)
	{
		if (strId.empty())
			return nullptr;

		for (const auto& widget : tDoc.vWidgets)
		{
			if (widget.strId == strId)
				return &widget;
		}

		return nullptr;
	}

	_bool Has_Animation(const UISEQ_WIDGET_NODE& tWidget, const _wstring& strAnimName)
	{
		if (strAnimName.empty())
			return false;

		for (const auto& animation : tWidget.vAnimations)
		{
			if (animation.strName == strAnimName)
				return true;
		}

		return false;
	}

	void Sanitize_DocReferences(UISEQ_DOC& tDoc)
	{
		for (size_t i = 0; i < tDoc.vSteps.size(); ++i)
		{
			auto& step = tDoc.vSteps[i];

			if (0 == i)
				step.bJoinPrev = false;

			switch (step.eKind)
			{
			case UI_SEQ_STEP_KIND::PLAY_ANIM:
			{
				const UISEQ_WIDGET_NODE* pTargetWidget = Find_Widget_ById(tDoc, step.strTargetId);
				if (nullptr == pTargetWidget)
				{
					step.strTargetId.clear();
					step.strAnimName.clear();
					break;
				}

				if (!Has_Animation(*pTargetWidget, step.strAnimName))
					step.strAnimName.clear();
				break;
			}

			case UI_SEQ_STEP_KIND::SET_VISIBLE:
				if (nullptr == Find_Widget_ById(tDoc, step.strTargetId))
					step.strTargetId.clear();
				break;

			case UI_SEQ_STEP_KIND::EFFECT_PLAY:
			case UI_SEQ_STEP_KIND::SFX_PLAY:
			case UI_SEQ_STEP_KIND::SIGNAL_FIRE:
				if (!step.strTargetId.empty() && nullptr == Find_Widget_ById(tDoc, step.strTargetId))
					step.strTargetId.clear();
				break;

			case UI_SEQ_STEP_KIND::EFFECT_STOP:
			case UI_SEQ_STEP_KIND::BGM_PLAY:
			case UI_SEQ_STEP_KIND::BGM_STOP:
				if (!step.strTargetId.empty())
					step.strTargetId.clear();
				break;

			default:
				break;
			}
		}
	}
}

HRESULT CEditor_Serializer::Save_Map(const _string& strPath, CEditInstance* pEditInstance)
{
	json root;
	root["metadata"] = { {"mapName", "Map"}, {"version", 1} };

	vector<OBJ_RECORD> Records = pEditInstance->Get_Records();

	for (auto& record : Records)
	{
		json entry;
		entry["name"] = WtoS(record.pObj->Get_Name());
		entry["protoLevel"] = record.iProtoLevel;
		entry["protoTag"] = record.strProtoTag;
		entry["layerLevel"] = record.iLayerLevel;
		entry["layerTag"] = record.strLayerTag;
		entry["transform"] = Serialize_Transform(record.pObj->Get_Transform());
		root["objects"].push_back(entry);
	}

	ofstream ofs(strPath);
	if (!ofs.is_open()) return E_FAIL;
	ofs << root.dump(2);
	return S_OK;
}

HRESULT CEditor_Serializer::Load_Map(const _string& strPath, CEditInstance* pEditInstance)
{
	ifstream ifs(strPath);
	if (!ifs.is_open()) return E_FAIL;

	json root = json::parse(ifs, nullptr, false);
	if (root.is_discarded()) return E_FAIL;

	vector<CGameObject*> vEditorObjects = pEditInstance->Get_EditorObjects();

	for (auto& entry : root["objects"])
	{
		_uint protoLevel = entry["protoLevel"];
		_uint layerLevel = entry["layerLevel"];
		WNameID protoTag = entry["protoTag"];
		WNameID layerTag = entry["layerTag"];

		size_t iNumObjects = vEditorObjects.size();
		pEditInstance->Register_Object(protoLevel, protoTag, layerLevel, layerTag, nullptr);

		// 마지막 등록된 오브젝트의 이름·Transform 복원
		auto& objs = pEditInstance->Get_EditorObjects();
		if (objs.size() <= iNumObjects)	// 등록 실패
			continue;

		CGameObject* pObj = objs.back();
		pObj->Set_Name(StoW(entry["name"].get<_string>()));
		Deserialize_Transform(pObj->Get_Transform(), entry["transform"]);
	}

	return S_OK;
}

HRESULT CEditor_Serializer::Save_UISequence(const _string& strPath, const UISEQ_DOC& tDoc)
{
	UISEQ_DOC tSanitized = tDoc;
	Helper::Sanitize_DocReferences(tSanitized);

	json root;
	root["meta"] =
	{
			{ "version", 2 },
			{ "name", tSanitized.strName },
			{ "designWidth", tSanitized.fDesignWidth },
			{ "designHeight", tSanitized.fDesignHeight },
			{ "scalePolicy", Engine::To_String(tSanitized.eScalePolicy) },
	};

	for (const auto& w : tSanitized.vWidgets)
		root["widgets"].push_back(Helper::To_Json(w));

	for (const auto& s : tSanitized.vSteps)
		root["timeline"].push_back(Helper::To_Json(s));

	ofstream ofs(strPath);
	if (!ofs.is_open())
		return E_FAIL;

	ofs << root.dump(2);
	return S_OK;
}

HRESULT CEditor_Serializer::Load_UISequence(const _string& strPath, UISEQ_DOC& tDoc)
{
	ifstream ifs(strPath);
	if (!ifs.is_open())
		return E_FAIL;

	json root = json::parse(ifs, nullptr, /*allow_exceptions=*/false);
	if (root.is_discarded())
		return E_FAIL;

	UISEQ_DOC tLocal{};

	const auto meta = root.value("meta", json::object());
	const _int iVersion = meta.value("version", 1);

	if (iVersion != 1 && iVersion != 2)
		return E_FAIL;

	tLocal.iVersion = (iVersion >= 2) ? 2 : 1;
	tLocal.strName = meta.value("name", _string{});

	if (iVersion >= 2)
	{
		tLocal.fDesignWidth = meta.value("designWidth", 1920.f);
		tLocal.fDesignHeight = meta.value("designHeight", 1080.f);
		tLocal.eScalePolicy = UI_SCALE_POLICY_From_String(
			meta.value("scalePolicy", _string("UNIFORM_FIT")).c_str());

		if (tLocal.eScalePolicy == UI_SCALE_POLICY::END)
			tLocal.eScalePolicy = UI_SCALE_POLICY::UNIFORM_FIT;
	}
	else
	{
		tLocal.fDesignWidth = Helper::kLegacyDesignWidth;
		tLocal.fDesignHeight = Helper::kLegacyDesignHeight;
		tLocal.eScalePolicy = UI_SCALE_POLICY::UNIFORM_FIT;
	}

	for (const auto& jw : root.value("widgets", json::array()))
	{
		UISEQ_WIDGET_NODE w{};
		Helper::From_Json(jw, w);

		auto& tBase = Get_BaseDesc(w);
		tBase.tCanvasDesc.fDesignWidth = tLocal.fDesignWidth;
		tBase.tCanvasDesc.fDesignHeight = tLocal.fDesignHeight;
		tBase.tCanvasDesc.eScalePolicy = tLocal.eScalePolicy;

		if (iVersion < 2)
		{
			tBase.fPivotX = 0.5f;
			tBase.fPivotY = 0.5f;
		}

		tLocal.vWidgets.push_back(std::move(w));
	}

	for (const auto& js : root.value("timeline", json::array()))
	{
		UISEQ_STEP_NODE s{};
		Helper::From_Json(js, s);
		tLocal.vSteps.push_back(std::move(s));
	}

	Helper::Sanitize_DocReferences(tLocal);

	tDoc = std::move(tLocal);
	return S_OK;
}

HRESULT CEditor_Serializer::Save_EffectPreset(const _string& strPath, const vector<struct EFFECT_PRESET>& Presets)
{
	return E_NOTIMPL;
}

HRESULT CEditor_Serializer::Load_EffectPreset(const _string& strPath, vector<struct EFFECT_PRESET>& Presets)
{
	return E_NOTIMPL;
}

json CEditor_Serializer::Serialize_Transform(CTransform* pTransformCom)
{
	float pos[3], rot[3], scale[3];
	ImGuizmo::DecomposeMatrixToComponents(
		reinterpret_cast<const _float*>(pTransformCom->Get_WorldMatrixPtr()), pos, rot, scale);

	return
	{
		{"pos",   {pos[0],		pos[1],		pos[2]}},
		{"rot",   {rot[0],		rot[1],		rot[2]}},
		{"scale", {scale[0],	scale[1],	scale[2]}}
	};
}

void CEditor_Serializer::Deserialize_Transform(CTransform* pTransformCom, const json& j)
{
	float pos[3] = { j["pos"][0],   j["pos"][1],   j["pos"][2] };
	float rot[3] = { j["rot"][0],   j["rot"][1],   j["rot"][2] };
	float scale[3] = { j["scale"][0], j["scale"][1], j["scale"][2] };

	Set_WorldMatrix(pTransformCom, pos, rot, scale);

	//float mat[16];
	//ImGuizmo::RecomposeMatrixFromComponents(pos, rot, scale, mat);

	//XMFLOAT4X4 m;
	//memcpy(&m, mat, sizeof(float) * 16);
	//pTransformCom->Set_State(STATE::RIGHT, XMLoadFloat4(reinterpret_cast<XMFLOAT4*>(&m._11)));
	//pTransformCom->Set_State(STATE::UP, XMLoadFloat4(reinterpret_cast<XMFLOAT4*>(&m._21)));
	//pTransformCom->Set_State(STATE::LOOK, XMLoadFloat4(reinterpret_cast<XMFLOAT4*>(&m._31)));
	//pTransformCom->Set_State(STATE::POSITION, XMLoadFloat4(reinterpret_cast<XMFLOAT4*>(&m._41)));
}
