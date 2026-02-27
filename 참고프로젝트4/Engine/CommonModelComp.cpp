#include "CommonModelComp.h"

#include "GameInstance.h"

#include "ModelContainer.h"

IMPLEMENT_CLONE(CCommonModelComp, CComponent)
IMPLEMENT_CREATE(CCommonModelComp)

CCommonModelComp::CCommonModelComp(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: Base(pDevice, pContext)
{
	if (nullptr == (m_pShaderComp = CShader::Create(pDevice, pContext)))
		__debugbreak();
}

CCommonModelComp::CCommonModelComp(const CCommonModelComp& rhs)
    : Base(rhs)
	, m_pMeshComps(rhs.m_pMeshComps)
	, m_pMaterialComps(rhs.m_pMaterialComps)
	, m_iNumMeshes(rhs.m_iNumMeshes)
	, m_iNumMaterials(rhs.m_iNumMaterials)
	, m_eModelType(rhs.m_eModelType)
	, m_ActiveMeshes(rhs.m_ActiveMeshes)
	, m_iNumActiveMeshes(rhs.m_iNumActiveMeshes)
{
	m_pShaderComp = DynPtrCast<CShader>(rhs.m_pShaderComp->Clone(nullptr));

	if (m_eModelType == TYPE_ANIM)
	{
		if (nullptr == (m_pSkeletalComp = static_pointer_cast<CSkeletalComponent>(rhs.m_pSkeletalComp->Clone())))
			__debugbreak();
		if (nullptr == (m_pAnimationComp = static_pointer_cast<CAnimationComponent>(rhs.m_pAnimationComp->Clone())))
			__debugbreak();

		m_pAnimationComp->Bind_BoneGroup(m_pSkeletalComp->Get_BoneGroup());
	}
}

HRESULT CCommonModelComp::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		RETURN_EFAIL;

    return S_OK;
}

HRESULT CCommonModelComp::Initialize_Prototype(COMMON_MODEL_DESC& tDesc)
{
	Link_Model(tDesc.eModelType, tDesc.strModelFilePath);
	if (FAILED(Link_Shader(tDesc.strShaderFilePath, tDesc.pElements, tDesc.iNumElements)))
		RETURN_EFAIL;

	return S_OK;
}

HRESULT CCommonModelComp::Initialize(void* Arg)
{
	if (FAILED(__super::Initialize(Arg)))
		RETURN_EFAIL;

    return S_OK;
}

void CCommonModelComp::Priority_Tick(const _float& fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);

}

void CCommonModelComp::Tick(const _float& fTimeDelta)
{
	__super::Tick(fTimeDelta);

}

