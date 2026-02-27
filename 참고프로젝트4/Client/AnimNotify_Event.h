#pragma once

#include "Client_Defines.h"
#include "AnimNotify.h"


BEGIN(Client)

struct FNotifyType_Event : public FNotifyType
{

};

class CAnimNotify_Event final : public CAnimNotify
{
	INFO_CLASS(CAnimNotify_Event, CAnimNotify)

public:
	explicit CAnimNotify_Event();
	explicit CAnimNotify_Event(const CAnimNotify_Event& rhs);
	virtual ~CAnimNotify_Event() = default;

public:
	virtual HRESULT Initialize() override { return S_OK; }
	virtual HRESULT Initialize(_float fTrackPos, FNotifyType_Event Desc);
	virtual HRESULT Initialize(FRapidJson& InputData);

	// 트리거를 작동하기 전상태로 되돌린다.
	virtual void OnReset() override;

	// 작동시 발생시킬 기능
	virtual _bool OnTrigger() override;

public:
	static shared_ptr<CAnimNotify_Event> Create(_float fTrackPos, FNotifyType_Event Desc);
	static shared_ptr<CAnimNotify_Event> Create(FRapidJson& InputData);
	virtual void Free() override;
	virtual shared_ptr<CAnimNotify> Clone() override;

public:
	virtual FRapidJson Bake_Notify() override;
	HRESULT Input_RpJson(FRapidJson InputData);

};

END