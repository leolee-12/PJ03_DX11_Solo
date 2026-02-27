#pragma once
#include "Base.h"
#include "Bone_Data.h"

BEGIN(Engine)

struct ENGINE_DLL Channel_Data
{
	string szName = "";
	_uint				iNumKeyFrames = { 0 };
	_uint				iBoneIndex = { 0 };
	vector<KEYFRAME>	KeyFrames;


#ifdef _DEBUG
	HRESULT		Bake_ChannelData(const aiNodeAnim* pChannel, vector<BONE_DATA> Bones);
#endif
	HRESULT		Bake_Binary(ofstream& fout);
	HRESULT		Load_Binary(ifstream& fin);
}typedef CHANNEL_DATA;


END