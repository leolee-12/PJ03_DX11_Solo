#include "UISequence.h"
#include "UIAnimator.h"
#include "UIContainer.h"
#include "UIImage.h"
#include "UIText.h"
#include "UIButton.h"
#include "UIProgressBar.h"
#include "UITween.h"
#include "GameInstance.h"

namespace
{
	constexpr _float kLegacyDesignWidth = 1280.f;
	constexpr _float kLegacyDesignHeight = 720.f;

#ifdef _DEBUG
	inline void Log_Loader(const char* fmt, ...)
	{
		char szBuf[512];
		va_list ap;
		va_start(ap, fmt);
		vsnprintf_s(szBuf, _TRUNCATE, fmt, ap);
		va_end(ap);
		OutputDebugStringA("[UISequence] ");
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
		WNameID strPrototypeTag{ INVALID_TAG };
		DescVariant tDesc;
		vector<LoaderAnim> vAnimations;
	};

	struct LoaderStep
	{
		UI_SEQ_STEP_KIND eKind{ UI_SEQ_STEP_KIND::WAIT };
		_string strTargetId;
		_string strSlotId;
		_wstring strAnimName;
		_float fWaitSec{ 0.f };
		_bool bVisible{ true };
		_string strCallbackId;
		_bool bJoinPrev{ false };
		_bool bRequired{ false };
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
	UI_TYPE				StrToUIType(const _string& s) { return detail::Enum_From_String(s.c_str(), detail::kUIType, UI_TYPE::END); }
	UI_LAYOUT			StrToUILayout(const _string& s) { return detail::Enum_From_String(s.c_str(), detail::kUILayout, UI_LAYOUT::END); }
	UI_PROGRESS_DIR		StrToProgressDir(const _string& s) { return detail::Enum_From_String(s.c_str(), detail::kProgressDir, UI_PROGRESS_DIR::END); }
	UI_TWEEN_TARGET		StrToTweenTarget(const _string& s) { return detail::Enum_From_String(s.c_str(), detail::kTweenTarget, UI_TWEEN_TARGET::END); }
	UI_EASE				StrToEase(const _string& s) { return detail::Enum_From_String(s.c_str(), detail::kEase, UI_EASE::END); }
	UI_TWEEN_LOOP		StrToLoop(const _string& s) { return detail::Enum_From_String(s.c_str(), detail::kTweenLoop, UI_TWEEN_LOOP::END); }
	UI_SEQ_STEP_KIND	StrToStepKind(const _string& s) { return detail::Enum_From_String(s.c_str(), detail::kStepKind, UI_SEQ_STEP_KIND::END); }
	UI_ANCHOR			StrToAnchor(const _string& s) { return detail::Enum_From_String(s.c_str(), detail::kAnchor, UI_ANCHOR::END); }
	UI_TEXT_ALIGN		StrToTextAlign(const _string& s) { return detail::Enum_From_String(s.c_str(), detail::kTextAlign, UI_TEXT_ALIGN::END); }
	UI_TEXT_VALIGN		StrToTextVAlign(const _string& s) { return detail::Enum_From_String(s.c_str(), detail::kTextVAlign, UI_TEXT_VALIGN::END); }

	// -- tag 변환 (editor SToTag와 동일 정책) --
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

	// -- 정책 B: 텍스처 미싱 시 실패 --
	HRESULT Validate_RequiredResources(DescVariant& tDesc, UI_TYPE eType);

	// -- prototype 매핑 --
	WNameID Map_DefaultPrototypeTag(UI_TYPE eType)
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

	WNameID Resolve_PrototypeTag(const LoaderWidget& w)
	{
		if (INVALID_TAG != w.strPrototypeTag)
			return w.strPrototypeTag;

		return Map_DefaultPrototypeTag(w.eType);
	}

