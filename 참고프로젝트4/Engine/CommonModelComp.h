#pragma once

#include "SceneComponent.h"
#include "MeshComp.h"
#include "MaterialComponent.h"
#include "SkeletalComponent.h"
#include "AnimationComponent.h"
#include "Shader.h"

BEGIN(Engine)


/// <summary>
/// 학원 프레임워크에 맞춰서 제작된 범용 모델 컴포넌트
/// 이를 기반으로 추후 모델로드 매니저 개선을 할 예정
/// 
/// 모델 매니저와 통신도 가능한 형태로 제작된다.
/// 이 컴포넌트에도 모델 로드 기능은 존재.
/// 이는 프로토타입으로도 모델을 제작 할 수 있게 하기 위함임.
/// </summary>
class ENGINE_DLL CCommonModelComp final : public CSceneComponent
{
	DERIVED_CLASS(CSceneComponent, CCommonModelComp)

public:
	enum TYPE { TYPE_NONANIM, TYPE_ANIM, TYPE_END };
	enum class ERootMotionType { Trans, Rotate, Scale };

public:
	struct COMMON_MODEL_DESC
	{
		TYPE	eModelType;
		wstring strModelFilePath;
		wstring strShaderFilePath;
		const D3D11_INPUT_ELEMENT_DESC* pElements;
		_uint iNumElements;
	};

protected:
	explicit CCommonModelComp(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	explicit CCommonModelComp(const CCommonModelComp& rhs);
	virtual ~CCommonModelComp() = default;

public:
	virtual HRESULT	Initialize_Prototype() override;
	virtual HRESULT	Initialize_Prototype(COMMON_MODEL_DESC& tDesc);
	virtual HRESULT Initialize(void* Arg = nullptr) override;
	virtual void	Priority_Tick(const _float& fTimeDelta);
	virtual void	Tick(const _float& fTimeDelta);
	virtual void	Late_Tick(const _float& fTimeDelta);
	virtual HRESULT	Render();

public:
	static shared_ptr<CCommonModelComp> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	static shared_ptr<CCommonModelComp> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, COMMON_MODEL_DESC& tDesc);
	virtual shared_ptr<CComponent> Clone(void* Arg = nullptr);

protected:
	virtual void	Free() override;



#pragma region 모델 링킹
public:
	// 매니저로부터 모델을 링크하는 기능을 가진다.
	HRESULT Link_Model(TYPE eType, const wstring& strModelFilePath);
	HRESULT Link_Model(const wstring& strModelFilePath) { return Link_Model(m_eModelType, strModelFilePath); }
	// 메쉬링크 따로, 뼈 링크 따로 거는 경우 이 함수를 쓰세요.
	HRESULT Link_Model(TYPE eType, const wstring& strMeshMatrialModel, const wstring& strBoneAnimModel);
	void	Unlink_Model();

	_bool	Intersect_MousePos(SPACETYPE eSpacetype, _float3* pOut, _matrix matWorld, _float* pLengthOut = nullptr);
#pragma endregion




#pragma region 애니메이션 제어
	// 애니메이션 제어 함수
public:
	// 애니메이션 세팅
	void Set_Animation(_uint iAnimIndex, _float fSpeedMultiply, _bool bIsLoop, _bool bIsTransition = true, _float fTransitionSpeed = 1.0f, _bool bReverse = false, _bool bIsRootReset = true);

	// 애니메이션 이름으로 세팅
	void Set_Animation(const string& strAnimName, _float fSpeedMultiply, _bool bIsLoop, _bool bIsTransition = true, _float fTransitionSpeed = 1.0f, _bool bReverse = false, _bool bIsRootReset = true);

	// 애니메이션 현재 키 프레임 유지재생
	void Set_AnimationMaintain(_uint iAnimIndex, _float fSpeedMultiply, _bool bIsLoop, _bool bReverse = false, _float fTransitionSpeed = 0.1f, _bool bIsResetRootMotion = false);

	// 애니메이션 재생, 뼈 업데이트, 행렬 업데이트
	void Tick_AnimTime(const _float& fTimeDelta);

	void Apply_RootMotion_To_GameObject();

	//현재 애니메이션 이름 받아오기
	string Get_CurrentAnimationName();
	string Get_PrevAnimationName();

	// 루트 모션 이동량 얻기
	_float3 Get_RootTransDelta_Float3();
	_vector Get_RootTransDelta_Vector();

	// 애니메이션 컴포넌트 얻기
	shared_ptr<CAnimationComponent> Get_AnimationComponent() { return m_pAnimationComp; }

	_float3 Get_RootTrans();

