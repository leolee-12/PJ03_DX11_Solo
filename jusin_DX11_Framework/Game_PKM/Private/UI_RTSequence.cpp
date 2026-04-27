#include "UI_RTSequence.h"

#include "SharedTexture_Manager.h"

#include "UISequence.h"
#include "UIImage.h"
#include "UIText.h"
#include "UIButton.h"
#include "UIProgressBar.h"
#include "UITween.h"
#include "UIAnimator.h"
#include "GameInstance.h"

namespace
{
#ifdef _DEBUG
	inline void Log_Loader(const char* fmt, ...)
	{
		char szBuf[512];
		va_list ap;
		va_start(ap, fmt);
		vsnprintf_s(szBuf, _TRUNCATE, fmt, ap);
		va_end(ap);
		OutputDebugStringA("[UI_RTSequence] ");
		OutputDebugStringA(szBuf);
		OutputDebugStringA("\n");
	}
#else
	inline void Log_Loader(const char*, ...) {}
#endif

	constexpr const _char* kDefaultShader = "Prototype_Component_Shader_UI";
	constexpr const _char* kDefaultVIBuffer = "Prototype_Component_VIBuffer_Rect";

	using DescVariant = std::variant<
		CUIContainer::UICONTAINER_DESC,
		CUIImage::UIIMAGE_DESC,
		CUIText::UITEXT_DESC,
		CUIButton::UIBUTTON_DESC,
		CUIProgressBar::UIPROGRESSBAR_DESC>;

	struct LoaderAnim
	{
		_wstring strName;
		vector<CUITween::UITWEEN_DESC> vTracks;
	};

	struct LoaderWidget
	{
		_string strId;
		UI_TYPE eType{ UI_TYPE::END };
		DescVariant tDesc;
		vector<LoaderAnim> vAnimations;
	};

	struct LoaderStep
	{
		UI_SEQ_STEP_KIND eKind{ UI_SEQ_STEP_KIND::WAIT };
		_string strTargetId;
		_wstring strAnimName;
		_float fWaitSec{ 0.f };
		_bool bVisible{ true };
		_string strCallbackId;
		_bool bJoinPrev{ false };
	};

	// 함수 전방 선언
	HRESULT Parse_File(const _string& strPath, json& jOut);

	HRESULT Build_ContainerDesc(const json& jDesc, CUIContainer::UICONTAINER_DESC& tOut);
	HRESULT Build_ImageDesc(const json& jDesc, CUIImage::UIIMAGE_DESC& tOut);
	HRESULT Build_TextDesc(const json& jDesc, CUIText::UITEXT_DESC& tOut);
	HRESULT Build_ButtonDesc(const json& jDesc, CUIButton::UIBUTTON_DESC& tOut);
	HRESULT Build_ProgressDesc(const json& jDesc, CUIProgressBar::UIPROGRESSBAR_DESC& tOut);

	void    Build_BaseDesc(const json& jDesc, CUIObject::UIOBJECT_DESC& tBase);
	HRESULT Build_TweenDesc(const json& jTween, CUITween::UITWEEN_DESC& tOut);

	HRESULT Build_LoaderWidget(const json& jw, LoaderWidget& wOut);
	HRESULT Build_LoaderAnim(const json& ja, LoaderAnim& aOut);
	HRESULT Build_LoaderStep(const json& js, LoaderStep& sOut);

