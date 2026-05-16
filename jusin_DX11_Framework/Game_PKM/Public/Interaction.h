#pragma once
#include "Game_PKM_Defines.h"
#include "Component.h"

NS_BEGIN(Engine)
class CGameObject;
NS_END

NS_BEGIN(Game_PKM)

enum class INTERACTION_EVENT
{
	TALK,
	TOUCH,
	TRIGGER_ENTER,
	INSPECT,
	END
};

struct INTERACTION_CONTEXT
{
	CGameObject* pCaller = { nullptr };
	CGameObject* pTarget = { nullptr };

	INTERACTION_EVENT eEvent = { INTERACTION_EVENT::TALK };

	_float4 vCallerPosition = {};
	_float4 vCallerLook = {};
};

class CInteraction abstract : public CComponent
{
protected:
	CInteraction(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CInteraction(const CInteraction& Prototype);
	virtual ~CInteraction() = default;

public:
	virtual _bool Supports(INTERACTION_EVENT eEvent) const { return false; }
	virtual _bool CanInteract(const INTERACTION_CONTEXT& ctx) const { return Supports(ctx.eEvent); }
		virtual void Execute(const INTERACTION_CONTEXT & ctx) {}
		virtual _int Get_Priority(const INTERACTION_CONTEXT & ctx) const { return 0; }
		virtual void Tick(_float fTimeDelta) {}

protected:
	virtual void Free() override;
};

NS_END