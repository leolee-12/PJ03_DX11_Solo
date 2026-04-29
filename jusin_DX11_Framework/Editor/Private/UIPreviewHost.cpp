#include "UIPreviewHost.h"
#include "UIEditorSession.h"
#include "EditInstance.h"

#include "UISequence.h"
#include "UIAnimator.h"
#include "GameInstance.h"

CUIPreviewHost::CUIPreviewHost(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }
	, m_pContext{ pContext }
	, m_pGameInstance{ CGameInstance::GetInstance() }
	, m_pEditInstance{ CEditInstance::GetInstance() }
	, m_pSession{ m_pEditInstance->Get_UISession() }
{
	Safe_AddRef(m_pSession);
	Safe_AddRef(m_pEditInstance);
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pDevice);
}

const vector<CUIObject*>& CUIPreviewHost::Get_Widgets() const
{
	static const vector<CUIObject*> sEmpty;
	return m_pSequence ? m_pSequence->Get_Children() : sEmpty;
}

HRESULT CUIPreviewHost::Initialize()
{
	return S_OK;
}

void CUIPreviewHost::Tick(_float fTimeDelta)
{
	// 캔버스(게임 RT) 크기 변경 감지 - UI 레이아웃은 캔버스 기준이므로 패널 표시 크기는 트리거 대상 아님.
	const _float2 vCanvas = m_pGameInstance->Get_ViewportSize();
	const ImVec2 vNow(vCanvas.x, vCanvas.y);
	if (vNow.x != m_vLastViewportSize.x || vNow.y != m_vLastViewportSize.y)
	{
		m_vLastViewportSize = vNow;
		m_bRebuildPending = true;
	}

	if (m_eMode == UI_PREVIEW_MODE::SELECTED_ANIM)
	{
		const _int iW = m_pSession->Get_SelectedWidget();
		const _int iA = m_pSession->Get_SelectedAnimation();
		if (iW != m_iLastSelWidget || iA != m_iLastSelAnim)
		{
			if (m_eState != UI_PREVIEW_STATE::IDLE)
				Stop();
			m_iLastSelWidget = iW;
			m_iLastSelAnim = iA;
		}
	}

	if (nullptr == m_pSequence) return;

	// Sprite 진행 허용 여부 - 자식 일괄 적용
	const _bool bAllowSprite = (m_eMode != UI_PREVIEW_MODE::LAYOUT);
	for (CUIObject* p : m_pSequence->Get_Children())
		if (p) p->Set_SpriteTickAllowed(bAllowSprite);

	const _bool bPaused = (m_eState == UI_PREVIEW_STATE::PAUSED);

	m_pSequence->Priority_Update(fTimeDelta);
	if (!bPaused)
		m_pSequence->Update(fTimeDelta);
	// CUISequence::Update가 자식 재귀 + timeline 진행(m_bPlaying 기준)

// PLAYING -> IDLE 자동 전이
	if (m_eState == UI_PREVIEW_STATE::PLAYING)
	{
		_bool bAnyActive = false;
		switch (m_eMode)
		{
		case UI_PREVIEW_MODE::LAYOUT:
			bAnyActive = false;
			break;
		case UI_PREVIEW_MODE::SELECTED_ANIM:
		{
			const _int iSel = m_pSession->Get_SelectedWidget();
			const auto& vW = m_pSession->Get_Doc().vWidgets;
			if (iSel >= 0 && iSel < (_int)vW.size())
			{
				CUIObject* pUI = Find_Runtime(vW[iSel].strId);
				if (pUI && pUI->Get_Animator() && pUI->Get_Animator()->Is_Playing())
					bAnyActive = true;
			}
			break;
		}
		case UI_PREVIEW_MODE::SEQUENCE:
			if (m_pSequence && m_pSequence->Is_Playing())
				bAnyActive = true;
			break;
		}
		if (!bAnyActive)
			m_eState = UI_PREVIEW_STATE::IDLE;
	}
}