	// 애니메이션 상태 확인
public:
	_bool	IsAnimation_Finished();
	_bool	IsAnimation_Finished(string strAnimName);
	_bool	IsAnimation_UpTo(_float m_fTrackPos);
	_bool	IsAnimation_Frame_Once(_float m_fTrackPos);
	_bool	IsAnimation_Range(_float fTrackMin, _float fTrackMax);
	_bool	IsAnimation_Transition() const;
	_bool	IsCurrentAnimation(string strAnimName);

#pragma endregion



private:
	// 모델 매니저에 연결
	HRESULT Link_ToModelManager();

public:
	const TYPE& Get_ModelType() const { return m_eModelType; }

private:
	// 데이터들을 모두 컴포넌트 형태로 저장함. 원본과 다른 부분
	TYPE	m_eModelType = { TYPE_END };
	wstring m_strModelPath = TEXT("");		// 프로토타입에 쓰임
	wstring m_strEffectPath = TEXT("");		// 프로토타입에 쓰임
public:
	wstring Get_ModelPath() { return m_strModelPath; }


#pragma region 메쉬 컴포넌트, 기능
public:
	// 메쉬 렌더 활성화
	void Active_Mesh(_uint iIndex);
	// 전체 메쉬 렌더 활성화
	void Active_AllMeshes();
	// 메쉬 비활성화
	void Deactive_Mesh(_uint iIndex);
	// 전체 메쉬 비활성화
	void Deactive_AllMeshes();
	// 외부 수동 바인드용
	HRESULT BindAndRender_Mesh(_uint iIndex);
	// 활성화된 메쉬 정보를 얻을 수 있다.
	const vector<_uint>& Get_ActiveMeshes() const { return m_ActiveMeshes; }
	const vector<shared_ptr<CMeshComp>>& Get_MeshComps() const { return m_pMeshComps; }

private:		// 메쉬 관련
	_uint								m_iNumMeshes = { 0 };
	vector<shared_ptr<CMeshComp>>		m_pMeshComps;
	_uint								m_iNumActiveMeshes = { 0 };
	vector<_uint>						m_ActiveMeshes;
#pragma endregion




#pragma region 머터리얼
public:
	// 원하는 메쉬의 머터리얼에 저장된 텍스처를 셰이더에 바인드한다.
	HRESULT Bind_MeshMaterialToShader(_uint iMeshIndex, TEXTURETYPE eType, const _char* pConstName);

private:		// 머터리얼 관련
	_uint									m_iNumMaterials = { 0 };
	vector<shared_ptr<CMaterialComponent>>	m_pMaterialComps;
#pragma endregion



#pragma region 스켈레탈 컴포넌트(뼈)
public:
	_float4x4	Get_BoneTransformFloat4x4(const wstring& strBoneName);
	_matrix		Get_BoneTransformMatrix(const wstring& strBoneName);
	_float4x4	Get_BoneTransformFloat4x4WithParents(_uint iIndex);
	_matrix		Get_BoneTransformMatrixWithParents(_uint iIndex);
	_matrix		Get_BoneTransformMatrixWithParents(const wstring& strBoneName);
	CBoneGroup* Get_BoneGroup() const;
	HRESULT		Bind_BoneToShader(_uint iMeshIndex, const _char* pBoneConstantName);
	void		Set_PreRotate(_matrix Matrix);
	// 보통 1번이 대부분 루트본이었음
	void		Set_RootTransBone(_uint iIndex = 1);
	// 원하는 뼈를 트랜스 뼈로 설정 하도록 바꿀 수 있다.
	void		Set_RootTransBone(const wstring& strBoneName);

private:		// 뼈 관련
	// 뼈 하나의 정보가 저장된 컴포넌트 여러개, 언리얼 용어를 따왔다.
	shared_ptr<CSkeletalComponent> m_pSkeletalComp = { nullptr };
#pragma endregion




#pragma region 애니메이션 컴포넌트
public:
	HRESULT Clone_NotifyFromPrototype(const wstring& strProtoTag);
	// 노티파이 이벤트 등록
	HRESULT Regist_EventToNotify(const string& strAnimName, const string& strCollectionName,
		const string& strNotifyName, FastDelegate0<> Event);
	HRESULT Regist_ModelCompToNotify(weak_ptr<CCommonModelComp> pModelComp);

public:
	shared_ptr<CAnimationComponent> AnimationComp() { return m_pAnimationComp; }

private:
	shared_ptr<CAnimationComponent> m_pAnimationComp = { nullptr };
#pragma endregion




#pragma region 셰이더 컴포넌트
	// 외부 설정용
public:
	// 이펙트를 바인드 한다.
	HRESULT Link_Shader(const wstring& strEffectKey, const D3D11_INPUT_ELEMENT_DESC* pElements = nullptr, _uint iNumElements = 0);
	// 이펙트를 언바인드 한다. 안쓸듯
	HRESULT Unlink_Shader();
	// 셰이더 비긴
	HRESULT Begin_Shader(_uint iPass);

	shared_ptr<CShader> ShaderComp() const { return m_pShaderComp; }

private:
	shared_ptr<CShader> m_pShaderComp = { nullptr };
#pragma endregion

};

END