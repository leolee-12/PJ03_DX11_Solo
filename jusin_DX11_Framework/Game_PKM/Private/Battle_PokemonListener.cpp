#include "Battle_PokemonListener.h"
#include "Battle_Pokemon.h"
#include "Battle_Manager.h"
#include "Battler.h"

CBattle_PokemonListener::CBattle_PokemonListener()
{
}

HRESULT CBattle_PokemonListener::Initialize()
{
	return S_OK;
}

void CBattle_PokemonListener::Bind(CBattle_Pokemon* pPokemon, _uint iSide, CBattle_Manager* pManager)
{
	m_pPokemon = pPokemon;
	m_iSide = iSide;
	m_pManager = pManager;
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

void CBattle_PokemonListener::On_PokemonSwitched(const EVENT_POKEMON_SWITCHED& tEvent)
{
	if (nullptr == m_pPokemon || nullptr == m_pManager)
		return;

	if (tEvent.iSide != m_iSide)
		return;

	CBattler* pBattler = m_pManager->Get_Battler(m_iSide);
	if (nullptr == pBattler || nullptr == pBattler->Get_Instance())
		return;

	m_pPokemon->Apply_Switch(pBattler->Get_Instance());
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
	m_pManager = nullptr;

	__super::Free();
}