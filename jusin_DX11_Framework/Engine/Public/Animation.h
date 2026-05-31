#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class CAnimation final : public CBase
{
private:
	CAnimation();
	CAnimation(const CAnimation& Prototype);
	virtual ~CAnimation() = default;

public:
	const unordered_set<_uint>* Get_ChanneledBoneIndicesPtr() const { return &m_ChanneledBoneIndices; }
	_bool Has_Channel(_uint iBoneIdx) const { return (m_ChanneledBoneIndices.find(iBoneIdx) != m_ChanneledBoneIndices.end()); }
	_vector Reset_TrackPosition(_uint iRootBoneIdx);

	HRESULT Initialize(const WMODEL_ANIMATION& tAnimData);
	_uint Update_TransformationMatrices(const vector<class CBone*>& Bones, _float fTimeDelta, _bool isLoop);

private:
	_char m_szName[MAX_PATH] = {};
	_float m_fDuration = {};				/* 현재 애니메이션트랙 총 길이.  */
	_float m_fTicksPerSecond = {};			/* 현재 트랙의 초당 재생 속도. */
	_float m_fCurrentTrackPosition = {};	/* 현재 재생 위치. */

	_uint m_iNumChannels = {};				/* 현재 애니메이션의 재생을 위해 상태를 제어해야하는 뼈의 갯수 */
	vector<class CChannel*>	m_Channels;
	unordered_set<_uint> m_ChanneledBoneIndices;

public:
	static CAnimation* Create(const WMODEL_ANIMATION& tAnimData);
	CAnimation* Clone();
	virtual void Free() override;
};

NS_END