//#include "..\Public\Navigation.h"
//#include "Cell.h"
//#include "GameInstance.h"
//
//IMPLEMENT_CREATE_EX2(CNavigation, _uint ,iNumLevels, const wstring& ,strNavigationFilePath)
//IMPLEMENT_CLONE(CNavigation,CComponent)
//
//
//_float4x4 CNavigation::m_WorldMatrix = { };
//vector<class CCell*>* CNavigation::m_Cells;
//
//CNavigation::CNavigation(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
//	: CComponent(pDevice, pContext)
//{
//
//}
//
//CNavigation::CNavigation(const CNavigation& rhs)
//	: CComponent(rhs)
//	, m_iNumLevels(rhs.m_iNumLevels)
//	, m_iCurrentLevel(rhs.m_iCurrentLevel)
//#ifdef _DEBUG
//	, m_pShader(rhs.m_pShader)
//#endif
//{
//	//for (auto& pCell : m_Cells[m_iCurrentLevel])
//	//	Safe_AddRef(pCell);
//#ifdef _DEBUG
//	Safe_AddRef(m_pShader);
//#endif
//
//}
//
//HRESULT CNavigation::Initialize_Prototype(_uint iNumLevels, const wstring& strNavigationFilePath)
//{
//	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());
//	m_iNumLevels = iNumLevels;
//	m_Cells = new vector<CCell*>[iNumLevels] ;
//
//	if (!strNavigationFilePath.empty())
//	{
//		for (_uint i = 0; i < iNumLevels; i++)
//		{
//			_tchar		szFullPath[MAX_PATH] = TEXT("");
//			wsprintf(szFullPath, strNavigationFilePath.c_str(), i);
//
//			ifstream fin;
//			fin.open(szFullPath, std::ios::binary);
//			if (fin.is_open())
//			{
//				Load_Binary(fin);
//			}
//		}
//	}
//
//	/*HANDLE		hFile = CreateFile(strNavigationFilePath.c_str(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
//	if (0 == hFile)
//		RETURN_EFAIL;
//
//	_ulong		dwByte = { 0 };
//
//	while (true)
//	{
//		_float3		vPoints[3];
//
//		ReadFile(hFile, vPoints, sizeof(_float3) * 3, &dwByte, nullptr);
//		if (0 == dwByte)
//			break;
//
//		CCell* pCell = CCell::Create(m_pDevice, m_pContext, vPoints, m_Cells[m_iCurrentLevel].size());
//		if (nullptr == pCell)
//			RETURN_EFAIL;
//
//		m_Cells[m_iCurrentLevel].push_back(pCell);
//	}
//	   
//	CloseHandle(hFile);
//
//	if (FAILED(Make_Neighbors()))
//		RETURN_EFAIL;*/
//
//#ifdef _DEBUG
//	m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("Shader_Navigation"), VTXPOS::Elements, VTXPOS::iNumElements);
//	if (nullptr == m_pShader)
//		RETURN_EFAIL;
//#endif
//
//	return S_OK;
//}
//
//HRESULT CNavigation::Initialize(void* pArg)
//{
//	if (nullptr != pArg)
//	{
//		m_iCurrentIndex = ((NAVI_DESC*)pArg)->iCurrentIndex;
//	}
//		m_iCurrentLevel = m_pGameInstance->Get_CreateLevelIndex();
//
//	return S_OK;
//}
//
//HRESULT CNavigation::Render()
//{
//
//	if (m_pGameInstance->Get_CreateLevelIndex() == 1)
//	{
//
//		if (!m_Cells[m_iCurrentLevel].empty())
//		{
//			/*#임시_네비메쉬_월드_항등세팅*/
//			XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());
//
//			/* 셀들의 위치가 월드상에 존재한다. */
//			_float4		vColor = { 0.0f, 0.f, 0.f, 1.f };
//		
//			//m_WorldMatrix.m[3][1] = m_iCurrentIndex == -1 ? m_WorldMatrix.m[3][1] : m_WorldMatrix.m[3][1] + 0.1f;
//		
//			if (FAILED(m_pShader.get()->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
//				RETURN_EFAIL;
//			if (FAILED(m_pShader.get()->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::TS_VIEW))))
//				RETURN_EFAIL;
//			if (FAILED(m_pShader.get()->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::TS_PROJ))))
//				RETURN_EFAIL;
//
//			//vColor = m_iCurrentIndex == -1 ? _float4(0.f, 1.f, 0.f, 1.f) : _float4(1.f, 0.f, 0.f, 1.f);
//			vColor = _float4(0.f, 1.f, 0.f, 1.f);
//			m_pShader.get()->Bind_RawValue("g_vColor", &vColor, sizeof(_float4));
//
//			m_pShader.get()->Begin(0);
//
//			for (auto& pCell : m_Cells[m_iCurrentLevel])
//			{
//				/*if(m_iCurrentIndex != -1)
//					if(pCell == m_Cells[m_iCurrentLevel][m_iCurrentIndex])
//						continue;*/
//
//				if (nullptr != pCell)
//					pCell->Render(m_pShader);
//			}
//		
//			m_WorldMatrix.m[3][1] = m_WorldMatrix.m[3][1] + 0.1f;
//			vColor = _float4(1.f, 0.f, 1.f, 1.f);
//			if (FAILED(m_pShader.get()->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
//				RETURN_EFAIL;
//			m_pShader.get()->Bind_RawValue("g_vColor", &vColor, sizeof(_float4));
//			m_pShader.get()->Begin(0);
//
//			if (-1 != m_iCurrentIndex)
//			{
//				m_Cells[m_iCurrentLevel][m_iCurrentIndex]->Render(m_pShader);
//				//goto Exit;
//			}
//		}
//	}
//	
//
//
////Exit:
//	return S_OK;
//}
//
//void CNavigation::Update(_fmatrix WorldMatrix)
//{
//	XMStoreFloat4x4(&m_WorldMatrix, WorldMatrix);
//}
//
//_bool CNavigation::isMove(_fvector vPosition, _float3* vLine)
//{
//	if (m_Cells[m_iCurrentLevel].empty())
//		return true;
//
//	_int		iNeighborIndex = { -1 };
//
//	if (true == m_Cells[m_iCurrentLevel][m_iCurrentIndex]->isIn(vPosition, XMLoadFloat4x4(&m_WorldMatrix), &iNeighborIndex, vLine))
//		return true;
//
//	else
//	{
//		if (-1 != iNeighborIndex)
//		{
//			while (true)
//			{
//				if (-1 == iNeighborIndex)
//					return false;
//				if (true == m_Cells[m_iCurrentLevel][iNeighborIndex]->isIn(vPosition, XMLoadFloat4x4(&m_WorldMatrix), &iNeighborIndex, vLine))
//				{
//					m_iCurrentIndex = iNeighborIndex;
//					return true;
//				}
//			}
//		}
//		else
//			return false;
//	}
//}
//
//_float CNavigation::Get_HeightOnNavigation(_fvector vPosition)
//{
//	if (m_Cells[m_iCurrentLevel].empty())
//		return XMVectorGetY(vPosition);
//
//	_vector vPlane = XMPlaneFromPoints(XMLoadFloat3(m_Cells[m_iCurrentLevel][m_iCurrentIndex]->Get_Point(CCell::POINT_A)), XMLoadFloat3(m_Cells[m_iCurrentLevel][m_iCurrentIndex]->Get_Point(CCell::POINT_B)), XMLoadFloat3(m_Cells[m_iCurrentLevel][m_iCurrentIndex]->Get_Point(CCell::POINT_C)));
//	
//	return (-XMVectorGetX(vPlane) * XMVectorGetX(vPosition) - XMVectorGetZ(vPlane) * XMVectorGetZ(vPosition) - XMVectorGetW(vPlane)) / XMVectorGetY(vPlane);
//}
//
//HRESULT CNavigation::Make_Neighbors()
//{
//	for (auto& pSourCell : m_Cells[m_iCurrentLevel])
//	{
//		_bool CheckNeighbor[3] = { false };
//		for (auto& pDestCell : m_Cells[m_iCurrentLevel])
//		{
//			if (pSourCell == pDestCell)
//				continue;
//
//			if (true == pDestCell->Compare_Points(pSourCell->Get_Point(CCell::POINT_A), pSourCell->Get_Point(CCell::POINT_B)))
//			{
//				pSourCell->SetUp_Neighbor(CCell::LINE_AB, pDestCell);
//				CheckNeighbor[CCell::LINE_AB] = true;
//			}
//
//			if (true == pDestCell->Compare_Points(pSourCell->Get_Point(CCell::POINT_B), pSourCell->Get_Point(CCell::POINT_C)))
//			{
//				pSourCell->SetUp_Neighbor(CCell::LINE_BC, pDestCell);
//				CheckNeighbor[CCell::LINE_BC] = true;
//			}
//			
//			if (true == pDestCell->Compare_Points(pSourCell->Get_Point(CCell::POINT_C), pSourCell->Get_Point(CCell::POINT_A)))
//			{
//				pSourCell->SetUp_Neighbor(CCell::LINE_CA, pDestCell);
//				CheckNeighbor[CCell::LINE_CA] = true;
//			}
//		}
//
//		for (_uint i = 0; i < 3; i++)
//		{
//			if(!CheckNeighbor[i])
//				pSourCell->SetUp_Neighbor((CCell::LINE)i, -1);
//		}
//	}
//
//	return S_OK;
//}
//
//void CNavigation::Reset_Cells()
//{
//	for (auto& pCell : m_Cells[m_iCurrentLevel])
//		Safe_Release(pCell);
//	m_Cells[m_iCurrentLevel].clear();
//}
//
//HRESULT CNavigation::Bake_Cells(vector<array<_float3, 3>>* pCellPoints)
//{
//	//for (auto& pCell : m_Cells[m_iCurrentLevel])
//	//	Safe_Release(pCell);
//	//m_Cells[m_iCurrentLevel].clear();
//
//	for (auto iter : *pCellPoints)
//	{
//		_float3		vPoints[3] = { iter[0],iter[1],iter[2] };
//		
//		CCell* pCell = CCell::Create(m_pDevice, m_pContext, vPoints,(_uint)m_Cells[m_iCurrentLevel].size());
//		if (nullptr == pCell)
//			RETURN_EFAIL;
//
//		m_Cells[m_iCurrentLevel].push_back(pCell);
//	}
//
//	if (FAILED(Make_Neighbors()))
//		RETURN_EFAIL;
//
//	return S_OK;
//}
//
//void CNavigation::Write_Json(json& Out_Json)
//{
//	Out_Json["CNavigation"]["CurrentIndex"] =  m_iCurrentIndex;
//
//	/*_int iIndex = 0;
//	for (const auto& iter : m_Cells[m_iCurrentLevel])
//	{
//		for (_int i = 0; i < 3; i++)
//		{
//			_float3 fCellPoint = *(iter)->Get_Point((CCell::POINT)i);
//			_float fTemp[3] = { fCellPoint.x, fCellPoint.y, fCellPoint.z };
//			Out_Json["CNavigation"]["CCell"][iIndex][i] = fTemp;
//		}
//		++iIndex;
//	}*/
//}
//
//void CNavigation::Load_Json(const json& In_Json)
//{
//	m_iCurrentIndex = In_Json["CNavigation"]["CurrentIndex"];
//	/*Reset_Cells();
//
//	for (const auto& iter : In_Json["CNavigation"]["CCell"])
//	{
//		_float3 fCellPoints[3] = {};
//		for (_int i = 0; i < 3; i++)
//		{
//			fCellPoints[i] = {iter[i][0],iter[i][1],iter[i][2]};
//		}
//
//		CCell* pCell = CCell::Create(m_pDevice, m_pContext, fCellPoints, m_Cells[m_iCurrentLevel].size());
//		m_Cells[m_iCurrentLevel].push_back(pCell);
//	}
//
//	Make_Neighbors();*/
//}
//
//#ifdef _DEBUG
//void CNavigation::Copy_Cell_LevelToLevel(const _uint& iDestLevel,const _uint& iSourLevel)
//{
//	for (auto& iter : m_Cells[iDestLevel])
//		Safe_Release(iter);
//	m_Cells[iDestLevel].clear();
//
//	for (auto& iter : m_Cells[iSourLevel])
//		m_Cells[iDestLevel].push_back(iter);
//}
//
//HRESULT CNavigation::Bake_Binary(ofstream& fout, _uint iLevelIndex, _uint iToolLevelIndex )
//{
//	size_t Size = m_Cells[iToolLevelIndex].size();
//	fout.write(reinterpret_cast<char*>(&iLevelIndex), sizeof(iLevelIndex));
//	fout.write(reinterpret_cast<char*>(&Size), sizeof(size_t));
//
//	for (auto& iter : m_Cells[iToolLevelIndex])
//	{		
//		iter->Bake_Binary(fout);
//	}
//	return S_OK;
//}
//#endif
//
//HRESULT CNavigation::Load_Binary(ifstream& fin)
//{
//	_uint iLevelIndex = 0;
//	size_t Size = 0;
//	_float3 vPoints[3] = {};
//	fin.read(reinterpret_cast<char*>(&iLevelIndex), sizeof(iLevelIndex));
//	fin.read(reinterpret_cast<char*>(&Size), sizeof(size_t));
//
//	for (size_t i = 0; i < Size; i++)
//	{
//		CCell* pCell = CCell::Create(m_pDevice, m_pContext, vPoints, 0);
//		if (nullptr == pCell)
//			RETURN_EFAIL;
//		pCell->Load_Binary(fin);
//		m_Cells[iLevelIndex].push_back(pCell);
//	}	
//
//	return S_OK;
//}
//
//
//void CNavigation::Free()
//{
//	__super::Free();
//
//	if (!m_isCloned)
//	{
//		for (_uint i = 0; i < m_iNumLevels; i++)
//		{
//			for (auto& pCell : m_Cells[i])
//				Safe_Release(pCell);
//			m_Cells[i].clear();
//		}
//		Safe_Delete_Array(m_Cells);
//	}
//
//}
