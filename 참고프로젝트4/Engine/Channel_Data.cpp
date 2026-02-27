#include "Channel_Data.h"

#ifdef _DEBUG
HRESULT Channel_Data::Bake_ChannelData(const aiNodeAnim* pChannel, vector<BONE_DATA> Bones)
{
	szName = pChannel->mNodeName.data;

	_uint		iIndex = { 0 };

	auto	iter = find_if(Bones.begin(), Bones.end(), [&](BONE_DATA pBone)
		{
			if (szName == pBone.szName)
			{
				return true;
			}

			++iIndex;

			return false;
		});

	if (iter == Bones.end())
		RETURN_EFAIL;

	iBoneIndex = iIndex;

	iNumKeyFrames = max(pChannel->mNumScalingKeys, pChannel->mNumRotationKeys);
	iNumKeyFrames = max(iNumKeyFrames, pChannel->mNumPositionKeys);

	_float3 vScale;
	_float4 vRotation;
	_float3 vPosition;

	for (_uint i = 0; i < iNumKeyFrames; i++)
	{
		KEYFRAME KeyFrame = {};
		if (i < pChannel->mNumScalingKeys)
		{
			memcpy(&vScale, &pChannel->mScalingKeys[i].mValue, sizeof(_float3));
			KeyFrame.fTrackPosition = pChannel->mScalingKeys[i].mTime;
		}
		if (i < pChannel->mNumRotationKeys)
		{
			vRotation.x = pChannel->mRotationKeys[i].mValue.x;
			vRotation.y = pChannel->mRotationKeys[i].mValue.y;
			vRotation.z = pChannel->mRotationKeys[i].mValue.z;
			vRotation.w = pChannel->mRotationKeys[i].mValue.w;
			KeyFrame.fTrackPosition = pChannel->mRotationKeys[i].mTime;
		}
		if (i < pChannel->mNumPositionKeys)
		{
			memcpy(&vPosition, &pChannel->mPositionKeys[i].mValue, sizeof(_float3));
			KeyFrame.fTrackPosition = pChannel->mPositionKeys[i].mTime;
		}

		KeyFrame.vScale = vScale;
		KeyFrame.vRotation = vRotation;
		KeyFrame.vPosition = vPosition;

		KeyFrames.push_back(KeyFrame);
	}


	return S_OK;
}
#endif

HRESULT Channel_Data::Bake_Binary(ofstream& fout)
{
	size_t strSize = szName.size();
	fout.write(reinterpret_cast<char*>(&strSize), sizeof(strSize));
	fout.write(reinterpret_cast<char*>(&szName[0]), (strSize));

	fout.write(reinterpret_cast<char*>(&iNumKeyFrames), sizeof(iNumKeyFrames));
	fout.write(reinterpret_cast<char*>(&iBoneIndex), sizeof(iBoneIndex));

	for(auto iter : KeyFrames)
		fout.write(reinterpret_cast<char*>(&iter), sizeof(KEYFRAME));

	return S_OK;
}

HRESULT Channel_Data::Load_Binary(ifstream& fin)
{
	size_t strSize = {};
	fin.read(reinterpret_cast<char*>(&strSize), sizeof(strSize));
	szName.resize(strSize);
	fin.read(reinterpret_cast<char*>(&szName[0]), (strSize));

	fin.read(reinterpret_cast<char*>(&iNumKeyFrames), sizeof(iNumKeyFrames));
	fin.read(reinterpret_cast<char*>(&iBoneIndex), sizeof(iBoneIndex));

	for (_uint i = 0; i < iNumKeyFrames; i++)
	{
		KEYFRAME KeyFrame = {};
		fin.read(reinterpret_cast<char*>(&KeyFrame), sizeof(KEYFRAME));
		KeyFrames.push_back(KeyFrame);
	}
	return S_OK;
}
