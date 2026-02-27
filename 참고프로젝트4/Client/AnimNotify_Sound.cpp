#include "stdafx.h"
#include "AnimNotify_Sound.h"
#include "GameInstance.h"

#include "Sound_Manager.h"

CAnimNotify_Sound::CAnimNotify_Sound()
{
}

CAnimNotify_Sound::CAnimNotify_Sound(const CAnimNotify_Sound& rhs)
	: BASE(rhs)
{
	if (m_pNotifyData != nullptr)
	{
		auto pClone = new FNotifyType_Sound;
		(*pClone) = (*Cast<FNotifyType_Sound*>(m_pNotifyData));
		m_pNotifyData = pClone;
	}
}


HRESULT CAnimNotify_Sound::Initialize(_float fTrackPos, FNotifyType_Sound Desc)
{
	m_pNotifyData = new FNotifyType_Sound;
	FNotifyType_Sound& NotiDat = (*Cast<FNotifyType_Sound*>(m_pNotifyData));
	NotiDat.Event = Desc.Event;
	NotiDat.iSoundChannel = Desc.iSoundChannel;
	NotiDat.fSoundVolume = Desc.fSoundVolume;
	NotiDat.strSoundGroupTags = Desc.strSoundGroupTags;
	NotiDat.strSoundNameTags = Desc.strSoundNameTags;
	NotiDat.fRandom = Desc.fRandom;
	NotiDat.vOffsetPos = NotiDat.vOffsetPos;

	__super::Initialize(fTrackPos, Desc.strName);

	return S_OK;
}

HRESULT CAnimNotify_Sound::Initialize(FRapidJson& InputData)
{
	return Input_RpJson(InputData);
}

void CAnimNotify_Sound::OnReset()
{
	__super::OnReset();


}

_bool CAnimNotify_Sound::OnTrigger()
{
	if (!__super::OnTrigger())
		return false;

	if (m_pNotifyData == nullptr)
		return false;

	FNotifyType_Sound& NotiDat = (*Cast<FNotifyType_Sound*>(m_pNotifyData));

	auto pModelComp = m_pCollection->Get_ModelComp();
	if (pModelComp.expired())
	{
		// 툴에서 재생시 필요
		_float fRandom = m_pGI->Random(0.f, 100.f);
		if (0.f < fRandom && fRandom <= NotiDat.fRandom)
		{
			_int iIndex = m_pGI->RandomInt(0, Cast<_int>(NotiDat.strSoundGroupTags.size()) - 1);
			// 사운드 재생
			m_pGI->Play_3DSound(NotiDat.strSoundGroupTags[iIndex], NotiDat.strSoundNameTags[iIndex],
				(ESoundGroup)NotiDat.iSoundChannel, NotiDat.fSoundVolume, NotiDat.vOffsetPos);
		}

		return false;
	}

	auto pOwner = pModelComp.lock()->Get_Owner().lock();
	if (nullptr == pOwner)
		return false;

	_float fRandom = m_pGI->Random(0.f, 100.f);
	if (0.f < fRandom && fRandom <= NotiDat.fRandom)
	{
		_int iIndex = m_pGI->RandomInt(0, Cast<_int>(NotiDat.strSoundGroupTags.size()) - 1);
		if ((ESoundGroup)NotiDat.iSoundChannel == ESoundGroup::Narration)
		{
			m_pGI->Play_Sound(NotiDat.strSoundGroupTags[iIndex], NotiDat.strSoundNameTags[iIndex], (ESoundGroup)NotiDat.iSoundChannel, NotiDat.fSoundVolume);
		}
		else
		{
			// 사운드 재생
			m_pGI->Play_3DSound(NotiDat.strSoundGroupTags[iIndex], NotiDat.strSoundNameTags[iIndex],
				(ESoundGroup)NotiDat.iSoundChannel, NotiDat.fSoundVolume,
				pOwner->Get_TransformCom().lock()->Get_State(CTransform::STATE_POSITION) + NotiDat.vOffsetPos);
		}

		

		// 사운드 재생
		/*m_pGI->Play_Sound(NotiDat.strSoundGroupTags[iIndex], NotiDat.strSoundNameTags[iIndex],
			(ESoundGroup)NotiDat.iSoundChannel, NotiDat.fSoundVolume);*/
	}

	return true;
}

