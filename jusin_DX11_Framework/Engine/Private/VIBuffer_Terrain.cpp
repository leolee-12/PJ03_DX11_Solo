#include "VIBuffer_Terrain.h"
#include "GameInstance.h"
#include "QuadTree.h"

CVIBuffer_Terrain::CVIBuffer_Terrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pHeightMapFilePath)
	: CVIBuffer{ pDevice, pContext }
	, m_strHeightMapFilePath{ pHeightMapFilePath }
{
}

CVIBuffer_Terrain::CVIBuffer_Terrain(const CVIBuffer_Terrain& Prototype)
	: CVIBuffer{ Prototype }
	, m_iNumVerticesX{ Prototype.m_iNumVerticesX }
	, m_iNumVerticesZ{ Prototype.m_iNumVerticesZ }
	, m_pVtxPos{ Prototype.m_pVtxPos }
	, m_pQuadTree{ Prototype.m_pQuadTree }
{
	Safe_AddRef(m_pQuadTree);
}

HRESULT CVIBuffer_Terrain::Initialize_Prototype()
{
	// 1. 높이맵 파일 로드
	_ulong dwByte = {};
	HANDLE hFile = CreateFile(m_strHeightMapFilePath.c_str(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
	
	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	BITMAPFILEHEADER fh{};
	BITMAPINFOHEADER ih{};
	_uint* pPixels = { nullptr };

	ReadFile(hFile, &fh, sizeof fh, &dwByte, nullptr);
	ReadFile(hFile, &ih, sizeof ih, &dwByte, nullptr);
	m_iNumVerticesX = ih.biWidth;
	m_iNumVerticesZ = ih.biHeight;
	m_iNumVertices = m_iNumVerticesX * m_iNumVerticesZ;

	pPixels = new _uint[m_iNumVertices];
	ZeroMemory(pPixels, sizeof(_uint) * m_iNumVertices);
	ReadFile(hFile, pPixels, sizeof(_uint) * m_iNumVertices, &dwByte, nullptr);
	CloseHandle(hFile);



	// 2. VB 정보 세팅
	m_iNumVertexBuffers = 1;
	m_iVertexStride = sizeof(VTXNORTEX);
	m_iNumIndices = (m_iNumVerticesX - 1) * (m_iNumVerticesZ - 1) * 2 * 3;
	m_iIndexStride = 4;
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_ePrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	D3D11_BUFFER_DESC VertexBufferDesc{};
	VertexBufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	VertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	VertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	VertexBufferDesc.CPUAccessFlags = 0;
	VertexBufferDesc.MiscFlags = 0;
	VertexBufferDesc.StructureByteStride = 0;

	VTXNORTEX* pVertices = new VTXNORTEX[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXNORTEX) * m_iNumVertices);

	m_pVtxPos = new _float3[m_iNumVertices];
	ZeroMemory(m_pVtxPos, sizeof(_float3) * m_iNumVertices);

	for (size_t i = 0; i < m_iNumVerticesZ; i++)
	{
		for (size_t j = 0; j < m_iNumVerticesX; j++)
		{
			size_t iIndex = i * m_iNumVerticesX + j;

			m_pVtxPos[iIndex] = pVertices[iIndex].vPosition = _float3(_float(j), _float(pPixels[iIndex] & 0x000000ff) * 0.1f, _float(i));
			pVertices[iIndex].vNormal = _float3(0.f, 0.f, 0.f);
			pVertices[iIndex].vTexcoord = _float2(_float(j) / (m_iNumVerticesX - 1.f), _float(i) / (m_iNumVerticesZ - 1.f));
		}
	}



	// 3. IB 정보 세팅 및 버텍스 법선 세팅
	D3D11_BUFFER_DESC IndexBufferDesc{};
	IndexBufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
	IndexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	IndexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	IndexBufferDesc.CPUAccessFlags = 0;
	IndexBufferDesc.MiscFlags = 0;
	IndexBufferDesc.StructureByteStride = 0;

	_uint* pIndices = new _uint[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_uint) * m_iNumIndices);

	_uint iNumIndices = {};
	_vector vSour, vDest, vTriNorm;

	for (size_t i = 0; i < m_iNumVerticesZ - 1; i++)
	{
		for (size_t j = 0; j < m_iNumVerticesX - 1; j++)
		{
			size_t iIndex = i * m_iNumVerticesX + j;

			_uint iIndices[4] =
			{
				_uint(iIndex + m_iNumVerticesX),
				_uint(iIndex + m_iNumVerticesX + 1),
				_uint(iIndex + 1),
				_uint(iIndex)
			};

			pIndices[iNumIndices++] = iIndices[0];
			pIndices[iNumIndices++] = iIndices[1];
			pIndices[iNumIndices++] = iIndices[2];

			vSour = XMLoadFloat3(&pVertices[iIndices[1]].vPosition) - XMLoadFloat3(&pVertices[iIndices[0]].vPosition);
			vDest = XMLoadFloat3(&pVertices[iIndices[2]].vPosition) - XMLoadFloat3(&pVertices[iIndices[1]].vPosition);
			//vTriNorm = XMVector3Normalize(XMVector3Cross(vSour, vDest));	// 정규화할 경우, 삼각형 면적 별 가중치가 반영되지 않음 : 균등 평균 방식
			vTriNorm = XMVector3Cross(vSour, vDest);	// 정규화하지 않으면 삼각형 면적에 비례하는 외적 벡터의 크기가 그대로 반영 : 면적 가중 방식

			XMStoreFloat3(&pVertices[iIndices[0]].vNormal, XMLoadFloat3(&pVertices[iIndices[0]].vNormal) + vTriNorm);
			XMStoreFloat3(&pVertices[iIndices[1]].vNormal, XMLoadFloat3(&pVertices[iIndices[1]].vNormal) + vTriNorm);
			XMStoreFloat3(&pVertices[iIndices[2]].vNormal, XMLoadFloat3(&pVertices[iIndices[2]].vNormal) + vTriNorm);

			pIndices[iNumIndices++] = iIndices[0];
			pIndices[iNumIndices++] = iIndices[2];
			pIndices[iNumIndices++] = iIndices[3];

			vSour = XMLoadFloat3(&pVertices[iIndices[2]].vPosition) - XMLoadFloat3(&pVertices[iIndices[0]].vPosition);
			vDest = XMLoadFloat3(&pVertices[iIndices[3]].vPosition) - XMLoadFloat3(&pVertices[iIndices[2]].vPosition);
			//vTriNorm = XMVector3Normalize(XMVector3Cross(vSour, vDest));
			vTriNorm = XMVector3Cross(vSour, vDest);

			XMStoreFloat3(&pVertices[iIndices[0]].vNormal, XMLoadFloat3(&pVertices[iIndices[0]].vNormal) + vTriNorm);
			XMStoreFloat3(&pVertices[iIndices[2]].vNormal, XMLoadFloat3(&pVertices[iIndices[2]].vNormal) + vTriNorm);
			XMStoreFloat3(&pVertices[iIndices[3]].vNormal, XMLoadFloat3(&pVertices[iIndices[3]].vNormal) + vTriNorm);
		}
	}



	// 4. 법선벡터 정규화
	for (size_t i = 0; i < m_iNumVertices; i++)
		XMStoreFloat3(&pVertices[i].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[i].vNormal)));



	// 5. VB, IB 생성
	D3D11_SUBRESOURCE_DATA VertexInitialData{};
	VertexInitialData.pSysMem = pVertices;

	if (FAILED(m_pDevice->CreateBuffer(&VertexBufferDesc, &VertexInitialData, &m_pVB)))
	{
		Safe_Delete_Array(pVertices);
		Safe_Delete_Array(pIndices);
		Safe_Delete_Array(pPixels);
		return E_FAIL;
	}

	D3D11_SUBRESOURCE_DATA IndexInitialData{};
	IndexInitialData.pSysMem = pIndices;

	if (FAILED(m_pDevice->CreateBuffer(&IndexBufferDesc, &IndexInitialData, &m_pIB)))
	{
		Safe_Delete_Array(pVertices);
		Safe_Delete_Array(pIndices);
		Safe_Delete_Array(pPixels);
		return E_FAIL;
	}

	m_pQuadTree = CQuadTree::Create(m_iNumVerticesX * m_iNumVerticesZ - m_iNumVerticesX, m_iNumVerticesX * m_iNumVerticesZ - 1, m_iNumVerticesX - 1, 0);
	if (nullptr == m_pQuadTree)
		return E_FAIL;

	Safe_Delete_Array(pVertices);
	Safe_Delete_Array(pIndices);
	Safe_Delete_Array(pPixels);
	return S_OK;
}

