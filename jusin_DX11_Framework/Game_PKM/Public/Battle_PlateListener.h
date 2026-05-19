#pragma once
#include "Battle_EventListenerBase.h"

NS_BEGIN(Game_PKM)

class CBattlePlate;

class CBattle_PlateListener final : public CBattle_EventListenerBase
{
private:
	CBattle_PlateListener();
	virtual ~CBattle_PlateListener() = default;

public:
	HRESULT Initialize();
	void Bind(CBattlePlate* pBattlePlate);

	virtual void On_PokemonSwitched(const EVENT_POKEMON_SWITCHED& tEvent) override;

private:
	CBattlePlate* m_pBattlePlate = { nullptr };  // weak

public:
	static CBattle_PlateListener* Create();

private:
	virtual void Free() override;
};

NS_END