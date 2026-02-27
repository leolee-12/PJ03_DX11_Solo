#pragma once

#include "Base.h"

/* 특정 애니메이션이 컨트롤해야할 뼈. */
/* 재생위치별 표현해야할 뼈의 상태를 보관한다. */
/* 현재 재생위치에 맞는 상태행렬을 보간하여 생성한다. 생성한 상태는 뼈에게로. */

NS_BEGIN(Engine)

class CChannel final : public CBase
{
private:
	CChannel();
	virtual ~CChannel() = default;
public:
	HRESULT Initialize(const aiNodeAnim* pAIChannel, const vector<class CBone*>& Bones);
	void Update_TransformationMatrix(_float fCurrentTrackPosition, const vector<CBone*>& Bones, _uint* pCurrentKeyFrameIndex);
	
private:
	_char				m_szName[MAX_PATH];
	_uint				m_iNumKeyFrames = {};
	vector<KEYFRAME>	m_KeyFrames;

	_uint				m_iBoneIndex = {};


public:
	static CChannel* Create(const aiNodeAnim* pAIChannel, const vector<class CBone*>& Bones);
	virtual void Free() override;
};

NS_END