	// enum/string
	UI_TYPE				StrToUIType(const _string& s)		{ return detail::Enum_From_String(s.c_str(), detail::kUIType, UI_TYPE::END); }
	UI_LAYOUT			StrToUILayout(const _string& s)		{ return detail::Enum_From_String(s.c_str(), detail::kUILayout, UI_LAYOUT::END); }
	UI_PROGRESS_DIR		StrToProgressDir(const _string& s)	{ return detail::Enum_From_String(s.c_str(), detail::kProgressDir, UI_PROGRESS_DIR::END); }
	UI_TWEEN_TARGET		StrToTweenTarget(const _string& s)	{ return detail::Enum_From_String(s.c_str(), detail::kTweenTarget, UI_TWEEN_TARGET::END); }
	UI_EASE				StrToEase(const _string& s)			{ return detail::Enum_From_String(s.c_str(), detail::kEase, UI_EASE::END); }
	UI_TWEEN_LOOP		StrToLoop(const _string& s)			{ return detail::Enum_From_String(s.c_str(), detail::kTweenLoop, UI_TWEEN_LOOP::END); }
	UI_SEQ_STEP_KIND	StrToStepKind(const _string& s)		{ return detail::Enum_From_String(s.c_str(), detail::kStepKind, UI_SEQ_STEP_KIND::END); }
	UI_ANCHOR			StrToAnchor(const _string& s)		{ return detail::Enum_From_String(s.c_str(), detail::kAnchor, UI_ANCHOR::END); }
	UI_TEXT_ALIGN		StrToTextAlign(const _string& s)	{ return detail::Enum_From_String(s.c_str(), detail::kTextAlign, UI_TEXT_ALIGN::END); }

	// ── tag 변환 (editor SToTag와 동일 정책) ──
	inline WNameID SToTag(const _string& s)
	{
		if (s.empty()) return INVALID_TAG;
		if (s[0] == '-' || (s[0] >= '0' && s[0] <= '9'))
		{
			try { return static_cast<WNameID>(std::stoul(s)); }
			catch (...) {}
		}
		return WNAME(StoW(s));
	}

	// ── 정책 B: 텍스처 미싱 시 실패 ──
	HRESULT Validate_RequiredResources(const DescVariant& tDesc, UI_TYPE eType);

	// ── prototype 매핑 ──
	WNameID Map_PrototypeTag(UI_TYPE eType)
	{
		switch (eType)
		{
		case UI_TYPE::CONTAINER:   return PROTO_UI_CONTAINER;
		case UI_TYPE::IMAGE:       return PROTO_UI_IMAGE;
		case UI_TYPE::TEXT:        return PROTO_UI_TEXT;
		case UI_TYPE::BUTTON:      return PROTO_UI_BUTTON;
		case UI_TYPE::PROGRESSBAR: return PROTO_UI_PROGRESSBAR;
		default:                   return INVALID_TAG;
		}
	}

	// ── sanitize (editor Sanitize_DocReferences와 동일 정책) ──
	void Sanitize_Steps(vector<LoaderStep>& vSteps,
		const unordered_map<_string, set<_wstring>>& mapWidgetAnims)
	{
		for (size_t i = 0; i < vSteps.size(); ++i)
		{
			auto& s = vSteps[i];
			if (i == 0) s.bJoinPrev = false;

			switch (s.eKind)
			{
			case UI_SEQ_STEP_KIND::PLAY_ANIM:
			{
				auto it = mapWidgetAnims.find(s.strTargetId);
				if (it == mapWidgetAnims.end())
				{
					s.strTargetId.clear();
					s.strAnimName.clear();
					break;
				}
				if (it->second.find(s.strAnimName) == it->second.end())
					s.strAnimName.clear();
				break;
			}
			case UI_SEQ_STEP_KIND::SET_VISIBLE:
				if (mapWidgetAnims.find(s.strTargetId) == mapWidgetAnims.end())
					s.strTargetId.clear();
				break;
			default:
				break;
			}
		}
	}
}

CUI_RTSequence::CUI_RTSequence(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }
	, m_pContext{ pContext }
	, m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pDevice);
}

