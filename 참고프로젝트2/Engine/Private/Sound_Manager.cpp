#include "..\Public\Sound_Manager.h"
#include "Layer.h"
#include "GameObject.h"
#include <locale>
#include <codecvt>

CSound_Manager::CSound_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CSound_Manager::CSound_Manager(const CSound_Manager& Prototype)
	: CGameObject(Prototype)
{
}


HRESULT CSound_Manager::Initialize_Prototype()
{
	// FMOD 시스템 초기화
	FMOD_System_Create(&m_pSoundSystem, FMOD_VERSION);
	FMOD_System_Init(m_pSoundSystem, MAX_CHANNELS, FMOD_INIT_NORMAL | FMOD_INIT_STREAM_FROM_UPDATE, nullptr);

	// 채널 그룹 생성
	FMOD_System_CreateChannelGroup(m_pSoundSystem, nullptr, &m_pChannelGroup);

	// 카테고리별 초기 볼륨 설정 (기본값: 1.0f)
	m_categoryVolumes[SOUND_CATEGORY::BGM] = 1.0f;
	m_categoryVolumes[SOUND_CATEGORY::VOICE] = 1.0f;
	m_categoryVolumes[SOUND_CATEGORY::SFX] = 1.0f;


	return S_OK;
}

HRESULT CSound_Manager::Initialize(void* pArg)
{
	return S_OK;
}

void CSound_Manager::Camera_Update(_float fTimeDelta)
{

}

void CSound_Manager::Update(_float fTimeDelta)
{
	FMOD_System_Update(m_pSoundSystem);

	// 재생 완료된 채널 정리
	int playingChannels = 0;
	FMOD_ChannelGroup_GetNumChannels(m_pChannelGroup, &playingChannels);

	for (int i = 0; i < playingChannels; ++i)
	{
		FMOD_CHANNEL* tempChannel = nullptr;
		if (FMOD_ChannelGroup_GetChannel(m_pChannelGroup, i, &tempChannel) == FMOD_OK)
		{
			FMOD_BOOL isPlaying = false;
			if (FMOD_Channel_IsPlaying(tempChannel, &isPlaying) == FMOD_OK && !isPlaying)
			{
				FMOD_Channel_Stop(tempChannel);
			}
		}
	}
}

void CSound_Manager::Late_Update(_float fTimeDelta)
{

}

HRESULT CSound_Manager::Render(_float fTimeDelta)
{
	return S_OK;
}

void CSound_Manager::Register_Sound(const std::wstring& filePath, SOUND_KEY_NAME alias, SOUND_CATEGORY category, _bool loop, _bool isNonOverlapping)
{
	// 이미 등록된 사운드인지 확인
	if (m_soundMap.find(alias) != m_soundMap.end())
	{
		return; // 이미 존재하므로 등록하지 않고 반환
	}

	FMOD_SOUND* sound;

	// wstring을 string으로 변환
	std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
	std::string filePathStr = converter.to_bytes(filePath);

	// 모드를 설정하여 사운드 생성
	FMOD_MODE mode = loop ? FMOD_LOOP_NORMAL | FMOD_CREATESAMPLE : FMOD_LOOP_OFF | FMOD_CREATESAMPLE;
	if (FMOD_System_CreateSound(m_pSoundSystem, filePathStr.c_str(), FMOD_DEFAULT | mode, nullptr, &sound) != FMOD_OK)
	{
		return;
	}

	// 사운드 등록
	m_soundMap[alias] = sound;
	// 카테고리 맵에 추가
	m_soundCategoryMap[alias] = category;

	// 중복 재생 방지 사운드로 등록
	if (isNonOverlapping)
	{
		m_nonOverlappingSounds.insert(alias);
	}
}

