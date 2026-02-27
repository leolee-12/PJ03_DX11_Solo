#pragma once

#include "Base.h"

#include "Utility/LogicDeviceBasic.h"

#include <mutex>
#include <future>
#include <thread>

#define SOUND_MANAGER_BGM FALSE		// FALSE : 비활성화, TRUE : 활성화


BEGIN(Engine)

class CGameObject;

const static _int g_iMaxSoundChannel = 1024;


enum class ESoundGroup
{
	BGM,			// BGM
	Environment,	// 환경용
	UI,				// UI용
	Voice,			// 일반 목소리용
	Narration,		// 나래이션용
	Effect,			// 이펙트용
	End
};

const _char* SoundGroupNames[] = {
	"BGM", "Environment", "UI", "Voice", "Narration", "Effect"
};

enum class ESoundGroupRange
{
	BGM_Start,
	BGM_End = 1,
	Environment_Start,
	Environment_End = 500,
	UI_Start,
	UI_End = 550,
	Narration_Start,
	Narration_End = 610,
	Voice_Start,
	Voice_End = 850,
	Effect_Start,
	Effect_End = 1023
};

class ENGINE_DLL FSoundData final : public CBase
{
	INFO_CLASS(FSoundData, CBase)
private:
	FSoundData() {}
	virtual ~FSoundData() {}

public:
	static shared_ptr<FSoundData> Create()
	{
		DERIVED* pInstance = new DERIVED;

		return BaseMakeShared(pInstance);
	}

	virtual void Free()
	{
		mapSound.clear();
	}

public:
	using SMapSound = _unmap<wstring, FMOD::Sound*>;
	GETSET_EX1(SMapSound, mapSound, MapSound, GET_REF)

private:
	SMapSound mapSound;
};

class FChannelGroupData final : public CBase
{

};


/// <summary>
/// 사운드 담당 클래스
/// </summary>
class CSound_Manager final : public CBase
{
	DERIVED_CLASS(CBase, CSound_Manager)

private:
	explicit CSound_Manager();
	explicit CSound_Manager(const CSound_Manager& rhs) = delete;
	virtual ~CSound_Manager() = default;

public:
	HRESULT		Initialize(const string& strMainPath);
	void		Tick(_cref_time fTimeDelta);

public:
	static CSound_Manager*	Create(const string& strMainPath);

private:
	virtual void		Free() override;
	


#pragma region 제어 함수
public:
	// 일반 적인 사운드 재생 (UI 등), 재생 채널을 반환함.
	_int Play_Sound(const wstring& strGroupKey, const wstring& strSoundKey,
		ESoundGroup eSoundGroup, _float fVolume, FMOD::Channel** ppOutChannel = nullptr);
	// 3D 사운드 재생 (게임 플레이 사운드 등)
	_int Play_3DSound(const wstring& strGroupKey, const wstring& strSoundKey,
		ESoundGroup eSoundGroup, _float fVolume, _float3 vPos, FMOD::Channel** ppOutChannel = nullptr);
	_int Play_3DSound(const wstring& strGroupKey, vector<string>& strSoundKeys,
		ESoundGroup eSoundGroup, shared_ptr<CGameObject> pOwner, _float3 vOffsetPos = _float3(0.f, 0.f, 0.f), _float fVolume = 1.f, FMOD::Channel** ppOutChannel = nullptr);
	
	_int Play_3DSound(vector<string>& strGroupKey, vector<string>& strSoundKeys,
		ESoundGroup eSoundGroup, shared_ptr<CGameObject> pOwner, _float3 vOffsetPos = _float3(0.f, 0.f, 0.f), _float fVolume = 1.f, FMOD::Channel** ppOutChannel = nullptr);

	_int Play_3DSound(vector<string> strGroupKeys, vector<string> strSoundKeys, ESoundGroup eSoundGroup
		, _float3 vPos
		, _float fVolume = 1.f, _float f3DMin = 0.5f, _float f3DMax = 50.f, FMOD_MODE eRollOffMode = FMOD_3D_LINEARSQUAREROLLOFF,
		FMOD::Channel** ppOutChannel = nullptr);

	_bool	IsPlaying_Channel(_int* pChannelIndex, FMOD::Channel** ppChannel);
	void	Sound_Move(_int* pChannelIndex, FMOD::Channel** ppChannel, _float3 vPos);
	void	Sound_StopByChannel(_int* pChannelIndex, FMOD::Channel** ppChannel);

	// 브금 재생
	_int Play_BGM(const wstring& strGroupKey, const wstring& strSoundKey, _float fVolume);
	_int Play_BGM_LoopRange(const wstring& strGroupKey, const wstring& strSoundKey,
							_float fVolume, _uint fStart, _uint fEnd);

	void FadeOut_BGM(_float fFadeOutSpeed = 1.f);
	// 배경음악 스위칭
	void Swap_BGM(_float fBlendSpeed = 1.f, _bool bSwapImmediate = false);

