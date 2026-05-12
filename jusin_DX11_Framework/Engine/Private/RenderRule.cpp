#include "RenderRule.h"

#include <cctype>
#include <sstream>
#include <cstdlib>
#include <fstream>

#pragma region RenderRule
CRenderRule::CRenderRule()
{
}

CRenderRule::CRenderRule(const CRenderRule& Prototype)
	: CBase{ Prototype }
{
}

void CRenderRule::Free()
{
	__super::Free();
}
#pragma endregion

#pragma region RenderRule_Static
CRenderRule_Static::CRenderRule_Static()
	: CRenderRule{}
{
}

CRenderRule_Static::CRenderRule_Static(const CRenderRule_Static& Prototype)
	: CRenderRule{ Prototype }
	, m_Rules{ Prototype.m_Rules }
{
}

HRESULT CRenderRule_Static::Initialize(const vector<RULE_ENTRY>& Rules)
{
	m_Rules = Rules;
	return S_OK;
}

_bool CRenderRule_Static::Resolve(const _char* pMeshName, _uint /* iMaterialIndex */,
	RESOLVE_RESULT& Out) const
{
	const string strMeshName = To_Lower(pMeshName);

	for (const auto& Rule : m_Rules)
	{
		if (true == Match_Wildcard(To_Lower(Rule.strPattern), strMeshName))
		{
			Out.iPass = Rule.iPass;
			Out.Variants = Rule.Variants;
			return true;
		}
	}

	return false;
}

_bool CRenderRule_Static::Match_Wildcard(const string& Pattern, const string& Text)
{
	size_t iPattern = 0;
	size_t iText = 0;
	size_t iStar = string::npos;
	size_t iMatch = 0;

	while (iText < Text.size())
	{
		if (iPattern < Pattern.size() &&
			(Pattern[iPattern] == Text[iText]))
		{
			++iPattern;
			++iText;
		}
		else if (iPattern < Pattern.size() && Pattern[iPattern] == '*')
		{
			iStar = iPattern++;
			iMatch = iText;
		}
		else if (iStar != string::npos)
		{
			iPattern = iStar + 1;
			iText = ++iMatch;
		}
		else
		{
			return false;
		}
	}

	while (iPattern < Pattern.size() && Pattern[iPattern] == '*')
		++iPattern;

	return iPattern == Pattern.size();
}

string CRenderRule_Static::To_Lower(const _char* pText)
{
	if (nullptr == pText)
		return "";

	return To_Lower(string{ pText });
}

string CRenderRule_Static::To_Lower(const string& Text)
{
	string Result = Text;

	for (char& ch : Result)
		ch = static_cast<char>(tolower(static_cast<unsigned char>(ch)));

	return Result;
}

HRESULT CRenderRule_Static::Parse_File(const _char* pRuleFilePath, vector<RULE_ENTRY>& OutRules)
{
	if (nullptr == pRuleFilePath)
		return E_FAIL;

	ifstream File{ pRuleFilePath };
	if (false == File.is_open())
		return E_FAIL;

	string Line;
	while (getline(File, Line))
	{
		RULE_ENTRY Rule{};
		_bool bHasRule = false;

		if (FAILED(Parse_Line(Line, Rule, bHasRule)))
			return E_FAIL;

		if (true == bHasRule)
			OutRules.push_back(Rule);
	}

	return OutRules.empty() ? E_FAIL : S_OK;
}

HRESULT CRenderRule_Static::Parse_Line(const string& Line, RULE_ENTRY& OutRule, _bool& bHasRule)
{
	bHasRule = false;

	string Body = Line;
	const size_t iComment = Body.find('#');
	if (iComment != string::npos)
		Body = Body.substr(0, iComment);

	Body = Trim(Body);
	if (Body.empty())
		return S_OK;

	stringstream Stream{ Body };

	string strPass;
	if (!(Stream >> OutRule.strPattern >> strPass))
		return E_FAIL;

	if (FAILED(Parse_UInt(strPass, OutRule.iPass)))
		return E_FAIL;

	string strOverrides;
	getline(Stream, strOverrides);
	strOverrides = Trim(strOverrides);

	if (false == strOverrides.empty())
	{
		if (FAILED(Parse_VariantOverrides(strOverrides, OutRule.Variants)))
			return E_FAIL;
	}

	bHasRule = true;
	return S_OK;
}

