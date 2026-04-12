#include "Model.h"
#include "Mesh.h"
#include "Material.h"
#include "Bone.h"
#include "Animation.h"

CModel::CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _char* pModelFilePath)
	: CComponent{ pDevice, pContext }
	, m_pModelFilePath{ pModelFilePath }
{
}

CModel::CModel(const CModel& Prototype)
	: CComponent{ Prototype }
	, m_eType{ Prototype.m_eType }
	, m_iNumMeshes{ Prototype.m_iNumMeshes }
	, m_Meshes{ Prototype.m_Meshes }
	, m_iNumBones{ Prototype.m_iNumBones }
	, m_iNumMaterials{ Prototype.m_iNumMaterials }
	, m_Materials{ Prototype.m_Materials }
	//, m_Bones{ Prototype.m_Bones } 깊은복사
	, m_iNumAnimations{ Prototype.m_iNumAnimations }
	//, m_Animations{ Prototype.m_Animations } 깊은복사
	, m_iRootBoneIndex{ Prototype.m_iRootBoneIndex }
{
	for (auto& pPrototypeAnimation : Prototype.m_Animations)
		m_Animations.push_back(pPrototypeAnimation->Clone());

	for (auto& pPrototypeBone : Prototype.m_Bones)
		m_Bones.push_back(pPrototypeBone->Clone());

	for (auto& pMaterial : m_Materials)
		Safe_AddRef(pMaterial);

	for (auto& pMesh : m_Meshes)
		Safe_AddRef(pMesh);
}

_uint CModel::Get_MeshMaterialIndex(_uint iMeshIdx)
{
	return Get_Mesh(iMeshIdx)->Get_MaterialIndex();
}

_int CModel::Get_BoneIndex(const _char* pBoneName)
{
	_int iIndex = { -1 };

	auto iter = find_if(m_Bones.begin(), m_Bones.end(), [&](CBone* pBone)->_bool
		{
			++iIndex;

			if (true == pBone->Compare_Name(pBoneName))
				return true;
			
			return false;
		});

	if (iter == m_Bones.end())
		return -1;

	return iIndex;
}

const _float4x4* CModel::Get_BoneMatrixPtr(const _char* pBoneName) const
{
	auto iter = find_if(m_Bones.begin(), m_Bones.end(), [&](CBone* pBone)->_bool
		{
			if (true == pBone->Compare_Name(pBoneName))
				return true;

			return false;
		});

	if (iter == m_Bones.end())
		return nullptr;

	return (*iter)->Get_CombinedTransformationMatrixPtr();
}

