#include "UITween.h"
#include "UIImage.h"
#include "UIText.h"
#include "UIButton.h"
#include "UIProgressBar.h"

HRESULT CUITween::Initialize(CUIObject* pOwner, const UITWEEN_DESC& tDesc)
{
	if (nullptr == pOwner) return E_FAIL;
	if (ETOUI(UI_TWEEN_TARGET::END) <= ETOUI(tDesc.eTarget)) return E_FAIL;
	if (tDesc.fDuration < 0.f) return E_FAIL;

	m_pOwner = pOwner;
	m_tDesc = tDesc;

	// 타깃별 호환성 체크 + 타입 특화 캐시
	switch (tDesc.eTarget)
	{
	case UI_TWEEN_TARGET::FILL_AMOUNT:
		if (UI_TYPE::PROGRESSBAR != pOwner->Get_UIType())
			return E_FAIL;	// fail-fast

		m_pOwnerAsBar = static_cast<CUIProgressBar*>(pOwner);
		break;
		// POSITION_*, SIZE_*, COLOR_*, ANCHOR_OFFSET_* 는 CUIObject 공통 — 추가 체크 불요
	default:
		break;
	}
	return S_OK;
}

void CUITween::Tick(_float fTimeDelta, class CUIObject* pOwner)
{
	_float fValue{ fTimeDelta };

	switch (m_tDesc.eTarget)
	{
	case UI_TWEEN_TARGET::COLOR_A:
		if (auto p = dynamic_cast<CUIImage*>(pOwner))
		{
			auto c = p->Get_Color(); c.w = fValue; p->Set_Color(c);
		}
		else if (auto p = dynamic_cast<CUIText*>(pOwner))
		{
			auto c = p->Get_Color(); c.w = fValue; p->Set_Color(c);
		}
		break;

	case UI_TWEEN_TARGET::FILL_AMOUNT:
		if (auto p = dynamic_cast<CUIProgressBar*>(pOwner)) p->Set_FillAmount(fValue);
		break;

	case UI_TWEEN_TARGET::POSITION_X:
		if (UI_ANCHOR::END != pOwner->Get_Anchor())
		{
			auto c = pOwner->Get_Center();
			pOwner->Set_Center(fValue, c.y);
		}
		break;
	}
}

_bool CUITween::Is_Finished() const
{
	return _bool();
}

void CUITween::Stop()
{

}

_float CUITween::Evaluate_Ease(_float t) const
{
	return _float();
}

void CUITween::Apply_To_Owner(_float fValue)
{
	switch (m_tDesc.eTarget)
	{
	case UI_TWEEN_TARGET::POSITION_X:       m_pOwner->Set_CenterX(v);       break;
	case UI_TWEEN_TARGET::POSITION_Y:       m_pOwner->Set_CenterY(v);       break;
	case UI_TWEEN_TARGET::SIZE_X:           m_pOwner->Set_Width(v);         break;
	case UI_TWEEN_TARGET::SIZE_Y:           m_pOwner->Set_Height(v);        break;
	case UI_TWEEN_TARGET::ANCHOR_OFFSET_X:  m_pOwner->Set_AnchorOffsetX(v); break;
	case UI_TWEEN_TARGET::ANCHOR_OFFSET_Y:  m_pOwner->Set_AnchorOffsetY(v); break;

	case UI_TWEEN_TARGET::COLOR_R:
	case UI_TWEEN_TARGET::COLOR_G:
	case UI_TWEEN_TARGET::COLOR_B:
	case UI_TWEEN_TARGET::COLOR_A:
		m_pOwner->Set_Color_Channel(m_tDesc.eTarget, v);	// virtual, 위젯별 오버라이드
		break;

	case UI_TWEEN_TARGET::FILL_AMOUNT:
		m_pOwnerAsBar->Set_FillAmount(v);	// 이미 Initialize에서 보장, 캐스트·널체크 없음
		break;

	default:
		break;
	}
}

CUITween* CUITween::Create(CUIObject* pOwner, const UITWEEN_DESC& tDesc)
{
	CUITween* pInstance = new CUITween;

	if (FAILED(pInstance->Initialize(pOwner, tDesc)))
	{
		MSG_BOX("Failed to Create : CUITween");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUITween::Free()
{
	__super::Free();
}
