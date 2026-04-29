#include "UIEditorSession.h"
#include "UIPreviewHost.h"

#include "GameInstance.h"
#include "EditInstance.h"

CUIEditorSession::CUIEditorSession(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
	, m_pGameInstance(CGameInstance::GetInstance())
	, m_pEditInstance(CEditInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pEditInstance);
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CUIEditorSession::Initialize()
{
	if (FAILED(Initialize_Sentinels()))
		return E_FAIL;

	Reset_Doc();
	return S_OK;
}

void CUIEditorSession::Update(_float fTimeDelta)
{
}

_int CUIEditorSession::Find_WidgetIndexById(const _string& strId) const
{
	for (_int i = 0; i < (_int)m_Doc.vWidgets.size(); ++i)
		if (m_Doc.vWidgets[i].strId == strId) return i;
	return -1;
}

_wstring CUIEditorSession::Make_UniqueAnimationName(const UISEQ_WIDGET_NODE& tWidget, const _wstring& strBase, _int iSkipIndex) const
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

void CUIEditorSession::Rename_Animation(UISEQ_WIDGET_NODE& w, _int iAnimIdx, const _wstring& strOldName, const _wstring& strNewName)
{
	if (iAnimIdx < 0 || iAnimIdx >= (_int)w.vAnimations.size()) return;

	const _wstring strNew = Make_UniqueAnimationName(w, strNewName, iAnimIdx);
	w.vAnimations[iAnimIdx].strName = strNew;

	if (strOldName != strNew)
	{
		for (auto& tStep : m_Doc.vSteps)
		{
			if (tStep.eKind == UI_SEQ_STEP_KIND::PLAY_ANIM
				&& tStep.strTargetId == w.strId
				&& tStep.strAnimName == strOldName)
			{
				tStep.strAnimName = strNew;
			}
		}
	}
	Mark_Dirty("Animation renamed");
}

HRESULT CUIEditorSession::Initialize_Sentinels()
{
	m_pSentinels[0] = CUIContainer::Create(m_pDevice, m_pContext);
	if (nullptr == m_pSentinels[0])
		return E_FAIL;

	m_pSentinels[1] = CUIImage::Create(m_pDevice, m_pContext);
	if (nullptr == m_pSentinels[1])
		return E_FAIL;

	m_pSentinels[2] = CUIText::Create(m_pDevice, m_pContext);
	if (nullptr == m_pSentinels[2])
		return E_FAIL;

	m_pSentinels[3] = CUIButton::Create(m_pDevice, m_pContext);
	if (nullptr == m_pSentinels[3])
		return E_FAIL;

	m_pSentinels[4] = CUIProgressBar::Create(m_pDevice, m_pContext);
	if (nullptr == m_pSentinels[4])
		return E_FAIL;

	return S_OK;
}

void CUIEditorSession::Reset_Doc()
{
	m_Doc = {};
	m_Doc.iVersion = 1;
	m_Doc.strName = "HUD_Layout";

	m_iSelectedWidget = -1;
	m_iSelectedAnimation = -1;
	m_iSelectedTrack = -1;
	m_iSelectedStep = -1;
	m_bDirty = false;
}

_string CUIEditorSession::Make_NextWidgetId() const
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

_string CUIEditorSession::Make_NextCallbackId() const
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

UISEQ_WIDGET_NODE CUIEditorSession::Make_DefaultWidget(UI_TYPE eType) const
{
	_float2 vRefSize = m_pGameInstance->Get_CurrentRefSize();

	auto InitializeBase = [vRefSize](auto& tDesc, _float fSizeX, _float fSizeY)
		{
			tDesc.fCenterX = vRefSize.x * 0.5f;
			tDesc.fCenterY = vRefSize.y * 0.5f;
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
		tDesc.eDirection = UI_PROGRESS_DIR::LEFT_TO_RIGHT;
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

CUITween::UITWEEN_DESC CUIEditorSession::Make_DefaultTrack(const UISEQ_WIDGET_NODE& tWidget) const
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

UISEQ_STEP_NODE CUIEditorSession::Make_DefaultStep(UI_SEQ_STEP_KIND eKind, _bool bJoinPrev) const
{
	UISEQ_STEP_NODE tStep{};
	tStep.eKind = eKind;
	tStep.bJoinPrev = bJoinPrev && !m_Doc.vSteps.empty();

	const UISEQ_WIDGET_NODE* pWidget = nullptr;
	if (m_iSelectedWidget >= 0 && m_iSelectedWidget < static_cast<_int>(m_Doc.vWidgets.size()))
		pWidget = &m_Doc.vWidgets[m_iSelectedWidget];

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

void CUIEditorSession::Duplicate_Widget(_int iWidgetIndex)
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

void CUIEditorSession::Erase_Widget(_int iWidgetIndex)
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

_bool CUIEditorSession::Sanitize_DocReferences()
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

CUIObject* CUIEditorSession::Resolve_Sentinel(UI_TYPE eType) const
{
	switch (eType)
	{
	case UI_TYPE::CONTAINER: return m_pSentinels[0];
	case UI_TYPE::IMAGE: return m_pSentinels[1];
	case UI_TYPE::TEXT: return m_pSentinels[2];
	case UI_TYPE::BUTTON: return m_pSentinels[3];
	case UI_TYPE::PROGRESSBAR: return m_pSentinels[4];
	default: return nullptr;
	}
}

void CUIEditorSession::Normalize_Selection()
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

void CUIEditorSession::Mark_Dirty(const char* pszReason)
{
	m_bDirty = true;
	m_strStatus = (nullptr != pszReason && '\0' != pszReason[0]) ? pszReason : "Modified";

	// host에 rebuild 신호
	if (auto* pHost = m_pEditInstance->Get_UIPreviewHost())
		pHost->Mark_Rebuild_Pending();
}

void CUIEditorSession::Mark_Dirty_Property(const char* pszReason)
{
	m_bDirty = true;
	m_strStatus = (pszReason && pszReason[0]) ? pszReason : "Modified";
	// host->Mark_Rebuild_Pending() 호출하지 않음
}

const UISEQ_WIDGET_NODE* CUIEditorSession::Find_WidgetById(const _string& strId) const
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

_bool CUIEditorSession::Apply_StepTargetFallback(UISEQ_STEP_NODE& tStep) const
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

HRESULT CUIEditorSession::Save(const _string& strPath)
{
	std::error_code ec;
	if (std::filesystem::exists(strPath, ec))
	{
		const _string strBackup = strPath + ".bak";
		std::filesystem::copy_file(strPath, strBackup,
			std::filesystem::copy_options::overwrite_existing, ec);
		// ec 무시 - 권한 문제 등은 status 표시 정도만
	}

	if (FAILED(m_pEditInstance->Save_UISequence(strPath, m_Doc)))
	{
		m_strStatus = "Save failed: " + strPath;
		return E_FAIL;
	}
	m_strDocPath = strPath;
	m_strStatus = "Saved: " + strPath;
	Clear_Dirty();
	return S_OK;
}

HRESULT CUIEditorSession::Load(const _string& strPath)
{
	UISEQ_DOC tLoaded{};
	if (FAILED(m_pEditInstance->Load_UISequence(strPath, tLoaded)))
	{
		m_strStatus = "Load failed: " + strPath;
		return E_FAIL;
	}
	m_Doc = std::move(tLoaded);
	m_strDocPath = strPath;
	m_iSelectedWidget = m_iSelectedAnimation = m_iSelectedTrack = m_iSelectedStep = -1;
	Sanitize_DocReferences();
	Normalize_Selection();
	Clear_Dirty();
	m_strStatus = "Loaded: " + strPath;

	if (auto* pHost = m_pEditInstance->Get_UIPreviewHost())
		pHost->Mark_Rebuild_Pending();

	return S_OK;
}

HRESULT CUIEditorSession::New_Doc()
{
	Reset_Doc();
	m_strStatus = "New document";

	if (auto* pHost = m_pEditInstance->Get_UIPreviewHost())
		pHost->Mark_Rebuild_Pending();

	return S_OK;
}

void CUIEditorSession::Clear_Dirty()
{
	m_bDirty = false;
}

CUIEditorSession* CUIEditorSession::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUIEditorSession* pInstance = new CUIEditorSession(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Create : CUIEditorSession");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUIEditorSession::Free()
{
	__super::Free();

	for (int i = 0; i < 5; ++i)
		Safe_Release(m_pSentinels[i]);

	Safe_Release(m_pEditInstance);
	Safe_Release(m_pGameInstance);
	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);
}
