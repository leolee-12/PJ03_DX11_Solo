#include "Spawn_Manager.h"
#include "PokemonData_Manager.h"
#include "RenderRule_Manager.h"
#include "Actor_WildPokemon.h"
#include "Actor_NPC.h"
#include "Body_Pokemon.h"
#include "Body_Human.h"
#include "Navigation.h"
#include "Battle_AnimDef.h"

#include "GameInstance.h"

//#include <fstream>
#include <sstream>

#ifdef _DEBUG
#include "DebugDraw.h"
#endif

NS_BEGIN(Game_PKM)
IMPLEMENT_SINGLETON(CSpawn_Manager)

namespace
{
	inline std::string Trim_(const std::string& s)
	{
		size_t b = s.find_first_not_of(" \t\r\n");
		if (b == std::string::npos) return "";
		size_t e = s.find_last_not_of(" \t\r\n");
		return s.substr(b, e - b + 1);
	}

	inline _bool Parse_Bool_(const std::string& v)
	{
		return (v == "true" || v == "True" || v == "1");
	}

	inline _float3 Parse_Float3_(const std::string& v)
	{
		_float3 r{};
		std::stringstream ss(v);
		std::string tok;
		if (std::getline(ss, tok, ',')) r.x = std::stof(Trim_(tok));
		if (std::getline(ss, tok, ',')) r.y = std::stof(Trim_(tok));
		if (std::getline(ss, tok, ',')) r.z = std::stof(Trim_(tok));
		return r;
	}

	inline _float2 Parse_Float2_(const std::string& v)
	{
		_float2 r{};
		std::stringstream ss(v);
		std::string tok;
		if (std::getline(ss, tok, ',')) r.x = std::stof(Trim_(tok));
		if (std::getline(ss, tok, ',')) r.y = std::stof(Trim_(tok));
		return r;
	}

	inline SPAWN_KIND Parse_Kind_(const std::string& v)
	{
		if (v == "WildPokemon")  return SPAWN_KIND::WILD_POKEMON;
		if (v == "Trainer")      return SPAWN_KIND::TRAINER;
		if (v == "NPC")          return SPAWN_KIND::NPC;
		if (v == "EventObject")  return SPAWN_KIND::EVENT_OBJECT;
		return SPAWN_KIND::WILD_POKEMON;
	}

	inline SPAWN_NPC_PROFILE Parse_NpcProfile_(const std::string& v)
	{
		if (v == "Doctor")       return SPAWN_NPC_PROFILE::DOCTOR;
		if (v == "Juveniles")    return SPAWN_NPC_PROFILE::JUVENILES;
		if (v == "Fat")          return SPAWN_NPC_PROFILE::FAT;
		if (v == "Shortpants")   return SPAWN_NPC_PROFILE::SHORTPANTS;
		if (v == "Nurse")        return SPAWN_NPC_PROFILE::NURSE;
		if (v == "Rock")         return SPAWN_NPC_PROFILE::ROCK;
		if (v == "Water")        return SPAWN_NPC_PROFILE::WATER;
		if (v == "PM0001_00")    return SPAWN_NPC_PROFILE::PM0001_00;
		if (v == "PM0004_00")    return SPAWN_NPC_PROFILE::PM0004_00;
		if (v == "PM0007_00")    return SPAWN_NPC_PROFILE::PM0007_00;
		if (v == "PM0010_00")    return SPAWN_NPC_PROFILE::PM0010_00;
		if (v == "PM0025_00")    return SPAWN_NPC_PROFILE::PM0025_00;
		if (v == "PM0041_00")    return SPAWN_NPC_PROFILE::PM0041_00;
		if (v == "PM0043_00")    return SPAWN_NPC_PROFILE::PM0043_00;
		if (v == "PM0059_00")    return SPAWN_NPC_PROFILE::PM0059_00;
		if (v == "PM0074_00")    return SPAWN_NPC_PROFILE::PM0074_00;
		if (v == "PM0095_00")    return SPAWN_NPC_PROFILE::PM0095_00;
		if (v == "PM0121_00")    return SPAWN_NPC_PROFILE::PM0121_00;
		return SPAWN_NPC_PROFILE::NONE;
	}

	template<size_t N>
	inline void Copy_Wide_(wchar_t(&dst)[N], const std::string& src)
	{
		const _wstring wide = StoW(src);
		wcsncpy_s(dst, wide.c_str(), _TRUNCATE);
	}

