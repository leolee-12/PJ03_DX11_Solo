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

CMesh::CMesh(const CMesh& Prototype)
	: CVIBuffer{ Prototype }
{
}

HRESULT CMesh::Initialize_Prototype()
{
	strcpy_s(m_szName, m_pAIMesh->mName.data);

	m_iMaterialIndex = m_pAIMesh->mMaterialIndex;
	m_iNumVertexBuffers = 1;
	m_iNumVertices = m_pAIMesh->mNumVertices;
	_uint iNumFaces = m_pAIMesh->mNumFaces;
	m_iNumIndices = iNumFaces * 3;
	m_iIndexStride = 4;
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_ePrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	// VB 생성
	if (FAILED(MODEL::NONANIM == m_eType ? Ready_NonAnimMesh() : Ready_AnimMesh(m_pModel)))
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

	for (size_t i = 0; i < iNumFaces; i++)
	{
		pIndices[iNumIndices++] = m_pAIMesh->mFaces[i].mIndices[0];
		pIndices[iNumIndices++] = m_pAIMesh->mFaces[i].mIndices[1];
		pIndices[iNumIndices++] = m_pAIMesh->mFaces[i].mIndices[2];
	}

	D3D11_SUBRESOURCE_DATA IndexInitialData{};
	IndexInitialData.pSysMem = pIndices;

	HRESULT hr = m_pDevice->CreateBuffer(&IndexBufferDesc, &IndexInitialData, &m_pIB);
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

HRESULT CMesh::Ready_NonAnimMesh()
{
	m_iVertexStride = sizeof(VTXMESH);

	D3D11_BUFFER_DESC VertexBufferDesc{};
	VertexBufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	VertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	VertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	VertexBufferDesc.CPUAccessFlags = 0;
	VertexBufferDesc.MiscFlags = 0;
	VertexBufferDesc.StructureByteStride = 0;

	VTXMESH* pVertices = new VTXMESH[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXMESH) * m_iNumVertices);

	for (size_t i = 0; i < m_iNumVertices; i++)
	{
		memcpy(&pVertices[i].vPosition, &m_pAIMesh->mVertices[i], sizeof(_float3));
		XMStoreFloat3(&pVertices[i].vPosition,
			XMVector3TransformCoord(XMLoadFloat3(&pVertices[i].vPosition), XMLoadFloat4x4(&m_PreTransformMatrix)));
		
		memcpy(&pVertices[i].vNormal, &m_pAIMesh->mNormals[i], sizeof(_float3));
		XMStoreFloat3(&pVertices[i].vNormal,
			XMVector3TransformNormal(XMLoadFloat3(&pVertices[i].vNormal), XMLoadFloat4x4(&m_PreTransformMatrix)));
		
		memcpy(&pVertices[i].vTexcoord, &m_pAIMesh->mTextureCoords[0][i], sizeof(_float2));
		
		memcpy(&pVertices[i].vTangent, &m_pAIMesh->mTangents[i], sizeof(_float3));
		XMStoreFloat3(&pVertices[i].vTangent,
			XMVector3TransformNormal(XMLoadFloat3(&pVertices[i].vTangent), XMLoadFloat4x4(&m_PreTransformMatrix)));
		
		memcpy(&pVertices[i].vBinormal, &m_pAIMesh->mBitangents[i], sizeof(_float3));
		XMStoreFloat3(&pVertices[i].vBinormal,
			XMVector3TransformNormal(XMLoadFloat3(&pVertices[i].vBinormal), XMLoadFloat4x4(&m_PreTransformMatrix)));
	}

	D3D11_SUBRESOURCE_DATA      VertexInitialData{};
	VertexInitialData.pSysMem = pVertices;

	HRESULT hr = m_pDevice->CreateBuffer(&VertexBufferDesc, &VertexInitialData, &m_pVB);
	Safe_Delete_Array(pVertices);

	if (FAILED(hr))
		return E_FAIL;

	return S_OK;
}