void CCommonModelComp::Late_Tick(const _float& fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CCommonModelComp::Render()
{

    return S_OK;
}

void CCommonModelComp::Free()
{
	__super::Free();

	m_pMeshComps.clear();
	m_pMaterialComps.clear();

	//for (auto& MeshComp : m_pMeshComps)
	//	Safe_Release(MeshComp);

	////for (auto& MaterComp : m_pMaterialComps)
	//	Safe_Release(MaterComp);

	//Safe_Release(m_pSkeletalComp);
	//Safe_Release(m_pAnimationComp);
	//Safe_Release(m_pShaderComp);
}


HRESULT CCommonModelComp::Link_ToModelManager()
{
	if (nullptr == m_pGameInstance)
		RETURN_EFAIL;

	// 모델 매니저에 직접 접근하지는 않고, 게임 인스턴스에 연결되는지만 확인한다.

	return S_OK;
}

HRESULT CCommonModelComp::Link_Model(TYPE eType, const wstring& strModelFilePath)
{
	if (FAILED(Link_ToModelManager()))
		RETURN_EFAIL;

	// 로드 모델은 이미 있던 로드 성공했던 S_OK를 반환. 실패하면 파일이 없는 것.
	if (FAILED(m_pGameInstance->Load_Model(strModelFilePath)))
		RETURN_EFAIL;

	// 기존 모델이 있다면 해제한다.
	Unlink_Model();

	m_eModelType = eType;
	m_strModelPath = strModelFilePath;

	const FModelData* pModelData = m_pGameInstance->Find_ModelData(strModelFilePath);
	// 로드 성공이면 무조건 찾지만, 혹시 모를 버그를 위해 안전 코드
	// 있다면 안전하게 각기 컴포넌트들이 매니저로부터 데이터를 얻어올 수 있다는 증거이다.
	if (nullptr == pModelData)
		RETURN_EFAIL;

	m_iNumMeshes = pModelData->pMeshGroup->Get_NumMeshes();
	m_pMeshComps.reserve(m_iNumMeshes);
	for (_uint i = 0; i < m_iNumMeshes; i++)
	{
		m_pMeshComps.push_back(CMeshComp::Create(m_pDevice, m_pContext));
		m_pMeshComps[i]->Load_Mesh(strModelFilePath, i);
	}
	m_iNumMaterials = pModelData->pMaterialGroup->Get_NumMaterials();
	m_pMaterialComps.reserve(m_iNumMaterials);
	for (_uint i = 0; i < m_iNumMaterials; i++)
	{
		m_pMaterialComps.push_back(CMaterialComponent::Create(m_pDevice, m_pContext));
		m_pMaterialComps[i]->Ready_Materials_ByModel(strModelFilePath, i);
	}

	// 애니메이션 타입이면 뼈와 애니메이션을 로드한다.
	if (eType == TYPE_ANIM)
	{
		m_pSkeletalComp = CSkeletalComponent::Create(m_pDevice, m_pContext);
		if (nullptr == m_pSkeletalComp)
			RETURN_EFAIL;

		m_pAnimationComp = CAnimationComponent::Create(m_pDevice, m_pContext);
		if (nullptr == m_pAnimationComp)
			RETURN_EFAIL;

		m_pSkeletalComp->Load_Skeletal(strModelFilePath);
		m_pAnimationComp->Bind_BoneGroup(m_pSkeletalComp->Get_BoneGroup());
		m_pAnimationComp->Load_Animations(strModelFilePath);
	}

	Active_AllMeshes();

	return S_OK;
}

HRESULT CCommonModelComp::Link_Model(TYPE eType, const wstring& strMeshMatrialModel, const wstring& strBoneAnimModel)
{
	if (FAILED(Link_ToModelManager()))
		RETURN_EFAIL;

	// 로드 모델은 이미 있던 로드 성공했던 S_OK를 반환. 실패하면 파일이 없는 것.
	if (FAILED(m_pGameInstance->Load_Model(strMeshMatrialModel)))
		RETURN_EFAIL;

	// 기존 모델이 있다면 해제한다.
	Unlink_Model();

	m_eModelType = eType;
	m_strModelPath = strMeshMatrialModel;

	const FModelData* pModelData = m_pGameInstance->Find_ModelData(strMeshMatrialModel);
	// 로드 성공이면 무조건 찾지만, 혹시 모를 버그를 위해 안전 코드
	// 있다면 안전하게 각기 컴포넌트들이 매니저로부터 데이터를 얻어올 수 있다는 증거이다.
	if (nullptr == pModelData)
		RETURN_EFAIL;

	m_iNumMeshes = pModelData->pMeshGroup->Get_NumMeshes();
	m_pMeshComps.reserve(m_iNumMeshes);
	for (_uint i = 0; i < m_iNumMeshes; i++)
	{
		m_pMeshComps.push_back(CMeshComp::Create(m_pDevice, m_pContext));
		m_pMeshComps[i]->Load_Mesh(strMeshMatrialModel, i);
	}
	m_iNumMaterials = pModelData->pMaterialGroup->Get_NumMaterials();
	m_pMaterialComps.reserve(m_iNumMaterials);
	for (_uint i = 0; i < m_iNumMaterials; i++)
	{
		m_pMaterialComps.push_back(CMaterialComponent::Create(m_pDevice, m_pContext));
		m_pMaterialComps[i]->Ready_Materials_ByModel(strMeshMatrialModel, i);
	}

	// 애니메이션 타입이면 뼈와 애니메이션을 로드한다.
	if (eType == TYPE_ANIM)
	{
		m_pSkeletalComp = CSkeletalComponent::Create(m_pDevice, m_pContext);
		if (nullptr == m_pSkeletalComp)
			RETURN_EFAIL;

		m_pAnimationComp = CAnimationComponent::Create(m_pDevice, m_pContext);
		if (nullptr == m_pAnimationComp)
			RETURN_EFAIL;

		m_pSkeletalComp->Load_Skeletal(strBoneAnimModel);
		m_pAnimationComp->Bind_BoneGroup(m_pSkeletalComp->Get_BoneGroup());
		m_pAnimationComp->Load_Animations(strBoneAnimModel);
	}

	Active_AllMeshes();

	return S_OK;
}

void CCommonModelComp::Unlink_Model()
{
	for (auto& MeshComp : m_pMeshComps)
		MeshComp.reset();
	m_pMeshComps.clear();

	for (auto& MaterComp : m_pMaterialComps)
		MaterComp.reset();
	m_pMaterialComps.clear();

	m_pSkeletalComp.reset()	;
	m_pAnimationComp.reset();
}

_bool CCommonModelComp::Intersect_MousePos(SPACETYPE eSpacetype, _float3* pOut, _matrix matWorld, _float* pLengthOut)
{
	_float fMinLength = INFINITY;
	_float3 vOut = {};
	_bool bCheck = false;
	for (auto iter : m_pMeshComps)
	{
		if (iter->Intersect_MousePos(eSpacetype, pOut, matWorld, pLengthOut))
		{
			if (*pLengthOut < fMinLength)
			{
				fMinLength = *pLengthOut;
				vOut = *pOut;
				bCheck = true;
			}
		}
	}
	*pLengthOut = fMinLength;
	*pOut = vOut;

	return bCheck;
}


void CCommonModelComp::Set_Animation(_uint iAnimIndex, _float fSpeedMultiply, _bool bIsLoop, _bool bIsTransition, _float fTransitionSpeed , _bool bReverse , _bool bIsRootReset)
{
	if (nullptr == m_pAnimationComp)
		return;

	m_pAnimationComp->Set_Animation(iAnimIndex,  fSpeedMultiply, bIsLoop, bIsTransition,  fTransitionSpeed, bReverse, bIsRootReset);
}

void CCommonModelComp::Set_Animation(const string& strAnimName, _float fSpeedMultiply, _bool bIsLoop, _bool bIsTransition, _float fTransitionSpeed, _bool bReverse, _bool bIsRootReset)
{
	if (nullptr == m_pAnimationComp)
		return;

	m_pAnimationComp->Set_Animation(StrToWstr(strAnimName), fSpeedMultiply, bIsLoop, bIsTransition, fTransitionSpeed, bReverse, bIsRootReset);
}

void CCommonModelComp::Set_AnimationMaintain(_uint iAnimIndex, _float fSpeedMultiply, _bool bIsLoop, _bool bReverse, _float fTransitionSpeed, _bool bIsResetRoot)
{
	if (nullptr == m_pAnimationComp)
		return;

	m_pAnimationComp->Set_AnimationMaintain(iAnimIndex, fSpeedMultiply, bIsLoop, bReverse, fTransitionSpeed, bIsResetRoot);
}

void CCommonModelComp::Tick_AnimTime(const _float& fTimeDelta)
{
	if (m_eModelType != TYPE_ANIM)
		return;

	if (m_pSkeletalComp != nullptr && m_pSkeletalComp->Get_BoneGroup() != nullptr
		&& m_pSkeletalComp->Get_BoneGroup()->CRef_BoneDatas_Count() > 0)
	{
		// 애니메이션 시간 업데이트
		if (nullptr != m_pAnimationComp)
			m_pAnimationComp->Tick_AnimTime(fTimeDelta);

		// 마스킹 애니메이션 시간 업데이트
		if (nullptr != m_pAnimationComp)
			m_pAnimationComp->Tick_MaskAnimTime(fTimeDelta);

		// ADD 애니메이션 시간 업데이트
		if (nullptr != m_pAnimationComp)
			m_pAnimationComp->Tick_ADD_AnimTime(fTimeDelta);

		// 애니메이션 적용
		auto start = chrono::steady_clock::now();
		if (nullptr != m_pAnimationComp)
			m_pAnimationComp->Invalidate_Animation(fTimeDelta);// , Transform().Get_WorldMatrix());

		// 뼈 Combine 적용
		if (nullptr != m_pSkeletalComp)
			m_pSkeletalComp->Invalidate_BoneTransforms();
	}

	
	// 애니메이션 CombineTransform으로부터 루트 애니메이션 값 적용
	/*if (nullptr != m_pAnimationComp)
		m_pAnimationComp->Invalidate_RootMotion();*/

}

void CCommonModelComp::Apply_RootMotion_To_GameObject()
{
	auto pOwner = m_pOwner.lock().get();
	if (pOwner == nullptr)
		return;

	//pOwner->Get_TransformCom().lock()->Move_To_AnimationPos
}

string CCommonModelComp::Get_CurrentAnimationName()
{
	return m_pAnimationComp->Get_CurrentAnimName();
}

string CCommonModelComp::Get_PrevAnimationName()
{
	return m_pAnimationComp->Get_PrevAnimName();
}

_float3 CCommonModelComp::Get_RootTransDelta_Float3()
{
	if (m_pAnimationComp == nullptr)
		return _float3();

	return m_pAnimationComp->Get_Root_TransDelta();
}

_vector CCommonModelComp::Get_RootTransDelta_Vector()
{
	if (m_pAnimationComp == nullptr)
		return _vector();

	return m_pAnimationComp->Get_Root_TransDeltaVector();
}

_float3 CCommonModelComp::Get_RootTrans()
{
	if (m_pAnimationComp == nullptr)
		return _vector();

	return m_pAnimationComp->Get_Root_Trans();
}


_bool CCommonModelComp::IsAnimation_Finished()
{
	if (m_pAnimationComp == nullptr)
		return false;

	return m_pAnimationComp->IsAnimation_Finished();
}

_bool CCommonModelComp::IsAnimation_Finished(string strAnimName)
{
	if (m_pAnimationComp == nullptr)
		return false;

	return m_pAnimationComp->IsAnimation_Finished(StrToWstr(strAnimName));
}

_bool CCommonModelComp::IsAnimation_UpTo(_float m_fTrackPos)
{
	if (m_pAnimationComp == nullptr)
		return false;

	return m_pAnimationComp->IsAnimation_UpTo(m_fTrackPos);
}

_bool CCommonModelComp::IsAnimation_Frame_Once(_float m_fTrackPos)
{
	if (m_pAnimationComp == nullptr)
		return false;

	return m_pAnimationComp->IsAnimation_Frame_Once(m_fTrackPos);
}

_bool CCommonModelComp::IsAnimation_Range(_float fTrackMin, _float fTrackMax)
{
	if (m_pAnimationComp == nullptr)
		return false;

	return m_pAnimationComp->IsAnimation_Range(fTrackMin,fTrackMax);
}

_bool CCommonModelComp::IsAnimation_Transition() const
{
	if (m_pAnimationComp == nullptr)
		return false;

	return m_pAnimationComp->IsAnimation_Transition();
}

_bool CCommonModelComp::IsCurrentAnimation(string strAnimName)
{
	return m_pAnimationComp->IsCurrentAnimation(StrToWstr(strAnimName));
}

#pragma region 셰이더

HRESULT CCommonModelComp::Link_Shader(const wstring& strShaderKey, const D3D11_INPUT_ELEMENT_DESC* pElements, _uint iNumElements)
{
	if (m_pShaderComp == nullptr)
		RETURN_EFAIL;

	m_strEffectPath = strShaderKey;

	return m_pShaderComp->Link_Effect(strShaderKey, pElements, iNumElements);
}

HRESULT CCommonModelComp::Unlink_Shader()
{
	if (m_pShaderComp == nullptr)
		RETURN_EFAIL;

	return m_pShaderComp->Unlink_Effect();
}

HRESULT CCommonModelComp::Begin_Shader(_uint iPass)
{
	if (m_pShaderComp == nullptr)
		RETURN_EFAIL;

	return m_pShaderComp->Begin(iPass);
}

#pragma endregion


#pragma region 메쉬 활성화
void CCommonModelComp::Active_Mesh(_uint iIndex)
{
	if (iIndex < 0 || iIndex >= m_iNumMeshes)
		return;

	auto iter = find_if(m_ActiveMeshes.begin(), m_ActiveMeshes.end(),
		[&iIndex](const _uint iActive) {
			return iIndex == iActive;
		});
	if (iter != m_ActiveMeshes.end())
		return;

	m_ActiveMeshes.push_back(iIndex);
	++m_iNumActiveMeshes;
}

void CCommonModelComp::Active_AllMeshes()
{
	m_iNumActiveMeshes = 0;
	m_ActiveMeshes.clear();
	for (_uint i = 0; i < m_iNumMeshes; i++)
	{
		m_ActiveMeshes.push_back(i);
		++m_iNumActiveMeshes;
	}
}

void CCommonModelComp::Deactive_Mesh(_uint iIndex)
{
	if (iIndex < 0 || iIndex >= m_iNumMeshes)
		return;

	auto iter = find_if(m_ActiveMeshes.begin(), m_ActiveMeshes.end(),
		[&iIndex](const _uint iActive) {
			return iIndex == iActive;
		});
	if (iter == m_ActiveMeshes.end())
		return;

	m_ActiveMeshes.erase(iter);
	--m_iNumActiveMeshes;
}

void CCommonModelComp::Deactive_AllMeshes()
{
	m_iNumActiveMeshes = 0;
	m_ActiveMeshes.clear();
}

HRESULT CCommonModelComp::BindAndRender_Mesh(_uint iIndex)
{
	if (iIndex < 0 || iIndex >= m_iNumMeshes)
		RETURN_EFAIL;

	// 버퍼를 장치에 바인드
	m_pMeshComps[iIndex]->Bind_VIBuffer();

	// 바인딩된 정점, 인덱스 그리기
	m_pMeshComps[iIndex]->Render_VIBuffer();

	return S_OK;
}

#pragma endregion


HRESULT CCommonModelComp::Bind_MeshMaterialToShader(_uint iMeshIndex, TEXTURETYPE eType, const _char* pConstName)
{
	if (iMeshIndex < 0 || iMeshIndex >= m_iNumMeshes)
		RETURN_EFAIL;

	_uint iMatIndex = m_pMeshComps[iMeshIndex]->Get_MeshMaterialIndex();
	m_pMaterialComps[iMatIndex]->Bind_SRVToShader(m_pShaderComp.get(), pConstName, eType, 0);

	return S_OK;
}

_float4x4 CCommonModelComp::Get_BoneTransformFloat4x4(const wstring& strBoneName)
{
	if (!m_pSkeletalComp)
		return XMMatrixIdentity();

	_matrix CompMatrix = m_pSkeletalComp->Get_BoneTransformMatrix(strBoneName);

	_float4x4 ResultFloat4x4 = {};
	XMStoreFloat4x4(&ResultFloat4x4, CompMatrix);

	return ResultFloat4x4;
}

_matrix CCommonModelComp::Get_BoneTransformMatrix(const wstring& strBoneName)
{
	if (!m_pSkeletalComp)
		return XMMatrixIdentity();

	_matrix CompMatrix = m_pSkeletalComp->Get_BoneTransformMatrix(strBoneName);

	return CompMatrix;
}

_float4x4 CCommonModelComp::Get_BoneTransformFloat4x4WithParents(_uint iIndex)
{
	if (!m_pSkeletalComp)
		return _float4x4();

	_matrix CompMatrix = Calculate_TransformMatrixFromParent();
	CompMatrix = m_pSkeletalComp->Get_BoneTransformMatrix(iIndex) * CompMatrix;

	_float4x4 ResultFloat4x4 = {};
	XMStoreFloat4x4(&ResultFloat4x4, CompMatrix);

	return ResultFloat4x4;
}

_matrix CCommonModelComp::Get_BoneTransformMatrixWithParents(_uint iIndex)
{
	if (!m_pSkeletalComp)
		return _matrix();

	_matrix CompMatrix = Calculate_TransformMatrixFromParent();
	CompMatrix = m_pSkeletalComp->Get_BoneTransformMatrix(iIndex) * CompMatrix;

	return CompMatrix;
}

_matrix CCommonModelComp::Get_BoneTransformMatrixWithParents(const wstring& strBoneName)
{
	if (!m_pSkeletalComp)
		return _matrix();

	_matrix CompMatrix = Calculate_TransformMatrixFromParent();
	CompMatrix = m_pSkeletalComp->Get_BoneTransformMatrix(strBoneName) * CompMatrix;

	return CompMatrix;
}

CBoneGroup* CCommonModelComp::Get_BoneGroup() const
{
	if (nullptr == m_pSkeletalComp)
		return nullptr;

	return m_pSkeletalComp->Get_BoneGroup();
}

HRESULT CCommonModelComp::Bind_BoneToShader(_uint iMeshIndex, const _char* pBoneConstantName)
{
	if (m_pSkeletalComp.get() == nullptr 
		|| m_pSkeletalComp->Get_BoneGroup() == nullptr 
		|| iMeshIndex < 0 || iMeshIndex >= m_iNumMeshes)
		RETURN_EFAIL;

	return m_pMeshComps[iMeshIndex]->Bind_BoneMatricesToShader(m_pShaderComp.get(), pBoneConstantName, *m_pSkeletalComp->Get_BoneGroup());
}

void CCommonModelComp::Set_PreRotate(_matrix Matrix)
{
	if (m_pSkeletalComp == nullptr && m_pSkeletalComp->Get_BoneGroup() == nullptr)
		return;

	m_pSkeletalComp->Get_BoneGroup()->Set_PreRotate(Matrix);
}

void CCommonModelComp::Set_RootTransBone(_uint iIndex)
{
	if (m_pSkeletalComp == nullptr && m_pSkeletalComp->Get_BoneGroup() == nullptr)
		return;

	if (iIndex < 0 || iIndex >= m_pSkeletalComp->Get_BoneGroup()->CRef_BoneDatas_Count())
		return;

	m_pSkeletalComp->Get_BoneGroup()->Set_RootTransBoneInd(iIndex);
}

void CCommonModelComp::Set_RootTransBone(const wstring& strBoneName)
{
	if (m_pSkeletalComp == nullptr && m_pSkeletalComp->Get_BoneGroup() == nullptr)
		return;

	auto BoneData = m_pSkeletalComp->Get_BoneGroup()->Find_BoneData(strBoneName);
	if (BoneData == nullptr)
		return;

	m_pSkeletalComp->Get_BoneGroup()->Set_RootTransBoneInd(BoneData->iID);
}


HRESULT CCommonModelComp::Clone_NotifyFromPrototype(const wstring& strProtoTag)
{
	if (nullptr == m_pAnimationComp)
		RETURN_EFAIL;

	return m_pAnimationComp->Clone_NotifyFromPrototype(strProtoTag);
}

HRESULT CCommonModelComp::Regist_EventToNotify(const string& strAnimName, const string& strCollectionName, const string& strNotifyName, FastDelegate0<> Event)
{
	if (nullptr == m_pAnimationComp)
		RETURN_EFAIL;

	return m_pAnimationComp->Regist_EventToNotify(strAnimName, strCollectionName, strNotifyName, Event);
}

HRESULT CCommonModelComp::Regist_ModelCompToNotify(weak_ptr<CCommonModelComp> pModelComp)
{
	if (nullptr == m_pAnimationComp)
		RETURN_EFAIL;

	return m_pAnimationComp->Regist_ModelCompToNotify(pModelComp);
}
