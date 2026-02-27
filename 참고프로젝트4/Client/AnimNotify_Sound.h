#pragma once

#include "Client_Defines.h"
#include "AnimNotify.h"
#include "Utility/RapidJson_Utility.h"


BEGIN(Client)

struct FNotifyType_Sound : public FNotifyType
{
	vector<wstring> strSoundGroupTags;	// 사운드 그룹키
	vector<wstring> strSoundNameTags;	// 사운드 이름키
	_int			iSoundChannel;		// 채널
	_float			fSoundVolume;		// 볼륨
	_float			fRandom;			// 음원 재생확률
	_float3			vOffsetPos;			// 3D 효과를 주기위한 위치값
	// 기타 사운드 설정들
};

class CAnimNotify_Sound final : public CAnimNotify
{
	INFO_CLASS(CAnimNotify_Sound, CAnimNotify)

public:
	explicit CAnimNotify_Sound();
	explicit CAnimNotify_Sound(const CAnimNotify_Sound& rhs);
	virtual ~CAnimNotify_Sound() = default;

public:
	virtual HRESULT Initialize() override { return S_OK; }
	virtual HRESULT Initialize(_float m_fTrackPos, FNotifyType_Sound Desc);
	virtual HRESULT Initialize(FRapidJson& InputData);

	// 트리거를 작동하기 전상태로 되돌린다.
	virtual void OnReset() override;

	// 작동시 발생시킬 기능
	virtual _bool OnTrigger() override;

public:
	static shared_ptr<CAnimNotify_Sound> Create(_float fTrackPos, FNotifyType_Sound Desc);
	static shared_ptr<CAnimNotify_Sound> Create(FRapidJson& InputData);
	virtual void Free() override;
	virtual shared_ptr<CAnimNotify> Clone() override;


public:
	virtual FRapidJson Bake_Notify() override;
	HRESULT Input_RpJson(FRapidJson InputData);

};

END