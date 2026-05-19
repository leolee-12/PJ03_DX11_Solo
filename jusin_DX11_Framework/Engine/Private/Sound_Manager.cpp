#include "Sound_Manager.h"
#include "GameInstance.h"

#include "fmod/fmod.hpp"
#include "fmod/fmod_errors.h"
using namespace FMOD;

namespace
{
	constexpr const _tchar* SOUND_ROOT = L"../../Resources/Sounds/";

#ifdef _DEBUG
	void DebugLogW(const wchar_t* pMessage)
	{
		OutputDebugStringW(pMessage);
		OutputDebugStringW(L"\n");
	}

	void DebugLogA(const char* pMessage)
	{
		OutputDebugStringA(pMessage);
		OutputDebugStringA("\n");
	}

	void DebugLogFMOD(const char* pContext, FMOD_RESULT eResult)
	{
		char szMessage[512] = {};
		sprintf_s(szMessage, "[Sound_Manager] %s failed: %s", pContext, FMOD_ErrorString(eResult));
		DebugLogA(szMessage);
	}
#else
	void DebugLogW(const wchar_t*) {}
	void DebugLogA(const char*) {}
	void DebugLogFMOD(const char*, FMOD_RESULT) {}
#endif

	bool IsSupportedSoundFile(const filesystem::path& FilePath)
	{
		wstring strExt = FilePath.extension().wstring();

		transform(strExt.begin(), strExt.end(), strExt.begin(),
			[](wchar_t ch)
			{
				return static_cast<wchar_t>(towlower(ch));
			});

		return strExt == L".wav" || strExt == L".mp3" || strExt == L".ogg";
	}

	wstring GetFirstPathElementLower(const filesystem::path& RelativePath)
	{
		auto iter = RelativePath.begin();
		if (iter == RelativePath.end())
			return {};

		wstring strFirst = iter->wstring();

		transform(strFirst.begin(), strFirst.end(), strFirst.begin(),
			[](wchar_t ch)
			{
				return static_cast<wchar_t>(towlower(ch));
			});

		return strFirst;
	}

	bool IsBGMPath(const filesystem::path& RelativePath)
	{
		return GetFirstPathElementLower(RelativePath) == L"bgm";
	}

	bool Is3DSoundPath(const filesystem::path& RelativePath)
	{
		return GetFirstPathElementLower(RelativePath) == L"3d";
	}

	string ToFMODPath(const filesystem::path& FilePath)
	{
		return WtoS(FilePath.generic_wstring());
	}

	const char* GetChannelGroupName(CHANNELID eChannelID)
	{
		switch (eChannelID)
		{
		case CHANNELID::BGM:
			return "BGM";
		case CHANNELID::UI:
			return "UI";
		case CHANNELID::VOICE:
			return "VOICE";
		case CHANNELID::AMBIENT:
			return "AMBIENT";
		case CHANNELID::SFX:
			return "SFX";
		default:
			return "UNKNOWN";
		}
	}

	_float ClampVolume(_float fVolume)
	{
		if (fVolume < 0.f)
			return 0.f;

		if (fVolume > 1.f)
			return 1.f;

		return fVolume;
	}

	FMOD_MODE ApplyLoopMode(FMOD_MODE eMode, _bool bLoop)
	{
		eMode &= ~(FMOD_LOOP_OFF | FMOD_LOOP_NORMAL | FMOD_LOOP_BIDI);
		eMode |= bLoop ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF;

		return eMode;
	}

	template<typename T>
	void Safe_FMOD_Release(T*& pInstance, const char* pContext)
	{
		if (nullptr == pInstance)
			return;

		FMOD_RESULT eResult = pInstance->release();
		if (FMOD_OK != eResult)
			DebugLogFMOD(pContext, eResult);

		pInstance = nullptr;
	}
}