	// -- sanitize (editor Sanitize_DocReferences와 동일 정책) --
	void Sanitize_Steps(vector<LoaderStep>& vSteps, const unordered_map<_string, set<_wstring>>& mapWidgetAnims)
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
			case UI_SEQ_STEP_KIND::EFFECT_PLAY:
			case UI_SEQ_STEP_KIND::SFX_PLAY:
			case UI_SEQ_STEP_KIND::SIGNAL_FIRE:
				if (!s.strTargetId.empty() && mapWidgetAnims.find(s.strTargetId) == mapWidgetAnims.end())
					s.strTargetId.clear();
				break;

			case UI_SEQ_STEP_KIND::EFFECT_STOP:
			case UI_SEQ_STEP_KIND::BGM_PLAY:
			case UI_SEQ_STEP_KIND::BGM_STOP:
				s.strTargetId.clear();
				break;
			default:
				break;
			}
		}
	}
}

CUISequence::CUISequence(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIContainer{ pDevice, pContext }
{
}

CUISequence::CUISequence(const CUISequence& Prototype)
	: CUIContainer{ Prototype }
{
}

CUIObject* CUISequence::Find_Widget(const _string& strId) const
{
	auto it = m_mapById.find(strId);
	return (it == m_mapById.end()) ? nullptr : it->second;
}

_bool CUISequence::Insert_Step(_int iIndex, const UISEQ_STEP& step)
{
	if (m_bPlaying) return false;
	if (iIndex < 0 || iIndex > static_cast<_int>(m_Steps.size())) return false;

	m_Steps.insert(m_Steps.begin() + iIndex, step);
	return true;
}

_bool CUISequence::Remove_Step(_int iIndex)
{
	if (m_bPlaying) return false;
	if (iIndex < 0 || iIndex >= static_cast<_int>(m_Steps.size())) return false;

	m_Steps.erase(m_Steps.begin() + iIndex);
	return true;
}

_bool CUISequence::Move_Step(_int iFrom, _int iTo)
{
	if (m_bPlaying) return false;

	const _int iSize = static_cast<_int>(m_Steps.size());
	if (iFrom < 0 || iFrom >= iSize) return false;
	if (iTo < 0 || iTo >= iSize) return false;
	if (iFrom == iTo) return true;

	UISEQ_STEP tMoved = m_Steps[iFrom];
	m_Steps.erase(m_Steps.begin() + iFrom);
	m_Steps.insert(m_Steps.begin() + iTo, tMoved);
	return true;
}

_bool CUISequence::Update_Step(_int iIndex, const UISEQ_STEP& step)
{
	if (m_bPlaying) return false;
	if (iIndex < 0 || iIndex >= static_cast<_int>(m_Steps.size())) return false;

	m_Steps[iIndex] = step;
	return true;
}

void CUISequence::Seek_ToStep(_int iIndex)
{
	if (iIndex < 0 || iIndex >= static_cast<_int>(m_Steps.size())) return;

	m_iCursor = iIndex;
	m_fTimer = 0.f;
	m_bStepStarted = false;
}

void CUISequence::Set_Timer(_float fTimer)
{
	m_fTimer = fTimer;
}

HRESULT CUISequence::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUISequence::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (m_fSizeX <= 0.f || m_fSizeY <= 0.f)
	{
		m_fSizeX = m_tCanvasDesc.fDesignWidth;
		m_fSizeY = m_tCanvasDesc.fDesignHeight;
		m_fCenterX = m_tCanvasDesc.fDesignWidth * 0.5f;
		m_fCenterY = m_tCanvasDesc.fDesignHeight * 0.5f;
		Refresh_Layout();
	}

	if (nullptr == pArg)	// pArg 없음 = 빈 시퀀스 (수동 Append/Join용)
		return S_OK;

	auto pDesc = static_cast<UISEQUENCE_DESC*>(pArg);
	if (pDesc->strPath.empty())
		return S_OK;		// path 미지정 = 빈 시퀀스

	return Build_FromFile(pDesc->strPath, pDesc->iProtoLevel);
}

