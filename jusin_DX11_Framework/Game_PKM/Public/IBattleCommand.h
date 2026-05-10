#pragma once
#include "Base.h"
#include "Battle_Context.h"

NS_BEGIN(Game_PKM)

class IBattleCommand abstract : public CBase
{
protected:
	IBattleCommand() = default;
	virtual ~IBattleCommand() = default;

public:
	virtual ACTION_TYPE Get_Type() const PURE;
	virtual _uint Get_ActorSide() const PURE;
	virtual _byte Get_Priority() const PURE;
	virtual _ushort Get_ActorSpeed(const BATTLE_CONTEXT& ctx) const PURE;
	virtual HRESULT Execute(const BATTLE_CONTEXT& ctx) PURE;

protected:
	virtual void Free() override
	{
		__super::Free();
	}
};

NS_END