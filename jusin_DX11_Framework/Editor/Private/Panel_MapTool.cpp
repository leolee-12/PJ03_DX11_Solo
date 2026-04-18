#include "Panel_MapTool.h"
#include "GameInstance.h"
#include "EditInstance.h"

CPanel_MapTool::CPanel_MapTool()
	: CPanel_Base()
{
}

HRESULT CPanel_MapTool::Initialize()
{
	m_strTitle = "Map";

	if (FAILED(__super::Initialize()))
		return E_FAIL;

	strcpy_s(m_szNavSavePath, "../../Resources/Navigation/NavMesh.nav");

	return S_OK;
}

void CPanel_MapTool::Update(_float fTimeDelta)
{
	if (!m_bNavEditMode || m_eToolMode != NAV_TOOL_MODE::MOVE || !m_bDragging)
		return;

	if (m_iDragVertex < 0 || m_iSelectedCell < 0 || m_iSelectedCell >=
		static_cast<_int>(m_NavCells.size()))
	{
		m_bDragging = false;
		return;
	}

	// 마우스 놓으면 드래그 종료
	if (m_pGameInstance->Mouse_Up(DIMB::LBUTTON))
	{
		m_bDragging = false;
		m_iDragVertex = -1;
		return;
	}

	// 마우스 누르고 있는 동안: EditInstance를 통해 현재 월드 히트 좌표를 가져와 꼭짓점 갱신
	// ※ 이 부분은 Viewport에서 매 프레임 히트 좌표를 갱신해주는 구조가 필요함 (6단계 참조)
	if (m_bHasDragHit)
	{
		_float3 vSnapped = Snap_Point(m_vDragHitPos);

		// 이 꼭짓점을 공유하는 모든 셀을 갱신 (스냅된 좌표끼리는 exact match)
		const _float3& vOld = m_NavCells[m_iSelectedCell][m_iDragVertex];
		for (auto& cell : m_NavCells)
		{
			for (_int v = 0; v < 3; ++v)
			{
				if (cell[v].x == vOld.x && cell[v].y == vOld.y && cell[v].z == vOld.z)
					cell[v] = vSnapped;
			}
		}

		m_bHasDragHit = false;
	}
}