void CUISequence::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);  // CUIContainer::Update -> CUIObject::Update -> Animator Tick

	if (!m_bPlaying)
		return;

	if (m_iCursor < 0 || m_iCursor >= static_cast<_int>(m_Steps.size()))
	{
		m_bPlaying = false;
		return;
	}

	m_fSequenceTime += fTimeDelta;

	while (true)
	{
		const UISEQ_STEP& s = m_Steps[m_iCursor];

		// 시작 작업
		if (!m_bStepStarted)
		{
			switch (s.eKind)
			{
			case UI_SEQ_STEP_KIND::PLAY_ANIM:
				if (s.pTarget && s.pTarget->Get_Animator())
					s.pTarget->Get_Animator()->Play_Animation(s.strAnimName);
				break;

			case UI_SEQ_STEP_KIND::SET_VISIBLE:
				if (s.pTarget) s.pTarget->Set_Visible(s.bVisible);
				break;

			case UI_SEQ_STEP_KIND::USE_CALLBACK:
				if (s.fnCallback) s.fnCallback();
				break;

			case UI_SEQ_STEP_KIND::WAIT:
				m_fTimer = 0.f;
				break;

			case UI_SEQ_STEP_KIND::EFFECT_PLAY:
			case UI_SEQ_STEP_KIND::BGM_PLAY:
			case UI_SEQ_STEP_KIND::SFX_PLAY:
			case UI_SEQ_STEP_KIND::SIGNAL_FIRE:
				Fire_Slot(s);
				break;

			case UI_SEQ_STEP_KIND::EFFECT_STOP:
			case UI_SEQ_STEP_KIND::BGM_STOP:
				Release_Slot(s);
				break;

			default: break;
			}
			m_bStepStarted = true;
		}

		// 종료 판정
		_bool bStepDone = false;
		switch (s.eKind)
		{
		case UI_SEQ_STEP_KIND::WAIT:
			m_fTimer += fTimeDelta;
			if (m_fTimer >= s.fWaitSec) bStepDone = true;
			break;

		case UI_SEQ_STEP_KIND::PLAY_ANIM:
		case UI_SEQ_STEP_KIND::SET_VISIBLE:
		case UI_SEQ_STEP_KIND::USE_CALLBACK:
		case UI_SEQ_STEP_KIND::EFFECT_PLAY:
		case UI_SEQ_STEP_KIND::EFFECT_STOP:
		case UI_SEQ_STEP_KIND::BGM_PLAY:
		case UI_SEQ_STEP_KIND::BGM_STOP:
		case UI_SEQ_STEP_KIND::SFX_PLAY:
		case UI_SEQ_STEP_KIND::SIGNAL_FIRE:
			bStepDone = true;   // 즉발형
			break;

		default: break;
		}

		if (!bStepDone) return;

		const UI_SEQ_STEP_KIND ePrevKind = s.eKind;

		m_iCursor++;
		m_bStepStarted = false;

		if (m_iCursor >= static_cast<_int>(m_Steps.size()))
		{
			m_bPlaying = false;
			return;
		}

		// 체이닝: 다음이 Join이거나 이전이 즉발형이면 같은 프레임 내 진행
		const _bool bChain = m_Steps[m_iCursor].bJoinPrev
			|| ePrevKind != UI_SEQ_STEP_KIND::WAIT;
		if (!bChain) return;
		// 계속 루프
	}
}

_bool CUISequence::Append(const UISEQ_STEP& step)
{
	if (m_bPlaying) return false;

	UISEQ_STEP s = step;
	s.bJoinPrev = false;
	m_Steps.push_back(s);
	return true;
}

_bool CUISequence::Join(const UISEQ_STEP& step)
{
	if (m_bPlaying) return false;

	UISEQ_STEP s = step;
	s.bJoinPrev = true;
	m_Steps.push_back(s);
	return true;
}

