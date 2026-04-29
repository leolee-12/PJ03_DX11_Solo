#include "Panel_Viewport.h"
#include "EditInstance.h"
#include "UIEditorSession.h"
#include "UIPreviewHost.h"

#include "GameInstance.h"
#include "Model.h"
#include "Mesh.h"

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

	// 스크린 픽셀(=display 좌표) -> doc 픽셀(=canvas 좌표)
	inline ImVec2 ScreenToDoc(const ImVec2& vScreen,
		const ImVec2& vDispPos,
		const ImVec2& vCanvasSize,
		const ImVec2& vDispSize)
	{
		const _float sx = (vDispSize.x > 0.f) ? vCanvasSize.x / vDispSize.x : 1.f;
		const _float sy = (vDispSize.y > 0.f) ? vCanvasSize.y / vDispSize.y : 1.f;
		return ImVec2((vScreen.x - vDispPos.x) * sx,
			(vScreen.y - vDispPos.y) * sy);
	}

	// doc 사각형(rect.xy, w, h) -> 스크린 min/max
	inline void DocRectToScreen(const _float4& rcDoc,
		const ImVec2& vDispPos,
		const ImVec2& vCanvasSize,
		const ImVec2& vDispSize,
		ImVec2* pOutMin, ImVec2* pOutMax)
	{
		const _float sx = (vCanvasSize.x > 0.f) ? vDispSize.x / vCanvasSize.x : 1.f;
		const _float sy = (vCanvasSize.y > 0.f) ? vDispSize.y / vCanvasSize.y : 1.f;
		pOutMin->x = vDispPos.x + rcDoc.x * sx;
		pOutMin->y = vDispPos.y + rcDoc.y * sy;
		pOutMax->x = pOutMin->x + rcDoc.z * sx;
		pOutMax->y = pOutMin->y + rcDoc.w * sy;
	}
}

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

	const _float2 vCanvas = m_pGameInstance->Get_ViewportSize();
	m_vCanvasSize = ImVec2(vCanvas.x, vCanvas.y);

	if (FAILED(Create_RenderTarget()))
		return E_FAIL;

	return S_OK;
}

