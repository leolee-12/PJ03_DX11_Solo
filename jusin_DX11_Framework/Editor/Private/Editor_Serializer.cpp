#include "Editor_Serializer.h"
#include "EditInstance.h"
#include <fstream>

HRESULT CEditor_Serializer::Save_Map(const _string& strPath, CEditInstance* pEditInstance)
{
	json root;
	root["metadata"] = { {"mapName", "Map"}, {"version", 1} };

	vector<OBJ_RECORD> Records = pEditInstance->Get_Records();

	for (auto& record : Records)
	{
		json entry;
		entry["name"]				= WtoS(record.pObj->Get_Name());
		entry["protoLevel"]			= record.iProtoLevel;
		entry["protoTag"]			= record.strProtoTag;
		entry["layerLevel"]			= record.iLayerLevel;
		entry["layerTag"]			= record.strLayerTag;
		entry["transform"]			= Serialize_Transform(record.pObj->Get_Transform());
		root["objects"].push_back(entry);
	}

	ofstream ofs(strPath);
	if (!ofs.is_open()) return E_FAIL;
	ofs << root.dump(2);
	return S_OK;
}

HRESULT CEditor_Serializer::Load_Map(const _string& strPath, CEditInstance* pEditInstance)
{
	ifstream ifs(strPath);
	if (!ifs.is_open()) return E_FAIL;

	json root = json::parse(ifs, nullptr, false);
	if (root.is_discarded()) return E_FAIL;

	vector<CGameObject*> vEditorObjects = pEditInstance->Get_EditorObjects();

	for (auto& entry : root["objects"])
	{
		_uint protoLevel = entry["protoLevel"];
		_uint layerLevel = entry["layerLevel"];
		WNameID protoTag = entry["protoTag"];
		WNameID layerTag = entry["layerTag"];

		size_t iNumObjects = vEditorObjects.size();
		pEditInstance->Register_Object(protoLevel, protoTag, layerLevel, layerTag, nullptr);

		// 마지막 등록된 오브젝트의 이름·Transform 복원
		auto& objs = pEditInstance->Get_EditorObjects();
		if(objs.size() <= iNumObjects)	// 등록 실패
			continue;

		CGameObject* pObj = objs.back();
		pObj->Set_Name(StoW(entry["name"].get<_string>()));
		Deserialize_Transform(pObj->Get_Transform(), entry["transform"]);
	}

	return S_OK;
}

HRESULT CEditor_Serializer::Save_UILayout(const _string& strPath, const vector<struct UI_ELEMENT>& Elements)
{
	return E_NOTIMPL;
}

HRESULT CEditor_Serializer::Load_UILayout(const _string& strPath, vector<struct UI_ELEMENT>& Elements)
{
	return E_NOTIMPL;
}

HRESULT CEditor_Serializer::Save_EffectPreset(const _string& strPath, const vector<struct EFFECT_PRESET>& Presets)
{
	return E_NOTIMPL;
}

HRESULT CEditor_Serializer::Load_EffectPreset(const _string& strPath, vector<struct EFFECT_PRESET>& Presets)
{
	return E_NOTIMPL;
}

json CEditor_Serializer::Serialize_Transform(CTransform* pTransformCom)
{
	float pos[3], rot[3], scale[3];
	ImGuizmo::DecomposeMatrixToComponents(
		reinterpret_cast<const _float*>(pTransformCom->Get_WorldMatrixPtr()), pos, rot, scale);
	
	return
	{
		{"pos",   {pos[0],		pos[1],		pos[2]}},
		{"rot",   {rot[0],		rot[1],		rot[2]}},
		{"scale", {scale[0],	scale[1],	scale[2]}}
	};
}

void CEditor_Serializer::Deserialize_Transform(CTransform* pTransformCom, const json& j)
{
	float pos[3]	= { j["pos"][0],   j["pos"][1],   j["pos"][2] };
	float rot[3]	= { j["rot"][0],   j["rot"][1],   j["rot"][2] };
	float scale[3]	= { j["scale"][0], j["scale"][1], j["scale"][2] };

	float mat[16];
	ImGuizmo::RecomposeMatrixFromComponents(pos, rot, scale, mat);

	XMFLOAT4X4 m;
	memcpy(&m, mat, sizeof(float) * 16);
	pTransformCom->Set_State(STATE::RIGHT, XMLoadFloat4(reinterpret_cast<XMFLOAT4*>(&m._11)));
	pTransformCom->Set_State(STATE::UP, XMLoadFloat4(reinterpret_cast<XMFLOAT4*>(&m._21)));
	pTransformCom->Set_State(STATE::LOOK, XMLoadFloat4(reinterpret_cast<XMFLOAT4*>(&m._31)));
	pTransformCom->Set_State(STATE::POSITION, XMLoadFloat4(reinterpret_cast<XMFLOAT4*>(&m._41)));
}
