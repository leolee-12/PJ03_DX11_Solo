#pragma once

#include "stdafx.h"
#include "Effect_Animation.h"
#include "GameInstance.h"
#include "Effect_Animation.h"

CEffect_Animation::CEffect_Animation()
	: m_pGameInstance(CGameInstance::Get_Instance())
{
	Safe_AddRef(m_pGameInstance);
}

CEffect_Animation::CEffect_Animation(const CEffect_Animation& Prototype)
	: m_pGameInstance(CGameInstance::Get_Instance())
	, m_EffectKeyFrames{ Prototype.m_EffectKeyFrames }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CEffect_Animation::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CEffect_Animation::Initialize(void* pArg)
{
	return S_OK;
}

void CEffect_Animation::Add_KeyFrame(_uint KeyFrameNumber, EFFECT_KEYFRAME NewKeyFrameData)
{
	auto it = m_EffectKeyFrames.find(KeyFrameNumber);
	if (it != m_EffectKeyFrames.end()) {
		it->second = NewKeyFrameData;
	}
	else {
		m_EffectKeyFrames.emplace(KeyFrameNumber, NewKeyFrameData);
	}
}

void CEffect_Animation::Delete_KeyFrame(_uint KeyFrameNumber)
{
	auto it = m_EffectKeyFrames.find(KeyFrameNumber);

	if (it != m_EffectKeyFrames.end())
	{
		m_EffectKeyFrames.erase(it);
	}
}

_bool CEffect_Animation::Find_KeyFrame(_uint KeyFrameNumber)
{
	auto it = m_EffectKeyFrames.find(KeyFrameNumber);

	return it != m_EffectKeyFrames.end();
}

EFFECT_KEYFRAME CEffect_Animation::Get_KeyFrame(_uint KeyFrameNumber)
{
	auto it = m_EffectKeyFrames.find(KeyFrameNumber);

	if (it != m_EffectKeyFrames.end()) {
		return (it->second);
	}
	else {
		return EFFECT_KEYFRAME();
	}
}

EFFECT_KEYFRAME CEffect_Animation::Get_Near_Front_KeyFrame(_uint KeyFrameNumber)
{
	if (m_EffectKeyFrames.empty())
		return EFFECT_KEYFRAME();

	auto it = m_EffectKeyFrames.upper_bound(KeyFrameNumber);
	if (it == m_EffectKeyFrames.begin())
		return EFFECT_KEYFRAME();

	--it;
	return it->second;
}

EFFECT_KEYFRAME CEffect_Animation::Play_Animation(_float CurAnimPos, _bool bIsLoop)
{
	if (m_EffectKeyFrames.empty())
		return EFFECT_KEYFRAME();

	auto it1 = m_EffectKeyFrames.upper_bound(CurAnimPos);	// curpos 이상의 첫 번째 프레임
	auto it2 = (it1 == m_EffectKeyFrames.begin()) ? it1 : prev(it1);		//이전 프레임 없다면

	if (it1 == m_EffectKeyFrames.end())
	{
		if (bIsLoop)
		{
			CurAnimPos = fmod(CurAnimPos, std::prev(it1)->first);
			it1 = m_EffectKeyFrames.upper_bound(CurAnimPos);
			it2 = (it1 == m_EffectKeyFrames.begin()) ? it1 : std::prev(it1);
		}
		else
		{
			return std::prev(it1)->second;
		}
	}
	//여기 왜 터짐
	if (it1 == it2 || it1->first == CurAnimPos)
		return it1->second;

	const EFFECT_KEYFRAME& keyFrame1 = it2->second;
	const EFFECT_KEYFRAME& keyFrame2 = it1->second;

	float timeDiff = it1->first - it2->first;
	float factor = (CurAnimPos - it2->first) / timeDiff;

	EFFECT_KEYFRAME interpolatedKeyFrame;
	interpolatedKeyFrame.vPosition = Lerp(keyFrame1.vPosition, keyFrame2.vPosition, factor);
	interpolatedKeyFrame.vScale = Lerp(keyFrame1.vScale, keyFrame2.vScale, factor);
	interpolatedKeyFrame.vRotation = Lerp(keyFrame1.vRotation, keyFrame2.vRotation, factor);
	interpolatedKeyFrame.bIsNotPlaying = keyFrame1.bIsNotPlaying;

	return interpolatedKeyFrame;
}



_float3 CEffect_Animation::Lerp(const _float3& start, const _float3& end, _float factor)
{
	XMVECTOR vecStart = XMLoadFloat3(&start);
	XMVECTOR vecEnd = XMLoadFloat3(&end);

	XMVECTOR interpolatedVec = XMVectorLerp(vecStart, vecEnd, factor);

	_float3 result;
	XMStoreFloat3(&result, interpolatedVec);

	return result;
}

CEffect_Animation* CEffect_Animation::Create()
{
	CEffect_Animation* pInstance = new CEffect_Animation();

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CEffect_Animation"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CEffect_Animation* CEffect_Animation::Clone(void* pArg)
{
	CEffect_Animation* pInstance = new CEffect_Animation(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CEffect_Animation"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEffect_Animation::Free()
{
	__super::Free();

	m_EffectKeyFrames.clear();

	Safe_Release(m_pGameInstance);

}
