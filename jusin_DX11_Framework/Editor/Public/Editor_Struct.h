#ifndef Editor_Struct_h__
#define Editor_Struct_h__

NS_BEGIN(Engine)
class CModel;
NS_END

NS_BEGIN(Editor)

struct OBJ_RECORD
{
	_uint iProtoLevel = { };
	WNameID strProtoTag = { };
	_uint iLayerLevel = { };
	WNameID strLayerTag = { };
	CGameObject* pObj = { nullptr };
};

struct EFFECT_PRESET
{
	/* 향후 채움 */
};

struct CATALOG_ITEM
{
	_uint iProtoLevel = { };
	WNameID strProtoTag = { };
	_uint iLayerLevel = { };
	WNameID strLayerTag = { };
	_string strDisplayName;
	_string strCategory;
};

struct EDITOR_OBJECT_ENTRY
{
	CGameObject* pObj = { nullptr };
	Engine::CModel* pModel = { nullptr };

	_bool bPickable = { false };
	_bool bSelectable = { false };
	_bool bPlacementSurface = { false };
};

NS_END

#endif // Editor_Struct_h__