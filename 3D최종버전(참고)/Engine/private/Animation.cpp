#include "Animation.h"
#include "Channel.h"

CAnimation::CAnimation()
{
}

CAnimation::CAnimation(const CAnimation& Prototype)
    : m_fDuration{ Prototype.m_fDuration }
    , m_fCurrentTrackPosition { Prototype.m_fCurrentTrackPosition }
    , m_fTickPerSecond{ Prototype.m_fTickPerSecond }
    , m_iNumChannels{ Prototype.m_iNumChannels }
    , m_Channels{ Prototype.m_Channels }
    , m_CurrentKeyFrameIndex { Prototype.m_CurrentKeyFrameIndex }
{
    for (auto& pChannel : m_Channels)
        Safe_AddRef(pChannel);
}

HRESULT CAnimation::Initialize(const aiAnimation* pAIAnimation, const vector<class CBone*>& Bones)
{
    m_iNumChannels = pAIAnimation->mNumChannels;
    m_fDuration = pAIAnimation->mDuration;
    m_fTickPerSecond = pAIAnimation->mTicksPerSecond;

    m_CurrentKeyFrameIndex.resize(m_iNumChannels);

    for (size_t i = 0; i < m_iNumChannels; i++)
    {
        CChannel* pChannel = CChannel::Create(pAIAnimation->mChannels[i], Bones);
        if (nullptr == pChannel)
            return E_FAIL;

        m_Channels.push_back(pChannel);
    }   

    return S_OK;
}

void CAnimation::Update_TransformationMatrix(_float fTimeDelta, const vector<CBone*>& Bones, _bool isLoop, _bool* pFinished)
{
    /* 현재 내 애니메이션의 재생 위치를 계산해 준다. */
    m_fCurrentTrackPosition += m_fTickPerSecond * fTimeDelta;

    if (m_fCurrentTrackPosition >= m_fDuration)
    {
        if (false == isLoop)
        {
            *pFinished = true;
            return;
        }
    
        m_fCurrentTrackPosition = 0.f;

    
    }         

    /* 이 애니메이션이 컨트롤하는 모든 뼈를 다 순회하면서 해당 뼈의 상태를 키프레임기반으로 보간하여 생성해주고 */
    /* 위에서 생성 행렬을 채널과 이름이 같은 본에게 전달해 준다 .*/

    _uint       iIndex = {};

    for (auto& pChannel : m_Channels)
    {
        
        pChannel->Update_TransformationMatrix(m_fCurrentTrackPosition, Bones, &m_CurrentKeyFrameIndex[iIndex++]);
    }
}

void CAnimation::Reset()
{
    m_fCurrentTrackPosition = 0.f;

    for (auto& KeyFrameIndex : m_CurrentKeyFrameIndex)    
        KeyFrameIndex = 0;
}

CAnimation* CAnimation::Create(const aiAnimation* pAIAnimation, const vector<class CBone*>& Bones)
{
    CAnimation* pInstance = new CAnimation();

    if (FAILED(pInstance->Initialize(pAIAnimation, Bones)))
    {
        MSG_BOX("Failed to Created : CAnimation");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CAnimation* CAnimation::Clone()
{
    return new CAnimation(*this);
}


void CAnimation::Free()
{
    __super::Free();

    for (auto& pChannel : m_Channels)
        Safe_Release(pChannel);

    m_Channels.clear();

}
