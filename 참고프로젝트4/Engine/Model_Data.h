#pragma once
#include "Base.h"
#include "Bone_Data.h"
#include "Mesh_Data.h"
#include "Material_Data.h"
#include "Animation_Data.h"

BEGIN(Engine)

struct ENGINE_DLL Model_Data
{
	_float4x4				PivotMatrix;
	MODELTYPE				eModelType = { MODELTYPE::MODELTYPE_END };

	_uint					iNumMeshes = { 0 };
	_uint					iNumMaterials = { 0 };
	_uint					iNumAnimations = { 0 };
	_uint					iNumBones = { 0 };

	string						szModelFilePath = "";
	string						szModelFileName = "";

	vector<MESH_DATA>		Meshes;
	vector<MATERIAL_DATA>	Materials;
	vector<ANIMATION_DATA>	Animations;
	vector<BONE_DATA>		Bones;
	

#ifdef _DEBUG
	HRESULT Bake_Meshes(const aiScene* AIScene, _fmatrix matPivot);
	HRESULT Bake_Materials(const aiScene* AIScene, const string& strModelFilePath); //Material == Texture
	HRESULT Bake_Bones(aiNode* pAINode, _int iParentIndex);
	HRESULT Bake_Animations(const aiScene* AIScene);
#endif

	HRESULT Bake_Binary(ofstream& fout);
	HRESULT	Load_Binary(ifstream& fin);

}typedef MODEL_DATA;

END