shared_ptr<CAnimNotify_Sound> CAnimNotify_Sound::Create(_float fTrackPos, FNotifyType_Sound Desc)
{
	DERIVED* pInstance = new DERIVED();

	if (FAILED(pInstance->Initialize(fTrackPos, Desc)))
	{
		MSG_BOX("Notify_Sound : Failed");
		Safe_Release(pInstance);
	}

	return BaseMakeShared<DERIVED>(pInstance);
}

shared_ptr<CAnimNotify_Sound> CAnimNotify_Sound::Create(FRapidJson& InputData)
{
	DERIVED* pInstance = new DERIVED();

	if (FAILED(pInstance->Initialize(InputData)))
	{
		MSG_BOX("Notify_Sound : Failed");
		Safe_Release(pInstance);
	}

	return BaseMakeShared<DERIVED>(pInstance);
}

void CAnimNotify_Sound::Free()
{
	__super::Free();


}

shared_ptr<CAnimNotify> CAnimNotify_Sound::Clone()
{
	DERIVED* pInstance = new DERIVED(*this);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("AnimNotify_Effect : Failed");
		Safe_Release(pInstance);
	}

	return DynPtrCast<CAnimNotify>(BaseMakeShared(pInstance));
}

FRapidJson CAnimNotify_Sound::Bake_Notify()
{
	FRapidJson Data = __super::Bake_Notify();

	FNotifyType_Sound& NotiDat = (*Cast<FNotifyType_Sound*>(m_pNotifyData));
	if (&NotiDat == nullptr)
	{
		cerr << "파싱 실패" << endl;
		return FRapidJson();
	}

	Data.Write_Data("NotifyID", 1);

	for (_uint i = 0; i < Cast<_uint>(NotiDat.strSoundGroupTags.size()); i++)
		Data.Pushback_StringToArray("SoundGroups", NotiDat.strSoundGroupTags[i]);

	for (_uint i = 0; i < Cast<_uint>(NotiDat.strSoundNameTags.size()); i++)
		Data.Pushback_StringToArray("SoundNames", NotiDat.strSoundNameTags[i]);

	Data.Write_Data("SoundChannel", NotiDat.iSoundChannel);
	Data.Write_Data("SoundVolume", NotiDat.fSoundVolume); 
	Data.Write_Data("SoundPlayPercent", NotiDat.fRandom);
	Data.Write_Data("OffsetPos", NotiDat.vOffsetPos);

	return Data;
}

HRESULT CAnimNotify_Sound::Input_RpJson(FRapidJson InputData)
{
	if (nullptr == m_pNotifyData)
		m_pNotifyData = new FNotifyType_Sound;
	
	
	FNotifyType_Sound& NotiDat = (*Cast<FNotifyType_Sound*>(m_pNotifyData));
	//NotiDat.Event = InputData.Event;
	InputData.Read_Data("TrackPos", m_fTrackPos);
	InputData.Read_Data("NotifyName", m_strName);

	_uint iNumSoundGroups = InputData.Read_ArraySize("SoundGroups");
	NotiDat.strSoundGroupTags.resize(iNumSoundGroups);
	for (_uint i = 0; i < iNumSoundGroups; i++)
		InputData.Read_DataFromArray("SoundGroups", i, NotiDat.strSoundGroupTags[i]);

	_uint iNumSoundNames = InputData.Read_ArraySize("SoundNames");
	NotiDat.strSoundNameTags.resize(iNumSoundNames);
	for (_uint i = 0; i < iNumSoundNames; i++)
		InputData.Read_DataFromArray("SoundNames", i, NotiDat.strSoundNameTags[i]);

	InputData.Read_Data("SoundChannel", NotiDat.iSoundChannel);
	InputData.Read_Data("SoundVolume", NotiDat.fSoundVolume);
	InputData.Read_Data("SoundPlayPercent", NotiDat.fRandom);
	InputData.Read_Data("OffsetPos", NotiDat.vOffsetPos);

	return S_OK;
}