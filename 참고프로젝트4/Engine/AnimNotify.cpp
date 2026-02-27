#include "AnimNotify.h"
#include "GameInstance.h"


CAnimNotify::CAnimNotify()
	: m_pGI(GI())
{
	Safe_AddRef(m_pGI);
}

CAnimNotify::CAnimNotify(const CAnimNotify& rhs)
	: m_pGI(rhs.m_pGI)
	, m_fTrackPos(rhs.m_fTrackPos), m_strName(rhs.m_strName), m_bIsTriggered(rhs.m_bIsTriggered)
	, m_pNotifyData(rhs.m_pNotifyData)
{
	Safe_AddRef(m_pGI);
}

HRESULT CAnimNotify::Initialize(_float fTrackPos, const string& strName)
{
	m_fTrackPos = fTrackPos;
	m_strName = strName;

	return S_OK;
}

void CAnimNotify::OnReset()
{
	m_bIsTriggered = false;
}

_bool CAnimNotify::OnTrigger()
{
	if (m_bIsTriggered)
		return false;

	m_bIsTriggered = true;

	if (!m_pNotifyData->Event.empty())
		m_pNotifyData->Event();

	return true;
}

void CAnimNotify::Free()
{
	__super::Free();

	Safe_Release(m_pGI);
	Safe_Delete(m_pNotifyData);
}

FRapidJson CAnimNotify::Bake_Notify()
{
	FRapidJson Data;

	Data.Write_Data("TrackPos", m_fTrackPos);
	Data.Write_StringData("NotifyName", m_strName);

	return Data;
}

void CAnimNotify::Set_NotifyTrackPos(const _float fTrackPos)
{
	m_fTrackPos = fTrackPos;
}

HRESULT CAnimNotify::Set_Event(NotifyEventDelegate0 Event)
{
	if (m_pNotifyData == nullptr)
		RETURN_EFAIL;

	m_pNotifyData->Event = Event;

	return S_OK;
}




#pragma region 노티파이 컬렉션

CAnimNotifyCollection::CAnimNotifyCollection()
{
}

CAnimNotifyCollection::CAnimNotifyCollection(const CAnimNotifyCollection& rhs)
	: m_fPrevTrackPos(rhs.m_fPrevTrackPos)
	, m_Notifies(rhs.m_Notifies)
{
	for (auto iter = m_Notifies.begin(); iter != m_Notifies.end(); ++iter)
	{
		auto& TrackNotifies = (*iter).second;
		for (auto iterNotifies = TrackNotifies.begin(); iterNotifies != TrackNotifies.end(); ++iterNotifies)
		{
			(*iterNotifies) = (*iterNotifies)->Clone();
			(*iterNotifies)->Set_NotifyCollection(this);
		}
	}
}

void CAnimNotifyCollection::OnReset()
{
	// 모든 노티파이를 원래 상태로 되돌림. 다시 트리거를 작동시킬 수 있게됨.
	for (auto iter = m_Notifies.begin(); iter != m_Notifies.end(); ++iter)
	{
		for (_uint i = 0; i < (*iter).second.size(); i++)
			(*iter).second[i]->OnReset();
	}
	m_fPrevTrackPos = 0.f;
}

void CAnimNotifyCollection::OnTrigger(_cref_time fTrackPos)
{
	if (m_Notifies.empty())
		return;

	// 방향성 체크
	_int iSign = Cast<_int>(fTrackPos - m_fPrevTrackPos);

	// 모든 컬렉션 순회
	for (auto iter = m_Notifies.begin(); iter != m_Notifies.end(); ++iter)
	{
		vector<shared_ptr<CAnimNotify>>& NotifyVector = (*iter).second;
		if (NotifyVector.empty())
			continue;

		// 정렬되어 있다는 전제하에 노티파이 체크 위치 설정, lower_bound로 빠른 검색
		// 방향성이 정방향
		if (iSign >= 0)
		{
			_float fMin = m_fPrevTrackPos;
			auto iterNotify = lower_bound(NotifyVector.begin(), NotifyVector.end(), fMin,
				[](const auto& pNotify, const _float& fMin) {
					return pNotify->Get_NotifyTrackPos() < fMin;
				});

			// 시간의 흐름에 따라 모든 트리거를 발동시킨다.
			if (iterNotify != NotifyVector.end())
			{
				const _float& fRefTrackPos = (*iterNotify)->Get_NotifyTrackPos();
				// 양방향 지원
				if (m_fPrevTrackPos <= fRefTrackPos && fRefTrackPos <= fTrackPos)
					(*iterNotify)->OnTrigger();
				//else { break; }

				// 방향성에 맞게 반복자 체크
				//++iterNotify;
			}
		}
		// 방향성이 반대
		//else
		//{
		//	_float fMax = m_fPrevTrackPos;
		//	auto iterNotify = lower_bound(NotifyVector.rbegin(), NotifyVector.rend(), fMax,
		//		[](const auto& pNotify, const _float& fMax) {
		//			return pNotify->Get_NotifyTrackPos() > fMax;
		//		});

		//	// 시간의 흐름에 따라 모든 트리거를 발동시킨다.
		//	while (iterNotify != NotifyVector.rend())
		//	{
		//		const _float& fRefTrackPos = (*iterNotify)->Get_NotifyTrackPos();
		//		// 양방향 지원
		//		if (fTrackPos <= fRefTrackPos && fRefTrackPos <= m_fPrevTrackPos)
		//			(*iterNotify)->OnTrigger();
		//		else { break; }

		//		// 방향성에 맞게 반복자 체크
		//		++iterNotify;
		//	}
		//}
	}

	m_fPrevTrackPos = fTrackPos;
}

