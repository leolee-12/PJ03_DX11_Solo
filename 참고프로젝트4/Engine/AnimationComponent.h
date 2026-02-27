#pragma once

#include "Component.h"

#include "BoneAnimContainer.h"
#include "SkeletalComponent.h"
#include "AnimNotify.h"
#include "AnimNotifyComp.h"

#include "Utility/LogicDeviceBasic.h"

BEGIN(Engine)

class CBoneAnimGroup;
class FBoneAnimData;
class FSkeletalData;
class FModelData;
class CBoneGroup;
class CComputeShaderComp;

enum ERootAnimType
{
	None, X, Y, Z, XY, YZ, ZX, XYZ
};

struct TAnimData
{
	_uint		iAnimID;
	string		strAnimName;
	_float		fDuration;
	_float		fTickPerSeconds;
	_float		fTrackPos;
	_float		fPrevTrackPos;
	_float		fSpeedMultiply;
	_bool		bIsLoop;
	_bool		bIsReverse;
	_bool		bIsTransition;
	ERootAnimType	eRootMotion_TransApplyType;	// 루트모션 적용 타입, 선택된 걸로 적용 범위가 달라짐
	wstring			strStartBoneName;		// 뼈 제한 시작뼈
	vector<wstring>	strEndBoneNames;		// 뼈 제한 끝뼈들
	_uint			iRecursive = { UINT_MAX };
};

/// <summary>
/// 애니메이션에 대한 처리를 담당하는 컴포넌트
/// 뼈 하나에 대응하는 
/// </summary>
class ENGINE_DLL CAnimationComponent final : public CComponent
{
	DERIVED_CLASS(CComponent, CAnimationComponent)
protected:
	explicit CAnimationComponent(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	explicit CAnimationComponent(const CAnimationComponent& rhs);
	virtual ~CAnimationComponent() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* Arg = nullptr) override;

public:
	static shared_ptr<CAnimationComponent> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual shared_ptr<CComponent> Clone(void* Arg = nullptr) override;

protected:
	virtual void	Free() override;

public:
	HRESULT Bind_BoneGroup(CBoneGroup* pBoneGroup);
	HRESULT Load_Animations(const wstring& strModelFilePath);

public:
	CBoneGroup* const		Get_BoneGroup() const { return m_pBoneGroup; }
	CBoneAnimGroup* const	Get_AnimGroup() const { return m_pAnimGroup; }

public:
	const FBoneAnimData* Find_BoneAnim(_uint iAnimID) const;

private:
	CBoneGroup* m_pBoneGroup = { nullptr };
	CBoneAnimGroup* m_pAnimGroup = { nullptr };		// 단 하나의 뼈에 대한 애니메이션 정보를 가진다.
	// 이 녀석이 설정되어 있어야 기능을 할 수 있다.
#pragma region 컴퓨트 셰이더 컴포넌트 + 버퍼
	/* Deprecate, 애니메이션 키프레임 계산은 되나 Staging 버퍼 전송 비용 문제로 해결 비용으로 인한 취소
public:
	struct TBoneBlenderConstants
	{
		_uint iSize;
		_float fBlend;
		_float fPadding[2];
	};
	// 블렌딩용 버퍼 모음
	struct TBoneBlenderBuffers
	{
		// 상수 버퍼
		ComPtr<ID3D11Buffer>				pConstantBuffer;

		// 계산들어가는 SRV
		ComPtr<ID3D11Buffer>				pInputBuffers[2];	// 입력용 버퍼
		ComPtr<ID3D11ShaderResourceView>	pInputSRVs[2];
		ComPtr<ID3D11Buffer>				pOutputBuffer;
		ComPtr<ID3D11UnorderedAccessView>	pOutputUAV;

		// 결과물 받는 스테이징 버퍼
		ComPtr<ID3D11Buffer>				pBackBuffer;
	};

	struct TTransformBuffers
	{
		// 상수 버퍼
		ComPtr<ID3D11Buffer>				pConstantBuffer;

		ComPtr<ID3D11Buffer>				pKeyframeBuffer;
		ComPtr<ID3D11ShaderResourceView>	pKeyframeSRV;
		ComPtr<ID3D11Buffer>				pTransformBuffer;
		ComPtr<ID3D11UnorderedAccessView>	pTransformUAV;

		ComPtr<ID3D11Buffer>				pStagingBuffer;
	};

	struct TMaskerBuffers
	{
		ComPtr<ID3D11Buffer>				pKeyframeBuffer;
		ComPtr<ID3D11ShaderResourceView>	pKeyframeSRV;
	};

private:
	// 행렬 블렌딩 계산 셰이더 생성
	HRESULT Create_BoneBlenderShader();
	// 행렬 블렌딩 계산 셰이더 실행
	HRESULT Compute_BoneBlenderShader(vector<FKeyFrameTR>& TargetTransforms, vector<FKeyFrameTR>& BlendTransforms, _float fBlend);
	//
	HRESULT Create_CreateTransformShader();
	// 트랜스폼 생성
	HRESULT Compute_CreateTransformShader(vector<_float4x4>& Transforms, vector<FKeyFrameTR>& Keyframes);

private:
	// 블렌드 컴퓨트 셰이더
	shared_ptr<CComputeShaderComp>		m_pBoneBlender = { nullptr };
	TBoneBlenderBuffers					m_BoneBlenderBuffers = {};

private:
	// 트랜스폼 컴퓨트 셰이더
	shared_ptr<CComputeShaderComp>		m_pTransformCreator = { nullptr };
	TTransformBuffers					m_TransformCreateBuffers = {};

private:
	// 마스킹 컴퓨트 셰이더
	shared_ptr<CComputeShaderComp>		m_BoneMasker = { nullptr };
	*/
#pragma endregion Deprecated

#pragma region 메인 애니메이션 체킹함수
public:
	_bool	IsAnimation_Finished();
	_bool	IsAnimation_Finished(wstring strAnimName);
	_bool	IsCurrentAnimation(wstring& strAnimName);

