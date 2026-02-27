#include "..\Public\Animation.h"
#include "Channel.h"
#include "Bone.h"
#include "GameInstance.h"


CAnimation::CAnimation()
	: m_pGameInstance(CGameInstance::Get_Instance()) // 필요한 초기화 작업
{
	Safe_AddRef(m_pGameInstance);
}

CAnimation::CAnimation(const CAnimation& Prototype)
	:m_pGameInstance(CGameInstance::Get_Instance())
	, m_SoundEvents(Prototype.m_SoundEvents)
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CAnimation::Initialize(AnimationData& animationData, const vector<class CBone*>& Bones, vector<_uint>& KeyFrameIndices)
{
	strcpy_s(m_szName, animationData.name.c_str());
	m_fDuration = animationData.duration;
	m_fTickPerSecond = animationData.ticksPerSecond;
	m_iNumChannels = animationData.channels.size();

	KeyFrameIndices.resize(m_iNumChannels); // 채널 개수만큼 키프레임 인덱스를 초기화

	m_Channels.resize(m_iNumChannels);
	for (size_t i = 0; i < m_iNumChannels; ++i) {

		CChannel* pChannel = CChannel::Create(animationData.channels[i], Bones);

		if (nullptr == pChannel) {
			return E_FAIL;
		}
		m_Channels[i] = pChannel;
	}

	return S_OK;
}

_bool CAnimation::Update_TransformationMatrix(_float* pCurrentAnimPosition, _float fTimeDelta, const vector<class CBone*>& Bones, _bool isLoop, vector<_uint>& KeyFrameIndices)
{
	_bool bAnimationEnd = false;

	*pCurrentAnimPosition += m_fTickPerSecond * fTimeDelta;

	if (*pCurrentAnimPosition >= m_fDuration)
	{
		bAnimationEnd = true;

		*pCurrentAnimPosition = m_fDuration;
		if (isLoop)
		{
			*pCurrentAnimPosition = 0.f;
		}
	}

	for (size_t i = 0; i < m_iNumChannels; i++)
	{
		m_Channels[i]->Compute_TransformationMatrix(*pCurrentAnimPosition, Bones, &KeyFrameIndices[i]);
		//m_Channels[i]->Compute_TransformationMatrix_NoLinear(*pCurrentAnimPosition, Bones, &KeyFrameIndices[i]);
	}

	return bAnimationEnd;
}

void CAnimation::Compute_FirstKeyFrameMatrixForBone(const char* boneName, const vector<class CBone*>& Bones, _matrix* outMatrix)
{
	for (size_t i = 0; i < m_iNumChannels; ++i)
	{
		if (strcmp(boneName, m_Channels[i]->GetName()) == 0)
		{
			_uint keyFrameIndex = 0;
			_float position = 0.0f;
			m_Channels[i]->Compute_TransformationMatrix(position, Bones, &keyFrameIndex);

			*outMatrix = XMLoadFloat4x4(Bones[m_Channels[i]->GetBoneIndex()]->Get_TransformationMatrix());
			return;
		}
	}

	// 만약 본을 찾지 못하면 단위 행렬을 반환
	*outMatrix = XMMatrixIdentity();
}

void CAnimation::Add_SoundEvent(_float triggerTime, const wstring& soundAlias, _float volume)
{
	SoundEvent event;
	event.triggerTime = triggerTime;
	event.soundAlias = soundAlias;
	event.hasPlayed = false;
	event.volume = volume;
	m_SoundEvents.push_back(event);
}

void CAnimation::Update_FrameIndex(_float* pCurrentAnimPosition, vector<_uint>& KeyFrameIndices)
{
	for (size_t i = 0; i < m_iNumChannels; i++)
	{
		m_Channels[i]->Update_FrameIndex(*pCurrentAnimPosition, &KeyFrameIndices[i]);
	}
}

CChannel* CAnimation::Find_Channel_ByName2(const char* targetName)
{
	auto it = std::find_if(m_Channels.begin(), m_Channels.end(),
		[targetName](CChannel* channel) {
			return std::strcmp(channel->Get_Name(), targetName) == 0;
		});

	if (it != m_Channels.end()) {
		return *it;
	}
	else {
		return nullptr;
	}

}



CAnimation* CAnimation::Create(AnimationData animationData, const vector<class CBone*>& Bones, vector<_uint>& KeyFrameIndices)
{
	CAnimation* pInstance = new CAnimation();

	if (FAILED(pInstance->Initialize(animationData, Bones, KeyFrameIndices)))
	{
		MSG_BOX(TEXT("Failed to Created : CAnimation"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CAnimation::Free()
{
	Safe_Release(m_pGameInstance);

	__super::Free();

}
