#pragma once
#include "Base.h"

NS_BEGIN(Engine)
class CGameInstance;
NS_END

NS_BEGIN(FMOD)
class System;
class Sound;
class Channel;
class ChannelGroup;
NS_END

NS_BEGIN(Engine)

class ENGINE_DLL CSound_Manager final : public CBase
{
private:
	CSound_Manager();
	virtual ~CSound_Manager() = default;

public:
	HRESULT Initialize();
	void Update();

public:
	HRESULT Play(const _tchar* pSoundKey, CHANNELID eChannelID = CHANNELID::SFX, _float fVolume = 1.f, _bool bLoop = false);
	HRESULT Play_BGM(const _tchar* pSoundKey, _float fVolume = 1.f);
	HRESULT Play_3D(const _tchar* pSoundKey, const _float3& vPosition, CHANNELID eChannelID = CHANNELID::SFX,
		_float fVolume = 1.f, _float fMinDistance = 1.f, _float fMaxDistance = 100.f, _bool bLoop = false);
	_float Get_SoundLengthSeconds(const _tchar* pSoundKey) const;

	void Stop_Sound(CHANNELID eChannelID);
	void Stop_Group(CHANNELID eChannelID);
	void Stop_All();

	void Set_ChannelVolume(CHANNELID eChannelID, _float fVolume);
	void Set_GroupVolume(CHANNELID eChannelID, _float fVolume);

private:
	HRESULT Load_Sounds();
	HRESULT Ready_ChannelGroups();

	FMOD::ChannelGroup* Get_ChannelGroup(CHANNELID eChannelID) const;
	_bool Is_ValidChannelID(CHANNELID eChannelID) const;

	void Update_Listener();

private:
	CGameInstance* m_pGameInstance = { nullptr };

	static constexpr _uint CHANNEL_COUNT = static_cast<_uint>(CHANNELID::MAXCHANNEL);
	FMOD::System* m_pSystem = { nullptr };
	FMOD::Channel* m_Channels[CHANNEL_COUNT] = {};
	FMOD::ChannelGroup* m_ChannelGroups[CHANNEL_COUNT] = {};
	unordered_map<wstring, FMOD::Sound*> m_Sounds;

	_float3 m_vListenerPos = {};
	_float3 m_vListenerLook = { 0.f, 0.f, 1.f };
	_float3 m_vListenerUp = { 0.f, 1.f, 0.f };

public:
	static CSound_Manager* Create();

private:
	virtual void Free() override;
};

NS_END
