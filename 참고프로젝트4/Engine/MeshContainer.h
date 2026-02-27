#pragma once

#include "Base.h"


BEGIN(Engine)

struct TMesh;

struct FMeshVertexData
{
	_float3			vPosition;
	_float3			vTexCoord[8];
	_float3			vNormal;
	_float3			vTangent;
	_float3			vBiTangent;
	vector<_int>	vecBoneID;
	vector<_float>	vecWeights;
};

struct FMeshBoneData
{
	_int		iBoneID;
	_float4x4	matOffset;
};

/// <summary>
/// 메쉬의 정보
/// 
/// </summary>
class ENGINE_DLL FMeshData final : public CBase
{
	DERIVED_CLASS(CBase, FMeshData)

public:
	enum TYPE { TYPE_MODEL, TYPE_ANIM_MODEL };

private:
	explicit FMeshData() = default;
	explicit FMeshData(const FMeshData& rhs) = delete;
	virtual ~FMeshData() = default;

public:
	HRESULT Initialize(const TMesh& MeshData, TYPE eTypeModel);
	HRESULT Create_ModelBuffer(const TMesh& MeshData);
	HRESULT Create_AnimModelBuffer(const TMesh& MeshData);

public:
	static FMeshData*	Create(const TMesh& MeshData, TYPE eTypeModel);
	virtual void		Free() override;

public:
	// 뼈에 대해 업데이트
	const vector<FMeshBoneData>& Provide_Offsets() const { return vecMeshBoneDatas; }
	
public:
	_uint						iID;					// 메쉬의 ID

	_uint						iNumVertices = { 0 };	// 버텍스 개수
	_uint						iVertexStride = { 0 };	// 버텍스당 바이트
	ComPtr<ID3D11Buffer>		pVB = { nullptr };		// 버텍스 버퍼
	vector<_float3>				vecVertices;			// 피킹용 버텍스
	vector<_int4>				vecBoneIDs;				// 정점별 가중치 적용 뼈
	vector<_float4>				vecWeights;				// 정점별 가중치

	_uint						iNumIndices;			// 인덱스 개수
	_uint						iIndexStride = { 0 };	// 인덱스당 바이트
	ComPtr<ID3D11Buffer>		pIB = { nullptr };		// 인덱스 버퍼
	vector<_uint>				vecIndices;				// 피킹용 .인덱스

	DXGI_FORMAT					eIndexFormat = {};		// 인덱스 포맷
	D3D11_PRIMITIVE_TOPOLOGY	eTopology = { };		// 토폴로지 옵션

	_uint						iMaterialIndex;			// 머터리얼 인덱스

	BoundingBox					AABB;

	vector<FMeshBoneData>		vecMeshBoneDatas;		// 뼈에 대한 오프셋 데이터와 뼈에 대한 ID
};



/// <summary>
/// 메쉬 데이터 그룹
/// </summary>
class CMeshGroup final : public CBase
{
	DERIVED_CLASS(CBase, CMeshGroup)
private:
	explicit CMeshGroup() = default;
	explicit CMeshGroup(const CMeshGroup& rhs) = delete;
	virtual ~CMeshGroup() = default;

public:
	static CMeshGroup* Create();
	virtual void Free() override;
	
public:
	_uint				Get_NumMeshes() const { return m_iNumMeshes; }
	FMeshData* const	Find_Mesh(const _uint iIndex) const;
	FMeshData* const	Find_Mesh(const wstring& strMeshKey, const _uint iRangeIndex) const;
	HRESULT				Add_Mesh(const wstring& strMeshKey, FMeshData* const pMeshData);
	
private:
	_uint							m_iNumMeshes;	// 메쉬 개수
public:
	multimap<wstring, FMeshData*>	mapMeshDatas;	// 이름 탐색
	vector<FMeshData*>				vecMeshDatas;	// 인덱스 검색 메쉬 데이터

	_float3			vBoxMin = {};
	_float3			vBoxMax = {};
	_float			fCullRadius = {};

public:
	// 메쉬가 모두 넣어진 상태에서 만들 것.
	HRESULT Create_VTF_OffsetBones(class CBoneGroup* pBoneGroup);

private:
	_bool								m_bIsUseVTF = { false };
	_uint								m_iNumVTFRows = { 0 };				// 줄 수
	_uint								m_iNumVTFColums = { 0 };			// 줄당 개수 (행렬)
	_uint								m_iNumVTF_LastRowColums = { 0 };	// 마지막 줄 요소 개수
	ComPtr<ID3D11Texture2D>				m_pVertexTexture = { nullptr };		// 2차원 배열 형태의 메시별 오프셋 행렬들
	ComPtr<ID3D11ShaderResourceView>	m_pVT_SRV = { nullptr };

};


END