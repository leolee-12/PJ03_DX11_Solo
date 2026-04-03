#include "Channel.h"
#include "Model.h"
#include "Bone.h"

CChannel::CChannel()
{
}

HRESULT CChannel::Initialize(const aiNodeAnim* pAINodeAnim, class CModel* pModel)
{
	m_iBoneIndex = pModel->Get_BoneIndex(pAINodeAnim->mNodeName.data);

	m_iNumKeyFrames = max(pAINodeAnim->mNumScalingKeys, pAINodeAnim->mNumRotationKeys);
	m_iNumKeyFrames = max(m_iNumKeyFrames, pAINodeAnim->mNumPositionKeys);

	_float3 vScale{};
	_float4 vRotation{};
	_float3 vTranslation{};

	for (size_t i = 0; i < m_iNumKeyFrames; i++)
	{
		KEYFRAME KeyFrame{};

		if (i < pAINodeAnim->mNumScalingKeys)
		{
			memcpy(&vScale, &pAINodeAnim->mScalingKeys[i].mValue, sizeof(_float3));
			KeyFrame.fTrackPosition = static_cast<_float>(pAINodeAnim->mScalingKeys[i].mTime);
		}
		if (i < pAINodeAnim->mNumRotationKeys)
		{
			vRotation.x = pAINodeAnim->mRotationKeys[i].mValue.x;
			vRotation.y = pAINodeAnim->mRotationKeys[i].mValue.y;
			vRotation.z = pAINodeAnim->mRotationKeys[i].mValue.z;
			vRotation.w = pAINodeAnim->mRotationKeys[i].mValue.w;
			KeyFrame.fTrackPosition = static_cast<_float>(pAINodeAnim->mRotationKeys[i].mTime);
		}
		if (i < pAINodeAnim->mNumPositionKeys)
		{
			memcpy(&vTranslation, &pAINodeAnim->mPositionKeys[i].mValue, sizeof(_float3));
			KeyFrame.fTrackPosition = static_cast<_float>(pAINodeAnim->mPositionKeys[i].mTime);
		}

		KeyFrame.vScale = vScale;
		KeyFrame.vRotation = vRotation;
		KeyFrame.vTranslation = vTranslation;

		m_KeyFrames.push_back(KeyFrame);
	}

	return S_OK;
}

void CChannel::Update_TransformationMatrix(const vector<class CBone*>& Bones, _float fCurrentTrackPosition, _uint* pCurrentKeyIndex)
{
	if (0.f == fCurrentTrackPosition)
		*pCurrentKeyIndex = 0;

	KEYFRAME LastKeyFrameDesc = m_KeyFrames.back();
	_vector vScale = {};
	_vector vRotation = {};
	_vector vTranslation = {};

	if (fCurrentTrackPosition >= LastKeyFrameDesc.fTrackPosition)
	{	// 마지막 키프레임을 넘기면 보간X
		vScale = XMLoadFloat3(&LastKeyFrameDesc.vScale);
		vRotation = XMLoadFloat4(&LastKeyFrameDesc.vRotation);
		vTranslation = XMVectorSetW(XMLoadFloat3(&LastKeyFrameDesc.vTranslation), 1.f);
	}
	else // 무조건 보간이 필요한 상태.
	{
		while (fCurrentTrackPosition >= m_KeyFrames[*pCurrentKeyIndex + 1].fTrackPosition)
			++*pCurrentKeyIndex;

		_vector vSourScale = XMLoadFloat3(&m_KeyFrames[*pCurrentKeyIndex].vScale);
		_vector vDestScale = XMLoadFloat3(&m_KeyFrames[*pCurrentKeyIndex + 1].vScale);

		_vector vSourRotation = XMLoadFloat4(&m_KeyFrames[*pCurrentKeyIndex].vRotation);
		_vector vDestRotation = XMLoadFloat4(&m_KeyFrames[*pCurrentKeyIndex + 1].vRotation);

		_vector vSourTranslation = XMVectorSetW(XMLoadFloat3(&m_KeyFrames[*pCurrentKeyIndex].vTranslation), 1.f);
		_vector vDestTranslation = XMVectorSetW(XMLoadFloat3(&m_KeyFrames[*pCurrentKeyIndex + 1].vTranslation), 1.f);

		_float  fRatio = (fCurrentTrackPosition - m_KeyFrames[*pCurrentKeyIndex].fTrackPosition) /
			(m_KeyFrames[*pCurrentKeyIndex + 1].fTrackPosition - m_KeyFrames[*pCurrentKeyIndex].fTrackPosition);

		vScale = XMVectorLerp(vSourScale, vDestScale, fRatio); // (vSourScale + vDestScale) * fRatio;
		vRotation = XMQuaternionSlerp(vSourRotation, vDestRotation, fRatio);
		vTranslation = XMVectorLerp(vSourTranslation, vDestTranslation, fRatio);
	}

	_matrix TransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vTranslation);

	Bones[m_iBoneIndex]->Set_TransformationMatrix(TransformationMatrix);
}

CChannel* CChannel::Create(const aiNodeAnim* pAINodeAnim, class CModel* pModel)
{
	CChannel* pInstance = new CChannel();

	if (FAILED(pInstance->Initialize(pAINodeAnim, pModel)))
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
