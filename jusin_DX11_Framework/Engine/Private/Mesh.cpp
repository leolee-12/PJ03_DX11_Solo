#include "Mesh.h"
#include "Bone.h"
#include "Shader.h"

CMesh::CMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODEL eType, class CModel* pModel, const aiMesh* pAIMesh, _cmatrix PreTransformMatrix)
	: CVIBuffer{ pDevice, pContext }
	, m_eType { eType }
	, m_pAIMesh{ pAIMesh }
	, m_pModel{ pModel }
{
	XMStoreFloat4x4(&m_PreTransformMatrix, PreTransformMatrix);

	for (_uint i = 0; i < g_iNumMeshBones; ++i)
		XMStoreFloat4x4(&m_BoneMatrices[i], XMMatrixIdentity());
}

CMesh::CMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODEL eType, const WMODEL_MESH& tMeshData)
	: CVIBuffer{ pDevice, pContext }
	, m_eType{ eType }
	, m_tMesh{ tMeshData }
{
	for (_uint i = 0; i < g_iNumMeshBones; ++i)
		XMStoreFloat4x4(&m_BoneMatrices[i], XMMatrixIdentity());
}

CMesh::CMesh(const CMesh& Prototype)
	: CVIBuffer{ Prototype }
{
}

HRESULT CMesh::Initialize_Prototype()
{
	// 공통 설정
	m_iNumVertexBuffers = 1;
	m_iIndexStride = 4;
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_ePrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	// 파일 데이터로 초기화
	strcpy_s(m_szName, m_tMesh.szName);
	m_iMaterialIndex = m_tMesh.iMaterialIndex;
	m_iNumIndices = static_cast<_uint>(m_tMesh.indices.size());
	m_iNumBones = static_cast<_uint>(m_tMesh.boneIndices.size());
	m_BoneIndices = m_tMesh.boneIndices;
	m_OffsetMatrices = m_tMesh.offsetMatrices;

	m_iVertexStride = (m_eType == MODEL::NONANIM ? sizeof(VTXMESH) : sizeof(VTXANIMMESH));
	
	const void* pVertices = (m_eType == MODEL::NONANIM
		? static_cast<const void*>(m_tMesh.nonAnimVertices.data())
		: static_cast<const void*>(m_tMesh.animVertices.data()));

	m_iNumVertices = static_cast<_uint>(m_eType == MODEL::NONANIM
		? m_tMesh.nonAnimVertices.size()
		: m_tMesh.animVertices.size());

	// 마우스 피킹용 데이터 캐싱
	if (m_eType == MODEL::NONANIM)
	{
		m_vecPositions.resize(m_tMesh.nonAnimVertices.size());

		for (_uint i = 0; i < static_cast<_uint>(m_tMesh.nonAnimVertices.size()); ++i)
			m_vecPositions[i] = m_tMesh.nonAnimVertices[i].vPosition;
	}
	else
	{
		m_vecPositions.resize(m_tMesh.animVertices.size());

		for (_uint i = 0; i < static_cast<_uint>(m_tMesh.animVertices.size()); ++i)
			m_vecPositions[i] = m_tMesh.animVertices[i].vPosition;
	}

	m_vecIndices = m_tMesh.indices;

	if (!m_vecPositions.empty())
	{
		BoundingBox::CreateFromPoints(
			m_tLocalAABB,
			static_cast<size_t>(m_vecPositions.size()),
			reinterpret_cast<const XMFLOAT3*>(m_vecPositions.data()),
			sizeof(XMFLOAT3));
	}

	// VB 생성
	D3D11_BUFFER_DESC VertexBufferDesc{};
	VertexBufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	VertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	VertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	VertexBufferDesc.CPUAccessFlags = 0;
	VertexBufferDesc.MiscFlags = 0;
	VertexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA VertexInitialData{};
	VertexInitialData.pSysMem = pVertices;

	HRESULT hr = m_pDevice->CreateBuffer(&VertexBufferDesc, &VertexInitialData, &m_pVB);

	if (FAILED(hr))
		return E_FAIL;

	// IB 생성
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

	for (size_t i = 0; i < m_iNumIndices; i++)
	{
		pIndices[i] = m_tMesh.indices[i];
	}

	D3D11_SUBRESOURCE_DATA IndexInitialData{};
	IndexInitialData.pSysMem = pIndices;

	hr = m_pDevice->CreateBuffer(&IndexBufferDesc, &IndexInitialData, &m_pIB);
	Safe_Delete_Array(pIndices);

	if (FAILED(hr))
		return E_FAIL;

	return S_OK;
}

HRESULT CMesh::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CMesh::Bind_BoneMatrices(CShader* pShader, const _char* pConstName, vector<class CBone*>& Bones)
{
	for (size_t i = 0; i < m_iNumBones; i++)
	{
		XMStoreFloat4x4(&m_BoneMatrices[i],
			XMLoadFloat4x4(&m_OffsetMatrices[i]) *
			XMLoadFloat4x4(Bones[m_BoneIndices[i]]->Get_CombinedTransformationMatrixPtr()));
	}

	return pShader->Bind_Matrices(pConstName, m_BoneMatrices, m_iNumBones);
}

CMesh* XM_CALLCONV CMesh::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODEL eType, CModel* pModel, const aiMesh* pAIMesh, _cmatrix PreTransformMatrix)
{
	CMesh* pInstance = new CMesh(pDevice, pContext, eType, pModel, pAIMesh, PreTransformMatrix);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CMesh");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CMesh* CMesh::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODEL eType, const WMODEL_MESH& tMeshData)
{
	CMesh* pInstance = new CMesh(pDevice, pContext, eType, tMeshData);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CMesh");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CMesh::Clone(void* pArg)
{
	return nullptr;
}

void CMesh::Free()
{
	__super::Free();

	m_pModel = nullptr;
}
