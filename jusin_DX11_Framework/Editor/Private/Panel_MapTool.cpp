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
		ImGui::Checkbox("Point Mode (Click to place)", &m_bNavPointMode);
		ImGui::SliderFloat("Snap Radius", &m_fSnapRadius, 0.01f, 2.f, "%.2f");

		ImGui::Spacing();

		// 상태 표시
		ImGui::Text("Pending : %d / 3", static_cast<_int>(m_PendingPoints.size()));
		ImGui::Text("Cells   : %d", static_cast<_int>(m_NavCells.size()));

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
	ImDrawList* pDraw = ImGui::GetForegroundDrawList();

	ImVec2 vViewPos = m_pEditInstance->Get_ViewportScreenPos();
	ImVec2 vViewSize = m_pEditInstance->Get_ViewportScreenSize();

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

		if (IsInViewport(p0) || IsInViewport(p1) || IsInViewport(p2))
			pDraw->AddTriangle(p0, p1, p2, IM_COL32(0, 220, 60, 200), 1.5f);
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
