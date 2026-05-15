#include "Panel_Viewport.h"
#include "EditInstance.h"
#include "UIEditorSession.h"
#include "VP_RenderTarget.h"
#include "VP_CoordMapper.h"
#include "VP_PickingCtrl.h"
#include "VP_UIEditCtrl.h"
#include "VP_OverlayDrawer.h"

#include "GameInstance.h"
#include "Camera.h"

namespace
{
	ImVec2 Fit_Size_To_Aspect(const ImVec2& vAvailableSize, _float fTargetAspect)
	{
		if (vAvailableSize.x <= 0.f || vAvailableSize.y <= 0.f || fTargetAspect <= 0.f)
			return ImVec2(1.f, 1.f);

		ImVec2 vFittedSize = vAvailableSize;
		const _float fAvailableAspect = vAvailableSize.x / vAvailableSize.y;

		if (fAvailableAspect > fTargetAspect)
			vFittedSize.x = vAvailableSize.y * fTargetAspect;
		else
			vFittedSize.y = vAvailableSize.x / fTargetAspect;

		vFittedSize.x = max(vFittedSize.x, 1.f);
		vFittedSize.y = max(vFittedSize.y, 1.f);

		return vFittedSize;
	}

	void SetNoBlendCallback(const ImDrawList*, const ImDrawCmd* pCmd)
	{
		ID3D11DeviceContext* pContext = static_cast<ID3D11DeviceContext*>(pCmd->UserCallbackData);
		if (nullptr == pContext)
			return;

		const FLOAT blendFactor[4] = { 0.f, 0.f, 0.f, 0.f };
		pContext->OMSetBlendState(nullptr, blendFactor, 0xffffffff);
	}
}

ImVec2 CPanel_Viewport::s_vFallBack0 = { 0.f, 0.f };
ImVec2 CPanel_Viewport::s_vFallBack1 = { 1.f, 1.f };