void CModel::Set_AnimationIndex(_uint iIndex, _bool isLoop, _float fBlendDuration)
{
	if (m_iCurrentAnimationIndex == iIndex) return;

	if (1e-6f > fBlendDuration)
	{
		m_iCurrentAnimationIndex = iIndex;
		m_isAnimLoop = isLoop;

		if (m_bEnableRootMotion)
		{
			XMStoreFloat3(&m_vPrevRootPos,
				m_Animations[iIndex]->Reset_TrackPosition(m_iRootBoneIndex));
			m_vPrevRootPos.y = 0.f;
		}

		return;
	}

	// 1. 벡터 준비
	m_BlendSnapshots.resize(m_Bones.size()); // resize : 크기만 조절
	m_BlendTargetMask.assign(m_Bones.size(), false); // assign : size 및 초기값 재설정

	// 2. 새 애니메이션의 채널 본 집합 캐싱
	m_pNextChanneledSet = m_Animations[iIndex]->Get_ChanneledBoneIndicesPtr();

	// 3. 스냅샷 캡처
	if (m_isBlending) // 3-1. 재전환(전환 중 다시 전환)
	{
		for (_uint i = 0; i < m_iNumBones; ++i)
		{
			_bool wasBlending = m_BlendTargetMask[i];	// 이전 블렌드에서 보간 중이었는 지
			_bool isInNext = { false };					// 다음 애니메이션에 속하는 지

			if (m_pNextChanneledSet->find(i) != m_pNextChanneledSet->end())
				isInNext = true;

			if (!wasBlending && !isInNext)
			{	// 둘 다 아니면 보간할 필요 없음
				m_BlendTargetMask[i] = false;
				continue;
			}

			m_BlendTargetMask[i] = true;
			m_Bones[i]->Decompose_Transformation(m_BlendSnapshots[i]);
		}
	}
	else
	{	// 3-2. 일반 전환
		const unordered_set<_uint>* pPrevChanneledSet = m_Animations[m_iCurrentAnimationIndex]->Get_ChanneledBoneIndicesPtr();

		for (_uint i = 0; i < m_iNumBones; ++i)
		{
			_bool isInPrev = { false };	// 이전 애니메이션에 속하는 지
			_bool isInNext = { false };	// 다음 애니메이션에 속하는 지

			if (pPrevChanneledSet->find(i) != pPrevChanneledSet->end())
				isInPrev = true;

			if (m_pNextChanneledSet->find(i) != m_pNextChanneledSet->end())
				isInNext = true;

			if (!isInPrev && !isInNext)
			{	// 둘 다 아니면 보간할 필요 없음
				m_BlendTargetMask[i] = false;
				continue;
			}

			m_BlendTargetMask[i] = true;

			if (isInPrev)	// 이전 애니메이션이 사용했다면 Transformation(AnimatedLocal)을 스냅샷
				m_Bones[i]->Decompose_Transformation(m_BlendSnapshots[i]);
			else			// 이전 애니메이션이 사용하지 않았다면 BindPose를 스냅샷
				m_Bones[i]->Decompose_BindPose(m_BlendSnapshots[i]);

		}
	}

	// 4. 블렌드 상태로 전환
	m_isBlending = true;
	m_fBlendDuration = fBlendDuration;
	m_fBlendElapsed = 0.f;

	m_iCurrentAnimationIndex = iIndex;
	m_isAnimLoop = isLoop;
	XMStoreFloat3(&m_vPrevRootPos, m_Animations[m_iCurrentAnimationIndex]->Reset_TrackPosition(m_iRootBoneIndex));
	m_vPrevRootPos.y = 0.f;
}

HRESULT CModel::Initialize_Prototype()
{
	// 0. 파일 열기
	FILE* fp{};
	errno_t errorOpen{};
	if (0 != fopen_s(&fp, m_pModelFilePath.c_str(), "rb") || nullptr == fp)
		return E_FAIL;

	// 1. 헤더 읽기 + 검증
	WMODEL_HEADER tHeader{};
	fread(&tHeader, sizeof(WMODEL_HEADER), 1, fp);

	if (memcmp(tHeader.szMagic, "WMDL", 4) != 0) { fclose(fp); return E_FAIL; }
	if (tHeader.iVersion != 2) { fclose(fp); return E_FAIL; }
	m_eType = static_cast<MODEL>(tHeader.iModelType);

	m_iNumBones = tHeader.iNumBones;
	if (FAILED(Ready_Bones(fp, m_iNumBones)))
	{
		fclose(fp); return E_FAIL;
	}

	if (FAILED(Ready_Materials(fp, tHeader.iNumMaterials)))
	{
		fclose(fp); return E_FAIL;
	}

	if (FAILED(Ready_Meshes(fp, tHeader.iNumMeshes)))
	{
		fclose(fp); return E_FAIL;
	}

	if (m_eType == MODEL::ANIM)
	{
		if (FAILED(Ready_Animations(fp, tHeader.iNumAnimations)))
		{
			fclose(fp); return E_FAIL;
		}
	}

	// 파일 닫기
	fclose(fp);

	return S_OK;
}

HRESULT CModel::Initialize(void* pArg)
{
	return S_OK;
}

