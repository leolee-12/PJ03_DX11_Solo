#include "Channel.h"
#include "Model.h"

CChannel::CChannel()
{
}

HRESULT CChannel::Initialize(const aiNodeAnim* pAINodeAnim, class CModel* pModel)
{
    m_iBoneIndex = pModel->Get_BoneIndex(pAINodeAnim->mNodeName.data);

    m_iNumKeyFrames = max(pAINodeAnim->mNumScalingKeys, pAINodeAnim->mNumRotationKeys);
    m_iNumKeyFrames = max(m_iNumKeyFrames, pAINodeAnim->mNumPositionKeys);

    _float3         vScale{};
    _float4         vRotation{};
    _float3         vTranslation{};

    for (size_t i = 0; i < m_iNumKeyFrames; i++)
    {
        KEYFRAME            KeyFrame{};

        if (i < pAINodeAnim->mNumScalingKeys)       
        {
            memcpy(&vScale, &pAINodeAnim->mScalingKeys[i].mValue, sizeof(_float3));
            KeyFrame.fTrackPosition = static_cast<_float>(pAINodeAnim->mScalingKeys[i].mTime);
        }
        if (i < pAINodeAnim->mNumRotationKeys)
        {
            // memcpy(&vRotation, &pAINodeAnim->mRotationKeys[i].mValue, sizeof(_float4));
            vRotation.x = pAINodeAnim->mRotationKeys[i].mValue.x;
            vRotation.y = pAINodeAnim->mRotationKeys[i].mValue.y;
            vRotation.z = pAINodeAnim->mRotationKeys[i].mValue.z;
            vRotation.w = pAINodeAnim->mRotationKeys[i].mValue.w;
            KeyFrame.fTrackPosition = static_cast<_float>(pAINodeAnim->mRotationKeys[i].mTime);
        }
        if (i < pAINodeAnim->mNumPositionKeys)
        {
            memcpy(&vTranslation, &pAINodeAnim->mPositionKeys[i].mValue, sizeof(_float3));
            KeyFrame.fTrackPosition = static_cast<_float>(pAINodeAnim->mPositionKeys[i].mTime);
        }

        KeyFrame.vScale = vScale;
        KeyFrame.vRotation = vRotation;
        KeyFrame.vTranslation = vTranslation;

        m_KeyFrames.push_back(KeyFrame);
    }

	return S_OK;
}

void CChannel::Update_TransformationMatrix(const vector<class CBone*>& Bones, _float fTimeDelta)
{
}

CChannel* CChannel::Create(const aiNodeAnim* pAINodeAnim, class CModel* pModel)
{
    CChannel* pInstance = new CChannel();

    if (FAILED(pInstance->Initialize(pAINodeAnim, pModel)))
    {
        MSG_BOX("Failed to Created : CChannel");
        Safe_Release(pInstance);
    }

    return pInstance;
}


void CChannel::Free()
{
    __super::Free();


}