CSound_Manager::CSound_Manager()
	: m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CSound_Manager::Initialize()
{
	FMOD_RESULT eResult = System_Create(&m_pSystem);
	if (FMOD_OK != eResult || nullptr == m_pSystem)
	{
		DebugLogFMOD("System_Create", eResult);
		return E_FAIL;
	}

	eResult = m_pSystem->init(512, FMOD_INIT_NORMAL, nullptr);
	if (FMOD_OK != eResult)
	{
		DebugLogFMOD("System::init", eResult);
		return E_FAIL;
	}

	eResult = m_pSystem->set3DSettings(1.f, 1.f, 1.f);
	if (FMOD_OK != eResult)
	{
		DebugLogFMOD("System::set3DSettings", eResult);
		return E_FAIL;
	}

	if (FAILED(Ready_ChannelGroups()))
		return E_FAIL;

	if (FAILED(Load_Sounds()))
		return E_FAIL;

	return S_OK;
}

void CSound_Manager::Update()
{
	if (nullptr == m_pSystem) return;

	Update_Listener();

	m_pSystem->update();
}

HRESULT CSound_Manager::Play(const _tchar* pSoundKey, CHANNELID eChannelID, _float fVolume, _bool
	bLoop)
{
	if (nullptr == m_pSystem || nullptr == pSoundKey)
		return E_FAIL;

	if (false == Is_ValidChannelID(eChannelID))
		return E_FAIL;

	auto iter = m_Sounds.find(pSoundKey);
	if (iter == m_Sounds.end() || nullptr == iter->second)
	{
#ifdef _DEBUG
		wstring strMessage = L"[Sound_Manager] sound key not found: ";
		strMessage += pSoundKey;
		DebugLogW(strMessage.c_str());
#endif
		return E_FAIL;
	}

	Sound* pSound = iter->second;
	ChannelGroup* pGroup = Get_ChannelGroup(eChannelID);
	if (nullptr == pGroup)
		return E_FAIL;

	FMOD_MODE eMode = FMOD_DEFAULT;
	FMOD_RESULT eResult = pSound->getMode(&eMode);
	if (FMOD_OK != eResult)
	{
		DebugLogFMOD("Sound::getMode", eResult);
		return E_FAIL;
	}

	if (0 != (eMode & FMOD_3D))
	{
#ifdef _DEBUG
		wstring strMessage = L"[Sound_Manager] Play requires a 2D sound. Use Play_3D instead: ";
		strMessage += pSoundKey;
		DebugLogW(strMessage.c_str());
#endif
		return E_FAIL;
	}

	eMode = ApplyLoopMode(eMode, bLoop);

	Channel* pChannel = nullptr;
	eResult = m_pSystem->playSound(pSound, pGroup, true, &pChannel);
	if (FMOD_OK != eResult || nullptr == pChannel)
	{
		DebugLogFMOD("System::playSound", eResult);
		return E_FAIL;
	}

	eResult = pChannel->setMode(eMode);
	if (FMOD_OK != eResult)
		DebugLogFMOD("Channel::setMode", eResult);

	eResult = pChannel->setVolume(ClampVolume(fVolume));
	if (FMOD_OK != eResult)
		DebugLogFMOD("Channel::setVolume", eResult);

	eResult = pChannel->setPaused(false);
	if (FMOD_OK != eResult)
	{
		DebugLogFMOD("Channel::setPaused", eResult);
		return E_FAIL;
	}

	m_Channels[static_cast<_uint>(eChannelID)] = pChannel;

	return S_OK;
}

HRESULT CSound_Manager::Play_BGM(const _tchar* pSoundKey, _float fVolume)
{
	Stop_Sound(CHANNELID::BGM);

	return Play(pSoundKey, CHANNELID::BGM, fVolume, true);
}

