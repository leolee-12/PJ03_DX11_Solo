#include "Model_Loader.h"
//using namespace Assimp;

HRESULT XM_CALLCONV CModel_Loader::Export_Binary(const _char* pFbxPath, const _char* pOutputPath, MODEL eType, _fmatrix PreTransform)
{
	if (!Is_ModelLoaded() || m_strFbxPath != pFbxPath)
		if (FAILED(Load_FBX(pFbxPath, eType, PreTransform))) return E_FAIL;

	if (FAILED(Write_Binary(pOutputPath))) return E_FAIL;

	_tchar szMsg[512] = {};
	wprintf_s(szMsg, L"Export 완료\n- Bones: %zu\n- Meshes: %zu\n- Materials: %zu\n- Animations: %zu\n- 출력: %s",
		m_Bones.size(), m_Meshes.size(), m_Materials.size(), m_Animations.size(), pOutputPath);
	MessageBox(NULL, szMsg, L"System Message", MB_OK);

	return S_OK;
}

HRESULT XM_CALLCONV CModel_Loader::Export_JSON(const _char* pFbxPath, const _char* pOutputPath, MODEL eType, _fmatrix PreTransform, _uint iVertexSampleCount)
{
	if (FAILED(Load_FBX(pFbxPath, eType, PreTransform))) return E_FAIL;
	return Write_JSON(pOutputPath, iVertexSampleCount);
}

HRESULT XM_CALLCONV CModel_Loader::Export_All(const _char* pFbxPath, const _char* pOutputDir, MODEL eType, _fmatrix PreTransform)
{
	if (FAILED(Load_FBX(pFbxPath, eType, PreTransform))) return E_FAIL;

	namespace fs = std::filesystem;
	_string stem = fs::path(pFbxPath).stem().string();
	_string binPath = _string(pOutputDir) + "/" + stem + ".wmodel";
	_string jsonPath = _string(pOutputDir) + "/" + stem + ".wmodel.json";

	if (FAILED(Write_Binary(binPath.c_str())))   return E_FAIL;
	if (FAILED(Write_JSON(jsonPath.c_str(), 3))) return E_FAIL;
	return S_OK;
}

HRESULT CModel_Loader::Initialize()
{
	return S_OK;
}

HRESULT XM_CALLCONV CModel_Loader::Load_FBX(const _char* pFbxPath, MODEL eType, _fmatrix PreTransform) // ~ CModel::Initialize_Prototype()
{
	// 0. 멤버 초기화
	Clear_Data();

	// 1. 변수 저장
	m_eType = eType;
	XMStoreFloat4x4(&m_PreTransformMatrix, PreTransform);
	m_strFbxPath = pFbxPath;

	// 2. iFlag 설정
	_uint iFlag = { aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast };
	if (m_eType == MODEL::NONANIM)
		iFlag |= aiProcess_PreTransformVertices;

	// 3. aiScene 로드
	m_pAIScene = m_Importer.ReadFile(m_strFbxPath.c_str(), iFlag);
	if (nullptr == m_pAIScene)
	{
		MSG_BOX("Model_Loader : FBX 파일 로드 실패");
		return E_FAIL;
	}

	// 4. Bones, Meshes, Materials, Animations 추출
	m_Bones.reserve(m_pAIScene->mRootNode->mNumChildren * 5);
	m_Meshes.reserve(m_pAIScene->mNumMeshes);
	m_Materials.reserve(m_pAIScene->mNumMaterials);
	m_Animations.reserve(m_pAIScene->mNumAnimations);

	if (FAILED(Extract_Bones(m_pAIScene->mRootNode, -1)))
	{
		MSG_BOX("Model_Loader : Bone 추출 실패");
		return E_FAIL;
	}
	if (FAILED(Extract_Meshes()))
	{
		MSG_BOX("Model_Loader : Mesh 추출 실패");
		return E_FAIL;
	}
	if (FAILED(Extract_Materials()))
	{
		MSG_BOX("Model_Loader : Material 추출 실패");
		return E_FAIL;
	}
	if (m_eType == MODEL::ANIM)
	{
		if (FAILED(Extract_Animations()))
		{
			MSG_BOX("Model_Loader : Animation 추출 실패");
			return E_FAIL;
		}
	}

	m_tHeader.szMagic[4] = {};
	m_tHeader.iVersion = {};
	m_tHeader.iModelType = ETOUI(m_eType);
	m_tHeader.iNumMeshes = static_cast<_uint>(m_Meshes.size());
	m_tHeader.iNumMaterials = static_cast<_uint>(m_Materials.size());;
	m_tHeader.iNumBones = static_cast<_uint>(m_Bones.size());;
	m_tHeader.iNumAnimations = static_cast<_uint>(m_Animations.size());;

	return S_OK;
}

