#ifndef Editor_Struct_h__
#define Editor_Struct_h__
#include "UIObject.h"

NS_BEGIN(Editor)

struct OBJ_RECORD
{
	_uint iProtoLevel = { };
	WNameID strProtoTag = { };
	_uint iLayerLevel = { };
	WNameID strLayerTag = { };
	CGameObject* pObj = { nullptr };
};

struct UI_ELEMENT
{
	_string id{};						// 고유 식별자
	_string displayName{};				// 표시 이름
	_string UITypeName{};				// "CHP_Bar", "CPokeball_Gauge" 등
	_string spriteName{};				// 연결 스프라이트 이름
	// 앵커: 부모(화면) 기준 9방향
	// "TL","TC","TR","ML","MC","MR","BL","BC","BR"
	_string anchor = "MC";
	_float color[4] = { 1,1,1,1 };		// RGBA 틴트

	CUIObject::UIOBJECT_DESC tDesc{};	// UI 위치 및 크기 정보

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

NS_END

#endif // Editor_Struct_h__