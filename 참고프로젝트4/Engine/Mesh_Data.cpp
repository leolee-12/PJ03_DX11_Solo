#include "Mesh_Data.h"

#ifdef _DEBUG
HRESULT Mesh_Data::Bake_MeshData(const aiMesh* pAIMesh, MODELTYPE eType, _fmatrix PivotMatrix, const vector<BONE_DATA> Bones)
{
	iMaterialIndex = pAIMesh->mMaterialIndex;
	eModelType = eType;
	szName = pAIMesh->mName.data;
	iNumVertices = pAIMesh->mNumVertices;
	iNumFaces = pAIMesh->mNumFaces;

#pragma region VERTEX_BUFFER
	HRESULT		hr = MODELTYPE::NONANIM == eModelType ? Ready_Vertices_NonAnim(pAIMesh, PivotMatrix) : Ready_Vertices_Anim(pAIMesh, Bones);

	if (FAILED(hr))
		RETURN_EFAIL;

#pragma endregion

#pragma region INDEX_BUFFER

	for (size_t i = 0; i < pAIMesh->mNumFaces; i++)
	{
		pIndices.push_back(pAIMesh->mFaces[i].mIndices[0]);
		pIndices.push_back(pAIMesh->mFaces[i].mIndices[1]);
		pIndices.push_back(pAIMesh->mFaces[i].mIndices[2]);
	}
#pragma endregion
	iNumOffsetMatrices = Cast<_uint>(OffsetMatrices.size());

	return S_OK;
}

HRESULT Mesh_Data::Ready_Vertices_NonAnim(const aiMesh* pAIMesh, _fmatrix PivotMatrix)
{
	for (size_t i = 0; i < iNumVertices; i++)
	{
		VTXMESH Vertex = {};
		memcpy(&Vertex.vPosition, &pAIMesh->mVertices[i], sizeof(_float3));
		XMStoreFloat3(&Vertex.vPosition, XMVector3TransformCoord(XMLoadFloat3(&Vertex.vPosition), PivotMatrix));

		memcpy(&Vertex.vNormal, &pAIMesh->mNormals[i], sizeof(_float3));
		XMStoreFloat3(&Vertex.vNormal, XMVector3TransformNormal(XMLoadFloat3(&Vertex.vNormal), PivotMatrix));

		memcpy(&Vertex.vTexcoord, &pAIMesh->mTextureCoords[0][i], sizeof(_float3));
		memcpy(&Vertex.vTangent, &pAIMesh->mTangents[i], sizeof(_float3));

		pVertices.push_back(Vertex);
	}

	return S_OK;
}

HRESULT Mesh_Data::Ready_Vertices_Anim(const aiMesh* pAIMesh, const vector<BONE_DATA> Bones)
{
	for (size_t i = 0; i < iNumVertices; i++)
	{
		VTXMESHANIM AnimVertex = {};
		memcpy(&AnimVertex.vPosition, &pAIMesh->mVertices[i], sizeof(_float3));
		memcpy(&AnimVertex.vNormal, &pAIMesh->mNormals[i], sizeof(_float3));
		memcpy(&AnimVertex.vTexcoord, &pAIMesh->mTextureCoords[0][i], sizeof(_float3));
		memcpy(&AnimVertex.vTangent, &pAIMesh->mTangents[i], sizeof(_float3));
		pAnimVertices.push_back(AnimVertex);
	}

	iNumBones = pAIMesh->mNumBones;


	/* 이 메시에게 영향을 주는 뼈을 순회하면서 각각의 뼈가 어떤 정점들에게 영향을 주는지 파악한다.*/
	for (_uint i = 0; i < pAIMesh->mNumBones; i++)
	{
		aiBone* pAIBone = pAIMesh->mBones[i];

		_float4x4 OffsetMatrix;
		memcpy(&OffsetMatrix, &pAIBone->mOffsetMatrix, sizeof(_float4x4)); XMStoreFloat4x4(&OffsetMatrix, XMMatrixTranspose(XMLoadFloat4x4(&OffsetMatrix)));
		OffsetMatrices.push_back(OffsetMatrix);

		_uint iBoneIndex = { 0 };

		auto iter = find_if(Bones.begin(), Bones.end(), [&](BONE_DATA pBone) {
			if (!strcmp(pAIBone->mName.data, pBone.szName.c_str()))
				return true;

			++iBoneIndex;
			return false;
			});

		if (iter == Bones.end())
			RETURN_EFAIL;

		BoneIndices.push_back(iBoneIndex);

		/* 이 뼈는 몇개의 정점에게 영향을 주는가?! */
		for (_uint j = 0; j < pAIBone->mNumWeights; j++)
		{
			/* pAIBone->mWeights[j].mVertexId : 이 뼈가 영향을 주는 j번째 정점의 인덱스 */
			if (0.0f == pAnimVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.x)
			{
				pAnimVertices[pAIBone->mWeights[j].mVertexId].vBlendIndices.x = i;
				pAnimVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.x = pAIBone->mWeights[j].mWeight;
			}

			else if (0.0f == pAnimVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.y)
			{
				pAnimVertices[pAIBone->mWeights[j].mVertexId].vBlendIndices.y = i;
				pAnimVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.y = pAIBone->mWeights[j].mWeight;
			}

			else if (0.0f == pAnimVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.z)
			{
				pAnimVertices[pAIBone->mWeights[j].mVertexId].vBlendIndices.z = i;
				pAnimVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.z = pAIBone->mWeights[j].mWeight;
			}

			else if (0.0f == pAnimVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.w)
			{
				pAnimVertices[pAIBone->mWeights[j].mVertexId].vBlendIndices.w = i;
				pAnimVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.w = pAIBone->mWeights[j].mWeight;
			}
		}
	};

	if (0 == iNumBones)
	{
		iNumBones = 1;

		_uint		iBoneIndex = { 0 };

		auto	iter = find_if(Bones.begin(), Bones.end(), [&](BONE_DATA pBone)
			{
				if (szName == pBone.szName)
				{
					return true;
				}

				++iBoneIndex;

				return false;
			});

		if (iter == Bones.end())
			RETURN_EFAIL;

		BoneIndices.push_back(iBoneIndex);

		_float4x4		OffsetMatrix;
		XMStoreFloat4x4(&OffsetMatrix, XMMatrixIdentity());
		OffsetMatrices.push_back(OffsetMatrix);
	}
	return S_OK;
}
#endif

