#include "Channel.h"
#include "Model.h"
#include "Bone.h"

CChannel::CChannel()
{
}

HRESULT CChannel::Initialize(const WMODEL_CHANNEL& tChannelData)
{
	m_iBoneIndex = tChannelData.iBoneIndex;
	m_ScalingKeys = tChannelData.scalingKeys;
	m_RotationKeys = tChannelData.rotationKeys;
	m_PositionKeys = tChannelData.positionKeys;

	m_DefaultScale = tChannelData.vDefaultScale;
	m_DefaultRotation = tChannelData.vDefaultRotation;
	m_DefaultTranslation = tChannelData.vDefaultTranslation;

	return S_OK;
}

void CChannel::Update_TransformationMatrix(const vector<class CBone*>& Bones, _float fCurrentTrackPosition)
{
	_vector vScale = Interpolate_Scale(fCurrentTrackPosition);
	_vector vRotation = Interpolate_Rotation(fCurrentTrackPosition);
	_vector vTranslation = Interpolate_Position(fCurrentTrackPosition);

	_matrix TransformationMatrix =
		XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vTranslation);

	Bones[m_iBoneIndex]->Set_TransformationMatrix(TransformationMatrix);
}

_vector CChannel::Interpolate_Scale(_float fCurrTrackPos) const
{
	// 비어있으면 Default값으로 fallback
	if (m_ScalingKeys.empty())
		return XMLoadFloat3(&m_DefaultScale);

	// 키프레임이 하나이거나, 첫 번째 칸이라면 첫 번째 키프레임 사용
	if (m_ScalingKeys.size() == 1 || fCurrTrackPos <= m_ScalingKeys[0].fTrackPosition)
		return XMLoadFloat3(&m_ScalingKeys[0].vScale);

	// 마지막 키프레임 이후라면 마지막 키프레임 사용
	if (fCurrTrackPos >= m_ScalingKeys.back().fTrackPosition)
		return XMLoadFloat3(&m_ScalingKeys.back().vScale);

	// 앞뒤 키프레임을 이용하여 보간
	_uint idx = Find_KeyIndex(m_ScalingKeys, fCurrTrackPos);
	const SCALING_KEY& tPrevKey = m_ScalingKeys[idx];
	const SCALING_KEY& tCurrKey = m_ScalingKeys[idx + 1];

	// 분모가 0인 경우에 대한 fallback
	_float fDenom = tCurrKey.fTrackPosition - tPrevKey.fTrackPosition;
	if (fabsf(fDenom) < 1e-6f)
		return XMLoadFloat3(&tCurrKey.vScale);

	_float fRatio = (fCurrTrackPos - tPrevKey.fTrackPosition) / fDenom;
	return XMVectorLerp(XMLoadFloat3(&tPrevKey.vScale), XMLoadFloat3(&tCurrKey.vScale), fRatio);
}

_vector CChannel::Interpolate_Rotation(_float fCurrTrackPos) const
{
	// 비어있으면 Default값으로 fallback
	if (m_RotationKeys.empty())
		return XMLoadFloat4(&m_DefaultRotation);

	// 키프레임이 하나이거나, 첫 번째 칸이라면 첫 번째 키프레임 사용
	if (m_RotationKeys.size() == 1 || fCurrTrackPos <= m_RotationKeys[0].fTrackPosition)
		return XMLoadFloat4(&m_RotationKeys[0].vRotation);

	// 마지막 키프레임 이후라면 마지막 키프레임 사용
	if (fCurrTrackPos >= m_RotationKeys.back().fTrackPosition)
		return XMLoadFloat4(&m_RotationKeys.back().vRotation);

	// 앞뒤 키프레임을 이용하여 보간
	_uint idx = Find_KeyIndex(m_RotationKeys, fCurrTrackPos);
	const ROTATION_KEY& tPrevKey = m_RotationKeys[idx];
	const ROTATION_KEY& tCurrKey = m_RotationKeys[idx + 1];

	// 분모가 0인 경우에 대한 fallback
	_float fDenom = tCurrKey.fTrackPosition - tPrevKey.fTrackPosition;
	if (fabsf(fDenom) < 1e-6f)
		return XMLoadFloat4(&tCurrKey.vRotation);

	_float fRatio = (fCurrTrackPos - tPrevKey.fTrackPosition) / fDenom;

	_vector vPrevQuat = XMLoadFloat4(&tPrevKey.vRotation);
	_vector vCurrQuat = XMLoadFloat4(&tCurrKey.vRotation);

	// 회전의 경우 같은 반구인지 체크(전처리와 함께 더블 체크)
	if (XMVectorGetX(XMVector4Dot(vPrevQuat, vCurrQuat)) < 0.f)
		vCurrQuat = XMVectorNegate(vCurrQuat);

	// 쿼터니언은 Slerp or Nlerp
	return XMQuaternionNormalize(XMQuaternionSlerp(vPrevQuat, vCurrQuat, fRatio));
}

_vector CChannel::Interpolate_Position(_float fCurrTrackPos) const
{
	// 비어있으면 Default값으로 fallback
	if (m_PositionKeys.empty())
		return XMVectorSetW(XMLoadFloat3(&m_DefaultTranslation), 1.f);

	// 키프레임이 하나이거나, 첫 번째 칸이라면 첫 번째 키프레임 사용
	if (m_PositionKeys.size() == 1 || fCurrTrackPos <= m_PositionKeys[0].fTrackPosition)
		return XMVectorSetW(XMLoadFloat3(&m_PositionKeys[0].vTranslation), 1.f);

	// 마지막 키프레임 이후라면 마지막 키프레임 사용
	if (fCurrTrackPos >= m_PositionKeys.back().fTrackPosition)
		return XMVectorSetW(XMLoadFloat3(&m_PositionKeys.back().vTranslation), 1.f);

	// 앞뒤 키프레임을 이용하여 보간
	_uint idx = Find_KeyIndex(m_PositionKeys, fCurrTrackPos);
	const POSITION_KEY& tPrevKey = m_PositionKeys[idx];
	const POSITION_KEY& tCurrKey = m_PositionKeys[idx + 1];

	// 분모가 0인 경우에 대한 fallback
	_float fDenom = tCurrKey.fTrackPosition - tPrevKey.fTrackPosition;
	if (fabsf(fDenom) < 1e-6f)
		return XMLoadFloat3(&tCurrKey.vTranslation);

	_float fRatio = (fCurrTrackPos - tPrevKey.fTrackPosition) / fDenom;

	// 이동의 경우, 벡터의 w값 채워주기
	_vector vPrevPos = XMVectorSetW(XMLoadFloat3(&tPrevKey.vTranslation), 1.f);
	_vector vCurrPos = XMVectorSetW(XMLoadFloat3(&tCurrKey.vTranslation), 1.f);

	return XMVectorLerp(vPrevPos, vCurrPos, fRatio);
}

CChannel* CChannel::Create(const WMODEL_CHANNEL& tChannelData)
{
	CChannel* pInstance = new CChannel();

	if (FAILED(pInstance->Initialize(tChannelData)))
	{
		MSG_BOX("Failed to Created : CChannel");
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CChannel::Free()
{
	__super::Free();
}
