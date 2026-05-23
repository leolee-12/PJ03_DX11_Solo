#ifndef Editor_UI_h__
#define Editor_UI_h__
#include "UIContainer.h"
#include "UIImage.h"
#include "UIText.h"
#include "UIButton.h"
#include "UIProgressBar.h"
#include "UITween.h"
#include "UI_Defines.h"

NS_BEGIN(Editor)

struct UISEQ_ANIMATION_NODE
{
	_wstring strName;
	vector<CUITween::UITWEEN_DESC> vTracks;
};

struct UISEQ_STEP_NODE
{
	UI_SEQ_STEP_KIND eKind{ UI_SEQ_STEP_KIND::WAIT };
	_string strTargetId;             // UTF-8 (UISEQ_WIDGET_NODE::strId 참조)
	_string strSlotId;               // 외부 슬롯 ID
	_wstring strAnimName;
	_float fWaitSec{ 0.f };
	_bool bVisible{ true };
	_string strCallbackId;           // 예약
	_bool bJoinPrev{ false };
	_bool bRequired{ false };
};

struct UISEQ_WIDGET_NODE
{
	using tDescType = std::variant<
		CUIContainer::UICONTAINER_DESC,
		CUIImage::UIIMAGE_DESC,
		CUIText::UITEXT_DESC,
		CUIButton::UIBUTTON_DESC,
		CUIProgressBar::UIPROGRESSBAR_DESC
	>;

	_string strId;
	_string strDisplayName;
	WNameID strPrototypeTag{ INVALID_TAG };
	tDescType tDesc;
	vector<UISEQ_ANIMATION_NODE> vAnimations;

	UI_TYPE Get_Type() const
	{
		return std::visit([](const auto& d) {
			using T = std::decay_t<decltype(d)>;
			if constexpr (std::is_same_v<T, CUIImage::UIIMAGE_DESC>)					return UI_TYPE::IMAGE;
			else if constexpr (std::is_same_v<T, CUIText::UITEXT_DESC>)					return UI_TYPE::TEXT;
			else if constexpr (std::is_same_v<T, CUIButton::UIBUTTON_DESC>)				return UI_TYPE::BUTTON;
			else if constexpr (std::is_same_v<T, CUIProgressBar::UIPROGRESSBAR_DESC>)	return UI_TYPE::PROGRESSBAR;
			else																		return UI_TYPE::CONTAINER;
			}, tDesc);
	}
};

// 파일 단위 문서
struct UISEQ_DOC
{
	_int iVersion{ 2 };
	_string strName;

	_float fDesignWidth{ 1280.f };
	_float fDesignHeight{ 720.f };
	UI_SCALE_POLICY eScalePolicy{ UI_SCALE_POLICY::UNIFORM_FIT };

	vector<UISEQ_WIDGET_NODE> vWidgets;
	vector<UISEQ_STEP_NODE> vSteps;
};



/* -------- 상수 콤보 라벨 -------- */
template <typename Pair, size_t N, size_t... Is>
constexpr auto Make_Names_Impl(const Pair(&t)[N], std::index_sequence<Is...>)
{
	return std::array<const char*, N>{ t[Is].s... };
}

template <typename Pair, size_t N>
constexpr auto Make_Names(const Pair(&t)[N])
{
	return Make_Names_Impl(t, std::make_index_sequence<N>{});
}

template <typename E, typename Pair, size_t N>
inline _bool Combo_Enum(const char* pszLabel, E& eValue, const Pair(&tTable)[N])
{
	_int iIdx = 0;
	for (size_t i = 0; i < N; ++i) if (tTable[i].e == eValue) { iIdx = (_int)i; break; }
	if (!ImGui::BeginCombo(pszLabel, tTable[iIdx].s)) return false;

	_bool bChanged = false;
	for (size_t i = 0; i < N; ++i)
	{
		const _bool bSel = ((size_t)iIdx == i);
		if (ImGui::Selectable(tTable[i].s, bSel) && !bSel)
		{
			eValue = tTable[i].e;
			bChanged = true;
		}
	}
	ImGui::EndCombo();
	return bChanged;
}

inline constexpr auto g_UITypeNames = Make_Names(Engine::detail::kUIType);
inline constexpr auto g_UILayoutNames = Make_Names(Engine::detail::kUILayout);
inline constexpr auto g_UIProgressDirNames = Make_Names(Engine::detail::kProgressDir);
inline constexpr auto g_AnchorNames = Make_Names(Engine::detail::kAnchor);
inline constexpr auto g_TextAlignNames = Make_Names(Engine::detail::kTextAlign);
inline constexpr auto g_StepKindNames = Make_Names(Engine::detail::kStepKind);
inline constexpr auto g_EaseNames = Make_Names(Engine::detail::kEase);
inline constexpr auto g_LoopNames = Make_Names(Engine::detail::kTweenLoop);



/* -------- WNameID <-> string -------- */
inline _string TagToString(WNameID strTag)
{
#ifdef _DEBUG
	if (INVALID_TAG == strTag)
		return {};

	const wchar_t* pszLookup = Engine::WNameRegistry::Lookup(strTag);
	return nullptr != pszLookup ? WtoS(_wstring(pszLookup)) : _string{};
#else
	return (INVALID_TAG == strTag) ? _string{} : std::to_string(static_cast<_uint32>(strTag));
#endif
}

inline WNameID StringToTag(const _string& strTag)
{
	if (strTag.empty())
		return INVALID_TAG;

	if (strTag[0] == '-' || (strTag[0] >= '0' && strTag[0] <= '9'))
	{
		try
		{
			return static_cast<WNameID>(std::stoul(strTag));
		}
		catch (...)
		{
		}
	}

	return WNAME(StoW(strTag));
}