HRESULT CUI_RTSequence::Initialize(const _string& strPath, _uint iLevel, WNameID strLayerTag)
{
	m_iLevel = iLevel;
	m_strLayerTag = strLayerTag;

	// 1) JSON 파싱 + version 검사
	json root;
	if (FAILED(Parse_File(strPath, root)))
		return E_FAIL;

	// 2) 임시 수집 (UISEQ_DOC 거치지 않음)
	vector<LoaderWidget> vLoadedWidgets;
	vector<LoaderStep>   vLoadedSteps;

	for (const auto& jw : root.value("widgets", json::array()))
	{
		LoaderWidget w{};
		if (FAILED(Build_LoaderWidget(jw, w))) return E_FAIL;
		
		if (FAILED(Validate_RequiredResources(w.tDesc, w.eType)))
		{
			Log_Loader("missing required resource (id=%s, type=%s)",
				w.strId.c_str(), To_String(w.eType));
			return E_FAIL;
		}

		vLoadedWidgets.push_back(std::move(w));
	}

	for (const auto& js : root.value("timeline", json::array()))
	{
		LoaderStep s{};
		if (FAILED(Build_LoaderStep(js, s))) return E_FAIL;
		vLoadedSteps.push_back(std::move(s));
	}

	// 3) sanitize용 (id -> animation name set) map
	unordered_map<_string, set<_wstring>> mapAnims;
	for (const auto& w : vLoadedWidgets)
	{
		auto& s = mapAnims[w.strId];
		for (const auto& a : w.vAnimations)
			if (!a.strName.empty()) s.insert(a.strName);
	}
	Sanitize_Steps(vLoadedSteps, mapAnims);

	// 4) iZOrder 오름차순 stable_sort
	std::stable_sort(vLoadedWidgets.begin(), vLoadedWidgets.end(),
		[](const LoaderWidget& a, const LoaderWidget& b)
		{
			const _int za = std::visit([](const auto& d) { return static_cast<const
				CUIObject::UIOBJECT_DESC&>(d).iZOrder; }, a.tDesc);
			const _int zb = std::visit([](const auto& d) { return static_cast<const
				CUIObject::UIOBJECT_DESC&>(d).iZOrder; }, b.tDesc);
			return za < zb;
		});

	// 5) 임시 clone (아직 layer 등록 X) + animator 등록
	vector<CUIObject*>                       vTmp;
	unordered_map<_string, CUIObject*>       mapTmpById;
	vTmp.reserve(vLoadedWidgets.size());

	auto Cleanup_Tmp = [&]()
		{
			for (auto* p : vTmp) Safe_Release(p);
			vTmp.clear();
			mapTmpById.clear();
		};

	for (auto& w : vLoadedWidgets)
	{
		const WNameID strProto = Map_PrototypeTag(w.eType);
		if (strProto == INVALID_TAG) { Cleanup_Tmp(); return E_FAIL; }

		void* pArg = std::visit([](auto& d) -> void* { return static_cast<void*>(&d); }, w.tDesc);

		CGameObject* pClone = static_cast<CGameObject*>(
			m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, m_iLevel, strProto, pArg));
		if (nullptr == pClone)
		{
			Log_Loader("Clone_Prototype failed (id=%s)", w.strId.c_str());
			Cleanup_Tmp(); return E_FAIL;
		}

		CUIObject* pUI = static_cast<CUIObject*>(pClone);
		vTmp.push_back(pUI);
		mapTmpById[w.strId] = pUI;

		if (CUIAnimator* pAnim = pUI->Get_Animator())
		{
			pAnim->Clear_Animations();
			for (const auto& a : w.vAnimations)
			{
				if (a.strName.empty()) continue;
				pAnim->Register_Animation(a.strName, a.vTracks);
			}
		}
	}

	// 6) sequence 생성
	CUISequence* pSeq = CUISequence::Create(m_pDevice, m_pContext);
	if (nullptr == pSeq) { Cleanup_Tmp(); return E_FAIL; }
	if (FAILED(pSeq->Initialize(nullptr))) { Safe_Release(pSeq); Cleanup_Tmp(); return E_FAIL; }
	pSeq->Clear_Timeline();

	// 7) timeline step 빌드
	for (const auto& sn : vLoadedSteps)
	{
		const _bool bNeedTarget = (sn.eKind == UI_SEQ_STEP_KIND::PLAY_ANIM ||
			sn.eKind == UI_SEQ_STEP_KIND::SET_VISIBLE);

		if (bNeedTarget && sn.strTargetId.empty())
			continue;
		if (sn.eKind == UI_SEQ_STEP_KIND::PLAY_ANIM && sn.strAnimName.empty())
			continue;

		CUISequence::UISEQ_STEP s{};
		s.eKind = sn.eKind;
		s.strAnimName = sn.strAnimName;
		s.fWaitSec = sn.fWaitSec;
		s.bVisible = sn.bVisible;
		s.bJoinPrev = sn.bJoinPrev;
		s.pTarget = nullptr;

		if (bNeedTarget)
		{
			auto it = mapTmpById.find(sn.strTargetId);
			if (it == mapTmpById.end()) continue;
			s.pTarget = it->second;
		}

		if (sn.eKind == UI_SEQ_STEP_KIND::USE_CALLBACK && !sn.strCallbackId.empty())
		{
			const _string strId = sn.strCallbackId;
			s.fnCallback = [strId]() { Log_Loader("callback stub fired: %s", strId.c_str()); };
		}

		if (!(sn.bJoinPrev ? pSeq->Join(s) : pSeq->Append(s)))
		{
			Log_Loader("Append/Join failed (kind=%d, target=%s)",
				(int)sn.eKind, sn.strTargetId.c_str());
			Cleanup_Tmp();
			Safe_Release(pSeq);
			return E_FAIL;
		}
	}

	// 8) 모든 검증 통과 — 이제 layer 등록 (§8.8 정책 1: all-or-nothing)
	for (size_t i = 0; i < vTmp.size(); ++i)
	{
		if (FAILED(m_pGameInstance->Add_GameObject_Ex(m_iLevel, m_strLayerTag, vTmp[i])))
		{
			// 이미 등록된 [0..i)는 layer가 owner — Set_Dead 또는 layer가 정리
			// 미등록 [i..end)는 loader가 release
			Log_Loader("Add_GameObject_Ex failed at index %zu", i);
			for (size_t k = i; k < vTmp.size(); ++k) Safe_Release(vTmp[k]);
			vTmp.clear();
			mapTmpById.clear();
			Safe_Release(pSeq);
			return E_FAIL;
		}
	}

	// 9) commit
	m_vWidgets = std::move(vTmp);     // weak view (Safe_AddRef 안 함)
	m_mapById = std::move(mapTmpById);
	m_pSequence = pSeq;
	m_bPlaying = false;               // §8.6: 명시적 Play 대기

	return S_OK;
}