void CSound_Manager::Register_Sound_Group(SOUND_GROUP_KEY groupKey, const std::wstring& filePath, SOUND_GROUP_KEY_NAME alias, SOUND_CATEGORY category, _bool loop)
{
	// 그룹이 존재하지 않으면 생성
	if (m_soundGroupMap.find(groupKey) == m_soundGroupMap.end())
	{
		m_soundGroupMap[groupKey] = std::vector<SOUND_GROUP_KEY_NAME>();
	}

	// 이미 등록된 사운드인지 확인
	if (m_groupSoundMap.find(alias) == m_groupSoundMap.end())
	{
		// 그룹 사운드 등록
		FMOD_SOUND* sound;

		// wstring을 string으로 변환
		std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
		std::string filePathStr = converter.to_bytes(filePath);

		// 모드를 설정하여 사운드 생성
		FMOD_MODE mode = loop ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF;
		if (FMOD_System_CreateSound(m_pSoundSystem, filePathStr.c_str(), FMOD_DEFAULT | mode, nullptr, &sound) != FMOD_OK)
		{
			return;
		}

		// 그룹 사운드 맵에 등록
		m_groupSoundMap[alias] = sound;
		m_groupSoundCategoryMap[alias] = category;
	}

	// 그룹에 해당 alias를 추가
	m_soundGroupMap[groupKey].push_back(alias);
}

void CSound_Manager::Play_Sound(SOUND_KEY_NAME alias, _bool loop, _float volume)
{
	if (!m_isImguiPlay)
		return;

	auto it = m_soundMap.find(alias);
	if (it == m_soundMap.end()) return;

	// 중복 재생 방지 사운드인지 확인
	_bool isNonOverlapping = (m_nonOverlappingSounds.find(alias) != m_nonOverlappingSounds.end());

	// 중복 재생 방지 사운드인 경우, 이미 재생 중인지 확인
	if (isNonOverlapping)
	{
		auto channelIt = m_channelMap.find(alias);
		if (channelIt != m_channelMap.end())
		{
			FMOD_BOOL isPlaying = false;
			FMOD_Channel_IsPlaying(channelIt->second.channel, &isPlaying);
			if (isPlaying)
			{
				// 이미 재생 중이므로 새로운 재생 요청 무시
				return;
			}
		}
	}


	FMOD_CHANNEL* channel = nullptr;

	// 채널 수 확인
	int playingChannels = 0;
	FMOD_ChannelGroup_GetNumChannels(m_pChannelGroup, &playingChannels);

	if (playingChannels >= MAX_CHANNELS)
	{
		// 오래된 채널을 중지하고 재사용
		FMOD_CHANNEL* oldestChannel = nullptr;
		FMOD_ChannelGroup_GetChannel(m_pChannelGroup, 0, &oldestChannel); // 예시로 첫 번째 채널 사용
		if (oldestChannel)
		{
			FMOD_Channel_Stop(oldestChannel);
		}
	}

	// 카테고리 볼륨 가져오기
	auto categoryIt = m_soundCategoryMap.find(alias);
	float categoryVolume = 1.0f;
	if (categoryIt != m_soundCategoryMap.end())
	{
		categoryVolume = m_categoryVolumes[categoryIt->second];
	}

	float actualVolume = volume * categoryVolume;

	// 사운드 재생 및 볼륨 설정
	if (FMOD_System_PlaySound(m_pSoundSystem, it->second, m_pChannelGroup, false, &channel) == FMOD_OK)
	{
		FMOD_Channel_SetVolume(channel, actualVolume);
		m_channelMap[alias] = { channel, volume }; // 초기 볼륨 저장
	}
}