HRESULT CModel_Loader::Extract_Bones(aiNode* pAINode, _int iParentIndex)	// ~ CModel::Ready_Bones() + CBone::Initialize()
{
	WMODEL_BONE Bone = {};

	// 1. Bone 이름, ParentIndex 저장
	strcpy_s(Bone.szName, pAINode->mName.data);
	Bone.iParentIndex = iParentIndex;

	// 2. Transformation 행렬 전치 후 저장
	memcpy(&Bone.transformation, &pAINode->mTransformation, sizeof(_float4x4));
	XMStoreFloat4x4(&Bone.transformation, XMMatrixTranspose(XMLoadFloat4x4(&Bone.transformation)));

	// 3. 컨테이너에 저장
	m_Bones.push_back(Bone);

	// 4. 자식 노드에 대해서도 재귀적으로 처리
	_int iMyIndex = static_cast<_int>(m_Bones.size()) - 1;
	_uint iNumChildren = pAINode->mNumChildren;

	for (_uint i = 0; i < iNumChildren; ++i)
	{
		if (FAILED(Extract_Bones(pAINode->mChildren[i], iMyIndex)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CModel_Loader::Extract_Meshes()	// ~ CMesh::Initialize_Prototype() (Ready_NonAnimMesh()/Ready_AnimMesh())
{
	// 0. 각 메쉬에 대해 작업
	_uint iNumMeshes = m_pAIScene->mNumMeshes;
	_matrix PreTM = XMLoadFloat4x4(&m_PreTransformMatrix);

	// 1. 메쉬 이름, 재질 인덱스 추출
	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		WMODEL_MESH Mesh = {};
		aiMesh* pAIMesh = m_pAIScene->mMeshes[i];
		strcpy_s(Mesh.szName, pAIMesh->mName.data);
		Mesh.iMaterialIndex = pAIMesh->mMaterialIndex;
		
		// 2. 인덱스 데이터 추출
		_uint iNumFaces = pAIMesh->mNumFaces;

		for (_uint j = 0; j < iNumFaces; ++j)
		{
			aiFace& AIFace = pAIMesh->mFaces[j];
			Mesh.indices.push_back(AIFace.mIndices[0]);
			Mesh.indices.push_back(AIFace.mIndices[1]);
			Mesh.indices.push_back(AIFace.mIndices[2]);
		}

		// 3. 버텍스 데이터 추출
		_uint iNumVertices = pAIMesh->mNumVertices;

		if (m_eType == MODEL::NONANIM)
		{
			VTXMESH* pVertices = new VTXMESH[iNumVertices];
			ZeroMemory(pVertices, sizeof(VTXMESH) * iNumVertices);

			for (size_t j = 0; j < iNumVertices; ++j)
			{
				memcpy(&pVertices[j].vPosition, &pAIMesh->mVertices[j], sizeof(_float3));
				memcpy(&pVertices[j].vNormal, &pAIMesh->mNormals[j], sizeof(_float3));
				memcpy(&pVertices[j].vTexcoord, &pAIMesh->mTextureCoords[0][j], sizeof(_float2));
				memcpy(&pVertices[j].vTangent, &pAIMesh->mTangents[j], sizeof(_float3));
				memcpy(&pVertices[j].vBinormal, &pAIMesh->mBitangents[j], sizeof(_float3));

				// PreTransform 적용
				XMStoreFloat3(&pVertices[j].vPosition, XMVector3TransformCoord(XMLoadFloat3(&pVertices[j].vPosition), PreTM));
				XMStoreFloat3(&pVertices[j].vNormal, XMVector3TransformNormal(XMLoadFloat3(&pVertices[j].vNormal), PreTM));
				XMStoreFloat3(&pVertices[j].vTangent, XMVector3TransformNormal(XMLoadFloat3(&pVertices[j].vTangent), PreTM));
				XMStoreFloat3(&pVertices[j].vBinormal, XMVector3TransformNormal(XMLoadFloat3(&pVertices[j].vBinormal), PreTM));
			}

			Mesh.nonAnimVertices = vector<VTXMESH>(pVertices, pVertices + iNumVertices);
			Safe_Delete_Array(pVertices);
		}
		else if (m_eType == MODEL::ANIM)
		{
			VTXANIMMESH* pVertices = new VTXANIMMESH[iNumVertices];
			ZeroMemory(pVertices, sizeof(VTXANIMMESH) * iNumVertices);

			for (size_t j = 0; j < iNumVertices; ++j)
			{
				memcpy(&pVertices[j].vPosition, &pAIMesh->mVertices[j], sizeof(_float3));
				memcpy(&pVertices[j].vNormal, &pAIMesh->mNormals[j], sizeof(_float3));
				memcpy(&pVertices[j].vTexcoord, &pAIMesh->mTextureCoords[0][j], sizeof(_float2));
				memcpy(&pVertices[j].vTangent, &pAIMesh->mTangents[j], sizeof(_float3));
				memcpy(&pVertices[j].vBinormal, &pAIMesh->mBitangents[j], sizeof(_float3));
			}

			// 3-1. AnimMesh인 경우, Bone 이름, 인덱스, offsetMatrix 추출
			_uint iNumBones = pAIMesh->mNumBones;
			Mesh.boneIndices.reserve(iNumBones);
			for (_uint j = 0; j < iNumBones; ++j)
			{
				aiBone* pAIBone = pAIMesh->mBones[j];
				_uint iBoneIndex = Find_BoneIndex(pAIBone->mName.data);
				if (iBoneIndex == -1)
					return E_FAIL;

				Mesh.boneIndices.push_back(iBoneIndex);

				_float4x4 offsetMatrix = {};
				XMStoreFloat4x4(&offsetMatrix, XMMatrixIdentity());
				memcpy(&offsetMatrix, &pAIBone->mOffsetMatrix, sizeof offsetMatrix);
				XMStoreFloat4x4(&offsetMatrix, XMMatrixTranspose(XMLoadFloat4x4(&offsetMatrix)));
				Mesh.offsetMatrices.push_back(offsetMatrix);

				// 3-2. Bone 영향 받는 버텍스의 가중치 저장
				_uint iNumWeights = pAIBone->mNumWeights;

				for (_uint k = 0; k < iNumWeights; ++k)
				{
					aiVertexWeight& AIVertexWeight = pAIBone->mWeights[k];
					VTXANIMMESH& vertex = pVertices[AIVertexWeight.mVertexId];

					if (0.f == vertex.vBlendWeight.x)
					{
						vertex.vBlendIndex.x = j;
						vertex.vBlendWeight.x = AIVertexWeight.mWeight;
					}
					else if (0.f == pVertices[AIVertexWeight.mVertexId].vBlendWeight.y)
					{
						vertex.vBlendIndex.y = j;
						vertex.vBlendWeight.y = AIVertexWeight.mWeight;
					}
					else if (0.f == pVertices[AIVertexWeight.mVertexId].vBlendWeight.z)
					{
						vertex.vBlendIndex.z = j;
						vertex.vBlendWeight.z = AIVertexWeight.mWeight;
					}
					else
					{
						vertex.vBlendIndex.w = j;
						vertex.vBlendWeight.w = AIVertexWeight.mWeight;
					}
				}
			}

			// 3-3. Bone이 없는 경우, 예외 처리
			if (0 == iNumBones)
			{
				iNumBones = 1;

				_uint iBoneIndex = Find_BoneIndex(Mesh.szName);
				if (-1 == iBoneIndex)
					return E_FAIL;

				_float4x4 OffsetMatrix = {};
				XMStoreFloat4x4(&OffsetMatrix, XMMatrixIdentity());
				Mesh.boneIndices.push_back(iBoneIndex);
				Mesh.offsetMatrices.push_back(OffsetMatrix);
			}

			Mesh.animVertices = vector<VTXANIMMESH>(pVertices, pVertices + iNumVertices);
			Safe_Delete_Array(pVertices);
		}

		// 4. 메쉬를 컨테이너에 저장
		m_Meshes.push_back(Mesh);
	}

	return S_OK;
}

HRESULT CModel_Loader::Extract_Materials()	// ~ CModel::Ready_Materials() + CMaterial::Initialize_Prototype()
{
	// 0. 각 재질에 대해 작업
	namespace fs = std::filesystem;
	fs::path modelDir = fs::path(m_strFbxPath).parent_path();
	aiString AITexPath = {};
	_uint iNumMaterials = m_pAIScene->mNumMaterials;

	for (_uint i = 0; i < iNumMaterials; ++i)
	{
		WMODEL_MATERIAL Material = {};
		aiMaterial* pAIMaterial = m_pAIScene->mMaterials[i];

		// 1. 각 텍스처 타입에 대해 작업
		for (_uint j = 0; j < AI_TEXTURE_TYPE_MAX; ++j)
		{
			if (j >= ETOUI(TEXTURE_TYPE::END)) continue;

			// 2. 텍스처마다 경로 추출하여 경로 컨테이너에 삽입
			_uint iNumTextures = pAIMaterial->GetTextureCount(static_cast<aiTextureType>(j));
			Material.TexturePaths[j].reserve(iNumTextures);

			for (_uint k = 0; k < iNumTextures; ++k)
			{
				if (pAIMaterial->GetTexture(static_cast<aiTextureType>(j), k, &AITexPath))
					return E_FAIL;

				_string filename = fs::path(AITexPath.data).filename().string();
				_string fullPath = modelDir.string() + "/" + filename;
				Material.TexturePaths[j].push_back(fullPath);
			}
		}

		// 3. 재질 컨테이너에 삽입
		m_Materials.push_back(Material);
	}

	return S_OK;
}

HRESULT CModel_Loader::Extract_Animations() // ~ CModel::Ready_Animations + CAnimation::Initialize + CChannel::Initialize
{
	// 0. 각 애니메이션에 대해 작업
	_uint iNumAnimations = m_pAIScene->mNumAnimations;

	for (_uint i = 0; i < iNumAnimations; ++i)
	{
		// 1. 애니메이션의 Duration, TicksPerSecond 정보 저장
		aiAnimation* pAIAnim = m_pAIScene->mAnimations[i];
		WMODEL_ANIMATION Anim = {};
		Anim.fDuration = static_cast<_float>(pAIAnim->mDuration);
		Anim.fTicksPerSecond = static_cast<_float>(pAIAnim->mTicksPerSecond);

		// 2. 각 채널에 대해 작업
		_uint iNumChannels = pAIAnim->mNumChannels;
		Anim.channels.reserve(iNumChannels);

		for (_uint j = 0; j < iNumChannels; ++j)
		{
			aiNodeAnim* pAINodeAnim = pAIAnim->mChannels[j];
			_int iBoneIndex = Find_BoneIndex(pAINodeAnim->mNodeName.data);
			if (-1 == iBoneIndex)
				return E_FAIL;

			WMODEL_CHANNEL Channel = {};
			Channel.iBoneIndex = static_cast<_uint>(iBoneIndex);

			// 3. 각 키프레임에 대해 작업
			_uint iNumScalingKeys = pAINodeAnim->mNumScalingKeys;
			_uint iNumRotationKeys = pAINodeAnim->mNumRotationKeys;
			_uint iNumPositionKeys = pAINodeAnim->mNumPositionKeys;
			_uint iNumKeyFrames = max(max(iNumScalingKeys, iNumRotationKeys), iNumPositionKeys);
			Channel.keyFrames.reserve(iNumKeyFrames);

			_float3 vScale{};
			_float4 vRotation{};
			_float3 vTranslation{};
			_float fTrackPosition{};

			for (_uint k = 0; k < iNumKeyFrames; ++k)
			{
				// 4. 크기, 회전, 이동 값과 TrackPosition 데이터 추출
				if (k < iNumScalingKeys)
				{
					memcpy(&vScale, &pAINodeAnim->mScalingKeys[k].mValue, sizeof(_float3));
					fTrackPosition = static_cast<_float>(pAINodeAnim->mScalingKeys[k].mTime);
				}
				if (k < iNumRotationKeys)
				{
					vRotation.x = pAINodeAnim->mRotationKeys[k].mValue.x;
					vRotation.y = pAINodeAnim->mRotationKeys[k].mValue.y;
					vRotation.z = pAINodeAnim->mRotationKeys[k].mValue.z;
					vRotation.w = pAINodeAnim->mRotationKeys[k].mValue.w;
					fTrackPosition = static_cast<_float>(pAINodeAnim->mRotationKeys[k].mTime);
				}
				if (k < iNumPositionKeys)
				{
					memcpy(&vTranslation, &pAINodeAnim->mPositionKeys[k].mValue, sizeof(_float3));
					fTrackPosition = static_cast<_float>(pAINodeAnim->mPositionKeys[k].mTime);
				}

				// 5. 키프레임 컨테이너에 삽입
				Channel.keyFrames.push_back({ vScale, vRotation, vTranslation, fTrackPosition });
			}

			// 6. 채널 컨테이너에 삽입
			Anim.channels.push_back(Channel);
		}
		// 7. 애니메이션 컨테이너에 삽입
		m_Animations.push_back(Anim);
	}

	return S_OK;
}

_int CModel_Loader::Find_BoneIndex(const _char* pBoneName) const
{
	size_t iNumBones = m_Bones.size();

	for (_uint i = 0; i < iNumBones; ++i)
		if (strcmp(m_Bones[i].szName, pBoneName) == 0) return i;

	return -1;
}

HRESULT CModel_Loader::Write_Binary(const _char* pOutputPath) const
{
	// 1. 파일 열기
	FILE* fp{};
	errno_t errorOpen{};
	if (0 != fopen_s(&fp, pOutputPath, "wb") || nullptr == fp)
		return E_FAIL;

	// 2. WMODEL_HEADER 쓰기
	WMODEL_HEADER header{};
	memcpy(header.szMagic, "WMDL", 4);
	header.iVersion = 2;
	header.iModelType = static_cast<_uint>(m_eType);
	header.iNumMeshes = static_cast<_uint>(m_Meshes.size());
	header.iNumMaterials = static_cast<_uint>(m_Materials.size());
	header.iNumBones = static_cast<_uint>(m_Bones.size());
	header.iNumAnimations = static_cast<_uint>(m_Animations.size());
	fwrite(&header, sizeof(WMODEL_HEADER), 1, fp);

	// 3. 본 데이터 쓰기
	fwrite(m_Bones.data(), sizeof(WMODEL_BONE), m_Bones.size(), fp);

	// 4. 재질 데이터 쓰기
	for (auto& material : m_Materials)
	{
		for (_uint i = 0; i < ETOUI(TEXTURE_TYPE::END); ++i)
		{
			_uint numTex = static_cast<_uint>(material.TexturePaths[i].size());
			fwrite(&numTex, sizeof(_uint), 1, fp);

			for (auto& path : material.TexturePaths[i])
			{
				_uint len = static_cast<_uint>(path.size() + 1);
				fwrite(&len, sizeof(_uint), 1, fp);
				fwrite(path.c_str(), 1, len, fp);
			}
		}
	}

	// 5. 메쉬 데이터 쓰기
	for (auto& mesh : m_Meshes)
	{
		fwrite(mesh.szName, 1, MAX_PATH, fp);
		fwrite(&mesh.iMaterialIndex, sizeof(uint32_t), 1, fp);
		_uint numVerts = static_cast<_uint>(m_eType == MODEL::NONANIM ? mesh.nonAnimVertices.size() : mesh.animVertices.size());
		_uint numIdx = static_cast<_uint>(mesh.indices.size());
		_uint numBones = static_cast<_uint>(mesh.boneIndices.size()); // (NONANIM이면 0)
		fwrite(&numVerts, sizeof(_uint), 1, fp);
		fwrite(&numIdx, sizeof(_uint), 1, fp);
		fwrite(&numBones, sizeof(_uint), 1, fp);

		if (MODEL::NONANIM == m_eType)
		{
			fwrite(mesh.nonAnimVertices.data(), sizeof(VTXMESH), numVerts, fp);
		}
		else if (MODEL::ANIM == m_eType)
		{
			fwrite(mesh.boneIndices.data(), sizeof(_uint), numBones, fp);
			fwrite(mesh.offsetMatrices.data(), sizeof(_float4x4), numBones, fp);
			fwrite(mesh.animVertices.data(), sizeof(VTXANIMMESH), numVerts, fp);
		}

		fwrite(mesh.indices.data(), sizeof(_uint), numIdx, fp);
	}

	// 6. 애니메이션 데이터 쓰기
	for (auto& anim : m_Animations)
	{
		fwrite(&anim.fDuration, sizeof(_float), 1, fp);
		fwrite(&anim.fTicksPerSecond, sizeof(_float), 1, fp);
		_uint numChannels = static_cast<_uint>(anim.channels.size());
		fwrite(&numChannels, sizeof(_uint), 1, fp);

		for (auto& channel : anim.channels)
		{
			fwrite(&channel.iBoneIndex, sizeof(_uint), 1, fp);
			_uint numKF = static_cast<_uint>(channel.keyFrames.size());
			fwrite(&numKF, sizeof(_uint), 1, fp);
			fwrite(channel.keyFrames.data(), sizeof(KEYFRAME), numKF, fp);
		}
	}

	// 7. 파일 닫기
	fclose(fp);

	return S_OK;
}

HRESULT CModel_Loader::Write_JSON(const _char* pOutputPath, _uint iVertexSampleCount) const
{
	// 1. json 객체 생성
	json root;

	// 2. 헤더 데이터 쓰기
	root["header"] =
	{
		{ "magic",			"WMDL" },
		{ "version",		2 },
		{ "modelType",		m_eType == MODEL::NONANIM ? "NONANIM" : "ANIM" },
		{ "numMeshes",		m_Meshes.size() },
		{ "numMaterials",	m_Materials.size() },
		{ "numBones",		m_Bones.size() },
		{ "numAnimations",	m_Animations.size() }
	};

	// 3. 본 데이터 쓰기
	_uint iIndex{};

	root["bones"] = json::array();
	for (auto& bone : m_Bones)
	{
		const _float4x4& m = bone.transformation;
		json bn_entry;
		bn_entry["index"] = iIndex++;
		bn_entry["name"] = bone.szName;
		bn_entry["parentIndex"] = bone.iParentIndex;
		bn_entry["transformation"] = {	m._11, m._12, m._13, m._14,
										m._21, m._22, m._23, m._24, 
										m._31, m._32, m._33, m._34, 
										m._41, m._42, m._43, m._44 };
		root["bones"].push_back(bn_entry);
	}

	// 4. 재질 데이터 쓰기
	iIndex = 0;

	root["materials"] = json::array();
	for (auto& material : m_Materials)
	{
		json mat_entry;
		mat_entry["index"] = iIndex++;
		json textures_obj;

		for (_uint slot = 1; slot < ETOUI(TEXTURE_TYPE::END); ++slot)
		{
			if (material.TexturePaths[slot].empty()) continue;

			textures_obj[to_string(slot)] = material.TexturePaths[slot];
		}
		mat_entry["textures"] = textures_obj;
		root["materials"].push_back(mat_entry);
	}

	// 5. 메쉬 데이터 쓰기
	iIndex = 0;

	root["meshes"] = json::array();
	for (auto& mesh : m_Meshes)
	{
		size_t numVerts = (m_eType == MODEL::NONANIM ? mesh.nonAnimVertices.size() : mesh.animVertices.size());
		json ms_entry;
		ms_entry["index"] = iIndex++;
		ms_entry["name"] = mesh.szName;
		ms_entry["materialIndex"] = mesh.iMaterialIndex;
		ms_entry["numVertices"] = numVerts;
		ms_entry["numIndices"] = mesh.indices.size();
		ms_entry["numBones"] = mesh.boneIndices.size();
		root["meshes"].push_back(ms_entry);
	}

	// 6. 애니메이션 데이터 쓰기
	iIndex = 0;

	root["animations"] = json::array();
	for (auto& anim : m_Animations)
	{
		json anim_entry;
		anim_entry["index"] = iIndex++;
		anim_entry["duration"] = anim.fDuration;
		anim_entry["tickPerSecond"] = anim.fTicksPerSecond;
		anim_entry["numChannels"] = anim.channels.size();

		anim_entry["channels"] = json::array();
		for (auto& channel : anim.channels)
		{
			json ch_entry;
			ch_entry["boneIndex"] = channel.iBoneIndex;
			ch_entry["numKeyFrames"] = channel.keyFrames.size();
			anim_entry["channels"].push_back(ch_entry);
		}
		root["animations"].push_back(anim_entry);
	}

	// 7. json 내보내기
	ofstream(pOutputPath) << root.dump(2);

	return S_OK;
}

void CModel_Loader::Clear_Data()
{
	m_Importer.FreeScene();
	m_pAIScene = nullptr;

	m_eType = MODEL::END;
	XMStoreFloat4x4(&m_PreTransformMatrix, XMMatrixIdentity());
	m_strFbxPath.clear();

	m_Bones.clear();
	m_Meshes.clear();
	m_Materials.clear();
	m_Animations.clear();

	m_tHeader = {};
}

CModel_Loader* CModel_Loader::Create()
{
	CModel_Loader* pInstance = new CModel_Loader;

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Create : CModel_Loader");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CModel_Loader::Free()
{
	__super::Free();

	m_Importer.FreeScene();
}
