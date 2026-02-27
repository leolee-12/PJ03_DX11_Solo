//#include "..\Public\Cell.h"
//#include "Shader.h"
//#include "VIBuffer_Cell.h"
//
//CCell::CCell(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
//	: m_pDevice(pDevice)
//	, m_pContext(pContext)
//{
//}
//
//HRESULT CCell::Initialize(const _float3* pPoints, _uint iIndex)
//{
//	memcpy(m_vPoints, pPoints, sizeof(_float3) * POINT_END);
//
//	m_iIndex = iIndex;
//
//	Make_Line();
//
//#ifdef _DEBUG
//	m_pVIBuffer = CVIBuffer_Cell::Create(m_pDevice, m_pContext, pPoints);
//	if (nullptr == m_pVIBuffer)
//		RETURN_EFAIL;
//#endif
//
//	return S_OK;
//}
//
//_bool CCell::Compare_Points(const _float3* pSourPoint, const _float3* pDestPoint)
//{
//	if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_A]), XMLoadFloat3(pSourPoint)) || XMVectorGetX(XMVector3Length(XMLoadFloat3(&m_vPoints[POINT_A])-XMLoadFloat3(pSourPoint)))< 0.05f)
//	{
//		if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_B]), XMLoadFloat3(pDestPoint)) || XMVectorGetX(XMVector3Length(XMLoadFloat3(&m_vPoints[POINT_B]) - XMLoadFloat3(pDestPoint))) < 0.05f)
//			return true;
//
//		if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_C]), XMLoadFloat3(pDestPoint)) || XMVectorGetX(XMVector3Length(XMLoadFloat3(&m_vPoints[POINT_C]) - XMLoadFloat3(pDestPoint))) < 0.05f)
//			return true;
//	}
//	if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_B]), XMLoadFloat3(pSourPoint)) || XMVectorGetX(XMVector3Length(XMLoadFloat3(&m_vPoints[POINT_B]) - XMLoadFloat3(pSourPoint))) < 0.05f)
//	{
//		if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_C]), XMLoadFloat3(pDestPoint)) || XMVectorGetX(XMVector3Length(XMLoadFloat3(&m_vPoints[POINT_C]) - XMLoadFloat3(pDestPoint))) < 0.05f)
//			return true;
//
//		if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_A]), XMLoadFloat3(pDestPoint)) || XMVectorGetX(XMVector3Length(XMLoadFloat3(&m_vPoints[POINT_A]) - XMLoadFloat3(pDestPoint))) < 0.05f)
//			return true;
//	}
//	if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_C]), XMLoadFloat3(pSourPoint)) || XMVectorGetX(XMVector3Length(XMLoadFloat3(&m_vPoints[POINT_C]) - XMLoadFloat3(pSourPoint))) < 0.05f)
//	{
//		if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_A]), XMLoadFloat3(pDestPoint)) || XMVectorGetX(XMVector3Length(XMLoadFloat3(&m_vPoints[POINT_A]) - XMLoadFloat3(pDestPoint))) < 0.05f)
//			return true;
//
//		if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_B]), XMLoadFloat3(pDestPoint)) || XMVectorGetX(XMVector3Length(XMLoadFloat3(&m_vPoints[POINT_B]) - XMLoadFloat3(pDestPoint))) < 0.05f)
//			return true;
//	}
//
//	return false;
//}
//
//_bool CCell::isIn(_fvector vPosition, _fmatrix WorldMatrix, _int* pNeighborIndex, _float3* vLine)
//{
//	for (_uint i = 0; i <(_uint) LINE_END; i++)
//	{
//		_vector	vStartPoint = XMVector3TransformCoord(XMLoadFloat3(&m_vPoints[i]), WorldMatrix);
//		_vector	vNormal = XMVector3TransformNormal(XMLoadFloat3(&m_vNormals[i]), WorldMatrix);
//
//		_vector	vSourDir = XMVectorSetY(vPosition,0.f) - XMVectorSetY(vStartPoint,0.f) ;
//
//		if (0 < XMVectorGetX(XMVector3Dot(XMVector3Normalize(vSourDir),
//			XMVector3Normalize(vNormal))))
//		{
//			*pNeighborIndex = m_iNeighbors[i];
//			XMStoreFloat3(vLine, XMVector3Normalize(XMLoadFloat3(&m_vLine[i])));
//
//			return false;
//		}
//	}
//
//	return true;
//}
//
//_bool CCell::isIn(_fvector vPosition, _uint &iIndex)
//{
//	_vector vDir = XMVectorSet(0.f, -1.f, 0.f, 0.f);
//	_vector vPointA = XMLoadFloat3(&m_vPoints[POINT_A]);
//	_vector vPointB =  XMLoadFloat3(&m_vPoints[POINT_B]);
//	_vector vPointC =  XMLoadFloat3(&m_vPoints[POINT_C]);
//	_float fDist = 10.f;
//	if (DirectX::TriangleTests::Intersects(vPosition, vDir, vPointA, vPointB, vPointC, fDist))
//	{
//		iIndex = m_iIndex;
//		return true;
//	}
//	else
//		return false;
//}
//
//void CCell::Make_Line()
//{
//	XMStoreFloat3(&m_vLine[LINE_AB], XMLoadFloat3(&m_vPoints[POINT_B]) - XMLoadFloat3(&m_vPoints[POINT_A]));
//	XMStoreFloat3(&m_vNormals[LINE_AB], XMVectorSet(m_vLine[LINE_AB].z * -1.f, 0.f, m_vLine[LINE_AB].x, 0.f));
//
//	XMStoreFloat3(&m_vLine[LINE_BC], XMLoadFloat3(&m_vPoints[POINT_C]) - XMLoadFloat3(&m_vPoints[POINT_B]));
//	XMStoreFloat3(&m_vNormals[LINE_BC], XMVectorSet(m_vLine[LINE_BC].z * -1.f, 0.f, m_vLine[LINE_BC].x, 0.f));
//
//	XMStoreFloat3(&m_vLine[LINE_CA], XMLoadFloat3(&m_vPoints[POINT_A]) - XMLoadFloat3(&m_vPoints[POINT_C]));
//	XMStoreFloat3(&m_vNormals[LINE_CA], XMVectorSet(m_vLine[LINE_CA].z * -1.f, 0.f, m_vLine[LINE_CA].x, 0.f));
//}
//
//void CCell::Update(_fmatrix WorldMatrix)
//{
//
//}
//
//HRESULT CCell::Render(shared_ptr<CShader> pShader)
//{
//	m_pVIBuffer->Bind_VIBuffers();
//
//	m_pVIBuffer->Render();
//
//	return S_OK;
//}
//
//HRESULT CCell::Bake_Binary(ofstream& fout)
//{
//	fout.write(reinterpret_cast<char*>(&m_iIndex), sizeof(m_iIndex));
//	for (_uint i = 0; i < 3; i++)
//	{
//		fout.write(reinterpret_cast<char*>(&m_vPoints[i]), sizeof(m_vPoints[i]));
//		fout.write(reinterpret_cast<char*>(&m_iNeighbors[i]), sizeof(m_iNeighbors[i]));
//		fout.write(reinterpret_cast<char*>(&m_vNormals[i]), sizeof(m_vNormals[i]));
//	}
//	return S_OK;
//}
//
//HRESULT CCell::Load_Binary(ifstream& fin)
//{
//	fin.read(reinterpret_cast<char*>(&m_iIndex), sizeof(m_iIndex));
//	for (_uint i = 0; i < 3; i++)
//	{
//		fin.read(reinterpret_cast<char*>(&m_vPoints[i]), sizeof(m_vPoints[i]));
//		fin.read(reinterpret_cast<char*>(&m_iNeighbors[i]), sizeof(m_iNeighbors[i]));
//		fin.read(reinterpret_cast<char*>(&m_vNormals[i]), sizeof(m_vNormals[i]));
//	}
//	Make_Line();
//	m_pVIBuffer.reset();
//	m_pVIBuffer = CVIBuffer_Cell::Create(m_pDevice, m_pContext, m_vPoints);
//	if (nullptr == m_pVIBuffer)
//		RETURN_EFAIL;
//
//	return S_OK;
//}
//
//CCell* CCell::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const _float3* pPoints, _uint iIndex)
//{
//	CCell* pInstance = new CCell(pDevice, pContext);
//
//	/* 원형객체를 초기화한다.  */
//	if (FAILED(pInstance->Initialize(pPoints, iIndex)))
//	{
//		MSG_BOX("Failed to Created : CCell");
//		Safe_Release(pInstance);
//	}
//	return pInstance;
//}
//
//
//void CCell::Free()
//{
//#ifdef _DEBUG	
//#endif	
//
//	 
//	 
//}