_bool CModel::Play_Animation(_float fTimeDelta)
{
	_bool isAnimFinished = { false };

	// 1. 애니메이션 갱신 : 시간 전진, 채널 갱신
	isAnimFinished = m_Animations[m_iCurrentAnimationIndex]->Update_TransformationMatrices(m_Bones, fTimeDelta, m_isAnimLoop);

	// 3. 루트 모션 추출
	if (m_bEnableRootMotion)
	{
		// 3-1. 현재 RootPos 추출
		const _float4x4& rootMat = m_Bones[m_iRootBoneIndex]->Get_TransformationMatrix();
		_float3 vCurrRootPos = { rootMat._41, 0.f, rootMat._43 };

		// 3-2. 델타 계산 (현재 - 이전)
		m_vRootMotionDelta.x = vCurrRootPos.x - m_vPrevRootPos.x;
		m_vRootMotionDelta.y = vCurrRootPos.y - m_vPrevRootPos.y;
		m_vRootMotionDelta.z = vCurrRootPos.z - m_vPrevRootPos.z;

		// 3-3. 이전 프레임 갱신 및 루트본 로컬 이동 제거
		m_vPrevRootPos = vCurrRootPos;
		m_Bones[m_iRootBoneIndex]->Zero_TranslationXZ();
	}

	// 2. 블렌드 상태인 경우 블렌드 로직 수행
	if (m_isBlending)
		Update_Blend(fTimeDelta);

	// 4. Combined 행렬 갱신
	for (auto& pBone : m_Bones)
		pBone->Update_CombinedTransformMatrices(m_Bones);

	// 5. 반환 값 결정 : 블렌드 중이면 Anim종료 보류
	if (m_isBlending)
		return false;

	return isAnimFinished;
}

void CModel::Update_Blend(_float fTimeDelta)
{
	m_fBlendElapsed += fTimeDelta;

	_float fRatio = (m_fBlendDuration > 0.f) ? clamp(m_fBlendElapsed / m_fBlendDuration, 0.f, 1.f) : 1.f;

	// 선형 보간 대신 ease 등 적용 가능
	// fRatio = fRatio * fRatio * (3.f - 2.f * fRatio);	// smoothstep

	for (_uint i = 0; i < m_iNumBones; ++i)
	{
		if (!m_BlendTargetMask[i])
			continue;

		// 2-1. 블렌드 타겟 설정
		BONE_SRT tTargetSRT;
		auto iter = m_pNextChanneledSet->find(i);
		if (iter != m_pNextChanneledSet->end())	// 다음 애니메이션에서 사용될 본인 경우
			m_Bones[i]->Decompose_Transformation(tTargetSRT);
		else									// 다음 애니메이션에서 사용되지 않을 본인 경우
			m_Bones[i]->Decompose_BindPose(tTargetSRT);

		// 2-2. 블렌드 소스 설정
		BONE_SRT& tSourceSRT = m_BlendSnapshots[i];

		// 2-3. 보간 수행
		_vector vScale = XMVectorLerp(XMLoadFloat3(&tSourceSRT.vScale), XMLoadFloat3(&tTargetSRT.vScale), fRatio);

		_vector vSrcQuat = XMLoadFloat4(&tSourceSRT.vRotation);
		_vector vTgtQuat = XMLoadFloat4(&tTargetSRT.vRotation);
		if (XMVectorGetX(XMVector4Dot(vSrcQuat, vTgtQuat)) < 0.f)
			vTgtQuat = XMVectorNegate(vTgtQuat);
		_vector vRotation = XMQuaternionNormalize(XMQuaternionSlerp(vSrcQuat, vTgtQuat, fRatio));

		_vector vTranslation = XMVectorLerp(XMLoadFloat3(&tSourceSRT.vTranslation), XMLoadFloat3(&tTargetSRT.vTranslation), fRatio);

		// 2-4. 본에 기록
		m_Bones[i]->Set_TransformationMatrix(XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vTranslation));
	}

	// 2-5. 시간 초과 시 블렌드 상태 종료
	if (fRatio >= 1.f)
	{
		m_isBlending = false;
		m_pNextChanneledSet = nullptr;
	}
}

