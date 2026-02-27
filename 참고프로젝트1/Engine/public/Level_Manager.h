#pragma once

#include "Base.h"

/* 레벨을 교체 시에 이전 레벨을 파괴한다. */
/* 현재 화면에 보여줘야 할 레벨을 보관한다. */
/* 기존 레벨용 자원들을 정리한다. */
/* 레벨의 반복적인업데이트와 렌더를 호출해준다.  */

NS_BEGIN(Engine)

class CLevel_Manager final : public CBase
{
private:
	CLevel_Manager();
	virtual ~CLevel_Manager() = default;

public:
	HRESULT Change_Level(_uint iLevelID, class CLevel* pNewLevel);
	void Update(_float fTimeDelta);
	HRESULT Render();

private:
	_uint					m_iLevelID = {};
	class CLevel*			m_pCurrentLevel = { nullptr };
	class CGameInstance*	m_pGameInstance = { nullptr };

public:
	static CLevel_Manager* Create();
	virtual void Free() override; 
};

NS_END