void CUIPreviewHost::Render_Queue_Submit()
{
	if (m_pSequence)
		m_pSequence->Late_Update(0.f);
}

HRESULT CUIPreviewHost::Rebuild()
{
	// 캔버스 크기가 아직 0이면 rebuild 보류(다음 프레임 재시도)
	if (m_vLastViewportSize.x < 1.f || m_vLastViewportSize.y < 1.f)
	{
		const _float2 vCanvas = m_pGameInstance->Get_ViewportSize();
		const ImVec2 vNow(vCanvas.x, vCanvas.y);
		if (vNow.x < 1.f || vNow.y < 1.f)
			return S_OK;
		m_vLastViewportSize = vNow;
	}

	Release_All();

	// 빈 sequence prototype clone (path 없음 -> doc 기반 manual build)
	CUISequence::UISEQUENCE_DESC tSeqDesc{};
	m_pSequence = static_cast<CUISequence*>(m_pGameInstance->Clone_Prototype(
		PROTOTYPE::GAMEOBJECT, ETOUI(LEVEL::STATIC), PROTO_UI_SEQUENCE, &tSeqDesc));
	if (nullptr == m_pSequence) return E_FAIL;
	m_pSequence->Clear_Timeline();   // 멱등성

	const UISEQ_DOC& tDoc = m_pSession->Get_Doc();

	// 자식 widget clone + animator 등록 + sequence Add_Child + id map
	for (const auto& w : tDoc.vWidgets)
	{
		UISEQ_WIDGET_NODE::tDescType tDescCopy = w.tDesc;

		std::visit([&](auto& d) {
			using T = std::decay_t<decltype(d)>;
			if      constexpr (std::is_same_v<T, CUIImage::UIIMAGE_DESC>)
				Apply_Fallback_Image(d);
			else if constexpr (std::is_same_v<T, CUIButton::UIBUTTON_DESC>)
				Apply_Fallback_Button(d);
			else if constexpr (std::is_same_v<T, CUIProgressBar::UIPROGRESSBAR_DESC>)
				Apply_Fallback_ProgressBar(d);
			else if constexpr (std::is_same_v<T, CUIText::UITEXT_DESC>)
				Apply_Fallback_Text(d);
			}, tDescCopy);

		WNameID strProto = INVALID_TAG;
		switch (w.Get_Type())
		{
		case UI_TYPE::CONTAINER:   strProto = PROTO_UI_CONTAINER;   break;
		case UI_TYPE::IMAGE:       strProto = PROTO_UI_IMAGE;       break;
		case UI_TYPE::TEXT:        strProto = PROTO_UI_TEXT;        break;
		case UI_TYPE::BUTTON:      strProto = PROTO_UI_BUTTON;      break;
		case UI_TYPE::PROGRESSBAR: strProto = PROTO_UI_PROGRESSBAR; break;
		default: continue;
		}

		void* pArg = std::visit([](auto& d) -> void* {
			return static_cast<void*>(&d);
			}, tDescCopy);

		CGameObject* pClone = static_cast<CGameObject*>(m_pGameInstance->Clone_Prototype(
			PROTOTYPE::GAMEOBJECT, ETOUI(LEVEL::STATIC), strProto, pArg));
		if (nullptr == pClone) continue;

		CUIObject* pUI = static_cast<CUIObject*>(pClone);

		if (CUIAnimator* pAnimator = pUI->Get_Animator())
		{
			pAnimator->Clear_Animations();
			for (const UISEQ_ANIMATION_NODE& a : w.vAnimations)
			{
				if (a.strName.empty()) continue;
				pAnimator->Register_Animation(a.strName, a.vTracks);
			}
		}

		m_id2Widget[w.strId] = pUI;   // weak ref
		m_pSequence->Add_Child(pUI);  // ref++ (sequence가 owner)
		Safe_Release(pUI);            // clone ref-- -> sequence ref만 남음
	}

	// timeline step 빌드
	for (const UISEQ_STEP_NODE& sn : tDoc.vSteps)
	{
		CUISequence::UISEQ_STEP s{};
		s.eKind = sn.eKind;
		s.pTarget = Find_Runtime(sn.strTargetId);
		s.strAnimName = sn.strAnimName;
		s.fWaitSec = sn.fWaitSec;
		s.bVisible = sn.bVisible;
		s.bJoinPrev = sn.bJoinPrev;

		if (sn.eKind == UI_SEQ_STEP_KIND::USE_CALLBACK && !sn.strCallbackId.empty())
		{
			const _string strId = sn.strCallbackId;
			s.fnCallback = [strId]() { /* TODO: callback registry */ };
		}

		if (sn.bJoinPrev) m_pSequence->Join(s);
		else              m_pSequence->Append(s);
	}

	Sort_ZOrder();
	m_bRebuildPending = false;
	m_eState = UI_PREVIEW_STATE::IDLE;
	return S_OK;
}