	struct NPC_PROFILE_INFO
	{
		SPAWN_NPC_PROFILE eProfile = { SPAWN_NPC_PROFILE::NONE };
		_bool bPokemon = { false };
		WNameID strModelProtoTag = {};
		const _char* pMappingPath = { nullptr };
		const wchar_t* pDefaultDialogueKey = { nullptr };
		_float fScale = { 1.f };
		_float fBodyOffsetX = { 0.f };
		_float fBodyOffsetY = { 0.f };
		_float fBodyOffsetZ = { 0.f };
	};

	const NPC_PROFILE_INFO* Find_NpcProfile_(SPAWN_NPC_PROFILE eProfile)
	{
		static constexpr NPC_PROFILE_INFO Profiles[] =
		{
			{ SPAWN_NPC_PROFILE::DOCTOR, false, PROTO_COM_MODEL_DOCTOR, "../../Resources/Models/people/doctor/doctor_mapping.json", L"dialogue_npc_doctor", 1.f },
			{ SPAWN_NPC_PROFILE::JUVENILES, false, PROTO_COM_MODEL_PPL_JUVENILES, "../../Resources/Models/people/juveniles/juveniles_mapping.json", L"dialogue_npc_juveniles", 1.f },
			{ SPAWN_NPC_PROFILE::FAT, false, PROTO_COM_MODEL_PPL_FAT, "../../Resources/Models/people/fat/fat_mapping.json", L"dialogue_npc_fat", 1.f },
			{ SPAWN_NPC_PROFILE::SHORTPANTS, false, PROTO_COM_MODEL_PPL_SHORTPANTS, "../../Resources/Models/people/shortpants/shortpants_mapping.json", L"dialogue_trainer_shortpants", 1.f },
			{ SPAWN_NPC_PROFILE::NURSE, false, PROTO_COM_MODEL_PPL_NURSE, "../../Resources/Models/people/nurse/nurse_mapping.json", L"dialogue_npc_nurse", 1.f, 0.f, 0.f, -1.3f },
			{ SPAWN_NPC_PROFILE::ROCK, false, PROTO_COM_MODEL_PPL_ROCK, "../../Resources/Models/people/rock/rock_mapping.json", L"dialogue_trainer_rock", 1.f },
			{ SPAWN_NPC_PROFILE::WATER, false, PROTO_COM_MODEL_PPL_WATER, "../../Resources/Models/people/water/water_mapping.json", L"dialogue_trainer_water", 1.f },

			{ SPAWN_NPC_PROFILE::PM0001_00, true, PROTO_COM_MODEL_PM0001_00, "../../Resources/Models/pkm/pm0001_00/pm0001_00_mapping.json", L"dialogue_pokemon_pm0001_00", 1.f },
			{ SPAWN_NPC_PROFILE::PM0004_00, true, PROTO_COM_MODEL_PM0004_00, "../../Resources/Models/pkm/pm0004_00/pm0004_00_mapping.json", L"dialogue_pokemon_pm0004_00", 1.f },
			{ SPAWN_NPC_PROFILE::PM0007_00, true, PROTO_COM_MODEL_PM0007_00, "../../Resources/Models/pkm/pm0007_00/pm0007_00_mapping.json", L"dialogue_pokemon_pm0007_00", 1.f },
			{ SPAWN_NPC_PROFILE::PM0010_00, true, PROTO_COM_MODEL_PM0010_00, "../../Resources/Models/pkm/pm0010_00/pm0010_00_mapping.json", L"dialogue_pokemon_pm0010_00", 1.f },
			{ SPAWN_NPC_PROFILE::PM0025_00, true, PROTO_COM_MODEL_PM0025_00, "../../Resources/Models/pkm/pm0025_00/pm0025_00_mapping.json", L"dialogue_pokemon_pm0025_00", 1.f },
			{ SPAWN_NPC_PROFILE::PM0041_00, true, PROTO_COM_MODEL_PM0041_00, "../../Resources/Models/pkm/pm0041_00/pm0041_00_mapping.json", L"dialogue_pokemon_pm0041_00", 1.f },
			{ SPAWN_NPC_PROFILE::PM0043_00, true, PROTO_COM_MODEL_PM0043_00, "../../Resources/Models/pkm/pm0043_00/pm0043_00_mapping.json", L"dialogue_pokemon_pm0043_00", 1.f },
			{ SPAWN_NPC_PROFILE::PM0059_00, true, PROTO_COM_MODEL_PM0059_00, "../../Resources/Models/pkm/pm0059_00/pm0059_00_mapping.json", L"dialogue_pokemon_pm0059_00", 1.f },
			{ SPAWN_NPC_PROFILE::PM0074_00, true, PROTO_COM_MODEL_PM0074_00, "../../Resources/Models/pkm/pm0074_00/pm0074_00_mapping.json", L"dialogue_pokemon_pm0074_00", 1.f },
			{ SPAWN_NPC_PROFILE::PM0095_00, true, PROTO_COM_MODEL_PM0095_00, "../../Resources/Models/pkm/pm0095_00/pm0095_00_mapping.json", L"dialogue_pokemon_pm0095_00", 1.f },
			{ SPAWN_NPC_PROFILE::PM0121_00, true, PROTO_COM_MODEL_PM0121_00, "../../Resources/Models/pkm/pm0121_00/pm0121_00_mapping.json", L"dialogue_pokemon_pm0121_00", 1.f },
		};

		for (const NPC_PROFILE_INFO& Profile : Profiles)
		{
			if (Profile.eProfile == eProfile)
				return &Profile;
		}

		return nullptr;
	}
}

