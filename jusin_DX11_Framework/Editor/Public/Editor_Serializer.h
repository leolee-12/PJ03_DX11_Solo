#pragma once
#include "GameObject.h"
#include "Editor_Defines.h"

NS_BEGIN(Editor)

class CEditor_Serializer final
{
private:
	CEditor_Serializer();
	virtual ~CEditor_Serializer() = default;

public:
	static HRESULT Save_Map(const _string& strPath, class CEditInstance* pEditInstance);
	static HRESULT Load_Map(const _string& strPath, class CEditInstance* pEditInstance);

	static HRESULT Save_UILayout(const _string& strPath, const vector<struct UI_ELEMENT>& Elements);
	static HRESULT Load_UILayout(const _string& strPath, vector<struct UI_ELEMENT>& Elements);

	static HRESULT Save_EffectPreset(const _string& strPath, const vector<struct EFFECT_PRESET>& Presets);
	static HRESULT Load_EffectPreset(const _string& strPath, vector<struct EFFECT_PRESET>& Presets);

	static json Serialize_Transform(CTransform* pTransformCom);
	static void Deserialize_Transform(CTransform* pTransformCom, const json& j);
};

NS_END