HRESULT CRenderRule_Static::Parse_VariantOverrides(const string& Text, vector<VARIANT_OVERRIDE>&
	OutVariants)
{
	size_t iBegin = 0;

	while (iBegin < Text.size())
	{
		const size_t iEnd = Text.find(',', iBegin);
		const string Token = Trim(Text.substr(iBegin, iEnd - iBegin));

		if (false == Token.empty())
		{
			const size_t iEqual = Token.find('=');
			if (iEqual == string::npos)
				return E_FAIL;

			VARIANT_OVERRIDE Override{};

			if (FAILED(Parse_MaterialType(Trim(Token.substr(0, iEqual)), Override.eType)))
				return E_FAIL;

			if (FAILED(Parse_UInt(Trim(Token.substr(iEqual + 1)), Override.iSlot)))
				return E_FAIL;

			OutVariants.push_back(Override);
		}

		if (iEnd == string::npos)
			break;

		iBegin = iEnd + 1;
	}

	return S_OK;
}

HRESULT CRenderRule_Static::Parse_MaterialType(const string& Text, MATERIAL_TYPE& OutType)
{
	const string Key = To_Lower(Text);

	if ("diffuse" == Key) OutType = MATERIAL_TYPE::DIFFUSE;
	else if ("emissive" == Key) OutType = MATERIAL_TYPE::EMISSIVE;
	else if ("layer_color" == Key) OutType = MATERIAL_TYPE::LAYER_COLOR;
	else if ("layer_mask" == Key) OutType = MATERIAL_TYPE::LAYER_MASK;
	else if ("specular" == Key) OutType = MATERIAL_TYPE::SPECULAR;
	else if ("normals" == Key || "normal" == Key) OutType = MATERIAL_TYPE::NORMALS;
	else if ("opacity" == Key) OutType = MATERIAL_TYPE::OPACITY;
	else return E_FAIL;

	return S_OK;
}

HRESULT CRenderRule_Static::Parse_UInt(const string& Text, _uint& OutValue)
{
	char* pEnd = nullptr;
	const unsigned long Value = strtoul(Text.c_str(), &pEnd, 10);

	if (pEnd == Text.c_str() || '\0' != *pEnd)
		return E_FAIL;

	OutValue = static_cast<_uint>(Value);
	return S_OK;
}

string CRenderRule_Static::Trim(const string& Text)
{
	size_t iBegin = 0;
	while (iBegin < Text.size() && isspace(static_cast<unsigned char>(Text[iBegin])))
		++iBegin;

	size_t iEnd = Text.size();
	while (iEnd > iBegin && isspace(static_cast<unsigned char>(Text[iEnd - 1])))
		--iEnd;

	return Text.substr(iBegin, iEnd - iBegin);
}

