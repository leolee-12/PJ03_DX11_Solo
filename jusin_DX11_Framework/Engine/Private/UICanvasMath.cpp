#include "UICanvasMath.h"

NS_BEGIN(Engine)

namespace UICanvasMath
{
	namespace
	{
		// policy별 design -> render에 적용할 effective scale.
		// - STRETCH       : x=scaleX, y=scaleY (비균등)
		// - MATCH_WIDTH   : x=y=scaleX (가로 기준 fit, 세로 letterbox)
		// - MATCH_HEIGHT  : x=y=scaleY (세로 기준 fit, 가로 letterbox)
		// - UNIFORM_FIT   : x=y=uniformScale (양쪽 letterbox)
		inline void Get_PolicyScale(UI_SCALE_POLICY ePolicy, const UICANVAS_TRANSFORM& t, _float& fOutScaleX, _float& fOutScaleY)
		{
			switch (ePolicy)
			{
			case UI_SCALE_POLICY::STRETCH:
				fOutScaleX = t.fScaleX;
				fOutScaleY = t.fScaleY;
				break;

			case UI_SCALE_POLICY::MATCH_WIDTH:
				fOutScaleX = t.fScaleX;
				fOutScaleY = t.fScaleX;
				break;

			case UI_SCALE_POLICY::MATCH_HEIGHT:
				fOutScaleX = t.fScaleY;
				fOutScaleY = t.fScaleY;
				break;

			case UI_SCALE_POLICY::UNIFORM_FIT:
			default:
				fOutScaleX = t.fUniformScale;
				fOutScaleY = t.fUniformScale;
				break;
			}
		}
	}

	UICANVAS_TRANSFORM Build_UITransform(_float fDesignWidth, _float fDesignHeight, _float fActualWidth, _float fActualHeight, UI_SCALE_POLICY ePolicy)
	{
		const _float fSafeDesignW = (fDesignWidth > 0.f) ? fDesignWidth : 1.f;
		const _float fSafeDesignH = (fDesignHeight > 0.f) ? fDesignHeight : 1.f;
		const _float fSafeActualW = (fActualWidth > 0.f) ? fActualWidth : fSafeDesignW;
		const _float fSafeActualH = (fActualHeight > 0.f) ? fActualHeight : fSafeDesignH;

		UICANVAS_TRANSFORM t = {};
		t.fScaleX = fSafeActualW / fSafeDesignW;
		t.fScaleY = fSafeActualH / fSafeDesignH;
		t.fUniformScale = (t.fScaleX < t.fScaleY) ? t.fScaleX : t.fScaleY;

		switch (ePolicy)
		{
		case UI_SCALE_POLICY::STRETCH:
			t.fCanvasOffsetX = 0.f;
			t.fCanvasOffsetY = 0.f;
			t.fRenderWidth = fSafeActualW;
			t.fRenderHeight = fSafeActualH;
			break;

		case UI_SCALE_POLICY::MATCH_WIDTH:
			t.fCanvasOffsetX = 0.f;
			t.fCanvasOffsetY = (fSafeActualH - fSafeDesignH * t.fScaleX) * 0.5f;
			t.fRenderWidth = fSafeActualW;
			t.fRenderHeight = fSafeDesignH * t.fScaleX;
			break;

		case UI_SCALE_POLICY::MATCH_HEIGHT:
			t.fCanvasOffsetX = (fSafeActualW - fSafeDesignW * t.fScaleY) * 0.5f;
			t.fCanvasOffsetY = 0.f;
			t.fRenderWidth = fSafeDesignW * t.fScaleY;
			t.fRenderHeight = fSafeActualH;
			break;

		case UI_SCALE_POLICY::UNIFORM_FIT:
		default:
			t.fCanvasOffsetX = (fSafeActualW - fSafeDesignW * t.fUniformScale) * 0.5f;
			t.fCanvasOffsetY = (fSafeActualH - fSafeDesignH * t.fUniformScale) * 0.5f;
			t.fRenderWidth = fSafeDesignW * t.fUniformScale;
			t.fRenderHeight = fSafeDesignH * t.fUniformScale;
			break;
		}

		return t;
	}

	_float2 Design_To_RenderPoint(const _float2& vDesignPoint, const UICANVAS_TRANSFORM& tTransform, UI_SCALE_POLICY ePolicy)
	{
		_float fSx = 1.f, fSy = 1.f;
		Get_PolicyScale(ePolicy, tTransform, fSx, fSy);

		return _float2(
			tTransform.fCanvasOffsetX + vDesignPoint.x * fSx,
			tTransform.fCanvasOffsetY + vDesignPoint.y * fSy);
	}

	_float2 Viewport_To_DesignPoint(const _float2& vViewportPoint, const UICANVAS_TRANSFORM& tTransform, UI_SCALE_POLICY ePolicy)
	{
		_float fSx = 1.f, fSy = 1.f;
		Get_PolicyScale(ePolicy, tTransform, fSx, fSy);

		const _float fInvX = (fSx != 0.f) ? 1.f / fSx : 0.f;
		const _float fInvY = (fSy != 0.f) ? 1.f / fSy : 0.f;

		return _float2(
			(vViewportPoint.x - tTransform.fCanvasOffsetX) * fInvX,
			(vViewportPoint.y - tTransform.fCanvasOffsetY) * fInvY);
	}

	_float4 Design_To_RenderRect(const _float4& vDesignRect, const UICANVAS_TRANSFORM& tTransform, UI_SCALE_POLICY ePolicy)
	{
		_float fSx = 1.f, fSy = 1.f;
		Get_PolicyScale(ePolicy, tTransform, fSx, fSy);

		return _float4(
			tTransform.fCanvasOffsetX + vDesignRect.x * fSx,
			tTransform.fCanvasOffsetY + vDesignRect.y * fSy,
			vDesignRect.z * fSx,
			vDesignRect.w * fSy);
	}
}

NS_END