void CPanel_Viewport::Update(_float fTimeDelta)
{
	const _float2 vCanvas = m_pGameInstance->Get_ViewportSize();
	ImVec2 vNewSize = ImVec2(vCanvas.x, vCanvas.y);
	
	if (m_vCanvasSize == vNewSize)
		return;

	m_vCanvasSize = vNewSize;
	Release_RenderTarget();
	Create_RenderTarget();
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
	const _float fTargetAspect = m_vCanvasSize.y > 0.f
		? (m_vCanvasSize.x / m_vCanvasSize.y) : 1.f;
	const ImVec2 vRenderSize = Fit_Size_To_Aspect(vAvail, fTargetAspect);

	m_vDisplaySize = vRenderSize;

	const ImVec2 vCursorPos = ImGui::GetCursorPos();
	const ImVec2 vOffset((vAvail.x - vRenderSize.x) * 0.5f, (vAvail.y - vRenderSize.y) * 0.5f);
	ImGui::SetCursorPos(ImVec2(vCursorPos.x + max(vOffset.x, 0.f), vCursorPos.y + max(vOffset.y,
		0.f)));
	m_vDisplayPos = ImGui::GetCursorScreenPos();

	if (nullptr != m_pSRV)
	{
		ImDrawList* dl = ImGui::GetWindowDrawList();
		dl->AddCallback(SetNoBlendCallback, m_pContext);
		ImGui::Image(m_pSRV, m_vDisplaySize);
		dl->AddCallback(ImDrawCallback_ResetRenderState, nullptr);

		// Image() 후에 실제 렌더된 위치로 갱신 → GetMousePos()와 좌표계 일치
		m_vDisplayPos = ImGui::GetItemRectMin();

		Draw_UIOverlay();   // 갱신된 m_vDisplayPos 기준으로 widget 박스/hover 라벨 그림

		m_bHovered = ImGui::IsItemHovered();
		m_bFocused = ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows);

		const auto eMode = m_pEditInstance->Get_UISession()->Get_VPMode();

		// 좌클릭 진입
		if (m_bHovered && m_pGameInstance->Mouse_Down(DIMB::LBUTTON))
		{
			switch (eMode)
			{
			case CUIEditorSession::VPMODE::SCENE:
				Handle_ViewportClick();
				break;
			case CUIEditorSession::VPMODE::UI_LAYOUT:
				Handle_UIPick();   // selection + drag start
				break;
			case CUIEditorSession::VPMODE::UI_ANIM:
				Handle_UIPick();   // selection만 (drag 차단)
				break;
			default: break;
			}
		}

		// drag 갱신은 UI_LAYOUT에서만
		if (eMode == CUIEditorSession::VPMODE::UI_LAYOUT)
		{
			Handle_UIDrag();
		}
		else if (m_bUIDragging)
		{
			// mode가 LAYOUT 밖으로 빠지면 진행 중 drag 강제 종료
			m_bUIDragging = false;
		}

		// nav drag는 SCENE에서만
		if (eMode == CUIEditorSession::VPMODE::SCENE
			&& m_bHovered && m_pEditInstance->Is_NavEditMode()
			&& m_pEditInstance->Get_NavToolMode() == 2 /* NAV_TOOL_MODE::MOVE */
			&& m_pGameInstance->Mouse_Pressing(DIMB::LBUTTON))
		{
			Handle_DebugPicking();
			if (m_bHasLastHit)
				m_pEditInstance->Update_NavDragHit(m_vLastWorldHitPos);
		}
	}
	else
	{
		ImGui::Dummy(vRenderSize);
		m_bHovered = false;
		m_bFocused = ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows);
	}

	ImDrawList* pDrawList = ImGui::GetWindowDrawList();

	const char* szCamState = m_pEditInstance->Is_CameraEnabled() ? "CAM: ON" : "CAM: OFF";
	pDrawList->AddText(
		ImVec2(static_cast<_float>(g_iWinSizeX) - m_vDisplayPos.x, m_vDisplayPos.y + 30.f),
		m_pEditInstance->Is_CameraEnabled() ? IM_COL32(100, 255, 100, 255) : IM_COL32(255, 100, 100,
			255),
		szCamState);

	pDrawList->AddText(
		ImVec2(m_vDisplayPos.x + 10.f, m_vDisplayPos.y + 10.f),
		IM_COL32(255, 255, 0, 255),
		m_strPickDebug.c_str()
	);

	pDrawList->AddText(
		ImVec2(m_vDisplayPos.x + 10.f, m_vDisplayPos.y + 30.f),
		IM_COL32(180, 220, 255, 255),
		m_strPickTarget.c_str()
	);

	const char* pModeText = m_pEditInstance->Is_PlaceMode() ? "Mode : Place" : "Mode : Select";
	pDrawList->AddText(
		ImVec2(m_vDisplayPos.x + 10.f, m_vDisplayPos.y + 50.f),
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
			ImVec2(m_vDisplayPos.x + 10.f, m_vDisplayPos.y + 70.f),
			colVPMode, pszVPMode);
	}

	if (m_bHasLastHit)
	{
		char szObjBuf[128] = {};
		sprintf_s(szObjBuf, "Obj Pos   : %.2f, %.2f, %.2f",
			m_vLastObjectPos.x, m_vLastObjectPos.y, m_vLastObjectPos.z);
		pDrawList->AddText(
			ImVec2(m_vDisplayPos.x + 10.f, m_vDisplayPos.y + 70.f),
			IM_COL32(255, 180, 120, 255), szObjBuf);

		char szLocalBuf[128] = {};
		sprintf_s(szLocalBuf, "Local Hit : %.2f, %.2f, %.2f",
			m_vLastLocalHitPos.x, m_vLastLocalHitPos.y, m_vLastLocalHitPos.z);
		pDrawList->AddText(
			ImVec2(m_vDisplayPos.x + 10.f, m_vDisplayPos.y + 90.f),
			IM_COL32(120, 220, 255, 255), szLocalBuf);

		char szWorldBuf[128] = {};
		sprintf_s(szWorldBuf, "World Hit : %.2f, %.2f, %.2f",
			m_vLastWorldHitPos.x, m_vLastWorldHitPos.y, m_vLastWorldHitPos.z);
		pDrawList->AddText(
			ImVec2(m_vDisplayPos.x + 10.f, m_vDisplayPos.y + 110.f),
			IM_COL32(0, 255, 0, 255), szWorldBuf);
	}

	End_Panel();
	return S_OK;
}

