#include "Animation_Data.h"

#ifdef _DEBUG
HRESULT Animation_Data::Bake_AnimationData(const aiAnimation* pAIAnimation, const vector<BONE_DATA> Bones)
{
	szName = pAIAnimation->mName.data;

	fDuration = (_float)pAIAnimation->mDuration;
	fTickPerSecond = (_float)pAIAnimation->mTicksPerSecond;
	iNumChannels = pAIAnimation->mNumChannels;

	/* 이 애니메이션에서 사용하기위한 뼈(aiNodeAnim,채널)의 정보를 만든다. */
	for (_uint i = 0; i < iNumChannels; i++)
	{
		CHANNEL_DATA Channel_Data;
		/**/
		Channel_Data.Bake_ChannelData(pAIAnimation->mChannels[i], Bones);
		Channels.push_back(Channel_Data);
	}
	return S_OK;
}
#endif

HRESULT Animation_Data::Bake_Binary(ofstream& fout)
{
	size_t strSize = szName.size();
	fout.write(reinterpret_cast<char*>(&strSize), sizeof(strSize));
	fout.write(reinterpret_cast<char*>(&szName[0]), (strSize));

	fout.write(reinterpret_cast<char*>(&iNumChannels), sizeof(iNumChannels));
	fout.write(reinterpret_cast<char*>(&fDuration), sizeof(fDuration));
	fout.write(reinterpret_cast<char*>(&fTickPerSecond), sizeof(fTickPerSecond));

	for (auto iter : Channels)
		iter.Bake_Binary(fout);

	return S_OK;
}

HRESULT Animation_Data::Load_Binary(ifstream& fin)
{
	size_t strSize = {};
	fin.read(reinterpret_cast<char*>(&strSize), sizeof(strSize));
	szName.resize(strSize);
	fin.read(reinterpret_cast<char*>(&szName[0]), (strSize));

	fin.read(reinterpret_cast<char*>(&iNumChannels), sizeof(iNumChannels));
	fin.read(reinterpret_cast<char*>(&fDuration), sizeof(fDuration));
	fin.read(reinterpret_cast<char*>(&fTickPerSecond), sizeof(fTickPerSecond));

	for (_uint i = 0; i < iNumChannels; i++)
	{
		CHANNEL_DATA Channel_Data = {};
		Channel_Data.Load_Binary(fin);
		Channels.push_back(Channel_Data);
	}

	return S_OK;
}
