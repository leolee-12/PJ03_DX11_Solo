#pragma once
#include "Base.h"

/* -------------------------------------------------- */
// 레벨 매니저
// - 보관 중인 레벨의 반복적인 Update, Render를 수행
// - 현재 할당된 레벨의 주소를 보관
// - 원활한 레벨 교체작업 수행
// (레벨 교체 시 이전 레벨 삭제, 기존 레벨용 자원을 정리)
/* -------------------------------------------------- */

NS_BEGIN(Engine)

class CLevel_Manager : public CBase
{
private:
	CLevel_Manager();
	virtual ~CLevel_Manager() = default;

public:
	HRESULT	Change_Level(_int iNewLevelIndex, class CLevel* pNewLevel);
	void	Update(_float fTimeDelta);
	HRESULT	Render();


private:
	class CLevel*			m_pCurrentLevel = { nullptr };
	class CGameInstance*	m_pGameInstance = { nullptr };
	_int					m_iCurrentLevelIndex = { -1 };

public:
	static CLevel_Manager* Create();
	virtual void Free() override;
};

NS_END