void CSound_Manager::Play_Group_Sound(SOUND_GROUP_KEY groupKey, _bool loop, _float volume)
{
	if (!m_isImguiPlay)
		return;

	// 그룹이 존재하는지 확인
	auto groupIt = m_soundGroupMap.find(groupKey);
	if (groupIt == m_soundGroupMap.end()) return;

	auto& soundList = groupIt->second;

	// 사운드 리스트가 비어있는지 확인
	if (soundList.empty()) return;

	// 마지막으로 재생된 사운드를 제외한 사운드 목록 생성
	std::vector<SOUND_GROUP_KEY_NAME> availableSounds;
	for (const auto& soundAlias : soundList)
	{
		if (m_lastPlayedSound[groupKey] != soundAlias)
		{
			availableSounds.push_back(soundAlias);
		}
	}

	// 재생 가능한 사운드가 없으면 모든 사운드를 다시 고려
	if (availableSounds.empty())
	{
		availableSounds = soundList;
	}

	// 랜덤으로 하나의 사운드 선택
	int randomIndex = rand() % availableSounds.size();
	SOUND_GROUP_KEY_NAME selectedSoundKey = availableSounds[randomIndex];

	// 선택한 사운드를 그룹 사운드 맵에서 찾기
	auto soundIt = m_groupSoundMap.find(selectedSoundKey);
	if (soundIt == m_groupSoundMap.end()) return;

	FMOD_SOUND* pSound = soundIt->second;

	FMOD_CHANNEL* channel = nullptr;

	// 채널 수 확인
	int playingChannels = 0;
	FMOD_ChannelGroup_GetNumChannels(m_pChannelGroup, &playingChannels);

	if (playingChannels >= MAX_CHANNELS)
	{
		// 오래된 채널을 중지하고 재사용
		FMOD_CHANNEL* oldestChannel = nullptr;
		FMOD_ChannelGroup_GetChannel(m_pChannelGroup, 0, &oldestChannel);
		if (oldestChannel)
		{
			FMOD_Channel_Stop(oldestChannel);
		}
	}

	// 카테고리 볼륨 가져오기
	auto categoryIt = m_groupSoundCategoryMap.find(selectedSoundKey);
	float categoryVolume = 1.0f;
	if (categoryIt != m_groupSoundCategoryMap.end())
	{
		categoryVolume = m_categoryVolumes[categoryIt->second];
	}

	float actualVolume = volume * categoryVolume;

	// 사운드 재생 및 볼륨 설정
	if (FMOD_System_PlaySound(m_pSoundSystem, pSound, m_pChannelGroup, false, &channel) == FMOD_OK)
	{
		FMOD_Channel_SetVolume(channel, actualVolume);
		m_groupChannelMap[selectedSoundKey] = { channel, volume }; // 초기 볼륨 저장
	}

	// 마지막으로 재생한 사운드 저장
	m_lastPlayedSound[groupKey] = selectedSoundKey;
}

void CSound_Manager::Stop_Sound(SOUND_KEY_NAME alias)
{
	auto it = m_channelMap.find(alias);
	if (it == m_channelMap.end()) return;

	FMOD_Channel_Stop(it->second.channel);
}

void CSound_Manager::Stop_Group_Sound(SOUND_GROUP_KEY groupKey)
{
	// 그룹 찾기
	auto groupIt = m_soundGroupMap.find(groupKey);
	if (groupIt == m_soundGroupMap.end()) return;

	// 그룹 내 모든 사운드의 채널을 중지
	const auto& soundList = groupIt->second;
	for (const auto& alias : soundList)
	{
		auto channelIt = m_groupChannelMap.find(alias);
		if (channelIt != m_groupChannelMap.end())
		{
			FMOD_Channel_Stop(channelIt->second.channel);
		}
	}
}

void CSound_Manager::Set_Volume(SOUND_KEY_NAME alias, float volume)
{
	auto it = m_channelMap.find(alias);
	if (it == m_channelMap.end()) return;

	// 초기 볼륨 업데이트
	it->second.baseVolume = volume;

	// 현재 카테고리 볼륨 가져오기
	auto categoryIt = m_soundCategoryMap.find(alias);
	float categoryVolume = 1.0f;
	if (categoryIt != m_soundCategoryMap.end())
	{
		categoryVolume = m_categoryVolumes[categoryIt->second];
	}

	float actualVolume = volume * categoryVolume;
	FMOD_Channel_SetVolume(it->second.channel, actualVolume);
}