CPanel_Viewport::CPanel_Viewport(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPanel_Base()
	, m_pDevice(pDevice)
	, m_pContext(pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CPanel_Viewport::Initialize()
{
	m_strTitle = "Viewport";
	m_iWindowFlags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;

	if (FAILED(__super::Initialize()))
		return E_FAIL;

	const _float2 vRT = m_pGameInstance->Get_ViewportSize();
	const ImVec2 vRTSize(vRT.x, vRT.y);

	if (auto* pSess = m_pEditInstance->Get_UISession())
	{
		const auto& tDoc = pSess->Get_Doc();
		m_vDocCanvasSize = ImVec2(tDoc.fDesignWidth, tDoc.fDesignHeight);
	}
	else
	{
		m_vDocCanvasSize = vRTSize;
	}

	// Panel RT 는 design 크기로 고정. (클라이언트 실행 시의 게임 화면과 동일하게 맞추기 위함)
	// doc 이 비정상이면 GameInstance viewport 크기로 fallback.
	const ImVec2 vRTInit =
		(m_vDocCanvasSize.x >= 1.f && m_vDocCanvasSize.y >= 1.f) ? m_vDocCanvasSize : vRTSize;

	m_pRenderTarget = CVP_RenderTarget::Create(m_pDevice, m_pContext, vRTInit);
	if (nullptr == m_pRenderTarget)
		return E_FAIL;

	m_pMapper = CVP_CoordMapper::Create();
	if (nullptr == m_pMapper)
		return E_FAIL;

	m_pPickingCtrl = CVP_PickingCtrl::Create(m_pMapper);
	if (nullptr == m_pPickingCtrl)
		return E_FAIL;

	m_pUIEditCtrl = CVP_UIEditCtrl::Create(m_pMapper);
	if (nullptr == m_pUIEditCtrl)
		return E_FAIL;

	m_pOverlayDrawer = CVP_OverlayDrawer::Create(m_pMapper);
	if (nullptr == m_pOverlayDrawer)
		return E_FAIL;

	return S_OK;
}

void CPanel_Viewport::Update(_float fTimeDelta)
{
	const _float2 vRT = m_pGameInstance->Get_ViewportSize();
	const ImVec2 vNewRT(vRT.x, vRT.y);

	if (auto* pSess = m_pEditInstance->Get_UISession())
	{
		const auto& tDoc = pSess->Get_Doc();
		m_vDocCanvasSize = ImVec2(tDoc.fDesignWidth, tDoc.fDesignHeight);
	}

	if (m_pRenderTarget)
	{
		const ImVec2& vCur = m_pRenderTarget->Get_Size();

		// RT 는 design 크기로 유지. doc 이 비정상이면 현재 RT 크기를 그대로 둔다.
		const _bool bDocValid = (m_vDocCanvasSize.x >= 1.f && m_vDocCanvasSize.y >= 1.f);
		const ImVec2 vTarget = bDocValid ? m_vDocCanvasSize : vCur;

		if (vCur.x != vNewRT.x || vCur.y != vNewRT.y)
		{
			Safe_Release(m_pRenderTarget);
			m_pRenderTarget = CVP_RenderTarget::Create(m_pDevice, m_pContext, vNewRT);
		}
	}
}

HRESULT CPanel_Viewport::Render()
{
    if (!Begin_Panel())
	{
		m_bHovered = false;
		m_bFocused = false;
		End_Panel();
		return S_OK;
	}

	// 패널 안에 그릴 이미지 사각형(letterbox)을 캔버스 종횡비 기준으로 계산
	ImVec2 vAvail = ImGui::GetContentRegionAvail();
	const ImVec2 vRTSize = m_pRenderTarget ? m_pRenderTarget->Get_Size() : s_vFallBack1;
	const _float fTargetAspect = vRTSize.y > 0.f ? (vRTSize.x / vRTSize.y) : 1.f;
	const ImVec2 vRenderSize = Fit_Size_To_Aspect(vAvail, fTargetAspect);

	const ImVec2 vCursorPos = ImGui::GetCursorPos();
	const ImVec2 vOffset((vAvail.x - vRenderSize.x) * 0.5f, (vAvail.y - vRenderSize.y) * 0.5f);
	ImGui::SetCursorPos(ImVec2(vCursorPos.x + max(vOffset.x, 0.f), vCursorPos.y + max(vOffset.y, 0.f)));

	ID3D11ShaderResourceView* pSRV = m_pRenderTarget ? m_pRenderTarget->Get_SRV() : nullptr;
	if (nullptr != pSRV)
	{
		ImDrawList* dl = ImGui::GetWindowDrawList();
		dl->AddCallback(SetNoBlendCallback, m_pContext);
		ImGui::Image(pSRV, vRenderSize);
		dl->AddCallback(ImDrawCallback_ResetRenderState, nullptr);

		const ImVec2 vDisplayPos = ImGui::GetItemRectMin();
		m_bHovered = ImGui::IsItemHovered();
		m_bFocused = ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows);

		// mapper 갱신: 매 프레임 한 번 (입력/Overlay 처리 직전)
		if (m_pMapper)
		{
			UI_SCALE_POLICY ePolicy = UI_SCALE_POLICY::UNIFORM_FIT;
			if (auto* pSess = m_pEditInstance->Get_UISession())
				ePolicy = pSess->Get_Doc().eScalePolicy;
			m_pMapper->Update(vDisplayPos, vRenderSize, vRTSize, m_vDocCanvasSize, ePolicy);
		}

		if (m_pOverlayDrawer) m_pOverlayDrawer->Draw(m_bHovered);

		const auto eMode = m_pEditInstance->Get_UISession()->Get_VPMode();

		// 좌클릭 진입
		if (m_bHovered && m_pGameInstance->Mouse_Down(DIMB::LBUTTON))
		{
			switch (eMode)
			{
			case CUIEditorSession::VPMODE::SCENE:
				if (m_pPickingCtrl) m_pPickingCtrl->Handle_ViewportClick();
				break;
			case CUIEditorSession::VPMODE::UI_LAYOUT:
				if (m_pUIEditCtrl) m_pUIEditCtrl->Handle_UIPick();   // selection + drag start
				break;
			case CUIEditorSession::VPMODE::UI_ANIM:
				if (m_pUIEditCtrl) m_pUIEditCtrl->Handle_UIPick();   // selection만 (drag 차단)
				break;
			default: break;
			}
		}

		// drag 갱신은 UI_LAYOUT에서만
		if (eMode == CUIEditorSession::VPMODE::UI_LAYOUT)
		{
			if (m_pUIEditCtrl) m_pUIEditCtrl->Handle_UIDrag();
		}
		else
		{
			// mode가 LAYOUT 밖으로 빠지면 진행 중 drag 강제 종료
			if (m_pUIEditCtrl) m_pUIEditCtrl->Cancel_Drag();
		}

		// nav drag는 SCENE에서만
		if (eMode == CUIEditorSession::VPMODE::SCENE
			&& m_bHovered && m_pEditInstance->Is_NavEditMode()
			&& m_pEditInstance->Get_NavToolMode() == 2 /* NAV_TOOL_MODE::MOVE */
			&& m_pGameInstance->Mouse_Pressing(DIMB::LBUTTON))
		{
			if (m_pPickingCtrl)
			{
				m_pPickingCtrl->Handle_DebugPicking();
				if (m_pPickingCtrl->Has_LastHit())
					m_pEditInstance->Update_NavDragHit(m_pPickingCtrl->Get_LastWorldHitPos());
			}
		}
	}
	else
	{
		ImGui::Dummy(vRenderSize);
		m_bHovered = false;
		m_bFocused = ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows);
	}

	Draw_DebugHUD();

	End_Panel();
	return S_OK;
}