void CUIPreviewHost::Process_Rebuild_If_Pending()
{
	if (m_bRebuildPending)
		Rebuild();
}

void CUIPreviewHost::Play()
{
	if (m_eMode == UI_PREVIEW_MODE::LAYOUT)
		return;

	if (m_eState == UI_PREVIEW_STATE::PAUSED)
	{
		m_eState = UI_PREVIEW_STATE::PLAYING;
		return;
	}

	if (m_eState != UI_PREVIEW_STATE::IDLE) return;

	switch (m_eMode)
	{
	case UI_PREVIEW_MODE::SELECTED_ANIM:
	{
		const _int iSel = m_pSession->Get_SelectedWidget();
		const _int iAnim = m_pSession->Get_SelectedAnimation();
		const auto& vW = m_pSession->Get_Doc().vWidgets;
		if (iSel < 0 || iSel >= (_int)vW.size()) break;
		if (iAnim < 0 || iAnim >= (_int)vW[iSel].vAnimations.size()) break;

		CUIObject* pUI = Find_Runtime(vW[iSel].strId);
		if (nullptr == pUI || nullptr == pUI->Get_Animator()) break;

		const _wstring& strName = vW[iSel].vAnimations[iAnim].strName;
		pUI->Get_Animator()->Stop_All();
		pUI->Get_Animator()->Play_Animation(strName);
	}
	break;

	case UI_PREVIEW_MODE::SEQUENCE:
		if (m_pSequence) m_pSequence->Play();
		break;

	default:
		break;
	}

	m_eState = UI_PREVIEW_STATE::PLAYING;
}

void CUIPreviewHost::Pause()
{
	if (m_eState == UI_PREVIEW_STATE::PLAYING)
		m_eState = UI_PREVIEW_STATE::PAUSED;
}

void CUIPreviewHost::Resume()
{
	if (m_eState == UI_PREVIEW_STATE::PAUSED)
		m_eState = UI_PREVIEW_STATE::PLAYING;
}

void CUIPreviewHost::Stop()
{
	if (m_pSequence)
	{
		for (CUIObject* p : m_pSequence->Get_Children())
			if (p && p->Get_Animator()) p->Get_Animator()->Stop_All();
		m_pSequence->Stop();
	}
	m_eState = UI_PREVIEW_STATE::IDLE;
}

void CUIPreviewHost::Restart()
{
	Stop();
	Play();
}

CUIObject* CUIPreviewHost::Find_Runtime(const _string& strId) const
{
	auto it = m_id2Widget.find(strId);
	return (it == m_id2Widget.end()) ? nullptr : it->second;
}

