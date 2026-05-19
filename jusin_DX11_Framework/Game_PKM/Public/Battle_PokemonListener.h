#pragma once
#include "Battle_EventListenerBase.h"

NS_BEGIN(Game_PKM)

class CBattle_Pokemon;

/* CBattle_PokemonListener
   - Dispatcher 구독자. 자기 슬롯(m_iSide) 이벤트만 필터링하여 Battle_Pokemon anim 트리거.
   - Battle_Pokemon 은 weak (Level 이 owns).
   - 본 시리즈 패턴 (BattleMsgListener) 과 동일한 어댑터 구조 - 다중 상속 회피. */
class CBattle_PokemonListener final : public CBattle_EventListenerBase
{
private:
	CBattle_PokemonListener();
	virtual ~CBattle_PokemonListener() = default;

public:
	HRESULT Initialize();
	void Bind(CBattle_Pokemon* pPokemon, _uint iSide);

	virtual void On_MoveUsed(const EVENT_MOVE_USED& tEvent) override;
	virtual void On_DamageDealt(const EVENT_DAMAGE_DEALT& tEvent) override;
	virtual void On_PokemonFainted(const EVENT_POKEMON_FAINTED& tEvent) override;

private:
	CBattle_Pokemon* m_pPokemon = { nullptr };  // weak
	_uint            m_iSide = { 0 };

public:
	static CBattle_PokemonListener* Create();

private:
	virtual void Free() override;
};

NS_END