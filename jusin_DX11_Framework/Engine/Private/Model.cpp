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
	, m_iNumMaterials{ Prototype.m_iNumMaterials }
	, m_Materials{ Prototype.m_Materials }
	//, m_Bones{ Prototype.m_Bones } 깊은복사
	, m_PreTransformMatrix{ Prototype.m_PreTransformMatrix }
	, m_iNumAnimations{ Prototype.m_iNumAnimations }
	//, m_Animations{ Prototype.m_Animations } 깊은복사
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
	XMStoreFloat4x4(&m_PreTransformMatrix, XMMatrixIdentity());

	if (FAILED(Ready_Bones(fp, tHeader.iNumBones)))
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
	_bool isFinished = { false };

	/* 현재 애니메이션 이용하고 있는 뼈들의 TransformationMatrix를 갱신해준다.  */
	isFinished = m_Animations[m_iCurrentAnimationIndex]->Update_TransformationMatrices(m_Bones, fTimeDelta, m_isAnimLoop);

	/* 위의 갱신이 끝났다면, 모든 뼈의 CombinedTransformationMatrix갱신한다. */
	for (auto& pBone : m_Bones)
	{
		pBone->Update_CombinedTransformMatrices(m_Bones, XMLoadFloat4x4(&m_PreTransformMatrix));
	}

	return isFinished;
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
