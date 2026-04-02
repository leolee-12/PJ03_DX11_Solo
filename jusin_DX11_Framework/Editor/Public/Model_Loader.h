#pragma once
#include "Editor_Defines.h"
//#include <assimp/scene.h>
//#include <assimp/Importer.hpp>
//#include <assimp/postprocess.h>

NS_BEGIN(Editor)

class CModel_Loader
{
private:
	struct MESH_DATA
	{
		_char szName[MAX_PATH];
		_uint iMaterialIndex;
		vector<VTXMESH> nonAnimVertices;	// NONANIM 전용
		vector<VTXANIMMESH> AnimVertices;	// ANIM 전용
		vector<_uint> indices;
		vector<_uint> boneIndices;			// ANIM 전용
		vector<_float4x4> offsetMatrices;	// ANIM 전용
	};

	struct MATERIAL_DATA
	{
		vector<_string> TexturePaths[ETOUI(TEXTURE_TYPE::END)];
	};

public:
	// 바이너리 Export
	static HRESULT XM_CALLCONV Export_Binary(const _char* pFbxPath, const _char* pOutputPath, MODEL eType, _fmatrix PreTransform);

	// JSON Export
	static HRESULT XM_CALLCONV Export_JSON(const _char* pFbxPath, const _char* pOutputPath, MODEL eType, _fmatrix PreTransform, _uint iVertexSampleCount = 3);

	// 바이너리 + JSON 동시
	static HRESULT XM_CALLCONV Export_All(const _char* pFbxPath, const _char* pOutputDir, MODEL eType, _fmatrix PreTransform);

private:
	const aiScene* m_pScene = { nullptr };
	Importer m_Importer = {};
	
	MODEL m_eType = { MODEL::END };
	_float4x4 m_PreTransformMatrix = {};
	_string m_strFbxPath = {};
	
	vector<WMODEL_BONE> m_Bones;
	vector<MATERIAL_DATA> m_Materials;
	vector<MESH_DATA> m_Meshes;

private:
	HRESULT XM_CALLCONV Load_FBX(const _char* pFbxPath, MODEL eType, _fmatrix PreTransform);
	HRESULT Extract_Bones(aiNode* pNode, _int iParentIndex);
	HRESULT Extract_Meshes();
	HRESULT Extract_Materials();
	_int    Find_BoneIndex(const _char* pBoneName) const;
	static TEXTURE_TYPE Convert_TextureType(aiTextureType eAIType);

	HRESULT Write_Binary(const _char* pOutputPath) const;
	HRESULT Write_JSON(const _char* pOutputPath, _uint iVertexSampleCount) const;

};

NS_END