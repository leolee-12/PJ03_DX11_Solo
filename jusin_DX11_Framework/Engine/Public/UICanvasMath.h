#ifndef UICanvasMath_h__
#define UICanvasMath_h__

#include "Engine_Defines.h"
#include "Engine_UI.h"

NS_BEGIN(Engine)

namespace UICanvasMath
{
	// Design canvas -> actual viewport 매핑 정보를 빌드한다.
	// 결과 UICANVAS_TRANSFORM은 scaleX/scaleY/uniformScale, canvasOffsetX/Y,
	// renderWidth/renderHeight를 담는다 (Engine_UI.h 정의와 동일).
	ENGINE_DLL UICANVAS_TRANSFORM Build_UITransform(
		_float fDesignWidth,
		_float fDesignHeight,
		_float fActualWidth,
		_float fActualHeight,
		UI_SCALE_POLICY ePolicy);

	// Design 좌표(문서 기준) -> Render(actual viewport) 좌표
	ENGINE_DLL _float2 Design_To_RenderPoint(
		const _float2& vDesignPoint,
		const UICANVAS_TRANSFORM& tTransform,
		UI_SCALE_POLICY ePolicy);

	// Render(actual viewport) 좌표 -> Design 좌표
	ENGINE_DLL _float2 Render_To_DesignPoint(
		const _float2& vRenderPoint,
		const UICANVAS_TRANSFORM& tTransform,
		UI_SCALE_POLICY ePolicy);

	// Design rect(x, y, w, h) -> Render rect(x, y, w, h)
	ENGINE_DLL _float4 Design_To_RenderRect(
		const _float4& vDesignRect,
		const UICANVAS_TRANSFORM& tTransform,
		UI_SCALE_POLICY ePolicy);
}	

NS_END

#endif // UICanvasMath_h__