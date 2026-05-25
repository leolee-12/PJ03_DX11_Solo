#include "Battle_Camera_Steps.h"
#include "Camera_Director.h"
#include "Battle_Manager.h"
#include "Battle_ActionSequencer.h"

#pragma region SCamera_PlaySequence
SCamera_PlaySequence::SCamera_PlaySequence()
{
}

HRESULT SCamera_PlaySequence::Initialize(CAMERA_SEQUENCE_ID eID, _bool bWait, _bool bRequireConnect)
{
	if (CAMERA_SEQUENCE_ID::NONE == eID)
		return E_FAIL;

	m_eID = eID;
	m_fDuration = 0.f;
	m_fElapsed = 0.f;
	m_bRequested = false;
	m_bWait = bWait;
	m_bRequireConnect = bRequireConnect;

	return S_OK;
}

void SCamera_PlaySequence::OnEnter(const BATTLE_CONTEXT& ctx)
{
	m_fElapsed = 0.f;
	m_bRequested = false;

	// 기술 카메라: 빗나감/타입 무효면 연출 생략 (즉시 완료 처리)
	if (m_bRequireConnect && nullptr != ctx.pManager)
	{
		const CBattle_ActionSequencer* pSeq = ctx.pManager->Get_Sequencer();
		if (nullptr != pSeq && false == pSeq->Get_ActionData().Connects())
			return;
	}

	CCamera_Director* pDirector = CCamera_Director::GetInstance();
	m_fDuration = pDirector->Get_Sequence_Duration(m_eID);

	if (m_fDuration <= 0.f)
		return;

	m_bRequested = pDirector->Play_Sequence(m_eID);
}

void SCamera_PlaySequence::Update(const BATTLE_CONTEXT& ctx, _float fTimeDelta)
{
	(void)ctx;

	if (m_bRequested)
		m_fElapsed += fTimeDelta;
}

_bool SCamera_PlaySequence::Is_Complete(const BATTLE_CONTEXT& ctx) const
{
	(void)ctx;

	if (false == m_bRequested)
		return true;

	if (false == m_bWait)
		return true;

	return m_fElapsed >= m_fDuration;
}

SCamera_PlaySequence* SCamera_PlaySequence::Create(CAMERA_SEQUENCE_ID eID, _bool bWait, _bool bRequireConnect)
{
	SCamera_PlaySequence* pInstance = new SCamera_PlaySequence();

	if (FAILED(pInstance->Initialize(eID, bWait, bRequireConnect)))
	{
		MSG_BOX("Failed to Created : SCamera_PlaySequence");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void SCamera_PlaySequence::Free()
{
	__super::Free();
}
#pragma endregion

#pragma region SCamera_Shake
SCamera_Shake::SCamera_Shake()
{
}

HRESULT SCamera_Shake::Initialize(_float fPower, _float fFrequency, _float fDuration)
{
	if (fPower <= 0.f || fFrequency <= 0.f || fDuration <= 0.f)
		return E_FAIL;

	m_fPower = fPower;
	m_fFrequency = fFrequency;
	m_fDuration = fDuration;
	m_fElapsed = 0.f;
	m_bStarted = false;

	return S_OK;
}

void SCamera_Shake::OnEnter(const BATTLE_CONTEXT& ctx)
{
	(void)ctx;

	m_fElapsed = 0.f;
	m_bStarted = true;

	CCamera_Director::GetInstance()->Start_Shake(m_fPower, m_fFrequency, m_fDuration);
}

void SCamera_Shake::Update(const BATTLE_CONTEXT& ctx, _float fTimeDelta)
{
	(void)ctx;

	if (m_bStarted)
		m_fElapsed += fTimeDelta;
}

_bool SCamera_Shake::Is_Complete(const BATTLE_CONTEXT& ctx) const
{
	(void)ctx;

	if (false == m_bStarted)
		return true;

	return m_fElapsed >= m_fDuration;
}

SCamera_Shake* SCamera_Shake::Create(_float fPower, _float fFrequency, _float fDuration)
{
	SCamera_Shake* pInstance = new SCamera_Shake();

	if (FAILED(pInstance->Initialize(fPower, fFrequency, fDuration)))
	{
		MSG_BOX("Failed to Created : SCamera_Shake");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void SCamera_Shake::Free()
{
	__super::Free();
}
#pragma endregion

#pragma region SCamera_Return
SCamera_Return::SCamera_Return()
{
}

HRESULT SCamera_Return::Initialize(_float fBlendTime)
{
	m_fBlendTime = (fBlendTime > 0.f) ? fBlendTime : 0.f;
	m_fElapsed = 0.f;

	return S_OK;
}

void SCamera_Return::OnEnter(const BATTLE_CONTEXT& ctx)
{
	(void)ctx;

	m_fElapsed = 0.f;
	CCamera_Director::GetInstance()->Return_To_BattleDefault(m_fBlendTime);
}

void SCamera_Return::Update(const BATTLE_CONTEXT& ctx, _float fTimeDelta)
{
	(void)ctx;
	m_fElapsed += fTimeDelta;
}

_bool SCamera_Return::Is_Complete(const BATTLE_CONTEXT& ctx) const
{
	(void)ctx;
	return m_fElapsed >= m_fBlendTime;
}

SCamera_Return* SCamera_Return::Create(_float fBlendTime)
{
	SCamera_Return* pInstance = new SCamera_Return();

	if (FAILED(pInstance->Initialize(fBlendTime)))
	{
		MSG_BOX("Failed to Created : SCamera_Return");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void SCamera_Return::Free()
{
	__super::Free();
}
#pragma endregion