void CAnimNotifyCollection::Free()
{
	__super::Free();
}

void CAnimNotifyCollection::Add_Notify(const string& strNotifyTag, shared_ptr<CAnimNotify>& pNotify)
{
	auto iter = m_Notifies.find(strNotifyTag);
	if (iter == m_Notifies.end())
		m_Notifies.emplace(strNotifyTag, vector<shared_ptr<CAnimNotify>>());

	m_Notifies[strNotifyTag].push_back(pNotify);
	pNotify->Set_NotifyCollection(this);
}

void CAnimNotifyCollection::Regist_ModelComponent(weak_ptr<CCommonModelComp> pModelCom)
{
	m_pModelCom = pModelCom;
}

void CAnimNotifyCollection::Reserve_Collection(const string& strNotifyTag)
{
	auto iter = m_Notifies.find(strNotifyTag);
	if (iter == m_Notifies.end())
		m_Notifies.emplace(strNotifyTag, vector<shared_ptr<CAnimNotify>>());
}

void CAnimNotifyCollection::Sort_Notifies()
{
	// 툴에서 노티파이를 정렬하기 위해 쓰이는 기능
	for (auto iter = m_Notifies.begin(); iter != m_Notifies.end(); ++iter)
	{
		auto& NotifyVector = (*iter).second;

		sort(NotifyVector.begin(), NotifyVector.end(), [](const auto& left, const auto& right) {
			return left->Get_NotifyTrackPos() < right->Get_NotifyTrackPos();
			});
	}
}

FRapidJson CAnimNotifyCollection::Bake_Collection()
{
	FRapidJson Data;
	Data.Write_StringData("AnimName", m_strAnimName);
	for (auto iter = m_Notifies.begin(); iter != m_Notifies.end(); ++iter)
	{
		FRapidJson NotifyMap;
		NotifyMap.Write_StringData("Name", (*iter).first.c_str());
		for (_uint i = 0; i < (*iter).second.size(); i++)
		{
			NotifyMap.Pushback_ObjectToArray("Notifies", (*iter).second[i]->Bake_Notify());
		}
		Data.Pushback_ObjectToArray("Collections", NotifyMap);
	}

	return Data;
}

void CAnimNotifyCollection::Clear_Collections()
{
	m_Notifies.clear();
}

void CAnimNotifyCollection::Clear_OnlyNotifies()
{
	for (auto iter = m_Notifies.begin(); iter != m_Notifies.end(); ++iter)
	{
		(*iter).second.clear();
	}
}

vector<shared_ptr<CAnimNotify>>* CAnimNotifyCollection::Get_NotifyGroupPtr(const string& strGroupTag)
{
	auto iter = m_Notifies.find(strGroupTag);
	if (iter == m_Notifies.end())
		return nullptr;

	return &(*iter).second;
}

HRESULT CAnimNotifyCollection::Regist_EventToNotify(const string& strGroupTag, const string& strNotifyTag, FastDelegate0<> Event)
{
	auto iterGroup = m_Notifies.find(strGroupTag);
	if (iterGroup == m_Notifies.end())
		RETURN_EFAIL;

	auto iterNotify = find_if((*iterGroup).second.begin(), (*iterGroup).second.end(),
		[&strNotifyTag](const shared_ptr<CAnimNotify>& pNotify) {
			return pNotify->Get_Name() == strNotifyTag;
		});
	if (iterNotify == (*iterGroup).second.end())
		RETURN_EFAIL;

	(*iterNotify)->Set_Event(Event);

	return S_OK;
}

#pragma endregion