HRESULT CPanel_Viewport::Begin_SceneRender()
{
	if (nullptr == m_pContext || nullptr == m_pRTV || nullptr == m_pDSV)
		return E_FAIL;

	if (m_vCanvasSize.x <= 0.f || m_vCanvasSize.y <= 0.f)
		return E_FAIL;

	Safe_Release(m_pPrevRTV);
	m_pPrevRTV = nullptr;
	Safe_Release(m_pPrevDSV);
	m_pPrevDSV = nullptr;

	m_iPrevViewportCount = 1;
	ZeroMemory(&m_tPrevViewport, sizeof(D3D11_VIEWPORT));

	m_pContext->OMGetRenderTargets(1, &m_pPrevRTV, &m_pPrevDSV);
	m_pContext->RSGetViewports(&m_iPrevViewportCount, &m_tPrevViewport);

	m_pContext->OMSetRenderTargets(1, &m_pRTV, m_pDSV);

	D3D11_VIEWPORT tViewport{};
	tViewport.TopLeftX = 0.f;
	tViewport.TopLeftY = 0.f;
	tViewport.Width = m_vCanvasSize.x;
	tViewport.Height = m_vCanvasSize.y;
	tViewport.MinDepth = 0.f;
	tViewport.MaxDepth = 1.f;
	m_pContext->RSSetViewports(1, &tViewport);

	const _float vClearColor[4] = { 0.10f, 0.12f, 0.15f, 1.f };
	m_pContext->ClearRenderTargetView(m_pRTV, vClearColor);
	m_pContext->ClearDepthStencilView(m_pDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);

	return S_OK;
}

HRESULT CPanel_Viewport::End_SceneRender()
{
	if (nullptr == m_pContext)
		return E_FAIL;

	m_pContext->OMSetRenderTargets(1, &m_pPrevRTV, m_pPrevDSV);

	if (m_iPrevViewportCount > 0)
		m_pContext->RSSetViewports(1, &m_tPrevViewport);

	Safe_Release(m_pPrevRTV);
	m_pPrevRTV = nullptr;
	Safe_Release(m_pPrevDSV);
	m_pPrevDSV = nullptr;

	return S_OK;
}

HRESULT CPanel_Viewport::Create_RenderTarget()
{
	if (nullptr == m_pDevice)
		return E_FAIL;

	Release_RenderTarget();

	const _uint iWidth = max(static_cast<_uint>(m_vCanvasSize.x), 1u);
	const _uint iHeight = max(static_cast<_uint>(m_vCanvasSize.y), 1u);

	D3D11_TEXTURE2D_DESC tTextureDesc{};
	tTextureDesc.Width = iWidth;
	tTextureDesc.Height = iHeight;
	tTextureDesc.MipLevels = 1;
	tTextureDesc.ArraySize = 1;
	tTextureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	tTextureDesc.SampleDesc.Count = 1;
	tTextureDesc.SampleDesc.Quality = 0;
	tTextureDesc.Usage = D3D11_USAGE_DEFAULT;
	tTextureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

	if (FAILED(m_pDevice->CreateTexture2D(&tTextureDesc, nullptr, &m_pRTTexture)))
		return E_FAIL;
	if (FAILED(m_pDevice->CreateRenderTargetView(m_pRTTexture, nullptr, &m_pRTV)))
		return E_FAIL;
	if (FAILED(m_pDevice->CreateShaderResourceView(m_pRTTexture, nullptr, &m_pSRV)))
		return E_FAIL;

	D3D11_TEXTURE2D_DESC tDepthDesc{};
	tDepthDesc.Width = iWidth;
	tDepthDesc.Height = iHeight;
	tDepthDesc.MipLevels = 1;
	tDepthDesc.ArraySize = 1;
	tDepthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	tDepthDesc.SampleDesc.Count = 1;
	tDepthDesc.SampleDesc.Quality = 0;
	tDepthDesc.Usage = D3D11_USAGE_DEFAULT;
	tDepthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

	if (FAILED(m_pDevice->CreateTexture2D(&tDepthDesc, nullptr, &m_pDSTexture)))
		return E_FAIL;
	if (FAILED(m_pDevice->CreateDepthStencilView(m_pDSTexture, nullptr, &m_pDSV)))
		return E_FAIL;

	return S_OK;
}

