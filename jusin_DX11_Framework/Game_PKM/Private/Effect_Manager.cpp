#include "Effect_Manager.h"
#include "Effect.h"
#include "GameInstance.h"

IMPLEMENT_SINGLETON(CEffect_Manager)

namespace
{
    using namespace Game_PKM;

    /* enum string mapping */
    CParticleEmitter::EMITTER_DESC::BILLBOARD_MODE BillboardFromString(const _string& s)
    {
        if (s == "AXIS_LOCKED")  return CParticleEmitter::EMITTER_DESC::BILLBOARD_MODE::AXIS_LOCKED;
        if (s == "FIXED_NORMAL") return
            CParticleEmitter::EMITTER_DESC::BILLBOARD_MODE::FIXED_NORMAL;
        return CParticleEmitter::EMITTER_DESC::BILLBOARD_MODE::VIEW_ALIGNED;
    }

    CParticleEmitter::EMITTER_DESC::BLEND_MODE BlendFromString(const _string& s)
    {
        if (s == "ALPHA") return CParticleEmitter::EMITTER_DESC::BLEND_MODE::ALPHA;
        return CParticleEmitter::EMITTER_DESC::BLEND_MODE::ADDITIVE;
    }

    /* 키 배열 파싱: [[t, v], [t, v], ...] — scalar curve */
    void Read_CurveFloat(const json& jKeys, CCurveFloat& outCurve)
    {
        outCurve.Clear();
        if (!jKeys.is_array()) return;
        for (const auto& jk : jKeys)
        {
            if (!jk.is_array() || jk.size() < 2) continue;
            const _float t = jk[0].get<_float>();
            const _float v = jk[1].get<_float>();
            outCurve.Add_Key(t, v);
        }
    }

    /* 키 배열 파싱: [[t, [r,g,b,a]], ...] — color curve */
    void Read_CurveColor(const json& jKeys, CCurveColor& outCurve)
    {
        outCurve.Clear();
        if (!jKeys.is_array()) return;
        for (const auto& jk : jKeys)
        {
            if (!jk.is_array() || jk.size() < 2) continue;
            if (!jk[1].is_array() || jk[1].size() < 4) continue;
            const _float t = jk[0].get<_float>();
            const _float4 v = _float4(
                jk[1][0].get<_float>(),
                jk[1][1].get<_float>(),
                jk[1][2].get<_float>(),
                jk[1][3].get<_float>());
            outCurve.Add_Key(t, v);
        }
    }

    /* 안전 getter: 키 없으면 default 유지 */
    template <typename T>
    void GetOpt(const json& j, const _char* key, T& out)
    {
        if (j.contains(key))
            out = j[key].get<T>();
    }

    void Read_Float2(const json& j, _float2& out)
    {
        if (j.is_array() && j.size() >= 2)
            out = _float2(j[0].get<_float>(), j[1].get<_float>());
    }
    void Read_Float3(const json& j, _float3& out)
    {
        if (j.is_array() && j.size() >= 3)
            out = _float3(j[0].get<_float>(), j[1].get<_float>(), j[2].get<_float>());
    }

    HRESULT Build_EmitterDef(const json& je, EMITTER_DEFINITION& out)
    {
        GetOpt(je, "name", out.strName);
        GetOpt(je, "capacity", out.iCapacity);
        GetOpt(je, "spawnRate", out.fSpawnRate);
        GetOpt(je, "burstCount", out.iBurstCount);
        if (je.contains("lifeTimeRange")) Read_Float2(je["lifeTimeRange"], out.vLifeTimeRange);
        if (je.contains("speedRange"))    Read_Float2(je["speedRange"], out.vSpeedRange);
        if (je.contains("sizeRange"))     Read_Float2(je["sizeRange"], out.vSizeRange);
        if (je.contains("emitDirection")) Read_Float3(je["emitDirection"], out.vEmitDirection);
        GetOpt(je, "emitConeHalfAngle", out.fEmitConeHalfAngle);

        if (je.contains("billboard"))
            out.eBillboard = BillboardFromString(je["billboard"].get<_string>());
        if (je.contains("billboardFixedAxis"))
            Read_Float3(je["billboardFixedAxis"], out.vBillboardFixedAxis);
        if (je.contains("blend"))
            out.eBlend = BlendFromString(je["blend"].get<_string>());

        /* textureProtoTag: M7b는 저장만, emitter 적용은 별도 단위 */
        if (je.contains("textureProtoTag"))
        {
            const _string s = je["textureProtoTag"].get<_string>();
            if (!s.empty())
                out.strTextureProtoTag = WNAME(StoW(s));
        }

        if (je.contains("curves"))
        {
            const json& jc = je["curves"];
            if (jc.contains("size"))  Read_CurveFloat(jc["size"], out.curveSize);
            if (jc.contains("color")) Read_CurveColor(jc["color"], out.curveColor);
            if (jc.contains("alpha")) Read_CurveFloat(jc["alpha"], out.curveAlpha);
        }

        return S_OK;
    }

    HRESULT Parse_EffectJson(const json& j, EFFECT_DEFINITION& out)
    {
        if (!j.contains("id") || !j["id"].is_string())
            return E_FAIL;

        out.strID = j["id"].get<_string>();
        out.Emitters.clear();

        if (j.contains("emitters") && j["emitters"].is_array())
        {
            for (const auto& je : j["emitters"])
            {
                EMITTER_DEFINITION em{};
                if (FAILED(Build_EmitterDef(je, em)))
                    continue;
                out.Emitters.push_back(em);
            }
        }

        return S_OK;
    }

    HRESULT Parse_File(const std::filesystem::path& path, EFFECT_DEFINITION& out)
    {
        std::ifstream ifs(path);
        if (!ifs.is_open())
            return E_FAIL;

        try
        {
            json j;
            ifs >> j;
            return Parse_EffectJson(j, out);
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
		return nullptr;

	CEffect::EFFECT_DESC desc{};
	desc.vSpawnPos = vSpawnPos;
	desc.pDefinition = pDef;
	desc.iSpawnLevel = iLevel;
	desc.strLayerTag = strLayerTag;
    desc.tAttach = tAttach;

	CGameInstance* pGI = CGameInstance::GetInstance();

	CBase* pCloned = pGI->Clone_Prototype(
		PROTOTYPE::GAMEOBJECT, iLevel, PROTO_OBJ_EFFECT, &desc);

	if (nullptr == pCloned)
		return nullptr;

	CEffect* pEffect = static_cast<CEffect*>(pCloned);

	if (FAILED(pGI->Add_GameObject_Ex(iLevel, strLayerTag, pEffect)))
	{
		Safe_Release(pEffect);
		return nullptr;
	}

	return pEffect;   // borrowed
}

void CEffect_Manager::Free()
{
	m_Definitions.clear();
}