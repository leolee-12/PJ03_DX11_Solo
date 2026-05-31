#pragma once
#include "Base.h"
#include "Editor_Defines.h"
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

NS_BEGIN(Editor)

class CModel_Loader final : public CBase
{
private:
	CModel_Loader() = default;
	virtual ~CModel_Loader() = default;

public:
	HRESULT XM_CALLCONV Export_Binary(const _char* pFbxPath, const _char* pOutputPath,
		MODEL eType, _fmatrix PreTransform, const _char* pMappingJsonPath = nullptr);

	HRESULT XM_CALLCONV Export_JSON(const _char* pFbxPath, const _char* pOutputPath,
		MODEL eType, _fmatrix PreTransform, _uint iVertexSampleCount = 3);

	HRESULT XM_CALLCONV Export_All(const _char* pFbxPath, const _char* pOutputDir,
		MODEL eType, _fmatrix PreTransform, const _char* pMappingJsonPath = nullptr);

	HRESULT XM_CALLCONV Load_FBX(const _char* pFbxPath, MODEL eType, _fmatrix PreTransform);

	HRESULT Generate_MappingJSON(const _char* pTexDir, const _char* pOutputPath);
	HRESULT Apply_MappingJSON(const _char* pMappingJsonPath);

	_bool Is_ModelLoaded() const { return m_pAIScene != nullptr; }
	const _char* Get_FbxPath() const { return m_strFbxPath.c_str(); }
	const WMODEL_HEADER& Get_ModelMetaData() const { return m_tHeader; }
	const vector<WMODEL_BONE>& Get_ModelBones() const { return m_Bones; }

private:
	const aiScene* m_pAIScene = { nullptr };
	Assimp::Importer m_Importer = {};
	
	MODEL m_eType = { MODEL::END };
	_float4x4 m_PreTransformMatrix = {};
	_string m_strFbxPath = {};
	
	vector<WMODEL_BONE> m_Bones;
	vector<WMODEL_MATERIAL> m_Materials;
	vector<WMODEL_MESH> m_Meshes;
	vector<WMODEL_ANIMATION> m_Animations;

	WMODEL_HEADER m_tHeader = {};

private:
	HRESULT Initialize();
	
	// 추출 기능
	HRESULT Extract_Bones(aiNode* pNode, _int iParentIndex);
	HRESULT Extract_Meshes();
	HRESULT Extract_Materials();
	HRESULT Extract_Animations();
	_int    Find_BoneIndex(const _char* pBoneName) const;
	HRESULT Write_Binary(const _char* pOutputPath) const;
	HRESULT Write_JSON(const _char* pOutputPath, _uint iVertexSampleCount) const;
	void Clear_Data();

public:
	static CModel_Loader* Create();

private:
	virtual void Free() override;
};

NS_END