void CPanel_Viewport::Release_RenderTarget()
{
	Safe_Release(m_pSRV);
	m_pSRV = nullptr;
	Safe_Release(m_pRTV);
	m_pRTV = nullptr;
	Safe_Release(m_pRTTexture);
	m_pRTTexture = nullptr;

	Safe_Release(m_pDSV);
	m_pDSV = nullptr;
	Safe_Release(m_pDSTexture);
	m_pDSTexture = nullptr;
}

void CPanel_Viewport::Handle_DebugPicking()
{
	if (m_pEditInstance->Get_UISession()->Get_VPMode()
		!= CUIEditorSession::VPMODE::SCENE) return;

	_float3 vRayOriginF3{}, vRayDirF3{};
	if (false == Build_MouseRay(&vRayOriginF3, &vRayDirF3))
	{
		m_bHasLastHit = false;
		m_strPickDebug = "Pick Failed : Ray Build Failed";
		m_strPickTarget = "Target : None";
		return;
	}

	XMVECTOR vRayOrigin = XMLoadFloat3(&vRayOriginF3);
	XMVECTOR vRayDir = XMVector3Normalize(XMLoadFloat3(&vRayDirF3));

	const vector<EDITOR_OBJECT_ENTRY>& Entries = m_pEditInstance->Get_EditorObjectEntries();

	_bool bHit = false;
	_float fMinDist = FLT_MAX;
	_float3 vBestLocalHit{};
	_float3 vBestWorldHit{};
	_float3 vBestObjectPos{};
	_string strBestTarget = "Target : None";

	for (const EDITOR_OBJECT_ENTRY& tEntry : Entries)
	{
		if (m_pEditInstance->Is_PlaceMode() || m_pEditInstance->Is_NavEditMode())
		{
			if (false == tEntry.bPlacementSurface)
				continue;
		}
		else
		{
			if (false == tEntry.bSelectable)
				continue;
		}

		CGameObject* pObj = tEntry.pObj;
		CModel* pModel = tEntry.pModel;

		if (nullptr == pObj || nullptr == pModel)
			continue;

		CTransform* pTransform = pObj->Get_Transform();
		if (nullptr == pTransform)
			continue;

		_float3 vLocalHitPos{};
		_float3 vWorldHitPos{};

		if (Pick_ModelObject(pObj, pModel, vRayOrigin, vRayDir, &vLocalHitPos, &vWorldHitPos))
		{
			const _float fHitDist = XMVectorGetX(XMVector3Length(XMLoadFloat3(&vWorldHitPos) - vRayOrigin));

			if (fHitDist < fMinDist)
			{
				fMinDist = fHitDist;
				bHit = true;

				vBestLocalHit = vLocalHitPos;
				vBestWorldHit = vWorldHitPos;

				_vector vObjPos = pTransform->Get_State(STATE::POSITION);
				XMStoreFloat3(&vBestObjectPos, vObjPos);

				strBestTarget = "Target : " + WtoS(pObj->Get_Name());
				m_pLastPickedObject = pObj;
			}
		}
	}

	if (!bHit)
	{
		m_bHasLastHit = false;
		m_pLastPickedObject = nullptr;
		m_strPickDebug = "Pick Result : No Hit";
		m_strPickTarget = "Target : None";
		return;
	}

	m_bHasLastHit = true;
	m_vLastObjectPos = vBestObjectPos;
	m_vLastLocalHitPos = vBestLocalHit;
	m_vLastWorldHitPos = vBestWorldHit;
	m_strPickDebug = "Pick Result : Hit";
	m_strPickTarget = strBestTarget;
}

