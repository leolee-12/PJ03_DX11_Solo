#include "UISequence.h"
#include "UIAnimator.h"

CUISequence::CUISequence(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIContainer{ pDevice, pContext }
{
}

CUISequence::CUISequence(const CUISequence& Prototype)
	: CUIContainer{ Prototype }
{
}

_bool CUISequence::Insert_Step(_int iIndex, const UISEQ_STEP& step)
{
	if (m_bPlaying) return false;
	if (iIndex < 0 || iIndex > static_cast<_int>(m_Steps.size())) return false;

	m_Steps.insert(m_Steps.begin() + iIndex, step);
	return true;
}

_bool CUISequence::Remove_Step(_int iIndex)
{
	if (m_bPlaying) return false;
	if (iIndex < 0 || iIndex >= static_cast<_int>(m_Steps.size())) return false;

	m_Steps.erase(m_Steps.begin() + iIndex);
	return true;
}

_bool CUISequence::Move_Step(_int iFrom, _int iTo)
{
	if (m_bPlaying) return false;

	const _int iSize = static_cast<_int>(m_Steps.size());
	if (iFrom < 0 || iFrom >= iSize) return false;
	if (iTo < 0 || iTo >= iSize) return false;
	if (iFrom == iTo) return true;

	UISEQ_STEP tMoved = m_Steps[iFrom];
	m_Steps.erase(m_Steps.begin() + iFrom);
	m_Steps.insert(m_Steps.begin() + iTo, tMoved);
	return true;
}

_bool CUISequence::Update_Step(_int iIndex, const UISEQ_STEP& step)
{
	if (m_bPlaying) return false;
	if (iIndex < 0 || iIndex >= static_cast<_int>(m_Steps.size())) return false;

	m_Steps[iIndex] = step;
	return true;
}

void CUISequence::Seek_ToStep(_int iIndex)
{
	if (iIndex < 0 || iIndex >= static_cast<_int>(m_Steps.size())) return;

	m_iCursor = iIndex;
	m_fTimer = 0.f;
	m_bStepStarted = false;
}

void CUISequence::Set_Timer(_float fTimer)
{
	m_fTimer = fTimer;
}

HRESULT CUISequence::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUISequence::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CUISequence::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);  // CUIContainer::Update → CUIObject::Update → Animator Tick

	if (!m_bPlaying)
		return;

	if (m_iCursor < 0 || m_iCursor >= static_cast<_int>(m_Steps.size()))
	{
		m_bPlaying = false;
		return;
	}

	while (true)
	{
		const UISEQ_STEP& s = m_Steps[m_iCursor];

		// 시작 작업
		if (!m_bStepStarted)
		{
			switch (s.eKind)
			{
			case UI_SEQ_STEP_KIND::PLAY_ANIM:
				if (s.pTarget && s.pTarget->Get_Animator())
					s.pTarget->Get_Animator()->Play_Animation(s.strAnimName);
				break;

			case UI_SEQ_STEP_KIND::SET_VISIBLE:
				if (s.pTarget) s.pTarget->Set_Visible(s.bVisible);
				break;

			case UI_SEQ_STEP_KIND::USE_CALLBACK:
				if (s.fnCallback) s.fnCallback();
				break;

			case UI_SEQ_STEP_KIND::WAIT:
				m_fTimer = 0.f;
				break;

			default: break;
			}
			m_bStepStarted = true;
		}

		// 종료 판정
		_bool bStepDone = false;
		switch (s.eKind)
		{
		case UI_SEQ_STEP_KIND::WAIT:
			m_fTimer += fTimeDelta;
			if (m_fTimer >= s.fWaitSec) bStepDone = true;
			break;

		case UI_SEQ_STEP_KIND::PLAY_ANIM:
		case UI_SEQ_STEP_KIND::SET_VISIBLE:
		case UI_SEQ_STEP_KIND::USE_CALLBACK:
			bStepDone = true;   // 즉발형
			break;

		default: break;
		}

		if (!bStepDone) return;

		const UI_SEQ_STEP_KIND ePrevKind = s.eKind;

		m_iCursor++;
		m_bStepStarted = false;

		if (m_iCursor >= static_cast<_int>(m_Steps.size()))
		{
			m_bPlaying = false;
			return;
		}

		// 체이닝: 다음이 Join이거나 이전이 즉발형이면 같은 프레임 내 진행
		const _bool bChain = m_Steps[m_iCursor].bJoinPrev
			|| ePrevKind != UI_SEQ_STEP_KIND::WAIT;
		if (!bChain) return;
		// 계속 루프
	}
}

_bool CUISequence::Append(const UISEQ_STEP& step)
{
	if (m_bPlaying) return false;

	UISEQ_STEP s = step;
	s.bJoinPrev = false;
	m_Steps.push_back(s);
	return true;
}

_bool CUISequence::Join(const UISEQ_STEP& step)
{
	if (m_bPlaying) return false;

	UISEQ_STEP s = step;
	s.bJoinPrev = true;
	m_Steps.push_back(s);
	return true;
}

void CUISequence::Play()
{
	if (m_Steps.empty()) return;
	m_iCursor = 0;
	m_fTimer = 0.f;
	m_bStepStarted = false;
	m_bPlaying = true;
}

void CUISequence::Stop()
{
	m_bPlaying = false;
	m_iCursor = -1;
	m_fTimer = 0.f;
	m_bStepStarted = false;
}

_bool CUISequence::Clear_Timeline()
{
	if (m_bPlaying) return false;
	
	m_Steps.clear();
	return true;
}

CUISequence* CUISequence::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUISequence* pInstance = new CUISequence(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CUISequence");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUISequence::Clone(void* pArg)
{
	CUISequence* pInstance = new CUISequence(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CUISequence");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUISequence::Free()
{
	__super::Free();

	m_Steps.clear();
}