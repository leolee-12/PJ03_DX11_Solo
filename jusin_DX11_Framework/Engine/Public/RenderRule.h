#pragma once
#include "Base.h"

NS_BEGIN(Engine)

#pragma region RenderRule
class ENGINE_DLL CRenderRule abstract : public CBase
{
public:
	struct VARIANT_OVERRIDE
	{
		MATERIAL_TYPE eType = { MATERIAL_TYPE::DIFFUSE };
		_uint iSlot = {};
	};

	struct RESOLVE_RESULT
	{
		_uint iPass = {};
		vector<VARIANT_OVERRIDE> Variants;

		_bool bHasLayerColors = { false };
		array<_float4, 4> LayerColors = {};
	};

protected:
	  CRenderRule();
	  CRenderRule(const CRenderRule& Prototype);
	  virtual ~CRenderRule() = default;

public:
	  virtual _bool Resolve(const _char* pMeshName, _uint iMaterialIndex, RESOLVE_RESULT& Out) const PURE;

protected:
	  virtual void Free() override;
};
#pragma endregion

#pragma region RenderRule_Static
class ENGINE_DLL CRenderRule_Static final : public CRenderRule
{
public:
	struct RULE_ENTRY
	{
		string strPattern;
		_uint iPass = {};
		vector<VARIANT_OVERRIDE> Variants;
	};

private:
	CRenderRule_Static();
	CRenderRule_Static(const CRenderRule_Static& Prototype);
	virtual ~CRenderRule_Static() = default;

public:
	HRESULT Initialize(const vector<RULE_ENTRY>& Rules);
	virtual _bool Resolve(const _char* pMeshName, _uint iMaterialIndex, RESOLVE_RESULT& Out) const override;

private:
	vector<RULE_ENTRY> m_Rules;

private:
	static _bool Match_Wildcard(const string& Pattern, const string& Text);
	static string To_Lower(const _char* pText);
	static string To_Lower(const string& Text);

	static HRESULT Parse_File(const _char* pRuleFilePath, vector<RULE_ENTRY>& OutRules);
	static HRESULT Parse_Line(const string& Line, RULE_ENTRY& OutRule, _bool& bHasRule);
	static HRESULT Parse_VariantOverrides(const string& Text, vector<VARIANT_OVERRIDE>& OutVariants);
	static HRESULT Parse_MaterialType(const string& Text, MATERIAL_TYPE& OutType);
	static HRESULT Parse_UInt(const string& Text, _uint& OutValue);
	static string Trim(const string& Text);

public:
	static CRenderRule_Static* Create(const vector<RULE_ENTRY>& Rules);
	static CRenderRule_Static* Create_FromFile(const _char* pRuleFilePath);

protected:
	virtual void Free() override;
};
#pragma endregion

#pragma region RenderRule_MappingJson
class ENGINE_DLL CRenderRule_MappingJson final : public CRenderRule
{
public:
	struct MATERIAL_RULE
	{
		_uint iPass = {};
		vector<VARIANT_OVERRIDE> Variants;
		_bool bHasLayerColors = { false };
		array<_float4, 4> LayerColors = {};
	};

private:
	CRenderRule_MappingJson();
	CRenderRule_MappingJson(const CRenderRule_MappingJson& Prototype);
	virtual ~CRenderRule_MappingJson() = default;

public:
	HRESULT Initialize(const _char* pMappingFilePath);
	virtual _bool Resolve(const _char* pMeshName, _uint iMaterialIndex, RESOLVE_RESULT& Out) const
		override;

private:
	unordered_map<_uint, MATERIAL_RULE> m_MaterialRules;

private:
	static HRESULT Parse_RenderRule(const json& Material, MATERIAL_RULE& OutRule, _bool& bHasRender);
	static HRESULT Parse_MaterialTypeKey(const string& Key, MATERIAL_TYPE& OutType);
	static HRESULT Parse_UInt(const string& Text, _uint& OutValue);
	static HRESULT Parse_LayerColors(const json& Render, MATERIAL_RULE& OutRule);
	static HRESULT Parse_LayerColorValue(const json& Value, _float4& OutColor);
	static _int LayerColorIndex(const string& strKey);

public:
	static CRenderRule_MappingJson* Create_FromFile(const _char* pMappingFilePath);

protected:
	virtual void Free() override;
};
#pragma endregion

NS_END