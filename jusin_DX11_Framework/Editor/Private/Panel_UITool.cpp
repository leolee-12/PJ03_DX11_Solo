#include "Panel_UITool.h"

#include "EditInstance.h"

#include <array>
#include <limits>
#include <sstream>

namespace
{
	struct CANVAS_BOX
	{
		_float fLeft{};
		_float fTop{};
		_float fWidth{};
		_float fHeight{};
	};

	constexpr const char* g_ppAnchorNames[] =
	{
		"TL", "TC", "TR",
		"ML", "MC", "MR",
		"BL", "BC", "BR",
	};

	constexpr const char* g_ppLayoutNames[] =
	{
		"NONE",
		"CANVAS",
		"HORIZONTAL",
		"VERTICAL",
		"OVERLAY",
	};

	constexpr const char* g_ppTextAlignNames[] =
	{
		"LEFT",
		"CENTER",
		"RIGHT",
	};

	constexpr const char* g_ppProgressDirNames[] =
	{
		"LEFT_TO_RIGHT",
		"RIGHT_TO_LEFT",
		"TOP_TO_BOTTOM",
		"BOTTOM_TO_TOP",
	};

	constexpr const char* g_ppStepKindNames[] =
	{
		"PLAY_ANIM",
		"SET_VISIBLE",
		"WAIT",
		"USE_CALLBACK",
	};

	constexpr const char* g_ppEaseNames[] =
	{
		"LINEAR",
		"EASE_IN_SINE",
		"EASE_OUT_SINE",
		"EASE_IN_OUT_SINE",
		"EASE_IN_QUAD",
		"EASE_OUT_QUAD",
		"EASE_IN_OUT_QUAD",
		"EASE_IN_CUBIC",
		"EASE_OUT_CUBIC",
		"EASE_IN_OUT_CUBIC",
	};

	constexpr const char* g_ppLoopNames[] =
	{
		"NONE",
		"LOOP",
		"PINGPONG",
	};

	_string TagToString(WNameID strTag)
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

	WNameID StringToTag(const _string& strTag)
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

	template <size_t N>
	_bool Edit_StringField(const char* pszLabel, _string& strValue, ImGuiInputTextFlags iFlags = 0)
	{
		std::array<char, N> szBuffer{};
		strncpy_s(szBuffer.data(), szBuffer.size(), strValue.c_str(), _TRUNCATE);

		if (!ImGui::InputText(pszLabel, szBuffer.data(), szBuffer.size(), iFlags))
			return false;

		strValue = szBuffer.data();
		return true;
	}

	template <size_t N>
	_bool Edit_WStringField(const char* pszLabel, _wstring& strValue, ImGuiInputTextFlags iFlags = 0)
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
	_bool Edit_WStringMultiline(const char* pszLabel, _wstring& strValue, const ImVec2& vSize)
	{
		_string strUtf8 = WtoS(strValue);
		std::array<char, N> szBuffer{};
		strncpy_s(szBuffer.data(), szBuffer.size(), strUtf8.c_str(), _TRUNCATE);

		if (!ImGui::InputTextMultiline(pszLabel, szBuffer.data(), szBuffer.size(), vSize))
			return false;

		strValue = StoW(szBuffer.data());
		return true;
	}

	template <size_t N>
	void Draw_ReadOnlyString(const char* pszLabel, const _string& strValue)
	{
		std::array<char, N> szBuffer{};
		strncpy_s(szBuffer.data(), szBuffer.size(), strValue.c_str(), _TRUNCATE);
		ImGui::InputText(pszLabel, szBuffer.data(), szBuffer.size(), ImGuiInputTextFlags_ReadOnly);
	}

	template <size_t N>
	_bool Edit_TagField(const char* pszLabel, WNameID& strTag)
	{
		_string strValue = TagToString(strTag);
		std::array<char, N> szBuffer{};
		strncpy_s(szBuffer.data(), szBuffer.size(), strValue.c_str(), _TRUNCATE);

		if (!ImGui::InputText(pszLabel, szBuffer.data(), szBuffer.size()))
			return false;

		strTag = StringToTag(szBuffer.data());
		return true;
	}

	_bool Edit_UIntField(const char* pszLabel, _uint& iValue)
	{
		_int iTemp = (INVALID_INDEX == iValue) ? -1 : static_cast<_int>(iValue);
		if (!ImGui::InputInt(pszLabel, &iTemp))
			return false;

		iValue = (iTemp < 0) ? INVALID_INDEX : static_cast<_uint>(iTemp);
		return true;
	}

	CUIObject::UIOBJECT_DESC& Get_BaseDesc(UISEQ_WIDGET_NODE& tWidget)
	{
		return std::visit([](auto& tDesc) -> CUIObject::UIOBJECT_DESC&
			{
				return static_cast<CUIObject::UIOBJECT_DESC&>(tDesc);
			}, tWidget.tDesc);
	}

	const CUIObject::UIOBJECT_DESC& Get_BaseDesc(const UISEQ_WIDGET_NODE& tWidget)
	{
		return std::visit([](const auto& tDesc) -> const CUIObject::UIOBJECT_DESC&
			{
				return static_cast<const CUIObject::UIOBJECT_DESC&>(tDesc);
			}, tWidget.tDesc);
	}

	_float2 Resolve_CanvasCenter(const CUIObject::UIOBJECT_DESC& tDesc)
	{
		if (!tDesc.tAnchorDesc.bUseAnchoredPos)
			return _float2(tDesc.fCenterX, tDesc.fCenterY);

		const _float fLeft = 0.f;
		const _float fTop = 0.f;
		const _float fRight = static_cast<_float>(g_iWinSizeX);
		const _float fBottom = static_cast<_float>(g_iWinSizeY);
		const _float fCenterX = static_cast<_float>(g_iWinSizeX) * 0.5f;
		const _float fCenterY = static_cast<_float>(g_iWinSizeY) * 0.5f;

		_float fAnchorX = fCenterX;
		_float fAnchorY = fCenterY;

		switch (tDesc.tAnchorDesc.eAnchor)
		{
		case UI_ANCHOR::TL: fAnchorX = fLeft;    fAnchorY = fTop;     break;
		case UI_ANCHOR::TC: fAnchorX = fCenterX; fAnchorY = fTop;     break;
		case UI_ANCHOR::TR: fAnchorX = fRight;   fAnchorY = fTop;     break;
		case UI_ANCHOR::ML: fAnchorX = fLeft;    fAnchorY = fCenterY; break;
		case UI_ANCHOR::MC: fAnchorX = fCenterX; fAnchorY = fCenterY; break;
		case UI_ANCHOR::MR: fAnchorX = fRight;   fAnchorY = fCenterY; break;
		case UI_ANCHOR::BL: fAnchorX = fLeft;    fAnchorY = fBottom;  break;
		case UI_ANCHOR::BC: fAnchorX = fCenterX; fAnchorY = fBottom;  break;
		case UI_ANCHOR::BR: fAnchorX = fRight;   fAnchorY = fBottom;  break;
		default: break;
		}

		return _float2(
			fAnchorX + tDesc.tAnchorDesc.fOffsetX,
			fAnchorY + tDesc.tAnchorDesc.fOffsetY);
	}

	CANVAS_BOX Make_CanvasBox(const UISEQ_WIDGET_NODE& tWidget)
	{
		const auto& tBase = Get_BaseDesc(tWidget);
		const _float2 vCenter = Resolve_CanvasCenter(tBase);

		CANVAS_BOX tBox{};
		tBox.fLeft = vCenter.x - tBase.fSizeX * 0.5f;
		tBox.fTop = vCenter.y - tBase.fSizeY * 0.5f;
		tBox.fWidth = tBase.fSizeX;
		tBox.fHeight = tBase.fSizeY;
		return tBox;
	}

	_bool Is_PointInBox(const CANVAS_BOX& tBox, _float fX, _float fY)
	{
		return fX >= tBox.fLeft
			&& fX <= tBox.fLeft + tBox.fWidth
			&& fY >= tBox.fTop
			&& fY <= tBox.fTop + tBox.fHeight;
	}

	ImU32 Get_WidgetColor(UI_TYPE eType, _bool bVisible)
	{
		switch (eType)
		{
		case UI_TYPE::IMAGE: return bVisible ? IM_COL32(60, 140, 220, 140) : IM_COL32(60, 140, 220, 70);
		case UI_TYPE::TEXT: return bVisible ? IM_COL32(60, 180, 120, 140) : IM_COL32(60, 180, 120, 70);
		case UI_TYPE::BUTTON: return bVisible ? IM_COL32(220, 120, 60, 140) : IM_COL32(220, 120, 60, 70);
		case UI_TYPE::PROGRESSBAR: return bVisible ? IM_COL32(180, 120, 220, 140) : IM_COL32(180, 120, 220, 70);
		case UI_TYPE::CONTAINER: return bVisible ? IM_COL32(120, 150, 150, 110) : IM_COL32(120, 150, 150, 55);
		default: return bVisible ? IM_COL32(0, 150, 255, 140) : IM_COL32(0, 150, 255, 70);
		}
	}

