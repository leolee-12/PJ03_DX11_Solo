#include "Event_Manager.h"
#include "Event_Definition.h"
#include "EventSequence_Player.h"
#include "EventSequence_Parser.h"
#include "Level_GamePlay.h"
#include "GameInstance.h"

CEvent_Manager::CEvent_Manager()
{
}

HRESULT CEvent_Manager::Initialize(CLevel_GamePlay* pOwnerLevel)
{
	if (nullptr == pOwnerLevel)
		return E_FAIL;

	m_pOwnerLevel = pOwnerLevel;
	m_pGameInstance = CGameInstance::GetInstance();

	if (nullptr == m_pGameInstance)
		return E_FAIL;

	return S_OK;
}

HRESULT CEvent_Manager::Load_From_File(const _tchar* pFilePath)
{
	vector<CEvent_Definition*> LoadedSequences;

	const HRESULT hr = CEventSequence_Parser::Load_From_File(pFilePath, LoadedSequences);
	if (FAILED(hr))
		return E_FAIL;

	for (CEvent_Definition* pSequence : LoadedSequences)
	{
		if (nullptr == pSequence)
			continue;

		const _wstring& strSequenceID = pSequence->Get_SequenceID();
		if (true == strSequenceID.empty())
		{
			Safe_Release(pSequence);
			continue;
		}

		auto iter = m_Sequences.find(strSequenceID);
		if (iter != m_Sequences.end())
			Safe_Release(iter->second);

		m_Sequences[strSequenceID] = pSequence;
	}

#ifdef _DEBUG
	OutputDebugStringA(("[Event] Load sequence count = " + to_string(m_Sequences.size()) +
		"\n").c_str());
#endif

	return S_OK;
}

void CEvent_Manager::Update(_float fTimeDelta)
{
	if (nullptr == m_pActivePlayer)
		return;

	const EVENT_PLAY_STATE eState = m_pActivePlayer->Update(fTimeDelta);

	if (EVENT_PLAY_STATE::FINISHED == eState ||
		EVENT_PLAY_STATE::FAILED == eState ||
		EVENT_PLAY_STATE::CANCELED == eState)
	{
#ifdef _DEBUG
		if (EVENT_PLAY_STATE::FINISHED == eState)
			OutputDebugStringA("[Event] Sequence Finish\n");
		else if (EVENT_PLAY_STATE::FAILED == eState)
			OutputDebugStringA("[Event Warn] Sequence Failed\n");
		else
			OutputDebugStringA("[Event] Sequence Canceled\n");
#endif
		Safe_Release(m_pActivePlayer);
	}
}

HRESULT CEvent_Manager::Start_Sequence(const _wstring& strSequenceID, EVENT_CONTEXT tContext)
{
	if (true == strSequenceID.empty())
		return E_FAIL;

	if (nullptr != m_pActivePlayer)
		return E_FAIL;

	const CEvent_Definition* pSequence = Find_Sequence(strSequenceID);
	if (nullptr == pSequence)
	{
#ifdef _DEBUG
		OutputDebugStringA("[Event Warn] sequence not found\n");
#endif
		return E_FAIL;
	}

	if (nullptr == tContext.pGameInstance)
		tContext.pGameInstance = m_pGameInstance;

	if (nullptr == tContext.pLevelGamePlay)
		tContext.pLevelGamePlay = m_pOwnerLevel;

	m_pActivePlayer = CEventSequence_Player::Create(pSequence, tContext);
	if (nullptr == m_pActivePlayer)
		return E_FAIL;

#ifdef _DEBUG
	OutputDebugStringA("[Event] Start Sequence\n");
#endif

	return S_OK;
}

_bool CEvent_Manager::Is_Playing() const
{
	if (nullptr == m_pActivePlayer)
		return false;

	const EVENT_PLAY_STATE eState = m_pActivePlayer->Get_State();

	return EVENT_PLAY_STATE::PLAYING == eState ||
		EVENT_PLAY_STATE::WAITING == eState;
}

void CEvent_Manager::Cancel_ActiveSequence()
{
	if (nullptr == m_pActivePlayer)
		return;

	m_pActivePlayer->Cancel();
	Safe_Release(m_pActivePlayer);
}

void CEvent_Manager::Clear()
{
	Cancel_ActiveSequence();

	for (auto& Pair : m_Sequences)
		Safe_Release(Pair.second);

	m_Sequences.clear();
}

const CEvent_Definition* CEvent_Manager::Find_Sequence(const _wstring& strSequenceID) const
{
	auto iter = m_Sequences.find(strSequenceID);
	if (iter == m_Sequences.end())
		return nullptr;

	return iter->second;
}

CEvent_Manager* CEvent_Manager::Create(CLevel_GamePlay* pOwnerLevel)
{
	CEvent_Manager* pInstance = new CEvent_Manager();

	if (FAILED(pInstance->Initialize(pOwnerLevel)))
	{
		MSG_BOX("Failed to Created : CEvent_Manager");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEvent_Manager::Free()
{
	Clear();

	m_pOwnerLevel = nullptr;
	m_pGameInstance = nullptr;

	__super::Free();
}