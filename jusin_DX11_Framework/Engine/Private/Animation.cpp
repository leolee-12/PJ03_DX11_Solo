#include "Animation.h"
#include "Channel.h"


CAnimation::CAnimation()
{
}

CAnimation::CAnimation(const CAnimation& Prototype)
	: m_fDuration{ Prototype.m_fDuration }
	, m_fTicksPerSecond{ Prototype.m_fTicksPerSecond }
	, m_fCurrentTrackPosition{ Prototype.m_fCurrentTrackPosition }
	, m_iNumChannels{ Prototype.m_iNumChannels }
	, m_Channels{ Prototype.m_Channels }
	, m_ChanneledBoneIndices{ Prototype.m_ChanneledBoneIndices }

{
	strcpy_s(m_szName, Prototype.m_szName);

	for (auto& pChannel : m_Channels)
		Safe_AddRef(pChannel);
}

HRESULT CAnimation::Initialize(const WMODEL_ANIMATION& tAnimData)
{
	m_fDuration = tAnimData.fDuration;
	m_fTicksPerSecond = tAnimData.fTicksPerSecond;
	m_iNumChannels = static_cast<_uint>(tAnimData.channels.size());

	const vector<WMODEL_CHANNEL>& channelDatas = tAnimData.channels;

	for (size_t i = 0; i < m_iNumChannels; i++)
	{
		CChannel* pChannel = CChannel::Create(channelDatas[i]);
		if (nullptr == pChannel)
			return E_FAIL;

		m_Channels.push_back(pChannel);
		m_ChanneledBoneIndices.insert(pChannel->Get_BoneIndex());
	}

	return S_OK;
}

_bool CAnimation::Update_TransformationMatrices(const vector<class CBone*>& Bones, _float fTimeDelta, _bool isLoop)
{
	m_fCurrentTrackPosition += m_fTicksPerSecond * fTimeDelta;

	if (m_fCurrentTrackPosition >= m_fDuration)
	{
		if (false == isLoop)
			return true;

		m_fCurrentTrackPosition = fmodf(m_fCurrentTrackPosition, m_fDuration);
	}

	_uint iChannelIndex = {};

	for (auto& pChannel : m_Channels)
	{
		pChannel->Update_TransformationMatrix(Bones, m_fCurrentTrackPosition);
	}

	return false;
}

CAnimation* CAnimation::Create(const WMODEL_ANIMATION& tAnimData)
{
	CAnimation* pInstance = new CAnimation();

	if (FAILED(pInstance->Initialize(tAnimData)))
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