_bool CPanel_Viewport::Build_MouseRay(_float3* pOutOrigin, _float3* pOutDir) const
{
	if (nullptr == pOutOrigin || nullptr == pOutDir)
		return false;

	ImVec2 vMouse = ImGui::GetMousePos();

	const _float fLocalX = vMouse.x - m_vDisplayPos.x;
	const _float fLocalY = vMouse.y - m_vDisplayPos.y;

	if (fLocalX < 0.f || fLocalY < 0.f || fLocalX > m_vDisplaySize.x || fLocalY > m_vDisplaySize.y)
		return false;

	const _float fNdcX = (2.f * fLocalX / m_vDisplaySize.x) - 1.f;
	const _float fNdcY = 1.f - (2.f * fLocalY / m_vDisplaySize.y);

	const _float4x4* pProjInv = m_pGameInstance->Get_Transform_Inverse(D3DTS::PROJ);
	const _float4x4* pViewInv = m_pGameInstance->Get_Transform_Inverse(D3DTS::VIEW);

	if (nullptr == pProjInv || nullptr == pViewInv)
		return false;

	_vector vNear = XMVector3TransformCoord(
		XMVectorSet(fNdcX, fNdcY, 0.f, 1.f),
		XMLoadFloat4x4(pProjInv));

	_vector vFar = XMVector3TransformCoord(
		XMVectorSet(fNdcX, fNdcY, 1.f, 1.f),
		XMLoadFloat4x4(pProjInv));

	_matrix matViewInv = XMLoadFloat4x4(pViewInv);

	_vector vWorldNear = XMVector3TransformCoord(vNear, matViewInv);
	_vector vWorldFar = XMVector3TransformCoord(vFar, matViewInv);

	XMStoreFloat3(pOutOrigin, vWorldNear);

	XMVECTOR vWorldDir = XMVector3Normalize(vWorldFar - vWorldNear);
	XMStoreFloat3(pOutDir, vWorldDir);

	return true;
}

