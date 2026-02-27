//#include "..\Public\Mesh.h"
//#include "Bone.h"
//#include "Shader.h"
//#include "GameInstance.h"
//
//
//CMesh::CMesh(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
//	: CVIBuffer(pDevice, pContext)
//{
//}
//
//CMesh::CMesh(const CMesh& rhs)
//	: CVIBuffer(rhs)
//{
//}
//
//#ifdef _DEBUG
//HRESULT CMesh::Initialize_Prototype(MODELTYPE eModelType, const aiMesh* pAIMesh, _fmatrix PivotMatrix, const vector<class CBone*> Bones)
//{
//	m_iMaterialIndex = pAIMesh->mMaterialIndex;
//	m_eModelType = eModelType;
//
//	strcpy_s(m_szName, pAIMesh->mName.data);
//
//	m_iNumVertexBuffers = 1;
//	m_iNumVertices = pAIMesh->mNumVertices;
//	m_iStride = sizeof(VTXMESH);
//
//	m_iNumIndices = pAIMesh->mNumFaces * 3;
//	m_iIndexStride = 4;
//	m_eIndexFormat = m_iIndexStride == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
//	m_eTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
//
//#pragma region VERTEX_BUFFER
//	HRESULT		hr = MODELTYPE::NONANIM == eModelType ? Ready_Vertices_NonAnim(pAIMesh, PivotMatrix) : Ready_Vertices_Anim(pAIMesh,Bones);
//
//	if (FAILED(hr))
//		RETURN_EFAIL;
//
//#pragma endregion
//
//#pragma region INDEX_BUFFER
//	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
//	m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
//	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT /*D3D11_USAGE_DYNAMIC*/;
//	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
//	m_BufferDesc.CPUAccessFlags = 0;
//	m_BufferDesc.MiscFlags = 0;
//	m_BufferDesc.StructureByteStride = 0;
//
//	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
//
//	_uint* pIndices = new _uint[m_iNumIndices];
//
//	_uint		iNumIndices = { 0 };
//
//	for (size_t i = 0; i < pAIMesh->mNumFaces; i++)
//	{
//		pIndices[iNumIndices++] = pAIMesh->mFaces[i].mIndices[0];
//		pIndices[iNumIndices++] = pAIMesh->mFaces[i].mIndices[1];
//		pIndices[iNumIndices++] = pAIMesh->mFaces[i].mIndices[2];
//
//		m_vecIndexInfo.push_back(_uint3(pAIMesh->mFaces[i].mIndices[0], pAIMesh->mFaces[i].mIndices[1], pAIMesh->mFaces[i].mIndices[2]));
//	}
//
//	m_SubResourceData.pSysMem = pIndices;
//
//	if (FAILED(__super::Create_Buffer(&m_pIB)))
//		RETURN_EFAIL;
//
//	Safe_Delete_Array(pIndices);
//
//#pragma endregion
//
//	return S_OK;
//}
//#endif
//
//HRESULT CMesh::Initialize_Prototype(MESH_DATA MeshData)
//{
//	m_iMaterialIndex = MeshData.iMaterialIndex;
//	m_eModelType = MeshData.eModelType;
//
//	strcpy_s(m_szName, MeshData.szName.c_str());
//
//	m_iNumVertexBuffers = 1;
//	m_iNumVertices = MeshData.iNumVertices;
//	m_iStride = sizeof(VTXMESH);
//
//	m_iNumIndices = MeshData.iNumFaces * 3;
//	m_iIndexStride = 4;
//	m_eIndexFormat = m_iIndexStride == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
//	m_eTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
//
//#pragma region VERTEX_BUFFER
//	HRESULT		hr = MODELTYPE::NONANIM == m_eModelType ? Ready_Vertices_NonAnim(MeshData) : Ready_Vertices_Anim(MeshData);
//
//	if (FAILED(hr))
//		RETURN_EFAIL;
//
//#pragma endregion
//
//#pragma region INDEX_BUFFER
//	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
//	m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
//	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT /*D3D11_USAGE_DYNAMIC*/;
//	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
//	m_BufferDesc.CPUAccessFlags = 0;
//	m_BufferDesc.MiscFlags = 0;
//	m_BufferDesc.StructureByteStride = 0;
//
//	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
//
//	_uint* pIndices = new _uint[m_iNumIndices];
//
//	
//	for(_uint i = 0; i < m_iNumIndices; i++)
//		pIndices[i] = MeshData.pIndices[i];
//
//	_uint iNumIndices = 0;
//	for (size_t i = 0; i < MeshData.iNumFaces; i++)
//	{
//		m_vecIndexInfo.push_back(_uint3(pIndices[iNumIndices++], pIndices[iNumIndices++], pIndices[iNumIndices++]));
//	}
//
//	m_SubResourceData.pSysMem = pIndices;
//
//	if (FAILED(__super::Create_Buffer(&m_pIB)))
//		RETURN_EFAIL;
//
//	Safe_Delete_Array(pIndices);
//
//#pragma endregion
//
//	return S_OK;
//}
//
//HRESULT CMesh::Initialize(void* pArg)
//{
//	return S_OK;
//}
//
//#ifdef _DEBUG
//HRESULT CMesh::Ready_Vertices_NonAnim(const aiMesh* pAIMesh, _fmatrix PivotMatrix)
//{
//	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
//
//	m_BufferDesc.ByteWidth = m_iStride * m_iNumVertices;
//	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT /*D3D11_USAGE_DYNAMIC*/;
//	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
//	m_BufferDesc.CPUAccessFlags = 0;
//	m_BufferDesc.MiscFlags = 0;
//	m_BufferDesc.StructureByteStride = m_iStride;
//
//	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
//
//	VTXMESH* pVertices = new VTXMESH[m_iNumVertices];
//
//	for (size_t i = 0; i < m_iNumVertices; i++)
//	{
//		memcpy(&pVertices[i].vPosition, &pAIMesh->mVertices[i], sizeof(_float3));
//		XMStoreFloat3(&pVertices[i].vPosition, XMVector3TransformCoord(XMLoadFloat3(&pVertices[i].vPosition), PivotMatrix));
//
//		memcpy(&pVertices[i].vNormal, &pAIMesh->mNormals[i], sizeof(_float3));
//		XMStoreFloat3(&pVertices[i].vNormal, XMVector3TransformNormal(XMLoadFloat3(&pVertices[i].vNormal), PivotMatrix));
//
//		memcpy(&pVertices[i].vTexcoord, &pAIMesh->mTextureCoords[0][i], sizeof(_float3));
//		memcpy(&pVertices[i].vTangent, &pAIMesh->mTangents[i], sizeof(_float3));
//		XMStoreFloat3(&pVertices[i].vTangent, XMVector3TransformNormal(XMLoadFloat3(&pVertices[i].vTangent), PivotMatrix));
//
//		m_vecVertexInfo.push_back(pVertices[i].vPosition);
//	}
//
//
//	m_SubResourceData.pSysMem = pVertices;
//
//	/* pVertices에 할당하여 채워놨던 정점들의 정보를 ID3D11Buffer로 할당한 공간에 복사하여 채워넣는다. */
//	if (FAILED(__super::Create_Buffer(&m_pVB)))
//		RETURN_EFAIL;
//
//	Safe_Delete_Array(pVertices);
//	return S_OK;
//}
//#endif
//
//HRESULT CMesh::Ready_Vertices_NonAnim(MESH_DATA MeshData)
//{
//	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
//
//	m_BufferDesc.ByteWidth = m_iStride * m_iNumVertices;
//	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT /*D3D11_USAGE_DYNAMIC*/;
//	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
//	m_BufferDesc.CPUAccessFlags = 0;
//	m_BufferDesc.MiscFlags = 0;
//	m_BufferDesc.StructureByteStride = m_iStride;
//
//	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
//
//	VTXMESH* pVertices = new VTXMESH[m_iNumVertices];
//
//	for (size_t i = 0; i < m_iNumVertices; i++)
//	{
//		pVertices[i] = MeshData.pVertices[i];
//		m_vecVertexInfo.push_back(pVertices[i].vPosition);
//	}
//
//	m_SubResourceData.pSysMem = pVertices;
//
//	/* pVertices에 할당하여 채워놨던 정점들의 정보를 ID3D11Buffer로 할당한 공간에 복사하여 채워넣는다. */
//	if (FAILED(__super::Create_Buffer(&m_pVB)))
//		RETURN_EFAIL;
//
//	Safe_Delete_Array(pVertices);
//
//	return S_OK;
//}
//
//#ifdef _DEBUG
//HRESULT CMesh::Ready_Vertices_Anim(const aiMesh* pAIMesh, const vector<class CBone*> Bones)
//{
//	m_iStride = sizeof(VTXMESHANIM);
//
//	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
//
//	if (m_pGameInstance->Get_CreateLevelIndex() == 1)
//	{
//		//툴용 동적 버퍼
//		m_BufferDesc.ByteWidth = m_iStride * m_iNumVertices;	// 동적배열의 크기
//		m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC;					// 정적버퍼인지 동적버퍼인지 설정/ 지금은 정적
//		m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;			// 버퍼 종류 설정
//		m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;		// 동적만 해당
//		m_BufferDesc.MiscFlags = 0;								// 동적만 해당
//		m_BufferDesc.StructureByteStride = m_iStride;		// 정점 하나의 크기를 집어 넣음
//	}
//	else
//	{
//		m_BufferDesc.ByteWidth = m_iStride * m_iNumVertices;
//		m_BufferDesc.Usage = D3D11_USAGE_DEFAULT /*D3D11_USAGE_DYNAMIC*/;
//		m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
//		m_BufferDesc.CPUAccessFlags = 0;
//		m_BufferDesc.MiscFlags = 0;
//		m_BufferDesc.StructureByteStride = m_iStride;
//	}
//
//
//
//	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
//
//	VTXMESHANIM* pVertices = new VTXMESHANIM[m_iNumVertices];
//	ZeroMemory(pVertices, sizeof(VTXMESHANIM) * m_iNumVertices);
//
//	for (size_t i = 0; i < m_iNumVertices; i++)
//	{
//		memcpy(&pVertices[i].vPosition, &pAIMesh->mVertices[i], sizeof(_float3));
//		memcpy(&pVertices[i].vNormal, &pAIMesh->mNormals[i], sizeof(_float3));
//		memcpy(&pVertices[i].vTexcoord, &pAIMesh->mTextureCoords[0][i], sizeof(_float3));
//		memcpy(&pVertices[i].vTangent, &pAIMesh->mTangents[i], sizeof(_float3));
//
//		m_vecVertexInfo.push_back(pVertices[i].vPosition);
//	}
//
//	m_iNumBones = pAIMesh->mNumBones;
//
//
//	/* 이 메시에게 영향을 주는 뼈을 순회하면서 각각의 뼈가 어떤 정점들에게 영향을 주는지 파악한다.*/
//	for (_uint i = 0; i < pAIMesh->mNumBones; i++)
//	{
//		aiBone* pAIBone = pAIMesh->mBones[i];
//
//		_float4x4 OffsetMatrix;
//		memcpy(&OffsetMatrix, &pAIBone->mOffsetMatrix, sizeof(_float4x4)); XMStoreFloat4x4(&OffsetMatrix, XMMatrixTranspose(XMLoadFloat4x4(&OffsetMatrix)));
//		m_OffsetMatrices.push_back(OffsetMatrix);
//
//		_uint iBoneIndex = { 0 };
//
//		auto iter = find_if(Bones.begin(), Bones.end(), [&](CBone* pBone) {
//			if (!strcmp(pAIBone->mName.data, pBone->Get_Name()))
//				return true;
//
//			++iBoneIndex;
//			return false;
//		});
//
//		if (iter == Bones.end())
//			RETURN_EFAIL;
//
//		m_BoneIndices.push_back(iBoneIndex);
//
//		/* 이 뼈는 몇개의 정점에게 영향을 주는가?! */
//		for (_uint j = 0; j < pAIBone->mNumWeights; j++)
//		{
//			/* pAIBone->mWeights[j].mVertexId : 이 뼈가 영향을 주는 j번째 정점의 인덱스 */
//			if (0.0f == pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.x)
//			{
//				pVertices[pAIBone->mWeights[j].mVertexId].vBlendIndices.x = i;
//				pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.x = pAIBone->mWeights[j].mWeight;
//			}
//
//			else if (0.0f == pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.y)
//			{
//				pVertices[pAIBone->mWeights[j].mVertexId].vBlendIndices.y = i;
//				pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.y = pAIBone->mWeights[j].mWeight;
//			}
//
//			else if (0.0f == pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.z)
//			{
//				pVertices[pAIBone->mWeights[j].mVertexId].vBlendIndices.z = i;
//				pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.z = pAIBone->mWeights[j].mWeight;
//			}
//
//			else if (0.0f == pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.w)
//			{
//				pVertices[pAIBone->mWeights[j].mVertexId].vBlendIndices.w = i;
//				pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.w = pAIBone->mWeights[j].mWeight;
//			}
//		}
//	};
//
//	m_SubResourceData.pSysMem = pVertices;
//
//	/* pVertices에 할당하여 채워놨던 정점들의 정보를 ID3D11Buffer로 할당한 공간에 복사하여 채워넣는다. */
//	if (FAILED(__super::Create_Buffer(&m_pVB)))
//		RETURN_EFAIL;
//
//	Safe_Delete_Array(pVertices);
//
//
//	if (0 == m_iNumBones)
//	{
//		m_iNumBones = 1;
//
//		_uint		iBoneIndex = { 0 };
//
//		auto	iter = find_if(Bones.begin(), Bones.end(), [&](CBone* pBone)
//			{
//				if (false == strcmp(m_szName, pBone->Get_Name()))
//				{
//					return true;
//				}
//
//				++iBoneIndex;
//
//				return false;
//			});
//
//		if (iter == Bones.end())
//			RETURN_EFAIL;
//
//		m_BoneIndices.push_back(iBoneIndex);
//
//		_float4x4		OffsetMatrix;
//		XMStoreFloat4x4(&OffsetMatrix, XMMatrixIdentity());
//		m_OffsetMatrices.push_back(OffsetMatrix);
//	}
//
//	return S_OK;
//}
//#endif
//
//HRESULT CMesh::Ready_Vertices_Anim(MESH_DATA MeshData)
//{
//	m_iStride = sizeof(VTXMESHANIM);
//
//	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
//
//	if (m_pGameInstance->Get_CreateLevelIndex() == 1)
//	{
//		//툴용 동적 버퍼
//		m_BufferDesc.ByteWidth = m_iStride * m_iNumVertices;	// 동적배열의 크기
//		m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC;					// 정적버퍼인지 동적버퍼인지 설정/ 지금은 정적
//		m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;			// 버퍼 종류 설정
//		m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;		// 동적만 해당
//		m_BufferDesc.MiscFlags = 0;								// 동적만 해당
//		m_BufferDesc.StructureByteStride = m_iStride;		// 정점 하나의 크기를 집어 넣음
//	}
//	else
//	{
//		m_BufferDesc.ByteWidth = m_iStride * m_iNumVertices;
//		m_BufferDesc.Usage = D3D11_USAGE_DEFAULT /*D3D11_USAGE_DYNAMIC*/;
//		m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
//		m_BufferDesc.CPUAccessFlags = 0;
//		m_BufferDesc.MiscFlags = 0;
//		m_BufferDesc.StructureByteStride = m_iStride;
//	}
//
//	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
//
//	VTXMESHANIM* pVertices = new VTXMESHANIM[m_iNumVertices];
//	ZeroMemory(pVertices, sizeof(VTXMESHANIM) * m_iNumVertices);
//	
//	for (size_t i = 0; i < m_iNumVertices; i++)
//	{
//		pVertices[i] = MeshData.pAnimVertices[i];
//		m_vecVertexInfo.push_back(pVertices[i].vPosition);
//	}
//
//	m_iNumBones = MeshData.iNumBones;
//
//	/* 이 메시에게 영향을 주는 뼈을 순회하면서 각각의 뼈가 어떤 정점들에게 영향을 주는지 파악한다.*/
//	for (_uint i = 0; i < m_iNumBones; i++)
//	{		
//		m_OffsetMatrices.push_back(MeshData.OffsetMatrices[i]);
//		m_BoneIndices.push_back(MeshData.BoneIndices[i]);
//	};
//
//	m_SubResourceData.pSysMem = pVertices;
//
//	/* pVertices에 할당하여 채워놨던 정점들의 정보를 ID3D11Buffer로 할당한 공간에 복사하여 채워넣는다. */
//	if (FAILED(__super::Create_Buffer(&m_pVB)))
//		RETURN_EFAIL;
//
//	Safe_Delete_Array(pVertices);
//
//	return S_OK;
//}
//
//
//HRESULT CMesh::Bind_BoneMatrices(CShader* pShader, const _char* pConstantName, const vector<class CBone*> Bones)
//{
//	_float4x4 BoneMatrices[256];
//
//	for (size_t i = 0; i < m_iNumBones; i++)
//	{
//		XMStoreFloat4x4(&BoneMatrices[i], XMLoadFloat4x4(&m_OffsetMatrices[i]) * Bones[m_BoneIndices[i]]->Get_CombinedTransformationMatrix());
//	}
//	return pShader->Bind_Matrices(pConstantName, BoneMatrices, 256);
//}
//
//#ifdef _DEBUG
//CMesh* CMesh::Create(MODELTYPE eModelType, ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const aiMesh* pAIMesh, _fmatrix PivotMatrix ,const vector<class CBone*> Bones)
//{
//	CMesh* pInstance = new CMesh(pDevice, pContext);
//
//	if (FAILED(pInstance->Initialize_Prototype(eModelType,pAIMesh, PivotMatrix,Bones)))
//	{
//		MSG_BOX("Failed to Created : CMesh");
//		Safe_Release(pInstance);
//	}
//	return pInstance;
//}
//#endif
//
//CMesh* CMesh::Create(MESH_DATA MeshData, ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
//{
//	CMesh* pInstance = new CMesh(pDevice, pContext);
//
//	if (FAILED(pInstance->Initialize_Prototype(MeshData)))
//	{
//		MSG_BOX("Failed to Created : CMesh");
//		Safe_Release(pInstance);
//	}
//	return pInstance;
//}
//
//CComponent* CMesh::Clone(void* pArg)
//{
//	return nullptr;
//}
//
//void CMesh::Free()
//{
//	__super::Free();
//}
