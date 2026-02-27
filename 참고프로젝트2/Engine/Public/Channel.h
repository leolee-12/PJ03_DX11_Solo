#pragma once

#include "Base.h"

/* 채널(aiNodeAnim) : 하나의 뼈 정보, 이 뼈가 특정 시간대에 어떤 상태를 띄어야 하는가? */
/* 즉, 채널은 시간에 따른 상태(키프레임)값들을 가진다. */

BEGIN(Engine)

class CChannel final : public CBase
{
private:
	CChannel();
	virtual ~CChannel() = default;

public:
	HRESULT Initialize(ChannelData& chanelData, const vector<class CBone*>& Bones);

	void Compute_TransformationMatrix(_float fCurrentPosition, const vector<class CBone*>& Bones, _uint* pKeyFrameIndex);
	void Compute_TransformationMatrix_NoLinear(_float fCurrentPosition, const vector<class CBone*>& Bones, _uint* pKeyFrameIndex);

	_char* GetName() { return m_szName; };
	_int GetBoneIndex() { return m_iBoneIndex; };
	void Update_FrameIndex(_float fCurrentPosition, _uint* pKeyFrameIndex);

	const _char* Get_Name()
	{
		return m_szName;
	}

	
public:
	_char				m_szName[MAX_PATH] = {};

	_uint				m_iBoneIndex = {};
	_uint				m_iNumKeyFrames = {};
	vector<KEYFRAME>	m_KeyFrames;

public:
	static CChannel* Create(ChannelData& chanelData, const vector<class CBone*>& Bones);

	virtual void Free() override;
};

END