_bool CPanel_Viewport::Pick_ModelObject(CGameObject* pObj, CModel* pModel, _fvector vRayOrigin, _fvector vRayDir, _float3* pOutLocalHitPos, _float3* pOutWorldHitPos) const
{
	if (nullptr == pObj || nullptr == pModel || nullptr == pOutLocalHitPos || nullptr == pOutWorldHitPos)
		return false;

	CTransform* pTransform = pObj->Get_Transform();
	if (nullptr == pTransform)
		return false;

	XMMATRIX matWorld = XMLoadFloat4x4(pTransform->Get_WorldMatrixPtr());

	_bool bHit = false;
	_float fMinDist = FLT_MAX;

	for (_uint i = 0; i < static_cast<_uint>(pModel->Get_NumMeshes()); ++i)
	{
		CMesh* pMesh = pModel->Get_Mesh(i);
		if (nullptr == pMesh)
			continue;

		BoundingBox worldAABB;
		pMesh->Get_LocalAABB().Transform(worldAABB, matWorld);

		_float fAABBDist = 0.f;
		if (!worldAABB.Intersects(vRayOrigin, vRayDir, fAABBDist))
			continue;

		const auto& Positions = pMesh->Get_Positions();
		const auto& Indices = pMesh->Get_Indices();

		if (Positions.empty() || Indices.empty())
			continue;

		XMMATRIX matWorldInv = XMMatrixInverse(nullptr, matWorld);
		XMVECTOR vLocalOrigin = XMVector3TransformCoord(vRayOrigin, matWorldInv);
		XMVECTOR vLocalDir = XMVector3Normalize(XMVector3TransformNormal(vRayDir, matWorldInv));

		for (_uint j = 0; j < static_cast<_uint>(Indices.size()); j += 3)
		{
			XMVECTOR v0 = XMLoadFloat3(&Positions[Indices[j]]);
			XMVECTOR v1 = XMLoadFloat3(&Positions[Indices[j + 1]]);
			XMVECTOR v2 = XMLoadFloat3(&Positions[Indices[j + 2]]);

			_float fDist = 0.f;
			if (TriangleTests::Intersects(vLocalOrigin, vLocalDir, v0, v1, v2, fDist))
			{
				if (fDist < fMinDist)
				{
					fMinDist = fDist;

					XMVECTOR vLocalHit = vLocalOrigin + vLocalDir * fDist;
					XMVECTOR vWorldHit = XMVector3TransformCoord(vLocalHit, matWorld);

					XMStoreFloat3(pOutLocalHitPos, vLocalHit);
					XMStoreFloat3(pOutWorldHitPos, vWorldHit);

					bHit = true;
				}
			}
		}
	}

	return bHit;
}

void CPanel_Viewport::Handle_ViewportClick()
{
	if (m_pEditInstance->Get_UISession()->Get_VPMode()
		!= CUIEditorSession::VPMODE::SCENE) return;

	Handle_DebugPicking();
	if (!m_bHasLastHit)
		return;

	// Nav 점 찍기 모드 — EditInstance를 통해 MapTool에 전달
	if (m_pEditInstance->Is_NavEditMode())
	{
		m_pEditInstance->Fire_NavClick(m_vLastWorldHitPos);
		return;
	}

	if (m_pEditInstance->Is_PlaceMode())
		Place_ObjectAtHit(m_vLastWorldHitPos);
	else
		Pick_SelectObject();
}

void CPanel_Viewport::Place_ObjectAtHit(const _float3& vHitPos)
{
	if (false == m_pEditInstance->Is_PlaceMode())
		return;

	const CATALOG_ITEM& tItem = m_pEditInstance->Get_PlaceItem();

	m_pEditInstance->Register_Object(
		tItem.iProtoLevel,
		tItem.strProtoTag,
		tItem.iLayerLevel,
		tItem.strLayerTag,
		nullptr);

	const auto& EditorObjects = m_pEditInstance->Get_EditorObjects();
	if (EditorObjects.empty())
		return;

	CGameObject* pNewObj = EditorObjects.back();
	if (nullptr == pNewObj)
		return;

	CTransform* pTransform = pNewObj->Get_Transform();
	if (nullptr == pTransform)
		return;

	pTransform->Set_State(STATE::POSITION, XMVectorSet(vHitPos.x, vHitPos.y, vHitPos.z, 1.f));
	m_pEditInstance->Select(pNewObj, false);
}

void CPanel_Viewport::Pick_SelectObject()
{
	if (nullptr == m_pLastPickedObject)
		return;

	m_pEditInstance->Select(m_pLastPickedObject, false);
}