HRESULT CVIBuffer_Terrain::Initialize(void* pArg)
{
	return S_OK;
}

void XM_CALLCONV CVIBuffer_Terrain::Culling(_fmatrix WorldMatrix)
{
	m_pGameInstance->Transform_Frustum_ToLocalSpace(WorldMatrix);


	D3D11_MAPPED_SUBRESOURCE        SubResource = { };

	m_pContext->Map(m_pIB, 0, D3D11_MAP_WRITE_DISCARD, 0, &SubResource);
	_uint* pIndices = static_cast<_uint*>(SubResource.pData);

	_uint       iNumIndices = {};

	m_pQuadTree->Culling(m_pGameInstance, m_pVtxPos, pIndices, &iNumIndices);

	m_pContext->Unmap(m_pIB, 0);

	m_iNumIndices = iNumIndices;
}

CVIBuffer_Terrain* CVIBuffer_Terrain::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pHeightMapFilePath)
{
	CVIBuffer_Terrain* pInstance = new CVIBuffer_Terrain(pDevice, pContext, pHeightMapFilePath);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CVIBuffer_Terrain");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CVIBuffer_Terrain::Clone(void* pArg)
{
	CVIBuffer_Terrain* pInstance = new CVIBuffer_Terrain(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CVIBuffer_Terrain");
		Safe_Release(pInstance);
	}

	return pInstance;
}
void CVIBuffer_Terrain::Free()
{
	Safe_Release(m_pQuadTree);
	Safe_Delete_Array(m_pVtxPos);

	__super::Free();
}
