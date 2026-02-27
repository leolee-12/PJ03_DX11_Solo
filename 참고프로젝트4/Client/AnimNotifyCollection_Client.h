#pragma once

#include "Client_Defines.h"
#include "AnimNotify.h"


enum class EAnimNotifyType
{
	Effect,
	Sound,
	Event,
};

BEGIN(Client)

class CAnimNotifyCollection_Client final : public CAnimNotifyCollection
{
	INFO_CLASS(CAnimNotifyCollection_Client, CAnimNotifyCollection)

private:
	explicit CAnimNotifyCollection_Client();
	explicit CAnimNotifyCollection_Client(const CAnimNotifyCollection_Client& rhs);
	virtual ~CAnimNotifyCollection_Client() = default;

public:
	static shared_ptr<CAnimNotifyCollection_Client> Create();
	static shared_ptr<CAnimNotifyCollection> Create_Callback();
	virtual void Free() override;
	virtual shared_ptr<CAnimNotifyCollection> Clone() override;

public:
	virtual HRESULT Input_Json(FRapidJson& InputData);

};


END