void CPanel_Viewport::Handle_UIPick()
{
	auto* pHost = m_pEditInstance->Get_UIPreviewHost();
	auto* pSess = m_pEditInstance->Get_UISession();
	if (!pHost || !pSess) return;

	const ImVec2 vMouse = ImGui::GetMousePos();
	const ImVec2 vDoc = ScreenToDoc(vMouse,
		m_vDisplayPos, m_vCanvasSize, m_vDisplaySize);

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

void CPanel_Viewport::Handle_UIDrag()
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
	const ImVec2 vMouse = ImGui::GetMousePos();
	const ImVec2 vNow = ScreenToDoc(vMouse,
		m_vDisplayPos, m_vCanvasSize, m_vDisplaySize);
	const ImVec2 vSt = ScreenToDoc(m_vDragStartMouse,
		m_vDisplayPos, m_vCanvasSize, m_vDisplaySize);
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

void CPanel_Viewport::Draw_UIOverlay()
{
	auto* pHost = m_pEditInstance->Get_UIPreviewHost();
	auto* pSess = m_pEditInstance->Get_UISession();
	if (nullptr == pHost || nullptr == pSess) return;

	const auto& vWidgets = pHost->Get_Widgets();
	const auto& vZIdx = pHost->Get_ZOrderIdx();
	if (vWidgets.empty()) return;

	ImDrawList* dl = ImGui::GetWindowDrawList();
	dl->PushClipRect(m_vDisplayPos,
		ImVec2(m_vDisplayPos.x + m_vDisplaySize.x,
			m_vDisplayPos.y + m_vDisplaySize.y),
		true);

	const _int iSelected = pSess->Get_SelectedWidget();
	const auto& vDocW = pSess->Get_Doc().vWidgets;

	// 1. 모든 widget bounding (z-order 오름차순)
	for (_int idx : vZIdx)
	{
		CUIObject* p = vWidgets[idx];
		if (!p) continue;

		const _float4 rc = p->Get_ScreenRect();
		ImVec2 vMin, vMax;
		DocRectToScreen(rc, m_vDisplayPos, m_vCanvasSize, m_vDisplaySize, &vMin, &vMax);

		ImU32 col = IM_COL32(180, 180, 180, 180);
		if (idx < (_int)vDocW.size())
		{
			switch (vDocW[idx].Get_Type())
			{
			case UI_TYPE::CONTAINER:   col = IM_COL32(160, 200, 255, 200); break;
			case UI_TYPE::IMAGE:       col = IM_COL32(200, 200, 200, 200); break;
			case UI_TYPE::TEXT:        col = IM_COL32(255, 220, 120, 200); break;
			case UI_TYPE::BUTTON:      col = IM_COL32(180, 255, 180, 200); break;
			case UI_TYPE::PROGRESSBAR: col = IM_COL32(255, 180, 180, 200); break;
			}
		}
		dl->AddRect(vMin, vMax, col, 0.f, 0, 1.f);
	}

	// 2. selected outline (가장 위)
	if (iSelected >= 0 && iSelected < (_int)vWidgets.size())
	{
		if (CUIObject* p = vWidgets[iSelected])
		{
			const _float4 rc = p->Get_ScreenRect();
			ImVec2 vMin, vMax;
			DocRectToScreen(rc, m_vDisplayPos, m_vCanvasSize, m_vDisplaySize, &vMin, &vMax);
			dl->AddRect(vMin, vMax, IM_COL32(255, 220, 0, 255), 0.f, 0, 2.5f);
		}
	}

	// 3. hover label
	if (m_bHovered)
	{
		const ImVec2 vMouse = ImGui::GetMousePos();
		const ImVec2 vDoc = ScreenToDoc(vMouse,
			m_vDisplayPos, m_vCanvasSize, m_vDisplaySize);
		const _string strHit = pHost->Hit_Test_TopMost(vDoc);
		if (!strHit.empty())
		{
			const _int iIdx = pSess->Find_WidgetIndexById(strHit);
			if (iIdx >= 0)
			{
				char szBuf[128];
				sprintf_s(szBuf, "[%s] %s",
					Engine::To_String(vDocW[iIdx].Get_Type()),
					vDocW[iIdx].strDisplayName.c_str());
				dl->AddText(ImVec2(vMouse.x + 12.f, vMouse.y + 12.f),
					IM_COL32(255, 255, 255, 240), szBuf);
			}
		}
	}

	dl->PopClipRect();
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

	Safe_Release(m_pPrevRTV);
	m_pPrevRTV = nullptr;
	Safe_Release(m_pPrevDSV);
	m_pPrevDSV = nullptr;

	Release_RenderTarget();

	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);
}
