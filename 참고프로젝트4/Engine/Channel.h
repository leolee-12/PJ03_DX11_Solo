//#pragma once
//
//#include "Model.h"
//#include "Channel_Data.h"
//
///* 이 애니메이션에서 사용하는 뼈의 정보다. */
///* 시간대에 따른 뼈의 상태(Scale, Rotation, Position)를 저장한다.*/
//
//BEGIN(Engine)
//
//class CChannel final : public CBase
//{
//private:
//	CChannel();
//	virtual ~CChannel() = default;
//
//public:
//#ifdef _DEBUG
//	HRESULT Initialize(const aiNodeAnim* pChannel,CModel::BONES Bones);
//#endif
//	HRESULT Initialize(CHANNEL_DATA ChannelData);
//	void Invalidate_TransformationMatrix(_float fCurrentTrackPosition, CModel::BONES Bones, _uint* pCurrentKeyFrameIndex);
//	void Transition(_float fCurrentTrackPosition, CModel::BONES Bones, KEYFRAME pPrevKeyFrame, const _float& fTransitionDuration);
//
//private:
//	_char				m_szName[MAX_PATH] = "";
//	_uint				m_iNumKeyFrames = { 0 };
//	vector<KEYFRAME>	m_KeyFrames;
//	//_uint				m_iCurrentKeyFrameIndex = { 0 };
//	_uint				m_iBoneIndex = { 0 };
//
//public:
//	KEYFRAME			Get_KeyFrame(_uint iIndex) { return m_KeyFrames[iIndex]; }
//	_char*				Get_Name() { return m_szName; }
//
//public:
//#ifdef _DEBUG
//	static CChannel* Create(const aiNodeAnim* pChannel, CModel::BONES Bones);
//#endif
//	static CChannel* Create(CHANNEL_DATA ChannelData);
//	virtual void Free() override;
//};
//
//END