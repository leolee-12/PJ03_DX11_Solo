#ifndef Editor_Struct_h__
#define Editor_Struct_h__
#include "UIImage.h"
#include "UIButton.h"
#include "UIProgressBar.h"
#include "UIText.h"
#include "UIContainer.h"
#include "UITween.h"

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

struct UISEQ_ANIMATION_NODE
{
	_wstring strName;
	vector<CUITween::UITWEEN_DESC> vTracks;
};

struct UISEQ_STEP_NODE
{
	UI_SEQ_STEP_KIND eKind{ UI_SEQ_STEP_KIND::WAIT };
	_string strTargetId;             // UTF-8 (UISEQ_WIDGET_NODE::strId 참조)
	_wstring strAnimName;
	_float fWaitSec{ 0.f };
	_bool bVisible{ true };
	_string strCallbackId;           // 예약
	_bool bJoinPrev{ false };
};

struct UISEQ_WIDGET_NODE
{
	_string strId;
	_string strDisplayName;
	std::variant<
		CUIImage::UIIMAGE_DESC,
		CUIButton::UIBUTTON_DESC,
		CUIProgressBar::UIPROGRESSBAR_DESC,
		CUIText::UITEXT_DESC,
		CUIContainer::UICONTAINER_DESC
	> tDesc;
	vector<UISEQ_ANIMATION_NODE> vAnimations;

	UI_TYPE Get_Type() const
	{
		return std::visit([](const auto& d) {
			using T = std::decay_t<decltype(d)>;
			if constexpr (std::is_same_v<T, CUIImage::UIIMAGE_DESC>)         return UI_TYPE::IMAGE;
			else if constexpr (std::is_same_v<T, CUIButton::UIBUTTON_DESC>)  return UI_TYPE::BUTTON;
			else if constexpr (std::is_same_v<T, CUIProgressBar::UIPROGRESSBAR_DESC>) return UI_TYPE::PROGRESSBAR;
			else if constexpr (std::is_same_v<T, CUIText::UITEXT_DESC>)      return UI_TYPE::TEXT;
			else                                                             return UI_TYPE::CONTAINER;
			}, tDesc);
	}
};

// 파일 단위 문서
struct UISEQ_DOC
{
	_int iVersion{ 1 };
	_string strName;
	vector<UISEQ_WIDGET_NODE> vWidgets;
	vector<UISEQ_STEP_NODE> vSteps;
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