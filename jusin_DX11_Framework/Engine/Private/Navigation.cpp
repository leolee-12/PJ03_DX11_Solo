#include "Navigation.h"
#include "Cell.h"
#include "GameInstance.h"

namespace
{
	constexpr _uint kInvalidCellLocal = static_cast<_uint>(-1);
	constexpr _uint kRandPointAttempts = 16;
	constexpr _uint kReachableAttempts = 8;
	constexpr _float kWanderProjectRadius = 2.f;

	inline _float RandomFloat_(_float fMin, _float fMax)
	{
		if (fMin >= fMax) return fMin;
		return fMin + (fMax - fMin) * (static_cast<_float>(rand()) / static_cast<_float>(RAND_MAX));
	}
}

CNavigation::CNavigation(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pNaviFilePath, const _tchar* pNeighborFilePath)
	: CComponent{ pDevice, pContext }
	, m_pNaviFilePath{ pNaviFilePath }
	, m_pNeighborFilePath{ pNeighborFilePath }
{
}

CNavigation::CNavigation(const CNavigation& Prototype)
	: CComponent{ Prototype }
	, m_pNaviFilePath{ Prototype.m_pNaviFilePath }
	, m_pNeighborFilePath{ Prototype.m_pNeighborFilePath }
	, m_Cells{ Prototype.m_Cells }
#ifdef _DEBUG
	, m_pShader{ Prototype.m_pShader }
#endif
{
#ifdef _DEBUG
	Safe_AddRef(m_pShader);
#endif

	for (auto& pCell : m_Cells)
		Safe_AddRef(pCell);
}

void CNavigation::Set_CurrentCellIndex(_int iCellIdx)
{
	if (0 <= iCellIdx && iCellIdx < static_cast<_int>(m_Cells.size()))
		m_iCurrentCellIndex = iCellIdx;
	else
		m_iCurrentCellIndex = -1;
}

_vector CNavigation::Get_CellPos()
{
	if (-1 == m_iCurrentCellIndex)
		return XMVectorZero();

	return m_Cells[m_iCurrentCellIndex]->Get_Center();
}

