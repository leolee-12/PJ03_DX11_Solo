#include "Model_Loader.h"
using namespace Assimp;

HRESULT XM_CALLCONV CModel_Loader::Export_Binary(const _char* pFbxPath, const _char* pOutputPath,
	MODEL eType, _fmatrix PreTransform, const _char* pMappingJsonPath)
{
	if (!Is_ModelLoaded() || m_strFbxPath != pFbxPath)
		if (FAILED(Load_FBX(pFbxPath, eType, PreTransform))) return E_FAIL;

	if (pMappingJsonPath != nullptr)
		if (FAILED(Apply_MappingJSON(pMappingJsonPath))) return E_FAIL;

	if (FAILED(Write_Binary(pOutputPath))) return E_FAIL;

	_tchar szMsg[512] = {};
	swprintf_s(szMsg, L"Export 완료\n- Bones: %zu\n- Meshes: %zu\n- Materials: %zu\n- Animations: %zu\n", m_Bones.size(), m_Meshes.size(), m_Materials.size(), m_Animations.size());
	MessageBox(NULL, szMsg, L"System Message", MB_OK);

	return S_OK;
}

HRESULT XM_CALLCONV CModel_Loader::Export_JSON(const _char* pFbxPath, const _char* pOutputPath,
	MODEL eType, _fmatrix PreTransform, _uint iVertexSampleCount)
{
	if (FAILED(Load_FBX(pFbxPath, eType, PreTransform))) return E_FAIL;
	return Write_JSON(pOutputPath, iVertexSampleCount);
}

HRESULT XM_CALLCONV CModel_Loader::Export_All(const _char* pFbxPath, const _char* pOutputDir,
	MODEL eType, _fmatrix PreTransform, const _char* pMappingJsonPath)
{
	if (FAILED(Load_FBX(pFbxPath, eType, PreTransform))) return E_FAIL;

	if (pMappingJsonPath != nullptr)
		if (FAILED(Apply_MappingJSON(pMappingJsonPath))) return E_FAIL;

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

	// * 루트노드에 사전변환 적용
	if (m_Bones.empty())
	{
		MSG_BOX("Model_Loader : Bone 추출 결과가 없음");
		return E_FAIL;
	}
	
	XMStoreFloat4x4(&m_Bones[0].transformation,
		PreTransform * XMLoadFloat4x4(&m_Bones[0].transformation));

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

	memcpy(m_tHeader.szMagic, "WMDL", 4);
	m_tHeader.iVersion = 2;
	m_tHeader.iModelType = ETOUI(m_eType);
	m_tHeader.iNumMeshes = static_cast<_uint>(m_Meshes.size());
	m_tHeader.iNumMaterials = static_cast<_uint>(m_Materials.size());
	m_tHeader.iNumBones = static_cast<_uint>(m_Bones.size());
	m_tHeader.iNumAnimations = static_cast<_uint>(m_Animations.size());

	return S_OK;
}

HRESULT CModel_Loader::Generate_MappingJSON(const _char* pTexDir, const _char* pOutputPath)
{
	if (!Is_ModelLoaded()) return E_FAIL;

	namespace fs = std::filesystem;
	json root;

	// 1. 모델 정보
	root["model"] = fs::path(m_strFbxPath).stem().string();

	// 2. 슬롯 정보 참고
	root["slot_reference"] =
	{
			{ "1",  "DIFFUSE" },
			{ "2",  "SPECULAR" },
			{ "3",  "AMBIENT" },
			{ "4",  "EMISSIVE" },
			{ "6",  "NORMALS" },
			{ "7",  "SHININESS" },
			{ "8",  "OPACITY" },
			{ "17", "AMBIENT_OCCLUSION" },
			{ "28", "LAYER_MASK" },
			{ "29", "LAYER_COLOR" }
	};

	// 3. 텍스처 디렉토리 내 파일 목록
	root["available_textures"] = json::array();
	if (pTexDir != nullptr && fs::exists(pTexDir) && fs::is_directory(pTexDir))
	{
		for (auto& entry : fs::directory_iterator(pTexDir))
		{
			if (entry.is_regular_file())
				root["available_textures"].push_back(entry.path().filename().string());
		}
		sort(root["available_textures"].begin(), root["available_textures"].end());
	}

	// 4. 머테리얼 목록
	root["materials"] = json::array();
	for (_uint i = 0; i < static_cast<_uint>(m_Materials.size()); ++i)
	{
		json matEntry;
		matEntry["index"] = i;

		// 머테리얼 이름 (Assimp)
		aiString matName;
		m_pAIScene->mMaterials[i]->Get(AI_MATKEY_NAME, matName);
		matEntry["name"] = matName.C_Str();

		// 이 머테리얼을 참조하는 메쉬 목록
		matEntry["meshes"] = json::array();
		for (auto& mesh : m_Meshes)
		{
			if (mesh.iMaterialIndex == i)
				matEntry["meshes"].push_back(mesh.szName);
		}

		// 텍스처 슬롯 - 직접 채울 것
		matEntry["textures"] = json::object();

		root["materials"].push_back(matEntry);
	}

	// 5. JSON 파일 출력
	ofstream(pOutputPath) << root.dump(2);
	return S_OK;
}