NS_END

CSpawn_Manager::CSpawn_Manager()
	: CBase{}
{
}

HRESULT CSpawn_Manager::Initialize(_uint iNaviProtoLevel, WNameID strNaviProtoTag)
{
	// 반복 호출 안전성: 직전 상태 wipe 후 재구성.
	Clear();

	m_pGameInstance = CGameInstance::GetInstance();   // weak

	// NavMesh 프로토타입 클론 - pArg=nullptr 이면 매니저 측 m_iCurrentCellIndex = -1.
	CBase* pCloned = m_pGameInstance->Clone_Prototype(
		PROTOTYPE::COMPONENT, iNaviProtoLevel, strNaviProtoTag, nullptr);
	if (nullptr == pCloned)
		return E_FAIL;

	m_pNavigationClone = static_cast<CNavigation*>(pCloned);
	if (nullptr == m_pNavigationClone)
	{
		Safe_Release(pCloned);
		return E_FAIL;
	}

#ifdef _DEBUG
	if (FAILED(Ready_DebugResources()))
		return E_FAIL;
#endif

	m_bInitialized = true;
	m_bBegan = false;
	return S_OK;
}

HRESULT CSpawn_Manager::Register_SpawnRect(const SPAWN_RECT_DESC& tDesc)
{
	if (!m_bInitialized)     return E_FAIL;
	if (m_bBegan)            return E_FAIL;
	if (0 == tDesc.iSpawnID) return E_FAIL;

	// iSpawnID 중복 차단.
	for (const auto& tRuntime : m_Runtimes)
	{
		if (tRuntime.tDesc.iSpawnID == tDesc.iSpawnID)
			return E_FAIL;
	}

	SPAWN_RECT_RUNTIME tRuntime{};
	tRuntime.tDesc = tDesc;
	m_Runtimes.push_back(tRuntime);
	return S_OK;
}

