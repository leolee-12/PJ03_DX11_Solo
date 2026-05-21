#pragma once
#include "Event_Defines.h"
#include "Event_Context.h"

NS_BEGIN(Game_PKM)

class CEvent_Definition;
class CEventSequence_Player;
class CLevel_GamePlay;

class CEvent_Manager final : public CBase
{
private:
	CEvent_Manager();
	virtual ~CEvent_Manager() = default;

public:
	HRESULT Initialize(CLevel_GamePlay* pOwnerLevel);
	HRESULT Load_From_File(const _tchar* pFilePath);
	void Update(_float fTimeDelta);

	HRESULT Start_Sequence(const _wstring& strSequenceID, EVENT_CONTEXT tContext);
	_bool Is_Playing() const;
	void Cancel_ActiveSequence();
	void Clear();

	const CEvent_Definition* Find_Sequence(const _wstring& strSequenceID) const;

public:
	static CEvent_Manager* Create(CLevel_GamePlay* pOwnerLevel);

private:
	CLevel_GamePlay* m_pOwnerLevel = { nullptr };   // weak
	CGameInstance* m_pGameInstance = { nullptr };   // weak

	unordered_map<_wstring, CEvent_Definition*> m_Sequences;
	CEventSequence_Player* m_pActivePlayer = { nullptr };

private:
	virtual void Free() override;
};

NS_END