	_bool	IsAnimation_UpTo(_float m_fTrackPos);
	_bool	IsAnimation_Frame_Once(_float m_fTrackPos);
	_bool	IsAnimation_Range(_float fTrackMin, _float fTrackMax);
	_bool	IsAnimation_Transition() const { return m_MainAnim.Cur.bIsTransition && !m_MainAnim.fTransBlend.IsMax(); }
	_bool   IsCurrentAnimation(wstring strAnimName);
#pragma endregion


#pragma region 메인 애니메이션 설정함수
public:
	void	Set_Animation(_uint iAnimIndex, _float fSpeedMultiply, _bool bIsLoop,
		_bool bIsTransition = true, _float fTransitionSpeed = 1.0f,
		_bool bReverse = false, _bool bIsRootReset = true,
		ERootAnimType eRootAnimType = ERootAnimType::XYZ);

	void	Set_Animation(const wstring& strAnimName, _float fSpeedMultiply, _bool bIsLoop,
		_bool bIsTransition = true, _float fTransitionSpeed = 1.0f,
		_bool bReverse = false, _bool bIsRootReset = true,
		ERootAnimType eRootAnimType = ERootAnimType::XYZ);

	void	Set_AnimationMaintain(_uint iAnimIndex, _float fSpeedMultiply, _bool bIsLoop,
		_bool bReverse = false, _float fTransitionSpeed = 1.0f,
		_bool bIsRootReset = true);

	void	Tick_AnimTime(_cref_time fTimeDelta);
	void	Set_AnimTrackPos(_int iIndex);
#pragma endregion


#pragma region 메인 애니메이션 정보 취득 함수
public:
	// 애니메이션 정보 취득 함수
	string	Get_PrevAnimName();
	string	Get_CurrentAnimName();
	_float	Get_AnimTrackPos();
	_float	Get_AnimDuration();
#pragma endregion


public:
	// 컴포넌트의 행렬도 회전에 적용된다.
	void	Invalidate_Animation(_cref_time fTimeDelta);
	//void	Invalidate_AnimationCShader(_cref_time fTimeDelta);
	void	Invalidate_AnimationWithMask(class CAnimMaskComp* pAnimMaskComp);

public:
	struct TAnimPair
	{
		TAnimData	Cur = {};
		TAnimData	Prev = {};
		_float		fTransSpeed = { 0.1f };
		FGauge		fTransBlend = { 1.f };

	};

private:
	// 현재 애니메이션에 대한 
	TAnimPair			m_MainAnim;


public:
	struct TMaskAnimPair
	{
		TAnimData	Cur = {};
		TAnimData	Prev = {};
		_bool		bUsing = { false };
		_float		fTransSpeed = { 0.1f };
		FGauge		fTransBlend = { 1.f };
		_bool		bEndMasking = { false };
		_float		fFadeOutSpeed = { 1.f };
		FGauge		fFadeOutBlend = { 1.f };
	};

public:
	void	Create_MaskAnim() { m_MaskAnim.emplace_back(TMaskAnimPair()); }
	void	Set_MaskAnim(_uint iIndex, const wstring& strAnimName
		, const wstring& strBasisBone, const vector<wstring>& strEndBoneNames
		, _uint iRecursive = UINT_MAX
		, _float fSpeedMultiply = 1.f, _bool bIsLoop = false
		, _bool bIsTransition = true, _float fTransitionSpeed = 1.0f
		, _bool bReverse = false);
	void	Exit_MaskAnim(_uint iIndex, _float fFadeOutSpeed = 1.f);
	void	Tick_MaskAnimTime(_cref_time fTimeDelta);


private:
	vector<TMaskAnimPair>	m_MaskAnim;


#pragma region ADD 애니메이션
public:
	struct TAddAnim
	{
		TAnimData	Add = {};
		_bool		bUsing = { false };
		_float		fWeight = { 1.f };
	};

public:
	void	Create_Add_Animation() { m_AddAnims.push_back(TAddAnim()); }
	_uint	Get_NumAdd_Animations() const { return (_uint)m_AddAnims.size(); }
	// 일반 ADD 애니메이션 실행함수
	void	Set_ADD_Animation(_uint iIndex, const wstring& strAnimName, const wstring strBasisBone
		, const vector<wstring>& strEndBoneNames, _uint iRecursive = UINT_MAX
		, _float fSpeedMultiply = 1.f, _float fWeight = 1.f, _bool bReverse = false);
	// 강제 종료
	void	Set_ADD_AnimFinish(_uint iIndex);