HRESULT CSpawn_Manager::Load_From_File(const _tchar* pFilePath)
{
	if (!m_bInitialized) return E_FAIL;
	if (m_bBegan)        return E_FAIL;
	if (nullptr == pFilePath) return E_FAIL;

	std::ifstream file(pFilePath);
	if (!file.is_open())
	{
		OutputDebugStringA("[SpawnLoader] failed to open file\n");
		return E_FAIL;
	}

	std::string     line;
	SPAWN_RECT_DESC tCur{};
	_bool           bInBlock = false;

	while (std::getline(file, line))
	{
		line = Trim_(line);
		if (line.empty() || line[0] == '#') continue;

		if (!bInBlock)
		{
			if (line.find("SpawnRect") != std::string::npos && line.find("{") != std::string::npos)
			{
				tCur = SPAWN_RECT_DESC{};
				bInBlock = true;
			}
			continue;
		}

		if (line[0] == '}')
		{
			if (FAILED(Register_SpawnRect(tCur)))
			{
				OutputDebugStringA(("[SpawnLoader Warn] Register failed for ID="
					+ std::to_string(tCur.iSpawnID) + "\n").c_str());
			}
			bInBlock = false;
			continue;
		}

		const size_t eq = line.find('=');
		if (eq == std::string::npos) continue;

		const std::string key = Trim_(line.substr(0, eq));
		const std::string val = Trim_(line.substr(eq + 1));

		if (key == "ID")                    tCur.iSpawnID = static_cast<_uint>(std::stoul(val));
		else if (key == "Kind")                  tCur.eSpawnKind = Parse_Kind_(val);
		else if (key == "Center")                tCur.vCenter = Parse_Float3_(val);
		else if (key == "Size")                  tCur.vSize = Parse_Float2_(val);
		else if (key == "RotationY")             tCur.fRotationY = std::stof(val);
		else if (key == "EncounterTable")        tCur.iEncounterTableID = static_cast<_uint>(std::stoul(val));
		else if (key == "SpeciesID")             tCur.iSpeciesID_Temp = static_cast<_uint>(std::stoul(val));
		else if (key == "Level")                 tCur.iLevel_Temp = static_cast<_uint>(std::stoul(val));
		else if (key == "MaxAliveCount")         tCur.iMaxAliveCount = static_cast<_uint>(std::stoul(val));
		else if (key == "ProjectRadius")         tCur.fProjectRadius = std::stof(val);
		else if (key == "LeashRadius")           tCur.fLeashRadius = std::stof(val);
		else if (key == "MinDistanceFromPlayer") tCur.fMinDistanceFromPlayer = std::stof(val);
		else if (key == "MaxDistanceFromPlayer") tCur.fMaxDistanceFromPlayer = std::stof(val);
		else if (key == "RequireReachable")      tCur.bRequireReachable = Parse_Bool_(val);
		else if (key == "SpawnOnLoad")           tCur.bSpawnOnLoad = Parse_Bool_(val);
		else if (key == "Respawn")               tCur.bRespawn = Parse_Bool_(val);
		else if (key == "RespawnDelay")          tCur.fRespawnDelay = std::stof(val);
		else if (key == "NPCProfile")            tCur.eNpcProfile = Parse_NpcProfile_(val);
		else if (key == "DialogueKey")           Copy_Wide_(tCur.szDialogueKey, val);
		else if (key == "TrainerID")             tCur.iTrainerID = static_cast<_uint>(std::stoul(val));
	}

	return S_OK;
}

HRESULT CSpawn_Manager::Begin()
{
	if (!m_bInitialized) return E_FAIL;
	if (m_bBegan)        return E_FAIL;

	for (auto& tRuntime : m_Runtimes)
	{
		Prepare_SpawnRect(tRuntime);

		// bSpawnOnLoad: valid 사각형 한정으로 즉시 1회 스폰.
		if (!tRuntime.bValid)             continue;
		if (!tRuntime.tDesc.bSpawnOnLoad) continue;

		switch (tRuntime.tDesc.eSpawnKind)
		{
		case SPAWN_KIND::WILD_POKEMON: Try_SpawnWildPokemon(tRuntime); break;
		case SPAWN_KIND::NPC:
		case SPAWN_KIND::TRAINER:      Spawn_NPC(tRuntime);            break;
		default: break;
		}
	}

	m_bBegan = true;
	return S_OK;
}

void CSpawn_Manager::Update(_float fTimeDelta)
{
	if (!m_bBegan) return;

	// 1) AliveCount 재계산 (1차: O(N) 스캔. 후속에 notify 기반 전환 여지)
	Recount_AliveCounts();

	// 2) 사각형별 진행
	for (auto& tRuntime : m_Runtimes)
	{
		if (!tRuntime.bValid) continue;

		if (tRuntime.fRespawnTimer > 0.f)
		{
			tRuntime.fRespawnTimer -= fTimeDelta;
			continue;
		}

		// bRespawn=false 이면 한 번 스폰 후 더 이상 시도 안 함
		if (!tRuntime.tDesc.bRespawn && tRuntime.bHasEverSpawned)
			continue;

		if (tRuntime.iAliveCount >= tRuntime.tDesc.iMaxAliveCount)
			continue;

		_bool bSpawned = false;
		switch (tRuntime.tDesc.eSpawnKind)
		{
		case SPAWN_KIND::WILD_POKEMON: bSpawned = Try_SpawnWildPokemon(tRuntime); break;
		case SPAWN_KIND::NPC:
		case SPAWN_KIND::TRAINER:      bSpawned = Spawn_NPC(tRuntime);            break;
		default: break;
		}

		if (!bSpawned)
			tRuntime.fRespawnTimer = tRuntime.tDesc.fRespawnDelay;
	}
}

void CSpawn_Manager::Clear()
{
	m_Runtimes.clear();
	Safe_Release(m_pNavigationClone);

#ifdef _DEBUG
	Safe_Delete(m_pEffect);
	Safe_Delete(m_pBatch);
	Safe_Release(m_pInputLayout);
#endif

	m_pGameInstance = nullptr;
	m_bInitialized = false;
	m_bBegan = false;
}