HRESULT CModel::Render(_uint iMeshIndex)
{
	if (iMeshIndex >= m_iNumMeshes)
		return E_FAIL;

	if (FAILED(m_Meshes[iMeshIndex]->Bind_Resources()))
		return E_FAIL;

	if (FAILED(m_Meshes[iMeshIndex]->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CModel::Bind_Material(CShader* pShader, const _char* pConstantName, _uint iMeshIndex, MATERIAL_TYPE eType, _uint iIndex)
{
	if (iMeshIndex >= m_iNumMeshes)
		return E_FAIL;

	_uint iMaterialIndex = m_Meshes[iMeshIndex]->Get_MaterialIndex();
	if (iMaterialIndex >= m_iNumMaterials)
		return E_FAIL;

	return m_Materials[iMaterialIndex]->Bind_ShaderResource(pShader, pConstantName, eType, iIndex);
}

HRESULT CModel::Bind_BoneMatrices(CShader* pShader, const _char* pConstName, _uint iMeshIndex)
{
	if (iMeshIndex >= m_iNumMeshes)
		return E_FAIL;

	return m_Meshes[iMeshIndex]->Bind_BoneMatrices(pShader, pConstName, m_Bones);
}

HRESULT CModel::Ready_Meshes(FILE* fp, _uint iNumMeshes)
{
	m_iNumMeshes = iNumMeshes;
	m_Meshes.reserve(iNumMeshes);

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		WMODEL_MESH tMesh{};

		fread(tMesh.szName, 1, MAX_PATH, fp);
		fread(&tMesh.iMaterialIndex, sizeof(_uint), 1, fp);

		_uint numVerts, numIdx, numBones;
		fread(&numVerts, sizeof(_uint), 1, fp);
		fread(&numIdx, sizeof(_uint), 1, fp);
		fread(&numBones, sizeof(_uint), 1, fp);

		if (MODEL::NONANIM == m_eType)
		{
			tMesh.nonAnimVertices.resize(numVerts);
			fread(tMesh.nonAnimVertices.data(), sizeof(VTXMESH), numVerts, fp);
		}
		else
		{
			tMesh.boneIndices.resize(numBones);
			fread(tMesh.boneIndices.data(), sizeof(_uint), numBones, fp);

			tMesh.offsetMatrices.resize(numBones);
			fread(tMesh.offsetMatrices.data(), sizeof(_float4x4), numBones, fp);

			tMesh.animVertices.resize(numVerts);
			fread(tMesh.animVertices.data(), sizeof(VTXANIMMESH), numVerts, fp);
		}

		tMesh.indices.resize(numIdx);
		fread(tMesh.indices.data(), sizeof(_uint), numIdx, fp);

		CMesh* pMesh = CMesh::Create(m_pDevice, m_pContext, m_eType, tMesh);
		if (!pMesh) return E_FAIL;
		m_Meshes.push_back(pMesh);
	}
	return S_OK;
}

HRESULT CModel::Ready_Materials(FILE* fp, _uint iNumMaterials)
{
	// 베이스 디렉터리
	_string baseDir = filesystem::path(m_pModelFilePath).parent_path().string() + "/";

	m_iNumMaterials = iNumMaterials;
	m_Materials.reserve(iNumMaterials);

	for (_uint i = 0; i < iNumMaterials; ++i)
	{
		WMODEL_MATERIAL tMat{};

		for (_uint j = 0; j < ETOUI(MATERIAL_TYPE::END); ++j)
		{
			_uint numTex = 0;
			fread(&numTex, sizeof(_uint), 1, fp);

			for (_uint k = 0; k < numTex; ++k)
			{
				_uint len = 0;
				fread(&len, sizeof(_uint), 1, fp);

				string path(len, '\0');
				fread(path.data(), 1, len, fp);
				path.resize(strlen(path.c_str()));  // null 제거
				tMat.TexturePaths[j].push_back(path);
			}
		}

		CMaterial* pMaterial = CMaterial::Create(m_pDevice, m_pContext, tMat, baseDir.c_str());
		if (!pMaterial) return E_FAIL;
		m_Materials.push_back(pMaterial);
	}
	return S_OK;
}

HRESULT CModel::Ready_Bones(FILE* fp, _uint iNumBones)
{
	vector<WMODEL_BONE> bones(iNumBones);
	fread(bones.data(), sizeof(WMODEL_BONE), iNumBones, fp);

	m_Bones.reserve(iNumBones);
	for (auto& tBone : bones)
	{
		CBone* pBone = CBone::Create(tBone);
		if (!pBone) return E_FAIL;
		m_Bones.push_back(pBone);
	}
	return S_OK;
}

HRESULT CModel::Ready_Animations(FILE* fp, _uint iNumAnimations)
{
	m_iNumAnimations = iNumAnimations;
	m_Animations.reserve(iNumAnimations);

	for (_uint i = 0; i < iNumAnimations; ++i)
	{
		WMODEL_ANIMATION tAnim{};
		_uint numChannels{};

		fread(tAnim.szName, 1, MAX_PATH, fp);
		fread(&tAnim.fDuration, sizeof(_float), 1, fp);
		fread(&tAnim.fTicksPerSecond, sizeof(_float), 1, fp);
		fread(&numChannels, sizeof(_uint), 1, fp);
		tAnim.channels.resize(numChannels);

		for (_uint j = 0; j < numChannels; ++j)
		{
			WMODEL_CHANNEL& tChannel = tAnim.channels[j];

			_uint numScalingKeys{};
			_uint numRotationKeys{};
			_uint numPositionKeys{};

			fread(&tChannel.iBoneIndex, sizeof(_uint), 1, fp);
			fread(&tChannel.vDefaultScale, sizeof(_float3), 1, fp);
			fread(&tChannel.vDefaultRotation, sizeof(_float4), 1, fp);
			fread(&tChannel.vDefaultTranslation, sizeof(_float3), 1, fp);
			fread(&numScalingKeys, sizeof(_uint), 1, fp);
			fread(&numRotationKeys, sizeof(_uint), 1, fp);
			fread(&numPositionKeys, sizeof(_uint), 1, fp);

			tChannel.scalingKeys.resize(numScalingKeys);
			tChannel.rotationKeys.resize(numRotationKeys);
			tChannel.positionKeys.resize(numPositionKeys);

			if (numScalingKeys > 0)
				fread(tChannel.scalingKeys.data(), sizeof(SCALING_KEY), numScalingKeys, fp);

			if (numRotationKeys > 0)
				fread(tChannel.rotationKeys.data(), sizeof(ROTATION_KEY), numRotationKeys, fp);

			if (numPositionKeys > 0)
				fread(tChannel.positionKeys.data(), sizeof(POSITION_KEY), numPositionKeys, fp);
		}

		CAnimation* pAnim = CAnimation::Create(tAnim);
		if (!pAnim) return E_FAIL;
		m_Animations.push_back(pAnim);
	}
	return S_OK;
}

CModel* CModel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _char* pModelFilePath)
{
	CModel* pInstance = new CModel(pDevice, pContext, pModelFilePath);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CModel");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CModel* CModel::Create_FromData(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODEL eType, const vector<CBone*> bones, vector<CMesh*>& meshes, vector<CMaterial*>& materials, vector<CAnimation*>& animations)
{
	CModel* pInstance = new CModel(pDevice, pContext);

	pInstance->m_eType = eType;

	pInstance->m_Meshes = meshes;
	pInstance->m_Materials = materials;
	pInstance->m_Bones = bones;
	pInstance->m_Animations = animations;

	pInstance->m_iNumMeshes = static_cast<_uint>(meshes.size());
	pInstance->m_iNumMaterials = static_cast<_uint>(materials.size());
	pInstance->m_iNumAnimations = static_cast<_uint>(animations.size());

	return pInstance;
}
 
CComponent* CModel::Clone(void* pArg)
{
	CModel* pInstance = new CModel(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CModel");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CModel::Free()
{
	__super::Free();
	for (auto& pAnimation : m_Animations)
		Safe_Release(pAnimation);
	m_Animations.clear();

	for (auto& pBone : m_Bones)
		Safe_Release(pBone);
	m_Bones.clear();

	for (auto& pMaterial : m_Materials)
		Safe_Release(pMaterial);
	m_Materials.clear();

	for (auto& pMesh : m_Meshes)
		Safe_Release(pMesh);
	m_Meshes.clear();
}
