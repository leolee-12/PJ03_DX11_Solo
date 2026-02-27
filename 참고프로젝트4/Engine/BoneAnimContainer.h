#pragma once

#include "Base.h"

BEGIN(Engine)

struct FKeyFrame
{
	_float fTrackPosition;
	_float3 vPos;
	_float4 qtRot;
	_float3 vScale;
};

// GPU용
struct FBoneKeyFrame
{
	_int iBoneID;
	_float fTrackPosition;
	_float3 vPos;
	_float4 qtRot;
	_float3 vScale;
};

/// <summary>
/// 트랙 포즈가 빠진 순수 키프레임값
/// </summary>
struct FKeyFrameTR
{
	_int iBoneID;
	_float3 vPos;
	_float4 qtRot;
	_float3 vScale;
};

// 단순 키프레임 보간 할 때 쓰인다. 이떄는 키 프레임의 결과가 나와 있어야 한다.
struct FKeyFrameInterpolate
{
	_float				fWeight;
	vector<FKeyFrameTR>	KeyFrames;
};

// 애니메이션 정보를 보간, 애니메이션에 현재 위치를 통해 보간하기 때문에. 마스크 내에서 트랜지션할 때 쓰인다.
struct FAnimInterpolate
{
	_int			iAnimID;		// 보간 후보 애니메이션 ID
	vector<_int>	vecChannelIDs;	// 보간 후보 애니메이션의 채널 ID들
	_float			fTrackPos;		// 트랙 위치
	_float			fWeight;		// 보간 가중치
};

/// <summary>
/// 애니메이션 채널의 프레임별 노드
/// </summary>
class ENGINE_DLL FBoneAnimChannelData final : public CBase
{
	DERIVED_CLASS(CBase, FBoneAnimChannelData)
private:
	explicit FBoneAnimChannelData() = default;
	explicit FBoneAnimChannelData(const FBoneAnimChannelData& rhs);
	virtual ~FBoneAnimChannelData() = default;

public:
	static FBoneAnimChannelData* Create();
	virtual void Free() override;

public:
	/// <summary>
	/// 설정한 시간으로 보간된 행렬을 반환하는 함수
	/// </summary>
	/// <param name="fTime">프레임 값으로 넣어준다.</param>
	/// <returns></returns>
	_float4x4 Interpolated_Matrix(const _float& fCurTrackPos) const;
	FKeyFrameTR Interpolated_KeyFrame(const _float& fCurTrackPos) const;

	// 각각 배열의 기준점을 찾아주는 함수, 해당 기준점을 통해 애니메이션 위치를 알아낸다.
	// 유틸성을 위해서 현재 위치를 저장하는 방식을 사용하지 않고, 기준점을 찾아준다.
	// 반복문을 통해 기준점을 찾아내는데. 기본적인 기준점은 비율로 구하며, 이는 Baking된 데이터에 대한 처리라
	// 왠만한 상황에서 상수시간의 알고리즘을 갖는다.
	_uint Calculate_Pivot(const _float& fCurTrackPos) const;

public:
	_int					iBoneID = -1;			// 뼈 ID
	_uint					iNumKeyFrames;			// 키 프레임 개수
	vector<FKeyFrame>		vecKeyFrames;			// 위치 데이터들


};



/// <summary>
/// 애니메이션 노드를 포함하는 데이터
/// 전체 재생 시간과 재생속도도 포함
/// </summary>
class ENGINE_DLL FBoneAnimData final : public CBase
{
	DERIVED_CLASS(CBase, FBoneAnimData)
private:
	explicit FBoneAnimData() = default;
	explicit FBoneAnimData(const FBoneAnimData& rhs) = delete;
	virtual ~FBoneAnimData() = default;

public:
	static FBoneAnimData* Create();
	virtual void Free() override;

public:
	const FBoneAnimChannelData* const Find_AnimChannelData(_uint iIndex) const;
	const FBoneAnimChannelData* const Find_AnimChannelData(const wstring& strNodeKey) const;
	void Add_AnimChannelData(const wstring& strNodeKey, FBoneAnimChannelData* pAnimNodeData);

public:
	// 시간 변화율로 애니메이션 타임라인의 현재 시간을 구해주는 함수, Mod를 켜면 반복됨
	_float Calculate_Time(_float fCurTime, _bool bMod = true) const;
	// 모든 채널 추가된 이후에 실행할 것.
	HRESULT Create_ChannelBuffers();
	
	
public:
	_uint	iID = 0;				// 애니메이션 ID
	wstring strName = L"";			// 애니메이션 이름
	_float	fDuration = 0.0;		// 진행 길이
	_float	fTickPerSecond = 0.0;	// 시간당 프레임

public:
	map<const wstring, FBoneAnimChannelData*>	mapAnimChannels;	// 노드 이름으로 검색 시스템
	vector<FBoneAnimChannelData*>				vecAnimChannels;	// 애니메이션 인덱스 검색 시스템
	
public:
	vector<ComPtr<ID3D11Buffer>>				AnimChannelBuffers;	// 애니메이션 버퍼채널, 뼈와 키프레임으로 구성
	vector<ComPtr<ID3D11ShaderResourceView>>	AnimChannelSRVs;	// SRV. 읽기용으로만 쓸 것임.
	
};



