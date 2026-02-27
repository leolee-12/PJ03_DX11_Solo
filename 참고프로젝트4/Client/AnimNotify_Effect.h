#pragma once

#include "Client_Defines.h"
#include "AnimNotify.h"
#include "Utility/RapidJson_Utility.h"
#include "Utility/DelegateTemplate.h"


BEGIN(Engine)
class CGameObject;
END

BEGIN(Client)

//typedef FastDelegate1<weak_ptr<CGameObject>, void> Delegate;
//typedef MulticastDelegate<Delegate, weak_ptr<CGameObject>>	MulticastDelegate;

struct FNotifyType_Effect : public FNotifyType
{
	EFFECT_TYPE	iEffectType;
	wstring		strEffectName;
	string		strSocketName;		// 현재 붙은 모델의 소켓을 찾아 나중에 처리를 해주게 된다.
	_float3		vPos;
	_float4		vRot;
};

class CAnimNotify_Effect : public CAnimNotify
{
	INFO_CLASS(CAnimNotify_Effect, CAnimNotify)

public:
	explicit CAnimNotify_Effect();
	explicit CAnimNotify_Effect(const CAnimNotify_Effect& rhs);
	virtual ~CAnimNotify_Effect() = default;

public:
	virtual HRESULT Initialize() override { return S_OK; }
	virtual HRESULT Initialize(_float fTrackPos, FNotifyType_Effect Desc);
	virtual HRESULT Initialize(FRapidJson& InputData);

	// 트리거를 작동하기 전상태로 되돌린다.
	virtual void OnReset() override;

	// 작동시 발생시킬 기능
	virtual _bool OnTrigger() override;

public:
	static shared_ptr<CAnimNotify_Effect> Create(_float fTrackPos, FNotifyType_Effect Desc);
	static shared_ptr<CAnimNotify_Effect> Create(FRapidJson& InputData);
	virtual void Free() override;
	virtual shared_ptr<CAnimNotify> Clone() override;

public:
	virtual FRapidJson Bake_Notify() override;
	HRESULT Input_RpJson(FRapidJson InputData);
};

END