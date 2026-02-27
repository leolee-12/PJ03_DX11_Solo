//#include "Channel.h"
//#include "Bone.h"
//
//CChannel::CChannel()
//{
//}
//
//#ifdef _DEBUG
//HRESULT CChannel::Initialize(const aiNodeAnim* pChannel, CModel::BONES Bones)
//{
//    strcpy_s(m_szName, pChannel->mNodeName.data);
//
//
//    _uint		iBoneIndex = { 0 };
//
//    auto	iter = find_if(Bones.begin(), Bones.end(), [&](CBone* pBone)
//        {
//            if (false == strcmp(m_szName, pBone->Get_Name()))
//            {
//                return true;
//            }
//
//            ++iBoneIndex;
//
//            return false;
//        });
//
//    if (iter == Bones.end())
//        RETURN_EFAIL;
//
//    m_iBoneIndex = iBoneIndex;
//
//    m_iNumKeyFrames = max(pChannel->mNumScalingKeys, pChannel->mNumRotationKeys);
//    m_iNumKeyFrames = max(m_iNumKeyFrames, pChannel->mNumPositionKeys);
//
//    _float3 vScale;
//    _float4 vRotation;
//    _float3 vPosition;
//
//    for (_uint i = 0; i < m_iNumKeyFrames; i++)
//    {
//        KEYFRAME KeyFrame = {};
//        if (i < pChannel->mNumScalingKeys)
//        {
//            memcpy(&vScale, &pChannel->mScalingKeys[i].mValue, sizeof(_float3));
//            KeyFrame.fTrackPosition = pChannel->mScalingKeys[i].mTime;
//        }
//        if (i < pChannel->mNumRotationKeys)
//        {
//            vRotation.x = pChannel->mRotationKeys[i].mValue.x;
//            vRotation.y = pChannel->mRotationKeys[i].mValue.y;
//            vRotation.z = pChannel->mRotationKeys[i].mValue.z;
//            vRotation.w = pChannel->mRotationKeys[i].mValue.w;
//            KeyFrame.fTrackPosition = pChannel->mRotationKeys[i].mTime;
//        }
//        if (i < pChannel->mNumPositionKeys)
//        {
//            memcpy(&vPosition, &pChannel->mPositionKeys[i].mValue, sizeof(_float3));
//            KeyFrame.fTrackPosition = pChannel->mPositionKeys[i].mTime;
//        }
//
//        KeyFrame.vScale = vScale;
//        KeyFrame.vRotation = vRotation;
//        KeyFrame.vPosition = vPosition;
//
//        m_KeyFrames.push_back(KeyFrame);
//    }
//
//
//    return S_OK;
//}
//#endif
//
//HRESULT CChannel::Initialize(CHANNEL_DATA ChannelData)
//{
//	strcpy_s(m_szName, ChannelData.szName.c_str());
//    	
//	m_iBoneIndex = ChannelData.iBoneIndex;
//
//	m_iNumKeyFrames = ChannelData.iNumKeyFrames;
//
//	for (_uint i = 0; i < m_iNumKeyFrames; i++)
//	{
//        KEYFRAME KeyFrame = ChannelData.KeyFrames[i];
//		m_KeyFrames.push_back(KeyFrame);
//	}
//	return S_OK;
//}
//
//void CChannel::Invalidate_TransformationMatrix(_float fCurrentTrackPosition , CModel::BONES Bones, _uint* pCurrentKeyFrameIndex)
//{
//    if (0.0f == fCurrentTrackPosition)
//        *pCurrentKeyFrameIndex = 0;
//
//    _vector vScale;
//    _vector vRotation;
//    _vector vPosition;
//
//    KEYFRAME LastKeyFrame = m_KeyFrames.back();
//
//    /* 마지막 키프레임 이후에는 마지막 상태(행렬)을 취하면 된다. */
//    if (fCurrentTrackPosition >= LastKeyFrame.fTrackPosition)
//    {
//        vScale = XMLoadFloat3(&LastKeyFrame.vScale);
//        vRotation = XMLoadFloat4(&LastKeyFrame.vRotation);
//        vPosition = XMLoadFloat3(&LastKeyFrame.vPosition);
//    }
//    else
//    {
//        /* 선형보간을 통해 현재의 상태을 만들어낸다. */
//        while (fCurrentTrackPosition >= m_KeyFrames[*pCurrentKeyFrameIndex + 1].fTrackPosition)
//            ++*pCurrentKeyFrameIndex;
//
//        _float3		vSourScale, vDestScale;
//        _float4		vSourRotation, vDestRotation;
//        _float3		vSourPosition, vDestPosition;
//
//        vSourScale = m_KeyFrames[*pCurrentKeyFrameIndex].vScale;
//        vSourRotation = m_KeyFrames[*pCurrentKeyFrameIndex].vRotation;
//        vSourPosition = m_KeyFrames[*pCurrentKeyFrameIndex].vPosition;
//
//        vDestScale = m_KeyFrames[*pCurrentKeyFrameIndex + 1].vScale;
//        vDestRotation = m_KeyFrames[*pCurrentKeyFrameIndex + 1].vRotation;
//        vDestPosition = m_KeyFrames[*pCurrentKeyFrameIndex + 1].vPosition;
//
//        _float		fRatio = (fCurrentTrackPosition - m_KeyFrames[*pCurrentKeyFrameIndex].fTrackPosition) /
//            (m_KeyFrames[*pCurrentKeyFrameIndex + 1].fTrackPosition - m_KeyFrames[*pCurrentKeyFrameIndex].fTrackPosition);
//
//        vScale = XMVectorLerp(XMLoadFloat3(&vSourScale), XMLoadFloat3(&vDestScale), fRatio);
//        vRotation = XMQuaternionSlerp(XMLoadFloat4(&vSourRotation), XMLoadFloat4(&vDestRotation), fRatio);
//        vPosition = XMVectorLerp(XMLoadFloat3(&vSourPosition), XMLoadFloat3(&vDestPosition), fRatio);
//    }
//
//    _matrix TransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vPosition);
//
//    /* 이 채널과 같은 이름을 가진 뼈를 찾아서 그 뼈의 TransformationMAtrix를 갱신한다. */
//    Bones[m_iBoneIndex]->Set_TransformationMatrix(TransformationMatrix);
//
//}
//
//void CChannel::Transition(_float fCurrentTrackPosition, CModel::BONES Bones, KEYFRAME pPrevKeyFrame, const _float& fTransitionDuration)
//{
//	_vector vScale;
//	_vector vRotation;
//	_vector vPosition;
//	
//	/* 선형보간을 통해 현재의 상태을 만들어낸다. */
//	
//	_float3		vSourScale, vDestScale;
//	_float4		vSourRotation, vDestRotation;
//	_float3		vSourPosition, vDestPosition;
//
//	vSourScale    = pPrevKeyFrame.vScale;
//	vSourRotation = pPrevKeyFrame.vRotation;
//	vSourPosition = pPrevKeyFrame.vPosition;
//
//	vDestScale    = m_KeyFrames[0].vScale;
//	vDestRotation = m_KeyFrames[0].vRotation;
//	vDestPosition = m_KeyFrames[0].vPosition;
//
//	_float		fRatio = (fCurrentTrackPosition) / (fTransitionDuration);
//
//	vScale = XMVectorLerp(XMLoadFloat3(&vSourScale), XMLoadFloat3(&vDestScale), fRatio);
//	vRotation = XMQuaternionSlerp(XMLoadFloat4(&vSourRotation), XMLoadFloat4(&vDestRotation), fRatio);
//	vPosition = XMVectorLerp(XMLoadFloat3(&vSourPosition), XMLoadFloat3(&vDestPosition), fRatio);
//	
//	_matrix TransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vPosition);
//
//	/* 이 채널과 같은 이름을 가진 뼈를 찾아서 그 뼈의 TransformationMAtrix를 갱신한다. */
//	Bones[m_iBoneIndex]->Set_TransformationMatrix(TransformationMatrix);
//
//}
//
//#ifdef _DEBUG
//CChannel* CChannel::Create(const aiNodeAnim* pChannel, CModel::BONES Bones)
//{
//    CChannel* pInstance = new CChannel();
//
//    if (FAILED(pInstance->Initialize(pChannel, Bones)))
//    {
//        MSG_BOX("Failed to Created : CChannel");
//        Safe_Release(pInstance);
//    }
//    return pInstance;
//}
//#endif
//
//CChannel* CChannel::Create(CHANNEL_DATA ChannelData)
//{
//	CChannel* pInstance = new CChannel();
//
//	if (FAILED(pInstance->Initialize(ChannelData)))
//	{
//		MSG_BOX("Failed to Created : CChannel");
//		Safe_Release(pInstance);
//	}
//	return pInstance;
//}
//
//void CChannel::Free()
//{
//}
