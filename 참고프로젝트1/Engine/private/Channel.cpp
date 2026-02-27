#include "Channel.h"
#include "Bone.h"

CChannel::CChannel()
{
}

HRESULT CChannel::Initialize(const aiNodeAnim* pAIChannel, const vector<class CBone*>& Bones)
{
    strcpy_s(m_szName, pAIChannel->mNodeName.data);
    

    auto    iter = find_if(Bones.begin(), Bones.end(), [&](CBone* pBone)->_bool {
        if (true == pBone->Compare_Name(m_szName))
            return true;

        ++m_iBoneIndex;

        return false;
    });

    if (iter == Bones.end())
        return E_FAIL;


    m_iNumKeyFrames = max(pAIChannel->mNumPositionKeys, pAIChannel->mNumScalingKeys);
    m_iNumKeyFrames = max(m_iNumKeyFrames, pAIChannel->mNumRotationKeys);

    _float3         vScale = {};
    _float4         vRotation = {};
    _float3         vTranslation = {};

    for (size_t i = 0; i < m_iNumKeyFrames; i++)
    {
        KEYFRAME            KeyFrame{};

        if(i < pAIChannel->mNumScalingKeys)
        {
            memcpy(&vScale, &pAIChannel->mScalingKeys[i].mValue, sizeof(_float3));
            KeyFrame.fTrackPosition = pAIChannel->mScalingKeys[i].mTime;
        }

        if (i < pAIChannel->mNumRotationKeys)
        {
            vRotation.x = pAIChannel->mRotationKeys[i].mValue.x;
            vRotation.y = pAIChannel->mRotationKeys[i].mValue.y;
            vRotation.z = pAIChannel->mRotationKeys[i].mValue.z;
            vRotation.w = pAIChannel->mRotationKeys[i].mValue.w;        
            KeyFrame.fTrackPosition = pAIChannel->mRotationKeys[i].mTime;
        }

        if (i < pAIChannel->mNumPositionKeys)
        {
            memcpy(&vTranslation, &pAIChannel->mPositionKeys[i].mValue, sizeof(_float3));
            KeyFrame.fTrackPosition = pAIChannel->mPositionKeys[i].mTime;
        }

        KeyFrame.vScale = vScale;
        KeyFrame.vRotation = vRotation;
        KeyFrame.vTranslation = vTranslation;        

        m_KeyFrames.push_back(KeyFrame);
    }


    return S_OK;
}

void CChannel::Update_TransformationMatrix(_float fCurrentTrackPosition, const vector<CBone*>& Bones, _uint* pCurrentKeyFrameIndex)
{    
    if (0.0f == fCurrentTrackPosition)
        (*pCurrentKeyFrameIndex) = 0;

    _vector     vScale, vRotation, vTranslation;

    KEYFRAME    LastKeyFrame = m_KeyFrames.back();

    if (fCurrentTrackPosition >= LastKeyFrame.fTrackPosition)
    {
        vScale = XMLoadFloat3(&LastKeyFrame.vScale);
        vRotation = XMLoadFloat4(&LastKeyFrame.vRotation);
        vTranslation = XMVectorSetW(XMLoadFloat3(&LastKeyFrame.vTranslation), 1.f);
    }

    else
    {   
        while(fCurrentTrackPosition >= m_KeyFrames[(*pCurrentKeyFrameIndex) + 1].fTrackPosition)
            ++(*pCurrentKeyFrameIndex);

        _vector     vSourScale, vDestScale;
        _vector     vSourRotation, vDestRotation;
        _vector     vSourTranslation, vDestTranslation;        

        vSourScale = XMLoadFloat3(&m_KeyFrames[(*pCurrentKeyFrameIndex)].vScale);
        vSourRotation = XMLoadFloat4(&m_KeyFrames[(*pCurrentKeyFrameIndex)].vRotation);
        vSourTranslation = XMVectorSetW(XMLoadFloat3(&m_KeyFrames[(*pCurrentKeyFrameIndex)].vTranslation), 1.f);

        vDestScale = XMLoadFloat3(&m_KeyFrames[(*pCurrentKeyFrameIndex) + 1].vScale);
        vDestRotation = XMLoadFloat4(&m_KeyFrames[(*pCurrentKeyFrameIndex) + 1].vRotation);
        vDestTranslation = XMVectorSetW(XMLoadFloat3(&m_KeyFrames[(*pCurrentKeyFrameIndex) + 1].vTranslation), 1.f);

        _float      fRatio = (fCurrentTrackPosition - m_KeyFrames[(*pCurrentKeyFrameIndex)].fTrackPosition) /
            (m_KeyFrames[(*pCurrentKeyFrameIndex) + 1].fTrackPosition - m_KeyFrames[(*pCurrentKeyFrameIndex)].fTrackPosition);                

        vScale = XMVectorLerp(vSourScale, vDestScale, fRatio);
        vRotation = XMQuaternionSlerp(vSourRotation, vDestRotation, fRatio);
        vTranslation = XMVectorLerp(vSourTranslation, vDestTranslation, fRatio);
    }

    _matrix     TransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vTranslation);

    Bones[m_iBoneIndex]->Set_TransformationMatrix(TransformationMatrix);
}

CChannel* CChannel::Create(const aiNodeAnim* pAIChannel, const vector<class CBone*>& Bones)
{
    CChannel* pInstance = new CChannel();

    if (FAILED(pInstance->Initialize(pAIChannel, Bones)))
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
