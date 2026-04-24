#pragma once
#include "Panel_Base.h"

NS_BEGIN(Editor)

class CPanel_UITool final : public CPanel_Base
{
private:
	CPanel_UITool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CPanel_UITool() = default;

public:
	HRESULT Initialize() override;
	void Update(_float fTimeDelta) override;
	HRESULT Render() override;

private:
	UISEQ_DOC m_Doc{};
	_int m_iSelectedWidget{ -1 };
	_int m_iSelectedAnimation{ -1 };
	_int m_iSelectedTrack{ -1 };
	_int m_iSelectedStep{ -1 };
	_bool m_bDirty{ false };
	_string m_strCurrentPath{ "../../Data/UI/HUD_Layout.uiseq" };
	_string m_strStatus{};

	ID3D11Device* m_pDevice{ nullptr };
	ID3D11DeviceContext* m_pContext{ nullptr };

	CUIContainer* m_pContainerSentinel{ nullptr };
	CUIImage* m_pImageSentinel{ nullptr };
	CUIText* m_pTextSentinel{ nullptr };
	CUIButton* m_pButtonSentinel{ nullptr };
	CUIProgressBar* m_pProgressBarSentinel{ nullptr };

	_bool m_bCanvasDragging{ false };
	_int m_iCanvasDragWidget{ -1 };
	ImVec2 m_vDragStartMouse{};
	_float2 m_vDragStartValue{};

private:
	HRESULT Initialize_Sentinels();
	void Reset_Doc();
	void Normalize_Selection();
	void Mark_Dirty(const char* pszReason);
	UISEQ_WIDGET_NODE* Get_SelectedWidget();
	const UISEQ_WIDGET_NODE* Get_SelectedWidget() const;
	UISEQ_ANIMATION_NODE* Get_SelectedAnimation();
	const UISEQ_ANIMATION_NODE* Get_SelectedAnimation() const;
	CUITween::UITWEEN_DESC* Get_SelectedTrack();
	const CUITween::UITWEEN_DESC* Get_SelectedTrack() const;
	UISEQ_STEP_NODE* Get_SelectedStep();
	const UISEQ_STEP_NODE* Get_SelectedStep() const;
	CUIObject* Resolve_Sentinel(UI_TYPE eType) const;
	const UISEQ_WIDGET_NODE* Find_WidgetById(const _string& strId) const;
	_string Make_NextWidgetId() const;
	_wstring Make_UniqueAnimationName(const UISEQ_WIDGET_NODE& tWidget, const _wstring& strBase, _int iSkipIndex = -1) const;
	_string Make_NextCallbackId() const;
	UISEQ_WIDGET_NODE Make_DefaultWidget(UI_TYPE eType) const;
	void Duplicate_Widget(_int iWidgetIndex);
	void Erase_Widget(_int iWidgetIndex);
	_bool Sanitize_DocReferences();
	_bool Apply_StepTargetFallback(UISEQ_STEP_NODE& tStep) const;
	CUITween::UITWEEN_DESC Make_DefaultTrack(const UISEQ_WIDGET_NODE& tWidget) const;
	UISEQ_STEP_NODE Make_DefaultStep(UI_SEQ_STEP_KIND eKind, _bool bJoinPrev) const;

	void Draw_Toolbar();
	void Draw_Hierarchy();
	void Draw_Timeline();
	void Draw_Inspector();
	void Draw_Canvas();

public:
	static CPanel_UITool* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

private:
	virtual void Free() override;
};

NS_END
