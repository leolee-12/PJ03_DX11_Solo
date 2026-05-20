#include "Effect_Manager.h"
#include "Effect.h"
#include "GameInstance.h"

IMPLEMENT_SINGLETON(CEffect_Manager)

namespace
{
    using namespace Game_PKM;

    HRESULT Parse_File(const std::filesystem::path& path, EFFECT_DEFINITION& out)
    {
        std::ifstream ifs(path);
        if (!ifs.is_open())
            return E_FAIL;

        try
        {
            json j;
            ifs >> j;
            return Effect_ParseDefinitionJson(j, out);
        }
        catch (const std::exception&)
        {
            return E_FAIL;
        }
    }
}

CEffect_Manager::CEffect_Manager()
{
}

HRESULT CEffect_Manager::Initialize()
{
    if (m_bInitialized)
        return S_OK;

    /* 디스크의 *.effect.json 자동 로드. 실패해도 Initialize 자체는 성공으로 본다
       (검증/디버그용 임시 코드 등록도 가능한 상태 유지). */
    Load_Definitions("../../Resources/Effects");

    m_bInitialized = true;
    return S_OK;
}

HRESULT CEffect_Manager::Register_Definition(const EFFECT_DEFINITION& def)
{
	if (def.strID.empty())
		return E_FAIL;

	m_Definitions[def.strID] = def;   // 같은 ID로 재등록 시 덮어씀
	return S_OK;
}

const EFFECT_DEFINITION* CEffect_Manager::Find_Definition(const _string& strID) const
{
	auto iter = m_Definitions.find(strID);
	if (iter == m_Definitions.end())
		return nullptr;
	return &iter->second;
}

HRESULT CEffect_Manager::Load_Definitions(const _char* pFolderPath)
{
    namespace fs = std::filesystem;

    if (nullptr == pFolderPath)
        return E_FAIL;

    fs::path dir = pFolderPath;
    std::error_code ec;
    if (!fs::exists(dir, ec) || !fs::is_directory(dir, ec))
    {
        /* 디렉토리 없으면 silent skip. M7b 시점 폴더 미존재 자체는 에러가 아님
           (사용자가 아직 폴더 생성 전일 수 있음). */
        return S_FALSE;
    }

    _uint iLoaded = 0;
    _uint iFailed = 0;
    for (const auto& entry : fs::directory_iterator(dir, ec))
    {
        if (!entry.is_regular_file()) continue;
        if (entry.path().extension() != ".json") continue;

        EFFECT_DEFINITION def{};
        if (FAILED(Parse_File(entry.path(), def)) || def.strID.empty())
        {
            ++iFailed;
            continue;
        }
        Register_Definition(def);
        ++iLoaded;
    }

#ifdef _DEBUG
    {
        char szLog[256];
        sprintf_s(szLog, "[Effect_Manager] Loaded %u definitions (failed %u) from %s\n",
            iLoaded, iFailed, pFolderPath);
        OutputDebugStringA(szLog);
    }
#endif

    return (iLoaded > 0) ? S_OK : S_FALSE;
}

CEffect* CEffect_Manager::Spawn(const _string& strID, const _float3& vSpawnPos,
    _uint iLevel, WNameID strLayerTag, const CEffect::EFFECT_DESC::ATTACH_INFO& tAttach)
{
    const EFFECT_DEFINITION* pDef = Find_Definition(strID);
    if (nullptr == pDef)
    {
        OutputDebugStringA("[EffectMgr] Spawn fail: definition not found\n");
        return nullptr;
    }

    CEffect::EFFECT_DESC desc{};
    desc.vSpawnPos = vSpawnPos;
    desc.pDefinition = pDef;
    desc.iSpawnLevel = iLevel;
    desc.strLayerTag = strLayerTag;
    desc.tAttach = tAttach;

    CGameInstance* pGI = CGameInstance::GetInstance();

    CBase* pCloned = pGI->Clone_Prototype(
        PROTOTYPE::GAMEOBJECT, ETOUI(LEVEL::STATIC), PROTO_OBJ_EFFECT, &desc);

    if (nullptr == pCloned)
    {
        OutputDebugStringA("[EffectMgr] Spawn fail: Clone_Prototype(EFFECT) returned null\n");
        return nullptr;
    }

    CEffect* pEffect = static_cast<CEffect*>(pCloned);

    if (FAILED(pGI->Add_GameObject_Ex(iLevel, strLayerTag, pEffect)))
    {
        OutputDebugStringA("[EffectMgr] Spawn fail: Add_GameObject_Ex failed\n");
        Safe_Release(pEffect);
        return nullptr;
    }

    OutputDebugStringA("[EffectMgr] Spawn ok\n");
    return pEffect;
}

void CEffect_Manager::Free()
{
	m_Definitions.clear();
}