HRESULT CSound_Manager::Play_3D(const _tchar* pSoundKey, const _float3& vPosition, CHANNELID
	eChannelID,
	_float fVolume, _float fMinDistance, _float fMaxDistance, _bool bLoop)
{
	if (nullptr == m_pSystem || nullptr == pSoundKey)
		return E_FAIL;

	if (false == Is_ValidChannelID(eChannelID))
		return E_FAIL;

	auto iter = m_Sounds.find(pSoundKey);
	if (iter == m_Sounds.end() || nullptr == iter->second)
	{
#ifdef _DEBUG
		wstring strMessage = L"[Sound_Manager] 3D sound key not found: ";
		strMessage += pSoundKey;
		DebugLogW(strMessage.c_str());
#endif
		return E_FAIL;
	}

	Sound* pSound = iter->second;

	FMOD_MODE eMode = FMOD_DEFAULT;
	FMOD_RESULT eResult = pSound->getMode(&eMode);
	if (FMOD_OK != eResult)
	{
		DebugLogFMOD("Sound::getMode", eResult);
		return E_FAIL;
	}

	if (0 == (eMode & FMOD_3D))
	{
#ifdef _DEBUG
		wstring strMessage = L"[Sound_Manager] Play_3D requires a sound under Sounds/3D/: ";
		strMessage += pSoundKey;
		DebugLogW(strMessage.c_str());
#endif
		return E_FAIL;
	}

	eMode = ApplyLoopMode(eMode, bLoop);

	if (fMinDistance < 0.f)
		fMinDistance = 0.f;

	if (fMaxDistance < fMinDistance)
		fMaxDistance = fMinDistance;

	ChannelGroup* pGroup = Get_ChannelGroup(eChannelID);
	if (nullptr == pGroup)
		return E_FAIL;

	Channel* pChannel = nullptr;
	eResult = m_pSystem->playSound(pSound, pGroup, true, &pChannel);
	if (FMOD_OK != eResult || nullptr == pChannel)
	{
		DebugLogFMOD("System::playSound(3D)", eResult);
		return E_FAIL;
	}

	FMOD_VECTOR vFMODPosition = { vPosition.x, vPosition.y, vPosition.z };

	eResult = pChannel->setMode(eMode);
	if (FMOD_OK != eResult)
		DebugLogFMOD("Channel::setMode(3D)", eResult);

	eResult = pChannel->set3DMinMaxDistance(fMinDistance, fMaxDistance);
	if (FMOD_OK != eResult)
		DebugLogFMOD("Channel::set3DMinMaxDistance", eResult);

	eResult = pChannel->set3DAttributes(&vFMODPosition, nullptr);
	if (FMOD_OK != eResult)
		DebugLogFMOD("Channel::set3DAttributes", eResult);

	eResult = pChannel->setVolume(ClampVolume(fVolume));
	if (FMOD_OK != eResult)
		DebugLogFMOD("Channel::setVolume(3D)", eResult);

	eResult = pChannel->setPaused(false);
	if (FMOD_OK != eResult)
	{
		DebugLogFMOD("Channel::setPaused(3D)", eResult);
		return E_FAIL;
	}

	m_Channels[static_cast<_uint>(eChannelID)] = pChannel;

	return S_OK;
}

void CSound_Manager::Stop_Sound(CHANNELID eChannelID)
{
	if (nullptr == m_pSystem)
		return;

	if (false == Is_ValidChannelID(eChannelID))
		return;

	const _uint iChannel = static_cast<_uint>(eChannelID);
	Channel* pChannel = m_Channels[iChannel];

	if (nullptr == pChannel)
		return;

	bool bPlaying = false;
	FMOD_RESULT eResult = pChannel->isPlaying(&bPlaying);

	// invalid handle / 이미 정지 -> 조용히 정리만
	if (FMOD_OK == eResult && bPlaying)
	{
		eResult = pChannel->stop();
		if (FMOD_OK != eResult)
			DebugLogFMOD("Channel::stop", eResult);
	}

	m_Channels[iChannel] = nullptr;
}

void CSound_Manager::Stop_Group(CHANNELID eChannelID)
{
	if (nullptr == m_pSystem)
		return;

	if (false == Is_ValidChannelID(eChannelID))
		return;

	ChannelGroup* pGroup = Get_ChannelGroup(eChannelID);
	if (nullptr == pGroup)
		return;

	FMOD_RESULT eResult = pGroup->stop();
	if (FMOD_OK != eResult)
		DebugLogFMOD("ChannelGroup::stop", eResult);

	m_Channels[static_cast<_uint>(eChannelID)] = nullptr;
}