/* -------- ImGui input wrappers -------- */
template <size_t N>
inline _bool Edit_StringField(const char* pszLabel, _string& strValue, ImGuiInputTextFlags iFlags = 0)
{
	std::array<char, N> szBuffer{};
	strncpy_s(szBuffer.data(), szBuffer.size(), strValue.c_str(), _TRUNCATE);

	if (!ImGui::InputText(pszLabel, szBuffer.data(), szBuffer.size(), iFlags))
		return false;

	strValue = szBuffer.data();
	return true;
}

template <size_t N>
inline _bool Edit_WStringField(const char* pszLabel, _wstring& strValue, ImGuiInputTextFlags iFlags = 0)
{
	_string strUtf8 = WtoS(strValue);
	std::array<char, N> szBuffer{};
	strncpy_s(szBuffer.data(), szBuffer.size(), strUtf8.c_str(), _TRUNCATE);

	if (!ImGui::InputText(pszLabel, szBuffer.data(), szBuffer.size(), iFlags))
		return false;

	strValue = StoW(szBuffer.data());
	return true;
}

template <size_t N>
inline _bool Edit_WStringMultiline(const char* pszLabel, _wstring& v, ImVec2 size)
{
	_string s = WtoS(v);
	char buf[N] = {};
	::strncpy_s(buf, s.c_str(), _TRUNCATE);
	if (ImGui::InputTextMultiline(pszLabel, buf, N, size))
	{
		v = StoW(buf);
		return true;
	}
	return false;
}

template <size_t N>
inline _bool Edit_TagField(const char* pszLabel, WNameID& strTag)
{
	_string strValue = TagToString(strTag);
	std::array<char, N> szBuffer{};
	strncpy_s(szBuffer.data(), szBuffer.size(), strValue.c_str(), _TRUNCATE);

	if (!ImGui::InputText(pszLabel, szBuffer.data(), szBuffer.size()))
		return false;

	strTag = StringToTag(szBuffer.data());
	return true;
}

inline _bool Combo_UITexture(const char* pszLabel, WNameID& strTag)
{
	const Game_PKM::UI_TEXTURE_OPTION* pCurrent = Game_PKM::UI_FindTextureOption(strTag);

	_string strPreview;
	if (INVALID_TAG == strTag)
		strPreview = "(none)";
	else if (nullptr != pCurrent)
		strPreview = pCurrent->pLabel;
	else
	{
		const _string strLookup = TagToString(strTag);
		strPreview = strLookup.empty() ? "(unknown)" : ("(unknown: " + strLookup + ")");
	}

	if (!ImGui::BeginCombo(pszLabel, strPreview.c_str()))
		return false;

	_bool bChanged = false;

	{
		const _bool bSel = (PROTO_COM_TEX_DUMMY_WHITE == strTag);
		if (ImGui::Selectable("(none)", bSel) && !bSel)
		{
			strTag = PROTO_COM_TEX_DUMMY_WHITE;
			bChanged = true;
		}
		if (bSel)
			ImGui::SetItemDefaultFocus();
	}

	/* 카탈로그에 없는 태그는 비활성 라인으로만 노출 — 의도치 않은 덮어쓰기 방지 */
	if (INVALID_TAG != strTag && nullptr == pCurrent)
	{
		ImGui::BeginDisabled();
		ImGui::Selectable(strPreview.c_str(), true);
		ImGui::EndDisabled();
	}

	for (const auto& opt : Game_PKM::g_UITextureOptions)
	{
		const _bool bSel = (opt.strTag == strTag);
		if (ImGui::Selectable(opt.pLabel, bSel) && !bSel)
		{
			strTag = opt.strTag;
			bChanged = true;
		}
		if (bSel)
			ImGui::SetItemDefaultFocus();
	}

	ImGui::EndCombo();
	return bChanged;
}

inline _bool Edit_UIntField(const char* pszLabel, _uint& iValue)
{
	_int iTemp = (INVALID_INDEX == iValue) ? -1 : static_cast<_int>(iValue);
	if (!ImGui::InputInt(pszLabel, &iTemp))
		return false;

	iValue = (iTemp < 0) ? INVALID_INDEX : static_cast<_uint>(iTemp);
	return true;
}

template <size_t N>
inline void Draw_ReadOnlyString(const char* pszLabel, const _string& strValue)
{
	std::array<char, N> szBuffer{};
	strncpy_s(szBuffer.data(), szBuffer.size(), strValue.c_str(), _TRUNCATE);
	ImGui::InputText(pszLabel, szBuffer.data(), szBuffer.size(), ImGuiInputTextFlags_ReadOnly);
}



/* -------- variant<5종> -> CUIObject::UIOBJECT_DESC& 추출 -------- */
inline CUIObject::UIOBJECT_DESC& Get_BaseDesc(UISEQ_WIDGET_NODE& tWidget)
{
	return std::visit([](auto& tDesc) -> CUIObject::UIOBJECT_DESC&
		{
			return static_cast<CUIObject::UIOBJECT_DESC&>(tDesc);
		}, tWidget.tDesc);
}

inline const CUIObject::UIOBJECT_DESC& Get_BaseDesc(const UISEQ_WIDGET_NODE& tWidget)
{
	return std::visit([](const auto& tDesc) -> const CUIObject::UIOBJECT_DESC&
		{
			return static_cast<const CUIObject::UIOBJECT_DESC&>(tDesc);
		}, tWidget.tDesc);
}



inline void Label_Left(const char* pszLabel)
{
	ImGui::AlignTextToFramePadding();
	ImGui::TextUnformatted(pszLabel);
	ImGui::SameLine();
}

void Draw_VPModeRadio(class CUIEditorSession* pSession, const char* pszId);

NS_END

#endif // Editor_UI_h__