HRESULT CModel_Loader::Apply_MappingJSON(const _char* pMappingJsonPath)
{
	ifstream f(pMappingJsonPath);
	if (!f.is_open())
	{
		MSG_BOX("Model_Loader : Mapping JSON 열기 실패");
		return E_FAIL;
	}

	json root = json::parse(f);
	if (!root.contains("materials"))
	{
		MSG_BOX("Model_Loader : Mapping JSON에 materials 키 없음");
		return E_FAIL;
	}

	for (auto& matEntry : root["materials"])
	{
		_uint idx = matEntry["index"].get<_uint>();
		if (idx >= static_cast<_uint>(m_Materials.size())) continue;

		// 기존 데이터 초기화
		for (auto& paths : m_Materials[idx].TexturePaths)
			paths.clear();

		// JSON에서 읽어서 배정
		if (!matEntry.contains("textures")) continue;

		for (auto& [slotStr, files] : matEntry["textures"].items())
		{
			int iSlot = stoi(slotStr);
			if (iSlot < 0 || iSlot >= ETOUI(MATERIAL_TYPE::END)) continue;

			for (auto& file : files)
				m_Materials[idx].TexturePaths[iSlot].push_back(file.get<_string>());
		}
	}

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

			// 3-4. weight 정규화 (fallback)
			for (_uint v = 0; v < iNumVertices; ++v)
			{
				_float fSum =
					pVertices[v].vBlendWeight.x +
					pVertices[v].vBlendWeight.y +
					pVertices[v].vBlendWeight.z +
					pVertices[v].vBlendWeight.w;

				if (fSum > 0.f)
				{
					pVertices[v].vBlendWeight.x /= fSum;
					pVertices[v].vBlendWeight.y /= fSum;
					pVertices[v].vBlendWeight.z /= fSum;
					pVertices[v].vBlendWeight.w /= fSum;
				}
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
	aiString AITexPath = {};
	_uint iNumMaterials = m_pAIScene->mNumMaterials;

	for (_uint i = 0; i < iNumMaterials; ++i)
	{
		WMODEL_MATERIAL Material = {};
		aiMaterial* pAIMaterial = m_pAIScene->mMaterials[i];

		// 1. 각 텍스처 타입에 대해 작업
		for (_uint j = 0; j < AI_TEXTURE_TYPE_MAX; ++j)
		{
			if (j >= ETOUI(MATERIAL_TYPE::END)) continue;

			// 2. 텍스처마다 경로 추출하여 경로 컨테이너에 삽입
			_uint iNumTextures = pAIMaterial->GetTextureCount(static_cast<aiTextureType>(j));
			Material.TexturePaths[j].reserve(iNumTextures);

			for (_uint k = 0; k < iNumTextures; ++k)
			{
				if (pAIMaterial->GetTexture(static_cast<aiTextureType>(j), k, &AITexPath))
					return E_FAIL;

				_string filename = fs::path(AITexPath.data).filename().string();
				Material.TexturePaths[j].push_back(filename);
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
		// 1. 애니메이션의 Name, Duration, TicksPerSecond 정보 저장
		aiAnimation* pAIAnim = m_pAIScene->mAnimations[i];
		WMODEL_ANIMATION Anim = {};
		strcpy_s(Anim.szName, pAIAnim->mName.data);
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

			_vector vScale, vRotation, vTranslation;
			_matrix bindLocal = XMLoadFloat4x4(&m_Bones[iBoneIndex].transformation);

			if (false == XMMatrixDecompose(&vScale, &vRotation, &vTranslation, bindLocal))
			{
				vScale = XMVectorSet(1.f, 1.f, 1.f, 0.f);
				vRotation = XMQuaternionIdentity();
				vTranslation = XMVectorZero();
			}

			XMStoreFloat3(&Channel.vDefaultScale, vScale);
			XMStoreFloat4(&Channel.vDefaultRotation, vRotation);
			XMStoreFloat3(&Channel.vDefaultTranslation, vTranslation);

			_uint iNumScalingKeys = pAINodeAnim->mNumScalingKeys;
			_uint iNumRotationKeys = pAINodeAnim->mNumRotationKeys;
			_uint iNumPositionKeys = pAINodeAnim->mNumPositionKeys;
			Channel.scalingKeys.reserve(iNumScalingKeys);
			Channel.rotationKeys.reserve(iNumRotationKeys);
			Channel.positionKeys.reserve(iNumPositionKeys);

			// 3. 스케일 키 추출
			for (_uint k = 0; k < iNumScalingKeys; ++k)
			{
				SCALING_KEY key{};
				key.vScale.x = pAINodeAnim->mScalingKeys[k].mValue.x;
				key.vScale.y = pAINodeAnim->mScalingKeys[k].mValue.y;
				key.vScale.z = pAINodeAnim->mScalingKeys[k].mValue.z;
				key.fTrackPosition = static_cast<_float>(pAINodeAnim->mScalingKeys[k].mTime);
				Channel.scalingKeys.push_back(key);
			}

			// 4. 회전 키 추출
			for (_uint k = 0; k < iNumRotationKeys; ++k)
			{
				ROTATION_KEY key{};
				key.vRotation.x = pAINodeAnim->mRotationKeys[k].mValue.x;
				key.vRotation.y = pAINodeAnim->mRotationKeys[k].mValue.y;
				key.vRotation.z = pAINodeAnim->mRotationKeys[k].mValue.z;
				key.vRotation.w = pAINodeAnim->mRotationKeys[k].mValue.w;
				key.fTrackPosition = static_cast<_float>(pAINodeAnim->mRotationKeys[k].mTime);
				Channel.rotationKeys.push_back(key);

				// 4-1. 회전 키프레임 전처리 : hemisphere 통일
				if (k > 0)
				{
					_vector vPrevQuat = XMLoadFloat4(&Channel.rotationKeys[k - 1].vRotation);
					_vector vCurrQuat = XMLoadFloat4(&Channel.rotationKeys[k].vRotation);

					// 이전 키프레임과 내적했을 때 음수이면(다른 반구에 있다면)
					if (XMVectorGetX(XMVector4Dot(vPrevQuat, vCurrQuat)) < 0.f)
					{
						vCurrQuat = XMVectorNegate(vCurrQuat);	// 모든 성분에 -1을 곱함
						XMStoreFloat4(&Channel.rotationKeys[k].vRotation, vCurrQuat);
					}
				}
			}

			// 5. 위치 키 추출
			for (_uint k = 0; k < iNumPositionKeys; ++k)
			{
				POSITION_KEY key{};
				key.vTranslation.x = pAINodeAnim->mPositionKeys[k].mValue.x;
				key.vTranslation.y = pAINodeAnim->mPositionKeys[k].mValue.y;
				key.vTranslation.z = pAINodeAnim->mPositionKeys[k].mValue.z;
				key.fTrackPosition = static_cast<_float>(pAINodeAnim->mPositionKeys[k].mTime);
				Channel.positionKeys.push_back(key);
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
		for (_uint i = 0; i < ETOUI(MATERIAL_TYPE::END); ++i)
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
		fwrite(anim.szName, 1, MAX_PATH, fp);
		fwrite(&anim.fDuration, sizeof(_float), 1, fp);
		fwrite(&anim.fTicksPerSecond, sizeof(_float), 1, fp);
		_uint numChannels = static_cast<_uint>(anim.channels.size());
		fwrite(&numChannels, sizeof(_uint), 1, fp);

		for (auto& channel : anim.channels)
		{
			fwrite(&channel.iBoneIndex, sizeof(_uint), 1, fp);
			fwrite(&channel.vDefaultScale, sizeof(_float3), 1, fp);
			fwrite(&channel.vDefaultRotation, sizeof(_float4), 1, fp);
			fwrite(&channel.vDefaultTranslation, sizeof(_float3), 1, fp);
			_uint numScalingKeys = static_cast<_uint>(channel.scalingKeys.size());
			_uint numRotationKeys = static_cast<_uint>(channel.rotationKeys.size());
			_uint numPositionKeys = static_cast<_uint>(channel.positionKeys.size());

			fwrite(&numScalingKeys, sizeof(_uint), 1, fp);
			fwrite(&numRotationKeys, sizeof(_uint), 1, fp);
			fwrite(&numPositionKeys, sizeof(_uint), 1, fp);

			if (!channel.scalingKeys.empty())
				fwrite(channel.scalingKeys.data(), sizeof(SCALING_KEY), numScalingKeys, fp);
			if (!channel.rotationKeys.empty())
				fwrite(channel.rotationKeys.data(), sizeof(ROTATION_KEY), numRotationKeys, fp);
			if (!channel.positionKeys.empty())
				fwrite(channel.positionKeys.data(), sizeof(POSITION_KEY), numPositionKeys, fp);
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

		for (_uint slot = 1; slot < ETOUI(MATERIAL_TYPE::END); ++slot)
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
		anim_entry["name"] = anim.szName;
		anim_entry["duration"] = anim.fDuration;
		anim_entry["tickPerSecond"] = anim.fTicksPerSecond;
		anim_entry["numChannels"] = anim.channels.size();

		anim_entry["channels"] = json::array();
		for (auto& channel : anim.channels)
		{
			json ch_entry;
			ch_entry["boneIndex"] = channel.iBoneIndex;
			ch_entry["defaultScale"] =
			{
				channel.vDefaultScale.x,
				channel.vDefaultScale.y,
				channel.vDefaultScale.z
			};
			ch_entry["defaultRotation"] =
			{
				channel.vDefaultRotation.x,
				channel.vDefaultRotation.y,
				channel.vDefaultRotation.z,
				channel.vDefaultRotation.w
			};
			ch_entry["defaultTranslation"] =
			{
				channel.vDefaultTranslation.x,
				channel.vDefaultTranslation.y,
				channel.vDefaultTranslation.z
			};
			ch_entry["numScalingKeys"] = channel.scalingKeys.size();
			ch_entry["numRotationKeys"] = channel.rotationKeys.size();
			ch_entry["numPositionKeys"] = channel.positionKeys.size();
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