void CSound_Manager::Stop_All()
{
	if (nullptr == m_pSystem)
		return;

	for (_uint i = 0; i < CHANNEL_COUNT; ++i)
	{
		if (nullptr == m_ChannelGroups[i])
			continue;

		FMOD_RESULT eResult = m_ChannelGroups[i]->stop();
		if (FMOD_OK != eResult)
			DebugLogFMOD("ChannelGroup::stop all", eResult);

		m_Channels[i] = nullptr;
	}
}

void CSound_Manager::Set_ChannelVolume(CHANNELID eChannelID, _float fVolume)
{
	if (false == Is_ValidChannelID(eChannelID))
		return;

	const _uint iChannel = static_cast<_uint>(eChannelID);
	Channel* pChannel = m_Channels[iChannel];

	if (nullptr == pChannel)
		return;

	bool bPlaying = false;
	FMOD_RESULT eResult = pChannel->isPlaying(&bPlaying);

	if (FMOD_OK != eResult || false == bPlaying)
	{
		m_Channels[iChannel] = nullptr;
		return;
	}

	eResult = pChannel->setVolume(ClampVolume(fVolume));
	if (FMOD_OK != eResult)
		DebugLogFMOD("Channel::setVolume", eResult);
}

void CSound_Manager::Set_GroupVolume(CHANNELID eChannelID, _float fVolume)
{
	ChannelGroup* pGroup = Get_ChannelGroup(eChannelID);
	if (nullptr == pGroup)
		return;

	FMOD_RESULT eResult = pGroup->setVolume(ClampVolume(fVolume));
	if (FMOD_OK != eResult)
		DebugLogFMOD("ChannelGroup::setVolume", eResult);
}

HRESULT CSound_Manager::Load_Sounds()
{
	if (nullptr == m_pSystem)
		return E_FAIL;

	const filesystem::path RootPath = SOUND_ROOT;

	if (false == filesystem::exists(RootPath))
	{
		DebugLogW(L"[Sound_Manager] ../../Resources/Sounds/ does not exist.");
		return S_OK;
	}

	for (const auto& Entry : filesystem::recursive_directory_iterator(RootPath))
	{
		if (false == Entry.is_regular_file())
			continue;

		const filesystem::path& FilePath = Entry.path();

		if (false == IsSupportedSoundFile(FilePath))
			continue;

		filesystem::path RelativePath = filesystem::relative(FilePath, RootPath);
		wstring strSoundKey = RelativePath.generic_wstring();

		if (m_Sounds.end() != m_Sounds.find(strSoundKey))
		{
#ifdef _DEBUG
			wstring strMessage = L"[Sound_Manager] duplicated sound key skipped: " + strSoundKey;
			DebugLogW(strMessage.c_str());
#endif
			continue;
		}

		Sound* pSound = nullptr;
		const string strFilePath = ToFMODPath(FilePath);

		const _bool bBGM = IsBGMPath(RelativePath);
		const _bool b3D = Is3DSoundPath(RelativePath);

		FMOD_MODE eMode = FMOD_DEFAULT;
		FMOD_RESULT eResult = FMOD_OK;

		if (bBGM)
		{
			eMode = FMOD_DEFAULT | FMOD_2D | FMOD_LOOP_NORMAL;
			eResult = m_pSystem->createStream(strFilePath.c_str(), eMode, nullptr, &pSound);
		}
		else if (b3D)
		{
			eMode = FMOD_DEFAULT | FMOD_3D | FMOD_3D_WORLDRELATIVE | FMOD_3D_LINEARROLLOFF | FMOD_LOOP_OFF;
			eResult = m_pSystem->createSound(strFilePath.c_str(), eMode, nullptr, &pSound);
		}
		else
		{
			eMode = FMOD_DEFAULT | FMOD_2D | FMOD_LOOP_OFF;
			eResult = m_pSystem->createSound(strFilePath.c_str(), eMode, nullptr, &pSound);
		}

		if (FMOD_OK != eResult || nullptr == pSound)
		{
			DebugLogFMOD("create sound", eResult);
			continue;
		}

		m_Sounds.emplace(strSoundKey, pSound);
	}

#ifdef _DEBUG
	wchar_t szMessage[256] = {};
	swprintf_s(szMessage, L"[Sound_Manager] loaded sounds: %zu", m_Sounds.size());
	DebugLogW(szMessage);
#endif

	return S_OK;
}