_string CUIPreviewHost::Hit_Test_TopMost(const ImVec2& vDocXY) const
{
	if (nullptr == m_pSequence) return {};
	const auto& vChildren = m_pSequence->Get_Children();

	// m_vZOrderIdx는 z-order 오름차순 -> 위에서부터 hit-test하려면 역순 순회
	for (auto it = m_vZOrderIdx.rbegin(); it != m_vZOrderIdx.rend(); ++it)
	{
		if (*it < 0 || *it >= (_int)vChildren.size()) continue;
		CUIObject* pUI = vChildren[*it];
		if (nullptr == pUI) continue;
		if (!pUI->Get_Visible()) continue;

		const _float4 rc = pUI->Get_ScreenRect();
		if (vDocXY.x >= rc.x && vDocXY.x <= rc.x + rc.z &&
			vDocXY.y >= rc.y && vDocXY.y <= rc.y + rc.w)
		{
			for (const auto& kv : m_id2Widget)
				if (kv.second == pUI) return kv.first;
		}
	}
	return {};
}

void CUIPreviewHost::Release_All()
{
	// sequence가 자식 owner이므로 자식은 sequence Free에서 자동 release
	Safe_Release(m_pSequence);
	m_id2Widget.clear();
	m_vZOrderIdx.clear();
}

void CUIPreviewHost::Apply_Fallback_Image(CUIImage::UIIMAGE_DESC& d) const
{
	if (INVALID_TAG == d.strTextureTag)
	{
		d.strTextureTag = PROTO_COM_TEXTURE_DUMMY_WHITE;
		d.iTextureLevel = ETOUI(LEVEL::STATIC);
		d.iTextureIndex = 0;
	}
}

void CUIPreviewHost::Apply_Fallback_Text(CUIText::UITEXT_DESC& d) const
{
	if (INVALID_TAG == d.strFontTag)
		d.strFontTag = FONT_MALGUN;
}

void CUIPreviewHost::Apply_Fallback_Button(CUIButton::UIBUTTON_DESC& d) const
{
	if (INVALID_TAG == d.strTextureTag)
	{
		d.strTextureTag = PROTO_COM_TEXTURE_DUMMY_WHITE;
		d.iTextureLevel = ETOUI(LEVEL::STATIC);
		d.iNormalTextureIndex = d.iHoverTextureIndex
			= d.iPressedTextureIndex = d.iDisabledTextureIndex = 0;
	}
}

void CUIPreviewHost::Apply_Fallback_ProgressBar(CUIProgressBar::UIPROGRESSBAR_DESC& d) const
{
	if (INVALID_TAG == d.strBackTextureTag)
	{
		d.strBackTextureTag = PROTO_COM_TEXTURE_DUMMY_WHITE;
		d.iBackTextureLevel = ETOUI(LEVEL::STATIC);
		d.iBackTextureIndex = 0;
	}
	if (INVALID_TAG == d.strFillTextureTag)
	{
		d.strFillTextureTag = PROTO_COM_TEXTURE_DUMMY_WHITE;
		d.iFillTextureLevel = ETOUI(LEVEL::STATIC);
		d.iFillTextureIndex = 0;
	}
}

void CUIPreviewHost::Sort_ZOrder()
{
	m_vZOrderIdx.clear();
	if (nullptr == m_pSequence) return;

	const auto& vChildren = m_pSequence->Get_Children();
	m_vZOrderIdx.resize(vChildren.size());
	std::iota(m_vZOrderIdx.begin(), m_vZOrderIdx.end(), 0);
	std::stable_sort(m_vZOrderIdx.begin(), m_vZOrderIdx.end(),
		[&vChildren](_int a, _int b) {
			return vChildren[a]->Get_ZOrder() < vChildren[b]->Get_ZOrder();
		});
}

CUIPreviewHost* CUIPreviewHost::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUIPreviewHost* pInstance = new CUIPreviewHost(pDevice, pContext);
	if (FAILED(pInstance->Initialize()))
	{
		delete pInstance;
		return nullptr;
	}
	return pInstance;
}

void CUIPreviewHost::Free()
{
	__super::Free();
	Release_All();
	Safe_Release(m_pSession);
	Safe_Release(m_pEditInstance);
	Safe_Release(m_pGameInstance);
	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);
}
