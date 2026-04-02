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

private:
	_uint				m_iBoneIndex = {};
	_uint				m_iNumKeyFrames = {};
	vector<KEYFRAME>	m_KeyFrames;

public:
	static CChannel* Create(const aiNodeAnim* pAINodeAnim, class CModel* pModel);
	virtual void Free() override;
};

NS_END