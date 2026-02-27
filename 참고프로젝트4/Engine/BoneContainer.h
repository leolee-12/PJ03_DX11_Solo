#pragma once

#include "Base.h"

BEGIN(Engine)

/// <summary>
/// 주로 뼈의 노드로 쓰이는 클래스
/// </summary>
class ENGINE_DLL FBoneData final : public CBase
{
	DERIVED_CLASS(CBase, FBoneData)

protected:
	explicit FBoneData() = default;
	explicit FBoneData(const FBoneData& rhs);
	virtual ~FBoneData() = default;

public:
	static FBoneData*	Create();
	virtual FBoneData*	Clone();
	virtual void		Free() override;

public:
	wstring			strName;				// 노드 이름

	_short			iParentID = { -1 };		// 부모 노드 ID
	_short			iID = -1;				// 뼈 구분용 ID
	
	vector<_short>	iChildrenID;			// 자식 노드 ID
	
};


/// <summary>
/// 뼈 노드 데이터 집합
/// </summary>
class ENGINE_DLL CBoneGroup final : public CBase
{
	DERIVED_CLASS(CBase, CBoneGroup)

private:
	explicit CBoneGroup();
	explicit CBoneGroup(const CBoneGroup& rhs);
	virtual ~CBoneGroup() = default;

public:
	static CBoneGroup* Create();
	virtual CBoneGroup* Clone();
	virtual void Free() override;

public:
	FBoneData* Find_BoneData(_int iID);
	FBoneData* Find_BoneData(const wstring& strBoneNodeKey);
	FBoneData* Find_RootTransBoneData();
	FBoneData* Find_RootRotateBoneData();
	FBoneData* Find_RootScaleBoneData();

public:
	// 뼈 추가함수
	HRESULT		Add_BoneData(const wstring& strBoneNodeKey, FBoneData* pNode, _fmatrix& TransformMatrix);

public:
	// 트랜스폼 행렬을 특정 뼈에 적용한다. 애니메이션을 적용할 때 쓰인다.
	HRESULT		Invalidate_BoneTransform(_uint iIndex, _fmatrix& matTransform);
	HRESULT		Invalidate_BoneTransforms(vector<_float4x4>& Transforms, vector<_bool>& Applies);
	// 뼈 업데이트 함수, 애니메이션 적용 후에 불러온다.
	void		Invalidate_FinalTransforms();
	void		Provide_ChildrenList(vector<_short>& vecBones, const wstring& strBoneNodeKey, const vector<wstring>& strEndBones, _uint iRecursive) const;

private:
	void		Provide_ChildrenList_Recursive(vector<_short>& vecBones, const vector<_short>& vecEndBones, const _short& iIndex, _uint iRecursive) const;

public:
	// 뼈의 개수
	const _uint					CRef_BoneDatas_Count() const { return m_iNumBones; }
	vector<FBoneData*>			Get_BoneDatasVector() const { return m_BonesVector; }
	const vector<_float4x4>&	CRef_BoneTransforms() const { return m_TransformMatrices; }
	const vector<_float4x4>&	CRef_BoneCombinedTransforms() const { return m_CombinedTransformMatrices; }
	//const vector<_float4x4>&	CRef_BoneOffsetTransforms() const { return m_OffsetTransformMatrices; }
	void Reserve_BoneSize(_uint iNumBoneSize);

public:
	const _uint			CRef_RootTransBoneInd() const { return m_iRootTransBoneInd; }
	void				Set_RootTransBoneInd(const _uint iIndex) { m_iRootTransBoneInd = iIndex; }
	const _uint			CRef_RootRotateBoneInd() const { return m_iRootRotateBoneInd; }
	void				Set_RootRotateBoneInd(const _uint iIndex) { m_iRootRotateBoneInd = iIndex; }
	const _uint			CRef_RootScaleBoneInd() const { return m_iRootScaleBoneInd; }
	void				Set_RootScaleBoneInd(const _uint iIndex) { m_iRootScaleBoneInd = iIndex; }

public:
	// 모든 뼈의 루트인 노드의 사전회전 세팅을 한다.
	void				Set_PreRotate(_matrix Matrix);

	// 뼈 컨테이너
private:
	_uint						m_iNumBones = { 0 };			// 뼈 개수
	map<wstring, FBoneData*>	m_BonesMap;						// 뼈 이름 검색
	vector<FBoneData*>			m_BonesVector;					// 뼈 인덱스 검색

	// 트랜스폼
private:
	vector<_float4x4>			m_TransformMatrices;			// 애니메이션 트랜스폼. 최적화용
	vector<_float4x4>			m_CombinedTransformMatrices;	// 계층관계 적용. 최적화용
	//vector<_float4x4>			m_OffsetTransformMatrices;		// 오프셋행렬. 특수한 경우에 쓰인다.
	//vector<_bool>				m_IsBoneCalculated;				// 뼈가 계산되었는지, 매프레임 초기화 해야함.


	// 루트모션 뼈
private:
	_uint						m_iRootTransBoneInd = { UINT_MAX };		// 루트모션 이동 뼈 인덱스
	_uint						m_iRootRotateBoneInd = { UINT_MAX };	// 루트모션 회전 뼈 인덱스
	_uint						m_iRootScaleBoneInd = { UINT_MAX };		// 루트모션 스케일 뼈 인덱스

	// VTF, [Deprecated]
/*public:
	HRESULT Create_VTF_Bones();
	HRESULT Bind_TransformsToVTF();

	const _bool	CRef_IsUse_VTF() const { return m_bUse_VTF; }


	// VTF
private:
	_bool								m_bUse_VTF = { false };				// 상수버퍼가 커지면 VTF를 만든다. 256개로 하고, 그 이상은 텍스처 처리
	_uint								m_iNumVTFRows = { 0 };				// 줄 수
	_uint								m_iNumVTFColums = { 0 };			// 줄당 개수 (행렬)
	_uint								m_iNumVTF_LastRowColums = { 0 };	// 마지막 줄 요소 개수
	ComPtr<ID3D11Texture2D>				m_pVertexTexture = { nullptr };		// VTF용 텍스처
	ComPtr<ID3D11ShaderResourceView>	m_pVT_SRV = { nullptr };			// VTF 셰이더 리소스


public:
	HRESULT Create_TransformBuffers();
	*/


	// GPU 계산용 행렬버퍼
/*private:
	ComPtr<ID3D11Buffer>				m_TransformBuffer = { nullptr };	// GPU용 트랜스폼
	ComPtr<ID3D11ShaderResourceView>	m_TransformSRV = { nullptr };		// SRV
	ComPtr<ID3D11UnorderedAccessView>	m_TransformUAV = { nullptr };		// UAV

	ComPtr<ID3D11Buffer>				m_CombinedTransformBuffer = { nullptr };	// GPU용 최종 행렬 버퍼
	ComPtr<ID3D11ShaderResourceView>	m_CombinedTransformSRV = { nullptr };		// SRV
	ComPtr<ID3D11UnorderedAccessView>	m_CombinedTransformUAV = { nullptr };		// UAV
	*/
};

END