#include "UITween.h"
#include "UIObject.h"

HRESULT CUITween::Initialize(CUIObject* pOwner, const UITWEEN_DESC& tDesc)
{
	if (nullptr == pOwner) return E_FAIL;
	if (ETOUI(UI_TWEEN_TARGET::END) <= ETOUI(tDesc.eTarget)) return E_FAIL;
	if (tDesc.fDuration < 0.f) return E_FAIL;
	if (!pOwner->Can_Apply_Tween_Target(tDesc.eTarget)) return E_FAIL;

	m_pOwner = pOwner;
	m_tDesc = tDesc;
	return S_OK;
}

void CUITween::Tick(_float fTimeDelta)
{
	if (m_bFinished) return;

	m_fElapsed += fTimeDelta;
	if (m_fElapsed < m_tDesc.fDelay) return;

	const _float fLocal = m_fElapsed - m_tDesc.fDelay;

	_float t = (m_tDesc.fDuration <= 0.f) ? 1.f : min(1.f, fLocal / m_tDesc.fDuration);

	const _float fTForEase = m_bForward ? t : (1.f - t);
	const _float fEased = Evaluate_Ease(fTForEase);
	const _float fValue = m_tDesc.fStart + (m_tDesc.fEnd - m_tDesc.fStart) * fEased;

	Apply_To_Owner(fValue);

	if (t >= 1.f)
	{
		switch (m_tDesc.eLoop)
		{
		case UI_TWEEN_LOOP::NONE:
			m_bFinished = true;
			break;
		case UI_TWEEN_LOOP::LOOP:
			m_fElapsed = m_tDesc.fDelay;   // delay는 최초 1회만
			break;
		case UI_TWEEN_LOOP::PINGPONG:
			m_fElapsed = m_tDesc.fDelay;
			m_bForward = !m_bForward;
			break;
		default:
			m_bFinished = true;
			break;
		}
	}
}

_float CUITween::Evaluate_Ease(_float t) const
{
	const _float kPI = 3.14159265f;

	switch (m_tDesc.eEase)
	{
	case UI_EASE::LINEAR:
		return t;

	case UI_EASE::EASE_IN_SINE:
		return 1.f - cosf(t * 0.5f * kPI);
	
	case UI_EASE::EASE_OUT_SINE:
		return sinf(t * 0.5f * kPI);
	
	case UI_EASE::EASE_IN_OUT_SINE:
		return -0.5f * (cosf(kPI * t) - 1.f);
	
	case UI_EASE::EASE_IN_QUAD:
		return t * t;
	
	case UI_EASE::EASE_OUT_QUAD:
		return 1.f - (1.f - t) * (1.f - t);
	
	case UI_EASE::EASE_IN_OUT_QUAD:
		return t < 0.5f ? 2.f * t * t : 1.f - powf(-2.f * t + 2.f, 2.f) * 0.5f;
	
	case UI_EASE::EASE_IN_CUBIC:
		return t * t * t;
	
	case UI_EASE::EASE_OUT_CUBIC:
		return 1.f - powf(1.f - t, 3.f);
	
	case UI_EASE::EASE_IN_OUT_CUBIC:
		return t < 0.5f ? 4.f * t * t * t : 1.f - powf(-2.f * t + 2.f, 3.f) * 0.5f;
	
	default:
		return t;
	}
}

void CUITween::Apply_To_Owner(_float fValue)
{
	m_pOwner->Apply_Tween_Target(m_tDesc.eTarget, fValue);
}

CUITween* CUITween::Create(CUIObject* pOwner, const UITWEEN_DESC& tDesc)
{
	CUITween* pInstance = new CUITween();

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

	// m_pOwner 약한참조 -> 해제X
}