HRESULT CMesh::Ready_AnimMesh(CModel* pModel)
{
	m_iVertexStride = sizeof(VTXANIMMESH);

	D3D11_BUFFER_DESC VertexBufferDesc{};
	VertexBufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	VertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	VertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	VertexBufferDesc.CPUAccessFlags = 0;
	VertexBufferDesc.MiscFlags = 0;
	VertexBufferDesc.StructureByteStride = 0;

	VTXANIMMESH* pVertices = new VTXANIMMESH[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXANIMMESH) * m_iNumVertices);

	for (size_t i = 0; i < m_iNumVertices; i++)
	{
		memcpy(&pVertices[i].vPosition, &m_pAIMesh->mVertices[i], sizeof(_float3));
		memcpy(&pVertices[i].vNormal, &m_pAIMesh->mNormals[i], sizeof(_float3));
		memcpy(&pVertices[i].vTexcoord, &m_pAIMesh->mTextureCoords[0][i], sizeof(_float2));
		memcpy(&pVertices[i].vTangent, &m_pAIMesh->mTangents[i], sizeof(_float3));
		memcpy(&pVertices[i].vBinormal, &m_pAIMesh->mBitangents[i], sizeof(_float3));
	}

	m_iNumBones = m_pAIMesh->mNumBones;

	/* 이 메시에게 영향을 주는 뼈의 갯수 */
	for (_uint i = 0; i < m_iNumBones; i++)
	{
		aiBone* pAIBone = m_pAIMesh->mBones[i];

		_int iBoneIndex = pModel->Get_BoneIndex(pAIBone->mName.data);
		if (-1 == iBoneIndex)
			return E_FAIL;

		m_BoneIndices.push_back(iBoneIndex);

		_float4x4 OffsetMatrix = {};
		memcpy(&OffsetMatrix, &pAIBone->mOffsetMatrix, sizeof OffsetMatrix);
		XMStoreFloat4x4(&OffsetMatrix, XMMatrixTranspose(XMLoadFloat4x4(&OffsetMatrix)));
		m_OffsetMatrices.push_back(OffsetMatrix);

		/* 이 뼈가 영향을 주는 정점의 갯수  */
		for (_uint j = 0; j < pAIBone->mNumWeights; j++)
		{
			aiVertexWeight AIWeight = pAIBone->mWeights[j];

			if (0.f == pVertices[AIWeight.mVertexId].vBlendWeight.x)
			{
				pVertices[AIWeight.mVertexId].vBlendIndex.x = i;
				pVertices[AIWeight.mVertexId].vBlendWeight.x = AIWeight.mWeight;
			}
			else if (0.f == pVertices[AIWeight.mVertexId].vBlendWeight.y)
			{
				pVertices[AIWeight.mVertexId].vBlendIndex.y = i;
				pVertices[AIWeight.mVertexId].vBlendWeight.y = AIWeight.mWeight;
			}
			else if (0.f == pVertices[AIWeight.mVertexId].vBlendWeight.z)
			{
				pVertices[AIWeight.mVertexId].vBlendIndex.z = i;
				pVertices[AIWeight.mVertexId].vBlendWeight.z = AIWeight.mWeight;
			}
			else
			{
				pVertices[AIWeight.mVertexId].vBlendIndex.w = i;
				pVertices[AIWeight.mVertexId].vBlendWeight.w = AIWeight.mWeight;
			}
		}
	}

	if (0 == m_iNumBones)
	{
		m_iNumBones = 1;

		_uint iBoneIndex = pModel->Get_BoneIndex(m_szName);
		if (-1 == iBoneIndex)
			return E_FAIL;

		_float4x4      OffsetMatrix = {};
		XMStoreFloat4x4(&OffsetMatrix, XMMatrixIdentity());

		m_OffsetMatrices.push_back(OffsetMatrix);

		m_BoneIndices.push_back(iBoneIndex);
	}

	D3D11_SUBRESOURCE_DATA VertexInitialData{};
	VertexInitialData.pSysMem = pVertices;

	HRESULT hr = m_pDevice->CreateBuffer(&VertexBufferDesc, &VertexInitialData, &m_pVB);
	Safe_Delete_Array(pVertices);

	if (FAILED(hr))
		return E_FAIL;

	return S_OK;
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

CComponent* CMesh::Clone(void* pArg)
{
	return nullptr;
}

void CMesh::Free()
{
	__super::Free();

	m_pModel = nullptr;
}