void CUISequence::Play()
{
	if (m_Steps.empty()) return;

	Release_All_ActiveSlots();

	/* 자식 위젯들의 진행 상태를 깨끗하게 리셋.
   - Animator::Stop_All() : 진행 중 트윈을 정지. 후속 PLAY_ANIM step 이
	 Play_Animation 으로 트윈을 새로 발화하면 시작값(start)부터 진행됨.
   - CUIImage::Reset_SpriteAnim() : sprite 프레임 인덱스/누적시간 0 복원.
   두 번째 재생부터 위젯 상태가 첫 재생의 끝값에 머물러 깨지던 문제 해결. */
	for (auto& pair : m_mapById)
	{
		CUIObject* pChild = pair.second;
		if (nullptr == pChild)
			continue;

		if (CUIAnimator* pAnim = pChild->Get_Animator())
			pAnim->Stop_All();

		if (CUIImage* pImage = dynamic_cast<CUIImage*>(pChild))
			pImage->Reset_SpriteAnim();
	}

	m_iCursor = 0;
	m_fTimer = 0.f;
	m_fSequenceTime = 0.f;
	m_bStepStarted = false;
	m_bPlaying = true;
}

void CUISequence::Stop()
{
	Release_All_ActiveSlots();

	m_bPlaying = false;
	m_iCursor = -1;
	m_fTimer = 0.f;
	m_fSequenceTime = 0.f;
	m_bStepStarted = false;
}

_bool CUISequence::Clear_Timeline()
{
	if (m_bPlaying) return false;

	Release_All_ActiveSlots();

	m_Steps.clear();
	return true;
}

void CUISequence::Bind_Slot(const _string& strSlotId, UISEQ_SLOT_CATEGORY eCategory, UISEQ_SLOT_FUNC fnFire, UISEQ_SLOT_FUNC fnRelease)
{
	if (strSlotId.empty())
		return;

	UISEQ_SLOT_BINDING tBinding{};
	tBinding.eCategory = eCategory;
	tBinding.fnFire = fnFire;
	tBinding.fnRelease = fnRelease;

	m_SlotBindings[strSlotId] = tBinding;
}

void CUISequence::Bind_Effect(const _string& strSlotId, UISEQ_SLOT_FUNC fnFire, UISEQ_SLOT_FUNC fnRelease)
{
	Bind_Slot(strSlotId, UISEQ_SLOT_CATEGORY::EFFECT, fnFire, fnRelease);
}

void CUISequence::Bind_BGM(const _string& strSlotId, UISEQ_SLOT_FUNC fnFire, UISEQ_SLOT_FUNC fnRelease)
{
	Bind_Slot(strSlotId, UISEQ_SLOT_CATEGORY::BGM, fnFire, fnRelease);
}

void CUISequence::Bind_SFX(const _string& strSlotId, UISEQ_SLOT_FUNC fnFire)
{
	Bind_Slot(strSlotId, UISEQ_SLOT_CATEGORY::SFX, fnFire, nullptr);
}

void CUISequence::Bind_Signal(const _string& strSlotId, UISEQ_SLOT_FUNC fnFire)
{
	Bind_Slot(strSlotId, UISEQ_SLOT_CATEGORY::SIGNAL, fnFire, nullptr);
}

void CUISequence::Unbind_Slot(const _string& strSlotId)
{
	if (strSlotId.empty())
		return;

	if (m_ActiveReleaseSlots.find(strSlotId) != m_ActiveReleaseSlots.end())
	{
		UISEQ_STEP s{};
		s.strSlotId = strSlotId;
		Release_Slot(s);
	}

	m_SlotBindings.erase(strSlotId);
	m_ActiveReleaseSlots.erase(strSlotId);
}

void CUISequence::Clear_Bindings()
{
	Release_All_ActiveSlots();

	m_SlotBindings.clear();
	m_ActiveReleaseSlots.clear();
}