	_string Make_WidgetListLabel(const UISEQ_WIDGET_NODE& tWidget)
	{
		return tWidget.strDisplayName + " [" + To_String(tWidget.Get_Type()) + "]";
	}

	_string Make_StepLabel(_int iIndex, const UISEQ_STEP_NODE& tStep)
	{
		std::ostringstream oss;
		oss << iIndex << " " << To_String(tStep.eKind);

		switch (tStep.eKind)
		{
		case UI_SEQ_STEP_KIND::PLAY_ANIM:
			oss << " " << (tStep.strTargetId.empty() ? "<none>" : tStep.strTargetId);
			if (!tStep.strAnimName.empty())
				oss << " / " << WtoS(tStep.strAnimName);
			break;

		case UI_SEQ_STEP_KIND::SET_VISIBLE:
			oss << " " << (tStep.strTargetId.empty() ? "<none>" : tStep.strTargetId)
				<< " = " << (tStep.bVisible ? "true" : "false");
			break;

		case UI_SEQ_STEP_KIND::WAIT:
			oss << " " << tStep.fWaitSec << "s";
			break;

		case UI_SEQ_STEP_KIND::USE_CALLBACK:
			oss << " " << (tStep.strCallbackId.empty() ? "<none>" : tStep.strCallbackId);
			break;

		default:
			break;
		}

		if (tStep.bJoinPrev)
			oss << " [Join]";

		return oss.str();
	}
}

