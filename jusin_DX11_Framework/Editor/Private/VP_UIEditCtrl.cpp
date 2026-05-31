#include "VP_UIEditCtrl.h"
#include "VP_CoordMapper.h"
#include "UIEditorSession.h"
#include "UIPreviewHost.h"
#include "EditInstance.h"

#include "GameInstance.h"
#include "UIObject.h"

CVP_UIEditCtrl::CVP_UIEditCtrl()
	: m_pGameInstance(CGameInstance::GetInstance())
	, m_pEditInstance(CEditInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pEditInstance);
}

HRESULT CVP_UIEditCtrl::Initialize(CVP_CoordMapper* pMapper)
{
	if (nullptr == pMapper)
		return E_FAIL;

	m_pMapper = pMapper;
	Safe_AddRef(m_pMapper);

	return S_OK;
}

void CVP_UIEditCtrl::Handle_UIPick()
{
	auto* pHost = m_pEditInstance->Get_UIPreviewHost();
	auto* pSess = m_pEditInstance->Get_UISession();
	if (!pHost || !pSess) return;

	const ImVec2 vMouse = ImGui::GetMousePos();
	const ImVec2 vDoc = m_pMapper->ScreenToDoc(vMouse);

	const _string strHit = pHost->Hit_Test_TopMost(vDoc);
	if (strHit.empty())
	{
		pSess->Set_SelectedWidget(-1);
		m_bUIDragging = false;
		return;
	}

	const _int iIdx = pSess->Find_WidgetIndexById(strHit);
	if (iIdx < 0) return;

	pSess->Set_SelectedWidget(iIdx);

	// drag 시작 상태 캡처
	const auto& tBase = Get_BaseDesc(pSess->Get_Doc().vWidgets[iIdx]);
	m_bUIDragging = true;
	m_strDragId = strHit;
	m_vDragStartMouse = vMouse;
	m_bDragWasAnchored = tBase.tAnchorDesc.bUseAnchoredPos;
	if (m_bDragWasAnchored)
	{
		m_fDragStartCenterX = tBase.tAnchorDesc.fOffsetX;
		m_fDragStartCenterY = tBase.tAnchorDesc.fOffsetY;
	}
	else
	{
		m_fDragStartCenterX = tBase.fCenterX;
		m_fDragStartCenterY = tBase.fCenterY;
	}
}

void CVP_UIEditCtrl::Handle_UIDrag()
{
	if (!m_bUIDragging) return;

	auto* pHost = m_pEditInstance->Get_UIPreviewHost();
	auto* pSess = m_pEditInstance->Get_UISession();
	if (!pHost || !pSess) { m_bUIDragging = false; return; }

	// 마우스 떼면 종료
	if (!m_pGameInstance->Mouse_Pressing(DIMB::LBUTTON))
	{
		m_bUIDragging = false;
		return;
	}

	const _int iIdx = pSess->Find_WidgetIndexById(m_strDragId);
	if (iIdx < 0) { m_bUIDragging = false; return; }

	// doc 좌표계 델타 계산
	if (!m_pMapper) { m_bUIDragging = false; return; }

	const ImVec2 vMouse = ImGui::GetMousePos();
	const ImVec2 vNow = m_pMapper->ScreenToDoc(vMouse);
	const ImVec2 vSt = m_pMapper->ScreenToDoc(m_vDragStartMouse);
	const _float dx = vNow.x - vSt.x;
	const _float dy = vNow.y - vSt.y;

	auto& tNode = pSess->Get_DocMutable().vWidgets[iIdx];
	auto& tBase = Get_BaseDesc(tNode);

	if (m_bDragWasAnchored)
	{
		tBase.tAnchorDesc.fOffsetX = m_fDragStartCenterX + dx;
		tBase.tAnchorDesc.fOffsetY = m_fDragStartCenterY + dy;
	}
	else
	{
		tBase.fCenterX = m_fDragStartCenterX + dx;
		tBase.fCenterY = m_fDragStartCenterY + dy;
	}

	// runtime widget에 즉시 반영 (rebuild 회피)
	if (CUIObject* p = pHost->Find_Runtime(m_strDragId))
	{
		if (m_bDragWasAnchored)
			p->Set_AnchorOffset(tBase.tAnchorDesc.fOffsetX,
				tBase.tAnchorDesc.fOffsetY);
		else
			p->Set_Center(tBase.fCenterX, tBase.fCenterY);
	}

	pSess->Mark_Dirty_Property("Drag move");
}

CVP_UIEditCtrl* CVP_UIEditCtrl::Create(CVP_CoordMapper* pMapper)
{
    CVP_UIEditCtrl* pInstance = new CVP_UIEditCtrl();

    if (FAILED(pInstance->Initialize(pMapper)))
    {
        MSG_BOX("Failed to Create : CVP_UIEditCtrl");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CVP_UIEditCtrl::Free()
{
    __super::Free();

    Safe_Release(m_pMapper);
    Safe_Release(m_pEditInstance);
    Safe_Release(m_pGameInstance);
}