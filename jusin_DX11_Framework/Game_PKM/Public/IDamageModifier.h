#pragma once
#include "Base.h"
#include "Damage_Pipe.h"
#include "Battle_Context.h"

NS_BEGIN(Game_PKM)

class IDamageModifier abstract : public CBase
{
protected:
	IDamageModifier() = default;
	virtual ~IDamageModifier() = default;

public:
	virtual void Apply(const BATTLE_CONTEXT& ctx, DAMAGE_PIPE_DATA& pipe) PURE;
	virtual const _tchar* Get_Tag() const { return TEXT("Modifier"); }
};

NS_END