	// 일반 채널 사운드 정지
	void Stop_Sound(_int iChannel);
	// 배경음악, 필드용 정지
	void Stop_BGM_Front();
	// 배경음악, 스위칭용 정지
	void Stop_BGM_Back();
	// 배경음악 전체 정지
	void Stop_BGM_All();

	// 모든 사운드 정지
	void Stop_All();
	// 채널의 볼륨 설정
	void Set_ChannelVolume(_int eID, float fVolume);
#pragma endregion

private:
	FMOD::Sound* Find_Sound(const wstring& strGroupKey, const wstring& strSoundKey);
	void Get_RangeBySoundGroup(ESoundGroup eSoundGroup, _int& iStart, _int& iEnd);

private:
	_int		m_iActiveBGM_Channel = { 0 };	// 활성화된 (앞인) BGM 채널
	FGauge		m_fBGM_Blend = { 1.f };			// BGM 블렌드
	_float		m_fBGM_BlendSpeed = { 1.f };	// BGM 블렌드 스피드

	_bool		m_bIsBGM_FadeOut = { false };	// 페이드 아웃 중임.
	FGauge		m_fBGM_FadeOut = { 1.f };		// BGM 페이드 아웃
	_float		m_fBGM_FadeOutSpeed = { 1.f };	// 페이드 아웃 속도

#pragma region 로드 함수
private:
	// 내부적으로 로드할 사운드 파일 폴더를 지정해 로드하는 함수
	void Load_SoundFile(const wstring& pGroupKey, const string& pPath, FMOD_MODE eFlage);
	// 사운드 파일을 그룹 단위로 비동기 로드할 때 쓰이는 함수
	void Load_SoundFile_GroupAsync(const wstring& pGroupKey, const string& pPath, FMOD_MODE eFlage);
	// 비동기 처리를 기다리는 함수
	void Wait_GroupAsync();
	// 사운드 파일 단일로 비동기 로드할 때 쓰이는 함수ㅈ
	FMOD_RESULT LoadSoundFile_Async(const string& pPath, const string& pFileName,
		FMOD_RESULT& hResult, FMOD::Sound** pSound, FMOD_MODE eMode);
#pragma endregion



public:
	const _unmap<wstring, shared_ptr<FSoundData>>& CRef_SoundMap() const { return m_mapSound; }
	// 그룹의 모든 사운드 이름을 반환함
	vector<wstring> Provide_GroupSoundNames(const wstring& strGroupKey) const;

private:
	using SSoundDataMap = _unmap<wstring, shared_ptr<FSoundData>>;
	string					m_strPainPath;

	// 사운드 리소스 정보를 갖는 객체 
	SSoundDataMap			m_mapSound;
	mutex					m_mtxSound;
	vector<future<void>>	m_vecAsyncSoundGroup;
	


public:
	// 채널 그룹에 대한 볼륨 설정 및 설정값 가져오기
	_float	Get_MasterVolume() const;
	void	Set_MasterVolume(_float fVolume);
	_float	Get_BGMVolume() const;
	void	Set_BGMVolume(_float fVolume);
	_float	Get_VoiceVolume() const;
	void	Set_VoiceVolume(_float fVolume);
	_float	Get_EffectVolume() const;
	void	Set_EffectVolume(_float fVolume);

public:
	// 클라에 만들어야 어울리지만 임시로 여기서 설정한다.
	void NormalModeDSP();
	void WaitModeDSP();

private:
	enum class EDspMode { Normal, Wait };
	EDspMode		m_eDspMode = EDspMode::Normal;

public:
	using SDspMap = _unmap<string, FMOD::DSP*>;

private:
	// 사운드 ,채널 객체 및 장치를 관리하는 객체 
	FMOD::System*		m_pSystem = { nullptr };

	// 채널 그룹
	FMOD::ChannelGroup* m_pMasterChanelGroup = { nullptr };			// 마스터 채널그룹
	
	FMOD::ChannelGroup*	m_pChannelGroups[ECast(ESoundGroup::End)];	// 브금, 목소리, 효과음
	SDspMap				m_pChannelGroupDSPs;						// 디지털 음향 변조기

	// FMOD_CHANNEL : 재생하고 있는 사운드를 관리할 객체 
	FMOD::Channel*		m_pChannels[g_iMaxSoundChannel];			// 각 채널 (256개)
	

public:
	// 행렬을 통해 청자의 정보를 설정하는 함수
	void Set_ListenerAttributeByMatrix(_fmatrix Matrix);
	void Set_ListenerLookLength(_float fLength) { m_ListenerLookLength = fLength; }

private:
	FMOD_3D_ATTRIBUTES		m_ListenerAttribute = {};
	_float					m_ListenerLookLength = { 0.f };
};

END