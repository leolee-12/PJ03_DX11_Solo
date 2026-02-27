#include "Sound_Manager.h"

#include "GameInstance.h"
#include "Utility/RapidJson_Utility.h"




CSound_Manager::CSound_Manager()
{

}

HRESULT CSound_Manager::Initialize(const string& strMainPath)
{
	m_strPainPath = strMainPath;

	FMOD_RESULT	result = FMOD_OK;

	// 사운드를 담당하는 대표객체를 생성하는 함수
	result = FMOD::System_Create(&m_pSystem, FMOD_VERSION);

	result = m_pSystem->setOutput(FMOD_OUTPUTTYPE_ALSA);
	result = m_pSystem->setSoftwareChannels(g_iMaxSoundChannel);
	
	// 3D 세팅
	result = m_pSystem->set3DSettings(1.f, 1.f, 0.17f);
	result = m_pSystem->set3DNumListeners(1);
	/*result = FMOD_Channel_Set3DMinMaxDistance(m_pSystem, 1);
	result = FMOD_ChannelGroup_(FMOD_BOOL)
	FMOD_System_Set3DListenerAttributes(m_pSystem);*/
	
	// 1. 시스템 포인터, 2. 사용할 가상채널 수 , 초기화 방식) 
	result = m_pSystem->init(g_iMaxSoundChannel, FMOD_INIT_NORMAL, NULL);
	
	// 채널 그룹 세팅
	m_pSystem->getMasterChannelGroup(&m_pMasterChanelGroup);

	m_pSystem->createChannelGroup("Group_BGM", &m_pChannelGroups[ECast(ESoundGroup::BGM)]);
	m_pSystem->createChannelGroup("Group_Environment", &m_pChannelGroups[ECast(ESoundGroup::Environment)]);
	m_pSystem->createChannelGroup("Group_UI", &m_pChannelGroups[ECast(ESoundGroup::UI)]);
	m_pSystem->createChannelGroup("Group_Voice", &m_pChannelGroups[ECast(ESoundGroup::Voice)]);
	m_pSystem->createChannelGroup("Group_Narration", &m_pChannelGroups[ECast(ESoundGroup::Narration)]);
	m_pSystem->createChannelGroup("Group_Effect", &m_pChannelGroups[ECast(ESoundGroup::Effect)]);
	
	//m_pChannelGroups[ECast(ESoundGroup::Voice)]->setMode(FMOD_3D);
	//m_pChannelGroups[ECast(ESoundGroup::Effect)]->setMode(FMOD_3D);
	//m_pChannelGroups[ECast(ESoundGroup::Environment)]->setMode(FMOD_3D_LINEARSQUAREROLLOFF);
	//m_pChannelGroups[ECast(ESoundGroup::Environment)]->set3DMinMaxDistance(0.3f, 10.f);
#if !SOUND_MANAGER_BGM
	m_pChannelGroups[ECast(ESoundGroup::BGM)]->setMute(true);
#endif
	//m_pChannelGroups[ECast(ESoundGroup::Environment)]->setVolume(1.f);
	m_pChannelGroups[ECast(ESoundGroup::Voice)]->set3DMinMaxDistance(1.5f, 15.f);
	//m_pChannelGroups[ECast(ESoundGroup::Voice)]->setVolume(0.5f);
	m_pChannelGroups[ECast(ESoundGroup::Effect)]->set3DMinMaxDistance(1.5f, 15.f);
	
	FMOD::DSP* pDsp = { nullptr };
	m_pSystem->createDSPByType(FMOD_DSP_TYPE_SFXREVERB, &pDsp);
	m_pChannelGroupDSPs.emplace("Voice_Reverb", pDsp);
	m_pSystem->createDSPByType(FMOD_DSP_TYPE_ECHO, &pDsp);
	m_pChannelGroupDSPs.emplace("Voice_Echo", pDsp);
	m_pSystem->createDSPByType(FMOD_DSP_TYPE_THREE_EQ, &pDsp);
	m_pChannelGroupDSPs.emplace("Voice_ThreeEq", pDsp);
	m_pSystem->createDSPByType(FMOD_DSP_TYPE_SFXREVERB, &pDsp);
	m_pChannelGroupDSPs.emplace("Effect_Reverb", pDsp);
	m_pSystem->createDSPByType(FMOD_DSP_TYPE_ECHO, &pDsp);
	m_pChannelGroupDSPs.emplace("Effect_Echo", pDsp);
	m_pSystem->createDSPByType(FMOD_DSP_TYPE_THREE_EQ, &pDsp);
	m_pChannelGroupDSPs.emplace("Effect_ThreeEq", pDsp);

	

	m_pChannelGroupDSPs["Voice_Reverb"]->setParameterFloat(FMOD_DSP_SFXREVERB_DECAYTIME, 1.5f);
	m_pChannelGroupDSPs["Voice_Reverb"]->setParameterFloat(FMOD_DSP_SFXREVERB_HFREFERENCE, 5000.f);
	m_pChannelGroupDSPs["Voice_Reverb"]->setParameterFloat(FMOD_DSP_SFXREVERB_HFDECAYRATIO, 50.f);
	
	m_pChannelGroupDSPs["Voice_Reverb"]->setParameterFloat(FMOD_DSP_SFXREVERB_WETLEVEL, -10.f);
	m_pChannelGroupDSPs["Voice_Reverb"]->setParameterFloat(FMOD_DSP_SFXREVERB_DRYLEVEL, 0.f);

	m_pChannelGroupDSPs["Voice_Reverb"]->setParameterFloat(FMOD_DSP_SFXREVERB_EARLYDELAY, 20.f);
	m_pChannelGroupDSPs["Voice_Reverb"]->setParameterFloat(FMOD_DSP_SFXREVERB_LATEDELAY, 40.f);
	m_pChannelGroupDSPs["Voice_Reverb"]->setParameterFloat(FMOD_DSP_SFXREVERB_EARLYLATEMIX, 50.f);

	m_pChannelGroupDSPs["Voice_Reverb"]->setParameterFloat(FMOD_DSP_SFXREVERB_DIFFUSION, 100.0f);
	m_pChannelGroupDSPs["Voice_Reverb"]->setParameterFloat(FMOD_DSP_SFXREVERB_DENSITY, 100.f);

	m_pChannelGroupDSPs["Voice_Reverb"]->setParameterFloat(FMOD_DSP_SFXREVERB_LOWSHELFFREQUENCY, 250.f);
	m_pChannelGroupDSPs["Voice_Reverb"]->setParameterFloat(FMOD_DSP_SFXREVERB_LOWSHELFGAIN, 0.5f);
	m_pChannelGroupDSPs["Voice_Reverb"]->setParameterFloat(FMOD_DSP_SFXREVERB_HIGHCUT, 5000.0f);


	
	//FMOD_REVERB_PROPERTIES prop2;// = FMOD_PRESET_CONCERTHALL;
	//prop2.DecayTime = 1.5f;
	//prop2.EarlyDelay = 20.f;
	//prop2.LateDelay = 40.f;
	//prop2.HFReference = 5000.f;
	//prop2.HFDecayRatio = 50.f;
	//prop2.Diffusion = 50.f;
	//prop2.Density = 100.f;
	//prop2.LowShelfFrequency = 250.f;
	//prop2.LowShelfGain = 0.5f;
	//prop2.HighCut = 5000.f;
	//prop2.EarlyLateMix = 50.f;
	//prop2.WetLevel = -10.f;

	//// 모든 상황에 적용될 목소리 리버브
	//((FMOD::Reverb3D*)m_pChannelGroupDSPs["Voice_Reverb"])->setProperties(&prop2);
	
	// WaitMode시 사용할 목소리 에코
	m_pChannelGroupDSPs["Voice_Echo"]->setParameterFloat(FMOD_DSP_ECHO_DELAY, 500.f);
	m_pChannelGroupDSPs["Voice_Echo"]->setParameterFloat(FMOD_DSP_ECHO_FEEDBACK, 45.f);
	m_pChannelGroupDSPs["Voice_Echo"]->setParameterFloat(FMOD_DSP_ECHO_WETLEVEL, -12.f);
	m_pChannelGroupDSPs["Voice_Echo"]->setParameterFloat(FMOD_DSP_ECHO_DRYLEVEL, 0.f);
	

	// 이펙트 리버브
	//((FMOD::Reverb3D*)m_pChannelGroupDSPs["Effect_Reverb"])->setProperties(&prop2);
	m_pChannelGroupDSPs["Effect_Reverb"]->setParameterFloat(FMOD_DSP_SFXREVERB_DECAYTIME, 0.1f);
	m_pChannelGroupDSPs["Effect_Reverb"]->setParameterFloat(FMOD_DSP_SFXREVERB_HFREFERENCE, 5000.f);
	m_pChannelGroupDSPs["Effect_Reverb"]->setParameterFloat(FMOD_DSP_SFXREVERB_HFDECAYRATIO, 50.f);

	m_pChannelGroupDSPs["Effect_Reverb"]->setParameterFloat(FMOD_DSP_SFXREVERB_WETLEVEL, -25.f);
	m_pChannelGroupDSPs["Effect_Reverb"]->setParameterFloat(FMOD_DSP_SFXREVERB_DRYLEVEL, 0.f);

	m_pChannelGroupDSPs["Effect_Reverb"]->setParameterFloat(FMOD_DSP_SFXREVERB_EARLYDELAY, 20.f);
	m_pChannelGroupDSPs["Effect_Reverb"]->setParameterFloat(FMOD_DSP_SFXREVERB_LATEDELAY, 40.f);
	m_pChannelGroupDSPs["Effect_Reverb"]->setParameterFloat(FMOD_DSP_SFXREVERB_EARLYLATEMIX, 50.f);

	m_pChannelGroupDSPs["Effect_Reverb"]->setParameterFloat(FMOD_DSP_SFXREVERB_DIFFUSION, 100.0f);
	m_pChannelGroupDSPs["Effect_Reverb"]->setParameterFloat(FMOD_DSP_SFXREVERB_DENSITY, 100.f);

	m_pChannelGroupDSPs["Effect_Reverb"]->setParameterFloat(FMOD_DSP_SFXREVERB_LOWSHELFFREQUENCY, 250.f);
	m_pChannelGroupDSPs["Effect_Reverb"]->setParameterFloat(FMOD_DSP_SFXREVERB_LOWSHELFGAIN, 0.5f);
	m_pChannelGroupDSPs["Effect_Reverb"]->setParameterFloat(FMOD_DSP_SFXREVERB_HIGHCUT, 5000.0f);

	// WaitMode시 사용할 목소리 에코
	m_pChannelGroupDSPs["Effect_Echo"]->setParameterFloat(FMOD_DSP_ECHO_DELAY, 500.f);
	m_pChannelGroupDSPs["Effect_Echo"]->setParameterFloat(FMOD_DSP_ECHO_FEEDBACK, 45.f);
	m_pChannelGroupDSPs["Effect_Echo"]->setParameterFloat(FMOD_DSP_ECHO_WETLEVEL, -12.f);
	m_pChannelGroupDSPs["Effect_Echo"]->setParameterFloat(FMOD_DSP_ECHO_DRYLEVEL, 0.f);

	// WaitMode시 사용할 이퀄라이저
	m_pChannelGroupDSPs["Effect_ThreeEq"]->setParameterInt(FMOD_DSP_THREE_EQ_CROSSOVERSLOPE, 0);
	m_pChannelGroupDSPs["Effect_ThreeEq"]->setParameterFloat(FMOD_DSP_THREE_EQ_HIGHGAIN, -40.f);
	
	// 기본 DSP 추가
	m_pChannelGroups[ECast(ESoundGroup::Voice)]->addDSP(0, m_pChannelGroupDSPs["Voice_Reverb"]);
	//m_pChannelGroups[ECast(ESoundGroup::Voice)]->addDSP(1, m_pChannelGroupDSPs["Voice_Echo"]);
	//m_pChannelGroups[ECast(ESoundGroup::Effect)]->addDSP(0, m_pChannelGroupDSPs["Effect_Reverb"]);
	//m_pChannelGroups[ECast(ESoundGroup::Effect)]->addDSP(1, m_pChannelGroupDSPs["Effect_Echo"]);
	//m_pChannelGroups[ECast(ESoundGroup::Effect)]->addDSP(2, m_pChannelGroupDSPs["Effect_ThreeEq"]);
	//m_pChannelGroups[ECast(ESoundGroup::Effect)]->setPitch(0.6f);
	
	result = m_pSystem->update();

	// 실제 사운드 폴더 찾아 로드
	Load_SoundFile_GroupAsync(TEXT("FF7_BGM"), (strMainPath + "FF7/BGM/Resident/"), FMOD_DEFAULT | FMOD_CREATECOMPRESSEDSAMPLE);
#if SOUND_MANAGER_BGM
#pragma region BGM
	Load_SoundFile_GroupAsync(TEXT("FF7_BGM_Slu5a"), (strMainPath + "FF7/BGM/Slu5a/"), FMOD_DEFAULT | FMOD_CREATECOMPRESSEDSAMPLE);
	Load_SoundFile_GroupAsync(TEXT("FF7_BGM_Slu5b"), (strMainPath + "FF7/BGM/Slu5b/"), FMOD_DEFAULT | FMOD_CREATECOMPRESSEDSAMPLE);
	Load_SoundFile_GroupAsync(TEXT("FF7_BGM_Mako1"), (strMainPath + "FF7/BGM/Mako1/"), FMOD_DEFAULT | FMOD_CREATECOMPRESSEDSAMPLE);
	Load_SoundFile_GroupAsync(TEXT("FF7_BGM_Mako5"), (strMainPath + "FF7/BGM/Mako5/"), FMOD_DEFAULT | FMOD_CREATECOMPRESSEDSAMPLE);
	Load_SoundFile_GroupAsync(TEXT("FF7_BGM_Boss"), (strMainPath + "FF7/BGM/Boss/"), FMOD_DEFAULT | FMOD_CREATECOMPRESSEDSAMPLE);
#pragma endregion  
#endif // SOUND_MANAGER_BGM



#pragma region 환경
	FMOD_MODE eMode = FMOD_3D | FMOD_CREATECOMPRESSEDSAMPLE;
	Load_SoundFile_GroupAsync(TEXT("FF7_Env_Mako1"), (strMainPath + "FF7/SFX/Field/Mako1/"), eMode);
	Load_SoundFile_GroupAsync(TEXT("FF7_Env_Slum7"), (strMainPath + "FF7/SFX/Field/Slum7/"), eMode);
	Load_SoundFile_GroupAsync(TEXT("FF7_Env_Slu5b"), (strMainPath + "FF7/SFX/Field/Slu5b/"), eMode);
#pragma endregion



#pragma region 공용 사운드
	Load_SoundFile_GroupAsync(TEXT("FF7_Resident_Battle"), (strMainPath + "FF7/SFX/Resident/Battle/"), FMOD_3D | FMOD_CREATECOMPRESSEDSAMPLE);
	Load_SoundFile_GroupAsync(TEXT("FF7_Resident_B_Fire"), (strMainPath + "FF7/SFX/Ability/Fire/"), FMOD_3D | FMOD_CREATECOMPRESSEDSAMPLE);
	Load_SoundFile_GroupAsync(TEXT("FF7_Resident_B_Aero"), (strMainPath + "FF7/SFX/Ability/Aero/"), FMOD_3D | FMOD_CREATECOMPRESSEDSAMPLE);
	Load_SoundFile_GroupAsync(TEXT("FF7_Resident_B_Blizzard"), (strMainPath + "FF7/SFX/Ability/Blizzard/"), FMOD_3D | FMOD_CREATECOMPRESSEDSAMPLE);
	Load_SoundFile_GroupAsync(TEXT("FF7_Resident_B_Thunder"), (strMainPath + "FF7/SFX/Ability/Thunder/"), FMOD_3D | FMOD_CREATECOMPRESSEDSAMPLE);
	Load_SoundFile_GroupAsync(TEXT("FF7_Resident_B_Reflect"), (strMainPath + "FF7/SFX/Ability/Reflect/"), FMOD_3D | FMOD_CREATECOMPRESSEDSAMPLE);
	Load_SoundFile_GroupAsync(TEXT("FF7_Resident_B_Heal"), (strMainPath + "FF7/SFX/Ability/Heal/"), FMOD_3D | FMOD_CREATECOMPRESSEDSAMPLE);
	Load_SoundFile_GroupAsync(TEXT("FF7_Resident_B_Barrier"), (strMainPath + "FF7/SFX/Ability/Barrier/"), FMOD_3D | FMOD_CREATECOMPRESSEDSAMPLE);

	Load_SoundFile_GroupAsync(TEXT("FF7_Resident_UI"), (strMainPath + "FF7/SFX/Resident/UI/"), FMOD_DEFAULT | FMOD_CREATECOMPRESSEDSAMPLE);
	Load_SoundFile_GroupAsync(TEXT("FF7_Resident_Dance"), (strMainPath + "FF7/SFX/Resident/Dance/"), FMOD_DEFAULT | FMOD_CREATECOMPRESSEDSAMPLE);
#pragma endregion



#pragma region 플레이어
	Load_SoundFile_GroupAsync(TEXT("FF7_Cloud_Voice"), (strMainPath + "FF7/Voice/Cloud/"), FMOD_3D | FMOD_CREATECOMPRESSEDSAMPLE);
	Load_SoundFile_GroupAsync(TEXT("FF7_Cloud_Effect"), (strMainPath + "FF7/SFX/Character/Cloud/"), FMOD_3D | FMOD_CREATECOMPRESSEDSAMPLE);
	Load_SoundFile_GroupAsync(TEXT("FF7_Cloud_AS"), (strMainPath + "FF7/SFX/Character/Cloud/AS/"), FMOD_3D | FMOD_CREATECOMPRESSEDSAMPLE);
	Load_SoundFile_GroupAsync(TEXT("FF7_Weapon_BusterSword"), (strMainPath + "FF7/SFX/Weapon/BusterSword/"), FMOD_3D | FMOD_CREATECOMPRESSEDSAMPLE);

	Load_SoundFile_GroupAsync(TEXT("FF7_Aerith_Voice"), (strMainPath + "FF7/Voice/Aerith/"), FMOD_3D | FMOD_CREATECOMPRESSEDSAMPLE);
	Load_SoundFile_GroupAsync(TEXT("FF7_Aerith_Effect"), (strMainPath + "FF7/SFX/Character/Aerith/"), FMOD_3D | FMOD_CREATECOMPRESSEDSAMPLE);
	Load_SoundFile_GroupAsync(TEXT("FF7_Aerith_AS"), (strMainPath + "FF7/SFX/Character/Aerith/AS/"), FMOD_3D | FMOD_CREATECOMPRESSEDSAMPLE);
	Load_SoundFile_GroupAsync(TEXT("FF7_Weapon_Rod"), (strMainPath + "FF7/SFX/Weapon/Rod/"), FMOD_3D | FMOD_CREATECOMPRESSEDSAMPLE);
#pragma endregion

	

#pragma region 몬스터
	Load_SoundFile_GroupAsync(TEXT("FF7_SecuritySoldier_Voice"), (strMainPath + "FF7/Voice/Enemy/SecuritySoldier/"), FMOD_3D | FMOD_CREATECOMPRESSEDSAMPLE);
	Load_SoundFile_GroupAsync(TEXT("FF7_SecuritySoldier_Effect"), (strMainPath + "FF7/SFX/Character/SecuritySoldier/"), FMOD_3D | FMOD_CREATECOMPRESSEDSAMPLE);

	Load_SoundFile_GroupAsync(TEXT("FF7_MonoDrive_Voice"), (strMainPath + "FF7/Voice/Enemy/MonoDrive/"), FMOD_3D | FMOD_CREATECOMPRESSEDSAMPLE);
	Load_SoundFile_GroupAsync(TEXT("FF7_MonoDrive_Effect"), (strMainPath + "FF7/SFX/Character/MonoDrive/"), FMOD_3D | FMOD_CREATECOMPRESSEDSAMPLE);

	Load_SoundFile_GroupAsync(TEXT("FF7_GuardHound_Effect"), (strMainPath + "FF7/SFX/Character/GuardHound/"), FMOD_3D | FMOD_CREATECOMPRESSEDSAMPLE);

	Load_SoundFile_GroupAsync(TEXT("FF7_Sweeper_Effect"), (strMainPath + "FF7/SFX/Character/Sweeper/"), FMOD_3D | FMOD_CREATECOMPRESSEDSAMPLE);

	Load_SoundFile_GroupAsync(TEXT("FF7_Stray_Effect"), (strMainPath + "FF7/SFX/Character/Stray/"), FMOD_3D | FMOD_CREATECOMPRESSEDSAMPLE);

	Load_SoundFile_GroupAsync(TEXT("FF7_Bahamut_Effect"), (strMainPath + "FF7/SFX/Character/Bahamut/"), FMOD_3D | FMOD_CREATECOMPRESSEDSAMPLE);

	Load_SoundFile_GroupAsync(TEXT("FF7_AirBurster_Effect"), (strMainPath + "FF7/SFX/Character/Airbuster/"), FMOD_3D | FMOD_CREATECOMPRESSEDSAMPLE);
#pragma endregion


#pragma region NPC
	Load_SoundFile_GroupAsync(TEXT("FF7_NPC_Man"), (strMainPath + "FF7/Voice/NPC/Man/"), FMOD_3D | FMOD_CREATECOMPRESSEDSAMPLE);
	Load_SoundFile_GroupAsync(TEXT("FF7_NPC_Woman"), (strMainPath + "FF7/Voice/NPC/Woman/"), FMOD_3D | FMOD_CREATECOMPRESSEDSAMPLE);
	Load_SoundFile_GroupAsync(TEXT("FF7_NPC_GymLeader"), (strMainPath + "FF7/Voice/NPC/GymLeader/"), FMOD_3D | FMOD_CREATECOMPRESSEDSAMPLE);
	Load_SoundFile_GroupAsync(TEXT("FF7_NPC_GymMember"), (strMainPath + "FF7/Voice/NPC/GymMember/"), FMOD_3D | FMOD_CREATECOMPRESSEDSAMPLE);
#pragma endregion



	Wait_GroupAsync();

	return S_OK;
}

