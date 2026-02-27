#pragma once

#include "Base.h"

/* 현재 레벨의 틱, 렌더를 무한히 호출한다. */

/* Open_Level() */
/* 현재 화면에 보여줘야할 레벨의 주소를 갖는다. */
/* 이전 레벨을 삭제한다. */
/* 이전 레벨용 자원들을 정리한다. */

BEGIN(Engine)

class CLevel_Manager final : public CBase
{
	INFO_CLASS(CLevel_Manager, CBase)

private:
	CLevel_Manager();
	virtual ~CLevel_Manager() = default;

public:
	HRESULT Initialize();
	void Tick(_cref_time fTimeDelta);
	void Late_Tick(_cref_time fTimeDelta);
	HRESULT Render();

public:
	HRESULT Open_Level(_uint iCurrentLevelIndex, class CLevel* pNewLevel);

	_bool	IsVisitedLevel(_uint iCurrentLevelIndex)
	{
		if (m_bVisitedLevelList.find(iCurrentLevelIndex) == m_bVisitedLevelList.end())
			return false;
		else
			return true;
	}

	void	Set_VisitedLevel(_uint iCurrentLevelIndex)
	{
			m_bVisitedLevelList.insert(iCurrentLevelIndex);
	}
		
	_uint	Get_CreateLevelIndex() { return m_iCreateLevelIndex; }
	void	Set_CreateLevelIndex(_uint eLevelIndex) { m_iCreateLevelIndex = eLevelIndex; }

	class CLevel* Get_CurrentLevel() { return m_pCurrentLevel; }
	_uint	Get_CurrentLevelIndex() { return m_iCurrentLevelIndex; }

private:
	_uint						m_iCurrentLevelIndex = { 0 };
	_uint						m_iCreateLevelIndex = { 0 };
	class CLevel*				m_pCurrentLevel = { nullptr };
	class CGameInstance*		m_pGameInstance = { nullptr };

	set<_uint>					m_bVisitedLevelList;

public:
	static CLevel_Manager* Create();
	virtual void Free() override;
};

END