HRESULT CNavigation::Initialize_Prototype()
{
	_ulong dwByte = {};
	HANDLE hFile = CreateFile(m_pNaviFilePath, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	while (true)
	{
		_float3 vPoints[3] = {};

		ReadFile(hFile, vPoints, sizeof(_float3) * 3, &dwByte, nullptr);
		if (0 == dwByte)
			break;

		CCell* pCell = CCell::Create(m_pDevice, m_pContext, vPoints, static_cast<_uint>(m_Cells.size()));
		if (nullptr == pCell)
			return E_FAIL;

		m_Cells.push_back(pCell);
	}

	CloseHandle(hFile);

	_bool bLoadedNeighborFromFile = false;

	if (nullptr != m_pNeighborFilePath && m_pNeighborFilePath[0] != TEXT('\0'))
	{
		DWORD dwAttr = GetFileAttributes(m_pNeighborFilePath);
		if (dwAttr != INVALID_FILE_ATTRIBUTES && !(dwAttr & FILE_ATTRIBUTE_DIRECTORY))
		{
			if (SUCCEEDED(SetUp_NeighborsFromFile()))
				bLoadedNeighborFromFile = true;
		}
	}

	if (false == bLoadedNeighborFromFile)
		SetUp_Neighbors();

#ifdef _DEBUG
	m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../../ShaderFiles/Shader_Cell.hlsl"), VTXPOS::Elements, VTXPOS::iNumElements);
	if (nullptr == m_pShader)
		return E_FAIL;
#endif

	return S_OK;
}

HRESULT CNavigation::Initialize(void* pArg)
{
	if (nullptr == pArg)
	{
		Set_CurrentCellIndex(-1);
		return S_OK;
	}

	auto pDesc = static_cast<NAVIGATION_DESC*>(pArg);
	Set_CurrentCellIndex(pDesc->iCurrentCellIndex);

	return S_OK;
}

HRESULT CNavigation::SetUp_Neighbors()
{
	for (auto& pSourCell : m_Cells)
	{
		for (auto& pDestCell : m_Cells)
		{
			if (pSourCell == pDestCell)
				continue;

			if (true == pDestCell->Compare_Points(pSourCell->Get_Point(VTXPOINT::A), pSourCell->Get_Point(VTXPOINT::B)))
				pSourCell->Set_Neighbor(LINE::AB, pDestCell);
			else if (true == pDestCell->Compare_Points(pSourCell->Get_Point(VTXPOINT::B), pSourCell->Get_Point(VTXPOINT::C)))
				pSourCell->Set_Neighbor(LINE::BC, pDestCell);
			else if (true == pDestCell->Compare_Points(pSourCell->Get_Point(VTXPOINT::C), pSourCell->Get_Point(VTXPOINT::A)))
				pSourCell->Set_Neighbor(LINE::CA, pDestCell);
		}
	}

	return S_OK;
}

HRESULT CNavigation::SetUp_NeighborsFromFile()
{
	_ulong dwByte = {};
	HANDLE hFile = CreateFile(m_pNeighborFilePath, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	_int iNeighbors[3] = {};

	for (auto& pCell : m_Cells)
	{
		ReadFile(hFile, iNeighbors, sizeof(_int) * 3, &dwByte, nullptr);
		pCell->Set_Neighbors(iNeighbors);
	}

	CloseHandle(hFile);
	return S_OK;
}

_bool XM_CALLCONV CNavigation::Is_Move(_fvector vResultPos)
{
	if (-1 == m_iCurrentCellIndex)
		return false;

	_int iNeighborIndex = { -1 };

	if (true == m_Cells[m_iCurrentCellIndex]->Is_In(vResultPos, &iNeighborIndex))
		return true;
	else
	{
		if (-1 != iNeighborIndex)
		{
			_int iCnt{};
			while (true)
			{
				if (++iCnt > g_kMaxTraverse) return false;

				if (true == m_Cells[iNeighborIndex]->Is_In(vResultPos, &iNeighborIndex))
					break;

				if (-1 == iNeighborIndex)
					return false;
			}

			m_iCurrentCellIndex = iNeighborIndex;
			return true;
		}
		else
			return false;
	}
}

_vector CNavigation::Compute_OnNavigation(const CTransform* pTargetTransform)
{
	if (-1 == m_iCurrentCellIndex)
		return XMVectorZero();

	_vector vCurrentPosition = pTargetTransform->Get_State(STATE::POSITION);

	_float fHeight = m_Cells[m_iCurrentCellIndex]->Compute_Height(vCurrentPosition);

	return XMVectorSetY(vCurrentPosition, fHeight);
}

_vector XM_CALLCONV CNavigation::Compute_Height(_fvector vPos) const
{
	if (-1 == m_iCurrentCellIndex)
		return vPos;

	_float fHeight = m_Cells[m_iCurrentCellIndex]->Compute_Height(vPos);
	return XMVectorSetY(vPos, fHeight);
}

_int XM_CALLCONV CNavigation::Find_CellIndex_ByPos(_fvector vWorldPos) const
{
	_float fMinHeightDiff = FLT_MAX;
	_int iBestIdx = -1;
	size_t iNumCells = m_Cells.size();

	for (_int i = 0; i < iNumCells; ++i)
	{
		_int iDummyNeighbor = -1;

		if (m_Cells[i]->Is_In(vWorldPos, &iDummyNeighbor))
		{
			const _float fCellHeight = m_Cells[i]->Compute_Height(vWorldPos);
			const _float fHeightDiff = fabsf(XMVectorGetY(vWorldPos) - fCellHeight);

			if (fHeightDiff < fMinHeightDiff)
			{
				fMinHeightDiff = fHeightDiff;
				iBestIdx = i;
			}
		}
	}

	return iBestIdx;
}

_vector XM_CALLCONV CNavigation::Compute_SlidePos(_fvector vCurPos, _fvector vDesiredPos)
{
	if (Is_Move(vDesiredPos))
		return vDesiredPos;

	_vector vNorm{};
	_int iCellIdx = m_iCurrentCellIndex;
	_int iTraverse{};

	while (true)
	{
		if (++iTraverse > g_kMaxTraverse) return vCurPos;

		_int iNeighbor{ -1 };
		if (m_Cells[iCellIdx]->Is_In(vDesiredPos, &iNeighbor, &vNorm))
			return vCurPos;

		if (-1 == iNeighbor)	// 진짜 벽을 만남
			break;				// vNorm에 그 벽 법선이 들어 있음

		iCellIdx = iNeighbor;	// 이웃 셀로 계속 추적
	}

	const _float fSlideScale = 0.5f;
	_vector vDelta = vDesiredPos - vCurPos;
	_vector vSlideDelta = vDelta - XMVector3Dot(vDelta, vNorm) * vNorm;
	_vector vSlidePos = vCurPos + vSlideDelta * fSlideScale;

	// 이동 가능 시 슬라이드, 아니면 원 위치
	return Is_Move(vSlidePos) ? vSlidePos : vCurPos;
}

_bool CNavigation::Project_PointToNavigation(
	const _float3& vWorldPos, _float fSearchRadius, _uint iAreaMask,
	_float3* pOutNavPos, _uint* pOutCellIndex) const
{
	if (nullptr == pOutNavPos || nullptr == pOutCellIndex)
		return false;

	(void)iAreaMask;   // 1차 no-op

	const _vector vPosVec = XMLoadFloat3(&vWorldPos);

	// 1차: XZ 평면상 직접 셀 내부 검색.
	const _int iDirect = Find_CellIndex_ByPos(vPosVec);
	if (-1 != iDirect)
	{
		const _float fHeight = m_Cells[iDirect]->Compute_Height(vPosVec);
		*pOutNavPos = _float3{ vWorldPos.x, fHeight, vWorldPos.z };
		*pOutCellIndex = static_cast<_uint>(iDirect);
		return true;
	}

	// 2차: fSearchRadius 안에서 가장 가까운 셀 중심.
	if (fSearchRadius <= 0.f)
		return false;
	const _float fRadiusSq = fSearchRadius * fSearchRadius;

	_int    iBestCell = -1;
	_float  fBestDistSq = FLT_MAX;
	_float3 vBestCenter = {};

	for (size_t i = 0; i < m_Cells.size(); ++i)
	{
		_float3 vCenter;
		XMStoreFloat3(&vCenter, m_Cells[i]->Get_Center());

		const _float fDx = vCenter.x - vWorldPos.x;
		const _float fDz = vCenter.z - vWorldPos.z;
		const _float fDistSq = fDx * fDx + fDz * fDz;

		if (fDistSq <= fRadiusSq && fDistSq < fBestDistSq)
		{
			fBestDistSq = fDistSq;
			iBestCell = static_cast<_int>(i);
			vBestCenter = vCenter;
		}
	}

	if (-1 == iBestCell)
		return false;

	*pOutNavPos = vBestCenter;
	*pOutCellIndex = static_cast<_uint>(iBestCell);
	return true;
}

_bool CNavigation::Is_Reachable(
	_uint iStartCellIndex, _uint iGoalCellIndex, _uint iAreaMask) const
{
	(void)iAreaMask;   // 1차 no-op

	if (iStartCellIndex >= m_Cells.size()) return false;
	if (iGoalCellIndex >= m_Cells.size()) return false;
	if (iStartCellIndex == iGoalCellIndex) return true;

	vector<_bool> bVisited(m_Cells.size(), false);
	queue<_uint>  oFrontier;

	oFrontier.push(iStartCellIndex);
	bVisited[iStartCellIndex] = true;

	while (!oFrontier.empty())
	{
		const _uint iCur = oFrontier.front();
		oFrontier.pop();

		_int* const pNeighbors = m_Cells[iCur]->Get_NeighborIndices();
		for (size_t i = 0; i < 3; ++i)
		{
			const _int iNeighbor = pNeighbors[i];
			if (-1 == iNeighbor) continue;

			const _uint iNeighU = static_cast<_uint>(iNeighbor);
			if (iNeighU >= m_Cells.size()) continue;
			if (bVisited[iNeighU])         continue;

			if (iNeighU == iGoalCellIndex)
				return true;

			bVisited[iNeighU] = true;
			oFrontier.push(iNeighU);
		}
	}
	return false;
}

_bool CNavigation::Find_RandomPoint_InRect(
	const _float3& vCenter, const _float2& vSize, _float fRotationY,
	_float fProjectRadius, _uint iAreaMask,
	_float3* pOutNavPos, _uint* pOutCellIndex) const
{
	if (nullptr == pOutNavPos || nullptr == pOutCellIndex)
		return false;

	const _float fHalfX = vSize.x * 0.5f;
	const _float fHalfZ = vSize.y * 0.5f;

	for (_uint i = 0; i < kRandPointAttempts; ++i)
	{
		const _float fLocalX = RandomFloat_(-fHalfX, fHalfX);
		const _float fLocalZ = RandomFloat_(-fHalfZ, fHalfZ);

		const _vector vLocal = XMVectorSet(fLocalX, 0.f, fLocalZ, 0.f);
		const _vector vRotated = XMVector3TransformCoord(vLocal, XMMatrixRotationY(fRotationY));
		const _vector vWorld = vRotated + XMLoadFloat3(&vCenter);

		_float3 vCandidate{};
		XMStoreFloat3(&vCandidate, vWorld);

		if (Project_PointToNavigation(vCandidate, fProjectRadius, iAreaMask, pOutNavPos, pOutCellIndex))
			return true;
	}
	return false;
}

_bool CNavigation::Find_RandomReachablePoint_InRadius(
	const _float3& vOrigin, _uint iOriginCellIndex,
	_float fRadius, _uint iAreaMask,
	_float3* pOutNavPos, _uint* pOutCellIndex) const
{
	if (nullptr == pOutNavPos || nullptr == pOutCellIndex) return false;
	if (fRadius <= 0.f)                                    return false;

	for (_uint i = 0; i < kReachableAttempts; ++i)
	{
		const _float fAngle = RandomFloat_(0.f, XM_2PI);
		const _float fDist = RandomFloat_(0.f, fRadius);

		const _float3 vCandidate{
				vOrigin.x + cosf(fAngle) * fDist,
				vOrigin.y,
				vOrigin.z + sinf(fAngle) * fDist
		};

		_float3 vNavPos = {};
		_uint   iCellIndex = kInvalidCellLocal;

		if (!Project_PointToNavigation(vCandidate, kWanderProjectRadius, iAreaMask, &vNavPos, &iCellIndex))
			continue;
		if (!Is_Reachable(iOriginCellIndex, iCellIndex, iAreaMask))
			continue;

		*pOutNavPos = vNavPos;
		*pOutCellIndex = iCellIndex;
		return true;
	}
	return false;
}

#ifdef _DEBUG
HRESULT CNavigation::Render()
{
	_float4x4 WorldMatrix{};
	XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());
	_float4 vColor = _float4(0.f, 1.f, 0.f, 1.f);

	m_pShader->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform(D3DTS::VIEW));
	m_pShader->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform(D3DTS::PROJ));

	if (-1 == m_iCurrentCellIndex)
	{
		m_pShader->Bind_Matrix("g_WorldMatrix", &WorldMatrix);
		m_pShader->Bind_RawValue("g_vColor", &vColor, sizeof vColor);
		m_pShader->Begin(0);

		for (auto& pCell : m_Cells)
		{
			if (nullptr != pCell)
				pCell->Render();
		}
	}

	else
	{
		WorldMatrix._42 += g_kDebugOffset_Y;
		vColor = _float4(1.f, 0.f, 0.f, 1.f);

		m_pShader->Bind_Matrix("g_WorldMatrix", &WorldMatrix);
		m_pShader->Bind_RawValue("g_vColor", &vColor, sizeof vColor);
		m_pShader->Begin(0);

		m_Cells[m_iCurrentCellIndex]->Render();
	}

	return S_OK;
}
#endif

CNavigation* CNavigation::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pNaviFilePath, const _tchar* pNeighborFilePath)
{
	CNavigation* pInstance = new CNavigation(pDevice, pContext, pNaviFilePath, pNeighborFilePath);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CNavigation");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CNavigation::Clone(void* pArg)
{
	CNavigation* pInstance = new CNavigation(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CNavigation");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CNavigation::Free()
{
	__super::Free();

	for (auto& pCell : m_Cells)
		Safe_Release(pCell);

#ifdef _DEBUG
	Safe_Release(m_pShader);
#endif
}