	// ADD타입 애니메이션 업데이트
	void	Tick_ADD_AnimTime(_cref_time fTimeDelta);
	// ADD타입 애니메이션 체킹함수
	_bool	IsADDAnim_Finished(_uint iIndex) const 
	{ return m_AddAnims[iIndex].Add.fTrackPos >= m_AddAnims[iIndex].Add.fDuration; }

private:
	_bool	IsADDAnim_Using(_uint iIndex) const { return m_AddAnims[iIndex].bUsing; }
#pragma endregion


private:
	// ADD 애니메이션
	vector<TAddAnim>	m_AddAnims = {};
	

	// 루트 모션 관련
public:
	// Translate, Scale 전용
	_vector			Apply_TransDeltaByRootAnimType(ERootAnimType eType, _fvector vTranslate);

	_float3			Get_Root_TransDelta() const { return m_vRootTransDelta; }
	_vector			Get_Root_TransDeltaVector() const { return m_vRootTransDelta; }
	_float4			Get_Root_RotateDelta() const { return m_vRootRotateDelta; }
	_vector			Get_Root_RotateDeltaVector() const { return m_vRootRotateDelta; }
	_float3			Get_Root_ScaleDelta() const { return m_vRootScaleDelta; }
	_vector			Get_Root_ScaleDeltaVector() const { return m_vRootScaleDelta; }
	
	_float3			Get_Root_Trans() const { return m_vRootTrans; }

private:
	void			RootReset() { m_vRootTransDelta = {}, m_vRootTrans = {}; m_vRootTrans_Prev = {}; }

private:
	_float3				m_vRootTransDelta = {};			// 이동값
	_float4				m_vRootRotateDelta = {};		// 회전값
	_float3				m_vRootScaleDelta = {};			// 스케일 값

	// 계산용
	_float3				m_vRootTrans = {};	// 이전
	_float4				m_vRootRotate = {};
	_float3				m_vRootScale = {};

	_float3				m_vRootTrans_Prev = {};	// 이전
	_float4				m_vRootRotate_Prev = {};
	_float3				m_vRootScale_Prev = {};

public:
	using SNotifyCollections = map<const string, shared_ptr<CAnimNotifyCollection>>;

public:
	HRESULT Clone_NotifyFromPrototype(const wstring& strProtoTag);
	// 노티파이 이벤트 등록
	HRESULT Regist_EventToNotify(const string& strAnimName, const string& strCollectionName, 
										const string& strNotifyName, FastDelegate0<> Event);
	// 노티파이에 모델 컴포넌트 등록
	HRESULT Regist_ModelCompToNotify(weak_ptr<CCommonModelComp> pModelComp);

private:
	// 노티파이 작동 함수
	void Trigger_AnimNotifyByCurAnim(_cref_time fTrackPos);
	// 노티파이 리셋 함수
	void ResetTrigger_AnimNotifyByCurAnim();

	void Set_AnimNameToNotify();

private:
	shared_ptr<CAnimNotifyComp>	m_pNotifyComp = { nullptr };	// 노티파이 저장소
};

END