HRESULT CUISequence::Build_FromFile(const _string& strPath, _uint iProtoLevel)
{
	if (INVALID_INDEX == iProtoLevel)
	{
		Log_Loader("invalid iProtoLevel (INVALID_INDEX) - caller must specify a valid prototype");
			return E_FAIL;
	}

	m_mapById.clear();

	// 1) json 파싱
	json root;
	if (FAILED(Parse_File(strPath, root)))
		return E_FAIL;

	const auto meta = root.value("meta", json::object());
	const _int iVersion = meta.value("version", 1);

	UICANVAS_DESC tCanvasDesc{};
	if (iVersion >= 2)
	{
		tCanvasDesc.fDesignWidth = meta.value("designWidth", 1920.f);
		tCanvasDesc.fDesignHeight = meta.value("designHeight", 1080.f);
		tCanvasDesc.eScalePolicy = UI_SCALE_POLICY_From_String(
			meta.value("scalePolicy", _string("UNIFORM_FIT")).c_str());

		if (tCanvasDesc.eScalePolicy == UI_SCALE_POLICY::END)
			tCanvasDesc.eScalePolicy = UI_SCALE_POLICY::UNIFORM_FIT;
	}
	else
	{
		tCanvasDesc.fDesignWidth = kLegacyDesignWidth;
		tCanvasDesc.fDesignHeight = kLegacyDesignHeight;
		tCanvasDesc.eScalePolicy = UI_SCALE_POLICY::UNIFORM_FIT;
	}

	Set_DesignCanvasSize(tCanvasDesc.fDesignWidth, tCanvasDesc.fDesignHeight);
	Set_ScalePolicy(tCanvasDesc.eScalePolicy);

	m_fSizeX = tCanvasDesc.fDesignWidth;
	m_fSizeY = tCanvasDesc.fDesignHeight;
	m_fCenterX = tCanvasDesc.fDesignWidth * 0.5f;
	m_fCenterY = tCanvasDesc.fDesignHeight * 0.5f;
	Refresh_Layout();

	// 2) loader 임시 수집
	vector<LoaderWidget> vLoadedWidgets;
	vector<LoaderStep> vLoadedSteps;

	for (const auto& jw : root.value("widgets", json::array()))
	{
		LoaderWidget w{};
		if (FAILED(Build_LoaderWidget(jw, w))) return E_FAIL;
		if (FAILED(Validate_RequiredResources(w.tDesc, w.eType)))
		{
			Log_Loader("missing required resource (id=%s)", w.strId.c_str());
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

	// 3) sanitize
	unordered_map<_string, set<_wstring>> mapAnims;
	for (const auto& w : vLoadedWidgets)
	{
		auto& s = mapAnims[w.strId];
		for (const auto& a : w.vAnimations)
			if (!a.strName.empty()) s.insert(a.strName);
	}
	Sanitize_Steps(vLoadedSteps, mapAnims);

	// 4) iZOrder stable_sort
	std::stable_sort(vLoadedWidgets.begin(), vLoadedWidgets.end(),
		[](const LoaderWidget& a, const LoaderWidget& b) {
			const _int za = std::visit([](const auto& d) {
				return static_cast<const CUIObject::UIOBJECT_DESC&>(d).iZOrder; }, a.tDesc);
			const _int zb = std::visit([](const auto& d) {
				return static_cast<const CUIObject::UIOBJECT_DESC&>(d).iZOrder; }, b.tDesc);
			return za < zb;
		});

	// 5) clone widget + animator 등록 (임시 vector)
	vector<CUIObject*> vTmp;
	unordered_map<_string, CUIObject*> mapTmpById;
	vTmp.reserve(vLoadedWidgets.size());

	auto Cleanup_Tmp = [&]() {
		for (auto* p : vTmp) Safe_Release(p);
		vTmp.clear(); mapTmpById.clear();
		};

	for (auto& w : vLoadedWidgets)
	{
		const WNameID strPrimary = Resolve_PrototypeTag(w);
		if (strPrimary == INVALID_TAG) { Cleanup_Tmp(); return E_FAIL; }

		std::visit([this](auto& d)
			{
				auto& tBase = static_cast<CUIObject::UIOBJECT_DESC&>(d);
				tBase.tCanvasDesc = m_tCanvasDesc;
			}, w.tDesc);

		void* pCArg = std::visit([](auto& d) -> void* {
			return static_cast<void*>(&d); }, w.tDesc);

		CGameObject* pClone = static_cast<CGameObject*>(
			m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, iProtoLevel, strPrimary, pCArg));

		if (nullptr == pClone && INVALID_TAG != w.strPrototypeTag)
		{
			const WNameID strFallback = Map_DefaultPrototypeTag(w.eType);
			if (INVALID_TAG != strFallback && strFallback != strPrimary)
			{
				Log_Loader("prototypeTag '%u' clone failed, falling back to default (id=%s)",
					w.strPrototypeTag, w.strId.c_str());

				pClone = static_cast<CGameObject*>(
					m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, iProtoLevel, strFallback, pCArg));
			}
		}

		if (nullptr == pClone)
		{
			Log_Loader("Clone failed (id=%s)", w.strId.c_str());
			Cleanup_Tmp(); return E_FAIL;
		}

		CUIObject* pUI = dynamic_cast<CUIObject*>(pClone);
		if (nullptr == pUI)
		{
			Log_Loader("Clone result is not CUIObject (id=%s)", w.strId.c_str());
			Safe_Release(pClone);
			Cleanup_Tmp(); return E_FAIL;
		}

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

	// 6) timeline build (self의 m_Steps에 직접 빌드)
	Clear_Timeline();
	for (const auto& sn : vLoadedSteps)
	{
		const _bool bNeedTarget =
			sn.eKind == UI_SEQ_STEP_KIND::PLAY_ANIM ||
			sn.eKind == UI_SEQ_STEP_KIND::SET_VISIBLE;

		const _bool bOptionalTarget =
			sn.eKind == UI_SEQ_STEP_KIND::EFFECT_PLAY ||
			sn.eKind == UI_SEQ_STEP_KIND::SFX_PLAY ||
			sn.eKind == UI_SEQ_STEP_KIND::SIGNAL_FIRE;

		const _bool bSlotStep =
			sn.eKind == UI_SEQ_STEP_KIND::EFFECT_PLAY ||
			sn.eKind == UI_SEQ_STEP_KIND::EFFECT_STOP ||
			sn.eKind == UI_SEQ_STEP_KIND::BGM_PLAY ||
			sn.eKind == UI_SEQ_STEP_KIND::BGM_STOP ||
			sn.eKind == UI_SEQ_STEP_KIND::SFX_PLAY ||
			sn.eKind == UI_SEQ_STEP_KIND::SIGNAL_FIRE;

		if (bNeedTarget && sn.strTargetId.empty()) continue;
		if (sn.eKind == UI_SEQ_STEP_KIND::PLAY_ANIM && sn.strAnimName.empty()) continue;

		if (bSlotStep && sn.strSlotId.empty())
		{
			Log_Loader("%sslot step skipped: empty slotId",
				sn.bRequired ? "required " : "");
			continue;
		}

		UISEQ_STEP s{};
		s.eKind = sn.eKind;
		s.strTargetId = sn.strTargetId;
		s.strSlotId = sn.strSlotId;
		s.strAnimName = sn.strAnimName;
		s.fWaitSec = sn.fWaitSec;
		s.bVisible = sn.bVisible;
		s.bJoinPrev = sn.bJoinPrev;
		s.bRequired = sn.bRequired;
		s.pTarget = nullptr;

		if (bNeedTarget || bOptionalTarget)
		{
			if (!sn.strTargetId.empty())
			{
				auto it = mapTmpById.find(sn.strTargetId);
				if (it != mapTmpById.end())
					s.pTarget = it->second;
				else if (bNeedTarget)
					continue;
			}
		}
		if (sn.eKind == UI_SEQ_STEP_KIND::USE_CALLBACK && !sn.strCallbackId.empty())
		{
			const _string strId = sn.strCallbackId;
			s.fnCallback = [strId]() { Log_Loader("callback stub: %s", strId.c_str()); };
		}

		if (!(sn.bJoinPrev ? Join(s) : Append(s)))
		{
			Log_Loader("Append/Join failed");
			Clear_Timeline();
			Cleanup_Tmp();
			return E_FAIL;
		}
	}

	// 7) commit: self의 children으로 흡수
	for (auto* pUI : vTmp)
	{
		Add_Child(pUI);     // ref++
		Safe_Release(pUI);  // ref-- : Container가 생성하여 소유(ref = 0)
	}
	vTmp.clear();
	m_mapById = std::move(mapTmpById);

	return S_OK;
}

void CUISequence::Fire_Slot(const UISEQ_STEP& s)
{
	if (s.strSlotId.empty())
	{
		Log_Loader("slot fire skipped: empty slotId");
		return;
	}

	auto it = m_SlotBindings.find(s.strSlotId);
	if (it == m_SlotBindings.end() || !it->second.fnFire)
	{
		Log_Loader("%sslot fire skipped: unbound slotId=%s",
			s.bRequired ? "required " : "",
			s.strSlotId.c_str());
		return;
	}

	UISEQ_EVENT_CONTEXT ctx{};
	ctx.pTarget = s.pTarget;
	ctx.strSlotId = s.strSlotId;
	ctx.strTargetId = s.strTargetId;
	ctx.fSequenceTime = m_fSequenceTime;

	it->second.fnFire(ctx);

	if (it->second.fnRelease)
		m_ActiveReleaseSlots.insert(s.strSlotId);
}

void CUISequence::Release_Slot(const UISEQ_STEP& s)
{
	if (s.strSlotId.empty())
	{
		Log_Loader("slot release skipped: empty slotId");
		return;
	}

	auto it = m_SlotBindings.find(s.strSlotId);
	if (it == m_SlotBindings.end() || !it->second.fnRelease)
	{
		Log_Loader("%sslot release skipped: unbound/no-release slotId=%s",
			s.bRequired ? "required " : "",
			s.strSlotId.c_str());

		m_ActiveReleaseSlots.erase(s.strSlotId);
		return;
	}

	UISEQ_EVENT_CONTEXT ctx{};
	ctx.pTarget = s.pTarget;
	ctx.strSlotId = s.strSlotId;
	ctx.strTargetId = s.strTargetId;
	ctx.fSequenceTime = m_fSequenceTime;

	it->second.fnRelease(ctx);
	m_ActiveReleaseSlots.erase(s.strSlotId);
}

void CUISequence::Release_All_ActiveSlots()
{
	vector<_string> vSlots;
	vSlots.reserve(m_ActiveReleaseSlots.size());

	for (const auto& strSlotId : m_ActiveReleaseSlots)
		vSlots.push_back(strSlotId);

	for (const auto& strSlotId : vSlots)
	{
		auto it = m_SlotBindings.find(strSlotId);
		if (it == m_SlotBindings.end() || !it->second.fnRelease)
			continue;

		UISEQ_EVENT_CONTEXT ctx{};
		ctx.strSlotId = strSlotId;
		ctx.fSequenceTime = m_fSequenceTime;

		it->second.fnRelease(ctx);
	}

	m_ActiveReleaseSlots.clear();
}

_bool CUISequence::Is_SlotPlayKind(UI_SEQ_STEP_KIND eKind) const
{
	return eKind == UI_SEQ_STEP_KIND::EFFECT_PLAY
		|| eKind == UI_SEQ_STEP_KIND::BGM_PLAY
		|| eKind == UI_SEQ_STEP_KIND::SFX_PLAY
		|| eKind == UI_SEQ_STEP_KIND::SIGNAL_FIRE;
}

_bool CUISequence::Is_SlotStopKind(UI_SEQ_STEP_KIND eKind) const
{
	return eKind == UI_SEQ_STEP_KIND::EFFECT_STOP
		|| eKind == UI_SEQ_STEP_KIND::BGM_STOP;
}

CUISequence* CUISequence::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUISequence* pInstance = new CUISequence(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CUISequence");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUISequence::Clone(void* pArg)
{
	CUISequence* pInstance = new CUISequence(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CUISequence");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUISequence::Free()
{
	Release_All_ActiveSlots();
	Clear_Bindings();

	__super::Free();

	m_Steps.clear();
	m_mapById.clear();
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
		const _int iVersion = meta.value("version", 1);

		if (iVersion != 1 && iVersion != 2)
		{
			Log_Loader("version mismatch: unsupported version %d (%s)", iVersion, strPath.c_str());
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
			d.fPivotX = t.value("pivotX", 0.5f);
			d.fPivotY = t.value("pivotY", 0.5f);
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

		// -- Sprite Animation --
		const auto jSprite = j.value("spriteAnim", json::object());
		d.bSpriteAnimEnabled = jSprite.value("enabled", false);
		d.fSpriteFrameDuration = jSprite.value("frameDuration", 0.f);

		if (d.bSpriteAnimEnabled && d.fSpriteFrameDuration <= 0.f)
		{
			
			Log_Loader("spriteAnim: enabled but frameDutation<=0 - disabling");
			d.bSpriteAnimEnabled = false;
			d.fSpriteFrameDuration = 0.f;
		}

		// -- Shared Textures --
		d.SharedTextureBindings.clear();
		for (const auto& jb : j.value("sharedTextures", json::array()))
		{
			UI_SHARED_TEXTURE_BINDING_DESC b{};
			b.strSharedTexName = jb.value("group", _string{});
			b.strShaderVarName = jb.value("shaderVar", _string{});
			b.iTextureIndex = jb.value("index", static_cast<unsigned int>(-1));

			// 결손 entry - 묵음 skip(편집 중간 상태 보호)
			if (b.strSharedTexName.empty()
				|| b.strShaderVarName.empty()
				|| b.iTextureIndex == static_cast<unsigned int>(-1))
			{
				Log_Loader("sharedTextures: incomplete entry skipped (group='%s', shaderVar='%s', index='%u')",
					b.strSharedTexName.c_str(),
					b.strShaderVarName.c_str(),
					b.iTextureIndex);
				continue;
			}

			// group 이름 검증 - 단일 진실원천(SHARED_TEXTURE_From_String) 사용
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

		d.strShaderTag = SToTag(sh.value("tag", _string(kDefaultShader)));
		d.iShaderLevel = sh.value("level", 0u);

		d.strVIBufferTag = SToTag(vb.value("tag", _string(kDefaultVIBuffer)));
		d.iVIBufferLevel = vb.value("level", 0u);

		d.strTextureTag = SToTag(tx.value("tag", _string{}));
		d.iTextureLevel = tx.value("level", 0u);

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
		d.eVAlign = StrToTextVAlign(j.value("verticalAlign", _string("CENTER")));
		if (UI_TEXT_VALIGN::END == d.eVAlign)
			d.eVAlign = UI_TEXT_VALIGN::CENTER;

		d.bWordWrap = j.value("wordWrap", false);
		d.bClipToRect = j.value("clipToRect", false);
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
		s.strSlotId = j.value("slotId", _string{});
		s.strAnimName = StoW(j.value("animName", _string{}));
		s.fWaitSec = j.value("waitSec", 0.f);
		s.bVisible = j.value("visible", true);
		s.strCallbackId = j.value("callbackId", _string{});
		s.bRequired = j.value("required", false);

		return S_OK;
	}

	HRESULT Build_LoaderWidget(const json& j, LoaderWidget& w)
	{
		w.strId = j.value("id", _string{});
		w.strPrototypeTag = SToTag(j.value("prototypeTag", _string{}));

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

	HRESULT Validate_RequiredResources(DescVariant& tDesc, UI_TYPE eType)
	{
		static const WNameID s_strDummyWhite = WNAME(L"Prototype_Component_Texture_Dummy_White");

		return std::visit([](auto& d) -> HRESULT
			{
				using T = std::decay_t<decltype(d)>;
				if constexpr (std::is_same_v<T, CUIImage::UIIMAGE_DESC>)
				{
					if (d.strTextureTag == INVALID_TAG)
						d.strTextureTag = s_strDummyWhite;
				}
				else if constexpr (std::is_same_v<T, CUIButton::UIBUTTON_DESC>)
				{
					if (d.strTextureTag == INVALID_TAG)
						d.strTextureTag = s_strDummyWhite;
				}
				else if constexpr (std::is_same_v<T, CUIProgressBar::UIPROGRESSBAR_DESC>)
				{
					if (d.strBackTextureTag == INVALID_TAG)
						d.strBackTextureTag = s_strDummyWhite;
					if (d.strFillTextureTag == INVALID_TAG)
						d.strFillTextureTag = s_strDummyWhite;
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