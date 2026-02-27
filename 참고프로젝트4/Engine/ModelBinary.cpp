#include "ModelBinary.h"
#include "Utility_File.h"

CModelBinary::CModelBinary()
{
}

HRESULT CModelBinary::Initialize()
{
	return S_OK;
}

CModelBinary* CModelBinary::Create()
{
	DERIVED* pInstance = new DERIVED();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("ModelBinary Create Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CModelBinary::Free()
{
}

#pragma region Assimp용
#ifdef _DEBUG
HRESULT CModelBinary::Bake_StaticModel(const wstring& strFolderPath)
{
	list<wstring> fbxFileList;
	fs::path directory(strFolderPath);
	if (fs::exists(strFolderPath) && fs::is_directory(directory))
	{
		CModelBinary::Bake_StaticModel_Recursive(strFolderPath, fbxFileList);
	}
	else
	{
		cerr << "잘못된 폴더입니다 : " << ConvertToString(strFolderPath) << endl;
	}

	// 자동 스케일 다운 및 회전(카메라 Look방향을 바라보도록)
	_matrix mat = XMMatrixAffineTransformation(XMVectorSet(0.01f, 0.01f, 0.01f, 0.f), XMQuaternionIdentity(), XMQuaternionRotationRollPitchYaw(0.f, XMConvertToRadians(0.f), 0.f), XMVectorSet(0.f, 0.f, 0.f, 1.f));
	for (auto iter = fbxFileList.begin(); iter != fbxFileList.end(); ++iter)
	{
		Bake_Model((*iter), mat);
	}

	return S_OK;
}

HRESULT CModelBinary::Bake_StaticModel_Recursive(const fs::path& directory, list<wstring>& fbxFileList)
{
	for (const auto& entry : fs::directory_iterator(directory))
	{
		if (fs::is_directory(entry))
		{
			CModelBinary::Bake_StaticModel_Recursive(entry.path(), fbxFileList);
		}
		else if (fs::is_regular_file(entry))
		{
			wstring strFileFullPath = entry.path().wstring();

			// 문자열 있으면
			if (strFileFullPath.find(TEXT(".fbx")) != wstring::npos
				|| strFileFullPath.find(TEXT(".afbx")) != wstring::npos
				|| strFileFullPath.find(TEXT(".sfbx")) != wstring::npos)
			{
				for (_tchar& ch : strFileFullPath)
				{
					if (ch == L'\\')
						ch = L'/';
				}
				fbxFileList.push_back(strFileFullPath);
			}
		}
	}

	return S_OK;
}
#ifdef _DEBUG
HRESULT CModelBinary::Bake_Model(const wstring& strFilePath, const _fmatrix PivotMatrix)
{
#pragma region 파일 상태, 경로 체크
	// afbx -> aasnim(애니메이션만), fbx -> amodel(일반 모델) 체크
	wstring strFileName = TEXT("");
	size_t iFindPos = wstring::npos;
	_bool bIsAnimFile = { false };
	_bool bIsAnimMesh = { false };
	if ((iFindPos = strFilePath.find(TEXT(".afbx"))) != wstring::npos)
	{
		strFileName = strFilePath.substr(0, iFindPos);
		strFileName += TEXT(".aanim");
		bIsAnimFile = true;
		bIsAnimMesh = true;
	}
	else if ((iFindPos = strFilePath.find(TEXT(".fbx"))) != wstring::npos)
	{
		strFileName = strFilePath.substr(0, iFindPos);
		strFileName += TEXT(".amodel");
	}
	else if ((iFindPos = strFilePath.find(TEXT(".sfbx"))) != wstring::npos)
	{
		strFileName = strFilePath.substr(0, iFindPos);
		strFileName += TEXT(".amodel");
		bIsAnimMesh = true;
	}

	// amodel 파일이 있으면 fbx를 amodel로 만들지 않는다.
	ifstream ifs(strFileName);
	if (ifs.is_open())
	{
		ifs.close();
		return S_OK;
	}
	ifs.close();
#pragma endregion

#pragma region Assimp 모델 로드
	_uint iFlag = 0;
	iFlag = aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast
#pragma region 플래그 참고
		| aiProcess_RemoveRedundantMaterials;
	//iFlag = aiProcess_JoinIdenticalVertices |   // 동일한 꼭지점 결합, 인덱싱 최적화
//	aiProcess_ValidateDataStructure |       // 로더의 출력을 검증
//	aiProcess_ImproveCacheLocality |        // 출력 정점의 캐쉬위치를 개선
//	aiProcess_ RemoveRedundantMaterials |    // 중복된 매터리얼 제거
//	aiProcess_GenUVCoords |                 // 구형, 원통형, 상자 및 평면 매핑을 적절한 UV로 변환
//	aiProcess_TransformUVCoords |           // UV 변환 처리기 (스케일링, 변환...)
//	aiProcess_FindInstances |               // 인스턴스된 매쉬를 검색하여 하나의 마스터에 대한 참조로 제거
//	aiProcess_LimitBoneWeights |            // 정점당 뼈의 가중치를 최대 4개로 제한
//	aiProcess_OptimizeMeshes |              // 가능한 경우 작은 매쉬를 조인
//	aiProcess_GenSmoothNormals |            // 부드러운 노말벡터(법선벡터) 생성
//	aiProcess_SplitLargeMeshes |			// 거대한 하나의 매쉬를 하위매쉬들로 분활(나눔)
//	aiProcess_Triangulate |                 // 3개 이상의 모서리를 가진 다각형 면을 삼각형으로 만듬(나눔)
//	aiProcess_ConvertToLeftHanded |         // D3D의 왼손좌표계로 변환
//	aiProcess_PopulateArmatureData |		// Aramature의 정보를 뼈에 기록할 수 있게 하는 플래그
//	aiProcess_SortByPType;					// 단일타입의 프리미티브로 구성된 '깨끗한' 매쉬를 만듬  
#pragma endregion

	Assimp::Importer importer;
	// Assimp에게 모델로드를 요청한다.
	string strConvert = ConvertToString(strFilePath);
	const aiScene* pScene = importer.ReadFile(strConvert, iFlag);

	if (!pScene || pScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !pScene->mRootNode)
	{
		cout << "모델 씬 로드 실패 : " << ConvertToString(strFilePath) << endl;
		importer.FreeScene();
		RETURN_EFAIL;
	}
#pragma endregion




#pragma region 베이킹 준비
	// 바이너리 객체 생성
	CModelBinary* pModel = CModelBinary::Create();
	pModel->m_bIsOnlyAnims = bIsAnimFile;
	pModel->m_MeshMatrices.resize(pScene->mNumMeshes, _float4x4());

	// Assimp에서 생성한 PreRotationMatrix를 찾아 RootNode를 돌려준다.
	// RootNode는 뼈의 Deform에 들어가지 않기 때문에 회전을 넣어두면 애니메이션에 적용된다.
	aiMatrix4x4 aiPreRotationMatrix = {};
	memcpy(&aiPreRotationMatrix, &XMMatrixTranspose(PivotMatrix), sizeof(_float4x4));
	_matrix dxPreRotationMatrix = XMMatrixIdentity();
	aiNode* pPreRotationNode = pModel->Find_PreRotationNode(pScene->mRootNode);
	if (nullptr != pPreRotationNode)
	{
		// 애니메이션이 없으면 PreRotation을 넣어준다. 
		// 이 행렬은 Assimp에서 축 교정 이유로 넣어주는 행렬이다.
		// 이 행렬이 없는 경우도 있는데. 이는 메쉬별로 들어가 있는 트랜스폼을 이용한다.
		if (!pScene->HasAnimations())
		{
			_matrix mat = XMMatrixAffineTransformation(XMVectorSet(100.f, 100.f, 100.f, 0.f), XMQuaternionIdentity(), XMQuaternionIdentity(), XMVectorSet(0.f, 0.f, 0.f, 1.f));
			_float4x4 StaticScale = XMMatrixTranspose(mat);
			aiMatrix4x4 aiTempMat = {};
			memcpy(&aiTempMat, &StaticScale, sizeof(_float4x4));
			// 열 우선, Pivot 곱하기
			aiPreRotationMatrix = aiTempMat * aiPreRotationMatrix * pPreRotationNode->mTransformation;
			// 행 우선으로 변경후 DX용 PreRotation에 넣어준다.
			_float4x4 dxTempFloat4x4 = {};
			memcpy(&dxTempFloat4x4, &aiPreRotationMatrix.Transpose(), sizeof(_float4x4));
			dxPreRotationMatrix = XMLoadFloat4x4(&dxTempFloat4x4);
		}
	}
	else
		cerr << "PreRotation 노드가 없습니다 : " << ConvertToString(strFilePath) << endl;

	pModel->Apply_PrePostRotation(pScene->mRootNode, aiPreRotationMatrix);

#pragma endregion

	//_float3 vRot = Get_RotEulerFromMatrix(dxPreRotationMatrix * PivotMatrix);
	//cout << "Pos : " << vPos.m128_f32[0] << ", " << vPos.m128_f32[1] << ", " << vPos.m128_f32[2] << endl;
	//cout << "Rot : " << vRot.x << ", " << vRot.y << ", " << vRot.z << endl;
	////cout << "Scale : " << vScale.m128_f32[0] << ", " << vScale.m128_f32[1] << ", " << vScale.m128_f32[2] << endl;
	//vRot = Get_RotEulerFromMatrix(dxPreRotationMatrix);
	//cout << "Rot : " << vRot.x << ", " << vRot.y << ", " << vRot.z << endl;
	//vRot = Get_RotEulerFromMatrix(PivotMatrix);
	//cout << "Rot : " << vRot.x << ", " << vRot.y << ", " << vRot.z << endl;

#pragma region 베이킹
	// 모델 해쉬 생성
	wstring strHash;
	pModel->Bake_BoneHash(pScene->mRootNode, strHash, true);

	// 뼈 베이킹
	if (FAILED(pModel->Bake_Bones(pScene->mRootNode, -1)))
	{
		Safe_Release(pModel);
		importer.FreeScene();
		RETURN_EFAIL;
	}
	// 애니메이션 베이킹
	if (FAILED(pModel->Bake_Animations(pScene)))
	{
		Safe_Release(pModel);
		importer.FreeScene();
		RETURN_EFAIL;
	}
	// 메쉬 베이킹, 모델에 애니메이션이 없으면 자동으로 메쉬의 정점들을 변환한다.
	// 애니메이션 분할 로드를 지원하지만 애니메이션 모델이라면 기본 애니메이션 1개 정도는 넣어주어야 합니다.
	if (!bIsAnimFile
		&& FAILED(pModel->Bake_Meshes(pScene, dxPreRotationMatrix * PivotMatrix, !pScene->HasAnimations(), bIsAnimMesh)))
	{
		Safe_Release(pModel);
		importer.FreeScene();
		RETURN_EFAIL;
	}
	// 머터리얼 베이킹
	if (!bIsAnimFile && FAILED(pModel->Bake_Materials(pScene, strConvert)))
	{
		Safe_Release(pModel);
		importer.FreeScene();
		RETURN_EFAIL;
	}
#pragma endregion




#pragma region 파일 저장 및 무결성 체크
	size_t iDotIndex = strFilePath.find_last_of(L".");
	wstring strPathWithOutExt = strFilePath.substr(0, iDotIndex);
	if (!bIsAnimFile)
		strPathWithOutExt += TEXT(".amodel");
	else
		strPathWithOutExt += TEXT(".aanim");

	_tchar		szFileName[MAX_PATH] = L"";
	_tchar		szEXT[MAX_PATH] = L"";
	_wsplitpath_s(strFileName.c_str(), nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szEXT, MAX_PATH);

	_tchar		szTmp[MAX_PATH] = L"";
	wcscpy_s(szTmp, szFileName);
	wcscpy_s(szTmp, szEXT);

	wstring strTemp = szFileName;
	strTemp += szEXT;
	//string strPrint = ConvertToString(TEXT("../Bin/Resources/Model/Bin/") + strTemp);//ConvertToString(strPathWithOutExt);
	string strPrint = ConvertToString(strPathWithOutExt);
	if (FAILED(pModel->Save_Binary(strPathWithOutExt)))
	{
		cout << "저장 실패" << strPrint << endl;
		Safe_Release(pModel);
		RETURN_EFAIL;
	}

	if (FAILED(pModel->Load_Binary(strPathWithOutExt)))
	{
		cout << "저장 실패" << endl;
	}
	cout << "저장 성공" << strPrint << " : " << pModel->m_iBoneHash << ", " << pModel->m_Bones.size() << endl;
#pragma endregion


	// 해제
	Safe_Release(pModel);
	importer.FreeScene();

	return S_OK;
}
#endif
void CModelBinary::Apply_PrePostRotation(aiNode* pAINode, const aiMatrix4x4& PreRotMatrix)
{
	pAINode->mTransformation = PreRotMatrix * pAINode->mTransformation;
}

aiNode* CModelBinary::Find_PreRotationNode(const aiNode* pAINode)
{
	string strName = pAINode->mName.C_Str();

	// 해당 노드를 찾으면 Return
	if (strName.find("$_PreRotation") != string::npos)
	{
		return ConCast<aiNode*>(pAINode);
	}

	for (_uint i = 0; i < pAINode->mNumChildren; i++)
	{
		return Find_PreRotationNode(pAINode->mChildren[i]);
	}

	return nullptr;
}

aiNode* CModelBinary::Find_MeshNode(const aiNode* pAINode, _uint iMeshIndex)
{
	// 해당 노드를 찾으면 Return
	for (_uint i = 0; i < pAINode->mNumMeshes; ++i)
	{
		if (pAINode->mMeshes[i] == iMeshIndex)
			return ConCast<aiNode*>(pAINode);
	}

	for (_uint i = 0; i < pAINode->mNumChildren; i++)
	{
		return Find_MeshNode(pAINode->mChildren[i], iMeshIndex);
	}

	return nullptr;
}

HRESULT CModelBinary::Bake_Meshes(const aiScene* AIScene
	, const _fmatrix PreRotationMatrix, _bool IsOnlyMesh, _bool IsAnimMesh)
{
	if (!AIScene->HasMeshes())
		return S_OK;

	auto pRootNode = AIScene->mRootNode;
	m_Meshes.resize(AIScene->mNumMeshes, {});
	for (_uint i = 0; i < AIScene->mNumMeshes; i++)
	{
		auto pMesh = AIScene->mMeshes[i];
		TMesh& MeshData = m_Meshes[i];

		// 메쉬 기본 정보 입력
		MeshData.iID = i;
		MeshData.strName = ConvertToWstring(pMesh->mName.C_Str());
		MeshData.iMaterialIndex = pMesh->mMaterialIndex;

		// 메쉬 트랜스폼 교정
		_float4x4 MeshMatrix = XMMatrixIdentity();
		auto pAIMeshNode = Find_MeshNode(pRootNode, MeshData.iID);
		if (nullptr != pAIMeshNode && IsOnlyMesh)
		{
			memcpy(&MeshMatrix, &pAIMeshNode->mTransformation.Transpose(), sizeof(_float4x4));
		}
		MeshMatrix *= PreRotationMatrix;

		// 정점 정보 추출
		MeshData.vecVertices.resize(pMesh->mNumVertices);
		for (_uint j = 0; j < pMesh->mNumVertices; j++)
		{
			memcpy(&MeshData.vecVertices[j].vPosition, &pMesh->mVertices[j], sizeof(_float3));
			for (_uint k = 0; k < pMesh->GetNumUVChannels(); k++)
				memcpy(&MeshData.vecVertices[j].vTexCoord[k], &pMesh->mTextureCoords[k][j], sizeof(_float) * pMesh->mNumUVComponents[k]);
			memcpy(&MeshData.vecVertices[j].vNormal, &pMesh->mNormals[j], sizeof(_float3));
			if (pMesh->HasTangentsAndBitangents())
			{
				memcpy(&MeshData.vecVertices[j].vTangent, &pMesh->mTangents[j], sizeof(_float3));
				memcpy(&MeshData.vecVertices[j].vBiTangent, &pMesh->mBitangents[j], sizeof(_float3));
			}


			// 메쉬만 로드되었다면 정점을 변환
			if (IsOnlyMesh && !IsAnimMesh)
			{
				XMStoreFloat3(&MeshData.vecVertices[j].vPosition,
					XMVector3TransformCoord(XMLoadFloat3(&MeshData.vecVertices[j].vPosition), MeshMatrix));
				XMStoreFloat3(&MeshData.vecVertices[j].vNormal,
					XMVector3TransformNormal(XMVector3Normalize(XMLoadFloat3(&MeshData.vecVertices[j].vNormal)), MeshMatrix));
				XMStoreFloat3(&MeshData.vecVertices[j].vTangent,
					XMVector3TransformNormal(XMVector3Normalize(XMLoadFloat3(&MeshData.vecVertices[j].vTangent)), MeshMatrix));
				XMStoreFloat3(&MeshData.vecVertices[j].vBiTangent,
					XMVector3TransformNormal(XMVector3Normalize(XMLoadFloat3(&MeshData.vecVertices[j].vBiTangent)), MeshMatrix));
			}

			MeshData.vecVertices[j].vecBoneID.reserve(AI_LMW_MAX_WEIGHTS);
			MeshData.vecVertices[j].vecWeights.reserve(AI_LMW_MAX_WEIGHTS);
		}

		// 인덱싱 정보 입력
		MeshData.vecIndices.resize(pMesh->mNumFaces * 3);
		_uint iNumIndices = 0;
		for (_uint j = 0; j < pMesh->mNumFaces; j++)
		{
			MeshData.vecIndices[iNumIndices++] = pMesh->mFaces[j].mIndices[0];
			MeshData.vecIndices[iNumIndices++] = pMesh->mFaces[j].mIndices[1];
			MeshData.vecIndices[iNumIndices++] = pMesh->mFaces[j].mIndices[2];
		}

		// 메쉬 오프셋 정보 입력
		MeshData.m_Bones.resize(pMesh->mNumBones);
		for (_uint j = 0; j < pMesh->mNumBones; j++)
		{
			auto pAIBone = pMesh->mBones[j];
			auto OffsetMatrix = pAIBone->mOffsetMatrix;
			_float4x4 OffsetConvert;
			memcpy(&OffsetConvert, &OffsetMatrix, sizeof(_float4x4));
			XMStoreFloat4x4(&MeshData.m_Bones[j].matOffset, XMMatrixTranspose(XMLoadFloat4x4(&OffsetConvert)));

			_uint		iBoneIndex = { 0 };
			vector<TBone>& vecBones = m_Bones;
			auto	iter = find_if(vecBones.begin(), vecBones.end(), [&](TBone& Bone)
				{
					if (ConvertToWstring(pAIBone->mName.C_Str()) == Bone.strName)
					{
						return true;
					}

					++iBoneIndex;

					return false;
				});

			if (iter == vecBones.end())
				RETURN_EFAIL;

			MeshData.m_Bones[j].iBoneID = iBoneIndex;
			for (_uint k = 0; k < pAIBone->mNumWeights; k++)
			{
				_uint iVertexID = pAIBone->mWeights[k].mVertexId;
				_float fWeight = pAIBone->mWeights[k].mWeight;

				if (fWeight != 0.f)
				{
					MeshData.vecVertices[iVertexID].vecBoneID.push_back(j);
					MeshData.vecVertices[iVertexID].vecWeights.push_back(fWeight);
				}
			}
		}

		for (_uint j = 0; j < pMesh->mNumVertices; j++)
		{
			MeshData.vecVertices[j].vecBoneID.resize(AI_LMW_MAX_WEIGHTS, 0);
			MeshData.vecVertices[j].vecWeights.resize(AI_LMW_MAX_WEIGHTS, 0.f);
		}
	}

	return S_OK;
}

HRESULT CModelBinary::Bake_Materials(const aiScene* AIScene, const string& strModelFilePath)
{
	if (!AIScene->HasMaterials())
		return S_OK;


	m_Materials.resize(AIScene->mNumMaterials);
	for (_uint i = 0; i < AIScene->mNumMaterials; i++)
	{
		aiMaterial* pAIMaterial = AIScene->mMaterials[i];
		TMaterial& MatrialData = m_Materials[i];

		MatrialData.strName = ConvertToWstring(pAIMaterial->GetName().C_Str());

		for (size_t j = 1; j < ECast(TEXTURETYPE_MAX); j++)
		{
			_char		szDrive[MAX_PATH] = "";
			_char		szDirectory[MAX_PATH] = "";

			_splitpath_s(strModelFilePath.c_str(), szDrive, MAX_PATH, szDirectory, MAX_PATH, nullptr, 0, nullptr, 0);

			aiString			strPath;
			if (FAILED(pAIMaterial->GetTexture(aiTextureType(j), 0, &strPath)))
				continue;

			_char		szFileName[MAX_PATH] = "";
			_char		szEXT[MAX_PATH] = "";

			_splitpath_s(strPath.data, nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szEXT, MAX_PATH);

			_char		szTmp[MAX_PATH] = "";
			strcpy_s(szTmp, szDrive);
			strcat_s(szTmp, szDirectory);
			strcat_s(szTmp, szFileName);
			strcat_s(szTmp, szEXT);

			_char szTexture[MAX_PATH] = "Model/Texture/";

			strcat_s(szTexture, szFileName);

			_tchar		szFullPath[MAX_PATH] = TEXT("");

			//MultiByteToWideChar(CP_ACP, 0, szTmp, strlen(szTmp), szFullPath, MAX_PATH);
			//string strFullPath = szFileName;
			string strFullPath = szTexture;
			strFullPath += szEXT;

			//MatrialData.strTextures[j] = ConvertToWstring(strFullPath);
			MatrialData.strTextures[j] = ConvertToWstring(szFileName);
		}
	}
	// 추가 머터리얼 적용
	Rebake_Materials(ConvertToWstring(strModelFilePath));

	return S_OK;
}

HRESULT CModelBinary::Bake_Bones(aiNode* pAINode, _int iParentIndex)
{
	TBone Bone = {};

	Bone.iID = Cast<_int>(m_Bones.size());                      // ID
	Bone.iParentID = iParentIndex;                              // 부모
	Bone.strName = ConvertToWstring(pAINode->mName.C_Str());    // 이름
	_float4x4 TransformFloat4x4;                                    // 행렬
	memcpy(&TransformFloat4x4, &pAINode->mTransformation, sizeof(_float4x4));
	XMStoreFloat4x4(&Bone.matTransform, XMMatrixTranspose(XMLoadFloat4x4(&TransformFloat4x4)));

	m_Bones.push_back(Bone);

	for (_uint i = 0; i < pAINode->mNumChildren; i++)
	{
		Bake_Bones(pAINode->mChildren[i], Bone.iID);
	}

	return S_OK;
}

HRESULT CModelBinary::Bake_BoneHash(aiNode* pAINode, wstring& strTotalName, _bool bIsRoot)
{
	m_iNumBones_ForHash += pAINode->mNumChildren;
	wstring strName = ConvertToWstring(pAINode->mName.C_Str());    // 이름
	// Assimp에서 정의한 노드가 아니면 해쉬계산에 넣는다. 
	if (strName.find(TEXT("$Assimp")) == wstring::npos)
	{
		// 메쉬가 있다면 _Lod 부분을 찾아 잘라 넣는다.
		if (pAINode->mNumMeshes)
		{
			size_t iCut = strName.find(TEXT("_Lod"));
			strTotalName += strName.substr(0, iCut);
		}
		else
			strTotalName += strName;
	}

	for (_uint i = 0; i < pAINode->mNumChildren; i++)
	{
		Bake_BoneHash(pAINode->mChildren[i], strTotalName, false);
	}

	if (bIsRoot)
	{
		//cout << ConvertToString(strTotalName) << endl;
		hash<wstring> strHash;
		m_iBoneHash = strHash(strTotalName.c_str());
		//cout << m_iBoneHash << ", " << strHash(strTotalName.c_str()) << endl;
	}

	return S_OK;
}

HRESULT CModelBinary::Bake_Animations(const aiScene* AIScene)
{
	if (!AIScene->HasAnimations())
		return S_OK;

	m_Anims.resize(AIScene->mNumAnimations, {});
	for (_uint i = 0; i < AIScene->mNumAnimations; i++)
	{
		auto pAnim = AIScene->mAnimations[i];
		TAnim& AnimData = m_Anims[i];

		AnimData.fDuration = pAnim->mDuration;
		AnimData.fTickPerSeconds = pAnim->mTicksPerSecond;
		AnimData.iID = i;
		AnimData.strName = ConvertToWstring(pAnim->mName.C_Str());

		AnimData.vecChannels.resize(pAnim->mNumChannels, {});
		for (_uint j = 0; j < pAnim->mNumChannels; j++)
		{
			auto pChannel = pAnim->mChannels[j];
			TChannel& ChannelData = AnimData.vecChannels[j];

			_uint		iBoneIndex = { 0 };
			vector<TBone>& vecBones = m_Bones;
			auto	iter = find_if(vecBones.begin(), vecBones.end(), [&](TBone& Bone)
				{
					if (ConvertToWstring(pChannel->mNodeName.C_Str()) == Bone.strName)
					{
						return true;
					}
					++iBoneIndex;

					return false;
				});

			if (iter == vecBones.end())
				RETURN_EFAIL;

			ChannelData.iBoneID = iBoneIndex;
			ChannelData.strName = ConvertToWstring(pChannel->mNodeName.C_Str());


			_uint iNumKeyFrames = max(max(pChannel->mNumPositionKeys, pChannel->mNumRotationKeys), pChannel->mNumScalingKeys);
			_float3 vPos = {}, vScale = {};
			_float4 vRot = {};
			_float fTrackPos = 0.f;

			ChannelData.vecKeyFrames.resize(iNumKeyFrames, {});
			for (_uint k = 0; k < iNumKeyFrames; k++)
			{
				TKeyFrame& KeyFrame = ChannelData.vecKeyFrames[k];

				if (k < pChannel->mNumScalingKeys)
				{
					memcpy(&vScale, &pChannel->mScalingKeys[k].mValue, sizeof(_float3));
					fTrackPos = pChannel->mScalingKeys[k].mTime;
				}
				if (k < pChannel->mNumRotationKeys)
				{
					vRot.x = pChannel->mRotationKeys[k].mValue.x;
					vRot.y = pChannel->mRotationKeys[k].mValue.y;
					vRot.z = pChannel->mRotationKeys[k].mValue.z;
					vRot.w = pChannel->mRotationKeys[k].mValue.w;
					fTrackPos = pChannel->mRotationKeys[k].mTime;
				}
				if (k < pChannel->mNumPositionKeys)
				{
					memcpy(&vPos, &pChannel->mPositionKeys[k].mValue, sizeof(_float3));
					fTrackPos = pChannel->mPositionKeys[k].mTime;
				}

				// Pre, Post 회전 적용
				_matrix Transform = XMMatrixAffineTransformation(XMLoadFloat3(&vScale), XMQuaternionIdentity(), XMLoadFloat4(&vRot), XMLoadFloat3(&vPos));
				//Transform = XMLoadFloat4x4(&m_PreRotationFloat4x4) * Transform;// *XMLoadFloat4x4(&m_PostRotationFloat4x4);

				_vector vSimPos = {}, vSimRot = {}, vSimScale = {};
				XMMatrixDecompose(&vSimScale, &vSimRot, &vSimPos, Transform);

				XMStoreFloat3(&KeyFrame.vPos, vSimPos);
				XMStoreFloat4(&KeyFrame.qtRot, vSimRot);
				XMStoreFloat3(&KeyFrame.vScale, vSimScale);
				KeyFrame.fTrackPosition = fTrackPos;
			}
		}
	}

	return S_OK;
}
#pragma endregion
#endif

HRESULT CModelBinary::Rebake_Materials(const wstring& strFilePath)
{
	_bool bIsChanged = false;
	for (_uint i = 0; i < m_Materials.size(); i++)
	{
		auto& Material = m_Materials[i];

		// 포함되지 않은 텍스처를 찾아내기 위해 필요한 베이스 텍스처 이름
		wstring strBaseName = L"";

		for (_uint j = 0; j < TEXTURETYPE_MAX; j++)
		{
			auto& TextureName = Material.strTextures[j];

			// 
			if (j == TextureType_DIFFUSE)
			{
				if (!TextureName.empty())
				{
					strBaseName = CUtility_File::ConvertToFileName(TextureName);
					size_t iPos = strBaseName.find(TEXT("_C"));
					if (iPos == wstring::npos)
						break;
					strBaseName = strBaseName.substr(0, iPos);
				}
				else
					break;
			}

			switch (j)
			{
			case TextureType_DIFFUSE:
			{
				break;
			}
			case TextureType_NORMALS:			// _N, 노멀, 특정 오브젝트가 엉뚱한 노멀을 가리켜 처리하게됨.
			{

				wstring strName = CUtility_File::Get_FilePath(CPath_Mgr::TEXTURE_FILE, strBaseName + TEXT("_N"));
				if (strName == TEXT("Not_Find"))
					continue;
				TextureName = CUtility_File::ConvertToFileName(strName);
				bIsChanged = true;

				break;
			}
			case TextureType_NORMAL_CAMERA:			// _NB, 노멀 블렌드, 머리카락 음영을 더 짙게줄 때 쓰이는 듯
			{
				wstring strName = CUtility_File::Get_FilePath(CPath_Mgr::TEXTURE_FILE, strBaseName + TEXT("_NB"));
				if (strName == TEXT("Not_Find"))
					continue;
				TextureName = CUtility_File::ConvertToFileName(strName);
				bIsChanged = true;
				break;
			}
			case TextureType_BASE_COLOR:			// _B, 붉은기? 캐릭터들한테 간혹 있음. 피부에 쓰이는 듯.
			{
				wstring strName = CUtility_File::Get_FilePath(CPath_Mgr::TEXTURE_FILE, strBaseName + TEXT("_B"));
				if (strName == TEXT("Not_Find"))
					continue;
				TextureName = CUtility_File::ConvertToFileName(strName);
				bIsChanged = true;
				break;
			}
			case TextureType_EMISSIVE:			// _EH, 발광 세기(캐릭터에 쓰임)
			{
				wstring strName = CUtility_File::Get_FilePath(CPath_Mgr::TEXTURE_FILE, strBaseName + TEXT("_EH"));
				if (strName == TEXT("Not_Find"))
					continue;
				TextureName = CUtility_File::ConvertToFileName(strName);
				bIsChanged = true;
				break;
			}
			case TextureType_OPACITY:			// _A, 알파
			{
				wstring strName = CUtility_File::Get_FilePath(CPath_Mgr::TEXTURE_FILE, strBaseName + TEXT("_A"));
				if (strName == TEXT("Not_Find"))
					continue;
				TextureName = CUtility_File::ConvertToFileName(strName);
				bIsChanged = true;
				break;
			}
			case TextureType_EMISSION_COLOR:			// _E, 발광 세기(캐릭터에 쓰임)
			{
				wstring strName = CUtility_File::Get_FilePath(CPath_Mgr::TEXTURE_FILE, strBaseName + TEXT("_E"));
				if (strName == TEXT("Not_Find"))
					continue;
				TextureName = CUtility_File::ConvertToFileName(strName);
				bIsChanged = true;
				break;
			}
			case TextureType_METALNESS:			// _M, 보통 메탈릭인데, 다른 용도로도 쓰이는 것 같음
			{
				wstring strName = CUtility_File::Get_FilePath(CPath_Mgr::TEXTURE_FILE, strBaseName + TEXT("_M"));
				if (strName == TEXT("Not_Find"))
					continue;
				TextureName = CUtility_File::ConvertToFileName(strName);
				bIsChanged = true;
				break;
			}
			case TextureType_DIFFUSE_ROUGHNESS:	// _MRV, 메탈릭, 러프니스, 빛 영향도
			{
				wstring strName = CUtility_File::Get_FilePath(CPath_Mgr::TEXTURE_FILE, strBaseName + TEXT("_MRV"));
				if (strName == TEXT("Not_Find"))
					continue;
				TextureName = CUtility_File::ConvertToFileName(strName);
				bIsChanged = true;
				break;
			}
			case TextureType_AMBIENT_OCCLUSION: // _O, 오클루전
			{
				wstring strName = CUtility_File::Get_FilePath(CPath_Mgr::TEXTURE_FILE, strBaseName + TEXT("_O"));
				if (strName == TEXT("Not_Find"))
					continue;
				TextureName = CUtility_File::ConvertToFileName(strName);
				bIsChanged = true;
				break;
			}
			default:

				break;
			}
		}
	}

	if (bIsChanged)
		return S_OK;
	else
		return E_FAIL;
}

HRESULT CModelBinary::Save_Binary(const wstring& strFilePath)
{
	ofstream ofs;

	ofs.open(strFilePath.c_str(), ios::binary);
	if (ofs.is_open())
	{
		// 헤더부
		// 메쉬개수, 머터리얼, 뼈, 애니메이션 개수
		_uint iNumMeshes = Cast<_uint>(m_Meshes.size());
		_uint iNumMaterials = Cast<_uint>(m_Materials.size());
		_uint iNumBones = Cast<_uint>(m_Bones.size());
		_uint iNumAnims = Cast<_uint>(m_Anims.size());

		TModelHeader Header = {};
		Header.iID = 0xFF05;
		Header.iNumMeshes = Cast<_ushort>(iNumMeshes);
		Header.iNumMaterials = Cast<_ushort>(iNumMaterials);
		Header.iNumBones = Cast<_ushort>(iNumBones);
		Header.iNumAnims = Cast<_ushort>(iNumAnims);
		Header.bIsOnlyAnims = m_bIsOnlyAnims;
		Header.iModelHash = m_iBoneHash;

		ofs.write(ReCast<const _char*>(&Header), sizeof(TModelHeader));

		// 애니메이션만 저장하기
		if (!m_bIsOnlyAnims)
		{
			// 메쉬, 머터리얼ID -> 정점, 인덱스, 뼈
			for (_uint i = 0; i < iNumMeshes; i++)
			{
				TMesh& Mesh = m_Meshes[i];

				// ID
				Mesh.iID = i;

				// 이름
				_uint iStrLength = Cast<_uint>(Mesh.strName.length());
				ofs.write(ReCast<const _char*>(&iStrLength), sizeof(_uint));
				if (iStrLength > 0)
					ofs.write(ReCast<const _char*>(Mesh.strName.data()), sizeof(_tchar) * (iStrLength + 1));

				// 머터리얼
				ofs.write(ReCast<const _char*>(&Mesh.iMaterialIndex), sizeof(_uint));

				// 정점
				_uint iNumVertices = Cast<_uint>(Mesh.vecVertices.size());
				ofs.write(ReCast<const _char*>(&iNumVertices), sizeof(_int));
				for (_uint j = 0; j < iNumVertices; j++)
				{
					TMeshVertex& Vertex = Mesh.vecVertices[j];

					ofs.write(ReCast<const _char*>(&Vertex), 4 * sizeof(_float3) + sizeof(_float3) * 8);
					//ofs.write(ReCast<const _char*>(&Vertex.vPosition), sizeof(_float3));
					//ofs.write(ReCast<const _char*>(&Vertex.vTexCoord), sizeof(_float2));
					//ofs.write(ReCast<const _char*>(&Vertex.vNormal), sizeof(_float3));
					//ofs.write(ReCast<const _char*>(&Vertex.vTangent), sizeof(_float3));
					//ofs.write(ReCast<const _char*>(&Vertex.vBiTangent), sizeof(_float3));

					_uint iNumWeights = Cast<_uint>(Vertex.vecBoneID.size());
					ofs.write(ReCast<const _char*>(&iNumWeights), sizeof(_uint));
					if (iNumWeights > 0)
					{
						ofs.write(ReCast<const _char*>(Vertex.vecBoneID.data()), sizeof(_int) * iNumWeights);
						ofs.write(ReCast<const _char*>(Vertex.vecWeights.data()), sizeof(_float) * iNumWeights);
					}
				}

				// 인덱스
				_uint iNumIndices = Cast<_uint>(Mesh.vecIndices.size());
				ofs.write(ReCast<const _char*>(&iNumIndices), sizeof(_int));
				if (iNumIndices > 0)
					ofs.write(ReCast<const _char*>(Mesh.vecIndices.data()), sizeof(_int) * iNumIndices);

				// 뼈
				_uint iNumBones = Cast<_uint>(Mesh.m_Bones.size());
				ofs.write(ReCast<const _char*>(&iNumBones), sizeof(_int));
				if (iNumBones > 0)
					ofs.write(ReCast<const _char*>(Mesh.m_Bones.data()), sizeof(TMeshBone) * iNumBones);
			}



			// 머터리얼
			for (_uint i = 0; i < iNumMaterials; i++)
			{
				TMaterial& Mat = m_Materials[i];

				// 이름
				_uint iStrLength = Cast<_uint>(Mat.strName.length());
				ofs.write(ReCast<const _char*>(&iStrLength), sizeof(_uint));
				if (iStrLength > 0)
					ofs.write(ReCast<const _char*>(Mat.strName.data()), sizeof(_tchar) * (iStrLength + 1));

				// 텍스처 개수
				_uint iNumTexures = Cast<_uint>(TEXTURETYPE_MAX);
				ofs.write(ReCast<const _char*>(&iNumTexures), sizeof(_uint));
				for (_uint j = 0; j < iNumTexures; j++)
				{
					// 텍스처 이름
					_uint iStrLength = Cast<_uint>(Mat.strTextures[j].length());
					ofs.write(ReCast<const _char*>(&iStrLength), sizeof(_uint));
					if (iStrLength > 0)
						ofs.write(ReCast<const _char*>(Mat.strTextures[j].data()), sizeof(_tchar) * (iStrLength + 1));
				}
			}



			// 뼈
			for (_uint i = 0; i < iNumBones; i++)
			{
				TBone& Bone = m_Bones[i];

				// ID
				Bone.iID = i;

				// 뼈 이름
				_uint iStrLength = Cast<_uint>(Bone.strName.length());
				ofs.write(ReCast<const _char*>(&iStrLength), sizeof(_uint));
				if (iStrLength > 0)
					ofs.write(ReCast<const _char*>(Bone.strName.data()), sizeof(_tchar) * (iStrLength + 1));

				// 부모ID, 행렬
				ofs.write(ReCast<const _char*>(&Bone.iParentID), sizeof(_int));
				ofs.write(ReCast<const _char*>(&Bone.matTransform), sizeof(_float4x4));
			}
		}



		// 애니메이션 개수
		for (_uint i = 0; i < iNumAnims; i++)
		{
			TAnim& Anim = m_Anims[i];



			// ID
			//Anim.iID = i;

			// 이름
			_uint iStrLength = Cast<_uint>(Anim.strName.length());
			ofs.write(ReCast<const _char*>(&iStrLength), sizeof(_uint));
			if (iStrLength > 0)
				ofs.write(ReCast<const _char*>(Anim.strName.data()), sizeof(_tchar) * (iStrLength + 1));

			// 시간
			ofs.write(ReCast<const _char*>(&Anim.fDuration), sizeof(_float));

			// 프레임당 시간
			ofs.write(ReCast<const _char*>(&Anim.fTickPerSeconds), sizeof(_float));

			// 채널
			_uint iNumChannels = Cast<_uint>(Anim.vecChannels.size());
			ofs.write(ReCast<const _char*>(&iNumChannels), sizeof(_uint));
			for (_uint j = 0; j < iNumChannels; j++)
			{
				TChannel& Channel = Anim.vecChannels[j];

				// ID
				ofs.write(ReCast<const _char*>(&Channel.iBoneID), sizeof(_uint));

				// 이름
				_uint iStrLength = Cast<_uint>(Channel.strName.length());
				ofs.write(ReCast<const _char*>(&iStrLength), sizeof(_uint));
				if (iStrLength > 0)
					ofs.write(ReCast<const _char*>(Channel.strName.data()), sizeof(_tchar) * (iStrLength + 1));

				// 키프레임
				_uint iNumKeyFrames = Cast<_uint>(Channel.vecKeyFrames.size());
				ofs.write(ReCast<const _char*>(&iNumKeyFrames), sizeof(_uint));
				if (iNumKeyFrames > 0)
					ofs.write(ReCast<const _char*>(Channel.vecKeyFrames.data()), sizeof(TKeyFrame) * iNumKeyFrames);
			}
		}
	}
	else
	{
		ofs.close();
		RETURN_EFAIL;
	}

	ofs.close();

	return S_OK;
}

HRESULT CModelBinary::Load_Binary(const wstring& strFilePath)
{
	ifstream ifs;
	const _uint iAssertNum = 500000;

	ifs.open(strFilePath, ios::binary);
	if (ifs.is_open())
	{
		TModelHeader Header = {};
		ifs.read(ReCast<_char*>(&Header), sizeof(Header));
		if (Header.iID != 0xFF05)
		{
			ifs.close();
			RETURN_EFAIL;
		}

		m_bIsOnlyAnims = Header.bIsOnlyAnims;
		m_iBoneHash = Header.iModelHash;

		if (!Header.bIsOnlyAnims)
		{
			// 메쉬, 머터리얼ID -> 정점, 인덱스, 뼈
			if (Header.iNumMeshes > iAssertNum)
				assert(true);
			m_Meshes.resize(Header.iNumMeshes, {});
			for (_uint i = 0; i < Header.iNumMeshes; i++)
			{
				TMesh& Mesh = m_Meshes[i];
				_uint iNumVertices;
				_uint iNumIndices;
				_uint iNumBones;

				// ID
				Mesh.iID = i;

				// 이름
				_uint iStrLength;
				ifs.read(ReCast<_char*>(&iStrLength), sizeof(_uint));
				Mesh.strName.resize(iStrLength, 0);
				if (iStrLength > 0)
				{
					if (iStrLength > MAX_PATH)
						assert(true);
					ifs.read(ReCast<_char*>(&Mesh.strName[0]), sizeof(_tchar) * (iStrLength + 1));
				}

				// 머터리얼 인덱스
				ifs.read(ReCast<_char*>(&Mesh.iMaterialIndex), sizeof(_uint));
				if (Mesh.iMaterialIndex > iAssertNum)
					assert(true);

				// 정점
				ifs.read(ReCast<_char*>(&iNumVertices), sizeof(_int));
				if (iNumVertices > iAssertNum)
					assert(true);
				Mesh.vecVertices.resize(iNumVertices, {});
				for (_uint j = 0; j < iNumVertices; j++)
				{
					TMeshVertex& Vertex = Mesh.vecVertices[j];
					_uint iNumWeights = Cast<_uint>(Vertex.vecBoneID.size());

					// 정점 정보
					ifs.read(ReCast<_char*>(&Vertex), 4 * sizeof(_float3) + sizeof(_float3) * 8);
					//ifs.read(ReCast<_char*>(&Vertex.vPosition), sizeof(_float3));
					//ifs.read(ReCast<_char*>(&Vertex.vTexCoord), sizeof(_float2));
					//ifs.read(ReCast<_char*>(&Vertex.vNormal), sizeof(_float3));
					//ifs.read(ReCast<_char*>(&Vertex.vTangent), sizeof(_float3));
					//ifs.read(ReCast<_char*>(&Vertex.vBiTangent), sizeof(_float3));

					// 뼈ID, WeightID 개수
					ifs.read(ReCast<_char*>(&iNumWeights), sizeof(_uint));
					if (iNumWeights > iAssertNum)
						assert(true);
					Vertex.vecBoneID.resize(iNumWeights, {});
					Vertex.vecWeights.resize(iNumWeights, {});
					if (iNumWeights > 0)
					{
						ifs.read(ReCast<_char*>(&Vertex.vecBoneID[0]), sizeof(_int) * iNumWeights);
						ifs.read(ReCast<_char*>(&Vertex.vecWeights[0]), sizeof(_float) * iNumWeights);
					}
				}

				// 인덱스
				ifs.read(ReCast<_char*>(&iNumIndices), sizeof(_int));
				if (iNumIndices > iAssertNum)
					assert(true);
				Mesh.vecIndices.resize(iNumIndices, {});
				if (iNumIndices > 0)
					ifs.read(ReCast<_char*>(&Mesh.vecIndices[0]), sizeof(_int) * iNumIndices);

				// 뼈
				ifs.read(ReCast<_char*>(&iNumBones), sizeof(_int));
				if (iNumBones > iAssertNum)
					assert(true);
				Mesh.m_Bones.resize(iNumBones, {});
				if (iNumBones > 0)
					ifs.read(ReCast<_char*>(&Mesh.m_Bones[0]), sizeof(TMeshBone) * iNumBones);
			}



			// 머터리얼
			if (Header.iNumMaterials > iAssertNum)
				assert(true);
			m_Materials.resize(Header.iNumMaterials, {});
			for (_uint i = 0; i < Header.iNumMaterials; i++)
			{
				TMaterial& Mat = m_Materials[i];
				_uint iNumTexures;

				// 이름
				_uint iStrLength;
				ifs.read(ReCast<_char*>(&iStrLength), sizeof(_uint));
				Mat.strName.resize(iStrLength, 0);
				if (iStrLength > 0)
				{
					if (iStrLength > MAX_PATH)
						assert(true);
					ifs.read(ReCast<_char*>(&Mat.strName[0]), sizeof(_tchar) * (iStrLength + 1));
				}

				// 텍스처 개수
				ifs.read(ReCast<_char*>(&iNumTexures), sizeof(_uint));
				for (_uint j = 0; j < iNumTexures; j++)
				{
					// 텍스처 이름
					_uint iStrLength;
					ifs.read(ReCast<_char*>(&iStrLength), sizeof(_uint));
					Mat.strTextures[j].resize(iStrLength, 0);
					if (iStrLength > 0)
					{
						if (iStrLength > MAX_PATH)
							assert(true);
						ifs.read(ReCast<_char*>(&Mat.strTextures[j][0]), sizeof(_tchar) * (iStrLength + 1));
					}
				}
			}



			// 뼈
			if (Header.iNumBones > iAssertNum)
				assert(true);
			m_Bones.resize(Header.iNumBones, {});
			for (_uint i = 0; i < Header.iNumBones; i++)
			{
				TBone& Bone = m_Bones[i];

				// ID
				Bone.iID = i;

				// 이름
				_uint iStrLength;
				ifs.read(ReCast<_char*>(&iStrLength), sizeof(_uint));
				Bone.strName.resize(iStrLength, 0);
				if (iStrLength > 0)
				{
					if (iStrLength > MAX_PATH)
						assert(true);
					ifs.read(ReCast<_char*>(&Bone.strName[0]), sizeof(_tchar) * (iStrLength + 1));
				}

				// 부모 ID
				ifs.read(ReCast<_char*>(&Bone.iParentID), sizeof(_int));

				// 트랜스폼
				ifs.read(ReCast<_char*>(&Bone.matTransform), sizeof(_float4x4));
			}
		}

		// 애니메이션 개수
		if (Header.iNumAnims > iAssertNum)
			assert(true);
		m_Anims.resize(Header.iNumAnims, TAnim());
		for (_uint i = 0; i < Header.iNumAnims; i++)
		{
			TAnim& Anim = m_Anims[i];
			_uint iNumChannels;

			// ID
			Anim.iID = i;

			// 애님 이름
			_uint iStrLength;
			ifs.read(ReCast<_char*>(&iStrLength), sizeof(_uint));
			Anim.strName.resize(iStrLength, 0);
			if (iStrLength > 0)
			{
				if (iStrLength > MAX_PATH)
					assert(true);
				ifs.read(ReCast<_char*>(&Anim.strName[0]), sizeof(_tchar) * (iStrLength + 1));
			}

			// 지속시간
			ifs.read(ReCast<_char*>(&Anim.fDuration), sizeof(_float));

			// 초당 프레임
			ifs.read(ReCast<_char*>(&Anim.fTickPerSeconds), sizeof(_float));

			// 채널 개수
			ifs.read(ReCast<_char*>(&iNumChannels), sizeof(_uint));
			Anim.vecChannels.resize(iNumChannels, {});
			for (_uint j = 0; j < iNumChannels; j++)
			{
				TChannel& Channel = Anim.vecChannels[j];
				_uint iNumKeyFrames;

				// ID
				ifs.read(ReCast<_char*>(&Channel.iBoneID), sizeof(_uint));

				// 채널 이름
				_uint iStrLength;
				ifs.read(ReCast<_char*>(&iStrLength), sizeof(_uint));
				Channel.strName.resize(iStrLength, 0);
				if (iStrLength > 0)
				{
					if (iStrLength > MAX_PATH)
						assert(true);
					ifs.read(ReCast<_char*>(&Channel.strName[0]), sizeof(_tchar) * (iStrLength + 1));
				}

				// 키프레임
				ifs.read(ReCast<_char*>(&iNumKeyFrames), sizeof(_uint));
				Channel.vecKeyFrames.resize(iNumKeyFrames, {});
				if (iNumKeyFrames > 0)
				{
					if (iNumKeyFrames > iAssertNum)
						assert(true);
					ifs.read(ReCast<_char*>(&Channel.vecKeyFrames[0]), sizeof(TKeyFrame) * iNumKeyFrames);
				}
			}
		}
	}

	ifs.close();

	return S_OK;
}

HRESULT CModelBinary::Save_GroupBinary(const wstring& strFilePath, vector<CModelBinary*> vecModels)
{
	if (vecModels.empty())
		RETURN_EFAIL;


	ofstream ofs;

	CModelBinary* pModel = CModelBinary::Create();
	hash<wstring> strHash;
	pModel->m_iBoneHash = strHash(strFilePath);


	vector<TMaterial> AllMtrl;
	for (_int i = 0; i < vecModels.size(); i++)
	{
		for (_int j = 0; j < vecModels[i]->m_Materials.size(); j++)
		{
			// 이름 중복 끄기
			auto iter = find_if(AllMtrl.begin(), AllMtrl.end(), [&](const TMaterial& Mtrl)->bool
				{
					return Mtrl.strName == vecModels[i]->m_Materials[j].strName;
				}
			);
			if (iter == AllMtrl.end())
				AllMtrl.push_back(vecModels[i]->m_Materials[j]);
		}
	}

	for (_int i = 0; i < vecModels.size(); i++)
	{
		// 메시 반복
		for (_int j = 0; j < vecModels[i]->m_Meshes.size(); j++)
		{
			auto& Mesh = vecModels[i]->m_Meshes[j];
			for (_uint k = 0; k < Mesh.vecVertices.size(); k++)
			{
				XMStoreFloat3(&Mesh.vecVertices[k].vPosition,
					XMVector3TransformCoord(XMLoadFloat3(&Mesh.vecVertices[k].vPosition), vecModels[i]->m_GroupMatrix));
			}

			pModel->m_Meshes.push_back(Mesh);
			// 머터리얼 반복
			for (_int k = 0; k < AllMtrl.size(); k++)
			{
				if (AllMtrl[k].strName == vecModels[i]->m_Materials[Mesh.iMaterialIndex].strName)
				{
					pModel->m_Meshes.back().iMaterialIndex = k;
				}
			}
		}
	}
	// 머터리얼 저장
	for (_int i = 0; i < AllMtrl.size(); i++)
	{
		pModel->m_Materials.push_back(AllMtrl[i]);
	}

	pModel->Save_Binary(strFilePath);

	Safe_Release(pModel);

	return S_OK;
}