void CSound_Manager::Tick(_cref_time fTimeDelta)
{
	_float fBlendedValue[2] = { 1.f, 1.f };

	// 블렌딩 스왑 적용
	if (m_iActiveBGM_Channel == 0)
	{
		if (!m_fBGM_Blend.Decrease(fTimeDelta * m_fBGM_BlendSpeed) || m_fBGM_Blend.IsZero_Once())
		{
			m_pChannels[0]->setVolume(fBlendedValue[0] = (1.f - m_fBGM_Blend.Get_Percent()));
			m_pSystem->update();
		}
	}
	else
	{
		if (!m_fBGM_Blend.Increase(fTimeDelta * m_fBGM_BlendSpeed) || m_fBGM_Blend.IsMax_Once())
		{
			m_pChannels[1]->setVolume(fBlendedValue[1] = m_fBGM_Blend.Get_Percent());
			m_pSystem->update();
		}
	}

	// 페이드 아웃 적용
	if (m_bIsBGM_FadeOut)
	{
		if (m_fBGM_FadeOut.Increase(fTimeDelta * m_fBGM_FadeOutSpeed))
		{
			m_pChannels[0]->stop();
			m_pChannels[1]->stop();
			m_bIsBGM_FadeOut = false;
		}

		m_pChannels[1]->setVolume((1.f - m_fBGM_FadeOut.Get_Percent()) * fBlendedValue[0]);
		m_pChannels[0]->setVolume((1.f - m_fBGM_FadeOut.Get_Percent()) * fBlendedValue[1]);
		m_pSystem->update();
	}
}

