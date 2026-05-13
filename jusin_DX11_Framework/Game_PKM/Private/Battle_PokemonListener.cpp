#include "Battle_PokemonListener.h"
#include "Battle_Pokemon.h"

CBattle_PokemonListener::CBattle_PokemonListener()
{
}

HRESULT CBattle_PokemonListener::Initialize()
{
	return S_OK;
}

void CBattle_PokemonListener::Bind(CBattle_Pokemon* pPokemon, _uint iSide)
{
	m_pPokemon = pPokemon;
	m_iSide = iSide;
}

void CBattle_PokemonListener::On_MoveUsed(const EVENT_MOVE_USED& tEvent)
{
	if (nullptr == m_pPokemon)
		return;

	if (tEvent.iSide != m_iSide)
		return;

	m_pPokemon->Play_Attack();
}

void CBattle_PokemonListener::On_DamageDealt(const EVENT_DAMAGE_DEALT& tEvent)
{
	if (nullptr == m_pPokemon)
		return;

	if (tEvent.iTargetSide != m_iSide)
		return;

	m_pPokemon->Play_Hurt();
}

void CBattle_PokemonListener::On_PokemonFainted(const EVENT_POKEMON_FAINTED& tEvent)
{
	if (nullptr == m_pPokemon)
		return;

	if (tEvent.iSide != m_iSide)
		return;

	m_pPokemon->Play_Faint();
}

CBattle_PokemonListener* CBattle_PokemonListener::Create()
{
	CBattle_PokemonListener* pInstance = new CBattle_PokemonListener();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CBattle_PokemonListener");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBattle_PokemonListener::Free()
{
	m_pPokemon = nullptr;
	__super::Free();
}