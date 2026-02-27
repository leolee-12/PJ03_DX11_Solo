#include "stdafx.h"
#include "AnimNotify_Event.h"
#include "GameInstance.h"

CAnimNotify_Event::CAnimNotify_Event()
{
}

CAnimNotify_Event::CAnimNotify_Event(const CAnimNotify_Event& rhs)
	: BASE(rhs)
{
	if (m_pNotifyData != nullptr)
	{
		auto pClone = new FNotifyType_Event;
		(*pClone) = (*Cast<FNotifyType_Event*>(m_pNotifyData));
		m_pNotifyData = pClone;
	}
}

HRESULT CAnimNotify_Event::Initialize(_float fTrackPos, FNotifyType_Event Desc)
{
	m_pNotifyData = new FNotifyType_Event;
	FNotifyType_Event& NotiDat = (*Cast<FNotifyType_Event*>(m_pNotifyData));
	NotiDat.Event = Desc.Event;

	__super::Initialize(fTrackPos, Desc.strName);

	return S_OK;
}

HRESULT CAnimNotify_Event::Initialize(FRapidJson& InputData)
{
	return Input_RpJson(InputData);
}

void CAnimNotify_Event::OnReset()
{
	__super::OnReset();

}

_bool CAnimNotify_Event::OnTrigger()
{
	if (!__super::OnTrigger())
		return false;


	return true;
}

shared_ptr<CAnimNotify_Event> CAnimNotify_Event::Create(_float fTrackPos, FNotifyType_Event Desc)
{
	DERIVED* pInstance = new DERIVED();

	if (FAILED(pInstance->Initialize(fTrackPos, Desc)))
	{
		MSG_BOX("AnimNotify_Event : Failed");
		Safe_Release(pInstance);
	}

	return BaseMakeShared<DERIVED>(pInstance);
}

shared_ptr<CAnimNotify_Event> CAnimNotify_Event::Create(FRapidJson& InputData)
{
	DERIVED* pInstance = new DERIVED();

	if (FAILED(pInstance->Initialize(InputData)))
	{
		MSG_BOX("AnimNotify_Event : Failed");
		Safe_Release(pInstance);
	}

	return BaseMakeShared<DERIVED>(pInstance);
}

void CAnimNotify_Event::Free()
{
	__super::Free();

}

shared_ptr<CAnimNotify> CAnimNotify_Event::Clone()
{
	DERIVED* pInstance = new DERIVED(*this);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("AnimNotify_Event : Failed");
		Safe_Release(pInstance);
	}

	return DynPtrCast<CAnimNotify>(BaseMakeShared(pInstance));
}

FRapidJson CAnimNotify_Event::Bake_Notify()
{
	FRapidJson Data = __super::Bake_Notify();

	FNotifyType_Event& NotiDat = (*Cast<FNotifyType_Event*>(m_pNotifyData));
	if (&NotiDat == nullptr)
	{
		cerr << "ÆÄ½Ì ½ÇÆÐ" << endl;
		return FRapidJson();
	}

	Data.Write_Data("NotifyID", 2);

	return Data;
}

HRESULT CAnimNotify_Event::Input_RpJson(FRapidJson InputData)
{
	if (nullptr == m_pNotifyData)
		m_pNotifyData = new FNotifyType_Event;

	FNotifyType_Event& NotiDat = (*Cast<FNotifyType_Event*>(m_pNotifyData));
	
	InputData.Read_Data("TrackPos", m_fTrackPos);
	InputData.Read_Data("NotifyName", m_strName);

	return S_OK;
}

