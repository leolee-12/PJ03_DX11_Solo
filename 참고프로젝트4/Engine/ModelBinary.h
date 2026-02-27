#pragma once

namespace ModelBinary {}
using namespace ModelBinary;
#include "Base.h"

#include <filesystem>

namespace fs = filesystem;

BEGIN(Engine)


//----------------헤더


struct TModelHeader
{
	_ushort		iID;			// 식별자 헤더
	_ushort		iNumMeshes;
	_ushort		iNumMaterials;
	_ushort		iNumBones;
	_ushort		iNumAnims;
	_bool		bIsOnlyAnims;	// 애니메이션 데이터만 쓸 것인지
	_uint_64	iModelHash;		// 모델과 애니메이션 연결을 위한 해시값으로 쓰인다.
};



//---------------메쉬

struct TMeshVertex
{
	_float3			vPosition;
	_float3			vTexCoord[8];
	_float3			vNormal;
	_float3			vTangent;
	_float3			vBiTangent;
	vector<_int>	vecBoneID;
	vector<_float>	vecWeights;
};

struct TMeshBone
{
	_int		iBoneID;	// 해당 뼈 ID
	_float4x4	matOffset;	// 뼈에 대한 오프셋
};

struct TMesh
{
	_uint				iID;
	wstring				strName;
	vector<TMeshVertex>	vecVertices;
	vector<_uint>		vecIndices;
	vector<TMeshBone>	m_Bones;
	_uint				iMaterialIndex;
};



//-----------------애님



struct TKeyFrame
{
	_float fTrackPosition;
	_float3 vPos;
	_float4 qtRot;
	_float3 vScale;
};

struct TChannel
{
	_uint				iBoneID;
	wstring				strName;
	vector<TKeyFrame>	vecKeyFrames;
};

struct TAnim
{
	_uint				iID;
	wstring				strName;
	_float				fDuration;
	_float				fTickPerSeconds;
	vector<TChannel>	vecChannels;
};



//-----------------본



struct TBone
{
	_uint			iID;
	wstring			strName;

	_int			iParentID;
	_float4x4		matTransform;
};


//-----------------머터리얼


struct TMaterial
{
	wstring strName;
	wstring strTextures[TEXTURETYPE_MAX];
};


/// <summary>
/// 컨버팅용 모델 데이터 저장 클래스
/// </summary>
class ENGINE_DLL CModelBinary : public CBase
{
	INFO_CLASS(CModelBinary, CBase)

protected:
	explicit CModelBinary();
	explicit CModelBinary(const CModelBinary& rhs) = default;
	virtual ~CModelBinary() = default;

public:
	virtual HRESULT	Initialize();

public:
	static CModelBinary* Create();

private:
	virtual void	Free() override;

	
public:
	// 폴더 탐색 fbx 베이킹, 애니메이션 없는 버전
	static HRESULT Bake_StaticModel(const wstring& strFolderPath);
private:
	static HRESULT Bake_StaticModel_Recursive(const fs::path& directory, list<wstring>& fbxFileList);

private:


#ifdef _DEBUG
public:
	// 일반 단일 모델 베이킹
	static HRESULT Bake_Model(const wstring& strFilePath, const _fmatrix PivotMatrix);
private:
	void Apply_PrePostRotation(aiNode* pAINode, const aiMatrix4x4& PreRotMatrix);
	// Assimp가 만들어 주는 PreRotation을 찾아주는 함수.
	aiNode* Find_PreRotationNode(const aiNode* pAINode);
	// 메쉬 인덱스로 메쉬에 대한 노드를 찾아주는 함수.
	aiNode* Find_MeshNode(const aiNode* pAINode, _uint iMeshIndex);
	HRESULT Bake_Meshes(const aiScene* AIScene, const _fmatrix PreRotationMatrix
	, _bool bIsOnlyMesh, _bool IsAnimMesh);
	HRESULT Bake_Materials(const aiScene* AIScene, const string& strModelFilePath); //Material == Texture
	HRESULT Bake_Bones(aiNode* pAINode, _int iParentIndex);
	HRESULT Bake_BoneHash(aiNode* pAINode, wstring& strTotalName, _bool bIsRoot);
	HRESULT Bake_Animations(const aiScene* AIScene);
#endif

public:
	HRESULT Rebake_Materials(const wstring& strFilePath);

public:
	HRESULT Save_Binary(const wstring& strFilePath);
	HRESULT Load_Binary(const wstring& strFilePath);

	HRESULT Save_GroupBinary(const wstring& strFilePath, vector<CModelBinary*> pModels);

public:
	_bool				m_bIsOnlyAnims = { false };
	_uint_64			m_iBoneHash = { 0 };		// 뼈의 구조를 해싱하여 모델에서 애니메이션을 찾는 용도로 쓰임
	_uint				m_iNumBones_ForHash = { 0 };
	vector<TMesh>		m_Meshes;
	vector<TMaterial>	m_Materials;
	vector<TBone>		m_Bones;
	vector<TAnim>		m_Anims;
	vector<_float4x4>	m_MeshMatrices;
	_float4x4			m_GroupMatrix;

};

END