HRESULT CPanel_Viewport::Begin_SceneRender()
{
	if (nullptr == m_pRenderTarget) return E_FAIL;
	return m_pRenderTarget->Begin_SceneRender();
}

HRESULT CPanel_Viewport::End_SceneRender()
{
	if (nullptr == m_pRenderTarget) return E_FAIL;
	return m_pRenderTarget->End_SceneRender();
}

const ImVec2& CPanel_Viewport::Get_DisplaySize() const
{
	return m_pMapper ? m_pMapper->Get_DisplaySize() : s_vFallBack1;
}

const ImVec2& CPanel_Viewport::Get_DisplayPos() const
{
	return m_pMapper ? m_pMapper->Get_DisplayPos() : s_vFallBack0;
}

void CPanel_Viewport::Draw_DebugHUD()
{
	ImDrawList* pDrawList = ImGui::GetWindowDrawList();
	const ImVec2& vDisplayPos = m_pMapper ? m_pMapper->Get_DisplayPos() : s_vFallBack0;

	const CCamera* pMainCamera = m_pGameInstance->Get_MainCamera();
	const _bool bCameraToggleOn = m_pEditInstance->Is_CameraEnabled();
	const _bool bControlEnabled = (nullptr != pMainCamera && pMainCamera->Is_ControlEnabled());
	const _bool bFollowing = (nullptr != pMainCamera && pMainCamera->Is_Following());

	const char* szCamState = "CAM: OFF";
	ImU32 iCamColor = IM_COL32(255, 100, 100, 255);

	if (bCameraToggleOn)
	{
		if (bFollowing)
		{
			szCamState = "CAM: FOLLOW";
			iCamColor = IM_COL32(255, 220, 100, 255);
		}
		else if (bControlEnabled)
		{
			szCamState = "CAM: FREE";
			iCamColor = IM_COL32(100, 255, 100, 255);
		}
		else
		{
			szCamState = "CAM: LOCKED";
			iCamColor = IM_COL32(255, 180, 100, 255);
		}
	}

	pDrawList->AddText(
		ImVec2(static_cast<_float>(g_iWinSizeX) - vDisplayPos.x, vDisplayPos.y + 30.f),
		iCamColor,
		szCamState);

	pDrawList->AddText(
		ImVec2(vDisplayPos.x + 10.f, vDisplayPos.y + 10.f),
		IM_COL32(255, 255, 0, 255),
		m_pPickingCtrl ? m_pPickingCtrl->Get_PickDebug().c_str() : "No Pick"
	);

	pDrawList->AddText(
		ImVec2(vDisplayPos.x + 10.f, vDisplayPos.y + 30.f),
		IM_COL32(180, 220, 255, 255),
		m_pPickingCtrl ? m_pPickingCtrl->Get_PickTarget().c_str() : "Target : None"
	);

	const char* pModeText = m_pEditInstance->Is_PlaceMode() ? "Mode : Place" : "Mode : Select";
	pDrawList->AddText(
		ImVec2(vDisplayPos.x + 10.f, vDisplayPos.y + 50.f),
		IM_COL32(255, 180, 120, 255),
		pModeText
	);

	{   // VPMode 라벨
		const auto eMode = m_pEditInstance->Get_UISession()->Get_VPMode();
		const char* pszVPMode = "VPMode : Scene";
		ImU32 colVPMode = IM_COL32(180, 180, 180, 255);
		switch (eMode)
		{
		case CUIEditorSession::VPMODE::UI_LAYOUT:
			pszVPMode = "VPMode : UI Layout";
			colVPMode = IM_COL32(255, 220, 80, 255);
			break;
		case CUIEditorSession::VPMODE::UI_ANIM:
			pszVPMode = "VPMode : UI Anim";
			colVPMode = IM_COL32(80, 220, 255, 255);
			break;
		default: break;
		}
		pDrawList->AddText(
			ImVec2(vDisplayPos.x + 10.f, vDisplayPos.y + 70.f),
			colVPMode, pszVPMode);
	}

	if (m_pPickingCtrl && m_pPickingCtrl->Has_LastHit())
	{
		const _float3& vLastObjectPos = m_pPickingCtrl->Get_LastObjectPos();
		const _float3& vLastLocalHitPos = m_pPickingCtrl->Get_LastLocalHitPos();
		const _float3& vLastWorldHitPos = m_pPickingCtrl->Get_LastWorldHitPos();

		char szObjBuf[128] = {};
		sprintf_s(szObjBuf, "Obj Pos   : %.2f, %.2f, %.2f",
			vLastObjectPos.x, vLastObjectPos.y, vLastObjectPos.z);
		pDrawList->AddText(
			ImVec2(vDisplayPos.x + 10.f, vDisplayPos.y + 70.f),
			IM_COL32(255, 180, 120, 255), szObjBuf);

		char szLocalBuf[128] = {};
		sprintf_s(szLocalBuf, "Local Hit : %.2f, %.2f, %.2f",
			vLastLocalHitPos.x, vLastLocalHitPos.y, vLastLocalHitPos.z);
		pDrawList->AddText(
			ImVec2(vDisplayPos.x + 10.f, vDisplayPos.y + 90.f),
			IM_COL32(120, 220, 255, 255), szLocalBuf);

		char szWorldBuf[128] = {};
		sprintf_s(szWorldBuf, "World Hit : %.2f, %.2f, %.2f",
			vLastWorldHitPos.x, vLastWorldHitPos.y, vLastWorldHitPos.z);
		pDrawList->AddText(
			ImVec2(vDisplayPos.x + 10.f, vDisplayPos.y + 110.f),
			IM_COL32(0, 255, 0, 255), szWorldBuf);
	}
}

CPanel_Viewport* CPanel_Viewport::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPanel_Viewport* pInstance = new CPanel_Viewport(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Create : CPanel_Viewport");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPanel_Viewport::Free()
{
	__super::Free();

	Safe_Release(m_pOverlayDrawer);
	Safe_Release(m_pUIEditCtrl);
	Safe_Release(m_pPickingCtrl);
	Safe_Release(m_pMapper);
	Safe_Release(m_pRenderTarget);

	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);
}
