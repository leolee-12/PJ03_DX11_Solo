#pragma once
#include "Panel_Base.h"
#include "UITween.h"

NS_BEGIN(Editor)

class CPanel_UIAnim final : public CPanel_Base
{
private:
	CPanel_UIAnim();
	virtual ~CPanel_UIAnim() = default;

public:
	HRESULT Initialize() override;
	void Update(_float fTimeDelta) override;
	HRESULT Render() override;

private:
	class CUIEditorSession* m_pSession = { nullptr };

private:
	void Draw_Header();
	void Draw_PreviewBar();
	void Draw_Animations();	// 선택 widget의 animation/track 편집
	void Draw_AnimationList(UISEQ_WIDGET_NODE& tWidget);
	void Draw_TrackList(UISEQ_WIDGET_NODE& tWidget, UISEQ_ANIMATION_NODE& tAnim);
	void Draw_TrackInspector(UISEQ_WIDGET_NODE& tWidget, CUITween::UITWEEN_DESC& tTrack);
	void Draw_Timeline();	// doc-level steps 편집
	void Draw_StepInspector(UISEQ_STEP_NODE& tStep);

public:
	static CPanel_UIAnim* Create();

private:
	virtual void Free() override;
};

NS_END