_bool CSpawn_Manager::Prepare_SpawnRect(SPAWN_RECT_RUNTIME& tRuntime)
{
	tRuntime.bValid = false;
	tRuntime.iCenterCellIndex = INVALID_NAV_CELL;

	if (nullptr == m_pNavigationClone) return false;

	_float3 vProjected = {};
	_uint   iCellIndex = INVALID_NAV_CELL;

	const _bool bProjected = m_pNavigationClone->Project_PointToNavigation(
		tRuntime.tDesc.vCenter,
		tRuntime.tDesc.fProjectRadius,
		tRuntime.tDesc.iAllowedAreaMask,
		&vProjected,
		&iCellIndex);

	if (!bProjected)
	{
#ifdef _DEBUG
		OutputDebugStringA(("[SpawnRect Warn] ID=" + std::to_string(tRuntime.tDesc.iSpawnID)
			+ " failed to project center to NavMesh\n").c_str());
#endif
		return false;
	}

	tRuntime.bValid = true;
	tRuntime.vProjectedCenter = vProjected;
	tRuntime.iCenterCellIndex = iCellIndex;
	return true;
}

_bool CSpawn_Manager::Try_SpawnWildPokemon(SPAWN_RECT_RUNTIME& tRuntime)
{
	if (!tRuntime.bValid)                                       return false;
	if (tRuntime.iAliveCount >= tRuntime.tDesc.iMaxAliveCount)  return false;
	if (nullptr == m_pNavigationClone)                          return false;
	if (nullptr == m_pGameInstance)                             return false;

	const SPAWN_RECT_DESC& tDesc = tRuntime.tDesc;

	// 종/렌더룰 조회 - Find 실패는 데이터 오류, 더 이상 시도해도 동일하므로 즉시 false.
	CPokemonData_Manager* pDataMgr = CPokemonData_Manager::GetInstance();
	if (nullptr == pDataMgr) return false;

	const SPECIES_DATA* pSpecies = pDataMgr->Find_Species(tDesc.iSpeciesID_Temp);
	if (nullptr == pSpecies || 0 == pSpecies->strModelTag) return false;

	CRenderRule_Manager* pRuleMgr = CRenderRule_Manager::GetInstance();
	if (nullptr == pRuleMgr) return false;

	const CRenderRule* pRenderRule = pRuleMgr->Find_PokemonRenderRule(pSpecies);
	if (nullptr == pRenderRule) return false;

	// 사각형 내부 랜덤 점 + NavMesh 투영을 최대 g_kMaxSpawnAttemptsPerTry 회 시도.
	for (_uint i = 0; i < g_kMaxSpawnAttemptsPerTry; ++i)
	{
		const _float3 vCandidate = SpawnMath::Make_RandomPointInRect(tDesc);

		_float3 vNavPos = {};
		_uint   iCellIndex = INVALID_NAV_CELL;

		if (!m_pNavigationClone->Project_PointToNavigation(
			vCandidate, tDesc.fProjectRadius, tDesc.iAllowedAreaMask,
			&vNavPos, &iCellIndex))
			continue;

		// 투영 결과가 사각형 밖으로 끌려갔는지 재검사.
		if (!SpawnMath::Is_PointInsideRectXZ(vNavPos, tDesc))
			continue;

		// 플레이어 거리 조건.
		if (!Check_DistanceFromPlayer(vNavPos,
			tDesc.fMinDistanceFromPlayer, tDesc.fMaxDistanceFromPlayer))
			continue;

		// 도달성 검사 (옵션).
		if (tDesc.bRequireReachable)
		{
			if (!m_pNavigationClone->Is_Reachable(
				tRuntime.iCenterCellIndex, iCellIndex, tDesc.iAllowedAreaMask))
				continue;
		}

		// ===== Actor 생성 =====
		CBody_Pokemon::BODY_POKEMON_DESC BodyDesc{};
		BodyDesc.strModelProtoTag = pSpecies->strModelTag;
		BodyDesc.strShaderProtoTag = PROTO_COM_SHADER_POKEMON;
		BodyDesc.iDefaultAnim = BattleAnim::Find_AnimIndex(pSpecies->strModelTag, ANIM_KIND::IDLE);
		BodyDesc.bLoop = true;
		BodyDesc.fScale = 1.f;
		BodyDesc.bEnableRootMotion = true;
		BodyDesc.iRootMotionBoneIndex = 0;
		BodyDesc.strRootMotionBoneName = "Origin";
		BodyDesc.pRenderRule = pRenderRule;

		CActor_WildPokemon::ACTOR_WILD_DESC WildDesc{};
		WildDesc.iBodyProtoLevel = ETOUI(LEVEL::STATIC);
		WildDesc.iComponentLevel = ETOUI(LEVEL::GAMEPLAY);
		WildDesc.strBodyProtoTag = PROTO_OBJ_BODY_POKEMON;
		WildDesc.pBodyDesc = &BodyDesc;
		WildDesc.iSpeciesID = tDesc.iSpeciesID_Temp;
		WildDesc.iLevel = tDesc.iLevel_Temp;
		WildDesc.vSpawnPos = vNavPos;
		WildDesc.fRotationPerSec = XMConvertToRadians(720.f);

		WildDesc.iSpawnRectID = tDesc.iSpawnID;
		WildDesc.vSpawnAnchor = tRuntime.vProjectedCenter;
		WildDesc.fLeashRadius = tDesc.fLeashRadius;
		WildDesc.iCurrentCellIndex = iCellIndex;
		WildDesc.tSpawnRectDesc = tDesc;
		WildDesc.strBodyModelProtoTag = pSpecies->strModelTag;

		if (FAILED(m_pGameInstance->Add_GameObject(
			ETOUI(LEVEL::GAMEPLAY), PROTO_OBJ_ACTOR_WILD_POKEMON,
			ETOUI(LEVEL::GAMEPLAY), LAYER_INTERACTABLE,
			&WildDesc)))
			return false;

		++tRuntime.iAliveCount;
		tRuntime.bHasEverSpawned = true;
		tRuntime.fRespawnTimer = 0.f;
		return true;
	}

#ifdef _DEBUG
	OutputDebugStringA(("[Spawn Fail] ID=" + std::to_string(tRuntime.tDesc.iSpawnID)
		+ " no valid random point in rect after " + std::to_string(g_kMaxSpawnAttemptsPerTry)
		+ " attempts\n").c_str());
#endif
	return false;
}

