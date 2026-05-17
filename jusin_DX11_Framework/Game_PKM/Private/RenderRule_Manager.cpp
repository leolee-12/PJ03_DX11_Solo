#include "RenderRule_Manager.h"
#include "Battle_Data.h"

IMPLEMENT_SINGLETON(CRenderRule_Manager)

CRenderRule_Manager::CRenderRule_Manager()
{
}

HRESULT CRenderRule_Manager::Initialize()
{
	if (true == m_bInitialized)
		return S_OK;

	if (FAILED(Load_BuiltinRules()))
		return E_FAIL;

	m_bInitialized = true;

	return S_OK;
}

const CRenderRule* CRenderRule_Manager::Find_Rule(RENDER_RULE_KEY eKey) const
{
	auto iter = m_Rules.find(ETOUI(eKey));
	if (iter == m_Rules.end())
		return nullptr;

	return iter->second;
}

const CRenderRule* CRenderRule_Manager::Find_OrLoadMappingRule(const _char* pMappingFilePath)
{
	if (nullptr == pMappingFilePath || '\0' == pMappingFilePath[0])
		return nullptr;

	const string strPath = pMappingFilePath;

	auto iter = m_MappingRules.find(strPath);
	if (iter != m_MappingRules.end())
		return iter->second;

	CRenderRule* pRule = CRenderRule_MappingJson::Create_FromFile(pMappingFilePath);
	if (nullptr == pRule)
		return nullptr;

	m_MappingRules.emplace(strPath, pRule);

	return pRule;
}

const CRenderRule* CRenderRule_Manager::Find_PokemonRenderRule(const SPECIES_DATA* pSpecies)
{
	if (nullptr == pSpecies)
		return nullptr;

	const CRenderRule* pRule = Find_OrLoadMappingRule(pSpecies->pRenderMappingPath);

	if (nullptr == pRule)
		pRule = Find_Rule(pSpecies->eRenderRuleKey);

	return pRule;
}

HRESULT CRenderRule_Manager::Load_BuiltinRules()
{
	{
		vector<CRenderRule_Static::RULE_ENTRY> Rules;
		Rules.push_back({ "*", 0, {} });

		if (FAILED(Register_StaticRuleWithFileFallback(
			RENDER_RULE_KEY::POKEMON_DEFAULT,
			"../../DataFiles/RenderRules/pokemon_default.rrule",
			Rules)))
			return E_FAIL;
	}

	{
		vector<CRenderRule_Static::RULE_ENTRY> Rules;
		Rules.push_back({ "*", 0, {} });

		if (FAILED(Register_StaticRuleWithFileFallback(
			RENDER_RULE_KEY::MAP_DEFAULT,
			"../../DataFiles/RenderRules/map_default.rrule",
			Rules)))
			return E_FAIL;

		return S_OK;
	}
}

HRESULT CRenderRule_Manager::Register_StaticRule(RENDER_RULE_KEY eKey, const vector<CRenderRule_Static::RULE_ENTRY>& Rules)
{
	if (eKey >= RENDER_RULE_KEY::END)
		return E_FAIL;

	const _uint iKey = ETOUI(eKey);

	auto iter = m_Rules.find(iKey);
	if (iter != m_Rules.end())
		Safe_Release(iter->second);

	CRenderRule* pRule = CRenderRule_Static::Create(Rules);
	if (nullptr == pRule)
		return E_FAIL;

	m_Rules[iKey] = pRule;

	return S_OK;
}

HRESULT CRenderRule_Manager::Register_StaticRuleWithFileFallback(
	RENDER_RULE_KEY eKey,
	const _char* pRuleFilePath,
	const vector<CRenderRule_Static::RULE_ENTRY>& FallbackRules)
{
	if (eKey >= RENDER_RULE_KEY::END)
		return E_FAIL;

	const _uint iKey = ETOUI(eKey);

	auto iter = m_Rules.find(iKey);
	if (iter != m_Rules.end())
	{
		Safe_Release(iter->second);
		m_Rules.erase(iter);
	}

	CRenderRule* pRule = CRenderRule_Static::Create_FromFile(pRuleFilePath);

	if (nullptr == pRule)
		pRule = CRenderRule_Static::Create(FallbackRules);

	if (nullptr == pRule)
		return E_FAIL;

	m_Rules.emplace(iKey, pRule);

	return S_OK;
}

void CRenderRule_Manager::Free()
{
	for (auto& Pair : m_Rules)
		Safe_Release(Pair.second);

	for (auto& Pair : m_MappingRules)
		Safe_Release(Pair.second);

	m_Rules.clear();
	m_MappingRules.clear();

	m_bInitialized = false;

	__super::Free();
}