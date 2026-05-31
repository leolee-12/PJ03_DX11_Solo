#include "Camera_Sequence.h"

HRESULT CCamera_Sequence::Initialize()
{
	/* M3 시점 별도 초기화 없음. M9 JSON 로드 도입 시 본 함수에서 처리 가능. */
	return S_OK;
}

void CCamera_Sequence::Push_Shot(const CAMERA_SHOT_DESC& shot)
{
	m_Shots.push_back(shot);
}

void CCamera_Sequence::Tick(_float fTimeDelta)
{
	if (m_bFinished)
		return;

	/* 빈 시퀀스 또는 cursor 가 이미 끝을 지난 경우 즉시 finished 처리. */
	if (m_iCursor >= m_Shots.size())
	{
		m_bFinished = true;
		return;
	}

	m_fElapsedInShot += fTimeDelta;

	/* 현재 shot 의 fDuration 경과 시 다음 cursor 로. 잔여 시간은 단순화 위해 0 으로 리셋.
	   1프레임 dt 가 fDuration 보다 큰 비정상 케이스는 다음 Tick 호출에서 동일 분기로 자연 진행. */
	if (m_fElapsedInShot >= m_Shots[m_iCursor].fDuration)
	{
		m_iCursor += 1;
		m_fElapsedInShot = 0.f;

		if (m_iCursor >= m_Shots.size())
			m_bFinished = true;
	}
}

const CAMERA_SHOT_DESC* CCamera_Sequence::Get_Current_Shot() const
{
	if (m_bFinished || m_iCursor >= m_Shots.size())
		return nullptr;
	return &m_Shots[m_iCursor];
}

void CCamera_Sequence::Reset()
{
	m_iCursor = 0;
	m_fElapsedInShot = 0.f;
	m_bFinished = false;
	/* m_Shots 자체는 유지. 동일 시퀀스를 처음부터 다시 재생. */
}

_float CCamera_Sequence::Get_Total_Duration() const
{
	_float fSum = 0.f;
	for (const auto& shot : m_Shots)
		fSum += shot.fDuration;
	return fSum;
}

CCamera_Sequence* CCamera_Sequence::Create()
{
	CCamera_Sequence* pInstance = new CCamera_Sequence();

	if (FAILED(pInstance->Initialize()))
	{
		Safe_Release(pInstance);
		return nullptr;
	}
	return pInstance;
}

void CCamera_Sequence::Free()
{
	m_Shots.clear();
}