#pragma once
#include "Base.h"
#include "Channel_Data.h"
#include "Bone_Data.h"

BEGIN(Engine)

struct ENGINE_DLL Animation_Data
{
	string szName = "";
	_uint				iNumChannels = { 0 }; /* 이 애니메이션이 사용하는 뼈의 갯수. */
	_float				fDuration = { 0.0f }; /* 내 애니메이션을 전체 재생하기위한 전체 길이. */
	_float				fTickPerSecond = { 0.f }; /* 애니메이션의 재생 속도 : m_TickPerSecond * fTimeDelta */

	vector<CHANNEL_DATA>	Channels;


#ifdef _DEBUG
	HRESULT Bake_AnimationData(const aiAnimation* pAIAnimation, const vector <BONE_DATA> Bones);
#endif
	HRESULT Bake_Binary(ofstream& fout);
	HRESULT	Load_Binary(ifstream& fin);
}typedef ANIMATION_DATA;


END