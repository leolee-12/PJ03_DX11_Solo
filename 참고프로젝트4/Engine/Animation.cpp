//#include "Animation.h"
//#include "Channel.h"
//
//CAnimation::CAnimation()
//{
//}
//
//CAnimation::CAnimation(const CAnimation& rhs)
//	: m_fDuration(rhs.m_fDuration)
//	, m_fTickPerSecond(rhs.m_fTickPerSecond)
//	, m_fTrackPosition(rhs.m_fTrackPosition)
//	, m_iNumChannels(rhs.m_iNumChannels)
//	, m_Channels(rhs.m_Channels)
//	, m_isFinished(rhs.m_isFinished)
//	, m_CurrentKeyFrames(rhs.m_CurrentKeyFrames)
//	, m_iTrackFrameIndex(rhs.m_iTrackFrameIndex)
//	, m_fCorrectionValue(rhs.m_fCorrectionValue)
//{
//	strcpy_s(m_szName, rhs.m_szName);
//
//	for (auto& pChannel : m_Channels)
//		Safe_AddRef(pChannel);
//}
//
//
//#ifdef _DEBUG
//HRESULT CAnimation::Initialize(const aiAnimation* pAIAnimation, const CModel::BONES& Bones)
//{
//    strcpy_s(m_szName, pAIAnimation->mName.data);
//
//    m_fDuration = (_float)pAIAnimation->mDuration;
//    m_fTickPerSecond = (_float)pAIAnimation->mTicksPerSecond;
//    m_iNumChannels = pAIAnimation->mNumChannels;
//
//	m_CurrentKeyFrames.resize(m_iNumChannels);
//
//    /* 이 애니메이션에서 사용하기위한 뼈(aiNodeAnim,채널)의 정보를 만든다. */
//    for (_uint i = 0; i < m_iNumChannels; i++)
//    {
//        CChannel* pChannel = CChannel::Create(pAIAnimation->mChannels[i], Bones);
//        if (nullptr == pChannel)
//            RETURN_EFAIL;
//
//        m_Channels.push_back(pChannel);
//    }
//    return S_OK;
//}
//#endif
//
//
//HRESULT CAnimation::Initialize(ANIMATION_DATA AnimationData)
//{
//	strcpy_s(m_szName, AnimationData.szName.c_str());
//
//	m_fDuration = AnimationData.fDuration;
//	m_fTickPerSecond = AnimationData.fTickPerSecond;
//	m_iNumChannels = AnimationData.iNumChannels;
//
//	m_CurrentKeyFrames.resize(m_iNumChannels);
//
//	/* 이 애니메이션에서 사용하기위한 뼈(aiNodeAnim,채널)의 정보를 만든다. */
//	for (_uint i = 0; i < m_iNumChannels; i++)
//	{
//		CChannel* pChannel = CChannel::Create(AnimationData.Channels[i]);
//		if (nullptr == pChannel)
//			RETURN_EFAIL;
//
//		m_Channels.push_back(pChannel);
//	}
//	return S_OK;
//}
//
//void CAnimation::Invalidate_TransformationMatrix(_bool isLoop,_cref_time fTimeDelta, vector<class CBone*> Bones, CAnimation* pPrevAnimation)
//{
//    if (m_bIsChanged)
//        m_bIsChanged = !m_bIsChanged;
//
//    m_fTrackPosition += m_fTickPerSecond * fTimeDelta * m_fAnimSpeed * m_fAnimAdjustSpeed;
//    m_iTrackFrameIndex = static_cast<_uint>(m_fTrackPosition / m_fCorrectionValue);
//
//    if (pPrevAnimation != nullptr)
//    {
//        m_pPrevAnimation = pPrevAnimation;
//        m_pPrevChannels = pPrevAnimation->Get_Channels();
//        m_pPrevCurrentKeyFrames = pPrevAnimation->Get_CurrentKeyFrames();
//        m_bIsTransition = true;
//        m_bIsChanged = true;
//    }
//
//    if (m_bIsTransition && m_pPrevChannels != nullptr && m_pPrevCurrentKeyFrames != nullptr)
//    {
//        if (m_fTrackPosition <= m_fTransitionDuration && m_fTransitionDuration != 0.f)
//        {
//            Transition(fTimeDelta, Bones);
//            return;
//        }
//        else
//		{
//            Reset_Animation();
//            m_pPrevAnimation->Reset_Animation();
//            m_pPrevAnimation = nullptr;
//            m_bIsTransition = false;
//            m_pPrevChannels = nullptr;
//            m_pPrevCurrentKeyFrames = nullptr;
//            m_fAnimAdjustSpeed = 1.f;
//			return;
//        }
//    }
//    else if (m_fTrackPosition >= m_fDuration)
//    {
//       
//        if (true == isLoop)
//        {
//            m_fTrackPosition = 0.0f;
//            m_iTrackFrameIndex = 0;
//            m_isFinished = false;
//
//        }
//        else
//        {
//			m_isFinished = true;
//			m_fTrackPosition = m_fDuration;
//        }
//    }
//
//    /* 내 애니메이션이 이용하는 전체 뼈의 상태를 m_fTrackPosition 시간에 맞는 상태로 갱신하다.*/
//    for (_uint i = 0; i < m_iNumChannels; i++)
//    {
//        m_Channels[i]->Invalidate_TransformationMatrix(m_fTrackPosition,Bones, &m_CurrentKeyFrames[i]);
//    }
//}
//
//void CAnimation::Transition(_cref_time fTimeDelta, vector<class CBone*> Bones)
//{
//    for (_uint i = 0; i < m_iNumChannels; i++)
//	{
//        for (_uint j = 0; j < (*m_pPrevCurrentKeyFrames).size(); j++)
//        {			
//            if (!strcmp((*m_pPrevChannels)[j]->Get_Name(), m_Channels[i]->Get_Name()))
//            {
//                KEYFRAME PrevKeyFrame = (*m_pPrevChannels)[j]->Get_KeyFrame((*m_pPrevCurrentKeyFrames)[j]);
//			    m_Channels[i]->Transition(m_fTrackPosition, Bones, PrevKeyFrame,m_fTransitionDuration);
//            }
//        }
// 	}
//}
//
//void CAnimation::Reset_Animation()
//{
//    //m_fAnimSpeed = 1.f;
//	m_fTrackPosition = 0.0f;
//    m_iTrackFrameIndex = 0;
//	m_isFinished = false;
//    for (_uint i = 0; i < m_iNumChannels; i++)
//        m_CurrentKeyFrames[i] = 0;
//}
//
//#ifdef _DEBUG
//CAnimation* CAnimation::Create(const aiAnimation* pAIAnimation, const CModel::BONES& Bones)
//{
//    CAnimation* pInstance = new CAnimation();
//
//    if (FAILED(pInstance->Initialize(pAIAnimation, Bones)))
//    {
//        MSG_BOX("Failed to Created : CAnimation");
//        Safe_Release(pInstance);
//    }
//    return pInstance;
//}
//#endif
//
//CAnimation* CAnimation::Create(ANIMATION_DATA AnimationData)
//{
//	CAnimation* pInstance = new CAnimation();
//
//	if (FAILED(pInstance->Initialize(AnimationData)))
//	{
//		MSG_BOX("Failed to Created : CAnimation");
//		Safe_Release(pInstance);
//	}
//	return pInstance;
//}
//
//CAnimation* CAnimation::Clone()
//{
//	return new CAnimation(*this);
//}
//
//void CAnimation::Free()
//{
//    for (auto& pChannel : m_Channels)
//    {
//        Safe_Release(pChannel);
//    }
//    m_Channels.clear();
//}
