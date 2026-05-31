#pragma once
#include "Panel_Base.h"

NS_BEGIN(Editor)
class CVP_RenderTarget;
class CVP_CoordMapper;
class CVP_PickingCtrl;
class CVP_UIEditCtrl;
class CVP_OverlayDrawer;

class CPanel_Viewport final : public CPanel_Base
{
private:
	CPanel_Viewport(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CPanel_Viewport() = default;

public:
	HRESULT Initialize() override;
	void Update(_float fTimeDelta) override;
	HRESULT Render() override;
	HRESULT Begin_SceneRender();
	HRESULT End_SceneRender();

	const ImVec2& Get_DisplaySize() const;
	const ImVec2& Get_DisplayPos() const;

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

	CVP_RenderTarget* m_pRenderTarget = { nullptr };
	CVP_CoordMapper* m_pMapper = { nullptr };
	CVP_PickingCtrl* m_pPickingCtrl = { nullptr };
	CVP_UIEditCtrl* m_pUIEditCtrl = { nullptr };
	CVP_OverlayDrawer* m_pOverlayDrawer = { nullptr };

	ImVec2 m_vDocCanvasSize = {};	// 문서 design canvas 크기
	static ImVec2 s_vFallBack0;
	static ImVec2 s_vFallBack1;

private:
	void Draw_DebugHUD();

public:
	static CPanel_Viewport* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

private:
	virtual void Free() override;
};

NS_END