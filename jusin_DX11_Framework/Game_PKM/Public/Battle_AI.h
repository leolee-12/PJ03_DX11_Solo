#pragma once
#include "IBattleAI.h"

NS_BEGIN(Game_PKM)

class CRandomAI final : public IBattleAI
{
private:
	CRandomAI();
	virtual ~CRandomAI() = default;

public:
	virtual IBattleCommand* Decide(const BATTLE_CONTEXT& ctx, _uint iSide) override;

public:
	static CRandomAI* Create();

private:
	virtual void Free() override;
};

NS_END