HRESULT CPanel_MapTool::Render()
{
	if (!Begin_Panel())
	{
		End_Panel();
		return S_OK;
	}

	// ── Nav 편집 모드 헤더 ──
	ImGui::Checkbox("Nav Edit Mode", &m_bNavEditMode);

	if (m_bNavEditMode)
	{
		ImGui::Separator();

		// 도구 상태
		int iMode = static_cast<int>(m_eToolMode);
		ImGui::RadioButton("Point", &iMode, NAV_TOOL_MODE::POINT);  ImGui::SameLine();
		ImGui::RadioButton("Select", &iMode, NAV_TOOL_MODE::SELECT); ImGui::SameLine();
		ImGui::RadioButton("Move", &iMode, NAV_TOOL_MODE::MOVE);   ImGui::SameLine();
		ImGui::RadioButton("Remove", &iMode, NAV_TOOL_MODE::REMOVE);
		m_eToolMode = static_cast<NAV_TOOL_MODE>(iMode);

		if (m_eToolMode == NAV_TOOL_MODE::POINT || m_eToolMode == NAV_TOOL_MODE::MOVE)
			ImGui::SliderFloat("Snap Radius", &m_fSnapRadius, 0.01f, 2.f, "%.2f");

		ImGui::Spacing();

		// 상태 표시
		ImGui::Text("Pending : %d / 3", static_cast<_int>(m_PendingPoints.size()));
		ImGui::Text("Cells   : %d", static_cast<_int>(m_NavCells.size()));

		if (m_iSelectedCell >= 0 && m_iSelectedCell < static_cast<_int>(m_NavCells.size()))
		{
			ImGui::Separator();
			ImGui::Text("Selected Cell: %d", m_iSelectedCell);
			const auto& c = m_NavCells[m_iSelectedCell];
			ImGui::Text("  A(%.2f, %.2f, %.2f)", c[0].x, c[0].y, c[0].z);
			ImGui::Text("  B(%.2f, %.2f, %.2f)", c[1].x, c[1].y, c[1].z);
			ImGui::Text("  C(%.2f, %.2f, %.2f)", c[2].x, c[2].y, c[2].z);

			if (ImGui::Button("Delete Selected"))
			{
				m_NavCells.erase(m_NavCells.begin() + m_iSelectedCell);
				m_iSelectedCell = -1;
			}
		}

		ImGui::Spacing();

		// 편집 버튼
		if (ImGui::Button("Undo Last Cell"))
		{
			if (!m_NavCells.empty())
				m_NavCells.pop_back();
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel Pending"))
			m_PendingPoints.clear();
		ImGui::SameLine();
		if (ImGui::Button("Clear All"))
		{
			m_NavCells.clear();
			m_PendingPoints.clear();
		}

		ImGui::Separator();

		// 저장/로드
		ImGui::InputText("Path##nav", m_szNavSavePath, sizeof(m_szNavSavePath));

		if (ImGui::Button("Save .nav"))
			Save_NavMesh();
		ImGui::SameLine();
		if (ImGui::Button("Load .nav"))
			Load_NavMesh();

		ImGui::Separator();

		// 셀 목록 (간이)
		if (ImGui::TreeNode("Cell List"))
		{
			for (_int i = 0; i < static_cast<_int>(m_NavCells.size()); ++i)
			{
				const auto& c = m_NavCells[i];
				ImGui::Text("[%d] A(%.1f,%.1f,%.1f) B(%.1f,%.1f,%.1f) C(%.1f,%.1f,%.1f)",
					i,
					c[0].x, c[0].y, c[0].z,
					c[1].x, c[1].y, c[1].z,
					c[2].x, c[2].y, c[2].z);
			}
			ImGui::TreePop();
		}

		// ── 뷰포트 오버레이 렌더링 ──
		Render_NavOverlay();
	}

	End_Panel();
	return S_OK;
}

void CPanel_MapTool::Add_NavPoint(const _float3& vWorldPos)
{
	// 1. 스냅
	_float3 vSnapped = Snap_Point(vWorldPos);

	// 2. 누적
	m_PendingPoints.push_back(vSnapped);

	// 3. 3개 모이면 셀 확정
	if (m_PendingPoints.size() == 3)
	{
		// 와인딩 보정 (CCW, +Y 법선 기준)
		XMVECTOR vA = XMLoadFloat3(&m_PendingPoints[0]);
		XMVECTOR vB = XMLoadFloat3(&m_PendingPoints[1]);
		XMVECTOR vC = XMLoadFloat3(&m_PendingPoints[2]);
		XMVECTOR vCross = XMVector3Cross(vB - vA, vC - vA);

		if (XMVectorGetY(vCross) < 0.f)
			std::swap(m_PendingPoints[1], m_PendingPoints[2]);

		// 셀 저장
		array<_float3, 3> cell = { m_PendingPoints[0], m_PendingPoints[1], m_PendingPoints[2] };
		m_NavCells.push_back(cell);

		m_PendingPoints.clear();
	}
}

void CPanel_MapTool::Clear_PendingPoints()
{
	m_PendingPoints.clear();
}

void CPanel_MapTool::Handle_NavClick(const _float3& vWorldPos)
{
	switch (m_eToolMode)
	{
	case NAV_TOOL_MODE::POINT:
		Add_NavPoint(vWorldPos);
		break;

	case NAV_TOOL_MODE::SELECT:
	{
		_int iHit = HitTest_Cell(vWorldPos);
		m_iSelectedCell = iHit;  // -1이면 선택 해제
		break;
	}

	case NAV_TOOL_MODE::REMOVE:
	{
		_int iHit = HitTest_Cell(vWorldPos);
		if (iHit >= 0)
		{
			m_NavCells.erase(m_NavCells.begin() + iHit);
			if (m_iSelectedCell == iHit)
				m_iSelectedCell = -1;
			else if (m_iSelectedCell > iHit)
				--m_iSelectedCell;
		}
		break;
	}

	case NAV_TOOL_MODE::MOVE:
	{
		// 클릭 시: 드래그 대상 꼭짓점 결정
		_int iCellIdx = -1;
		_int iVtx = HitTest_Vertex(vWorldPos, &iCellIdx);
		if (iVtx >= 0)
		{
			m_iSelectedCell = iCellIdx;
			m_iDragVertex = iVtx;
			m_bDragging = true;
		}
		break;
	}
	}
}

HRESULT CPanel_MapTool::Ready_EditableTexture(const _tchar* pFileDir)
{
	//ID3D11Texture2D* pTexture2D = { nullptr };
	//D3D11_TEXTURE2D_DESC TextureDesc{};

	//TextureDesc.Width = 256;
	//TextureDesc.Height = 256;
	//TextureDesc.MipLevels = 1;
	//TextureDesc.ArraySize = 1;
	//TextureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	//TextureDesc.SampleDesc.Quality = 0;
	//TextureDesc.SampleDesc.Count = 1;
	//TextureDesc.Usage = D3D11_USAGE_STAGING;
	//TextureDesc.BindFlags = 0;
	//TextureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
	//TextureDesc.MiscFlags = 0;

	//_uint* pPixels = new _uint[256 * 256];

	//for (size_t i = 0; i < 256; i++)
	//{
	//	for (size_t j = 0; j < 256; j++)
	//	{
	//		_uint   iIndex = i * 256 + j;

	//		pPixels[iIndex] = 0xffffffff;
	//	}
	//}

	//D3D11_SUBRESOURCE_DATA InitialData{};
	//InitialData.pSysMem = pPixels;
	//InitialData.SysMemPitch = 256 * 4;

	//if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, &InitialData, &pTexture2D)))
	//	return E_FAIL;

	//D3D11_MAPPED_SUBRESOURCE SubResource{};

	//if (FAILED(m_pContext->Map(pTexture2D, 0, D3D11_MAP_READ_WRITE, 0, &SubResource)))
	//	return E_FAIL;

	//_uint* pTexturePixels = static_cast<_uint*>(SubResource.pData);

	//for (size_t i = 0; i < 256; i++)
	//{
	//	for (size_t j = 0; j < 256; j++)
	//	{
	//		_uint   iIndex = i * 256 + j;

	//		if (j < 128)
	//			pTexturePixels[iIndex] = D3DCOLOR_ARGB(255, 255, 255, 255); /* a, b, g, r */
	//		else
	//			pTexturePixels[iIndex] = D3DCOLOR_ARGB(255, 0, 0, 0);
	//	}
	//}

	//m_pContext->Unmap(pTexture2D, 0);

	//SaveDDSTextureToFile(m_pContext, pTexture2D, pFileDir);

	return S_OK;
}

_float3 CPanel_MapTool::Snap_Point(const _float3& vInput) const
{
	_float  fMinDist = m_fSnapRadius;
	_float3 vResult = vInput;

	auto TrySnap = [&](const _float3& vCandidate)
		{
			_float dx = vCandidate.x - vInput.x;
			_float dy = vCandidate.y - vInput.y;
			_float dz = vCandidate.z - vInput.z;
			_float fDist = sqrtf(dx * dx + dy * dy + dz * dz);
			if (fDist < fMinDist)
			{
				fMinDist = fDist;
				vResult = vCandidate;
			}
		};

	for (const auto& cell : m_NavCells)
		for (const auto& v : cell)
			TrySnap(v);

	for (const auto& v : m_PendingPoints)
		TrySnap(v);

	return vResult;
}

void CPanel_MapTool::Render_NavOverlay()
{
	// GetForegroundDrawList: 모든 ImGui 윈도우 위에 그림
	ImDrawList* pDraw = ImGui::GetForegroundDrawList(ImGui::GetWindowViewport());

	ImVec2 vViewPos = m_pEditInstance->Get_ViewportScreenPos();
	ImVec2 vViewSize = m_pEditInstance->Get_ViewportScreenSize();

	// ── 뷰포트 영역으로 클립 ──
	ImVec2 vMin = vViewPos;
	ImVec2 vMax = ImVec2(vViewPos.x + vViewSize.x, vViewPos.y + vViewSize.y);
	pDraw->PushClipRect(vMin, vMax, true);   // true = 현재 클립과 교집합

	// 뷰포트 범위 안인지 확인용
	auto IsInViewport = [&](const ImVec2& p) -> _bool
		{
			return p.x >= vViewPos.x && p.x <= vViewPos.x + vViewSize.x
				&& p.y >= vViewPos.y && p.y <= vViewPos.y + vViewSize.y;
		};

	// 확정 셀 — 초록 와이어프레임
	for (const auto& cell : m_NavCells)
	{
		ImVec2 p0 = Project_To_Screen(cell[0]);
		ImVec2 p1 = Project_To_Screen(cell[1]);
		ImVec2 p2 = Project_To_Screen(cell[2]);

		pDraw->AddTriangle(p0, p1, p2, IM_COL32(0, 220, 60, 200), 1.5f);
	}

	// 선택 셀 — 노란색 하이라이트 + 꼭짓점 표시
	if (m_iSelectedCell >= 0 && m_iSelectedCell < static_cast<_int>(m_NavCells.size()))
	{
		const auto& sel = m_NavCells[m_iSelectedCell];
		ImVec2 s0 = Project_To_Screen(sel[0]);
		ImVec2 s1 = Project_To_Screen(sel[1]);
		ImVec2 s2 = Project_To_Screen(sel[2]);

		// 노란 삼각형 (두꺼운 선)
		pDraw->AddTriangle(s0, s1, s2, IM_COL32(255, 220, 0, 255), 2.5f);

		// 꼭짓점 원 (Move 모드에서 드래그 가능 피드백)
		const _float fVtxRadius = (m_eToolMode == NAV_TOOL_MODE::MOVE) ? 7.f : 4.f;
		pDraw->AddCircleFilled(s0, fVtxRadius, IM_COL32(255, 255, 0, 255));
		pDraw->AddCircleFilled(s1, fVtxRadius, IM_COL32(255, 255, 0, 255));
		pDraw->AddCircleFilled(s2, fVtxRadius, IM_COL32(255, 255, 0, 255));

		// 꼭짓점 라벨
		pDraw->AddText(ImVec2(s0.x + 8, s0.y - 8), IM_COL32(255, 255, 255, 255), "A");
		pDraw->AddText(ImVec2(s1.x + 8, s1.y - 8), IM_COL32(255, 255, 255, 255), "B");
		pDraw->AddText(ImVec2(s2.x + 8, s2.y - 8), IM_COL32(255, 255, 255, 255), "C");
	}

	// 대기 중인 점 — 빨간 점 + 연결선
	for (_int i = 0; i < static_cast<_int>(m_PendingPoints.size()); ++i)
	{
		ImVec2 p = Project_To_Screen(m_PendingPoints[i]);
		if (!IsInViewport(p)) continue;

		pDraw->AddCircleFilled(p, 5.f, IM_COL32(255, 80, 80, 255));

		if (i > 0)
		{
			ImVec2 prev = Project_To_Screen(m_PendingPoints[i - 1]);
			pDraw->AddLine(prev, p, IM_COL32(255, 80, 80, 200), 1.5f);
		}
	}

	pDraw->PopClipRect();
}

ImVec2 CPanel_MapTool::Project_To_Screen(const _float3& vWorldPos) const
{
	const _float4x4* pView = m_pGameInstance->Get_Transform(D3DTS::VIEW);
	const _float4x4* pProj = m_pGameInstance->Get_Transform(D3DTS::PROJ);
	if (!pView || !pProj)
		return ImVec2(-9999.f, -9999.f);

	XMMATRIX matVP = XMLoadFloat4x4(pView) * XMLoadFloat4x4(pProj);
	XMVECTOR vClip = XMVector4Transform(
		XMVectorSet(vWorldPos.x, vWorldPos.y, vWorldPos.z, 1.f), matVP);

	_float w = XMVectorGetW(vClip);
	// w < 0 이면 카메라 뒤쪽
	if (w < 1e-5f)
		return ImVec2(-9999.f, -9999.f);

	_float ndcX = XMVectorGetX(vClip) / w;
	_float ndcY = XMVectorGetY(vClip) / w;

	ImVec2 vPos = m_pEditInstance->Get_ViewportScreenPos();
	ImVec2 vSize = m_pEditInstance->Get_ViewportScreenSize();

	return ImVec2(
		vPos.x + (ndcX * 0.5f + 0.5f) * vSize.x,
		vPos.y + (1.f - (ndcY * 0.5f + 0.5f)) * vSize.y
	);
}

void CPanel_MapTool::Save_NavMesh()
{
	if (m_NavCells.empty() || m_szNavSavePath[0] == '\0')
		return;

	// char → wstring 변환
	int iLen = MultiByteToWideChar(CP_ACP, 0, m_szNavSavePath, -1, nullptr, 0);
	std::wstring wPath(iLen, L'\0');
	MultiByteToWideChar(CP_ACP, 0, m_szNavSavePath, -1, wPath.data(), iLen);

	HANDLE hFile = CreateFile(wPath.c_str(), GENERIC_WRITE, 0, nullptr,
		CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
	if (INVALID_HANDLE_VALUE == hFile)
	{
		MSG_BOX("Failed to Save NavMesh");
		return;
	}

	DWORD dwByte = 0;
	for (const auto& cell : m_NavCells)
		WriteFile(hFile, cell.data(), sizeof(_float3) * 3, &dwByte, nullptr);

	CloseHandle(hFile);
}

void CPanel_MapTool::Load_NavMesh()
{
	if (m_szNavSavePath[0] == '\0')
		return;

	int iLen = MultiByteToWideChar(CP_ACP, 0, m_szNavSavePath, -1, nullptr, 0);
	std::wstring wPath(iLen, L'\0');
	MultiByteToWideChar(CP_ACP, 0, m_szNavSavePath, -1, wPath.data(), iLen);

	HANDLE hFile = CreateFile(wPath.c_str(), GENERIC_READ, 0, nullptr,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
	if (INVALID_HANDLE_VALUE == hFile)
	{
		MSG_BOX("Failed to Load NavMesh");
		return;
	}

	m_NavCells.clear();
	m_PendingPoints.clear();

	while (true)
	{
		array<_float3, 3> cell{};
		DWORD dwByte = 0;
		ReadFile(hFile, cell.data(), sizeof(_float3) * 3, &dwByte, nullptr);
		if (dwByte == 0)
			break;
		m_NavCells.push_back(cell);
	}

	CloseHandle(hFile);
}

_int CPanel_MapTool::HitTest_Cell(const _float3& vWorldPos) const
{
	// XZ 평면 기준 삼각형 내부 판정 (CCW 와인딩 전제)
	for (_int i = 0; i < static_cast<_int>(m_NavCells.size()); ++i)
	{
		const auto& c = m_NavCells[i];

		// 엣지 AB, BC, CA의 외적 Y 부호가 모두 같으면 내부
		auto Cross2D = [](const _float3& a, const _float3& b, const _float3& p) -> _float
			{
				return (b.x - a.x) * (p.z - a.z) - (b.z - a.z) * (p.x - a.x);
			};

		_float d0 = Cross2D(c[0], c[1], vWorldPos);
		_float d1 = Cross2D(c[1], c[2], vWorldPos);
		_float d2 = Cross2D(c[2], c[0], vWorldPos);

		_bool bHasNeg = (d0 < 0) || (d1 < 0) || (d2 < 0);
		_bool bHasPos = (d0 > 0) || (d1 > 0) || (d2 > 0);

		if (!(bHasNeg && bHasPos))
			return i;
	}
	return -1;
}

_int CPanel_MapTool::HitTest_Vertex(const _float3& vWorldPos, _int* pOutCellIdx) const
{
	_float fMinDist = m_fSnapRadius * 2.f;  // 꼭짓점 선택은 스냅보다 넉넉하게
	_int iBestVtx = -1;
	_int iBestCell = -1;

	for (_int i = 0; i < static_cast<_int>(m_NavCells.size()); ++i)
	{
		for (_int v = 0; v < 3; ++v)
		{
			_float dx = m_NavCells[i][v].x - vWorldPos.x;
			_float dz = m_NavCells[i][v].z - vWorldPos.z;
			_float fDist = sqrtf(dx * dx + dz * dz);
			if (fDist < fMinDist)
			{
				fMinDist = fDist;
				iBestVtx = v;
				iBestCell = i;
			}
		}
	}

	if (pOutCellIdx)
		*pOutCellIdx = iBestCell;

	return iBestVtx;
}

CPanel_MapTool* CPanel_MapTool::Create()
{
	CPanel_MapTool* pInstance = new CPanel_MapTool();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Create : CPanel_MapTool");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPanel_MapTool::Free()
{
	__super::Free();
}
