#include "Model_Data.h"

#ifdef _DEBUG
HRESULT Model_Data::Bake_Meshes(const aiScene* pAIScene,_fmatrix matPivot)
{
	iNumMeshes = pAIScene->mNumMeshes;
	XMStoreFloat4x4(&PivotMatrix, matPivot);

	Meshes.reserve(iNumMeshes);

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		MESH_DATA MeshData = {};

		/*  */
		MeshData.Bake_MeshData(pAIScene->mMeshes[i], eModelType, matPivot, Bones);

		Meshes.push_back(MeshData);
	}

	return S_OK;
}

HRESULT Model_Data::Bake_Materials(const aiScene* pAIScene, const string& strModelFilePath)
{
	iNumMaterials = pAIScene->mNumMaterials;
	szModelFilePath = strModelFilePath;

	for (_uint i = 0; i < iNumMaterials; i++)
	{
		aiMaterial* pAIMaterial = pAIScene->mMaterials[i];
		MATERIAL_DATA   MaterialData = {};

		for (int j = 1; j < (int)AI_TEXTURE_TYPE_MAX; j++)
		{
			_char		szDrive[MAX_PATH] = "";
			_char		szDirectory[MAX_PATH] = "";

			_splitpath_s(strModelFilePath.c_str(), szDrive, MAX_PATH, szDirectory, MAX_PATH, nullptr, 0, nullptr, 0);

			aiString			strPath;
			if (FAILED(pAIMaterial->GetTexture(aiTextureType(j), 0, &strPath)))
				continue;

			_char		szFileName[MAX_PATH] = "";
			_char		szEXT[MAX_PATH] = "";

			_splitpath_s(strPath.data, nullptr, 0, nullptr, 0, szFileName, MAX_PATH, nullptr, 0);

			//dds»ç¿ë
			strcpy_s(szEXT, ".dds");

			_char		szFullPath[MAX_PATH] = "";
			strcpy_s(szFullPath, szDrive);
			strcat_s(szFullPath, szDirectory);
			strcat_s(szFullPath, szFileName);
			strcat_s(szFullPath, szEXT);

			MaterialData.szTextureFullPath[j] = string(szFullPath);
		}

		Materials.push_back(MaterialData);
	}

	return S_OK;
}

HRESULT Model_Data::Bake_Bones(aiNode* pAINode, _int iParentIndex)
{
	BONE_DATA BoneData;

	/* */
	BoneData.Bake_BoneData(pAINode, iParentIndex);

	Bones.push_back(BoneData);

	_int		iParentIdx = (_int)Bones.size() - 1;

	for (size_t i = 0; i < pAINode->mNumChildren; i++)
	{
		Bake_Bones(pAINode->mChildren[i], iParentIdx);
	}

	iNumBones = (_uint)Bones.size();

	return S_OK;
}

HRESULT Model_Data::Bake_Animations(const aiScene* pAIScene)
{
	iNumAnimations = pAIScene->mNumAnimations;

	for (_uint i = 0; i < iNumAnimations; i++)
	{
		ANIMATION_DATA AnimationData;

		/* */
		AnimationData.Bake_AnimationData(pAIScene->mAnimations[i], Bones);

		Animations.push_back(AnimationData);
	}
	return S_OK;
}
#endif

HRESULT Model_Data::Bake_Binary(ofstream& fout)
{
	fout.write(reinterpret_cast<char*>(&PivotMatrix), sizeof(PivotMatrix));
	fout.write(reinterpret_cast<char*>(&eModelType), sizeof(eModelType));

	fout.write(reinterpret_cast<char*>(&iNumMeshes), sizeof(iNumMeshes));
	fout.write(reinterpret_cast<char*>(&iNumMaterials), sizeof(iNumMaterials));
	fout.write(reinterpret_cast<char*>(&iNumAnimations), sizeof(iNumAnimations));
	fout.write(reinterpret_cast<char*>(&iNumBones), sizeof(iNumBones));

	size_t strSize = szModelFilePath.size();
	fout.write(reinterpret_cast<char*>(&strSize), sizeof(strSize));
	fout.write(reinterpret_cast<char*>(&szModelFilePath[0]), (strSize));
	
	strSize = szModelFileName.size();
	fout.write(reinterpret_cast<char*>(&strSize), sizeof(strSize));
	fout.write(reinterpret_cast<char*>(&szModelFileName[0]), (strSize));

	for (auto iter : Meshes)
		iter.Bake_Binary(fout);

	for (auto iter : Materials)
		iter.Bake_Binary(fout);

	for (auto iter : Animations)
		iter.Bake_Binary(fout);

	for (auto iter : Bones)
		iter.Bake_Binary(fout);
		
	return S_OK;
}

HRESULT Model_Data::Load_Binary(ifstream& fin)
{
	fin.read(reinterpret_cast<char*>(&PivotMatrix), sizeof(PivotMatrix));
	fin.read(reinterpret_cast<char*>(&eModelType), sizeof(eModelType));

	fin.read(reinterpret_cast<char*>(&iNumMeshes), sizeof(iNumMeshes));
	fin.read(reinterpret_cast<char*>(&iNumMaterials), sizeof(iNumMaterials));
	fin.read(reinterpret_cast<char*>(&iNumAnimations), sizeof(iNumAnimations));
	fin.read(reinterpret_cast<char*>(&iNumBones), sizeof(iNumBones));

	size_t strSize = {};
	fin.read(reinterpret_cast<char*>(&strSize), sizeof(strSize));
	szModelFilePath.resize(strSize);
	fin.read(reinterpret_cast<char*>(&szModelFilePath[0]), strSize);

	strSize = {};
	fin.read(reinterpret_cast<char*>(&strSize), sizeof(strSize));
	szModelFileName.resize(strSize);
	fin.read(reinterpret_cast<char*>(&szModelFileName[0]), strSize);

	for (_uint i = 0; i < iNumMeshes; i++)
	{
		MESH_DATA Mesh_Data;
		Mesh_Data.Load_Binary(fin);
		Meshes.push_back(Mesh_Data);
	}

	for (_uint i = 0; i < iNumMaterials; i++)
	{
		MATERIAL_DATA Material_Data;
		Material_Data.Load_Binary(fin);
		Materials.push_back(Material_Data);
	}

	for (_uint i = 0; i < iNumAnimations; i++)
	{
		ANIMATION_DATA Animation_Data;
		Animation_Data.Load_Binary(fin);
		Animations.push_back(Animation_Data);
	}

	for (_uint i = 0; i < iNumBones; i++)
	{
		BONE_DATA Bone_Data;
		Bone_Data.Load_Binary(fin);
		Bones.push_back(Bone_Data);
	}


	return S_OK;
}