HRESULT CSound_Manager::Ready_ChannelGroups()
{
	if (nullptr == m_pSystem)
		return E_FAIL;

	ChannelGroup* pMasterGroup = nullptr;
	FMOD_RESULT eResult = m_pSystem->getMasterChannelGroup(&pMasterGroup);
	if (FMOD_OK != eResult || nullptr == pMasterGroup)
	{
		DebugLogFMOD("getMasterChannelGroup", eResult);
		return E_FAIL;
	}

	for (_uint i = 0; i < CHANNEL_COUNT; ++i)
	{
		CHANNELID eChannelID = static_cast<CHANNELID>(i);

		eResult = m_pSystem->createChannelGroup(GetChannelGroupName(eChannelID), &m_ChannelGroups[i]);
		if (FMOD_OK != eResult || nullptr == m_ChannelGroups[i])
		{
			DebugLogFMOD("createChannelGroup", eResult);
			return E_FAIL;
		}

		eResult = pMasterGroup->addGroup(m_ChannelGroups[i]);
		if (FMOD_OK != eResult)
		{
			DebugLogFMOD("ChannelGroup::addGroup", eResult);
			return E_FAIL;
		}
	}

	return S_OK;
}

ChannelGroup* CSound_Manager::Get_ChannelGroup(CHANNELID eChannelID) const
{
	if (false == Is_ValidChannelID(eChannelID))
		return nullptr;

	return m_ChannelGroups[static_cast<_uint>(eChannelID)];
}

_bool CSound_Manager::Is_ValidChannelID(CHANNELID eChannelID) const
{
	const _uint iChannelID = static_cast<_uint>(eChannelID);
	return iChannelID < CHANNEL_COUNT;
}

void CSound_Manager::Update_Listener()
{
	if (nullptr == m_pSystem || nullptr == m_pGameInstance) return;

	_matrix CamWorld = XMLoadFloat4x4(m_pGameInstance->Get_Transform_Inverse(D3DTS::VIEW));
	XMStoreFloat3(&m_vListenerPos, CamWorld.r[3]);
	XMStoreFloat3(&m_vListenerLook, CamWorld.r[2]);
	XMStoreFloat3(&m_vListenerUp, CamWorld.r[1]);

	FMOD_VECTOR vListenerPos = { m_vListenerPos.x, m_vListenerPos.y, m_vListenerPos.z };
	FMOD_VECTOR	vListenerLook = { m_vListenerLook.x, m_vListenerLook.y, m_vListenerLook.z };
	FMOD_VECTOR vListenerUp = { m_vListenerUp.x, m_vListenerUp.y, m_vListenerUp.z };

	FMOD_RESULT eResult = m_pSystem->set3DListenerAttributes(0, &vListenerPos, nullptr, &vListenerLook, &vListenerUp);

	if (FMOD_OK != eResult)
		DebugLogFMOD("System::set3DListenerAttributes", eResult);
}

CSound_Manager* CSound_Manager::Create()
{
	CSound_Manager* pInstance = new CSound_Manager();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CSound_Manager");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSound_Manager::Free()
{
	__super::Free();

	for (_uint i = 0; i < CHANNEL_COUNT; ++i)
	{
		if (nullptr != m_Channels[i])
		{
			m_Channels[i]->stop();
			m_Channels[i] = nullptr;
		}
	}

	for (auto& Pair : m_Sounds)
		Safe_FMOD_Release(Pair.second, "Sound::release");
	m_Sounds.clear();

	for (_uint i = 0; i < CHANNEL_COUNT; ++i)
		Safe_FMOD_Release(m_ChannelGroups[i], "ChannelGroup::release");

	if (nullptr != m_pSystem)
	{
		FMOD_RESULT eResult = m_pSystem->close();
		if (FMOD_OK != eResult)
			DebugLogFMOD("System::close", eResult);

		Safe_FMOD_Release(m_pSystem, "System::release");
	}

	Safe_Release(m_pGameInstance);
}