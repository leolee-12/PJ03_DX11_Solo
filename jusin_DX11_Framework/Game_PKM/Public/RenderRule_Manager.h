#pragma once
#include "Base.h"
#include "Game_PKM_Defines.h"
#include "RenderRule.h"

NS_BEGIN(Game_PKM)

class CRenderRule_Manager final : public CBase
{
	DECLARE_SINGLETON(CRenderRule_Manager)

private:
	CRenderRule_Manager();
	virtual ~CRenderRule_Manager() = default;

public:
	HRESULT Initialize();

	const CRenderRule* Find_Rule(RENDER_RULE_KEY eKey) const;
	const CRenderRule* Find_OrLoadMappingRule(const _char* pMappingFilePath);

private:
	HRESULT Load_BuiltinRules();
	HRESULT Register_StaticRule(RENDER_RULE_KEY eKey, const vector<CRenderRule_Static::RULE_ENTRY>& Rules);
	HRESULT Register_StaticRuleWithFileFallback(
		RENDER_RULE_KEY eKey,
		const _char* pRuleFilePath,
		const vector<CRenderRule_Static::RULE_ENTRY>& FallbackRules);

private:
	unordered_map<_uint, CRenderRule*> m_Rules;
	unordered_map<string, CRenderRule*> m_MappingRules;
	_bool m_bInitialized = { false };

private:
	virtual void Free() override;
};

NS_END