CSound_Manager* CSound_Manager::Create(const string& strMainPath)
{
	ThisClass* pInstance = new ThisClass();

	if (FAILED(pInstance->Initialize(strMainPath)))
	{
		MSG_BOX("SoundMgr Create Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSound_Manager::Free()
{
	for (auto& pairSoundCtn : m_mapSound)
	{
		for (auto& pairSound : pairSoundCtn.second->Get_MapSound())
		{
			pairSound.second->release();
		}
	}
	m_mapSound.clear();

	m_pSystem->release();
	m_pSystem->close();
}


_int CSound_Manager::Play_Sound(
	const wstring& strGroupKey, const wstring& strSoundKey, 
	ESoundGroup eSoundGroup, _float fVolume, FMOD::Channel** ppOutChannel)
{
	// 사운드 찾기
	FMOD::Sound* pSound = Find_Sound(strGroupKey, strSoundKey);
	if (nullptr == pSound)
		return -1;

	// 동적 재생을 위한 채널 범위 찾기
	_int iStart = 0, iEnd = 0;
	Get_RangeBySoundGroup(eSoundGroup, iStart, iEnd);

	m_pSystem->update();

	// 여기부터 사운드 플레이
	_int iChannelGroup = ECast(eSoundGroup);
	_int iChannel = -1;
	for (_int i = iStart; i <= iEnd; i++)
	{
		_bool bPlay = FALSE;
		// 플레이중이지 않으면 재생
		if (m_pChannels[i]->isPlaying(&bPlay))
		{
			m_pSystem->playSound(pSound, m_pChannelGroups[iChannelGroup], FALSE, &m_pChannels[i]);
			if (ppOutChannel != nullptr)
				(*ppOutChannel) = m_pChannels[i];
			iChannel = i;
			break;
		}
		else
			continue;
	}

	// 재생하면 추가 세팅
	if (-1 != iChannel)
	{
		m_pChannels[iChannel]->setMode(FMOD_DEFAULT);
		m_pChannels[iChannel]->setVolume(fVolume);

		m_pSystem->update();
	}

	return iChannel;
}

_int CSound_Manager::Play_3DSound(const wstring& strGroupKey, const wstring& strSoundKey, ESoundGroup eSoundGroup, 
	_float fVolume, _float3 vPos, FMOD::Channel** ppOutChannel)
{
	// 사운드 얻어오기
	FMOD::Sound* pSound = Find_Sound(strGroupKey, strSoundKey);
	if (nullptr == pSound)
		return -1;

	// 동적 그룹 선택
	_int iStart = 0, iEnd = 0;
	Get_RangeBySoundGroup(eSoundGroup, iStart, iEnd);		


	_int iChannelGroup = ECast(eSoundGroup);
	_int iChannel = -1;
	// 여기부터 사운드 플레이
	for (_int i = iStart; i <= iEnd; i++)
	{
		_bool bPlay = FALSE;
		// 플레이중이지 않으면 재생
		if (m_pChannels[i]->isPlaying(&bPlay))
		{
			m_pSystem->playSound(pSound, m_pChannelGroups[iChannelGroup], true, &m_pChannels[i]);
			if (ppOutChannel != nullptr)
				(*ppOutChannel) = m_pChannels[i];
			iChannel = i;
			break;
		}
		else
			continue;
		/*else
		{
			FMOD_Channel_Stop(m_pChannels[i]);
			FMOD_System_PlaySound(m_pSystem, pSound, m_pChannelGroups[iChannelGroup], FALSE, &m_pChannels[i]);
		}*/
	}
	
	if (-1 != iChannel)
	{
		FMOD_VECTOR vFmodPos = { vPos.x, vPos.y, vPos.z };
		FMOD_VECTOR vFmodVelocity = {};

		//m_pChannelGroups[iChannelGroup]->set3DAttributes(&vFmodPos, &vFmodVelocity);
		m_pChannels[iChannel]->setMode(FMOD_3D);
		m_pChannels[iChannel]->set3DAttributes(&vFmodPos, &vFmodVelocity);
		m_pChannels[iChannel]->setVolume(fVolume);

		m_pChannels[iChannel]->setPaused(false);

		m_pSystem->update();
	}

	return iChannel;
}

_int CSound_Manager::Play_3DSound(const wstring& strGroupKey, vector<string>& strSoundKeys, ESoundGroup eSoundGroup
	, shared_ptr<CGameObject> pOwner, _float3 vOffsetPos
	, _float fVolume, FMOD::Channel** ppOutChannel)
{
	// 랜덤
	wstring strSoundName = ConvertToWstring(GI()->RandomString(strSoundKeys));

	// 사운드 얻어오기
	FMOD::Sound* pSound = Find_Sound(strGroupKey, strSoundName);
	if (nullptr == pSound)
		return -1;

	// 동적 그룹 선택
	_int iStart = 0, iEnd = 0;
	Get_RangeBySoundGroup(eSoundGroup, iStart, iEnd);

	_float3 vPos = {};
	if (pOwner)
	{
		vPos = pOwner->Get_TransformCom().lock()->Get_State(CTransform::STATE_POSITION);
	}

	_int iChannelGroup = ECast(eSoundGroup);
	_int iChannel = -1;
	// 여기부터 사운드 플레이
	for (_int i = iStart; i <= iEnd; i++)
	{
		_bool bPlay = FALSE;
		// 플레이중이지 않으면 재생
		if (m_pChannels[i]->isPlaying(&bPlay))
		{
			m_pSystem->playSound(pSound, m_pChannelGroups[iChannelGroup], true, &m_pChannels[i]);
			if (ppOutChannel != nullptr)
				ppOutChannel = &m_pChannels[i];
			iChannel = i;
			break;
		}
		else
			continue;
		/*else
		{
			FMOD_Channel_Stop(m_pChannels[i]);
			FMOD_System_PlaySound(m_pSystem, pSound, m_pChannelGroups[iChannelGroup], FALSE, &m_pChannels[i]);
		}*/
	}

	if (-1 != iChannel)
	{
		FMOD_VECTOR vFmodPos = { vPos.x, vPos.y, vPos.z };
		FMOD_VECTOR vFmodVelocity = {};

		//m_pChannelGroups[iChannelGroup]->set3DAttributes(&vFmodPos, &vFmodVelocity);
		m_pChannels[iChannel]->setMode(FMOD_3D);
		m_pChannels[iChannel]->set3DAttributes(&vFmodPos, &vFmodVelocity);
		m_pChannels[iChannel]->setVolume(fVolume);

		m_pChannels[iChannel]->setPaused(false);

		m_pSystem->update();
	}

	return iChannel;
}

_int CSound_Manager::Play_3DSound(vector<string>& strGroupKey, vector<string>& strSoundKeys, 
	ESoundGroup eSoundGroup, shared_ptr<CGameObject> pOwner, 
	_float3 vOffsetPos, _float fVolume, 
	FMOD::Channel** ppOutChannel)
{
	// 랜덤
	_uint iNumSound = GI()->RandomInt(0, (_int)strGroupKey.size());
	wstring strGroupName = ConvertToWstring(strGroupKey[iNumSound]);
	wstring strSoundName = ConvertToWstring(strSoundKeys[iNumSound]);

	// 사운드 얻어오기
	FMOD::Sound* pSound = Find_Sound(strGroupName, strSoundName);
	if (nullptr == pSound)
		return -1;

	// 동적 그룹 선택
	_int iStart = 0, iEnd = 0;
	Get_RangeBySoundGroup(eSoundGroup, iStart, iEnd);

	_float3 vPos = {};
	if (pOwner)
	{
		vPos = pOwner->Get_TransformCom().lock()->Get_State(CTransform::STATE_POSITION);
	}

	_int iChannelGroup = ECast(eSoundGroup);
	_int iChannel = -1;
	// 여기부터 사운드 플레이
	for (_int i = iStart; i <= iEnd; i++)
	{
		_bool bPlay = FALSE;
		// 플레이중이지 않으면 재생
		if (m_pChannels[i]->isPlaying(&bPlay))
		{
			m_pSystem->playSound(pSound, m_pChannelGroups[iChannelGroup], true, &m_pChannels[i]);
			if (ppOutChannel != nullptr)
				(*ppOutChannel) = m_pChannels[i];
			iChannel = i;
			break;
		}
		else
			continue;
		/*else
		{
			FMOD_Channel_Stop(m_pChannels[i]);
			FMOD_System_PlaySound(m_pSystem, pSound, m_pChannelGroups[iChannelGroup], FALSE, &m_pChannels[i]);
		}*/
	}

	if (-1 != iChannel)
	{
		FMOD_VECTOR vFmodPos = { vPos.x, vPos.y, vPos.z };
		FMOD_VECTOR vFmodVelocity = {};

		//m_pChannelGroups[iChannelGroup]->set3DAttributes(&vFmodPos, &vFmodVelocity);
		m_pChannels[iChannel]->setMode(FMOD_3D);
		m_pChannels[iChannel]->set3DAttributes(&vFmodPos, &vFmodVelocity);
		m_pChannels[iChannel]->setVolume(fVolume);

		m_pChannels[iChannel]->setPaused(false);

		m_pSystem->update();
	}

	return iChannel;
}

_int CSound_Manager::Play_3DSound(vector<string> strGroupKeys, vector<string> strSoundKeys, ESoundGroup eSoundGroup
	, _float3 vPos, _float fVolume, _float f3DMin, _float f3DMax, FMOD_MODE eRollOffMode, FMOD::Channel** ppOutChannel)
{
	if (strGroupKeys.size() == 0)
		return -1;

	// 랜덤
	_uint iNumSound = GI()->RandomInt(0, (_int)strGroupKeys.size() - 1);
	wstring strGroupName = ConvertToWstring(strGroupKeys[iNumSound]);
	wstring strSoundName = ConvertToWstring(strSoundKeys[iNumSound]);

	// 사운드 얻어오기
	FMOD::Sound* pSound = Find_Sound(strGroupName, strSoundName);
	if (nullptr == pSound)
		return -1;

	// 동적 그룹 선택
	_int iStart = 0, iEnd = 0;
	Get_RangeBySoundGroup(eSoundGroup, iStart, iEnd);

	_int iChannelGroup = ECast(eSoundGroup);
	_int iChannel = -1;
	// 여기부터 사운드 플레이
	for (_int i = iStart; i <= iEnd; i++)
	{
		_bool bPlay = FALSE;
		// 플레이중이지 않으면 재생
		if (m_pChannels[i]->isPlaying(&bPlay))
		{
			m_pSystem->playSound(pSound, m_pChannelGroups[iChannelGroup], true, &m_pChannels[i]);
			if (ppOutChannel != nullptr)
				(*ppOutChannel) = m_pChannels[i];
			iChannel = i;
			break;
		}
		else
			continue;
		/*else
		{
			FMOD_Channel_Stop(m_pChannels[i]);
			FMOD_System_PlaySound(m_pSystem, pSound, m_pChannelGroups[iChannelGroup], FALSE, &m_pChannels[i]);
		}*/
	}

	if (-1 != iChannel)
	{
		FMOD_VECTOR vFmodPos = { vPos.x, vPos.y, vPos.z };
		FMOD_VECTOR vFmodVelocity = {};

		//m_pChannelGroups[iChannelGroup]->set3DAttributes(&vFmodPos, &vFmodVelocity);
		m_pChannels[iChannel]->setMode(FMOD_3D | eRollOffMode);
		m_pChannels[iChannel]->set3DAttributes(&vFmodPos, &vFmodVelocity);
		m_pChannels[iChannel]->set3DMinMaxDistance(f3DMin, f3DMax);
		m_pChannels[iChannel]->setVolume(fVolume);

		m_pChannels[iChannel]->setPaused(false);

		m_pSystem->update();
	}

	return iChannel;
}

_bool CSound_Manager::IsPlaying_Channel(_int* pChannelIndex, FMOD::Channel** ppChannel)
{
	// 채널 자체가 없음
	if (*ppChannel == nullptr || *pChannelIndex == -1)
		return false;

	// 채널이 죽었음
	if (m_pChannels[*pChannelIndex] != *ppChannel)
		return false;

	_bool bIsPlaying = false;
	(*ppChannel)->isPlaying(&bIsPlaying);

	return bIsPlaying;
}

void CSound_Manager::Sound_Move(_int* pChannelIndex, FMOD::Channel** ppChannel, _float3 vPos)
{
	// 위치 변경
	if (IsPlaying_Channel(pChannelIndex, ppChannel))
	{
		FMOD_VECTOR vFmPos = { vPos.x, vPos.y, vPos.z };
		FMOD_VECTOR vFmVel = {};
		(*ppChannel)->set3DAttributes(&vFmPos, &vFmVel);
	}
}

void CSound_Manager::Sound_StopByChannel(_int* pChannelIndex, FMOD::Channel** ppChannel)
{
	// 사운드 정지
	if (IsPlaying_Channel(pChannelIndex, ppChannel))
	{
		(*ppChannel)->stop();
		(*ppChannel) = nullptr;
		(*pChannelIndex) = -1;
	}
}

_int CSound_Manager::Play_BGM(const wstring& strGroupKey, const wstring& strSoundKey, _float fVolume)
{
	FMOD::Sound* pSound = Find_Sound(strGroupKey, strSoundKey);
	if (nullptr == pSound)
		return -1;
	
	// 재생중이던 BGM을 중지시키고 새로운 BGM을 재생시킨다.
	Stop_BGM_All();
	m_pSystem->playSound(pSound, m_pChannelGroups[ECast(ESoundGroup::BGM)], FALSE, &m_pChannels[0]);
	m_pChannels[0]->setMode(FMOD_LOOP_NORMAL);
	m_pChannels[0]->setVolume(fVolume);

	m_pSystem->update();

	return 0;
}

_int CSound_Manager::Play_BGM_LoopRange(const wstring& strGroupKey, const wstring& strSoundKey,
	_float fVolume, _uint fStart, _uint fEnd)
{
	FMOD::Sound* pSound = Find_Sound(strGroupKey, strSoundKey);
	if (nullptr == pSound)
		return -1;

	// 여기부터 사운드 플레이
	Stop_BGM_All();
	m_pSystem->playSound(pSound, m_pChannelGroups[ECast(ESoundGroup::BGM)], FALSE, &m_pChannels[0]);
	m_pChannels[0]->setMode(FMOD_LOOP_NORMAL);
	m_pChannels[0]->setVolume(fVolume);
	m_pChannels[0]->setLoopPoints(fStart, FMOD_TIMEUNIT_MS, fEnd, FMOD_TIMEUNIT_MS);

	m_pSystem->update();

	return 0;
}

void CSound_Manager::FadeOut_BGM(_float fFadeOutSpeed)
{
	m_bIsBGM_FadeOut = true;
	m_fBGM_FadeOutSpeed = fFadeOutSpeed;
	m_fBGM_FadeOut.Reset();
}

void CSound_Manager::Swap_BGM(_float fBlendSpeed, _bool bSwapImmediate)
{
	m_fBGM_BlendSpeed = fBlendSpeed;
	if (bSwapImmediate)
	{
		if (m_iActiveBGM_Channel == 0)
			m_fBGM_Blend.Set_Max();
		else
			m_fBGM_Blend.Reset();
	}
}

void CSound_Manager::Stop_Sound(_int iChannel)
{
	m_pChannels[iChannel]->stop();
	m_pSystem->update();
}

void CSound_Manager::Stop_BGM_Front()
{
	m_pChannels[0]->stop();
	m_pSystem->update();
}

void CSound_Manager::Stop_BGM_Back()
{
	m_pChannels[1]->stop();
	m_pSystem->update();
}

void CSound_Manager::Stop_BGM_All()
{
	m_pChannelGroups[ECast(ESoundGroup::BGM)]->stop();
	m_pSystem->update();
}

void CSound_Manager::Stop_All()
{
	m_pMasterChanelGroup->stop();
	m_pSystem->update();
	/*for (int i = 0; i < g_iMaxSoundChannel; ++i)
		FMOD_Channel_Stop(m_pChannels[i]);*/
}

void CSound_Manager::Set_ChannelVolume(_int eID, float fVolume)
{
	m_pChannels[eID]->setVolume(fVolume);

	m_pSystem->update();
}

FMOD::Sound* CSound_Manager::Find_Sound(const wstring& strGroupKey, const wstring& strSoundKey)
{
	// 카테고리 키
	auto iterCate = m_mapSound.find(strGroupKey);
	if (iterCate == m_mapSound.end())
		return nullptr;

	// 사운드 키
	auto iter = iterCate->second->Get_MapSound().find(strSoundKey);
	if (iter == iterCate->second->Get_MapSound().end())
		return nullptr;

	return iter->second;
}

void CSound_Manager::Get_RangeBySoundGroup(ESoundGroup eSoundGroup, _int& iStart, _int& iEnd)
{
	switch (eSoundGroup)
	{
	case ESoundGroup::BGM:
		assert(false);	// 죽어
		break;
	case ESoundGroup::Environment:
		iStart = ECast(ESoundGroupRange::Environment_Start);
		iEnd = ECast(ESoundGroupRange::Environment_End);
		break;
	case ESoundGroup::UI:
		iStart = ECast(ESoundGroupRange::UI_Start);
		iEnd = ECast(ESoundGroupRange::UI_End);
		break;
	case ESoundGroup::Narration:
		iStart = ECast(ESoundGroupRange::Narration_Start);
		iEnd = ECast(ESoundGroupRange::Narration_End);
		break;
	case ESoundGroup::Voice:
		iStart = ECast(ESoundGroupRange::Voice_Start);
		iEnd = ECast(ESoundGroupRange::Voice_End);
		break;
	case ESoundGroup::Effect:
		iStart = ECast(ESoundGroupRange::Effect_Start);
		iEnd = ECast(ESoundGroupRange::Effect_End);
		break;
	default:
		break;
	}
}

void CSound_Manager::Load_SoundFile(const wstring& pGroupKey, const string& pPath, FMOD_MODE eFlage)
{
	// m_mapSound 보호
	m_mtxSound.lock();
	// 여기는 카테고리 키 만들기, 없으면 키를 만들고 컨테이너를 만들어 준다.
	auto iter = m_mapSound.find(pGroupKey);
	if (iter == m_mapSound.end())
	{
		m_mapSound.emplace(pGroupKey, FSoundData::Create());
	}
	m_mtxSound.unlock();

	// 여기부터 파일 로드부
	_char sText[128] = "";
	strcpy_s(sText, pPath.c_str());
	strcat_s(sText, "*.*");

	// _finddata_t : <io.h>에서 제공하며 파일 정보를 저장하는 구조체
	_finddata_t fd;

	// _findfirst : <io.h>에서 제공하며 사용자가 설정한 경로 내에서 가장 첫 번째 파일을 찾는 함수
	auto handle = _findfirst(sText, &fd);

	if (handle == -1)
		return;

	int iResult = 0;

	char szCurPath[MAX_PATH] = "";
	char szFullPath[MAX_PATH] = "";

	strcpy_s(szCurPath, pPath.c_str());

	// 이게 뭐게 ㅋ
	using sound_tuple = tuple<FMOD::Sound*, FMOD_RESULT, string, string>;
	enum ETMP_SOUND : int {
		TMP_SOUND,
		TMP_RESULT,
		TMP_PATH,
		TMP_FILE_NAME
	};

	// 비동기 처리는 쓰레드를 사용하기 때문에 모든 데이터를 별도의 컨테이너에 저장하고 처리해줍니다.
	vector<future<FMOD_RESULT>> vecAsync;
	vector<sound_tuple> vecSoundData;
	
	// 데이터 입력부
	while (iResult != -1)
	{
		strcpy_s(szFullPath, szCurPath);

		// "../Sound/Success.wav"
		strcat_s(szFullPath, fd.name);
		sound_tuple tpSound;

		// 쓰레드에 넘겨줄 데이터를 입력
		get<TMP_PATH>(tpSound) = string(szFullPath);
		get<TMP_FILE_NAME>(tpSound) = string(fd.name);
		get<TMP_RESULT>(tpSound) = FMOD_OK;
		get<TMP_SOUND>(tpSound) = nullptr;

		vecSoundData.push_back(tpSound);

		//_findnext : <io.h>에서 제공하며 다음 위치의 파일을 찾는 함수, 더이상 없다면 -1을 리턴
		iResult = _findnext(handle, &fd);
	}

	// 쓰레드 실행
	for (size_t i = 0; i < vecSoundData.size(); i++)
	{
		vecAsync.push_back(async(launch::async, &CSound_Manager::LoadSoundFile_Async, this
									, get<TMP_PATH>(vecSoundData[i]).c_str(), get<TMP_FILE_NAME>(vecSoundData[i]).c_str()
									, ref(get<TMP_RESULT>(vecSoundData[i])), &get<TMP_SOUND>(vecSoundData[i])
									, eFlage));
	}

	// 결과 받아오기
	for (size_t i = 0; i < vecSoundData.size(); i++)
	{
		vecAsync[i].get();

		m_mtxSound.lock();
		if (get<TMP_RESULT>(vecSoundData[i]) == FMOD_OK)
		{
			int iLength = (int)strlen(get<TMP_FILE_NAME>(vecSoundData[i]).c_str()) + 1;

			_tchar* pSoundKey = new _tchar[iLength];
			ZeroMemory(pSoundKey, sizeof(_tchar) * iLength);

			// 아스키 코드 문자열을 유니코드 문자열로 변환시켜주는 함수
			MultiByteToWideChar(CP_ACP, 0, get<TMP_FILE_NAME>(vecSoundData[i]).c_str(), iLength, pSoundKey, iLength);

			m_mapSound[pGroupKey]->Get_MapSound().emplace(pSoundKey, get<TMP_SOUND>(vecSoundData[i]));

			Safe_Delete_Array(pSoundKey);
		}
		m_mtxSound.unlock();
	}

	// FMOD 업데이트
	m_pSystem->update();

	_findclose(handle);
}

void CSound_Manager::Load_SoundFile_GroupAsync(const wstring& pGroupKey, const string& pPath, FMOD_MODE eFlag)
{
	m_vecAsyncSoundGroup.push_back(async(launch::async, &CSound_Manager::Load_SoundFile, this, pGroupKey, pPath, eFlag));
}

void CSound_Manager::Wait_GroupAsync()
{
	for (auto iter = m_vecAsyncSoundGroup.begin(); iter != m_vecAsyncSoundGroup.end();)
	{
		auto& Future = (*iter);
		auto state = Future.wait_for(1ms);
		if (state == future_status::ready)
		{
			Future.get();
			iter = m_vecAsyncSoundGroup.erase(iter);

			// 처음으로 돌아가기
			if (iter == m_vecAsyncSoundGroup.end())
			{
				iter = m_vecAsyncSoundGroup.begin();
				continue;
			}
		}
		else if (state == future_status::timeout)
		{
			iter = m_vecAsyncSoundGroup.erase(iter);
			continue;
		}

		if (++iter == m_vecAsyncSoundGroup.end())
			iter = m_vecAsyncSoundGroup.begin();
	}
	m_vecAsyncSoundGroup.clear();
}

FMOD_RESULT CSound_Manager::LoadSoundFile_Async(const string& pPath, const string& pFileName, 
	FMOD_RESULT& hResult, FMOD::Sound** pSound, FMOD_MODE eMode)
{
	hResult = m_pSystem->createSound(pPath.c_str(), eMode, NULL, pSound);

	return hResult;
}

vector<wstring> CSound_Manager::Provide_GroupSoundNames(const wstring& strGroupKey) const
{
	auto iter = m_mapSound.find(strGroupKey);
	if (iter == m_mapSound.end())
		return vector<wstring>();

	FSoundData* pSound = (*iter).second.get();
	if (pSound == nullptr)
		return vector<wstring>();

	vector<wstring> Result;
	for (auto iterSound = pSound->Get_MapSound().begin(); iterSound != pSound->Get_MapSound().end(); ++iterSound)
	{
		Result.push_back((*iterSound).first);
	}

	return Result;
}

_float CSound_Manager::Get_MasterVolume() const
{
	_float fVolume = 0.f;
	m_pMasterChanelGroup->getVolume(&fVolume);
	return fVolume;
}

void CSound_Manager::NormalModeDSP()
{
	if (m_eDspMode == EDspMode::Normal)
		return;

	m_pChannelGroups[ECast(ESoundGroup::Voice)]->removeDSP(m_pChannelGroupDSPs["Voice_Echo"]);
	m_pChannelGroups[ECast(ESoundGroup::Effect)]->removeDSP(m_pChannelGroupDSPs["Effect_Reverb"]);
	m_pChannelGroups[ECast(ESoundGroup::Effect)]->removeDSP(m_pChannelGroupDSPs["Effect_Echo"]);
	m_pChannelGroups[ECast(ESoundGroup::Effect)]->removeDSP(m_pChannelGroupDSPs["Effect_ThreeEq"]);

	m_pChannelGroups[ECast(ESoundGroup::Effect)]->setPitch(1.f);

	m_eDspMode = EDspMode::Normal;
}


void CSound_Manager::WaitModeDSP()
{
	if (m_eDspMode == EDspMode::Wait)
		return;

	_int iEchoIndex = -1;
	m_pChannelGroups[ECast(ESoundGroup::Voice)]->getDSPIndex(m_pChannelGroupDSPs["Voice_Echo"], &iEchoIndex);
	if (iEchoIndex != -1)
		m_pChannelGroups[ECast(ESoundGroup::Voice)]->addDSP(1, m_pChannelGroupDSPs["Voice_Echo"]);

	_int iEffectReverbIndex = -1;
	m_pChannelGroups[ECast(ESoundGroup::Effect)]->getDSPIndex(m_pChannelGroupDSPs["Effect_Reverb"], &iEffectReverbIndex);
	if (iEffectReverbIndex != -1)
		m_pChannelGroups[ECast(ESoundGroup::Effect)]->addDSP(1, m_pChannelGroupDSPs["Effect_Reverb"]);

	_int iEffectEchoIndex = -1;
	m_pChannelGroups[ECast(ESoundGroup::Effect)]->getDSPIndex(m_pChannelGroupDSPs["Effect_Echo"], &iEffectEchoIndex);
	if (iEffectEchoIndex != -1)
		m_pChannelGroups[ECast(ESoundGroup::Effect)]->addDSP(1, m_pChannelGroupDSPs["Effect_Echo"]);

	_int iThreeEqIndex = -1;
	m_pChannelGroups[ECast(ESoundGroup::Effect)]->getDSPIndex(m_pChannelGroupDSPs["Effect_ThreeEq"], &iThreeEqIndex);
	if (iThreeEqIndex != -1)
		m_pChannelGroups[ECast(ESoundGroup::Effect)]->addDSP(2, m_pChannelGroupDSPs["Effect_ThreeEq"]);

	m_pChannelGroups[ECast(ESoundGroup::Effect)]->setPitch(0.6f);

	m_eDspMode = EDspMode::Wait;
}



void CSound_Manager::Set_MasterVolume(_float fVolume)
{
	m_pMasterChanelGroup->setVolume(fVolume);
	m_pSystem->update();
}

_float CSound_Manager::Get_BGMVolume() const
{
	_float fVolume = 0.f;
	m_pChannelGroups[ECast(ESoundGroup::BGM)]->getVolume(&fVolume);
	return fVolume;
}

void CSound_Manager::Set_BGMVolume(_float fVolume)
{
	m_pChannelGroups[ECast(ESoundGroup::BGM)]->setVolume(fVolume);
	m_pSystem->update();
}

_float CSound_Manager::Get_VoiceVolume() const
{
	_float fVolume = 0.f;
	m_pChannelGroups[ECast(ESoundGroup::Voice)]->getVolume(&fVolume);
	return fVolume;
}

void CSound_Manager::Set_VoiceVolume(_float fVolume)
{
	m_pChannelGroups[ECast(ESoundGroup::Voice)]->setVolume(fVolume);
	m_pChannelGroups[ECast(ESoundGroup::Narration)]->setVolume(fVolume);
	m_pSystem->update();
}

_float CSound_Manager::Get_EffectVolume() const
{
	_float fVolume = 0.f;
	m_pChannelGroups[ECast(ESoundGroup::Effect)]->getVolume(&fVolume);
	return fVolume;
}

void CSound_Manager::Set_EffectVolume(_float fVolume)
{
	m_pChannelGroups[ECast(ESoundGroup::Environment)]->setVolume(fVolume);
	m_pChannelGroups[ECast(ESoundGroup::UI)]->setVolume(fVolume);
	m_pChannelGroups[ECast(ESoundGroup::Effect)]->setVolume(fVolume);
	m_pSystem->update();
}



void CSound_Manager::Set_ListenerAttributeByMatrix(_fmatrix Matrix)
{
	_float3 vPos, vLook, vUp;

	XMStoreFloat3(&vPos, Matrix.r[3]);
	XMStoreFloat3(&vLook, XMVector3Normalize(Matrix.r[2]));
	XMStoreFloat3(&vUp, XMVector3Normalize(Matrix.r[1]));

	vPos += vLook * m_ListenerLookLength;

	m_ListenerAttribute.velocity = {};
	memcpy(&m_ListenerAttribute.position, &vPos, sizeof(m_ListenerAttribute.position));
	memcpy(&m_ListenerAttribute.forward, &vLook, sizeof(m_ListenerAttribute.forward));
	memcpy(&m_ListenerAttribute.up, &vUp, sizeof(m_ListenerAttribute.up));

	m_pSystem->set3DListenerAttributes(0,
		&m_ListenerAttribute.position, &m_ListenerAttribute.velocity, &m_ListenerAttribute.forward, &m_ListenerAttribute.up);

	/*_float fDofflerScale, fDistanceFactor, fRollOffScale;
	FRapidJson Data = FRapidJson::Open_Json(GI()->MainJSONScriptPath() + L"Test.json");
	Data.Read_Data("DofflerScale", fDofflerScale);
	Data.Read_Data("DistanceFactor", fDistanceFactor);
	Data.Read_Data("RollOffScale", fRollOffScale);
	m_pSystem->set3DSettings(fDofflerScale, fDistanceFactor, fRollOffScale);*/

	m_pSystem->update();
}