void CUI_RTSequence::Update(_float fTimeDelta)
{
	if (m_bPlaying && m_pSequence)
		m_pSequence->Update(fTimeDelta);
}

void CUI_RTSequence::Play()
{
	if (m_pSequence)
	{
		m_pSequence->Play();
		m_bPlaying = true;
	}
}

void CUI_RTSequence::Stop()
{
	if (m_pSequence) m_pSequence->Stop();
	m_bPlaying = false;
}

CUIObject* CUI_RTSequence::Find_Widget(const _string& strId) const
{
	auto it = m_mapById.find(strId);
	return (it == m_mapById.end()) ? nullptr : it->second;
}

CUI_RTSequence* CUI_RTSequence::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext,
	const _string& strPath, _uint iLevel, WNameID strLayerTag)
{
	CUI_RTSequence* pInstance = new CUI_RTSequence(pDevice, pContext);

	if (FAILED(pInstance->Initialize(strPath, iLevel, strLayerTag)))
	{
		MSG_BOX("Failed to Create : CUI_RTSequence");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_RTSequence::Free()
{
	__super::Free();
	Stop();
	
	// weak view: release 책임 없음 (layer가 owner)
	m_vWidgets.clear();
	m_mapById.clear();

	Safe_Release(m_pSequence);
	Safe_Release(m_pGameInstance);
	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);
}

namespace
{
		// 파일 읽고 parse, version 검사
	HRESULT Parse_File(const _string& strPath, json& jOut)
	{
		std::ifstream ifs(strPath);
		if (!ifs.is_open())
		{
			Log_Loader("failed to open: %s", strPath.c_str());
			return E_FAIL;
		}

		jOut = json::parse(ifs, nullptr, false);
		if (jOut.is_discarded())
		{
			Log_Loader("JSON parse failed: %s", strPath.c_str());
			return E_FAIL;
		}

		const auto meta = jOut.value("meta", json::object());
		const _int iVersion = meta.value("version", 0);

		if (iVersion != 1)
		{
			Log_Loader("version mismatch: expected 1, got %d (%s)", iVersion, strPath.c_str());
			return E_FAIL;
		}

		return S_OK;
	}

	void Build_BaseDesc(const json& j, CUIObject::UIOBJECT_DESC& d)
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
		}
		if (j.contains("anchor"))
		{
			const auto& a = j["anchor"];
			d.tAnchorDesc.eAnchor = StrToAnchor(a.value("kind", _string("MC")));
			d.tAnchorDesc.fOffsetX = a.value("offsetX", 0.f);
			d.tAnchorDesc.fOffsetY = a.value("offsetY", 0.f);
			d.tAnchorDesc.bUseAnchoredPos = a.value("useAnchoredPos", false);
		}
		if (j.contains("layoutSlot"))
		{
			const auto& l = j["layoutSlot"];
			auto m = l.value("margin", json::array({ 0.f,0.f,0.f,0.f }));
			d.tLayoutSlot.vMargin = { m[0], m[1], m[2], m[3] };
			d.tLayoutSlot.fDesiredSizeX = l.value("desiredX", 0.f);
			d.tLayoutSlot.fDesiredSizeY = l.value("desiredY", 0.f);
		}
		d.pParentUI = nullptr;
	}

	HRESULT Build_ImageDesc(const json& j, CUIImage::UIIMAGE_DESC& d)
	{
		Build_BaseDesc(j, d);
		const auto sh = j.value("shader", json::object());
		const auto vb = j.value("viBuffer", json::object());
		const auto tx = j.value("texture", json::object());

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

		// ── Sprite Animation ──
		const auto jSprite = j.value("spriteAnim", json::object());
		d.bSpriteAnimEnabled = jSprite.value("enabled", false);
		d.fSpriteFrameDuration = jSprite.value("frameDuration", 0.f);

		if (d.bSpriteAnimEnabled && d.fSpriteFrameDuration <= 0.f)
		{
			Log_Loader("spriteAnim: enabled but frameDuration<=0 — disabling");
			d.bSpriteAnimEnabled = false;
			d.fSpriteFrameDuration = 0.f;
		}

		// ── Shared Textures ──
		d.SharedTextureBindings.clear();
		for (const auto& jb : j.value("sharedTextures", json::array()))
		{
			UI_SHARED_TEXTURE_BINDING_DESC b{};
			b.strSharedTexName = jb.value("group", _string{});
			b.strShaderVarName = jb.value("shaderVar", _string{});
			b.iTextureIndex = jb.value("index", static_cast<unsigned int>(-1));

			// 결손 entry — 묵음 skip(편집 중간 상태 보호)
			if (b.strSharedTexName.empty()
				|| b.strShaderVarName.empty()
				|| b.iTextureIndex == static_cast<unsigned int>(-1))
			{
				Log_Loader("sharedTextures: incomplete entry skipped (group='%s', shaderVar='%s', index = % u)",
					b.strSharedTexName.c_str(),
					b.strShaderVarName.c_str(),
					b.iTextureIndex);
					continue;
			}

			// group 이름 검증 — 단일 진실원천(SHARED_TEXTURE_From_String) 사용
			if (SHARED_TEXTURE_TYPE::END == SHARED_TEXTURE_From_String(b.strSharedTexName.c_str()))
			{
				Log_Loader("sharedTextures: unknown group '%s' (skipped)",
					b.strSharedTexName.c_str());
				continue;
			}

			d.SharedTextureBindings.push_back(std::move(b));
		}

		return S_OK;
	}

	HRESULT Build_ButtonDesc(const json& j, CUIButton::UIBUTTON_DESC& d)
	{
		Build_BaseDesc(j, d);
		const auto sh = j.value("shader", json::object());
		const auto vb = j.value("viBuffer", json::object());
		const auto tx = j.value("texture", json::object());
		const auto bi = j.value("buttonIndices", json::object());

		d.strShaderTag = SToTag(sh.value("tag", _string(kDefaultShader)));
		d.iShaderLevel = sh.value("level", 0u);
		d.strVIBufferTag = SToTag(vb.value("tag", _string(kDefaultVIBuffer)));
		d.iVIBufferLevel = vb.value("level", 0u);
		d.strTextureTag = SToTag(tx.value("tag", _string{}));
		d.iTextureLevel = tx.value("level", 0u);

		d.iNormalTextureIndex = bi.value("normal", 0u);
		d.iHoverTextureIndex = bi.value("hover", 0u);
		d.iPressedTextureIndex = bi.value("pressed", 0u);
		d.iDisabledTextureIndex = bi.value("disabled", static_cast<_uint>(-1));
		d.bInteractable = j.value("interactable", true);

		auto c = j.value("color", json::array({ 1.f,1.f,1.f,1.f }));
		d.vColor = { c[0], c[1], c[2], c[3] };
		return S_OK;
	}

	HRESULT Build_ProgressDesc(const json& j, CUIProgressBar::UIPROGRESSBAR_DESC& d)
	{
		Build_BaseDesc(j, d);
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
		d.eDirection = StrToProgressDir(j.value("direction", _string("LEFT_TO_RIGHT")));
		return S_OK;
	}

	HRESULT Build_TextDesc(const json& j, CUIText::UITEXT_DESC& d)
	{
		Build_BaseDesc(j, d);
		d.strText = StoW(j.value("text", _string{}));
		d.strFontTag = SToTag(j.value("font", _string{}));
		d.eAlign = StrToTextAlign(j.value("align", _string("LEFT")));

		auto c = j.value("color", json::array({ 1.f,1.f,1.f,1.f }));
		d.vColor = { c[0], c[1], c[2], c[3] };
		return S_OK;
	}

	HRESULT Build_ContainerDesc(const json& j, CUIContainer::UICONTAINER_DESC& d)
	{
		Build_BaseDesc(j, d);
		const auto lo = j.value("layout", json::object());
		d.tLayoutDesc.eLayout = StrToUILayout(lo.value("kind", _string("VERTICAL")));
		d.tLayoutDesc.fPadding = lo.value("padding", 0.f);
		d.tLayoutDesc.fSpacing = lo.value("spacing", 0.f);
		return S_OK;
	}

	HRESULT Build_TweenDesc(const json& j, CUITween::UITWEEN_DESC& t)
	{
		t.eTarget = StrToTweenTarget(j.value("target", _string("END")));
		if (t.eTarget == UI_TWEEN_TARGET::END) return E_FAIL;
		t.fStart = j.value("start", 0.f);
		t.fEnd = j.value("end", 0.f);
		t.fDuration = j.value("duration", 0.f);
		t.fDelay = 0.f;  // 현재 schema 미저장
		t.eEase = StrToEase(j.value("ease", _string("LINEAR")));
		t.eLoop = StrToLoop(j.value("loop", _string("NONE")));
		return S_OK;
	}

	HRESULT Build_LoaderAnim(const json& j, LoaderAnim& a)
	{
		a.strName = StoW(j.value("name", _string{}));
		a.vTracks.clear();
		for (const auto& jt : j.value("tracks", json::array()))
		{
			CUITween::UITWEEN_DESC t{};
			if (FAILED(Build_TweenDesc(jt, t))) return E_FAIL;
			a.vTracks.push_back(t);
		}
		return S_OK;
	}

	HRESULT Build_LoaderStep(const json& j, LoaderStep& s)
	{
		s.eKind = StrToStepKind(j.value("kind", _string("WAIT")));
		if (s.eKind == UI_SEQ_STEP_KIND::END) return E_FAIL;
		s.bJoinPrev = j.value("joinPrev", false);
		s.strTargetId = j.value("targetId", _string{});
		s.strAnimName = StoW(j.value("animName", _string{}));
		s.fWaitSec = j.value("waitSec", 0.f);
		s.bVisible = j.value("visible", true);
		s.strCallbackId = j.value("callbackId", _string{});
		return S_OK;
	}

	HRESULT Build_LoaderWidget(const json& j, LoaderWidget& w)
	{
		w.strId = j.value("id", _string{});
		const _string strType = j.value("type", _string{});
		w.eType = StrToUIType(strType);
		if (w.eType == UI_TYPE::END || w.eType == UI_TYPE::WIDGET)
		{
			Log_Loader("unknown widget type '%s' (id=%s)", strType.c_str(), w.strId.c_str());
			return E_FAIL;
		}

		const auto& jd = j.value("desc", json::object());
		switch (w.eType)
		{
		case UI_TYPE::CONTAINER:
		{
			CUIContainer::UICONTAINER_DESC d{};
			if (FAILED(Build_ContainerDesc(jd, d))) return E_FAIL;
			w.tDesc = d; break;
		}
		case UI_TYPE::IMAGE:
		{
			CUIImage::UIIMAGE_DESC d{};
			if (FAILED(Build_ImageDesc(jd, d))) return E_FAIL;
			w.tDesc = d; break;
		}
		case UI_TYPE::TEXT:
		{
			CUIText::UITEXT_DESC d{};
			if (FAILED(Build_TextDesc(jd, d))) return E_FAIL;
			w.tDesc = d; break;
		}
		case UI_TYPE::BUTTON:
		{
			CUIButton::UIBUTTON_DESC d{};
			if (FAILED(Build_ButtonDesc(jd, d))) return E_FAIL;
			w.tDesc = d; break;
		}
		case UI_TYPE::PROGRESSBAR:
		{
			CUIProgressBar::UIPROGRESSBAR_DESC d{};
			if (FAILED(Build_ProgressDesc(jd, d))) return E_FAIL;
			w.tDesc = d; break;
		}
		default: return E_FAIL;
		}

		for (const auto& ja : j.value("animations", json::array()))
		{
			LoaderAnim a{};
			if (FAILED(Build_LoaderAnim(ja, a))) return E_FAIL;
			w.vAnimations.push_back(std::move(a));
		}
		return S_OK;
	}

	// ── 정책 B: 텍스처 미싱 시 실패 ──
	HRESULT Validate_RequiredResources(const DescVariant& tDesc, UI_TYPE eType)
	{
		return std::visit([eType](const auto& d) -> HRESULT
			{
				using T = std::decay_t<decltype(d)>;
				if constexpr (std::is_same_v<T, CUIImage::UIIMAGE_DESC>)
				{
					if (d.strTextureTag == INVALID_TAG) return E_FAIL;
				}
				else if constexpr (std::is_same_v<T, CUIButton::UIBUTTON_DESC>)
				{
					if (d.strTextureTag == INVALID_TAG) return E_FAIL;
				}
				else if constexpr (std::is_same_v<T, CUIProgressBar::UIPROGRESSBAR_DESC>)
				{
					if (d.strBackTextureTag == INVALID_TAG) return E_FAIL;
					if (d.strFillTextureTag == INVALID_TAG) return E_FAIL;
				}
				else if constexpr (std::is_same_v<T, CUIText::UITEXT_DESC>)
				{
					if (d.strFontTag == INVALID_TAG) return E_FAIL;
				}
				// CONTAINER: 검사 대상 없음
				return S_OK;
			}, tDesc);
	}
}