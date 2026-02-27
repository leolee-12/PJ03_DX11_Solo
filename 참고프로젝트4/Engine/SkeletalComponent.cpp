#include "SkeletalComponent.h"

#include "GameInstance.h"

#include "Model_Manager.h"
#include "GameInstance.h"
#include "BoneContainer.h"


IMPLEMENT_CLONE(CSkeletalComponent, CComponent)
IMPLEMENT_CREATE(CSkeletalComponent)

CSkeletalComponent::CSkeletalComponent(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    : Base(pDevice, pContext)
{
    
}

CSkeletalComponent::CSkeletalComponent(const CSkeletalComponent& rhs)
    : Base(rhs)
{
    m_pBoneGroup = rhs.m_pBoneGroup->Clone();
}

HRESULT CSkeletalComponent::Initialize_Prototype()
{

    return S_OK;
}

HRESULT CSkeletalComponent::Initialize(void* Arg)
{
    return S_OK;
}

void CSkeletalComponent::Free()
{
    __super::Free();

    Safe_Release(m_pBoneGroup);
}

HRESULT CSkeletalComponent::Load_Skeletal(const wstring& strModelFilePath)
{
    if (nullptr == m_pGameInstance)
        RETURN_EFAIL;

    CBoneGroup* pBoneGroup = m_pGameInstance->Clone_BoneGroup(strModelFilePath);
    if (nullptr == pBoneGroup)
        RETURN_EFAIL;

    m_pBoneGroup = pBoneGroup;

    return S_OK;
}

CBoneGroup* CSkeletalComponent::Get_BoneGroup()
{
    return m_pBoneGroup;
}

void CSkeletalComponent::Invalidate_BoneTransforms()
{
    if (!m_pBoneGroup)
        return;

    m_pBoneGroup->Invalidate_FinalTransforms();


}

_float4x4 CSkeletalComponent::Get_BoneTransformFloat4x4(_uint iIndex)
{
    if (!m_pBoneGroup)
        return _float4x4();

    if (iIndex < 0 || iIndex >= m_pBoneGroup->CRef_BoneDatas_Count())
        return _float4x4();

    return m_pBoneGroup->CRef_BoneCombinedTransforms()[iIndex];
}

_matrix CSkeletalComponent::Get_BoneTransformMatrix(_uint iIndex)
{
    if (!m_pBoneGroup)
        return XMMatrixIdentity();

    if (iIndex < 0 || iIndex >= m_pBoneGroup->CRef_BoneDatas_Count())
        return _float4x4();

    return m_pBoneGroup->CRef_BoneCombinedTransforms()[iIndex];
}

_matrix CSkeletalComponent::Get_BoneTransformMatrix(const wstring& strBoneName)
{
    if (!m_pBoneGroup)
        return XMMatrixIdentity();

    _uint iIndex = m_pBoneGroup->Find_BoneData(strBoneName)->iID;
    if (iIndex < 0 || iIndex >= m_pBoneGroup->CRef_BoneDatas_Count())
        return _float4x4();

    return m_pBoneGroup->CRef_BoneCombinedTransforms()[iIndex];
}
