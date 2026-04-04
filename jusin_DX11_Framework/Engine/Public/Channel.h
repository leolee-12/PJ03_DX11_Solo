#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class CChannel final : public CBase
{
private:
	CChannel();
	virtual ~CChannel() = default;

public:
	HRESULT Initialize(const aiNodeAnim* pAINodeAnim, class CModel* pModel);
	HRESULT Initialize(const WMODEL_CHANNEL& tChannelData);
	void Update_TransformationMatrix(const vector<class CBone*>& Bones, _float fCurrentTrackPosition, _uint* pCurrentKeyIndex);

private:
	_uint m_iBoneIndex = {};
	_uint m_iNumKeyFrames = {};
	vector<KEYFRAME> m_KeyFrames;

public:
	static CChannel* Create(const aiNodeAnim* pAINodeAnim, class CModel* pModel);
	static CChannel* Create(const WMODEL_CHANNEL& tChannelData);
	virtual void Free() override;
};

NS_END