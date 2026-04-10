#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class CChannel final : public CBase
{
private:
	CChannel();
	virtual ~CChannel() = default;

public:
	HRESULT Initialize(const WMODEL_CHANNEL& tChannelData);
	void Update_TransformationMatrix(const vector<class CBone*>& Bones, _float fCurrentTrackPosition);

private:
	_uint m_iBoneIndex = {};
	_uint m_iNumKeyFrames = {};

	vector<SCALING_KEY> m_ScalingKeys;
	vector<ROTATION_KEY> m_RotationKeys;
	vector<POSITION_KEY> m_PositionKeys;
	_float3 m_DefaultScale{};
	_float4 m_DefaultRotation{};
	_float3 m_DefaultTranslation{};

private:
	_vector Interpolate_Scale(_float fCurrTrackPos) const;
	_vector Interpolate_Rotation(_float fCurrTrackPos) const;
	_vector Interpolate_Position(_float fCurrTrackPos) const;

public:
	static CChannel* Create(const WMODEL_CHANNEL& tChannelData);
	virtual void Free() override;
};

NS_END