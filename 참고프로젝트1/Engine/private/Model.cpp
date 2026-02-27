#include "Model.h"

#include "Mesh.h"
#include "Bone.h"
#include "Material.h"
#include "Animation.h"

CModel::CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent { pDevice, pContext }
{

}

CModel::CModel(const CModel& Prototype)
	: CComponent{ Prototype }
    , m_iNumMeshes { Prototype.m_iNumMeshes }
    , m_Meshes { Prototype.m_Meshes }
    , m_iNumMaterials { Prototype.m_iNumMaterials }
    , m_Materials { Prototype.m_Materials }   
    , m_PreTransformMatrix { Prototype.m_PreTransformMatrix }
    , m_iNumAnimations { Prototype.m_iNumAnimations }
    
{
    for (auto& pPrototypeAnim : Prototype.m_Animations)
        m_Animations.push_back(pPrototypeAnim->Clone());

    for (auto& pPrototypeBone : Prototype.m_Bones)
        m_Bones.push_back(pPrototypeBone->Clone());

    /*for (auto& pBone : m_Bones)
        Safe_AddRef(pBone);*/

    for (auto& pMesh : m_Meshes)
        Safe_AddRef(pMesh);

    for (auto& pMaterial : m_Materials)
        Safe_AddRef(pMaterial);

}

const _float4x4* CModel::Get_SocketBoneMatrix_Ptr(const _char* pBoneName) const
{
    auto    iter = find_if(m_Bones.begin(), m_Bones.end(), [&](CBone* pBone)->_bool {
        if (true == pBone->Compare_Name(pBoneName))
            return true; 
        return false;
    });

    if (iter == m_Bones.end())
        return nullptr;

    return (*iter)->Get_CombinedTransformationMatrix_Ptr();
}

HRESULT CModel::Initialize_Prototype(MODELTYPE eType, const _char* pModelFilePath, _fmatrix PreTransformMatrix)
{
    m_eType = eType;

    // aiProcess_PreTransformVertices : 모델의 정점에게 필요한 사전변환을 적용한다. 

    unsigned int        iFlag = {  aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast };

    if (MODELTYPE::NONANIM == m_eType)
        iFlag |= aiProcess_PreTransformVertices;

    m_pAIScene = m_Importer.ReadFile(pModelFilePath, iFlag);
    if (nullptr == m_pAIScene)
        return E_FAIL;    

    XMStoreFloat4x4(&m_PreTransformMatrix, PreTransformMatrix);

    if (FAILED(Ready_Bones(m_pAIScene->mRootNode, -1)))
        return E_FAIL;

    if (FAILED(Ready_Meshes()))
        return E_FAIL;

    if (FAILED(Ready_Materials(pModelFilePath)))
        return E_FAIL;

    if (FAILED(Ready_Animations()))
        return E_FAIL;

	return S_OK;
}

HRESULT CModel::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CModel::Bind_ShaderResource(_uint iMeshIndex, CShader* pShader, const _char* pConstantName, aiTextureType eType, _uint iIndex)
{
    _uint       iMaterialIndex = m_Meshes[iMeshIndex]->Get_MaterialIndex();
    if (iMaterialIndex >= m_iNumMaterials)
        return E_FAIL;

    return m_Materials[iMaterialIndex]->Bind_ShaderResource(pShader, pConstantName, eType, iIndex);    
}

HRESULT CModel::Bind_BoneMatrices(_uint iMeshIndex, CShader* pShader, const _char* pConstantName)
{
    if (iMeshIndex >= m_iNumMeshes)
        return E_FAIL;

    return m_Meshes[iMeshIndex]->Bind_BoneMatrices(pShader, pConstantName, m_Bones);    
}

void CModel::Play_Animation(_float fTimeDelta)
{
    /* 현재 애니메이션의 현재 재생시간에 맞는 상태로 뼈들을 갱신해 준다.(m_TransformationMatrix) */
    m_Animations[m_iCurrentAnimIndex]->Update_TransformationMatrix(fTimeDelta, m_Bones, m_isAnimLoop, &m_isAnimFinished);

    /* 뼈들의 갱신된 m_TransformationMatrix를 기반으로 하여 CombinedTransformationMatrix를 만들어준다. */
    for (auto& pBone : m_Bones)
    {
        pBone->Update_CombinedTransformationMatrix(m_Bones, XMLoadFloat4x4(&m_PreTransformMatrix));
    }
}

void CModel::Set_Animation(_uint iIndex, _bool isLoop)
{
    if (m_iCurrentAnimIndex == iIndex && m_isAnimLoop == isLoop)
        return;

    m_iCurrentAnimIndex = iIndex;
    m_isAnimLoop = isLoop;

    m_Animations[iIndex]->Reset();
}

HRESULT CModel::Render(_uint iMeshIndex)
{
    m_Meshes[iMeshIndex]->Bind_Buffers();
    m_Meshes[iMeshIndex]->Render();

    return S_OK;
}

HRESULT CModel::Ready_Meshes()
{
    m_iNumMeshes = m_pAIScene->mNumMeshes;

    for (size_t i = 0; i < m_iNumMeshes; i++)
    {
        CMesh* pMesh = CMesh::Create(m_pDevice, m_pContext, m_eType, m_pAIScene->mMeshes[i], m_Bones, XMLoadFloat4x4(&m_PreTransformMatrix));
        if (nullptr == pMesh)
            return E_FAIL;

        m_Meshes.push_back(pMesh);
    }

    return S_OK;
}

HRESULT CModel::Ready_Materials(const _char* pModelFilePath)
{
    m_iNumMaterials = m_pAIScene->mNumMaterials;

    for (size_t i = 0; i < m_iNumMaterials; i++)
    {
        CMaterial* pMaterial = CMaterial::Create(m_pDevice, m_pContext, pModelFilePath, m_pAIScene->mMaterials[i]);
        if (nullptr == pMaterial)
            return E_FAIL;

        m_Materials.push_back(pMaterial);
    }

    return S_OK;
}

HRESULT CModel::Ready_Bones(const aiNode* pAINode, _int iParentIndex)
{
    CBone* pBone = CBone::Create(pAINode, iParentIndex);
    if (nullptr == pBone)
        return E_FAIL;

    m_Bones.push_back(pBone);

    _int iPIndex = m_Bones.size() - 1;

    for (_uint i = 0; i < pAINode->mNumChildren; ++i)
    {
        Ready_Bones(pAINode->mChildren[i], iPIndex);
    }    
 
    return S_OK;
}

HRESULT CModel::Ready_Animations()
{
    m_iNumAnimations = m_pAIScene->mNumAnimations;

    for (size_t i = 0; i < m_iNumAnimations; i++)
    {
        CAnimation* pAnimation = CAnimation::Create(m_pAIScene->mAnimations[i], m_Bones);
        if (nullptr == pAnimation)
            return E_FAIL;

        m_Animations.push_back(pAnimation);
    }

    return S_OK;
}

CModel* CModel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODELTYPE eType, const _char* pModelFilePath, _fmatrix PreTransformMatrix)
{
    CModel* pInstance = new CModel(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype(eType, pModelFilePath, PreTransformMatrix)))
    {
        MSG_BOX("Failed to Created : CModel");
        Safe_Release(pInstance);
    }

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

    m_Importer.FreeScene();


}