/// <summary>
/// 뼈 애니메이션들을 저장하는 그룹
/// </summary>
class ENGINE_DLL CBoneAnimGroup final : public CBase
{
	DERIVED_CLASS(CBase, CBoneAnimGroup)
private:
	explicit CBoneAnimGroup() = default;
	explicit CBoneAnimGroup(const CBoneAnimGroup& rhs) = delete;
	virtual ~CBoneAnimGroup() = default;

public:
	static CBoneAnimGroup* Create();
	virtual void Free() override;

public:
	FBoneAnimData* const	Find_BoneAnim(const _uint iIndex);
	FBoneAnimData* const	Find_BoneAnim(const wstring& strAnimKey);
	HRESULT Add_BoneAnim(const wstring& strAnimKey, FBoneAnimData* pAnimData);

	
	// 한 애니메이션에 대한 모든 애니메이션의 키를 얻어야할 때 사용한다.
	// 이동 생성자를 통해 복사한다.
	vector<FKeyFrameTR> Provide_AnimKeyFrames(_uint iAnimIndex, const _float& fTrackPos, const class CBoneGroup& BoneGroup) const;

	// 한 애니메이션에 대한 일부 애니메이션의 키를 얻어야할 때 사용한다. 부모기준 하위로 간다.
	vector<FKeyFrameTR> Provide_AnimKeyFrames_ParentedDown(
		_uint iAnimIndex, const _float& fTrackPos, const class CBoneGroup& BoneGroup
		, const wstring& strParentBoneName, const vector<wstring>& strEndBones, _uint iRecursive = UINT_MAX) const;

	// 한 애니메이션에 대한 일부 애니메이션의 키를 얻어야할 때 사용한다. 부모기준 상위로 간다.
	vector<FKeyFrameTR> Provide_AnimKeyFrames_ParentedUp(
		_uint iAnimIndex, const _float& fTrackPos, const class CBoneGroup& BoneGroup
		, const wstring& strParentBoneName, _uint iRecursive = UINT_MAX) const;

	// 한 애니메이션에 대한 단일 보간값을 받아내야 할 때 사용한다.
	FKeyFrameTR Interpolated_Anim(const _uint iAnimIndex, const _uint iChannelIndex, const _float& fCurTrackPos) const;

	// 보간에 필요한 정보를 받아 내부의 애니메이션을 이용해 보간된 키프레임을 돌려주는 함수
	void Interpolated_Anims(FKeyFrameTR* pKeyFrames, size_t iNumKeyFrames, FAnimInterpolate* pArrInterpolateData, size_t iNumInterpolates);
	
	// 키프레임끼리 보간, 이미 나온 키프레임 결과물에 대해 시행한다.
	void Interpolated_KeyFrames(FKeyFrameTR* pKeyFrames, size_t iNumKeyFrames, FKeyFrameInterpolate* pArrInterpolate, size_t iNumInterpolates);
	
private:
	void CalcThread_Interpolated_KeyFrames(_uint iIndex, FBoneAnimData* pBoneAnim, FAnimInterpolate* pData, _float fWeightRatio,
		_vector* vPos, _vector* vRot, _vector* vScale);

public:
	const _uint& Get_NumAnims() const { return iNumAnims; }
	const vector<FBoneAnimData*>& Get_BoneAnimDatasVector() const { return vecAnimDatas; }

private:
	_uint									iNumAnims;
	unordered_map<wstring, FBoneAnimData*>	mapAnimDatas;	// 애니메이션 저장 맵, 키는 애니메이션 이름
	vector<FBoneAnimData*>					vecAnimDatas;	// 애니메이션 인덱스 검색
	
};

END