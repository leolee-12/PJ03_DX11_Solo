#include "TrainerData_Manager.h"
#include "PokemonData_Manager.h"

IMPLEMENT_SINGLETON(CTrainerData_Manager)

CTrainerData_Manager::CTrainerData_Manager()
{
}

HRESULT CTrainerData_Manager::Initialize()
{
	if (m_bInitialized)
		return S_OK;

	if (FAILED(Load_BuiltinSeed()))
		return E_FAIL;

	if (FAILED(Validate_Seed()))
	{
		m_TrainerTable.clear();
		return E_FAIL;
	}

	m_bInitialized = true;
	return S_OK;
}

const TRAINER_DATA* CTrainerData_Manager::Find_Trainer(_uint iTrainerID) const
{
	auto iter = m_TrainerTable.find(iTrainerID);

	if (iter == m_TrainerTable.end())
		return nullptr;

	return &iter->second;
}

HRESULT CTrainerData_Manager::Load_BuiltinSeed()
{
	auto* pDataMgr = CPokemonData_Manager::GetInstance();
	if (nullptr == pDataMgr)
		return E_FAIL;

	auto AddPokemon = [pDataMgr](TRAINER_DATA& tTrainer, _uint iSpeciesID, _ubyte iLevel) -> HRESULT
		{
			const SPECIES_DATA* pSpecies = pDataMgr->Find_Species(iSpeciesID);
			if (nullptr == pSpecies)
				return E_FAIL;

			POKEMON_INSTANCE tPokemon =
				Build_PokemonInstance(*pSpecies, iLevel, tTrainer.iTrainerID, 0, pDataMgr);

			if (!PartyOps::Add(tTrainer.tParty, tPokemon))
				return E_FAIL;

			return S_OK;
		};

	{
		TRAINER_DATA tData{};
		tData.iTrainerID = 1;
		wcscpy_s(tData.szName, TEXT("Debug Trainer"));
		tData.eAIType = TRAINER_AI::BASIC;
		tData.iRewardMoney = 120;
		tData.iRewardItemID = 0;
		wcscpy_s(tData.szEncounterDialog, TEXT("Let's battle!"));
		wcscpy_s(tData.szVictoryDialog, TEXT("I won!"));
		wcscpy_s(tData.szDefeatDialog, TEXT("I lost!"));

		PartyOps::Clear(tData.tParty);

		if (FAILED(AddPokemon(tData, 25, 5)))
			return E_FAIL;

		if (FAILED(AddPokemon(tData, 1, 5)))
			return E_FAIL;

		if (!m_TrainerTable.emplace(tData.iTrainerID, tData).second)
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CTrainerData_Manager::Validate_Seed() const
{
	if (m_TrainerTable.empty())
		return E_FAIL;

	for (const auto& Pair : m_TrainerTable)
	{
		if (0 == Pair.second.tParty.iCount)
			return E_FAIL;
	}

	return S_OK;
}

void CTrainerData_Manager::Free()
{
	m_TrainerTable.clear();
	m_bInitialized = false;

	__super::Free();
}