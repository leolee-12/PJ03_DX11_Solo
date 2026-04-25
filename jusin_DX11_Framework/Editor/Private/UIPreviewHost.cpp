#include "UIPreviewHost.h"
#include "UIEditorSession.h"
#include "UISequence.h"

#include "GameInstance.h"
#include "EditInstance.h"

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

HRESULT CUIPreviewHost::Initialize()
{
    return S_OK;
}

void CUIPreviewHost::Tick(_float fTimeDelta)
{
    // viewport size 변경 감지
    const ImVec2 vNow = m_pEditInstance->Get_ViewportScreenSize();
    if (vNow.x != m_vLastViewportSize.x || vNow.y != m_vLastViewportSize.y)
    {
        m_vLastViewportSize = vNow;
        m_bRebuildPending = true;
    }

    // 위젯 tick (paused 시 Update만 skip)
    const _bool bPaused = (m_eState == UI_PREVIEW_STATE::PAUSED);

    for (CUIObject* p : m_vWidgets) p->Priority_Update(fTimeDelta);

    if (!bPaused)
    {
        for (CUIObject* p : m_vWidgets) p->Update(fTimeDelta);
        if (m_eMode == UI_PREVIEW_MODE::SEQUENCE && m_pSequence)
            m_pSequence->Update(fTimeDelta);
    }
}

void CUIPreviewHost::Render_Queue_Submit()
{
    // z-order 오름차순으로 Late_Update 호출 (RENDERID::UI 가 FIFO이므로)
    for (_int idx : m_vZOrderIdx)
        m_vWidgets[idx]->Late_Update(0.f);
}

HRESULT CUIPreviewHost::Rebuild()
{
    // viewport size가 아직 0이면 rebuild 보류 (다음 프레임 재시도)
    if (m_vLastViewportSize.x < 1.f || m_vLastViewportSize.y < 1.f)
    {
        const ImVec2 vNow = m_pEditInstance->Get_ViewportScreenSize();
        if (vNow.x < 1.f || vNow.y < 1.f)
            return S_OK;   // pending 유지
        m_vLastViewportSize = vNow;
    }

    Release_All();

    const UISEQ_DOC& tDoc = m_pSession->Get_Doc();

    for (const auto& w : tDoc.vWidgets)
    {
        // desc 사본
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
            // CONTAINER: fallback 없음
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

        // desc 사본의 base 부분 포인터 추출
        void* pArg = std::visit([](auto& d) -> void* {
            return static_cast<void*>(&d);
            }, tDescCopy);

        CGameObject* pClone = static_cast<CGameObject*>(m_pGameInstance->Clone_Prototype(
            PROTOTYPE::GAMEOBJECT, ETOUI(LEVEL::STATIC), strProto, pArg));
        if (nullptr == pClone) continue;

        CUIObject* pUI = static_cast<CUIObject*>(pClone);
        m_vWidgets.push_back(pUI);
        m_id2Widget[w.strId] = pUI;
    }

    // CUISequence 직접 생성 (PROTO_UI_SEQUENCE 미등록 정책 §10.3)
    m_pSequence = CUISequence::Create(m_pDevice, m_pContext);
    if (nullptr != m_pSequence)
        m_pSequence->Initialize(nullptr);

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

void CUIPreviewHost::Pause()
{
}

void CUIPreviewHost::Resume()
{
}

void CUIPreviewHost::Release_All()
{
    for (auto* p : m_vWidgets)
        Safe_Release(p);
    
    m_vWidgets.clear();
    m_id2Widget.clear();
    m_vZOrderIdx.clear();
    Safe_Release(m_pSequence);
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
    m_vZOrderIdx.resize(m_vWidgets.size());
    std::iota(m_vZOrderIdx.begin(), m_vZOrderIdx.end(), 0);
    std::stable_sort(m_vZOrderIdx.begin(), m_vZOrderIdx.end(),
        [this](_int a, _int b) {
            // CUIObject는 z-order getter가 없으면 추가하거나 Get_BaseDesc 활용 어려움
            // → CUIObject에 _int Get_ZOrder() const { return m_iZOrder; } 추가 권장
            return m_vWidgets[a]->Get_ZOrder() < m_vWidgets[b]->Get_ZOrder();
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
