#include "VP_CoordMapper.h"
#include "UICanvasMath.h"

HRESULT CVP_CoordMapper::Initialize()
{
	return S_OK;
}

void CVP_CoordMapper::Update(const ImVec2& vDisplayPos, const ImVec2& vDisplaySize, const ImVec2& vRTSize, const ImVec2& vDocSize, UI_SCALE_POLICY ePolicy)
{
	m_vDisplayPos = vDisplayPos;
	m_vDisplaySize = vDisplaySize;
	m_vRTSize = ImVec2(max(vRTSize.x, 1.f), max(vRTSize.y, 1.f));
	m_ePolicy = ePolicy;

	const _float fSafeDocW = max(vDocSize.x, 1.f);
	const _float fSafeDocH = max(vDocSize.y, 1.f);

	m_tTransform = UICanvasMath::Build_UITransform(	fSafeDocW, fSafeDocH,
													m_vRTSize.x, m_vRTSize.y,
													ePolicy);
}

ImVec2 CVP_CoordMapper::ScreenToDoc(const ImVec2& vScreen) const
{
	// screen → RT (display ↔ RT 비율)
	const _float fRTx = (m_vDisplaySize.x > 0.f)
		? (vScreen.x - m_vDisplayPos.x) * (m_vRTSize.x / m_vDisplaySize.x)
		: 0.f;
	const _float fRTy = (m_vDisplaySize.y > 0.f)
		? (vScreen.y - m_vDisplayPos.y) * (m_vRTSize.y / m_vDisplaySize.y)
		: 0.f;

	// RT → Doc (canvas 변환)
	const _float2 vDoc = UICanvasMath::Render_To_DesignPoint(
		_float2(fRTx, fRTy), m_tTransform, m_ePolicy);

	return ImVec2(vDoc.x, vDoc.y);
}

void CVP_CoordMapper::DocRectToScreen(const _float4& rcDoc, ImVec2* pOutMin, ImVec2* pOutMax) const
{
	if (nullptr == pOutMin || nullptr == pOutMax)
		return;

	// Doc → RT rect
	const _float4 vRT = UICanvasMath::Design_To_RenderRect(rcDoc, m_tTransform, m_ePolicy);

	// RT → Screen (display ↔ RT 비율)
	const _float fSx = (m_vRTSize.x > 0.f) ? m_vDisplaySize.x / m_vRTSize.x : 1.f;
	const _float fSy = (m_vRTSize.y > 0.f) ? m_vDisplaySize.y / m_vRTSize.y : 1.f;

	pOutMin->x = m_vDisplayPos.x + vRT.x * fSx;
	pOutMin->y = m_vDisplayPos.y + vRT.y * fSy;
	pOutMax->x = pOutMin->x + vRT.z * fSx;
	pOutMax->y = pOutMin->y + vRT.w * fSy;
}

CVP_CoordMapper* CVP_CoordMapper::Create()
{
	CVP_CoordMapper* pInstance = new CVP_CoordMapper();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Create : CVP_CoordMapper");
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CVP_CoordMapper::Free()
{
	__super::Free();
}