void CSound_Manager::Set_Group_Volume(SOUND_GROUP_KEY groupKey, float volume)
{
	// 그룹 찾기
	auto groupIt = m_soundGroupMap.find(groupKey);
	if (groupIt == m_soundGroupMap.end()) return;

	// 그룹 내 모든 사운드의 볼륨을 업데이트
	const auto& soundList = groupIt->second;
	for (const auto& alias : soundList)
	{
		auto channelIt = m_groupChannelMap.find(alias);
		if (channelIt != m_groupChannelMap.end())
		{
			// 초기 볼륨 업데이트
			channelIt->second.baseVolume = volume;

			// 현재 카테고리 볼륨 가져오기
			auto categoryIt = m_groupSoundCategoryMap.find(alias);
			float categoryVolume = 1.0f;
			if (categoryIt != m_groupSoundCategoryMap.end())
			{
				categoryVolume = m_categoryVolumes[categoryIt->second];
			}

			float actualVolume = volume * categoryVolume;
			FMOD_Channel_SetVolume(channelIt->second.channel, actualVolume);
		}
	}
}

void CSound_Manager::Set_Category_Volume(SOUND_CATEGORY category, float volume)
{
	// 카테고리 볼륨 업데이트
	m_categoryVolumes[category] = volume;

	// 개별 사운드 볼륨 조절
	for (const auto& pair : m_soundCategoryMap)
	{
		if (pair.second == category)
		{
			auto channelIt = m_channelMap.find(pair.first);
			if (channelIt != m_channelMap.end())
			{
				float newVolume = channelIt->second.baseVolume * volume;
				FMOD_Channel_SetVolume(channelIt->second.channel, newVolume);
			}
		}
	}

	// 그룹 사운드 볼륨 조절
	for (const auto& pair : m_groupSoundCategoryMap)
	{
		if (pair.second == category)
		{
			auto channelIt = m_groupChannelMap.find(pair.first);
			if (channelIt != m_groupChannelMap.end())
			{
				float newVolume = channelIt->second.baseVolume * volume;
				FMOD_Channel_SetVolume(channelIt->second.channel, newVolume);
			}
		}
	}
}

_float CSound_Manager::Get_Category_Volume(SOUND_CATEGORY category)
{
	auto it = m_categoryVolumes.find(category);
	if (it != m_categoryVolumes.end())
	{
		return it->second;
	}
	return 1.0f; // 기본 볼륨은 1.0
}

void CSound_Manager::Set_ImguiPlay(_bool isPlay)
{
	m_isImguiPlay = isPlay;

	// m_isImguiPlay가 false라면 모든 사운드를 중지
	if (!m_isImguiPlay)
	{
		// 개별 사운드 채널 중지
		for (auto& pair : m_channelMap)
		{
			if (pair.second.channel)
			{
				FMOD_Channel_Stop(pair.second.channel);
			}
		}

		// 그룹 사운드 채널 중지
		for (auto& pair : m_groupChannelMap)
		{
			if (pair.second.channel)
			{
				FMOD_Channel_Stop(pair.second.channel);
			}
		}
	}
	else
	{
		// 예시로 BGM 재생
		Play_Sound(SOUND_KEY_NAME::SPACE_BGM, true, 0.2f);
	}
}


CSound_Manager* CSound_Manager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSound_Manager* pInstance = new CSound_Manager(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype())) {
		MessageBox(0, L"CSound_Manager Created Failed", L"System Error", MB_OK);
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSound_Manager::Clone(void* pArg)
{
	return this;
}

void CSound_Manager::Free()
{
	// 개별 사운드 해제
	for (auto& pair : m_soundMap)
	{
		FMOD_Sound_Release(pair.second);
	}
	m_soundMap.clear();
	m_channelMap.clear();

	// 그룹 사운드 해제
	for (auto& pair : m_groupSoundMap)
	{
		FMOD_Sound_Release(pair.second);
	}
	m_groupSoundMap.clear();
	m_groupChannelMap.clear();

	if (m_pSoundSystem)
	{
		FMOD_System_Close(m_pSoundSystem);
		FMOD_System_Release(m_pSoundSystem);
		m_pSoundSystem = nullptr;
	}

	__super::Free();
}
