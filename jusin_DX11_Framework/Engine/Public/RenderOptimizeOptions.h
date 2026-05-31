#pragma once

/* ------------------------------------------------------------ */
// RENDER_OPTIMIZE_OPTIONS : 렌더 기능 실험 토글.
// 본 프로파일러와 렌더링 최적화 계획서(M1+)가 공유하는 신규 구조체다.
// 본 정의는 "타입 선언"만 담당하며, 실제 렌더 분기(Shadow skip 등) 연결은
// 최적화 계획서의 M1~M7 에서 수행한다. 기본값은 현재 렌더 동작과 동일하게 둔다.
/* ------------------------------------------------------------ */

namespace Engine
{
	struct RENDER_OPTIMIZE_OPTIONS
	{
		// ── 최적화 계획서 §4.3 필드(M1~M3) ─────────────────────
		bool  bEnableShadow             = { true };
		bool  bEnableShadowCasterFilter = { false };
		float fShadowCasterMaxDistance  = { 0.f };
		int   iShadowMapSize            = { 0 };	// 0 = 현재 설정 유지
		bool  bEnableMapCulling         = { false };
		float fMapObjectCullMargin      = { 0.f };
		bool  bEnableBonePaletteCache   = { false };
		bool  bEnableDebugRender        = { true };
		bool  bEnableUIRender           = { true };

		// ── 본 프로파일러 실험 토글 ─────────────────────────────
		bool  bEnableMapObject          = { true };		// MapObject 렌더 On/Off(Culling 과 별개)
		bool  bEnableDebugText          = { false };
		bool  bEnableBlendPass          = { true };
	};
}