_bool CSpawn_Manager::Spawn_NPC(SPAWN_RECT_RUNTIME& tRuntime)
{
	if (!tRuntime.bValid)                                      return false;
	if (tRuntime.iAliveCount >= tRuntime.tDesc.iMaxAliveCount) return false;
	if (nullptr == m_pGameInstance)                            return false;

	const SPAWN_RECT_DESC& tDesc = tRuntime.tDesc;
	const NPC_PROFILE_INFO* pProfile = Find_NpcProfile_(tDesc.eNpcProfile);
	if (nullptr == pProfile)
		return false;

	CRenderRule_Manager* pRuleMgr = CRenderRule_Manager::GetInstance();
	if (nullptr == pRuleMgr)
		return false;

	const CRenderRule* pRenderRule = pRuleMgr->Find_OrLoadMappingRule(pProfile->pMappingPath);
	if (nullptr == pRenderRule)
		return false;

	CActor_NPC::ACTOR_NPC_DESC NpcDesc{};
	NpcDesc.iBodyProtoLevel = ETOUI(LEVEL::STATIC);
	NpcDesc.iComponentLevel = ETOUI(LEVEL::GAMEPLAY);
	NpcDesc.vSpawnPos = tRuntime.vProjectedCenter;
	NpcDesc.strDialogueKey = (L'\0' != tDesc.szDialogueKey[0]) ? tDesc.szDialogueKey : pProfile->pDefaultDialogueKey;
	NpcDesc.fRotationPerSec = XMConvertToRadians(720.f);
	NpcDesc.bIsTrainer = (SPAWN_KIND::TRAINER == tDesc.eSpawnKind);
	NpcDesc.iSpawnRectID = tDesc.iSpawnID;
	NpcDesc.bApplyInitialRotation = true;
	NpcDesc.fInitialRotationY = tDesc.fRotationY;

	if (SPAWN_KIND::TRAINER == tDesc.eSpawnKind)
	{
		NpcDesc.bStartBattleAfterDialogue = true;
		NpcDesc.iTrainerID = tDesc.iTrainerID;
		/* eBattleEnvironment / eBattleRule / iBGResourceID / iZoneID 는 ACTOR_NPC_DESC 기본값
		   (GRASS / TRAINER_SINGLE / 0 / 0). 외부 데이터 파서 추가는 후속. */
	}

	if (pProfile->bPokemon)
	{
		CBody_Pokemon::BODY_POKEMON_DESC BodyDesc{};
		BodyDesc.strModelProtoTag = pProfile->strModelProtoTag;
		BodyDesc.strShaderProtoTag = PROTO_COM_SHADER_POKEMON;
		BodyDesc.iDefaultAnim = BattleAnim::Find_AnimIndex(pProfile->strModelProtoTag, ANIM_KIND::IDLE);
		BodyDesc.bLoop = true;
		BodyDesc.fScale = pProfile->fScale;
		BodyDesc.vLocalOffset = _float3(
			pProfile->fBodyOffsetX,
			pProfile->fBodyOffsetY,
			pProfile->fBodyOffsetZ);
		BodyDesc.pRenderRule = pRenderRule;

		NpcDesc.strBodyProtoTag = PROTO_OBJ_BODY_POKEMON;
		NpcDesc.pBodyDesc = &BodyDesc;

		if (FAILED(m_pGameInstance->Add_GameObject(
			ETOUI(LEVEL::GAMEPLAY), PROTO_OBJ_ACTOR_NPC,
			ETOUI(LEVEL::GAMEPLAY), LAYER_NPC,
			&NpcDesc)))
			return false;
	}
	else
	{
		CBody_Human::BODY_HUMAN_DESC BodyDesc{};
		BodyDesc.strModelProtoTag = pProfile->strModelProtoTag;
		BodyDesc.iDefaultAnim = BattleAnim::Find_AnimIndex(pProfile->strModelProtoTag, ANIM_KIND::IDLE);
		BodyDesc.bLoop = true;
		BodyDesc.fScale = pProfile->fScale;
		BodyDesc.vLocalOffset = _float3(
			pProfile->fBodyOffsetX,
			pProfile->fBodyOffsetY,
			pProfile->fBodyOffsetZ);
		BodyDesc.pRenderRule = pRenderRule;

		NpcDesc.strBodyProtoTag = PROTO_OBJ_BODY_HUMAN;
		NpcDesc.pBodyDesc = &BodyDesc;

		if (FAILED(m_pGameInstance->Add_GameObject(
			ETOUI(LEVEL::GAMEPLAY), PROTO_OBJ_ACTOR_NPC,
			ETOUI(LEVEL::GAMEPLAY), LAYER_NPC,
			&NpcDesc)))
			return false;
	}

	++tRuntime.iAliveCount;
	tRuntime.bHasEverSpawned = true;
	tRuntime.fRespawnTimer = 0.f;

	return true;
}

