#pragma once
#include "Base.h"
#include "Editor_Defines.h"
//#include <assimp/scene.h>
//#include <assimp/Importer.hpp>
//#include <assimp/postprocess.h>

NS_BEGIN(Editor)

class CModel_Loader final : public CBase
{
private:
	struct MESH_DATA
	{
		_char szName[MAX_PATH];
		_uint iMaterialIndex;
		vector<VTXMESH> nonAnimVertices;		// NONANIM 전용
		vector<VTXANIMMESH> animVertices;		// ANIM 전용
		vector<_uint> indices;
		vector<_uint> boneIndices;				// ANIM 전용
		vector<_float4x4> boneOffsetMatrices;	// ANIM 전용
	};

	struct MATERIAL_DATA
	{
		vector<_string> TexturePaths[ETOUI(TEXTURE_TYPE::END)];
	};

	struct CHANNEL_DATA
	{
		_uint iBoneIndex;
		vector<KEYFRAME> keyFrames;
	};

	struct ANIMATION_DATA
	{
		_float fDuration;
		_float fTicksPerSecond;
		vector<CHANNEL_DATA> channels;
	};

private:
	CModel_Loader() = default;
	virtual ~CModel_Loader() = default;

public:
	// 바이너리 Export
	HRESULT XM_CALLCONV Export_Binary(const _char* pFbxPath, const _char* pOutputPath, MODEL eType, _fmatrix PreTransform);

	// JSON Export
	HRESULT XM_CALLCONV Export_JSON(const _char* pFbxPath, const _char* pOutputPath, MODEL eType, _fmatrix PreTransform, _uint iVertexSampleCount = 3);

	// 바이너리 + JSON 동시
	HRESULT XM_CALLCONV Export_All(const _char* pFbxPath, const _char* pOutputDir, MODEL eType, _fmatrix PreTransform);

	// FBX 로드
	HRESULT XM_CALLCONV Load_FBX(const _char* pFbxPath, MODEL eType, _fmatrix PreTransform);

private:
	const aiScene* m_pAIScene = { nullptr };
	Importer m_Importer = {};
	
	MODEL m_eType = { MODEL::END };
	_float4x4 m_PreTransformMatrix = {};
	_string m_strFbxPath = {};
	
	vector<WMODEL_BONE> m_Bones;
	vector<MATERIAL_DATA> m_Materials;
	vector<MESH_DATA> m_Meshes;
	vector<ANIMATION_DATA> m_Animations;

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

	// 로드 파일 조회
	_bool Is_Loaded() const { return m_pAIScene != nullptr; }
	size_t Get_NumBones() const { return m_Bones.size(); }
	size_t Get_NumMeshes() const { return m_Meshes.size(); }
	size_t Get_NumMaterials() const { return m_Materials.size(); }
	size_t Get_NumAnimations() const { return m_Animations.size(); }
	MODEL Get_ModelType() const { return m_eType; }
	const _char* Get_FbxPath() const { return m_strFbxPath.c_str(); }
	const vector<WMODEL_BONE>& Get_Bones() const { return m_Bones; }

public:
	static CModel_Loader* Create();

private:
	virtual void Free() override;
};

NS_END