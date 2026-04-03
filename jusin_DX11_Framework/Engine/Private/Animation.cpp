#include "Animation.h"
#include "Channel.h"


CAnimation::CAnimation()
{
}

CAnimation::CAnimation(const CAnimation& Prototype)
	: m_fDuration{ Prototype.m_fDuration }
	, m_fTickPerSecond{ Prototype.m_fTickPerSecond }
	, m_fCurrentTrackPosition{ Prototype.m_fCurrentTrackPosition }
	, m_iNumChannels{ Prototype.m_iNumChannels }
	, m_Channels{ Prototype.m_Channels }
	, m_CurrentKeyFrameIndices{ Prototype.m_CurrentKeyFrameIndices }

{
	for (auto& pChannel : m_Channels)
		Safe_AddRef(pChannel);
}

HRESULT CAnimation::Initialize(const aiAnimation* pAIAnimation, class CModel* pModel)
{
	m_fDuration = static_cast<_float>(pAIAnimation->mDuration);
	m_fTickPerSecond = static_cast<_float>(pAIAnimation->mTicksPerSecond);

	m_iNumChannels = pAIAnimation->mNumChannels;
	m_CurrentKeyFrameIndices.resize(m_iNumChannels);

	for (size_t i = 0; i < m_iNumChannels; i++)
	{
		CChannel* pChannel = CChannel::Create(pAIAnimation->mChannels[i], pModel);
		if (nullptr == pChannel)
			return E_FAIL;

		m_Channels.push_back(pChannel);
	}

	return S_OK;
}

_bool CAnimation::Update_TransformationMatrices(const vector<class CBone*>& Bones, _float fTimeDelta, _bool isLoop)
{
	m_fCurrentTrackPosition += m_fTickPerSecond * fTimeDelta;

	if (m_fCurrentTrackPosition >= m_fDuration)
	{
		if (false == isLoop)
			return true;

		m_fCurrentTrackPosition = 0.f;
	}

	_uint iChannelIndex = {};

	for (auto& pChannel : m_Channels)
	{
		pChannel->Update_TransformationMatrix(Bones, m_fCurrentTrackPosition, &m_CurrentKeyFrameIndices[iChannelIndex++]);
	}

	return false;
}

CAnimation* CAnimation::Create(const aiAnimation* pAIAnimation, class CModel* pModel)
{
	CAnimation* pInstance = new CAnimation();

	if (FAILED(pInstance->Initialize(pAIAnimation, pModel)))
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

	for (auto pChannel : m_Channels)
		Safe_Release(pChannel);
	m_Channels.clear();
}