_bool CSpawn_Manager::Check_DistanceFromPlayer(const _float3& vPos, _float fMin, _float fMax) const
{
	// 두 값이 모두 0 이하면 거리 조건 비활성.
	if (fMin <= 0.f && fMax <= 0.f) return true;
	if (nullptr == m_pGameInstance) return true;

	const list<CGameObject*>* pPlayerList =
		m_pGameInstance->Get_ObjectList(ETOUI(LEVEL::GAMEPLAY), LAYER_PLAYER);
	if (nullptr == pPlayerList || pPlayerList->empty()) return true;

	CGameObject* pPlayer = pPlayerList->front();
	if (nullptr == pPlayer) return true;

	const _vector vPlayerPos = pPlayer->Get_Transform()->Get_State(STATE::POSITION);
	const _vector vSpawnPos = XMLoadFloat3(&vPos);
	const _vector vDelta = vSpawnPos - vPlayerPos;

	const _float fDistSq = XMVectorGetX(XMVector3LengthSq(vDelta));

	if (fMin > 0.f && fDistSq < fMin * fMin) return false;
	if (fMax > 0.f && fDistSq > fMax * fMax) return false;
	return true;
}

void CSpawn_Manager::Recount_AliveCounts()
{
	// 1) WILD_POKEMON rect 만 직전 alive 스냅샷 + 카운트 reset.
	//    TRAINER rect 의 iAliveCount 는 Spawn_Trainer 가 ++ 한 값을 유지 (1회 스폰 후 변동 없음).
	for (auto& tRuntime : m_Runtimes)
	{
		if (tRuntime.tDesc.eSpawnKind != SPAWN_KIND::WILD_POKEMON) continue;

		tRuntime.iPrevAliveCount = tRuntime.iAliveCount;
		tRuntime.iAliveCount = 0;
	}

	if (nullptr == m_pGameInstance) return;

	const list<CGameObject*>* pList =
		m_pGameInstance->Get_ObjectList(ETOUI(LEVEL::GAMEPLAY), LAYER_INTERACTABLE);
	if (nullptr == pList) return;

	// 2) 현재 alive 카운트 재집계 (LAYER_INTERACTABLE 의 wild actor 만).
	for (CGameObject* pObj : *pList)
	{
		if (nullptr == pObj)   continue;
		if (pObj->Is_Dead())   continue;

		CActor_WildPokemon* pWild = dynamic_cast<CActor_WildPokemon*>(pObj);
		if (nullptr == pWild) continue;

		const _uint iRectID = pWild->Get_SpawnRectID();
		for (auto& tRuntime : m_Runtimes)
		{
			if (tRuntime.tDesc.iSpawnID == iRectID)
			{
				++tRuntime.iAliveCount;
				break;
			}
		}
	}

	// 3) 사망 감지 - WILD_POKEMON rect 만 대상.
	for (auto& tRuntime : m_Runtimes)
	{
		if (tRuntime.tDesc.eSpawnKind != SPAWN_KIND::WILD_POKEMON) continue;

		if (tRuntime.iAliveCount < tRuntime.iPrevAliveCount)
		{
			tRuntime.fRespawnTimer = tRuntime.tDesc.fRespawnDelay;
		}
	}
}

