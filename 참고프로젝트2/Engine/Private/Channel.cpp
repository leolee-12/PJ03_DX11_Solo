#include "..\Public\Channel.h"

#include "Bone.h"

CChannel::CChannel()
{
}

HRESULT CChannel::Initialize(ChannelData& channelData, const vector<class CBone*>& Bones)
{
	strcpy_s(m_szName, channelData.name.c_str());
	m_iNumKeyFrames = channelData.keyframes.size();

	// 이름을 기반으로 뼈 인덱스를 찾습니다.
	m_iBoneIndex = -1; // 기본값을 -1로 설정
	auto iter = find_if(Bones.begin(), Bones.end(), [&](CBone* pBone) {
		return strcmp(m_szName, pBone->Get_Name()) == 0;
		});

	if (iter != Bones.end()) {
		m_iBoneIndex = std::distance(Bones.begin(), iter); // 인덱스를 저장
	}
	else {
		// 뼈를 찾지 못했을 때의 처리 (필요한 경우)
		OutputDebugStringA("WARNING:: Bone not found for channel\n");
	}

	m_KeyFrames.resize(m_iNumKeyFrames);
	for (size_t i = 0; i < m_iNumKeyFrames; ++i) {
		m_KeyFrames[i] = channelData.keyframes[i];
	}

	return S_OK;
}

void CChannel::Compute_TransformationMatrix(_float fCurrentPosition, const vector<CBone*>& Bones, _uint* pKeyFrameIndex)
{
	if (0.f == fCurrentPosition)
		(*pKeyFrameIndex)= 0;

	_matrix		TransformationMatrix = XMMatrixIdentity();

	KEYFRAME	LastKeyFrame = m_KeyFrames.back();

	_vector		vScale{}, vRotation{}, vPosition{};

	/* 마지막 키프레임의 상태를 가지낟. */
	if (fCurrentPosition >= LastKeyFrame.fTime)
	{
		vScale = XMLoadFloat3(&LastKeyFrame.vScale);
		vRotation = XMLoadFloat4(&LastKeyFrame.vRotation);
		vPosition = XMVectorSetW(XMLoadFloat3(&LastKeyFrame.vPosition), 1.f);
	}
	/* 왼쪽키프레임와 오른쪽 키프레임사이의 보간된 값을 구하낟. */
	else
	{
		while (fCurrentPosition >= m_KeyFrames[(*pKeyFrameIndex)+ 1].fTime)
			++(*pKeyFrameIndex);

		_float		fRatio = (fCurrentPosition - m_KeyFrames[(*pKeyFrameIndex)].fTime) /
			(m_KeyFrames[(*pKeyFrameIndex)+ 1].fTime - m_KeyFrames[(*pKeyFrameIndex)].fTime);

		_vector		vSourScale, vDestScale;
		_vector		vSourRotation, vDestRotation;
		_vector		vSourPosition, vDestPosition;

		vSourScale = XMLoadFloat3(&m_KeyFrames[(*pKeyFrameIndex)].vScale);
		vSourRotation = XMLoadFloat4(&m_KeyFrames[(*pKeyFrameIndex)].vRotation);
		vSourPosition = XMVectorSetW(XMLoadFloat3(&m_KeyFrames[(*pKeyFrameIndex)].vPosition), 1.f);

		vDestScale = XMLoadFloat3(&m_KeyFrames[(*pKeyFrameIndex)+ 1].vScale);
		vDestRotation = XMLoadFloat4(&m_KeyFrames[(*pKeyFrameIndex)+ 1].vRotation);
		vDestPosition = XMVectorSetW(XMLoadFloat3(&m_KeyFrames[(*pKeyFrameIndex)+ 1].vPosition), 1.f);
		
		vScale = XMVectorLerp(vSourScale, vDestScale, fRatio);
		vRotation = XMQuaternionSlerp(vSourRotation, vDestRotation, fRatio);
		vPosition = XMVectorLerp(vSourPosition, vDestPosition, fRatio);
	}

	TransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vPosition);
	Bones[m_iBoneIndex]->SetUp_TransformationMatrix(TransformationMatrix);
}

void CChannel::Compute_TransformationMatrix_NoLinear(_float fCurrentPosition, const vector<class CBone*>& Bones, _uint* pKeyFrameIndex)
{
	if (0.f == fCurrentPosition)
		(*pKeyFrameIndex) = 0;

	_matrix		TransformationMatrix = XMMatrixIdentity();

	KEYFRAME	LastKeyFrame = m_KeyFrames.back();

	_vector		vScale{}, vRotation{}, vPosition{};

	/* 마지막 키프레임의 상태를 가지낟. */
	if (fCurrentPosition >= LastKeyFrame.fTime)
	{
		vScale = XMLoadFloat3(&LastKeyFrame.vScale);
		vRotation = XMLoadFloat4(&LastKeyFrame.vRotation);
		vPosition = XMVectorSetW(XMLoadFloat3(&LastKeyFrame.vPosition), 1.f);
	}
	/* 왼쪽키프레임와 오른쪽 키프레임사이의 보간된 값을 구하낟. */
	else
	{
		while (fCurrentPosition >= m_KeyFrames[(*pKeyFrameIndex) + 1].fTime)
			++(*pKeyFrameIndex);

		_float		fRatio = (fCurrentPosition - m_KeyFrames[(*pKeyFrameIndex)].fTime) /
			(m_KeyFrames[(*pKeyFrameIndex) + 1].fTime - m_KeyFrames[(*pKeyFrameIndex)].fTime);

		_vector		vSourScale, vDestScale;
		_vector		vSourRotation, vDestRotation;
		_vector		vSourPosition, vDestPosition;

		vSourScale = XMLoadFloat3(&m_KeyFrames[(*pKeyFrameIndex)].vScale);
		vSourRotation = XMLoadFloat4(&m_KeyFrames[(*pKeyFrameIndex)].vRotation);
		vSourPosition = XMVectorSetW(XMLoadFloat3(&m_KeyFrames[(*pKeyFrameIndex)].vPosition), 1.f);

		//vDestScale = XMLoadFloat3(&m_KeyFrames[(*pKeyFrameIndex) + 1].vScale);
		//vDestRotation = XMLoadFloat4(&m_KeyFrames[(*pKeyFrameIndex) + 1].vRotation);
		//vDestPosition = XMVectorSetW(XMLoadFloat3(&m_KeyFrames[(*pKeyFrameIndex) + 1].vPosition), 1.f);


		vScale = vSourScale;
		vRotation = vSourRotation;
		vPosition = vSourPosition;
	}

	TransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vPosition);
	Bones[m_iBoneIndex]->SetUp_TransformationMatrix(TransformationMatrix);
}

void CChannel::Update_FrameIndex(_float fCurrentPosition, _uint* pKeyFrameIndex)
{
	*pKeyFrameIndex = 0;

	while ((*pKeyFrameIndex + 1 < m_KeyFrames.size()) && fCurrentPosition >= m_KeyFrames[(*pKeyFrameIndex) + 1].fTime)
		++(*pKeyFrameIndex);

}


CChannel* CChannel::Create(ChannelData& chanelData, const vector<class CBone*>& Bones)
{
	CChannel* pInstance = new CChannel();

	if (FAILED(pInstance->Initialize(chanelData, Bones)))
	{
		MSG_BOX(TEXT("Failed to Created : CChannel"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CChannel::Free()
{
	__super::Free();

}

