#include "stdafx.h"
#include "AnimNotifyCollection_Client.h"

#include "AnimNotify_Effect.h"
#include "AnimNotify_Event.h"
#include "AnimNotify_Sound.h"

CAnimNotifyCollection_Client::CAnimNotifyCollection_Client()
{
}

CAnimNotifyCollection_Client::CAnimNotifyCollection_Client(const CAnimNotifyCollection_Client& rhs)
	: BASE(rhs)
{
}

shared_ptr<CAnimNotifyCollection_Client> CAnimNotifyCollection_Client::Create()
{
	DERIVED* pInstance = new DERIVED;

	if (!pInstance)
	{
		MSG_BOX("AnimNotifyCollection : Create Failed");
		Safe_Release(pInstance);
	}

	return BaseMakeShared(pInstance);
}

shared_ptr<CAnimNotifyCollection> CAnimNotifyCollection_Client::Create_Callback()
{
	DERIVED* pInstance = new DERIVED;

	if (!pInstance)
	{
		MSG_BOX("AnimNotifyCollection : Create Failed");
		Safe_Release(pInstance);
	}

	return DynPtrCast<CAnimNotifyCollection>(BaseMakeShared(pInstance));
}

void CAnimNotifyCollection_Client::Free()
{
	__super::Free();
}

shared_ptr<CAnimNotifyCollection> CAnimNotifyCollection_Client::Clone()
{
	DERIVED* pInstance = new DERIVED(*this);

	if (!pInstance)
	{
		MSG_BOX("AnimNotifyCollection : Create Failed");
		Safe_Release(pInstance);
	}

	return DynPtrCast<CAnimNotifyCollection>(BaseMakeShared(pInstance));
}

HRESULT CAnimNotifyCollection_Client::Input_Json(FRapidJson& InputData)
{
	// 노티파이 해제, 리로드
	Clear_OnlyNotifies();

	_uint iCollectionSize = InputData.Read_ArraySize("Collections");
	for (_uint i = 0; i < iCollectionSize; i++)
	{
		FRapidJson CollectionData;
		InputData.Read_ObjectFromArray("Collections", i, CollectionData);

		string strCollectionName = "";
		CollectionData.Read_Data("Name", strCollectionName);

		_uint iNotifySize = CollectionData.Read_ArraySize("Notifies");
		for (_uint j = 0; j < iNotifySize; j++)
		{
			shared_ptr<CAnimNotify> pNotify = { nullptr };

			FRapidJson NotifyData;
			CollectionData.Read_ObjectFromArray("Notifies", j, NotifyData);

			_uint iID = 0;
			NotifyData.Read_Data("NotifyID", iID);

			switch (iID)
			{
			case ECast(EAnimNotifyType::Effect):
			{
				pNotify = dynamic_pointer_cast<CAnimNotify>(CAnimNotify_Effect::Create(NotifyData));
				break;
			}
			case ECast(EAnimNotifyType::Sound):
			{
				pNotify = dynamic_pointer_cast<CAnimNotify>(CAnimNotify_Sound::Create(NotifyData));
				break;
			}
			case ECast(EAnimNotifyType::Event):
			{
				pNotify = dynamic_pointer_cast<CAnimNotify>(CAnimNotify_Event::Create(NotifyData));
				break;
			}
			case 3:

				break;
			}

			if (nullptr != pNotify)
				Add_Notify(strCollectionName, pNotify);
		}
	}

	return S_OK;
}