HRESULT Mesh_Data::Bake_Binary(ofstream& fout)
{
	size_t strSize = szName.size();
	fout.write(reinterpret_cast<char*>(&strSize), sizeof(strSize));
	fout.write(reinterpret_cast<char*>(&szName[0]), (strSize));

	fout.write(reinterpret_cast<char*>(&iMaterialIndex), sizeof(iMaterialIndex));
	fout.write(reinterpret_cast<char*>(&iNumVertices), sizeof(iNumVertices));
	fout.write(reinterpret_cast<char*>(&iNumFaces), sizeof(iNumFaces));
	fout.write(reinterpret_cast<char*>(&iNumBones), sizeof(iNumBones));
	fout.write(reinterpret_cast<char*>(&iNumOffsetMatrices), sizeof(iNumOffsetMatrices));

	fout.write(reinterpret_cast<char*>(&eModelType), sizeof(eModelType));

	if (eModelType == NONANIM)
	{
		for(auto& iter : pVertices)
			fout.write(reinterpret_cast<char*>(&iter), sizeof(VTXMESH));
	}
	else if (eModelType == ANIM)
	{
		for (auto& iter : pAnimVertices)
			fout.write(reinterpret_cast<char*>(&iter), sizeof(VTXMESHANIM));
	}

	for (auto& iter : pIndices)
		fout.write(reinterpret_cast<char*>(&iter), sizeof(_uint));

	for(auto& iter : BoneIndices)
		fout.write(reinterpret_cast<char*>(&iter), sizeof(_uint));

	for (auto& iter : OffsetMatrices)
		fout.write(reinterpret_cast<char*>(&iter), sizeof(_float4x4));

	return S_OK;
}

HRESULT Mesh_Data::Load_Binary(ifstream& fin)
{
	size_t strSize = szName.size();
	fin.read(reinterpret_cast<char*>(&strSize), sizeof(strSize));
	if (strSize >= 100)
		return S_OK;
	szName.resize(strSize);
	fin.read(reinterpret_cast<char*>(&szName[0]), (strSize));

	fin.read(reinterpret_cast<char*>(&iMaterialIndex), sizeof(iMaterialIndex));
	fin.read(reinterpret_cast<char*>(&iNumVertices), sizeof(iNumVertices));
	fin.read(reinterpret_cast<char*>(&iNumFaces), sizeof(iNumFaces));
	fin.read(reinterpret_cast<char*>(&iNumBones), sizeof(iNumBones));
	fin.read(reinterpret_cast<char*>(&iNumOffsetMatrices), sizeof(iNumOffsetMatrices));

	fin.read(reinterpret_cast<char*>(&eModelType), sizeof(eModelType));

	if (eModelType == NONANIM)
	{
		for (_uint i = 0; i < iNumVertices; i++)
		{
			VTXMESH Vertex = {};
			fin.read(reinterpret_cast<char*>(&Vertex), sizeof(VTXMESH));
			pVertices.push_back(Vertex);
		}
	}
	else if (eModelType == ANIM)
	{
		for (_uint i = 0; i < iNumVertices; i++)
		{
			VTXMESHANIM AnimVertex = {};
			fin.read(reinterpret_cast<char*>(&AnimVertex), sizeof(VTXMESHANIM));
			pAnimVertices.push_back(AnimVertex);
		}
	}

	for (_uint i = 0; i < iNumFaces * 3; i++)
	{
		_uint Index = {};
		fin.read(reinterpret_cast<char*>(&Index), sizeof(_uint));
		pIndices.push_back(Index);
	}
	
	for (_uint i = 0; i < iNumBones; i++)
	{
		_uint iBoneIndex = {};
		fin.read(reinterpret_cast<char*>(&iBoneIndex), sizeof(iBoneIndex));
		BoneIndices.push_back(iBoneIndex);
	}

	for (_uint i = 0; i < iNumOffsetMatrices; i++)
	{
		_float4x4 OffsetMatrix = {};
		fin.read(reinterpret_cast<char*>(&OffsetMatrix), sizeof(OffsetMatrix));
		OffsetMatrices.push_back(OffsetMatrix);
	}

	return S_OK;
}