CPanel_UITool::CPanel_UITool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPanel_Base()
	, m_pDevice(pDevice)
	, m_pContext(pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CPanel_UITool::Initialize()
{
	m_strTitle = "UI";

	if (FAILED(__super::Initialize()))
		return E_FAIL;

	if (FAILED(Initialize_Sentinels()))
		return E_FAIL;

	Reset_Doc();
	m_strStatus = "Ready";
	return S_OK;
}

void CPanel_UITool::Update(_float fTimeDelta)
{
}

HRESULT CPanel_UITool::Render()
{
	if (!Begin_Panel())
	{
		End_Panel();
		return S_OK;
	}

	Draw_Toolbar();
	ImGui::Separator();

	if (ImGui::BeginTable("##UIEditorLayout", 2, ImGuiTableFlags_Resizable | ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_SizingStretchProp))
	{
		const ImVec2 vAvail = ImGui::GetContentRegionAvail();
		const float fSpacingY = ImGui::GetStyle().ItemSpacing.y;
		const float fTopHeight = (std::max)(120.f, (vAvail.y - fSpacingY) * 0.5f);

		ImGui::TableSetupColumn("Left", ImGuiTableColumnFlags_WidthStretch, 1.f);
		ImGui::TableSetupColumn("Right", ImGuiTableColumnFlags_WidthStretch, 1.f);

		ImGui::TableNextColumn();
		ImGui::BeginChild("Hierarchy", ImVec2(0.f, fTopHeight), true);
		Draw_Hierarchy();
		ImGui::EndChild();
		ImGui::BeginChild("Timeline", ImVec2(0.f, 0.f), true);
		Draw_Timeline();
		ImGui::EndChild();

		ImGui::TableNextColumn();
		ImGui::BeginChild("Inspector", ImVec2(0.f, fTopHeight), true);
		Draw_Inspector();
		ImGui::EndChild();
		ImGui::BeginChild("Canvas", ImVec2(0.f, 0.f), true);
		Draw_Canvas();
		ImGui::EndChild();

		ImGui::EndTable();
	}

	End_Panel();
	return S_OK;
}

HRESULT CPanel_UITool::Initialize_Sentinels()
{
	m_pContainerSentinel = CUIContainer::Create(m_pDevice, m_pContext);
	if (nullptr == m_pContainerSentinel)
		return E_FAIL;

	m_pImageSentinel = CUIImage::Create(m_pDevice, m_pContext);
	if (nullptr == m_pImageSentinel)
		return E_FAIL;

	m_pTextSentinel = CUIText::Create(m_pDevice, m_pContext);
	if (nullptr == m_pTextSentinel)
		return E_FAIL;

	m_pButtonSentinel = CUIButton::Create(m_pDevice, m_pContext);
	if (nullptr == m_pButtonSentinel)
		return E_FAIL;

	m_pProgressBarSentinel = CUIProgressBar::Create(m_pDevice, m_pContext);
	if (nullptr == m_pProgressBarSentinel)
		return E_FAIL;

	return S_OK;
}

void CPanel_UITool::Reset_Doc()
{
	m_Doc = {};
	m_Doc.iVersion = 1;
	m_Doc.strName = "HUD_Layout";

	m_iSelectedWidget = -1;
	m_iSelectedAnimation = -1;
	m_iSelectedTrack = -1;
	m_iSelectedStep = -1;
	m_bDirty = false;

	m_bCanvasDragging = false;
	m_iCanvasDragWidget = -1;
	m_vDragStartMouse = {};
	m_vDragStartValue = {};
}

void CPanel_UITool::Normalize_Selection()
{
	if (m_Doc.vWidgets.empty())
	{
		m_iSelectedWidget = -1;
		m_iSelectedAnimation = -1;
		m_iSelectedTrack = -1;
	}
	else
	{
		if (m_iSelectedWidget >= static_cast<_int>(m_Doc.vWidgets.size()))
			m_iSelectedWidget = static_cast<_int>(m_Doc.vWidgets.size()) - 1;

		if (m_iSelectedWidget >= 0)
		{
			auto& vAnimations = m_Doc.vWidgets[m_iSelectedWidget].vAnimations;
			if (vAnimations.empty())
			{
				m_iSelectedAnimation = -1;
				m_iSelectedTrack = -1;
			}
			else
			{
				if (m_iSelectedAnimation >= static_cast<_int>(vAnimations.size()))
					m_iSelectedAnimation = static_cast<_int>(vAnimations.size()) - 1;

				if (m_iSelectedAnimation >= 0)
				{
					auto& vTracks = vAnimations[m_iSelectedAnimation].vTracks;
					if (vTracks.empty())
						m_iSelectedTrack = -1;
					else if (m_iSelectedTrack >= static_cast<_int>(vTracks.size()))
						m_iSelectedTrack = static_cast<_int>(vTracks.size()) - 1;
				}
				else
				{
					m_iSelectedTrack = -1;
				}
			}
		}
		else
		{
			m_iSelectedAnimation = -1;
			m_iSelectedTrack = -1;
		}
	}

	if (m_Doc.vSteps.empty())
		m_iSelectedStep = -1;
	else if (m_iSelectedStep >= static_cast<_int>(m_Doc.vSteps.size()))
		m_iSelectedStep = static_cast<_int>(m_Doc.vSteps.size()) - 1;
}

void CPanel_UITool::Mark_Dirty(const char* pszReason)
{
	m_bDirty = true;
	m_strStatus = (nullptr != pszReason && '\0' != pszReason[0]) ? pszReason : "Modified";
}

UISEQ_WIDGET_NODE* CPanel_UITool::Get_SelectedWidget()
{
	if (m_iSelectedWidget < 0 || m_iSelectedWidget >= static_cast<_int>(m_Doc.vWidgets.size()))
		return nullptr;

	return &m_Doc.vWidgets[m_iSelectedWidget];
}

const UISEQ_WIDGET_NODE* CPanel_UITool::Get_SelectedWidget() const
{
	if (m_iSelectedWidget < 0 || m_iSelectedWidget >= static_cast<_int>(m_Doc.vWidgets.size()))
		return nullptr;

	return &m_Doc.vWidgets[m_iSelectedWidget];
}

UISEQ_ANIMATION_NODE* CPanel_UITool::Get_SelectedAnimation()
{
	UISEQ_WIDGET_NODE* pWidget = Get_SelectedWidget();
	if (nullptr == pWidget)
		return nullptr;

	if (m_iSelectedAnimation < 0 || m_iSelectedAnimation >= static_cast<_int>(pWidget->vAnimations.size()))
		return nullptr;

	return &pWidget->vAnimations[m_iSelectedAnimation];
}

const UISEQ_ANIMATION_NODE* CPanel_UITool::Get_SelectedAnimation() const
{
	const UISEQ_WIDGET_NODE* pWidget = Get_SelectedWidget();
	if (nullptr == pWidget)
		return nullptr;

	if (m_iSelectedAnimation < 0 || m_iSelectedAnimation >= static_cast<_int>(pWidget->vAnimations.size()))
		return nullptr;

	return &pWidget->vAnimations[m_iSelectedAnimation];
}

CUITween::UITWEEN_DESC* CPanel_UITool::Get_SelectedTrack()
{
	UISEQ_ANIMATION_NODE* pAnimation = Get_SelectedAnimation();
	if (nullptr == pAnimation)
		return nullptr;

	if (m_iSelectedTrack < 0 || m_iSelectedTrack >= static_cast<_int>(pAnimation->vTracks.size()))
		return nullptr;

	return &pAnimation->vTracks[m_iSelectedTrack];
}

const CUITween::UITWEEN_DESC* CPanel_UITool::Get_SelectedTrack() const
{
	const UISEQ_ANIMATION_NODE* pAnimation = Get_SelectedAnimation();
	if (nullptr == pAnimation)
		return nullptr;

	if (m_iSelectedTrack < 0 || m_iSelectedTrack >= static_cast<_int>(pAnimation->vTracks.size()))
		return nullptr;

	return &pAnimation->vTracks[m_iSelectedTrack];
}

UISEQ_STEP_NODE* CPanel_UITool::Get_SelectedStep()
{
	if (m_iSelectedStep < 0 || m_iSelectedStep >= static_cast<_int>(m_Doc.vSteps.size()))
		return nullptr;

	return &m_Doc.vSteps[m_iSelectedStep];
}

const UISEQ_STEP_NODE* CPanel_UITool::Get_SelectedStep() const
{
	if (m_iSelectedStep < 0 || m_iSelectedStep >= static_cast<_int>(m_Doc.vSteps.size()))
		return nullptr;

	return &m_Doc.vSteps[m_iSelectedStep];
}

CUIObject* CPanel_UITool::Resolve_Sentinel(UI_TYPE eType) const
{
	switch (eType)
	{
	case UI_TYPE::CONTAINER: return m_pContainerSentinel;
	case UI_TYPE::IMAGE: return m_pImageSentinel;
	case UI_TYPE::TEXT: return m_pTextSentinel;
	case UI_TYPE::BUTTON: return m_pButtonSentinel;
	case UI_TYPE::PROGRESSBAR: return m_pProgressBarSentinel;
	default: return nullptr;
	}
}

const UISEQ_WIDGET_NODE* CPanel_UITool::Find_WidgetById(const _string& strId) const
{
	if (strId.empty())
		return nullptr;

	for (const auto& tWidget : m_Doc.vWidgets)
	{
		if (tWidget.strId == strId)
			return &tWidget;
	}

	return nullptr;
}

_string CPanel_UITool::Make_NextWidgetId() const
{
	for (_int iNumber = 1;; ++iNumber)
	{
		char szBuffer[32] = {};
		sprintf_s(szBuffer, "widget_%03d", iNumber);

		_bool bUsed = false;
		for (const auto& tWidget : m_Doc.vWidgets)
		{
			if (tWidget.strId == szBuffer)
			{
				bUsed = true;
				break;
			}
		}

		if (!bUsed)
			return szBuffer;
	}
}

_wstring CPanel_UITool::Make_UniqueAnimationName(const UISEQ_WIDGET_NODE& tWidget, const _wstring& strBase, _int iSkipIndex) const
{
	_wstring strCandidate = strBase.empty() ? L"Anim" : strBase;

	auto IsDuplicate = [&](const _wstring& strName) -> _bool
		{
			for (_int i = 0; i < static_cast<_int>(tWidget.vAnimations.size()); ++i)
			{
				if (i == iSkipIndex)
					continue;

				if (tWidget.vAnimations[i].strName == strName)
					return true;
			}

			return false;
		};

	if (!IsDuplicate(strCandidate))
		return strCandidate;

	for (_int iSuffix = 1;; ++iSuffix)
	{
		wchar_t szBuffer[64] = {};
		swprintf_s(szBuffer, L"%s_%03d", strCandidate.c_str(), iSuffix);
		if (!IsDuplicate(szBuffer))
			return szBuffer;
	}
}

_string CPanel_UITool::Make_NextCallbackId() const
{
	for (_int iNumber = 1;; ++iNumber)
	{
		char szBuffer[32] = {};
		sprintf_s(szBuffer, "callback_%03d", iNumber);

		_bool bUsed = false;
		for (const auto& tStep : m_Doc.vSteps)
		{
			if (tStep.strCallbackId == szBuffer)
			{
				bUsed = true;
				break;
			}
		}

		if (!bUsed)
			return szBuffer;
	}
}

UISEQ_WIDGET_NODE CPanel_UITool::Make_DefaultWidget(UI_TYPE eType) const
{
	auto InitializeBase = [](auto& tDesc, _float fSizeX, _float fSizeY)
		{
			tDesc.fCenterX = static_cast<_float>(g_iWinSizeX) * 0.5f;
			tDesc.fCenterY = static_cast<_float>(g_iWinSizeY) * 0.5f;
			tDesc.fSizeX = fSizeX;
			tDesc.fSizeY = fSizeY;
			tDesc.iZOrder = 0;
			tDesc.bVisible = true;
			tDesc.tAnchorDesc = {};
			tDesc.tAnchorDesc.eAnchor = UI_ANCHOR::MC;
			tDesc.tAnchorDesc.bUseAnchoredPos = false;
			tDesc.tLayoutSlot = {};
			tDesc.pParentUI = nullptr;
		};

	UISEQ_WIDGET_NODE tWidget{};
	tWidget.strId = Make_NextWidgetId();

	switch (eType)
	{
	case UI_TYPE::IMAGE:
	{
		CUIImage::UIIMAGE_DESC tDesc{};
		InitializeBase(tDesc, 200.f, 200.f);
		tDesc.strShaderTag = PROTO_COM_SHADER_UI;
		tDesc.iShaderLevel = ETOUI(LEVEL::STATIC);
		tDesc.strVIBufferTag = PROTO_COM_VIBUFFER_RECT;
		tDesc.iVIBufferLevel = ETOUI(LEVEL::STATIC);
		tDesc.strTextureTag = INVALID_TAG;
		tDesc.iTextureLevel = ETOUI(LEVEL::STATIC);
		tDesc.iTextureIndex = 0;
		tDesc.vColor = g_kWhite;
		tWidget.strDisplayName = "Image";
		tWidget.tDesc = tDesc;
		break;
	}

	case UI_TYPE::TEXT:
	{
		CUIText::UITEXT_DESC tDesc{};
		InitializeBase(tDesc, 240.f, 48.f);
		tDesc.strText = L"New Text";
		tDesc.strFontTag = FONT_MALGUN;
		tDesc.eAlign = UI_TEXT_ALIGN::LEFT;
		tDesc.vColor = g_kWhite;
		tWidget.strDisplayName = "Text";
		tWidget.tDesc = tDesc;
		break;
	}

	case UI_TYPE::BUTTON:
	{
		CUIButton::UIBUTTON_DESC tDesc{};
		InitializeBase(tDesc, 200.f, 64.f);
		tDesc.strShaderTag = PROTO_COM_SHADER_UI;
		tDesc.iShaderLevel = ETOUI(LEVEL::STATIC);
		tDesc.strVIBufferTag = PROTO_COM_VIBUFFER_RECT;
		tDesc.iVIBufferLevel = ETOUI(LEVEL::STATIC);
		tDesc.strTextureTag = INVALID_TAG;
		tDesc.iTextureLevel = ETOUI(LEVEL::STATIC);
		tDesc.iNormalTextureIndex = 0;
		tDesc.iHoverTextureIndex = 0;
		tDesc.iPressedTextureIndex = 0;
		tDesc.iDisabledTextureIndex = 0;
		tDesc.bInteractable = true;
		tDesc.vColor = g_kWhite;
		tWidget.strDisplayName = "Button";
		tWidget.tDesc = tDesc;
		break;
	}

	case UI_TYPE::PROGRESSBAR:
	{
		CUIProgressBar::UIPROGRESSBAR_DESC tDesc{};
		InitializeBase(tDesc, 240.f, 24.f);
		tDesc.strShaderTag = PROTO_COM_SHADER_UI;
		tDesc.iShaderLevel = ETOUI(LEVEL::STATIC);
		tDesc.strVIBufferTag = PROTO_COM_VIBUFFER_RECT;
		tDesc.iVIBufferLevel = ETOUI(LEVEL::STATIC);
		tDesc.strBackTextureTag = INVALID_TAG;
		tDesc.iBackTextureLevel = ETOUI(LEVEL::STATIC);
		tDesc.iBackTextureIndex = 0;
		tDesc.strFillTextureTag = INVALID_TAG;
		tDesc.iFillTextureLevel = ETOUI(LEVEL::STATIC);
		tDesc.iFillTextureIndex = 0;
		tDesc.vBackColor = g_kWhite;
		tDesc.vFillColor = g_kWhite;
		tDesc.fFillAmount = 1.f;
		tDesc.eDirection = CUIProgressBar::UI_PROGRESS_DIR::LEFT_TO_RIGHT;
		tWidget.strDisplayName = "ProgressBar";
		tWidget.tDesc = tDesc;
		break;
	}

	case UI_TYPE::CONTAINER:
	default:
	{
		CUIContainer::UICONTAINER_DESC tDesc{};
		InitializeBase(tDesc, 320.f, 180.f);
		tDesc.tLayoutDesc = {};
		tDesc.tLayoutDesc.eLayout = UI_LAYOUT::NONE;
		tWidget.strDisplayName = "Container";
		tWidget.tDesc = tDesc;
		break;
	}
	}

	return tWidget;
}

void CPanel_UITool::Duplicate_Widget(_int iWidgetIndex)
{
	if (iWidgetIndex < 0 || iWidgetIndex >= static_cast<_int>(m_Doc.vWidgets.size()))
		return;

	UISEQ_WIDGET_NODE tCopy = m_Doc.vWidgets[iWidgetIndex];
	tCopy.strId = Make_NextWidgetId();
	tCopy.strDisplayName += " Copy";

	m_Doc.vWidgets.insert(m_Doc.vWidgets.begin() + iWidgetIndex + 1, std::move(tCopy));
	m_iSelectedWidget = iWidgetIndex + 1;
	Normalize_Selection();
	Mark_Dirty("Widget duplicated");
}

void CPanel_UITool::Erase_Widget(_int iWidgetIndex)
{
	if (iWidgetIndex < 0 || iWidgetIndex >= static_cast<_int>(m_Doc.vWidgets.size()))
		return;

	const _string strTargetId = m_Doc.vWidgets[iWidgetIndex].strId;
	m_Doc.vWidgets.erase(m_Doc.vWidgets.begin() + iWidgetIndex);

	for (auto& tStep : m_Doc.vSteps)
	{
		if (tStep.strTargetId != strTargetId)
			continue;

		tStep.strTargetId.clear();
		if (UI_SEQ_STEP_KIND::PLAY_ANIM == tStep.eKind)
			tStep.strAnimName.clear();
	}

	Normalize_Selection();
	Mark_Dirty("Widget deleted");
}

_bool CPanel_UITool::Sanitize_DocReferences()
{
	_bool bChanged = false;

	for (_int iStep = 0; iStep < static_cast<_int>(m_Doc.vSteps.size()); ++iStep)
	{
		auto& tStep = m_Doc.vSteps[iStep];

		if (0 == iStep && tStep.bJoinPrev)
		{
			tStep.bJoinPrev = false;
			bChanged = true;
		}

		switch (tStep.eKind)
		{
		case UI_SEQ_STEP_KIND::PLAY_ANIM:
		{
			const UISEQ_WIDGET_NODE* pWidget = Find_WidgetById(tStep.strTargetId);
			if (nullptr == pWidget)
			{
				if (!tStep.strTargetId.empty() || !tStep.strAnimName.empty())
					bChanged = true;

				tStep.strTargetId.clear();
				tStep.strAnimName.clear();
				break;
			}

			_bool bHasAnimation = false;
			for (const auto& tAnimation : pWidget->vAnimations)
			{
				if (tAnimation.strName == tStep.strAnimName)
				{
					bHasAnimation = true;
					break;
				}
			}

			if (!bHasAnimation && !tStep.strAnimName.empty())
			{
				tStep.strAnimName.clear();
				bChanged = true;
			}
			break;
		}

		case UI_SEQ_STEP_KIND::SET_VISIBLE:
			if (nullptr == Find_WidgetById(tStep.strTargetId) && !tStep.strTargetId.empty())
			{
				tStep.strTargetId.clear();
				bChanged = true;
			}
			break;

		default:
			break;
		}
	}

	return bChanged;
}

_bool CPanel_UITool::Apply_StepTargetFallback(UISEQ_STEP_NODE& tStep) const
{
	_bool bChanged = false;

	if (UI_SEQ_STEP_KIND::PLAY_ANIM != tStep.eKind)
	{
		if (UI_SEQ_STEP_KIND::SET_VISIBLE == tStep.eKind && nullptr == Find_WidgetById(tStep.strTargetId))
		{
			if (!tStep.strTargetId.empty())
			{
				tStep.strTargetId.clear();
				bChanged = true;
			}
		}

		return bChanged;
	}

	const UISEQ_WIDGET_NODE* pWidget = Find_WidgetById(tStep.strTargetId);
	if (nullptr == pWidget)
	{
		if (!tStep.strTargetId.empty() || !tStep.strAnimName.empty())
			bChanged = true;

		tStep.strTargetId.clear();
		tStep.strAnimName.clear();
		return bChanged;
	}

	for (const auto& tAnimation : pWidget->vAnimations)
	{
		if (tAnimation.strName == tStep.strAnimName)
			return bChanged;
	}

	const _wstring strFallback = pWidget->vAnimations.empty() ? L"" : pWidget->vAnimations.front().strName;
	if (tStep.strAnimName != strFallback)
	{
		tStep.strAnimName = strFallback;
		bChanged = true;
	}

	return bChanged;
}

CUITween::UITWEEN_DESC CPanel_UITool::Make_DefaultTrack(const UISEQ_WIDGET_NODE& tWidget) const
{
	CUITween::UITWEEN_DESC tTrack{};
	tTrack.eTarget = UI_TWEEN_TARGET::SIZE_X;
	tTrack.fStart = 0.f;
	tTrack.fEnd = 1.f;
	tTrack.fDuration = 0.25f;
	tTrack.eEase = UI_EASE::LINEAR;
	tTrack.eLoop = UI_TWEEN_LOOP::NONE;

	CUIObject* pSentinel = Resolve_Sentinel(tWidget.Get_Type());
	if (nullptr == pSentinel)
		return tTrack;

	for (_int iTarget = 0; iTarget < static_cast<_int>(UI_TWEEN_TARGET::END); ++iTarget)
	{
		const UI_TWEEN_TARGET eTarget = static_cast<UI_TWEEN_TARGET>(iTarget);
		if (pSentinel->Can_Apply_Tween_Target(eTarget))
		{
			tTrack.eTarget = eTarget;
			break;
		}
	}

	return tTrack;
}

UISEQ_STEP_NODE CPanel_UITool::Make_DefaultStep(UI_SEQ_STEP_KIND eKind, _bool bJoinPrev) const
{
	UISEQ_STEP_NODE tStep{};
	tStep.eKind = eKind;
	tStep.bJoinPrev = bJoinPrev && !m_Doc.vSteps.empty();

	const UISEQ_WIDGET_NODE* pWidget = Get_SelectedWidget();

	switch (eKind)
	{
	case UI_SEQ_STEP_KIND::PLAY_ANIM:
		if (nullptr != pWidget)
		{
			tStep.strTargetId = pWidget->strId;
			if (!pWidget->vAnimations.empty())
				tStep.strAnimName = pWidget->vAnimations.front().strName;
		}
		break;

	case UI_SEQ_STEP_KIND::SET_VISIBLE:
		if (nullptr != pWidget)
			tStep.strTargetId = pWidget->strId;
		tStep.bVisible = true;
		break;

	case UI_SEQ_STEP_KIND::WAIT:
		tStep.fWaitSec = 0.25f;
		break;

	case UI_SEQ_STEP_KIND::USE_CALLBACK:
		tStep.strCallbackId = Make_NextCallbackId();
		break;

	default:
		break;
	}

	return tStep;
}

void CPanel_UITool::Draw_Toolbar()
{
	if (ImGui::Button("New Doc"))
	{
		Reset_Doc();
		m_strStatus = "New document";
	}

	ImGui::SameLine();
	if (ImGui::Button("Load"))
	{
		UISEQ_DOC tLoadedDoc{};
		if (SUCCEEDED(m_pEditInstance->Load_UISequence(m_strCurrentPath, tLoadedDoc)))
		{
			m_Doc = std::move(tLoadedDoc);
			Normalize_Selection();
			Sanitize_DocReferences();
			m_bDirty = false;
			m_strStatus = "Loaded";
		}
		else
		{
			m_strStatus = "Load failed";
		}
	}

	ImGui::SameLine();
	if (ImGui::Button("Save"))
	{
		Sanitize_DocReferences();
		if (SUCCEEDED(m_pEditInstance->Save_UISequence(m_strCurrentPath, m_Doc)))
		{
			m_bDirty = false;
			m_strStatus = "Saved";
		}
		else
		{
			m_strStatus = "Save failed";
		}
	}

	ImGui::SameLine();
	ImGui::Text("%s%s", m_strCurrentPath.c_str(), m_bDirty ? " *" : "");
	ImGui::Text("Status: %s", m_strStatus.c_str());
}

void CPanel_UITool::Draw_Hierarchy()
{
	ImGui::TextUnformatted("Hierarchy");

	auto AddWidget = [&](UI_TYPE eType)
		{
			m_Doc.vWidgets.push_back(Make_DefaultWidget(eType));
			m_iSelectedWidget = static_cast<_int>(m_Doc.vWidgets.size()) - 1;
			m_iSelectedAnimation = -1;
			m_iSelectedTrack = -1;
			Normalize_Selection();
			Mark_Dirty("Widget added");
		};

	if (ImGui::Button("Add Image")) AddWidget(UI_TYPE::IMAGE);
	ImGui::SameLine();
	if (ImGui::Button("Add Text")) AddWidget(UI_TYPE::TEXT);
	ImGui::SameLine();
	if (ImGui::Button("Add Button")) AddWidget(UI_TYPE::BUTTON);
	ImGui::SameLine();
	if (ImGui::Button("Add ProgressBar")) AddWidget(UI_TYPE::PROGRESSBAR);

	if (ImGui::Button("Delete")) Erase_Widget(m_iSelectedWidget);
	ImGui::SameLine();
	if (ImGui::Button("Duplicate")) Duplicate_Widget(m_iSelectedWidget);
	ImGui::SameLine();
	if (ImGui::Button("Up") && m_iSelectedWidget > 0)
	{
		std::swap(m_Doc.vWidgets[m_iSelectedWidget], m_Doc.vWidgets[m_iSelectedWidget - 1]);
		--m_iSelectedWidget;
		Mark_Dirty("Widget order changed");
	}
	ImGui::SameLine();
	if (ImGui::Button("Down") && m_iSelectedWidget >= 0 && m_iSelectedWidget + 1 < static_cast<_int>(m_Doc.vWidgets.size()))
	{
		std::swap(m_Doc.vWidgets[m_iSelectedWidget], m_Doc.vWidgets[m_iSelectedWidget + 1]);
		++m_iSelectedWidget;
		Mark_Dirty("Widget order changed");
	}

	ImGui::Separator();

	if (ImGui::BeginChild("HierarchyList", ImVec2(0.f, 0.f), false))
	{
		for (_int iWidget = 0; iWidget < static_cast<_int>(m_Doc.vWidgets.size()); ++iWidget)
		{
			const _string strLabel = Make_WidgetListLabel(m_Doc.vWidgets[iWidget]) + "##hierarchy_" + std::to_string(iWidget);
			if (ImGui::Selectable(strLabel.c_str(), m_iSelectedWidget == iWidget))
			{
				m_iSelectedWidget = iWidget;
				Normalize_Selection();
			}
		}
	}
	ImGui::EndChild();
}

void CPanel_UITool::Draw_Timeline()
{
	ImGui::TextUnformatted("Timeline");

	auto AppendStep = [&](UI_SEQ_STEP_KIND eKind, _bool bJoinPrev)
		{
			m_Doc.vSteps.push_back(Make_DefaultStep(eKind, bJoinPrev));
			m_iSelectedStep = static_cast<_int>(m_Doc.vSteps.size()) - 1;
			Normalize_Selection();
			Mark_Dirty("Step added");
		};

	if (ImGui::Button("Append Play")) AppendStep(UI_SEQ_STEP_KIND::PLAY_ANIM, false);
	ImGui::SameLine();
	if (ImGui::Button("Append Visible")) AppendStep(UI_SEQ_STEP_KIND::SET_VISIBLE, false);
	ImGui::SameLine();
	if (ImGui::Button("Append Wait")) AppendStep(UI_SEQ_STEP_KIND::WAIT, false);
	ImGui::SameLine();
	if (ImGui::Button("Append Callback")) AppendStep(UI_SEQ_STEP_KIND::USE_CALLBACK, false);

	if (ImGui::Button("Join Play")) AppendStep(UI_SEQ_STEP_KIND::PLAY_ANIM, true);
	ImGui::SameLine();
	if (ImGui::Button("Join Visible")) AppendStep(UI_SEQ_STEP_KIND::SET_VISIBLE, true);
	ImGui::SameLine();
	if (ImGui::Button("Join Wait")) AppendStep(UI_SEQ_STEP_KIND::WAIT, true);
	ImGui::SameLine();
	if (ImGui::Button("Join Callback")) AppendStep(UI_SEQ_STEP_KIND::USE_CALLBACK, true);

	if (ImGui::Button("Delete") && m_iSelectedStep >= 0)
	{
		m_Doc.vSteps.erase(m_Doc.vSteps.begin() + m_iSelectedStep);
		Normalize_Selection();
		Sanitize_DocReferences();
		Mark_Dirty("Step deleted");
	}

	ImGui::SameLine();
	if (ImGui::Button("Up") && m_iSelectedStep > 0)
	{
		std::swap(m_Doc.vSteps[m_iSelectedStep], m_Doc.vSteps[m_iSelectedStep - 1]);
		--m_iSelectedStep;
		Sanitize_DocReferences();
		Mark_Dirty("Step order changed");
	}

	ImGui::SameLine();
	if (ImGui::Button("Down") && m_iSelectedStep >= 0 && m_iSelectedStep + 1 < static_cast<_int>(m_Doc.vSteps.size()))
	{
		std::swap(m_Doc.vSteps[m_iSelectedStep], m_Doc.vSteps[m_iSelectedStep + 1]);
		++m_iSelectedStep;
		Sanitize_DocReferences();
		Mark_Dirty("Step order changed");
	}

	ImGui::Separator();

	if (ImGui::BeginChild("TimelineList", ImVec2(0.f, 140.f), true))
	{
		for (_int iStep = 0; iStep < static_cast<_int>(m_Doc.vSteps.size()); ++iStep)
		{
			const _string strLabel = Make_StepLabel(iStep, m_Doc.vSteps[iStep]) + "##timeline_" + std::to_string(iStep);
			if (ImGui::Selectable(strLabel.c_str(), m_iSelectedStep == iStep))
				m_iSelectedStep = iStep;
		}
	}
	ImGui::EndChild();

	UISEQ_STEP_NODE* pStep = Get_SelectedStep();
	if (nullptr == pStep)
	{
		ImGui::TextUnformatted("Select a timeline step.");
		return;
	}

	_int iKind = static_cast<_int>(pStep->eKind);
	if (iKind < 0 || iKind >= IM_ARRAYSIZE(g_ppStepKindNames))
		iKind = 0;

	if (ImGui::Combo("Kind", &iKind, g_ppStepKindNames, IM_ARRAYSIZE(g_ppStepKindNames)))
	{
		const _bool bJoinPrev = pStep->bJoinPrev;
		*pStep = Make_DefaultStep(static_cast<UI_SEQ_STEP_KIND>(iKind), bJoinPrev);
		if (m_iSelectedStep == 0)
			pStep->bJoinPrev = false;
		Mark_Dirty("Step kind changed");
	}

	_bool bJoinPrev = pStep->bJoinPrev;
	if (m_iSelectedStep == 0)
	{
		bJoinPrev = false;
		ImGui::BeginDisabled();
	}

	if (ImGui::Checkbox("Join Prev", &bJoinPrev))
	{
		pStep->bJoinPrev = bJoinPrev;
		Mark_Dirty("Step updated");
	}

	if (m_iSelectedStep == 0)
		ImGui::EndDisabled();

	switch (pStep->eKind)
	{
	case UI_SEQ_STEP_KIND::PLAY_ANIM:
	{
		const char* pszTargetPreview = pStep->strTargetId.empty() ? "<None>" : pStep->strTargetId.c_str();
		if (ImGui::BeginCombo("Target Widget", pszTargetPreview))
		{
			_bool bSelected = pStep->strTargetId.empty();
			if (ImGui::Selectable("<None>", bSelected))
			{
				pStep->strTargetId.clear();
				pStep->strAnimName.clear();
				Mark_Dirty("Step target changed");
			}

			for (const auto& tWidget : m_Doc.vWidgets)
			{
				const _bool bItemSelected = (pStep->strTargetId == tWidget.strId);
				if (ImGui::Selectable(tWidget.strId.c_str(), bItemSelected))
				{
					pStep->strTargetId = tWidget.strId;
					Apply_StepTargetFallback(*pStep);
					Mark_Dirty("Step target changed");
				}
			}

			ImGui::EndCombo();
		}

		const UISEQ_WIDGET_NODE* pTargetWidget = Find_WidgetById(pStep->strTargetId);
		const _string strAnimPreview = pStep->strAnimName.empty() ? _string("<None>") : WtoS(pStep->strAnimName);
		if (ImGui::BeginCombo("Animation", strAnimPreview.c_str()))
		{
			if (nullptr != pTargetWidget)
			{
				_bool bEmptySelected = pStep->strAnimName.empty();
				if (ImGui::Selectable("<None>", bEmptySelected))
				{
					pStep->strAnimName.clear();
					Mark_Dirty("Step animation changed");
				}

				for (const auto& tAnimation : pTargetWidget->vAnimations)
				{
					const _string strName = WtoS(tAnimation.strName);
					const _bool bAnimSelected = (pStep->strAnimName == tAnimation.strName);
					if (ImGui::Selectable(strName.c_str(), bAnimSelected))
					{
						pStep->strAnimName = tAnimation.strName;
						Mark_Dirty("Step animation changed");
					}
				}
			}

			ImGui::EndCombo();
		}
		break;
	}

	case UI_SEQ_STEP_KIND::SET_VISIBLE:
	{
		const char* pszTargetPreview = pStep->strTargetId.empty() ? "<None>" : pStep->strTargetId.c_str();
		if (ImGui::BeginCombo("Target Widget", pszTargetPreview))
		{
			_bool bSelected = pStep->strTargetId.empty();
			if (ImGui::Selectable("<None>", bSelected))
			{
				pStep->strTargetId.clear();
				Mark_Dirty("Step target changed");
			}

			for (const auto& tWidget : m_Doc.vWidgets)
			{
				const _bool bItemSelected = (pStep->strTargetId == tWidget.strId);
				if (ImGui::Selectable(tWidget.strId.c_str(), bItemSelected))
				{
					pStep->strTargetId = tWidget.strId;
					Mark_Dirty("Step target changed");
				}
			}

			ImGui::EndCombo();
		}

		if (ImGui::Checkbox("Visible", &pStep->bVisible))
			Mark_Dirty("Step updated");
		break;
	}

	case UI_SEQ_STEP_KIND::WAIT:
		if (ImGui::DragFloat("Wait Sec", &pStep->fWaitSec, 0.01f, 0.f, 60.f, "%.2f"))
			Mark_Dirty("Step updated");
		break;

	case UI_SEQ_STEP_KIND::USE_CALLBACK:
		if (Edit_StringField<256>("Callback Id", pStep->strCallbackId))
			Mark_Dirty("Step updated");
		break;

	default:
		break;
	}
}

void CPanel_UITool::Draw_Inspector()
{
	ImGui::TextUnformatted("Inspector");

	UISEQ_WIDGET_NODE* pWidget = Get_SelectedWidget();
	if (nullptr == pWidget)
	{
		ImGui::TextUnformatted("Select a widget.");
		return;
	}

	auto& tBase = Get_BaseDesc(*pWidget);
	Draw_ReadOnlyString<256>("Id", pWidget->strId);

	if (Edit_StringField<256>("Display Name", pWidget->strDisplayName))
		Mark_Dirty("Widget updated");

	if (ImGui::Checkbox("Visible", &tBase.bVisible))
		Mark_Dirty("Widget updated");

	if (ImGui::DragInt("Z Order", &tBase.iZOrder, 1.f))
		Mark_Dirty("Widget updated");

	if (ImGui::DragFloat("Size X", &tBase.fSizeX, 1.f, 1.f, static_cast<_float>(g_iWinSizeX) * 4.f))
		Mark_Dirty("Widget updated");

	if (ImGui::DragFloat("Size Y", &tBase.fSizeY, 1.f, 1.f, static_cast<_float>(g_iWinSizeY) * 4.f))
		Mark_Dirty("Widget updated");

	if (ImGui::Checkbox("Use Anchored Pos", &tBase.tAnchorDesc.bUseAnchoredPos))
		Mark_Dirty("Widget updated");

	if (tBase.tAnchorDesc.bUseAnchoredPos)
	{
		_int iAnchor = static_cast<_int>(tBase.tAnchorDesc.eAnchor);
		if (iAnchor < 0 || iAnchor >= IM_ARRAYSIZE(g_ppAnchorNames))
			iAnchor = static_cast<_int>(UI_ANCHOR::MC);

		if (ImGui::Combo("Anchor", &iAnchor, g_ppAnchorNames, IM_ARRAYSIZE(g_ppAnchorNames)))
		{
			tBase.tAnchorDesc.eAnchor = static_cast<UI_ANCHOR>(iAnchor);
			Mark_Dirty("Widget updated");
		}

		if (ImGui::DragFloat("Offset X", &tBase.tAnchorDesc.fOffsetX, 1.f))
			Mark_Dirty("Widget updated");

		if (ImGui::DragFloat("Offset Y", &tBase.tAnchorDesc.fOffsetY, 1.f))
			Mark_Dirty("Widget updated");
	}
	else
	{
		if (ImGui::DragFloat("Center X", &tBase.fCenterX, 1.f))
			Mark_Dirty("Widget updated");

		if (ImGui::DragFloat("Center Y", &tBase.fCenterY, 1.f))
			Mark_Dirty("Widget updated");
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
		Mark_Dirty("Widget updated");
	}

	if (ImGui::DragFloat("Desired X", &tBase.tLayoutSlot.fDesiredSizeX, 1.f))
		Mark_Dirty("Widget updated");

	if (ImGui::DragFloat("Desired Y", &tBase.tLayoutSlot.fDesiredSizeY, 1.f))
		Mark_Dirty("Widget updated");

	ImGui::Separator();

	std::visit([&](auto& tDesc)
		{
			using T = std::decay_t<decltype(tDesc)>;

			if constexpr (std::is_same_v<T, CUIImage::UIIMAGE_DESC>)
			{
				if (Edit_TagField<256>("Texture Tag", tDesc.strTextureTag)) Mark_Dirty("Widget updated");
				if (Edit_UIntField("Texture Level", tDesc.iTextureLevel)) Mark_Dirty("Widget updated");
				if (Edit_UIntField("Texture Index", tDesc.iTextureIndex)) Mark_Dirty("Widget updated");
				if (Edit_TagField<256>("Shader Tag", tDesc.strShaderTag)) Mark_Dirty("Widget updated");
				if (Edit_UIntField("Shader Level", tDesc.iShaderLevel)) Mark_Dirty("Widget updated");
				if (Edit_TagField<256>("VIBuffer Tag", tDesc.strVIBufferTag)) Mark_Dirty("Widget updated");
				if (Edit_UIntField("VIBuffer Level", tDesc.iVIBufferLevel)) Mark_Dirty("Widget updated");
				if (ImGui::ColorEdit4("Color", reinterpret_cast<float*>(&tDesc.vColor))) Mark_Dirty("Widget updated");
			}
			else if constexpr (std::is_same_v<T, CUIButton::UIBUTTON_DESC>)
			{
				if (Edit_TagField<256>("Texture Tag", tDesc.strTextureTag)) Mark_Dirty("Widget updated");
				if (Edit_UIntField("Texture Level", tDesc.iTextureLevel)) Mark_Dirty("Widget updated");
				if (Edit_TagField<256>("Shader Tag", tDesc.strShaderTag)) Mark_Dirty("Widget updated");
				if (Edit_UIntField("Shader Level", tDesc.iShaderLevel)) Mark_Dirty("Widget updated");
				if (Edit_TagField<256>("VIBuffer Tag", tDesc.strVIBufferTag)) Mark_Dirty("Widget updated");
				if (Edit_UIntField("VIBuffer Level", tDesc.iVIBufferLevel)) Mark_Dirty("Widget updated");
				if (Edit_UIntField("Normal Index", tDesc.iNormalTextureIndex)) Mark_Dirty("Widget updated");
				if (Edit_UIntField("Hover Index", tDesc.iHoverTextureIndex)) Mark_Dirty("Widget updated");
				if (Edit_UIntField("Pressed Index", tDesc.iPressedTextureIndex)) Mark_Dirty("Widget updated");
				if (Edit_UIntField("Disabled Index", tDesc.iDisabledTextureIndex)) Mark_Dirty("Widget updated");
				if (ImGui::Checkbox("Interactable", &tDesc.bInteractable)) Mark_Dirty("Widget updated");
				if (ImGui::ColorEdit4("Color", reinterpret_cast<float*>(&tDesc.vColor))) Mark_Dirty("Widget updated");
			}
			else if constexpr (std::is_same_v<T, CUIText::UITEXT_DESC>)
			{
				if (Edit_WStringMultiline<1024>("Text", tDesc.strText, ImVec2(0.f, 70.f))) Mark_Dirty("Widget updated");
				if (Edit_TagField<256>("Font Tag", tDesc.strFontTag)) Mark_Dirty("Widget updated");

				_int iAlign = static_cast<_int>(tDesc.eAlign);
				if (iAlign < 0 || iAlign >= IM_ARRAYSIZE(g_ppTextAlignNames))
					iAlign = 0;

				if (ImGui::Combo("Align", &iAlign, g_ppTextAlignNames, IM_ARRAYSIZE(g_ppTextAlignNames)))
				{
					tDesc.eAlign = static_cast<UI_TEXT_ALIGN>(iAlign);
					Mark_Dirty("Widget updated");
				}

				if (ImGui::ColorEdit4("Color", reinterpret_cast<float*>(&tDesc.vColor))) Mark_Dirty("Widget updated");
			}
			else if constexpr (std::is_same_v<T, CUIProgressBar::UIPROGRESSBAR_DESC>)
			{
				if (Edit_TagField<256>("Back Texture Tag", tDesc.strBackTextureTag)) Mark_Dirty("Widget updated");
				if (Edit_UIntField("Back Texture Level", tDesc.iBackTextureLevel)) Mark_Dirty("Widget updated");
				if (Edit_UIntField("Back Texture Index", tDesc.iBackTextureIndex)) Mark_Dirty("Widget updated");
				if (Edit_TagField<256>("Fill Texture Tag", tDesc.strFillTextureTag)) Mark_Dirty("Widget updated");
				if (Edit_UIntField("Fill Texture Level", tDesc.iFillTextureLevel)) Mark_Dirty("Widget updated");
				if (Edit_UIntField("Fill Texture Index", tDesc.iFillTextureIndex)) Mark_Dirty("Widget updated");
				if (Edit_TagField<256>("Shader Tag", tDesc.strShaderTag)) Mark_Dirty("Widget updated");
				if (Edit_UIntField("Shader Level", tDesc.iShaderLevel)) Mark_Dirty("Widget updated");
				if (Edit_TagField<256>("VIBuffer Tag", tDesc.strVIBufferTag)) Mark_Dirty("Widget updated");
				if (Edit_UIntField("VIBuffer Level", tDesc.iVIBufferLevel)) Mark_Dirty("Widget updated");
				if (ImGui::ColorEdit4("Back Color", reinterpret_cast<float*>(&tDesc.vBackColor))) Mark_Dirty("Widget updated");
				if (ImGui::ColorEdit4("Fill Color", reinterpret_cast<float*>(&tDesc.vFillColor))) Mark_Dirty("Widget updated");
				if (ImGui::SliderFloat("Fill Amount", &tDesc.fFillAmount, 0.f, 1.f)) Mark_Dirty("Widget updated");

				_int iDirection = static_cast<_int>(tDesc.eDirection);
				if (iDirection < 0 || iDirection >= IM_ARRAYSIZE(g_ppProgressDirNames))
					iDirection = 0;

				if (ImGui::Combo("Direction", &iDirection, g_ppProgressDirNames, IM_ARRAYSIZE(g_ppProgressDirNames)))
				{
					tDesc.eDirection = static_cast<CUIProgressBar::UI_PROGRESS_DIR>(iDirection);
					Mark_Dirty("Widget updated");
				}
			}
			else if constexpr (std::is_same_v<T, CUIContainer::UICONTAINER_DESC>)
			{
				_int iLayout = static_cast<_int>(tDesc.tLayoutDesc.eLayout);
				if (iLayout < 0 || iLayout >= IM_ARRAYSIZE(g_ppLayoutNames))
					iLayout = 0;

				if (ImGui::Combo("Layout Kind", &iLayout, g_ppLayoutNames, IM_ARRAYSIZE(g_ppLayoutNames)))
				{
					tDesc.tLayoutDesc.eLayout = static_cast<UI_LAYOUT>(iLayout);
					Mark_Dirty("Widget updated");
				}

				if (ImGui::DragFloat("Padding", &tDesc.tLayoutDesc.fPadding, 1.f)) Mark_Dirty("Widget updated");
				if (ImGui::DragFloat("Spacing", &tDesc.tLayoutDesc.fSpacing, 1.f)) Mark_Dirty("Widget updated");
			}
		}, pWidget->tDesc);

	ImGui::Separator();
	ImGui::TextUnformatted("Animations");

	if (ImGui::Button("Add Anim"))
	{
		_wstring strName;
		for (_int iAnim = 1;; ++iAnim)
		{
			wchar_t szBuffer[32] = {};
			swprintf_s(szBuffer, L"Anim_%03d", iAnim);
			strName = Make_UniqueAnimationName(*pWidget, szBuffer);
			if (strName == szBuffer)
				break;
		}

		pWidget->vAnimations.push_back({ strName, {} });
		m_iSelectedAnimation = static_cast<_int>(pWidget->vAnimations.size()) - 1;
		m_iSelectedTrack = -1;
		Mark_Dirty("Animation added");
	}

	ImGui::SameLine();
	if (ImGui::Button("Delete Anim") && m_iSelectedAnimation >= 0)
	{
		pWidget->vAnimations.erase(pWidget->vAnimations.begin() + m_iSelectedAnimation);
		Normalize_Selection();
		Sanitize_DocReferences();
		Mark_Dirty("Animation deleted");
	}

	if (ImGui::BeginChild("AnimationList", ImVec2(0.f, 90.f), true))
	{
		for (_int iAnim = 0; iAnim < static_cast<_int>(pWidget->vAnimations.size()); ++iAnim)
		{
			const _string strLabel = WtoS(pWidget->vAnimations[iAnim].strName) + "##anim_" + std::to_string(iAnim);
			if (ImGui::Selectable(strLabel.c_str(), m_iSelectedAnimation == iAnim))
			{
				m_iSelectedAnimation = iAnim;
				m_iSelectedTrack = -1;
				Normalize_Selection();
			}
		}
	}
	ImGui::EndChild();

	UISEQ_ANIMATION_NODE* pAnimation = Get_SelectedAnimation();
	if (nullptr != pAnimation)
	{
		const _wstring strOldName = pAnimation->strName;
		if (Edit_WStringField<256>("Animation Name", pAnimation->strName))
		{
			pAnimation->strName = Make_UniqueAnimationName(*pWidget, pAnimation->strName, m_iSelectedAnimation);
			if (strOldName != pAnimation->strName)
			{
				for (auto& tStep : m_Doc.vSteps)
				{
					if (tStep.eKind == UI_SEQ_STEP_KIND::PLAY_ANIM
						&& tStep.strTargetId == pWidget->strId
						&& tStep.strAnimName == strOldName)
					{
						tStep.strAnimName = pAnimation->strName;
					}
				}
			}
			Mark_Dirty("Animation renamed");
		}

		if (ImGui::Button("Add Track"))
		{
			pAnimation->vTracks.push_back(Make_DefaultTrack(*pWidget));
			m_iSelectedTrack = static_cast<_int>(pAnimation->vTracks.size()) - 1;
			Mark_Dirty("Track added");
		}

		ImGui::SameLine();
		if (ImGui::Button("Delete Track") && m_iSelectedTrack >= 0)
		{
			pAnimation->vTracks.erase(pAnimation->vTracks.begin() + m_iSelectedTrack);
			Normalize_Selection();
			Mark_Dirty("Track deleted");
		}

		if (ImGui::BeginChild("TrackList", ImVec2(0.f, 90.f), true))
		{
			for (_int iTrack = 0; iTrack < static_cast<_int>(pAnimation->vTracks.size()); ++iTrack)
			{
				const _string strLabel = std::to_string(iTrack) + " " + To_String(pAnimation->vTracks[iTrack].eTarget) + "##track_" + std::to_string(iTrack);
				if (ImGui::Selectable(strLabel.c_str(), m_iSelectedTrack == iTrack))
					m_iSelectedTrack = iTrack;
			}
		}
		ImGui::EndChild();

		CUITween::UITWEEN_DESC* pTrack = Get_SelectedTrack();
		if (nullptr != pTrack)
		{
			CUIObject* pSentinel = Resolve_Sentinel(pWidget->Get_Type());
			if (nullptr != pSentinel)
			{
				const char* pszPreview = To_String(pTrack->eTarget);
				if (ImGui::BeginCombo("Target", pszPreview))
				{
					for (_int iTarget = 0; iTarget < static_cast<_int>(UI_TWEEN_TARGET::END); ++iTarget)
					{
						const UI_TWEEN_TARGET eTarget = static_cast<UI_TWEEN_TARGET>(iTarget);
						if (!pSentinel->Can_Apply_Tween_Target(eTarget))
							continue;

						const _bool bSelected = (pTrack->eTarget == eTarget);
						if (ImGui::Selectable(To_String(eTarget), bSelected))
						{
							pTrack->eTarget = eTarget;
							Mark_Dirty("Track updated");
						}
					}
					ImGui::EndCombo();
				}
			}

			if (ImGui::DragFloat("Start", &pTrack->fStart, 0.01f)) Mark_Dirty("Track updated");
			if (ImGui::DragFloat("End", &pTrack->fEnd, 0.01f)) Mark_Dirty("Track updated");
			if (ImGui::DragFloat("Duration", &pTrack->fDuration, 0.01f, 0.f, 60.f)) Mark_Dirty("Track updated");

			_int iEase = static_cast<_int>(pTrack->eEase);
			if (iEase < 0 || iEase >= IM_ARRAYSIZE(g_ppEaseNames))
				iEase = 0;
			if (ImGui::Combo("Ease", &iEase, g_ppEaseNames, IM_ARRAYSIZE(g_ppEaseNames)))
			{
				pTrack->eEase = static_cast<UI_EASE>(iEase);
				Mark_Dirty("Track updated");
			}

			_int iLoop = static_cast<_int>(pTrack->eLoop);
			if (iLoop < 0 || iLoop >= IM_ARRAYSIZE(g_ppLoopNames))
				iLoop = 0;
			if (ImGui::Combo("Loop", &iLoop, g_ppLoopNames, IM_ARRAYSIZE(g_ppLoopNames)))
			{
				pTrack->eLoop = static_cast<UI_TWEEN_LOOP>(iLoop);
				Mark_Dirty("Track updated");
			}
		}
	}
}

void CPanel_UITool::Draw_Canvas()
{
	ImGui::TextUnformatted("Canvas");

	const ImVec2 vAvailable = ImGui::GetContentRegionAvail();
	const float fAspect = static_cast<float>(g_iWinSizeX) / static_cast<float>(g_iWinSizeY);
	const float fTargetHeight = 260.f;
	float fCanvasHeight = (std::min)(vAvailable.y, std::clamp(fTargetHeight, 220.f, 280.f));
	if (fCanvasHeight <= 1.f)
		fCanvasHeight = fTargetHeight;

	float fCanvasWidth = (std::min)(vAvailable.x, fCanvasHeight * fAspect);
	if (fCanvasWidth <= 1.f)
		fCanvasWidth = vAvailable.x;

	fCanvasHeight = fCanvasWidth / fAspect;
	const float fScale = fCanvasWidth / static_cast<float>(g_iWinSizeX);

	const ImVec2 vOrigin = ImGui::GetCursorScreenPos();
	ImDrawList* pDrawList = ImGui::GetWindowDrawList();
	pDrawList->AddRectFilled(vOrigin, ImVec2(vOrigin.x + fCanvasWidth, vOrigin.y + fCanvasHeight), IM_COL32(26, 30, 34, 255), 4.f);
	pDrawList->AddRect(vOrigin, ImVec2(vOrigin.x + fCanvasWidth, vOrigin.y + fCanvasHeight), IM_COL32(100, 110, 120, 255), 4.f);

	ImGui::InvisibleButton("##UICanvas", ImVec2(fCanvasWidth, fCanvasHeight));

	const ImVec2 vMouse = ImGui::GetIO().MousePos;
	const _float fDocMouseX = (vMouse.x - vOrigin.x) / (std::max)(fScale, 0.0001f);
	const _float fDocMouseY = (vMouse.y - vOrigin.y) / (std::max)(fScale, 0.0001f);

	auto HitTest = [&]() -> _int
		{
			_int iBestIndex = -1;
			_int iBestZ = std::numeric_limits<_int>::lowest();

			for (_int iWidget = 0; iWidget < static_cast<_int>(m_Doc.vWidgets.size()); ++iWidget)
			{
				const CANVAS_BOX tBox = Make_CanvasBox(m_Doc.vWidgets[iWidget]);
				if (!Is_PointInBox(tBox, fDocMouseX, fDocMouseY))
					continue;

				const _int iZOrder = Get_BaseDesc(m_Doc.vWidgets[iWidget]).iZOrder;
				if (iZOrder > iBestZ || (iZOrder == iBestZ && iWidget > iBestIndex))
				{
					iBestIndex = iWidget;
					iBestZ = iZOrder;
				}
			}

			return iBestIndex;
		};

	if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
	{
		m_iSelectedWidget = HitTest();
		Normalize_Selection();
		m_bCanvasDragging = false;
		m_iCanvasDragWidget = -1;

		if (m_iSelectedWidget >= 0)
		{
			auto& tBase = Get_BaseDesc(m_Doc.vWidgets[m_iSelectedWidget]);
			m_bCanvasDragging = true;
			m_iCanvasDragWidget = m_iSelectedWidget;
			m_vDragStartMouse = vMouse;
			m_vDragStartValue = tBase.tAnchorDesc.bUseAnchoredPos
				? _float2(tBase.tAnchorDesc.fOffsetX, tBase.tAnchorDesc.fOffsetY)
				: _float2(tBase.fCenterX, tBase.fCenterY);
		}
	}

	if (m_bCanvasDragging && m_iCanvasDragWidget >= 0 && m_iCanvasDragWidget < static_cast<_int>(m_Doc.vWidgets.size()))
	{
		if (ImGui::IsMouseDown(ImGuiMouseButton_Left))
		{
			auto& tBase = Get_BaseDesc(m_Doc.vWidgets[m_iCanvasDragWidget]);
			const ImVec2 vDelta = ImVec2(vMouse.x - m_vDragStartMouse.x, vMouse.y - m_vDragStartMouse.y);
			const _float fDeltaX = vDelta.x / (std::max)(fScale, 0.0001f);
			const _float fDeltaY = vDelta.y / (std::max)(fScale, 0.0001f);

			if (tBase.tAnchorDesc.bUseAnchoredPos)
			{
				tBase.tAnchorDesc.fOffsetX = m_vDragStartValue.x + fDeltaX;
				tBase.tAnchorDesc.fOffsetY = m_vDragStartValue.y + fDeltaY;
			}
			else
			{
				tBase.fCenterX = m_vDragStartValue.x + fDeltaX;
				tBase.fCenterY = m_vDragStartValue.y + fDeltaY;
			}

			Mark_Dirty("Canvas drag");
		}
		else
		{
			m_bCanvasDragging = false;
			m_iCanvasDragWidget = -1;
		}
	}

	vector<_int> vDrawOrder;
	vDrawOrder.reserve(m_Doc.vWidgets.size());
	for (_int iWidget = 0; iWidget < static_cast<_int>(m_Doc.vWidgets.size()); ++iWidget)
		vDrawOrder.push_back(iWidget);

	std::sort(vDrawOrder.begin(), vDrawOrder.end(),
		[&](const _int iLhs, const _int iRhs)
		{
			const _int iLeftZ = Get_BaseDesc(m_Doc.vWidgets[iLhs]).iZOrder;
			const _int iRightZ = Get_BaseDesc(m_Doc.vWidgets[iRhs]).iZOrder;
			if (iLeftZ != iRightZ)
				return iLeftZ < iRightZ;
			return iLhs < iRhs;
		});

	for (const _int iWidget : vDrawOrder)
	{
		const auto& tWidget = m_Doc.vWidgets[iWidget];
		const auto& tBase = Get_BaseDesc(tWidget);
		const CANVAS_BOX tBox = Make_CanvasBox(tWidget);

		const ImVec2 vMin(vOrigin.x + tBox.fLeft * fScale, vOrigin.y + tBox.fTop * fScale);
		const ImVec2 vMax(vMin.x + tBox.fWidth * fScale, vMin.y + tBox.fHeight * fScale);

		pDrawList->AddRectFilled(vMin, vMax, Get_WidgetColor(tWidget.Get_Type(), tBase.bVisible), 2.f);
		pDrawList->AddRect(vMin, vMax, (m_iSelectedWidget == iWidget) ? IM_COL32(255, 220, 80, 255) : IM_COL32(220, 230, 240, 180), 2.f, 0, (m_iSelectedWidget == iWidget) ? 2.f : 1.f);
		pDrawList->AddText(ImVec2(vMin.x + 4.f, vMin.y + 4.f), IM_COL32(255, 255, 255, 255), tWidget.strDisplayName.c_str());
	}
}

CPanel_UITool* CPanel_UITool::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPanel_UITool* pInstance = new CPanel_UITool(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Create : CPanel_UITool");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPanel_UITool::Free()
{
	Safe_Release(m_pProgressBarSentinel);
	Safe_Release(m_pButtonSentinel);
	Safe_Release(m_pTextSentinel);
	Safe_Release(m_pImageSentinel);
	Safe_Release(m_pContainerSentinel);

	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);

	__super::Free();
}
