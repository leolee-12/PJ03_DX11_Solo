#include "..\Public\Mesh.h"
#include "Bone.h"

CMesh::CMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer{ pDevice, pContext }
{

}

HRESULT CMesh::Initialize_Prototype(_bool isAnim, const MeshData& meshData, const vector<class CBone*>& m_Bones, _fmatrix PreTransformMatrix)
{
	//이름 저장
	strcpy_s(m_szName, meshData.name.c_str());

	m_iMaterialIndex = meshData.materialIndex;
	m_iNumVertexBuffers = 1;
	m_iNumVertices = meshData.vertexCount;

	m_iVertexStride = isAnim ? sizeof(VTXANIMMESH) : sizeof(VTXMESH);

	m_iNumIndices = meshData.indexCount;
	m_iIndexStride = 4;
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_ePrimitive_Topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	// 정점 버퍼 설정
	m_BufferDesc.ByteWidth = m_iNumVertices * m_iVertexStride;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iVertexStride;

	// 애니메이션 메쉬 데이터 생성
	void* pVertices = isAnim ? Create_AnimVertices(meshData.animVertices, meshData.bones, m_Bones) : Create_NonAnimVertices(meshData.vertices, PreTransformMatrix);

	if (nullptr == pVertices)
		return E_FAIL;

	ZeroMemory(&m_InitialData, sizeof m_InitialData);
	m_InitialData.pSysMem = pVertices;

	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

	// 인덱스 버퍼 설정
	m_BufferDesc.ByteWidth = m_iNumIndices * m_iIndexStride;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = 0;

	ZeroMemory(&m_InitialData, sizeof m_InitialData);
	m_InitialData.pSysMem = meshData.indices.data();

	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;

	return S_OK;
}

void CMesh::SetUp_Matrices(_float4x4* pMatrices, const vector<class CBone*>& Bones)
{
	for (size_t i = 0; i < m_iNumBones; i++)
	{
		XMStoreFloat4x4(&pMatrices[i], XMLoadFloat4x4(&m_OffsetMatrices[i]) * XMLoadFloat4x4(Bones[m_BoneIndices[i]]->Get_CombinedTransformationMatrix()));
	}
}

void* CMesh::Create_NonAnimVertices(const vector<VTXMESH>& Vertices, _fmatrix PreTransformMatrix)
{
	VTXMESH* pVertices = new VTXMESH[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXMESH) * m_iNumVertices);

	for (size_t i = 0; i < m_iNumVertices; i++)
	{
		pVertices[i] = Vertices[i];
		XMStoreFloat3(&pVertices[i].vPosition, XMVector3TransformCoord(XMLoadFloat3(&pVertices[i].vPosition), PreTransformMatrix));
		XMStoreFloat3(&pVertices[i].vNormal, XMVector3TransformNormal(XMLoadFloat3(&pVertices[i].vNormal), PreTransformMatrix));
	}

	return pVertices;
}


void* CMesh::Create_AnimVertices(const vector<VTXANIMMESH>& animVertices, const vector<BoneData>& bones, const vector<class CBone*>& m_Bones)
{
	VTXANIMMESH* pVertices = new VTXANIMMESH[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXANIMMESH) * m_iNumVertices);

	for (size_t i = 0; i < m_iNumVertices; i++)
	{
		pVertices[i] = animVertices[i];
	}

	m_iNumBones = bones.size();
	//m_BoneIndices.resize(m_iNumBones);
	m_OffsetMatrices.resize(m_iNumBones);

	for (size_t i = 0; i < m_iNumBones; i++)
	{
		string MeshboneName = bones[i].name;
		_uint iBoneIndex = {};

		auto iter = find_if(m_Bones.begin(), m_Bones.end(), [&](CBone* pBone) {
			if (strcmp(MeshboneName.c_str(), pBone->Get_Name()) == 0) // string과 char* 비교
				return true;
			++iBoneIndex;
			return false;
			});

		m_BoneIndices.emplace_back(iBoneIndex);

		m_OffsetMatrices[i] = bones[i].transformationMatrix;
	}



	if (0 == m_iNumBones)
	{
		/* 이 메시가 제대로된 위치에 배치되서 그려질 수 있도록. */
		/* 이 메시의 위치등의 상태를 표현하고 있ㄴ느 뼈를 찾을거야. */
		/* 메시와 이름이같은 뼈를 찾아주면 된다,. */
		m_iNumBones = 1;

		_uint		iBoneIndex = {};

		auto	iter = find_if(m_Bones.begin(), m_Bones.end(), [&](CBone* pBone) {
			if (false == strcmp(m_szName, pBone->Get_Name()))
				return true;
			++iBoneIndex;
			return false;
			});

		m_BoneIndices.emplace_back(iBoneIndex);

		_float4x4		OffsetMatrix{};

		XMStoreFloat4x4(&OffsetMatrix, XMMatrixIdentity());

		m_OffsetMatrices.emplace_back(OffsetMatrix);
		// 더미 뼈 설정
		m_Bones[iBoneIndex]->SetDummyBone(true);
	}

	return pVertices;
}

CMesh* CMesh::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _bool isAnim, const MeshData& meshData, const vector<class CBone*>& m_Bones, _fmatrix PreTransformMatrix)
{
	CMesh* pInstance = new CMesh(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(isAnim, meshData, m_Bones, PreTransformMatrix)))
	{
		MSG_BOX(TEXT("Failed to Created : CMesh"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMesh::Free()
{
	__super::Free();

}