CRenderRule_Static* CRenderRule_Static::Create(const vector<RULE_ENTRY>& Rules)
{
	CRenderRule_Static* pInstance = new CRenderRule_Static();

	if (FAILED(pInstance->Initialize(Rules)))
	{
		MSG_BOX("Failed to Created : CRenderRule_Static");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CRenderRule_Static* CRenderRule_Static::Create_FromFile(const _char* pRuleFilePath)
{
	vector<RULE_ENTRY> Rules;

	if (FAILED(Parse_File(pRuleFilePath, Rules)))
		return nullptr;

	return Create(Rules);
}

void CRenderRule_Static::Free()
{
	m_Rules.clear();

	__super::Free();
}
#pragma endregion

#pragma region RenderRule_MappingJson
CRenderRule_MappingJson::CRenderRule_MappingJson()
	: CRenderRule{}
{
}

CRenderRule_MappingJson::CRenderRule_MappingJson(const CRenderRule_MappingJson& Prototype)
	: CRenderRule{ Prototype }
	, m_MaterialRules{ Prototype.m_MaterialRules }
{
}

HRESULT CRenderRule_MappingJson::Initialize(const _char* pMappingFilePath)
{
	if (nullptr == pMappingFilePath)
		return E_FAIL;

	ifstream File{ pMappingFilePath };
	if (false == File.is_open())
		return E_FAIL;

	json Root = json::parse(File, nullptr, false);
	if (true == Root.is_discarded() || false == Root.is_object())
		return E_FAIL;

	const auto iterMaterials = Root.find("materials");
	if (iterMaterials == Root.end() || false == iterMaterials->is_array())
		return E_FAIL;

	for (const auto& Material : *iterMaterials)
	{
		if (false == Material.is_object())
			return E_FAIL;

		if (false == Material.contains("index") || false == Material["index"].is_number_unsigned())
			return E_FAIL;

		MATERIAL_RULE Rule{};
		_bool bHasRender = false;

		if (FAILED(Parse_RenderRule(Material, Rule, bHasRender)))
			return E_FAIL;

		if (false == bHasRender)
			continue;

		const _uint iMaterialIndex = Material["index"].get<_uint>();
		m_MaterialRules[iMaterialIndex] = Rule;
	}

	return m_MaterialRules.empty() ? E_FAIL : S_OK;
}

_bool CRenderRule_MappingJson::Resolve(const _char* /* pMeshName */, _uint iMaterialIndex, RESOLVE_RESULT& Out) const
{
	auto iter = m_MaterialRules.find(iMaterialIndex);
	if (iter == m_MaterialRules.end())
		return false;

	Out.iPass = iter->second.iPass;
	Out.Variants = iter->second.Variants;
	Out.bHasLayerColors = iter->second.bHasLayerColors;
	Out.LayerColors = iter->second.LayerColors;
	return true;
}

HRESULT CRenderRule_MappingJson::Parse_RenderRule(const json& Material, MATERIAL_RULE& OutRule,
	_bool& bHasRender)
{
	bHasRender = false;

	const auto iterRender = Material.find("render");
	if (iterRender == Material.end())
		return S_OK;

	if (false == iterRender->is_object())
		return E_FAIL;

	bHasRender = true;
	OutRule.iPass = iterRender->value("pass", 0u);

	OutRule.LayerColors[0] = _float4(1.f, 0.f, 0.f, 1.f);
	OutRule.LayerColors[1] = _float4(0.f, 1.f, 0.f, 1.f);
	OutRule.LayerColors[2] = _float4(0.f, 0.f, 1.f, 1.f);
	OutRule.LayerColors[3] = _float4(1.f, 1.f, 1.f, 1.f);

	if (FAILED(Parse_LayerColors(*iterRender, OutRule)))
		return E_FAIL;

	const auto iterVariants = iterRender->find("variants");
	if (iterVariants != iterRender->end())
	{
		if (false == iterVariants->is_object())
			return E_FAIL;

		for (auto iter = iterVariants->begin(); iter != iterVariants->end(); ++iter)
		{
			VARIANT_OVERRIDE Override{};

			if (FAILED(Parse_MaterialTypeKey(iter.key(), Override.eType)))
				return E_FAIL;

			if (iter.value().is_number_unsigned())
			{
				Override.iSlot = iter.value().get<_uint>();
			}
			else if (iter.value().is_string())
			{
				if (FAILED(Parse_UInt(iter.value().get<string>(), Override.iSlot)))
					return E_FAIL;
			}
			else
			{
				return E_FAIL;
			}

			OutRule.Variants.push_back(Override);
		}
	}

	return S_OK;
}

HRESULT CRenderRule_MappingJson::Parse_MaterialTypeKey(const string& Key, MATERIAL_TYPE& OutType)
{
	_uint iType = {};

	if (SUCCEEDED(Parse_UInt(Key, iType)))
	{
		if (0 == iType || iType >= ETOUI(MATERIAL_TYPE::END))
			return E_FAIL;

		OutType = static_cast<MATERIAL_TYPE>(iType);
		return S_OK;
	}

	string Lower = Key;
	for (char& ch : Lower)
		ch = static_cast<char>(tolower(static_cast<unsigned char>(ch)));

	if ("diffuse" == Lower) OutType = MATERIAL_TYPE::DIFFUSE;
	else if ("specular" == Lower) OutType = MATERIAL_TYPE::SPECULAR;
	else if ("ambient" == Lower) OutType = MATERIAL_TYPE::AMBIENT;
	else if ("emissive" == Lower) OutType = MATERIAL_TYPE::EMISSIVE;
	else if ("normals" == Lower || "normal" == Lower) OutType = MATERIAL_TYPE::NORMALS;
	else if ("opacity" == Lower) OutType = MATERIAL_TYPE::OPACITY;
	else if ("ambient_occlusion" == Lower) OutType = MATERIAL_TYPE::AMBIENT_OCCLUSION;
	else if ("layer_mask" == Lower) OutType = MATERIAL_TYPE::LAYER_MASK;
	else if ("layer_color" == Lower) OutType = MATERIAL_TYPE::LAYER_COLOR;
	else return E_FAIL;

	return S_OK;
}

HRESULT CRenderRule_MappingJson::Parse_UInt(const string& Text, _uint& OutValue)
{
	char* pEnd = nullptr;
	const unsigned long Value = strtoul(Text.c_str(), &pEnd, 10);

	if (pEnd == Text.c_str() || '\0' != *pEnd)
		return E_FAIL;

	OutValue = static_cast<_uint>(Value);
	return S_OK;
}

HRESULT CRenderRule_MappingJson::Parse_LayerColors(const json& Render, MATERIAL_RULE& OutRule)
{
	const auto iterLayerColors = Render.find("layer_colors");
	if (iterLayerColors == Render.end())
		return S_OK;

	if (false == iterLayerColors->is_object())
		return E_FAIL;

	OutRule.bHasLayerColors = true;

	for (auto iter = iterLayerColors->begin(); iter != iterLayerColors->end(); ++iter)
	{
		const _int iIndex = LayerColorIndex(iter.key());
		if (0 > iIndex)
			return E_FAIL;

		if (FAILED(Parse_LayerColorValue(iter.value(), OutRule.LayerColors[iIndex])))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CRenderRule_MappingJson::Parse_LayerColorValue(const json& Value, _float4& OutColor)
{
	if (false == Value.is_array())
		return E_FAIL;

	if (Value.size() < 3 || Value.size() > 4)
		return E_FAIL;

	for (size_t i = 0; i < Value.size(); ++i)
	{
		if (false == Value[i].is_number())
			return E_FAIL;
	}

	const _float fAlpha = (4 == Value.size()) ? Value[3].get<_float>() : 1.f;
	OutColor = _float4(
		Value[0].get<_float>(),
		Value[1].get<_float>(),
		Value[2].get<_float>(),
		fAlpha);

	return S_OK;
}

_int CRenderRule_MappingJson::LayerColorIndex(const string& strKey)
{
	string strLower = strKey;
	for (char& ch : strLower)
		ch = static_cast<char>(tolower(static_cast<unsigned char>(ch)));

	if ("r" == strLower || "red" == strLower || "0" == strLower)
		return 0;
	if ("g" == strLower || "green" == strLower || "1" == strLower)
		return 1;
	if ("b" == strLower || "blue" == strLower || "2" == strLower)
		return 2;
	if ("a" == strLower || "alpha" == strLower || "3" == strLower)
		return 3;

	return -1;
}

CRenderRule_MappingJson* CRenderRule_MappingJson::Create_FromFile(const _char* pMappingFilePath)
{
	CRenderRule_MappingJson* pInstance = new CRenderRule_MappingJson();

	if (FAILED(pInstance->Initialize(pMappingFilePath)))
	{
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CRenderRule_MappingJson::Free()
{
	m_MaterialRules.clear();

	__super::Free();
}
#pragma endregion