#ifdef _DEBUG
HRESULT CSpawn_Manager::Ready_DebugResources()
{
	ID3D11Device* pDevice = nullptr;
	ID3D11DeviceContext* pContext = nullptr;
	if (nullptr == m_pNavigationClone) return E_FAIL;

	// CNavigation 으로부터 디바이스/컨텍스트 빌릴 수 있지만 직접 노출이 없으므로
	// CGameInstance 의 보유 디바이스를 사용 - Component 와 동일 패턴.
	pDevice = m_pGameInstance->Get_Device();
	pContext = m_pGameInstance->Get_Context();
	if (nullptr == pDevice || nullptr == pContext) return E_FAIL;

	m_pBatch = new PrimitiveBatch<VertexPositionColor>(pContext);
	m_pEffect = new BasicEffect(pDevice);
	m_pEffect->SetVertexColorEnabled(true);

	const void* pShaderByteCode = nullptr;
	size_t      iShaderByteCodeLength = 0;
	m_pEffect->GetVertexShaderBytecode(&pShaderByteCode, &iShaderByteCodeLength);

	if (FAILED(pDevice->CreateInputLayout(
		VertexPositionColor::InputElements, VertexPositionColor::InputElementCount,
		pShaderByteCode, iShaderByteCodeLength, &m_pInputLayout)))
		return E_FAIL;

	return S_OK;
}

HRESULT CSpawn_Manager::Render_Debug()
{
	if (nullptr == m_pBatch || nullptr == m_pEffect || nullptr == m_pInputLayout) return E_FAIL;
	if (nullptr == m_pGameInstance) return E_FAIL;

	ID3D11DeviceContext* pContext = m_pGameInstance->Get_Context();
	if (nullptr == pContext) return E_FAIL;

	pContext->GSSetShader(nullptr, nullptr, 0);

	m_pEffect->SetWorld(XMMatrixIdentity());
	m_pEffect->SetView(XMLoadFloat4x4(m_pGameInstance->Get_Transform(D3DTS::VIEW)));
	m_pEffect->SetProjection(XMLoadFloat4x4(m_pGameInstance->Get_Transform(D3DTS::PROJ)));
	pContext->IASetInputLayout(m_pInputLayout);

	m_pEffect->Apply(pContext);
	m_pBatch->Begin();

	for (const auto& tRuntime : m_Runtimes)
	{
		const SPAWN_RECT_DESC& tDesc = tRuntime.tDesc;

		// 사각형 OBB (유효 = 초록, 무효 = 빨강)
		const _float3 vCenter = tRuntime.bValid
			? tRuntime.vProjectedCenter
			: tDesc.vCenter;
		const _float3 vExtents = _float3(tDesc.vSize.x * 0.5f, 0.05f, tDesc.vSize.y * 0.5f);

		_float4 vQuat{};
		XMStoreFloat4(&vQuat, XMQuaternionRotationRollPitchYaw(0.f, tDesc.fRotationY, 0.f));

		const BoundingOrientedBox obb(vCenter, vExtents, vQuat);
		const _vector vColor = tRuntime.bValid
			? Colors::Blue.v
			: Colors::Black.v;

		DX::Draw(m_pBatch, obb, vColor);

		// 유효한 경우 ProjectedCenter 에 작은 sphere
		if (tRuntime.bValid)
		{
			const BoundingSphere sphere(tRuntime.vProjectedCenter, 0.2f);
			DX::Draw(m_pBatch, sphere, Colors::Yellow.v);
		}
	}

	m_pBatch->End();
	return S_OK;
}
#endif

void CSpawn_Manager::Free()
{
	Clear();
	__super::Free();
}