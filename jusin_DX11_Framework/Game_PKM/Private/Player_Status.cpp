#include "Player_Status.h"
#include "PokemonData_Manager.h"

CPlayer_Status::CPlayer_Status(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
	m_strName = L"PlayerState";
}

CPlayer_Status::CPlayer_Status(const CPlayer_Status& Prototype)
	: CGameObject{ Prototype }
{
}

HRESULT CPlayer_Status::Initialize_Prototype()
{
	PartyOps::Clear(m_tParty);
	return S_OK;
}

HRESULT CPlayer_Status::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (0 == m_tParty.iCount)
	{
		auto* pDataMgr = CPokemonData_Manager::GetInstance();
		const SPECIES_DATA* pSpecies = pDataMgr ? pDataMgr->Find_Species(25) : nullptr;

		if (nullptr != pSpecies)
		{
			POKEMON_INSTANCE tSeed = {};

			tSeed.iSpeciesID = pSpecies->iDexNo;
			wcscpy_s(tSeed.szNickname, pSpecies->szName);

			tSeed.iLevel = 5;
			tSeed.iExp = 0;

			for (size_t i = 0; i < static_cast<size_t>(STAT::END); ++i)
			{
				tSeed.iIV[i] = g_kMaxIV;
				tSeed.iEV[i] = 0;
			}

			tSeed.eNature = NATURE::JOLLY;
			tSeed.iAbilityID = pSpecies->iAbility1;

			const _uint iInitialMoves[g_kMaxMovesPerPokemon] =
			{
				  pSpecies->iLearnset[0],
				  pSpecies->iLearnset[1],
				  pSpecies->iLearnset[2],
				  pSpecies->iLearnset[3],
			};

			Assign_Moves(tSeed, iInitialMoves, g_kMaxMovesPerPokemon, pDataMgr);

			tSeed.eStatus = STATUS_CONDITION::NONE;
			tSeed.iHeldItemID = 0;

			Recalc_All_Stats(tSeed, *pSpecies);
			tSeed.iCurrentHP = tSeed.iStat[static_cast<size_t>(STAT::HP)];

			tSeed.iOriginalTrainerID = m_iTrainerID;
			tSeed.iCapturedAtZoneID = 0;

			PartyOps::Add(m_tParty, tSeed);
		}
	}

	return S_OK;
}

CPlayer_Status* CPlayer_Status::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPlayer_Status* pInstance = new CPlayer_Status(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CPlayer_Status");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPlayer_Status::Clone(void* pArg)
{
	CPlayer_Status* pInstance = new CPlayer_Status(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CPlayer_Status");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer_Status::Free()
{
	__super::Free();
}