#include "Navigation.h"

#include "Cell.h"
#include "GameInstance.h"

const _float4x4* CNavigation::m_pParentMatrixPtr = { nullptr };

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
	auto pDesc = static_cast<NAVIGATION_DESC*>(pArg);

	m_iCurrentCellIndex = pDesc->iCurrentCellIndex;

	if (-1 == m_iCurrentCellIndex)
		m_pParentMatrixPtr = pDesc->pParentMatrix;

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
	if (0 == hFile)
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
				if (++iCnt > MAX_TRAVERSE) return false;

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

#ifdef _DEBUG
HRESULT CNavigation::Render()
{
	_float4x4 WorldMatrix = *m_pParentMatrixPtr;
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
		WorldMatrix._42 += DEBUG_Y_OFFSET;
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
