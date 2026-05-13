#include "Battle_ActionSequencer.h"
#include "IBattleAction_Step.h"

CBattle_ActionSequencer::CBattle_ActionSequencer()
{
}

HRESULT CBattle_ActionSequencer::Initialize()
{
	m_vSteps.reserve(16);
	return S_OK;
}

void CBattle_ActionSequencer::Push_Step(IBattleAction_Step* pStep)
{
	if (nullptr == pStep)
		return;

	Safe_AddRef(pStep);
	m_vSteps.push_back(pStep);
}

void CBattle_ActionSequencer::Submit()
{
	if (m_vSteps.empty())
		return;

	m_iCursor = 0;
	m_bStepEntered = false;
	m_bActive = true;
}

void CBattle_ActionSequencer::Clear()
{
	for (auto& p : m_vSteps)
		Safe_Release(p);

	m_vSteps.clear();
	m_iCursor = 0;
	m_bStepEntered = false;
	m_bActive = false;
	m_tActionData = {};
}

void CBattle_ActionSequencer::Tick(const BATTLE_CONTEXT& ctx, _float fTimeDelta)
{
	if (false == m_bActive || m_vSteps.empty() || m_iCursor >= m_vSteps.size())
	{
		m_bActive = false;
		return;
	}

	IBattleAction_Step* pCurrent = m_vSteps[m_iCursor];
	if (nullptr == pCurrent)
	{
		++m_iCursor;
		m_bStepEntered = false;
		return;
	}

	if (false == m_bStepEntered)
	{
		pCurrent->OnEnter(ctx);
		m_bStepEntered = true;
	}

	pCurrent->Update(ctx, fTimeDelta);

	if (pCurrent->Is_Complete(ctx))
	{
		++m_iCursor;
		m_bStepEntered = false;

		if (m_iCursor >= m_vSteps.size())
		{
			for (auto& p : m_vSteps)
				Safe_Release(p);

			m_vSteps.clear();
			m_iCursor = 0;
			m_bActive = false;
			m_tActionData = {};
		}
	}
}

CBattle_ActionSequencer* CBattle_ActionSequencer::Create()
{
	CBattle_ActionSequencer* pInstance = new CBattle_ActionSequencer();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CBattle_ActionSequencer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBattle_ActionSequencer::Free()
{
	Clear();
	__super::Free();
}