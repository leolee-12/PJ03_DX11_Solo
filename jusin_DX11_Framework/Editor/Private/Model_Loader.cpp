#include "Model_Loader.h"
//using namespace Assimp;

HRESULT XM_CALLCONV CModel_Loader::Export_Binary(const _char* pFbxPath, const _char* pOutputPath, MODEL eType, _fmatrix PreTransform)
{
	return S_OK;
}

HRESULT XM_CALLCONV CModel_Loader::Export_JSON(const _char* pFbxPath, const _char* pOutputPath, MODEL eType, _fmatrix PreTransform, _uint iVertexSampleCount)
{
	return S_OK;
}

HRESULT XM_CALLCONV CModel_Loader::Export_All(const _char* pFbxPath, const _char* pOutputDir, MODEL eType, _fmatrix PreTransform)
{
	return S_OK;
}

HRESULT XM_CALLCONV CModel_Loader::Load_FBX(const _char* pFbxPath, MODEL eType, _fmatrix PreTransform)
{
	return S_OK;
}

HRESULT CModel_Loader::Extract_Bones(aiNode* pNode, _int iParentIndex)
{
	return S_OK;
}

HRESULT CModel_Loader::Extract_Meshes()
{
	return S_OK;
}

HRESULT CModel_Loader::Extract_Materials()
{
	return S_OK;
}

_int CModel_Loader::Find_BoneIndex(const _char* pBoneName) const
{
	return _int();
}

TEXTURE_TYPE CModel_Loader::Convert_TextureType(aiTextureType eAIType)
{
	return TEXTURE_TYPE();
}

HRESULT CModel_Loader::Write_Binary(const _char* pOutputPath) const
{
	return S_OK;
}

HRESULT CModel_Loader::Write_JSON(const _char* pOutputPath, _uint iVertexSampleCount) const
{
	return S_OK;
}