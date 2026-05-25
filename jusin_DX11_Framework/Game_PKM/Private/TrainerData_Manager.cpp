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
		wcscpy_s(tData.szName, TEXT("반바지 꼬마 대지"));
		tData.eAIType = TRAINER_AI::BASIC;
		tData.iRewardMoney = 120;
		tData.iRewardItemID = 0;
		wcscpy_s(tData.szEncounterDialog, TEXT("포켓몬 트레이너끼리\n눈과 눈이 마주쳤다!\f내가 좋아하는 포켓몬과 승부하자!"));
		wcscpy_s(tData.szVictoryDialog, TEXT("내 포켓몬 어때?\n강하지!"));
		wcscpy_s(tData.szDefeatDialog, TEXT("강하다 너!"));

		tData.strBodyProtoTag = PROTO_OBJ_BODY_HUMAN;
		tData.strModelProtoTag = PROTO_COM_MODEL_PPL_SHORTPANTS;
		tData.strShaderProtoTag = PROTO_COM_SHADER_HUMAN;
		strcpy_s(tData.szMappingPath, sizeof(tData.szMappingPath),
			"../../Resources/Models/people/shortpants/shortpants_mapping.json");

		PartyOps::Clear(tData.tParty);

		if (FAILED(AddPokemon(tData, 10, 2)))
			return E_FAIL;

		if (FAILED(AddPokemon(tData, 4, 3)))
			return E_FAIL;

		if (!m_TrainerTable.emplace(tData.iTrainerID, tData).second)
			return E_FAIL;
	}

	{
		TRAINER_DATA tData{};
		tData.iTrainerID = 2;
		wcscpy_s(tData.szName, TEXT("체육관 관장 웅"));
		tData.eAIType = TRAINER_AI::BASIC;
		tData.iRewardMoney = 1920;
		tData.iRewardItemID = 0;
		wcscpy_s(tData.szEncounterDialog, TEXT("왔구나!\n나는 회색시티 포켓몬체육관의 관장인 웅이야!\f나의 굳은 의지는 내 포켓몬에게서도 드러나지!\n단단하고 참을성이 강해.\f좋아!\n자 덤벼라!"));
		wcscpy_s(tData.szVictoryDialog, TEXT("바위처럼 단단한 승부였지!"));
		wcscpy_s(tData.szDefeatDialog, TEXT("너를 얕잡아 본 것 같군"));

		tData.strBodyProtoTag = PROTO_OBJ_BODY_HUMAN;
		tData.strModelProtoTag = PROTO_COM_MODEL_PPL_ROCK;
		tData.strShaderProtoTag = PROTO_COM_SHADER_HUMAN;
		strcpy_s(tData.szMappingPath, sizeof(tData.szMappingPath),
			"../../Resources/Models/people/rock/rock_mapping.json");

		PartyOps::Clear(tData.tParty);
		if (FAILED(AddPokemon(tData, 74, 3)))   // 꼬마돌
			return E_FAIL;
		if (FAILED(AddPokemon(tData, 95, 11)))   // 롱스톤
			return E_FAIL;

		if (!m_TrainerTable.emplace(tData.iTrainerID, tData).second)
			return E_FAIL;
	}

	{
		TRAINER_DATA tData{};
		tData.iTrainerID = 3;
		wcscpy_s(tData.szName, TEXT("체육관 관장 이슬"));
		tData.eAIType = TRAINER_AI::BASIC;
		tData.iRewardMoney = 3040;
		tData.iRewardItemID = 0;
		wcscpy_s(tData.szEncounterDialog, TEXT("너!\f너는 포켓몬을 키울 때\n너만의 방침이 있니?\f나의 방침은 말이지...\n물타입 포켓몬으로 공격하고 ...또 공격하는거야!\f자! 세계의 미소녀\n이슬님이 상대해줄게!\f가라!\n내 귀염둥이!"));
		wcscpy_s(tData.szVictoryDialog, TEXT("파도처럼 휩쓸어 줬다!"));
		wcscpy_s(tData.szDefeatDialog, TEXT("으~응...!\n내가 져버렸네"));

		tData.strBodyProtoTag = PROTO_OBJ_BODY_HUMAN;
		tData.strModelProtoTag = PROTO_COM_MODEL_PPL_WATER;
		tData.strShaderProtoTag = PROTO_COM_SHADER_HUMAN;
		strcpy_s(tData.szMappingPath, sizeof(tData.szMappingPath),
			"../../Resources/Models/people/water/water_mapping.json");

		PartyOps::Clear(tData.tParty);
		if (FAILED(AddPokemon(tData, 7, 5)))    // 꼬부기
			return E_FAIL;
		if (FAILED(AddPokemon(tData, 121, 6))) // 아쿠스타
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