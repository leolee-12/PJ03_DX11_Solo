#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class CAnimation final : public CBase
{
private:
	CAnimation();
	CAnimation(const CAnimation& Prototype);;
	virtual ~CAnimation() = default;

public:
	HRESULT Initialize(const aiAnimation* pAIAnimation, const vector<class CBone*>& Bones);
	void Update_TransformationMatrix(_float fTimeDelta, const vector<CBone*>& Bones, _bool isLoop, _bool* pFinished);
	void Reset();
private:
	/* 이 애니메이션을 재생하는데 걸리는 총 시간(거리) */
	_float					m_fDuration = {};
	_float					m_fCurrentTrackPosition = {};
	_float					m_fTickPerSecond = {};

	_uint					m_iNumChannels = {};
	vector<class CChannel*> m_Channels;
	vector<_uint>			m_CurrentKeyFrameIndex = {};


public:
	static CAnimation* Create(const aiAnimation* pAIAnimation, const vector<class CBone*>& Bones);
	CAnimation* Clone();
	virtual void Free() override;

};

NS_END